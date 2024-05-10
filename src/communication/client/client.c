//
// Created by bqrry on 09.05.2024.
//
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

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

