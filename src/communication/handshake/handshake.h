//
// Created by bqrry on 09.05.2024.
//

#ifndef SI_PROIECT_HANDSHAKE_H
#define SI_PROIECT_HANDSHAKE_H

#include "../../algorithms/RSA/RSA.h"
#include "../communication_commons.h"

///@brief Initialize handshake
///@param[in] socket
///@param[in] key The RSA key to perform exchange
///@param[out] sym_key the bytearray_data should be freed
int init_handshake(socket_wb socket, rsa_keys key, byte_array* sym_key);
///@brief Accept handshake
///@param[in] socket
///@param[in] key The RSA key to perform exchange
///@param[out] sym_key the bytearray_data should be freed
int accept_handshake(socket_wb socket, rsa_keys key, byte_array* sym_key);
#endif //SI_PROIECT_HANDSHAKE_H
