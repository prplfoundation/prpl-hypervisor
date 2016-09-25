/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "./include/uECC.h"
#include "./include/types.h"
#include "./include/sha256.h"
#include "./include/security.h"
#include "rw_regs.h"
//#include "include/curve-specific.inc"
//#include "bare-metal-apps/include/pic32mz.h"

#include <vm.h>
#include <config.h>
//xxd -ps blink.bin
//int lVmConfig[][VMCONF_NUMCOLUNS] = VMCONF;

int isVmTrusted(void) {
    uint32_t count;
    //teste tempo para autenticacao VM
//    hal_sr_rcount(0);//set counter to 0
//    uint32_t count = hal_lr_rcount();
    
    
    unsigned char volatile *lAddrVm = (unsigned char*) 0x9d010000;

    //teste leitura da VM
    long i = 0;
    int countSign = 0, countPubKey = 0;
    unsigned char value, countLine = 0;
    long sizeVm = 131072; //size VM+pubKey+signature
    long sizeHash = sizeVm - 128; //only the size to calculate hash of vm
    char hash[sizeVm]; //tamanho da VM
    uint8_t public[64];
    uint8_t sigReceived[64];
    printf("\nValue VM add: %d\n", value);

    //read public key
    for (countPubKey = 0; countPubKey < 64; countPubKey++) {
        public[countPubKey] = lAddrVm[(sizeVm - 128) + countPubKey];
    }
    //verify public key
    if (!uECC_valid_public_key(public, uECC_secp256k1())) {
        printf("uECC_valid_public_key() failed\n");
    } else {
        printf("Chave publica valida\n");
    }
    //debug public key
    /*printf("uint8_t PubKey[2*NUM_ECC_DIGITS] = {");
    vli_print(public, sizeof (public));
    printf("};\n");
    printf("\n\n");*/

    //read signature
    for (countSign = 0; countSign < 64; countSign++) {
        sigReceived[countSign] = lAddrVm[(sizeVm - 64) + countSign];
    }

    //debug signature
    /*printf("uint8_t Signature[2*NUM_ECC_DIGITS] = {");
    vli_print(sigReceived, sizeof (sigReceived));
    printf("};\n");
    printf("\n\n");*/

    //read vm to calculate hash
    //for (i = 0; i < (sizeVm-128); i++) {//read vm excluding Public Key and Signature
    for (i = 0; i < (sizeHash); i++) {//read vm excluding Public Key and Signature
        hash[i] = lAddrVm[i];
        //debug
        /*printf("%02x", hash[i]);
        countLine++;
        if (countLine == 30) {
            printf("\n");
            countLine = 0;
        }*/
    }

    //calculate hash of vm
    uint8_t tmp[2 * SHA256_BLOCK_SIZE + 64];
    SHA256_HashContext ctx = {
        {
         &init_SHA256,
         &update_SHA256,
         &finish_SHA256,
         2 * SHA256_BLOCK_SIZE,
         SHA256_BLOCK_SIZE,
         tmp
        }
    };

    SHA256_CTX contextHash;
    BYTE buf[SHA256_BLOCK_SIZE];
    sha256_init(&contextHash);
    sha256_update(&contextHash, hash, sizeHash);
    sha256_final(&contextHash, buf);

    //debug hash
    /*printf("uint8_t hash[NUM_ECC_DIGITS] = {");
    vli_print(buf, sizeof (buf));
    printf("};\n");
    printf("\n\n");*/


    /*tests to detect fail*/
    //public[10]=0xAA;
    //sigReceived[1]=0xAA;
    //buf[30]=0xAA;


    //verify signature
    if (!uECC_verify(public, buf, sizeof (buf), sigReceived, uECC_secp256k1())) {
        printf("uECC_verify() failed\n");
        return 1;
    } else {
        printf("uECC_verify() OK\n");
    }
    
//    count = hal_lr_rcount();
    printf("Time totoal: %d\n", count);
}