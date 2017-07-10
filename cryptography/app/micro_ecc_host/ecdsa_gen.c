#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./include/uECC.h"
#include "./include/sha256.h"
#include <inttypes.h>
#include <stdint.h>
#include "ecdsa.h"

#define pubkey_file "./public_key.txt"
#define privkey_file "./private_key.txt"

int main(int argc, char *argv[])
{
    uint8_t private[PRIVKEY_SIZE];
    uint8_t public[PUBKEY_SIZE];

    //generate private/public key
    if (!uECC_make_key(public, private, uECC_secp256k1())) {
        printf("Failed to generate keypair.\n");
        return 1;
    }

    printf("private_key: ");
    save_key(privkey_file, private, sizeof(private));
    printf("public_key: ");
    save_key(pubkey_file, public, sizeof(public));

    /* load and verify it */
    load_privkey(privkey_file, private, public);
    load_pubkey(pubkey_file, public);

    return (0);
}
