#ifndef __ECDSA__
#define __ECDSA__

#include "./include/uECC.h"


#define NUM_ECC_DIGITS 32
#define PRIVKEY_SIZE NUM_ECC_DIGITS
#define PUBKEY_SIZE (NUM_ECC_DIGITS*2)


void print_hex(uint8_t key[], int len);
int read_key(char *filename, uint8_t key[], int len);
int load_pubkey(char *filename, uint8_t public[]);
int load_privkey(char *filename, uint8_t private[], uint8_t public[]);
int save_key(char *filename, uint8_t key[], int len);
long file_lenght(FILE *file);

#endif /* __ECDSA__ */