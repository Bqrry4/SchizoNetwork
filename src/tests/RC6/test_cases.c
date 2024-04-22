//
// Created by radu on 21.04.2024.
//

#include <stdlib.h>
#include <stdio.h>
#include "test_cases.h"
#include "RC6.h"

void test_case_1() {
//    char original_plaintext[] = "1234567812345678";
    char plaintext_string[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"; //and key
//    char key_string[] = "123456789abcdef";

    byte_array plaintext = {
            .data = malloc(16 * sizeof(byte)),
            .length = 16
    };
    memcpy(plaintext.data, plaintext_string, plaintext.length);

    byte_array key = {
            .data = malloc(16 * sizeof(byte)),
            .length = 16
    };
    memcpy(key.data, plaintext_string, key.length);

    u_int16_t ciphertext_length = get_encrypted_block_length(plaintext.length);

    byte_array ciphertext = {
            .data = malloc(ciphertext_length * sizeof(byte)),
            .length = ciphertext_length
    };

    byte_array deciphertext = {
            .data = malloc(plaintext.length * sizeof(byte)),
            .length = plaintext.length
    };

    printf("Test1:\n");
//    printf("\tOriginal text: %s\n", original_plaintext);

    encrypt(plaintext, key, ciphertext);

    printf("\tEncrypted text (hex) (length: %d): ", ciphertext.length);

    for (int i = 0; i < ciphertext.length; ++i) {
        printf("%x ", ciphertext.data[i]);
    }
    printf("\n");

    decrypt(ciphertext, key, deciphertext);

//    char *decyphertext_string = deciphertext.data;

    //printf("\tDecrypted text: %s\n", decyphertext_string);
    printf("\tDecrypted hex: ");


    for (int i = 0; i < deciphertext.length; ++i) {
        printf("%x", deciphertext.data[i]);
    }
    printf("\n");

    free(plaintext.data);
    free(key.data);
    free(ciphertext.data);
    free(deciphertext.data);
}

void execute_tests() {
    test_case_1();
}