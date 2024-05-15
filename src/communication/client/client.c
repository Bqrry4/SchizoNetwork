//
// Created by bqrry on 09.05.2024.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include "client.h"
#include "../communication_commons.h"
#include "RC6.h"

int connect_to(char *address, int port) {
    int connection_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connection_socket < 0) {
        perror("Failed to create listen_socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address | Address not supported ");
        return -1;
    }

    if (connect(connection_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        perror("Connection Failed");
        return -1;
    }

    return connection_socket;
}

int list_folder_request(socket_wb socket, byte_array sym, byte_array *ls_buffer)
{
    int err = 0;

    //Send request list file operation
    socket.send_buffer.data[0] = 100;
    socket.send_buffer.length = 1;

    byte_array rc6_output = {
            .data = malloc(4096),
            .length = 4096
    };

    RC6_encrypt(socket.send_buffer, sym, rc6_output);

    send(socket.socket_fd, rc6_output.data,get_encrypted_block_length(socket.send_buffer.length), 0);

    ssize_t bytes;
    if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE - 1, 0)) < 1) {
        printf("Failed to receive data");
        err = -1;
        goto finally;
    }

    socket.recv_buffer.length = bytes;
    RC6_decrypt(socket.recv_buffer, sym, rc6_output);

    ls_buffer->length = (rc6_output.data[1] << 8) + rc6_output.data[2];

    memcpy(ls_buffer->data, rc6_output.data + 3, ls_buffer->length);

    finally:
    free(rc6_output.data);
    return err;
}