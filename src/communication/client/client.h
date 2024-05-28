//
// Created by bqrry on 09.05.2024.
//

#include "../communication_commons.h"

#ifndef SI_PROIECT_CLIENT_H
#define SI_PROIECT_CLIENT_H
///@return socket file descriptor of the connection or -1 on error
int connect_to(char* address, int port);

///@param[in] socket Socket with buffers
///@param[in] sym Symmetric key
///@param[out] ls_buffer List of folder files
int list_folder_request(socket_wb socket, byte_array sym, byte_array *ls_buffer);

///@param[in] socket Socket with buffers
///@param[in] sym Symmetric key
///@param[in] file_name name of the file
///@returns number of blocks of DATAGRAM_SIZE, or <0 on error
int request_file(socket_wb socket, byte_array sym, char* file_name);

///@param[in] socket Socket with buffers
///@param[in] sym Symmetric key
///@param[in] file_name name of the file
///@param[in] blockID the block
///@param[out] block_buffer Buffer to write the block
int request_block(socket_wb socket, byte_array sym, char* file_name, int blockID, byte_array *block_buffer);
#endif //SI_PROIECT_CLIENT_H