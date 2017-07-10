#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./include/uECC.h"
#include "./include/sha256.h"
#include <inttypes.h>
#include <stdint.h>


#define NUM_ECC_DIGITS 32
#define PRIVKEY_SIZE NUM_ECC_DIGITS
#define PUBKEY_SIZE (NUM_ECC_DIGITS*2)

#define pubkey_file "./public_key.txt"
#define privkey_file "./private_key.txt"


void print_hex(uint8_t key[], int len)
{
    int i;
    for (i = 0; i < len; ++i) {
        printf("%02x", (unsigned char) key[i]);
    }
    printf("\n");
}

int read_key(char *filename, uint8_t key[], int len)
{
    int i;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Canno open file.\n");
        return -1;
    }
    
    for (i = 0; i < len; ++i) {
        fscanf(fp, "%02hhX", (unsigned char*)&key[i]);
    }
    fclose(fp);

    //print_hex(key, len);

    return 0;
}

int load_pubkey(char *filename, uint8_t public[])
{
    // read public key
    if (read_key(filename, public, PUBKEY_SIZE) < 0) {
        printf("Cannot read public key.\n");
        return -1;
    }

    // verify public key
    if (!uECC_valid_public_key(public, uECC_secp256k1())) {
        printf("Invalid public key.\n");
        return -1;
    }

    return 0;
}

int load_privkey(char *filename, uint8_t private[], uint8_t public[])
{
    // read private key
    if (read_key(filename, private, PRIVKEY_SIZE) < 0) {
        printf("Cannot read private key.\n");
        return -1;
    }

    // compute the public key from the private one
    if (!uECC_compute_public_key(private, public, uECC_secp256k1())) {
        printf("Cannot compute public key.\n");
        return -1;
    }

    // verify public key
    if (!uECC_valid_public_key(public, uECC_secp256k1())) {
        printf("Invalid public key.\n");
        return 1;
    }

    return 0;
}

int save_key(char *filename, uint8_t key[], int len)
{
    int i;
    FILE *fp;

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("\nCannot create file.\n");
        return 1;
    }

    for (i = 0; i < len; ++i) {
        fprintf(fp, "%02x", (unsigned char) key[i]);
    }
    fprintf(fp, "\n");

    fclose(fp);

    print_hex(key, len);

    return 0;
}

long file_lenght(FILE *file)
{
    long pos, end;
    pos = ftell(file);
    fseek(file, 0, SEEK_END);
    end = ftell(file);
    fseek(file, pos, SEEK_SET);
    return end;
}
