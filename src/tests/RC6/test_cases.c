//
// Created by radu on 21.04.2024.
//

#include <stdlib.h>
#include <stdio.h>
#include "test_cases.h"
#include "RC6.h"

void test_case_1() {
    char original_plaintext[] = "1234567890";
    char key_string[] = "abcdefghij";

    byte_array plaintext = get_byte_array(original_plaintext);
    byte_array key = get_byte_array(key_string);

    byte_array cyphertext = {
            .data = malloc(plaintext.length * sizeof(byte)),
            .length = plaintext.length
    };

    byte_array decyphertext = {
            .data = malloc(plaintext.length * sizeof(byte)),
            .length = plaintext.length
    };

    encrypt(plaintext, key, cyphertext);

    decrypt(cyphertext, key, decyphertext);

    char* decyphertext_string = decyphertext.data;

    printf("Original text: %s\nDecrypted text: %s", original_plaintext, decyphertext_string);
}