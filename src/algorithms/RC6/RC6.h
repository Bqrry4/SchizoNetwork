//
// Created by radu on 13.03.2024.
//

#ifndef SI_PROIECT_RC6_H
#define SI_PROIECT_RC6_H

#include "../../commons/data_types.h"

#define W 32
#define LOG_W 5
#define R 20
#define P 0xB7E15163
#define Q 0x9E3779B9

void encrypt(byte_array plaintext, byte_array key, byte_array ciphertext);

void decrypt(byte_array ciphertext, byte_array key, byte_array plaintext);

void key_schedule(byte_array key, word *S);

word get_little_endian_word(byte_array array, int index);

void insert_word(byte_array array, int index, word le_word);

int max(int a, int b);

static inline word RSR(word a, word b);

static inline word RSL(word a, word b);

#endif //SI_PROIECT_RC6_H
