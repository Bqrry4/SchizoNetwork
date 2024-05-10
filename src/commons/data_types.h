//
// Created by radu on 13.03.2024.
//

#ifndef SI_PROIECT_DATA_TYPES_H
#define SI_PROIECT_DATA_TYPES_H

#include "sys/types.h"
#include <stdbool.h>
#include <string.h>

typedef u_int8_t byte;

/** @struct byte_array
 *  @var data pointer to an array of bytes
 *  @var length of the populated part of the array, not the real size
 */
typedef struct {
    byte* data;
    size_t length;
} byte_array;

typedef u_int32_t word;

///@brief Length of static array
#define length(array) (sizeof(array)/sizeof(array[0]))

#define get_byte_array(string) { .data = string, .length = strlen(string) }

#define get_encrypted_block_length(plaintext_length) (plaintext_length % 16 != 0 ? (plaintext_length & ~0xF) + 16 : plaintext_length)

#endif //SI_PROIECT_DATA_TYPES_H
