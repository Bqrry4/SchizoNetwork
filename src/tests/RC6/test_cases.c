//
// Created by radu on 21.04.2024.
//

#include <stdlib.h>
#include <stdio.h>
#include "test_cases.h"
#include "RC6.h"

void test_case_1() {
    char plaintext_string[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"; //and key

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

    printf("Test 1:\n");
    printf("\tOriginal text (hex): ");
    for (int i = 0; i < plaintext.length; ++i) {
        printf("%x ", plaintext.data[i]);
    }
    printf("\n");

    encrypt(plaintext, key, ciphertext);

    printf("\tEncrypted hex: ");
    for (int i = 0; i < ciphertext.length; ++i) {
        printf("%x ", ciphertext.data[i]);
    }
    printf("\n");

    decrypt(ciphertext, key, deciphertext);

    printf("\tDecrypted hex: ");
    for (int i = 0; i < deciphertext.length; ++i) {
        printf("%x ", deciphertext.data[i]);
    }
    printf("\n");

    free(plaintext.data);
    free(key.data);
    free(ciphertext.data);
    free(deciphertext.data);
}

void test_case_2() {
    byte plaintext_bytes[] = {0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79, 0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0,
                              0xf1};
    byte key_bytes[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78};

    byte_array plaintext = {
            .data = malloc(16 * sizeof(byte)),
            .length = 16
    };
    memcpy(plaintext.data, plaintext_bytes, plaintext.length);

    byte_array key = {
            .data = malloc(16 * sizeof(byte)),
            .length = 16
    };
    memcpy(key.data, key_bytes, key.length);

    u_int16_t ciphertext_length = get_encrypted_block_length(plaintext.length);

    byte_array ciphertext = {
            .data = malloc(ciphertext_length * sizeof(byte)),
            .length = ciphertext_length
    };

    byte_array deciphertext = {
            .data = malloc(plaintext.length * sizeof(byte)),
            .length = plaintext.length
    };

    printf("Test 2:\n");
    printf("\tOriginal text (hex): ");
    for (int i = 0; i < plaintext.length; ++i) {
        printf("%x ", plaintext.data[i]);
    }
    printf("\n");

    encrypt(plaintext, key, ciphertext);

    printf("\tEncrypted hex: ");
    for (int i = 0; i < ciphertext.length; ++i) {
        printf("%x ", ciphertext.data[i]);
    }
    printf("\n");

    decrypt(ciphertext, key, deciphertext);

    printf("\tDecrypted hex: ");
    for (int i = 0; i < deciphertext.length; ++i) {
        printf("%x ", deciphertext.data[i]);
    }
    printf("\n");

    free(plaintext.data);
    free(key.data);
    free(ciphertext.data);
    free(deciphertext.data);
}

void execute_tests() {
    test_case_1();
    test_case_2();
}