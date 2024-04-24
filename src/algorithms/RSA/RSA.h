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

typedef struct{
    mpz_ptr n, e;
} rsa_pub_key;

typedef struct{
    mpz_ptr n, d;
} rsa_prv_key;

/// @brief init state
void RSA_init();
/// @brief clear state
void RSA_clear();

/// @brief Generates RSA keys
/// @param[out] rsa_keys
/// @param[in] n size of the prime numbers, key-size/2
void RSA_key_generation(rsa_keys *key, int n);

void RSA_encrypt(byte_array plaintext, rsa_pub_key key, byte_array *ciphertext);
void RSA_decrypt(byte_array ciphertext, rsa_prv_key key, byte_array *plaintext);

#endif //SI_PROIECT_RSA_H
