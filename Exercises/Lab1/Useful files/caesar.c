/**
 * \file brutus.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2020-03-05
 * \short Encrypt/Decrypt text using Caesar algorithm
 */

#include <stdio.h>

/* Enum for encryption mode */
typedef enum {
    ENCRYPT,
    DECRYPT
} encrypt_mode;

/**
 * \short Encrypt/decrypt character using Caesar algorithm
 *
 * \param ch Character to encrypt/decrypt
 * \param mode ENCRYPT or DECRYPT
 * \param key Key to use for Caesar algorithm. Must be in [0, 25] range
 *
 * \example caesar('A', ENCRYPT, 3) == 'D'
 * \example caesar('D', DECRYPT, 3) == 'A'
 *
 * \note Key must be in [0, 25] range!
 *
 * \return Result character of encryption or decryption
 */
char caesar(unsigned char ch, encrypt_mode mode, int key)
{
    if (ch >= 'a' && ch <= 'z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'a') ch += 26;
        }
        return ch;
    }

    if (ch >= 'A' && ch <= 'Z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'Z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'A') ch += 26;
        }
        return ch;
    }

    return ch;
}

/**
 * Program entry point.
 */
int main(int argc, char** argv) {
    printf("Encrypting %c with key %d ==> %c\n", 'a', 10, caesar('a', ENCRYPT, 10));
    printf("Decrypting %c with key %d ==> %c\n", 'Z', 20, caesar('Z', DECRYPT, 20));

    return 0;
}