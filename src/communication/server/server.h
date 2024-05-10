//
// Created by bqrry on 09.05.2024.
//

#ifndef SI_PROIECT_SERVER_H
#define SI_PROIECT_SERVER_H

#include "../../commons/data_types.h"

///@param onRead callback function
int listen_for_connections(void (*onRead)(int, byte_array));

void onRead(int socket_fd, byte_array data);
#endif //SI_PROIECT_SERVER_H
