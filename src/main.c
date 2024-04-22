//
// Created by bqrry on 06.04.2024.
//
#include <stdio.h>
#include "algorithms/RSA/RSA.h"
#include "tests/RC6/test_cases.h"
#include "RC6.h"

int main() {

//    rsa_keys key;
//    mpz_init2(key.e, 2048);
//    mpz_init2(key.n, 2048);
//    mpz_init2(key.d, 2048);
//
//    key_generation(&key);
//
//    mpz_out_str(stdout, 10, key.e);
//    printf("\n");
//    mpz_out_str(stdout, 10, key.n);
//    printf("\n");
//    mpz_out_str(stdout, 10, key.d);
//    printf("\n");
//    //mpz_out_str(stdout, 10, key.d);
//
//    mpz_clears(key.e, key.n, key.d, NULL);

    execute_tests();

    return 0;
}