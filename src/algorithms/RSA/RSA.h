//
// Created by bqrry on 27.03.2024.
//

#ifndef SI_PROIECT_RSA_H
#define SI_PROIECT_RSA_H
#include "../../commons/data_types.h"
#include <gmp.h>

typedef struct{
    mpz_t n, e, d;
} rsa_keys;

// @param out public_key
// @param out private_key
void key_generation(rsa_keys *key);

#endif //SI_PROIECT_RSA_H
