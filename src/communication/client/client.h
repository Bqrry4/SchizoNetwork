//
// Created by bqrry on 09.05.2024.
//

#include "../communication_commons.h"

#ifndef SI_PROIECT_CLIENT_H
#define SI_PROIECT_CLIENT_H
///@return socket file descriptor of the connection or -1 on error
int connect_to(char* address, int port);
int list_folder_request(socket_wb socket, byte_array sym, byte_array *ls_buffer);
#endif //SI_PROIECT_CLIENT_H