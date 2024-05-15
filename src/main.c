//
// Created by bqrry on 06.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "algorithms/RSA/RSA.h"
#include "communication/server/server.h"
#include "communication/client/client.h"
#include "communication/communication_commons.h"
#include "communication/handshake/handshake.h"

int main() {

    RSA_init();

    rsa_keys key;
    mpz_init2(key.e, 2048);
    mpz_init2(key.n, 2048);
    mpz_init2(key.d, 2048);

    //key of 2048 bits
    RSA_key_generation(&key, 2048);

    rsa_pub_key pub_key;
    pub_key.n = key.n;
    pub_key.e = key.e;

    rsa_prv_key prv_key;
    prv_key.d = key.d;
    prv_key.n = key.n;

    mpz_out_str(stdout, 16, pub_key.n);
    printf("\n");
    mpz_out_str(stdout, 16, pub_key.e);
    printf("\n");

    pid_t pid = fork();
    if (pid == 0) {//child will listen for connection requests
        int listener_socket = listen_for_connections();

        byte_array send_buffer, recv_buffer;
        send_buffer.data = calloc(DATAGRAM_SIZE, 1);
        recv_buffer.data = calloc(DATAGRAM_SIZE, 1);

        socket_wb socketWb = {
                .socket_fd = listener_socket,
                .recv_buffer = recv_buffer,
                .send_buffer = send_buffer
        };

        byte_array sym_key = {
                .data = malloc(4096)
        };

        if(accept_handshake(socketWb, key, &sym_key))
        {
            perror("Failed to init handshake");
        }

        listen_for_requests(socketWb, sym_key);

        free(recv_buffer.data);
        free(send_buffer.data);
        free(sym_key.data);
        close_socket(listener_socket);


    } else {//parent will connect

        //@TODO Here will be the CLI


        //@FIXME cuz of the "loopback"
        sleep(5);

        byte_array sym_key = {
                .data = malloc(4096)
        };

        int connection_socket = connect_to("127.0.0.1", PORT);

        byte_array send_buffer, recv_buffer;
        send_buffer.data = calloc(DATAGRAM_SIZE, 1);
        recv_buffer.data = calloc(DATAGRAM_SIZE, 1);

        socket_wb socketWb = {
                .socket_fd = connection_socket,
                .recv_buffer = recv_buffer,
                .send_buffer = send_buffer
        };


        if(init_handshake(socketWb, key, &sym_key))
        {
            perror("Failed to init handshake");
        }

        //request
        byte_array ls_buffer = {
                .data = calloc(DATAGRAM_SIZE, 1),
                .length = 4096
        };

        list_folder_request(socketWb, sym_key, &ls_buffer);

        printf("\n%s", ls_buffer.data);

        free(recv_buffer.data);
        free(send_buffer.data);
        free(sym_key.data);
        close_socket(connection_socket);

    }

    //free key
    mpz_clears(key.e, key.n, key.d, NULL);
    //clear RSA state
    RSA_clear();
    return 0;
}