#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./include/uECC.h"
#include "./include/sha256.h"
#include <inttypes.h>
#include <stdint.h>
#include "ecdsa.h"

#define privkey_file "./private_key.txt"
#define image_file "/tmp/ping.bin"

#define HASH_LEN SHA256_BLOCK_SIZE
#define SIGN_LEN 64

int compute_hash(char *filename, unsigned char hash[])
{
    FILE *fp;
    long file_size = 0;
    SHA256_CTX ctx;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open image file.\n");
        return -1;
    }
    file_size = file_lenght(fp);

    printf("file size: %ld\n", file_size);

    unsigned char hashBinarioPing[file_size];
    fread(hashBinarioPing, 1, file_size, fp);


    sha256_init(&ctx);
    sha256_update(&ctx, hashBinarioPing, file_size);
    sha256_final(&ctx, hash);

    return 0;
}

int save_signature(char *filename, uint8_t signature[], uint8_t public[])
{
    //create temp file to concatenate signatura in vm binary
    FILE *fp;
    char tmp[256];

    sprintf(tmp, "%s.security.tmp", filename);
    fp = fopen(tmp, "w+b");
    if (fp == NULL) {
        printf("Cannot create file,\n");
        return -1;
    }
    
    fwrite(public, 1, 64, fp);
    fwrite(signature, 1, 64, fp);
    fflush(fp);
    fclose(fp);

    return 0;
}

int main(int argc, char *argv[])
{
    uint8_t private[PRIVKEY_SIZE];
    uint8_t public[PUBKEY_SIZE];
    uint8_t hash[HASH_LEN];
    uint8_t signature[SIGN_LEN];

    if (argc != 3) {
        printf("Usage: %s image_file.bin private_key.txt\n", argv[0]);
        return 1;
    }

    /* load key */
    if (load_privkey(argv[2], private, public) < 0) {
        exit(1);
    }

    //printf("private_key: ");
    //print_hex(private, PRIVKEY_SIZE);

    printf("public_key: ");
    print_hex(public, PUBKEY_SIZE);

    if (compute_hash(argv[1], hash) < 0) {
        printf("Cannot compute file hash.\n");
        return 1;
    }

    printf("hash: ");
    print_hex(hash, HASH_LEN);

    if (!uECC_sign(private, hash, sizeof(hash), signature, uECC_secp256k1())) {
        printf("Failed to sign image file.\n");
        return 1;
    }

    printf("signature: ");
    print_hex(signature, SIGN_LEN);

    if (!uECC_verify(public, hash, sizeof(hash), signature, uECC_secp256k1())) {
        printf("Signature verification failed.\n");
        return 1;
    } else {
        printf("Signature verification OK.\n");
    }

    save_signature(argv[1], signature, public);

    return (0);
}
