//
// Created by bqrry on 09.05.2024.
//

#ifndef SI_PROIECT_SERVER_H
#define SI_PROIECT_SERVER_H

#include "../../commons/data_types.h"
#include "../communication_commons.h"

///@param onRead callback function
int listen_for_connections();
int listen_for_requests(socket_wb socket, byte_array sym);

void onRead(int socket_fd, byte_array data);
#endif //SI_PROIECT_SERVER_H
