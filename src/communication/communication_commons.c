//
// Created by bqrry on 09.05.2024.
//
#include <unistd.h>
#include "communication_commons.h"

void close_socket(int socket_fd) {
    close(socket_fd);
}
