//
// Created by bqrry on 09.05.2024.
//

#include "server.h"
#include "../communication_commons.h"
#include "RC6.h"
#include "../handshake/handshake.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/limits.h>
#include <signal.h>

bool is_running = true;

void sigchld_handler(int signo, siginfo_t *sinfo, void *context) {
    if (signo == SIGINT) {
        printf("child killed");
        is_running = false;
    }
}

int listen_for_connections(rsa_keys key) {
    int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket < 0) {
        perror("Failed to create listen_socket");
        return -1;
    }
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(listen_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Failed to bind listen_socket");
        return -1;
    }

    if (listen(listen_socket, 3) < 0) {
        perror("Failed to set listen to listen_socket");
        return -1;
    }

    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);

    act.sa_sigaction = sigchld_handler;

    act.sa_flags = SA_SIGINFO;

    if (-1 == sigaction(SIGINT, &act, NULL)) {
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }
    while (is_running) {
        int sock_recv = accept(listen_socket, (struct sockaddr *) &address, &addrlen);
        if (sock_recv < 0) {
            perror("Failed to accept connection");
            break;
        }

        pid_t pid = fork();
        if (pid == 0) {
            byte_array send_buffer, recv_buffer;
            send_buffer.data = calloc(DATAGRAM_SIZE, 1);
            recv_buffer.data = calloc(DATAGRAM_SIZE, 1);

            int listener_socket = sock_recv;
            socket_wb socketWb = {
                    .socket_fd = listener_socket,
                    .recv_buffer = recv_buffer,
                    .send_buffer = send_buffer
            };

            byte_array sym_key = {
                    .data = malloc(DATAGRAM_SIZE)
            };

            if (accept_handshake(socketWb, key, &sym_key)) {
                perror("Failed to accept handshake");
            } else {
                listen_for_requests(socketWb, sym_key);

                close_socket(listener_socket);
            }

            free(recv_buffer.data);
            free(send_buffer.data);
            free(sym_key.data);
        }
    }


    close(listen_socket);
    return 0;
}

///@return -1 on error, 1 when buffer overrun
int list_work_dir(byte_array *buffer) {
    int err = 0;

    FILE *fp;
    char path[PATH_MAX];

    fp = popen("ls -p | grep -v /", "r");
    if (fp == NULL) {
        err = -1;
        goto finally;
    }

    size_t ls_len = 0;
    while (fgets(path, PATH_MAX, fp) != NULL) {
        size_t slen = strlen(path);
        if (ls_len + slen >= buffer->length) {
            printf("Not enough buffer");
            err = 1;
            break;
        }
        memcpy(buffer->data + ls_len, path, slen);

        ls_len += slen;
    }

    buffer->length = ls_len;

    finally:
    pclose(fp);
    return err;
}

int listen_for_requests(socket_wb socket, byte_array sym) {

    int err = 0;
    ssize_t bytes;
    byte_array rc6_output = {
            .data = malloc(DATAGRAM_SIZE),
            .length = DATAGRAM_SIZE
    };

    while (true) {

        memset(socket.recv_buffer.data, 0, DATAGRAM_SIZE);

        if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE, 0)) < 1) {
            printf("Failed to receive data");
            err = -1;
            goto finally;
        }
        socket.recv_buffer.length = bytes;

        memset(rc6_output.data, 0, DATAGRAM_SIZE);

        RC6_decrypt(socket.recv_buffer, sym, rc6_output);

        memset(socket.send_buffer.data, 0, DATAGRAM_SIZE);

        switch (rc6_output.data[0]) {
            case LIST_FOLDER:
                socket.send_buffer.data[0] = 100;

                byte_array ls_dir = {
                        .data = socket.send_buffer.data + 3,
                        DATAGRAM_SIZE - 1
                };

                if (list_work_dir(&ls_dir)) {
                    goto finally;
                }

                socket.send_buffer.data[1] = ls_dir.length >> 8;
                socket.send_buffer.data[2] = ls_dir.length;

                socket.send_buffer.length = ls_dir.length + 3;

                memset(rc6_output.data, 0, DATAGRAM_SIZE);

                RC6_encrypt(socket.send_buffer, sym, rc6_output);
                send(socket.socket_fd, rc6_output.data, get_encrypted_block_length(socket.send_buffer.length), 0);
                break;
            case FILE_REQUEST:
                //parse the filename
                int file_name_len = (rc6_output.data[1] << 8) + rc6_output.data[2];
                char *filename = malloc(file_name_len);
                memcpy(filename, rc6_output.data + 3, file_name_len);

                FILE *fp = fopen(filename, "r");
                fseek(fp, 0, SEEK_END); // seek to end of file
                ulong size = ftell(fp);
                fclose(fp);

                //printf("\nOCTETS %d", size);

                ulong blocks_num = size / (DATAGRAM_SIZE/2); // 1 byte for header
                blocks_num += ((size % (DATAGRAM_SIZE/2)) != 0) ? 1 : 0;
                //return the number of blocks

                socket.send_buffer.data[0] = 101;
                socket.send_buffer.data[1] = blocks_num >> 8;
                socket.send_buffer.data[2] = blocks_num;

                socket.send_buffer.length = 3;

                memset(rc6_output.data, 0, DATAGRAM_SIZE);

                RC6_encrypt(socket.send_buffer, sym, rc6_output);
                send(socket.socket_fd, rc6_output.data, get_encrypted_block_length(socket.send_buffer.length), 0);

                free(filename);
                break;
            case BLOCK_REQUEST:
                //parse the filename
                file_name_len = (rc6_output.data[1] << 8) + rc6_output.data[2];
                filename = malloc(file_name_len);
                memcpy(filename, rc6_output.data + 3, file_name_len);
                //parse the block id
                int blockID = (rc6_output.data[file_name_len + 3] << 24) + (rc6_output.data[file_name_len + 4] << 16) +
                              (rc6_output.data[file_name_len + 5] << 8) + rc6_output.data[file_name_len + 6];

                memset(socket.send_buffer.data, 0, DATAGRAM_SIZE);

                socket.send_buffer.data[0] = BLOCK_REQUEST;

                fp = fopen(filename, "r");
                fseek(fp, blockID * (DATAGRAM_SIZE/2), SEEK_SET);
                size = fread(socket.send_buffer.data + 3, sizeof(byte), DATAGRAM_SIZE/2, fp);
                fclose(fp);

                //printf("\nBlock id: %d, ns %d", blockID, size);

                socket.send_buffer.data[1] = size >> 8;
                socket.send_buffer.data[2] = size;

                socket.send_buffer.length = size + 3;

                memset(rc6_output.data, 0, DATAGRAM_SIZE);

                RC6_encrypt(socket.send_buffer, sym, rc6_output);

                send(socket.socket_fd, rc6_output.data, get_encrypted_block_length(socket.send_buffer.length), 0);

                free(filename);
                break;

        }
    }

    finally:
    free(rc6_output.data);
    return err;
}