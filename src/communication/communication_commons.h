//
// Created by bqrry on 09.05.2024.
//

#ifndef SI_PROIECT_COMMUNICATION_COMMONS_H
#define SI_PROIECT_COMMUNICATION_COMMONS_H

#define PORT 35544
///@brief The maximum size of a tcp packet
#define DATAGRAM_SIZE 4096

void close_socket(int socket_fd);

#endif //SI_PROIECT_COMMUNICATION_COMMONS_H
