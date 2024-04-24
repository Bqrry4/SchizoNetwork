//
// Created by radu on 13.03.2024.
//

#include <malloc.h>
#include <limits.h>
#include "RC6.h"

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
        for (int r = 1; r <= R; r++) {
            word t = RSL(B * (2 * B + 1), LOG_W);
            word u = RSL(D * (2 * D + 1), LOG_W);
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

void decrypt(byte_array ciphertext, byte_array key, byte_array plaintext) {
    //round keys
    word S[2 * R + 4];

    key_schedule(key, S);
    //ciphertext words
    for (int i = 0; i < ciphertext.length; i += 16) {
        word A, B, C, D;
        A = get_little_endian_word(ciphertext, i);
        B = get_little_endian_word(ciphertext, i + 4);
        C = get_little_endian_word(ciphertext, i + 8);
        D = get_little_endian_word(ciphertext, i + 12);

        C = C - S[2 * R + 3];
        A = A - S[2 * R + 2];
        for (int r = R; r >= 1; r--) {
            word t = D;
            D = C;
            C = B;
            B = A;
            A = t;
            word u = RSL(D * ((D << 1) + 1), LOG_W);
            t = RSL(B * ((B << 1) + 1), LOG_W);
            C = RSR(C - S[2 * r + 1], t) ^ u;
            A = RSR(A - S[2 * r], u) ^ t;
        }
        D = D - S[1];
        B = B - S[0];

        insert_word(plaintext, i, A);
        insert_word(plaintext, i + 4, B);
        insert_word(plaintext, i + 8, C);
        insert_word(plaintext, i + 12, D);
    }
}

word get_little_endian_word(byte_array array, int index) {
    word first_byte = index < array.length ? array.data[index] : 0;
    word second_byte = (index + 1) < array.length ? (array.data[index + 1] << 8) : 0;
    word third_byte = (index + 2) < array.length ? (array.data[index + 2] << 16) : 0;
    word fourth_byte = (index + 3) < array.length ? (array.data[index + 3] << 24) : 0;
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
    u_int16_t c = (key.length / 4) + ((key.length % 4) != 0 ? 1 : 0);
    c = max(1, c);

    word *L = malloc(sizeof(word) * c);

    for (u_int16_t i = 0; i < key.length; i += 4) {
        L[i / 4] = get_little_endian_word(key, i);
    }

    S[0] = P;
    for (int i = 1; i < 2 * R + 4; i++) {
        S[i] = S[i - 1] + Q;
    }

    word A = 0, B = 0;
    int i = 0, j = 0;
    int v = 3 * (max(c, 2 * R + 4));
    for (int s = 1; s <= v; s++) {
        A = S[i] = RSL(S[i] + A + B, 3);
        B = L[j] = RSL(L[j] + A + B, (A + B));
        i = (i + 1) % (2 * R + 4);
        j = (j + 1) % c;
    }

    free(L);
}

inline int max(int a, int b) {
    return a < b ? b : a;
}

//word RSR(word a, word b) {
//    u_int32_t times = b & 0x1f;
//    return (a >> times) | (a << (32 - times));
//}
//
//word RSL(word a, word b) {
//    u_int32_t times = b & 0x1f;
//    return (a << times) | (a >> (32 - times));
//}

static inline word RSL (word n, word c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);  // assumes width is a power of 2.

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n<<c) | (n>>( (-c)&mask ));
}

static inline word RSR (word n, word c)
{
    const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);

    // assert ( (c<=mask) &&"rotate by type width or more");
    c &= mask;
    return (n>>c) | (n<<( (-c)&mask ));
}