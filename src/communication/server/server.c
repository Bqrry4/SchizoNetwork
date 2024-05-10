//
// Created by bqrry on 09.05.2024.
//

#include "server.h"
#include "../communication_commons.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int listen_for_connections(void (*onRead)(int, byte_array)) {
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

    //@TODO launch sock_recv in a new process
    int sock_recv = accept(listen_socket, (struct sockaddr *) &address, &addrlen);
    if (sock_recv < 0) {
        perror("Failed to accept connection");
        return -1;
    }

    close(listen_socket);
    return sock_recv;
    ssize_t bytes;
    byte_array network_buffer;
    network_buffer.data = malloc(DATAGRAM_SIZE);
    //read buffer
    while (true) {
        bytes = recv(sock_recv, network_buffer.data, DATAGRAM_SIZE - 1, 0);
        if (bytes < 0) {
            perror("Failed to read from network_buffer");
            continue;
        }
        if (bytes == 0) {
            printf("Socket closed\n");
            break;
        }

        network_buffer.length = bytes;
        onRead(sock_recv, network_buffer);
    }

    free(network_buffer.data);
    close(sock_recv);
    close(listen_socket);

    return 0;
}


void onRead(int socket_fd, byte_array data) {
    printf("%s\n", data.data);

}