//
// Created by bqrry on 06.04.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms/RSA/RSA.h"

int main() {

    RSA_init();

    rsa_keys key;
    mpz_init2(key.e, 2048);
    mpz_init2(key.n, 2048);
    mpz_init2(key.d, 2048);

    RSA_key_generation(&key, 1024);

    mpz_out_str(stdout, 16, key.e);
    printf("\n");
    mpz_out_str(stdout, 16, key.n);
    printf("\n");
    mpz_out_str(stdout, 16, key.d);
    printf("\n");

    byte_array a;
    byte_array b, e;

    a.data = malloc( 4096 *sizeof(char));
    char ch[] = "I understand that it's an antipattern, but it's convenient.";

    memcpy(a.data, ch, sizeof(ch)/sizeof(char));
    a.length = sizeof(ch)/sizeof(char);

    b.data = malloc(4096 * sizeof(char));
    b.length = 4096;

    e.data = malloc(4096 * sizeof(char));
    e.length = 3;

    rsa_pub_key pub_key;
    pub_key.n = key.n;
    pub_key.e = key.e;

    rsa_prv_key prv_key;
    prv_key.d = key.d;
    prv_key.n = key.n;


    RSA_encrypt(a,pub_key, &b);
    printf("\nEncrypted: ");
    for(int i = 0; i < b.length; ++i)
    {
        printf("%x", b.data[i]);
    }
    printf("\n");

    RSA_decrypt(b, prv_key, &e);
    printf("Decrypted: %s \nHex: ", e.data);

    for(int i = 0; i < e.length; ++i)
    {
        printf("%x", e.data[i]);
    }
    printf("\n");

    free(a.data);
    free(b.data);
    free(e.data);

    mpz_clears(key.e, key.n, key.d,  NULL);

    RSA_clear();
    return 0;
}