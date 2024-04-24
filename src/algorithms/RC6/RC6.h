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

///@param [in] plaintext Bytes to be encrypted, will be padded with zeroes if length is not a multiple of 16
///@param [in] key Encryption key, length must be between 0 and 255 bytes
///@param [out] ciphertext Encrypted bytes, length will always be a multiple of 16
void RC6_encrypt(byte_array plaintext, byte_array key, byte_array ciphertext);

///@param [in] ciphertext Encrypted bytes, length will always be a multiple of 16
///@param [in] key Encryption key, length must be between 0 and 255 bytes
///@param [out] plaintext Decrypted
void RC6_decrypt(byte_array ciphertext, byte_array key, byte_array plaintext);

#endif //SI_PROIECT_RC6_H
