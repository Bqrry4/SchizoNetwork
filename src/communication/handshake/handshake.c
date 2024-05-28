//
// Created by bqrry on 09.05.2024.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "handshake.h"
#include "../communication_commons.h"

//@NOTE: this is a hardcoded light version of a handshake, think about making the true tls handshake in the future

static void gen_random_number(mpz_t rand, int n) {
    static gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    gmp_randseed_ui(rstate, time(NULL));

    mpz_urandomb(rand, rstate, n);

    gmp_randclear(rstate);
}

int init_handshake(socket_wb socket, rsa_keys key, byte_array *sym_key) {

    ssize_t bytes;
    byte_array number_as_bytes = {
            .data = malloc(4096)
    };

    rsa_prv_key prv_key;
    prv_key.d = key.d;
    prv_key.n = key.n;

    //Perform key exchange in TLS Handshake manner
    //Send ClientHello
    socket.send_buffer.data[0] = 1;
    send(socket.socket_fd, socket.send_buffer.data, 4, 0);

    //Wait for ServerHello
    if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE - 1, 0)) < 1) {
        printf("Failed to receive data");
        return -1;
    }
    if(socket.recv_buffer.data[0] != 2)
    {
        printf("Expected ServerHello");
        return -1;
    }

    //Parse the server pub_key
    mpz_t serv_pub_e, serv_pub_n;
    mpz_init(serv_pub_e);
    mpz_init(serv_pub_n);

    size_t offset = 4;
    size_t len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];
    mpz_import (serv_pub_e, len, 1, sizeof(socket.recv_buffer.data[0]), 0, 0, socket.recv_buffer.data + offset + 2);
    offset += len + 2;
    len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];
    mpz_import (serv_pub_n, len, 1, sizeof(socket.recv_buffer.data[0]), 0, 0, socket.recv_buffer.data + offset + 2);

    rsa_pub_key serv_pub_key = {
            .n = serv_pub_n,
            .e = serv_pub_e
    };


//    mpz_out_str(stdout,16, serv_pub_n);
//    printf("\n");

//    mpz_out_str(stdout,16, serv_pub_e);
//    printf("\n");

    //Send public key + encoded number Client Finish
    socket.send_buffer.data[0] = 20;

    offset = 4;

    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, key.e);
    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;

    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, key.n);
    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;

    //gen 1023 bit number
    mpz_t rand;
    mpz_init(rand);
    gen_random_number(rand, 1023);
    //encode number with server pub_key
    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, rand);
    RSA_encrypt(number_as_bytes, serv_pub_key, &number_as_bytes);

    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;
    len = offset - 4;
    socket.send_buffer.data[1] = len >> 16;
    socket.send_buffer.data[2] = len >> 8;
    socket.send_buffer.data[3] = len;

    send(socket.socket_fd, socket.send_buffer.data, offset, 0);

    //Wait for Server Finish with the number
    if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE - 1, 0)) < 1) {
        printf("Failed to receive data");
        return -1;
    }
    if(socket.recv_buffer.data[0] != 20)
    {
        printf("Expected Server Finish");
        return -1;
    }

    offset = 4;
    len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];

    memcpy(number_as_bytes.data, socket.recv_buffer.data + offset + 2, len);
    number_as_bytes.length = len;

    RSA_decrypt(number_as_bytes, prv_key, &number_as_bytes);

    mpz_t serv_rand;
    mpz_init(serv_rand);

    mpz_import (serv_rand, number_as_bytes.length, 1, sizeof(number_as_bytes.data[0]), 0, 0, number_as_bytes.data);

//    printf("\nGenerated Client:");
//    mpz_out_str(stdout, 16, rand);
//    printf("\nReceived Client:");
//    mpz_out_str(stdout, 16, serv_rand);

    mpz_add(rand, rand, serv_rand);

//    printf("\nKey:");
//    mpz_out_str(stdout, 10, rand);

    mpz_export(sym_key->data, &(sym_key->length), 1, sizeof(sym_key->data[0]), 0, 0, rand);

    free(number_as_bytes.data);
    mpz_clears(serv_pub_e, serv_pub_n, rand, serv_rand, NULL);
    return 0;
}

int accept_handshake(socket_wb socket, rsa_keys key, byte_array *sym_key) {

    rsa_prv_key prv_key;
    prv_key.d = key.d;
    prv_key.n = key.n;

    ssize_t bytes;
    byte_array number_as_bytes = {
            .data = malloc(4096)
    };

    //Check for ClientHello
    if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE - 1, 0)) < 1) {
        printf("Failed to receive data");
        return -1;
    }
    if(!(socket.recv_buffer.data[0] == 1 || socket.recv_buffer.data[1] == 0 || socket.recv_buffer.data[2] == 0 || socket.recv_buffer.data[3] == 0))
    {
        printf("Expected ClientHello");
        return -1;
    }

    //Send ServerHello with pub_key
    socket.send_buffer.data[0] = 2;

    size_t offset = 4;
    size_t len;

    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, key.e);
    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;

    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, key.n);
    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;
    len = offset - 4;

    socket.send_buffer.data[1] = len >> 16;
    socket.send_buffer.data[2] = len >> 8;
    socket.send_buffer.data[3] = len;

    send(socket.socket_fd, socket.send_buffer.data, offset, 0);

    //Wait for Client Finish
    if ((bytes = recv(socket.socket_fd, socket.recv_buffer.data, DATAGRAM_SIZE - 1, 0)) < 1) {
        printf("Failed to receive data");
        return -1;
    }
    if(socket.recv_buffer.data[0] != 20)
    {
        printf("Expected Client Finish");
        return -1;
    }
    //Parse the client pub_key and number
    mpz_t clnt_pub_e, clnt_pub_n;
    mpz_init(clnt_pub_e);
    mpz_init(clnt_pub_n);

    offset = 4;
    len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];
    mpz_import (clnt_pub_e, len, 1, sizeof(socket.recv_buffer.data[0]), 0, 0, socket.recv_buffer.data + offset + 2);
    offset += len + 2;
    len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];
    mpz_import (clnt_pub_n, len, 1, sizeof(socket.recv_buffer.data[0]), 0, 0, socket.recv_buffer.data + offset + 2);

    rsa_pub_key clnt_pub_key = {
            .n = clnt_pub_n,
            .e = clnt_pub_e
    };

//    mpz_out_str(stdout,16, clnt_pub_n);
//    printf("\n");
//
//    mpz_out_str(stdout,16, clnt_pub_e);
//    printf("\n");

    offset += len + 2;
    len = (socket.recv_buffer.data[offset] << 8) + socket.recv_buffer.data[offset + 1];

    memcpy(number_as_bytes.data, socket.recv_buffer.data + offset + 2, len);
    number_as_bytes.length = len;

    RSA_decrypt(number_as_bytes, prv_key, &number_as_bytes);

    //gen number 1023 bits
    mpz_t rand, clnt_rand;
    mpz_init(rand);
    mpz_init(clnt_rand);
    gen_random_number(rand, 1023);

    mpz_import (clnt_rand, number_as_bytes.length, 1, sizeof(number_as_bytes.data[0]), 0, 0, number_as_bytes.data);

    //Send Server Finish
    socket.send_buffer.data[0] = 20;

    offset = 4;

    //encode number with server pub_key
    mpz_export(number_as_bytes.data, &(number_as_bytes.length), 1, sizeof(number_as_bytes.data[0]), 0, 0, rand);
    RSA_encrypt(number_as_bytes, clnt_pub_key, &number_as_bytes);

    socket.send_buffer.data[offset] = number_as_bytes.length >> 8;
    socket.send_buffer.data[offset + 1] = number_as_bytes.length;
    memcpy(socket.send_buffer.data + offset + 2, number_as_bytes.data, number_as_bytes.length);

    offset += number_as_bytes.length + 2;
    len = offset - 4;
    socket.send_buffer.data[1] = len >> 16;
    socket.send_buffer.data[2] = len >> 8;
    socket.send_buffer.data[3] = len;

    send(socket.socket_fd, socket.send_buffer.data, offset, 0);

    mpz_add(rand, rand, clnt_rand);

//    printf("\nKey:");
//    mpz_out_str(stdout, 10, rand);


    mpz_export(sym_key->data, &(sym_key->length), 1, sizeof(sym_key->data[0]), 0, 0, rand);

    mpz_clears(clnt_pub_e, clnt_pub_n, rand, clnt_rand, NULL);
    free(number_as_bytes.data);
    return 0;
}