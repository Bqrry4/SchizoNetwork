//
// Created by bqrry on 09.05.2024.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <linux/limits.h>
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

    //Send request list folder operation
    socket.send_buffer.data[0] = LIST_FOLDER;
    socket.send_buffer.length = 1;

    byte_array rc6_output = {
            .data = malloc(DATAGRAM_SIZE),
            .length = DATAGRAM_SIZE
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

    if(rc6_output.data[0] != LIST_FOLDER)
    {
        printf("Expected List folder response");
        err = -1;
        goto finally;
    }

    ls_buffer->length = (rc6_output.data[1] << 8) + rc6_output.data[2];

    memcpy(ls_buffer->data, rc6_output.data + 3, ls_buffer->length);

    finally:
    free(rc6_output.data);
    return err;
}


int request_file(socket_wb socket, byte_array sym, char* file_name)
{
    int err;

    //Send request file
    socket.send_buffer.data[0] = FILE_REQUEST;
    ulong file_name_len = strlen(file_name) + 1;  //include the null terminator
    socket.send_buffer.data[1] = file_name_len >> 8;
    socket.send_buffer.data[2] = file_name_len;
    memcpy(socket.send_buffer.data + 3, file_name, file_name_len);

    socket.send_buffer.length = file_name_len + 3;

    byte_array rc6_output = {
            .data = malloc(DATAGRAM_SIZE),
            .length = DATAGRAM_SIZE
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

    if(rc6_output.data[0] != FILE_REQUEST)
    {
        printf("Expected File request response");
        err = -1;
        goto finally;
    }

    int blocks_num = (rc6_output.data[1] << 8) + rc6_output.data[2];
    return blocks_num;

    finally:
    free(rc6_output.data);
    return err;

}

int request_block(socket_wb socket, byte_array sym, char* file_name, int blockID, byte_array *block_buffer)
{
    int err = 0;

    //Send block request
    socket.send_buffer.data[0] = BLOCK_REQUEST;
    ulong file_name_len = strlen(file_name) + 1;  //include the null terminator
    socket.send_buffer.data[1] = file_name_len >> 8;
    socket.send_buffer.data[2] = file_name_len;
    memcpy(socket.send_buffer.data + 3, file_name, file_name_len);

    socket.send_buffer.data[file_name_len + 3] = blockID >> 24;
    socket.send_buffer.data[file_name_len + 4] = blockID >> 16;
    socket.send_buffer.data[file_name_len + 5] = blockID >> 8;
    socket.send_buffer.data[file_name_len + 6] = blockID;

    socket.send_buffer.length = file_name_len + 7;

    byte_array rc6_output = {
            .data = malloc(DATAGRAM_SIZE),
            .length = DATAGRAM_SIZE
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

    if(rc6_output.data[0] != BLOCK_REQUEST)
    {
        printf("Expected Block request response");
        err = -1;
        goto finally;
    }

    block_buffer->length = bytes - 1;

    memcpy(block_buffer->data, rc6_output.data + 1, block_buffer->length);

    finally:
    free(rc6_output.data);
    return err;
}