/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//#include "./include/uECC.h"
#include <uECC.h>
#include "./include/types.h"
#include <sha256.h>
#include <security.h>
#include <rw_regs.h>

#include <libc.h>
#include <vm.h>
#include <config.h>
#include <vm.h>
//xxd -ps blink.bin
//int lVmConfig[][VMCONF_NUMCOLUNS] = VMCONF;

int isVmTrust(void) {
    //Tarefas: 

    //2) Funcionar contador de ticks para medir tempo
    //3) Work for two vm
    //4) Read hypercalls
    //5) Get size vm dinamicaly

    //mostrar numero de vms
    
    int numMachines = 0;
    //loop to verify all vms
    for (; numMachines < NVMACHINES; numMachines++) {
      
        

        //teste tempo para autenticacao VM
        //hal_sr_rcount(0);//set counter to 0
        //uint32_t count = hal_lr_rcount();
        //printf("\nValidacao da VM");

        char *lAddrVm;/* = (unsigned char*) 0x9d010000;*/
        //lAddrVm = (unsigned char*) 0x9d010000;
        
        //lAddrVm=VMCONF[numMachines].
        
        //lAddrVm = (unsigned char*) 0x9d018000;
        
        
        
        
        

        //teste leitura da VM
        long i = 0;
        int countSign = 0, countPubKey = 0;
        unsigned char value, countLine = 0;
        //long sizeVm = 131072; //size VM+pubKey+signature
        //TODO - get size of vm dynamic
        long sizeVm = 32768; //size VM+pubKey+signature 
        long sizeHash = sizeVm - 128; //only the size to calculate hash of vm
        //long sizeHash = 32768 - 128; //only the size to calculate hash of vm
        //char hash[32640]; //tamanho da VM
        //char *hash; //tamanho da VM
        //hash=(char*)malloc(sizeof(sizeHash));
        uint8_t public[64];
        uint8_t sigReceived[64];

        //read public key
        for (countPubKey = 0; countPubKey < 64; countPubKey++) {
            public[countPubKey] = lAddrVm[(sizeVm - 128) + countPubKey];
        }
        //verify public key
        if (!uECC_valid_public_key(public, uECC_secp256k1())) {
            printf("\nuECC_valid_public_key() failed\n");
            return 1;
        } else {
            printf("\nValid Public Key\n");
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
        /*for (i = 0; i < (sizeHash); i++) {//read vm excluding Public Key and Signature
            hash[i] = lAddrVm[i];
            //debug
            printf("%02x", hash[i]);
            countLine++;
            if (countLine == 30) {
                printf("\n");
                countLine = 0;
            }
        }*/

        //test- read memory of flas and sendo byte-byte do hash
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
        //sha256_update(&contextHash, hash, sizeHash);
        //for (i = 0; i < (sizeHash); i++) {
        sha256_update(&contextHash, lAddrVm, sizeHash);
        //}
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
        //    printf("Count totoal: %d\n", count);
    }
}