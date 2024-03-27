//
// Created by radu on 13.03.2024.
//

#include <malloc.h>
#include "RC6.h"

//Encrypts null-terminated byte array
void encrypt(byte_array plaintext, byte_array key, byte_array ciphertext) {
    //round keys
    word S[2 * R + 4];

    key_schedule(key, S);
    //plaintext words
    for (int i = 0; i < plaintext.length; i += 16) {
        word A, B, C, D;
        A = get_little_endian_word(plaintext, i);
        B = get_little_endian_word(plaintext, i + 4);
        C = get_little_endian_word(plaintext, i + 8);
        D = get_little_endian_word(plaintext, i + 12);

        B = B + S[0];
        D = D + S[1];
        for (int r = 0; r < R; r++) {
            word t = RSL(B * ((B << 1) + 1), LOGW);
            word u = RSL(D * ((D << 1) + 1), LOGW);
            A = RSL(A ^ t, u) + S[2 * r];
            C = RSL(C ^ u, t) + S[2 * r + 1];
            t = A;
            A = B;
            B = C;
            C = D;
            D = t;
        }
        A = A + S[2 * R + 2];
        C = C + S[2 * R + 3];

        insert_word(ciphertext, i, A);
        insert_word(ciphertext, i + 4, B);
        insert_word(ciphertext, i + 8, C);
        insert_word(ciphertext, i + 12, D);
    }
}

word get_little_endian_word(byte_array array, int index) {
    byte first_byte = index < array.length ? array.data[index] : 0;
    byte second_byte = index + 1 < array.length ? (array.data[index + 1] << 8) : 0;
    byte third_byte = index + 2 < array.length ? (array.data[index + 2] << 16) : 0;
    byte fourth_byte = index + 3 < array.length ? (array.data[index + 3] << 24) : 0;
    return first_byte | second_byte | third_byte | fourth_byte;
}

void insert_word(byte_array array, int index, word le_word) {
    byte first_byte = le_word & 0xFF;
    byte second_byte = (le_word >> 8) & 0xFF;
    byte third_byte = (le_word >> 16) & 0xFF;
    byte fourth_byte = (le_word >> 24) & 0xFF;

    if (index < array.length) {
        array.data[index] = first_byte;
        if (index + 1 < array.length) {
            array.data[index + 1] = second_byte;
            if (index + 2 < array.length) {
                array.data[index + 2] = third_byte;
                if (index + 3 < array.length)
                    array.data[index + 3] = fourth_byte;
            }
        }
    }
}

void key_schedule(byte_array key, word *S) {
    //Load key bytes into words
    u_int16_t c = key.length / 4 + key.length % 4 != 0 ? 1 : 0;
    word *L = malloc(sizeof(word) * c);
    for (u_int16_t i = 0; i < key.length; i += 4) {
        L[i / 4] = get_little_endian_word(key, i);
    }

    S[0] = P;
    for (byte i = 1; i < (byte) (2 * R + 4); i++) {
        S[i] = S[i - 1] + Q;
    }

    word A = 0, B = 0;
    byte i = 0, j = 0;
    int v = max(c, 2 * R + 4);
    for (int s = 0; s < v; s++) {
        A = S[i] = RSL(S[i] + A + B, 3);
        B = L[j] = RSL(L[j] + A + B, (A + B));
        i = (i + 1) % (2 * R + 4);
        j = (j + 1) % c;
    }

    free(L);
}

int max(int a, int b) {
    return a < b ? b : a;
}

word RSR(word a, word b) {
    u_int32_t times = b & 0x1f;
    return (a >> times) | (a << (32 - times));
}

word RSL(word a, word b) {
    u_int32_t times = b & 0x1f;
    return (a << times) | (a >> (32 - times));
}