//
// Created by bqrry on 09.05.2024.
//

#ifndef SI_PROIECT_SERVER_H
#define SI_PROIECT_SERVER_H

#include "../../commons/data_types.h"
#include "../communication_commons.h"
#include "../../algorithms/RSA/RSA.h"

///@param key rsa keys to perform handshake
int listen_for_connections(rsa_keys key) ;
int listen_for_requests(socket_wb socket, byte_array sym);

void onRead(int socket_fd, byte_array data);
#endif //SI_PROIECT_SERVER_H
