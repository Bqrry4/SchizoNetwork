//
// Created by bqrry on 27.03.2024.
//

//Note: test functions were written with big numbers in mind, so they are local, don't reuse them
#include <gmp.h>
#include "../../commons/data_types.h"

#include "RSA.h"

static int first_primes[] = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89,
                             97};
///@brief Performs division test on first odd primes <100
///@param in possible_prime
static bool trial_division_test(mpz_t possible_prime) {
    bool is_prime = false;
    mpz_t res;
    mpz_init(res);

    for (int i = 0; i < length(first_primes); ++i) {
        mpz_mod_ui(res, possible_prime, first_primes[i]);

        if (!mpz_cmp_ui(res, 0)) {
            is_prime = true;
            break;
        }
    }

    mpz_clear(res);
    return is_prime;
}

///@brief Performs the Baillie–PSW primality test
///@param in possible_prime
static bool baillie_psw_test(mpz_t possible_prime) {

}

///@brief Performs the Miller-Rabin primality test
///@param in possible_prime
static bool miller_rabin_test(mpz_t possible_prime, int reps) {

//    int s, d;
//    for (int i = 0; i < reps; ++i) {
//        int a = mpz_urandomm();
//        int x =;
//        for (int j = 0; j < s; ++j) {
//            int y =;
//            if (y == 1 && x != 1 && x != n - 1);
//            return false;
//            x = y;
//        }
//
//    }

}

static bool primality_test(mpz_t possible_prime) {
    return trial_division_test(possible_prime) &&
           baillie_psw_test(possible_prime) &&
           miller_rabin_test(possible_prime, 24);
}

///@brief Generate a prime number in the (2^(n - 1); 2^n - 1) interval
///@param[out] prime
///@param[in] n
static void gen_prime(mpz_t prime, int n) {

    mpz_t min, rand;
    mpz_init2(min, n);
    mpz_init2(rand, n);

    //bitwise lshift, init min bound as 2^(n-1) or set the MSB to 1
    mpz_set_ui(min, 1);
    mpz_mul_2exp(min, min, n - 1);

    //using Mersenne Twister algorithm
    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    gmp_randseed_ui(rstate, 3);

    do {
        //Generate values for bits other than MSB
        mpz_urandomb(rand, rstate, n - 1); //mpz_rrandomb??
        //Construct the number
        mpz_add(rand, min, rand);
        //Make it odd, as all primes >2 are odd
        if (mpz_even_p(rand)) {
            mpz_sub_ui(rand, rand, 1);
        }
    }
    //while (primality_test(rand));
    while (!mpz_probab_prime_p(rand, 24));
    mpz_set(prime, rand);

    gmp_randclear(rstate);
    mpz_clears(min, rand, NULL);
}

///@brief Compute the a*x = 1 mod m
///@param[out] x
///@param[in] a
///@param[in] m
static void inverse_modular(mpz_t x, mpz_t a, mpz_t m)
{
    mpz_t r0, r1, s0, s1, q, t;
    mpz_init_set(r0, a);
    mpz_init_set(r1, m);
    mpz_init_set_ui(s0, 1);
    mpz_init_set_ui(s1, 0);
    mpz_init(q);
    mpz_init(t);

    while(mpz_cmp_ui(r1, 0))
    {
        //q = r0 / r1;
        mpz_tdiv_q(q, r0, r1);
        //t = r1;
        mpz_set(t, r1);
        //r1 = r0 - q * r1;
        mpz_mul(r1, q, r1);
        mpz_sub(r1, r0, r1);
        //r0 = t;
        mpz_set(r0, t);
        //t = s1;
        mpz_set(t, s1);
        //s1 = s0 - q * s1;
        mpz_mul(s1, q, s1);
        mpz_sub(s1, s0, s1);
        //s0 = t;
        mpz_set(s0, t);
    }

    if (mpz_cmp_ui(r1, 0) < 0)
        mpz_mod(s0, s0, m);

    mpz_set(x, s0);

    mpz_clears(r0, r1, s0, s1, q, t, NULL);
}

void key_generation(rsa_keys *key) {
    mpz_t p, q, lambda;

    mpz_init2(p, 1024);
    mpz_init2(q, 1024);
    mpz_init2(lambda, 1024);

    gen_prime(p, 1024);
    gen_prime(q, 1024);

    mpz_mul(key->n, p, q);
    //Using the lambda version
    //lambda = lcm(p - 1, q - 1);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_lcm(lambda, p, q);

    //choosing the e
    mpz_set_ui(key->e, 65537);

    //finding the d
    inverse_modular(key->d, key->e, lambda);

    mpz_clears(p, q, lambda, NULL);
}

void encrypt_RSA() {

}

void decrypt_RSA() {

}