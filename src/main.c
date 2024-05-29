//
// Created by bqrry on 06.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "algorithms/RSA/RSA.h"
#include "communication/server/server.h"
#include "communication/client/client.h"
#include "communication/communication_commons.h"
#include "communication/handshake/handshake.h"
#include "cli/cli.h"

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

//    mpz_out_str(stdout, 16, pub_key.n);
//    printf("\n");
//    mpz_out_str(stdout, 16, pub_key.e);
//    printf("\n");

    pid_t pid = fork();
    if (pid != 0) {//child will listen for connection requests

        chdir("./folder1");

        listen_for_connections(key);

    } else {//parent will connect

        chdir("./folder2");

        //@TODO Here will be the CLI

        //@FIXME cuz of the "loopback"
        sleep(5);
        init_cli();

        char *address = malloc(sizeof(char) * 16);

        scan_network(address);

        byte_array sym_key = {
                .data = malloc(DATAGRAM_SIZE)
        };
//        printf("\n%s\n", address);
//        getchar();

        int connection_socket = connect_to(address, PORT);

        byte_array send_buffer, recv_buffer;
        send_buffer.data = calloc(DATAGRAM_SIZE, 1);
        recv_buffer.data = calloc(DATAGRAM_SIZE, 1);

        socket_wb socketWb = {
                .socket_fd = connection_socket,

                .recv_buffer = recv_buffer,
                .send_buffer = send_buffer
        };


        if (init_handshake(socketWb, key, &sym_key)) {
            perror("Failed to init handshake");
        }

        //List folder request
        byte_array ls_buffer = {
                .data = calloc(DATAGRAM_SIZE, 1),
                .length = DATAGRAM_SIZE
        };

        list_folder_request(socketWb, sym_key, &ls_buffer);

        int line_count = get_line_count(ls_buffer.data) - 1;

        char **options = malloc(sizeof(char *) * line_count);

        split_options(ls_buffer.data, options);

        puts("Select a file to 'borrow':\n");
        int opt = select_option(line_count, options);

        char *filename = options[opt];
        //Request the file
        int blocks_num = request_file(socketWb, sym_key, filename);
        //printf("\n Blocks %d \n", blocks_num);


        byte_array block_buffer = {
                .data = calloc(DATAGRAM_SIZE, 1),
                .length = DATAGRAM_SIZE
        };

        printf("\n");
        FILE *fp = fopen(filename, "w");
        for (int i = 0; i < blocks_num; ++i) {
            //request block
            request_block(socketWb, sym_key, filename, i, &block_buffer);
            //move pointer
            fseek(fp, i * (DATAGRAM_SIZE/2), SEEK_SET);
            //write
            fwrite(block_buffer.data, sizeof(byte), block_buffer.length, fp);
            print_progress(i, blocks_num - 1);
        }
        printf(" Done\n");
        fclose(fp);

        free(block_buffer.data);
        free(ls_buffer.data);
        free(recv_buffer.data);
        free(send_buffer.data);
        free(sym_key.data);
        free(address);
        free(options);
        close_socket(connection_socket);

        kill(pid, SIGINT);
    }

    //free key
    mpz_clears(key.e, key.n, key.d, NULL);
    //clear RSA state
    RSA_clear();
    return 0;
}