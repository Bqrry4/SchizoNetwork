//
// Created by radu on 13.03.2024.
//

#ifndef SI_PROIECT_DATA_TYPES_H
#define SI_PROIECT_DATA_TYPES_H

#include "sys/types.h"
#include <stdbool.h>

typedef u_int8_t byte;

typedef struct {
    byte* data;
    u_int16_t length;
} byte_array;

typedef u_int32_t word;

///@brief Length of static array
#define length(array) (sizeof(array)/sizeof(array[0]))

#endif //SI_PROIECT_DATA_TYPES_H
