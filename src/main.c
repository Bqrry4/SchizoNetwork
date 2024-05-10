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
        int listener_socket = listen_for_connections(onRead);


        byte_array sym_key;

        if(accept_handshake(listener_socket, key, &sym_key))
        {
            perror("Failed to init handshake");
        }

        close_socket(listener_socket);


    } else {//parent will connect

        //@TODO Here will be the CLI



        //@FIXME cuz of the "loopback"
        sleep(5);
        byte_array sym_key;


        int connection_socket = connect_to("127.0.0.1", PORT);
        if(init_handshake(connection_socket, key, &sym_key))
        {
            perror("Failed to init handshake");
        }


        close_socket(connection_socket);

    }

    //free key
    mpz_clears(key.e, key.n, key.d, NULL);
    //clear RSA state
    RSA_clear();
    return 0;
}