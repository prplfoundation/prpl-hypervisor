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
//#include <rw_regs.h>
#include <hal.h>
#include <libc.h>
#include <vm.h>
#include <config.h>
#include <vm.h>
#include <globals.h>
//#include <crypto.h>


int sha_test(void) {


    //    securityAssociation enc_sa __attribute__((coherent, aligned (16)));
    //    securityAssociation dec_sa __attribute__((coherent, aligned (16)));

    return 1;

}

int isVmTrust(void)
{
    int i;
    uint32_t initialCountTotal = 0, finalCountTotal = 0;
    uint32_t initialCountHash = 0, finalCountHash = 0;
    uint32_t initialCountVerifySignature = 0;
    uint32_t initialCountVerifyPK = 0, finalCounterVerifyPK = 0;
    
    printf("\nVerifying virtual machines.\n");
    printf("Number of VMs: %d\n", NVMACHINES);


    unsigned char *lAddrVm = NULL;
    int countNumMachines = 0;

    //loop to verify all vms
    for (; countNumMachines < NVMACHINES; countNumMachines++) {

        initialCountTotal = getCounter();

        (lAddrVm) = (unsigned char*) VMCONF[countNumMachines].flash_base_add;

        //        INFO("Configuring VM %d, of size %d FLASH size, address: %x.\n\n", countNumMachines, VMCONF[countNumMachines].flash_size, (char*) lAddrVm);

        int countSign = 0, countPubKey = 0;
        long sizeVm = VMCONF[countNumMachines].flash_size; //size VM+pubKey+signature 
        long sizeHash = sizeVm - 128; //only the size to calculate hash of vm
        uint8_t public[64];
        uint8_t sigReceived[64];

        //----------------------------------------------------------------------
        //read public key
        for (countPubKey = 0; countPubKey < 64; countPubKey++) {
            public[countPubKey] = lAddrVm[(sizeVm - 128) + countPubKey];
        }
        //debug public key
        //        printf("uint8_t PubKey[2*NUM_ECC_DIGITS] = {");
        //        vli_print(public, sizeof (public));
        //        printf("};\n");
        //        printf("\n\n");
        initialCountVerifyPK = getCounter();
        //verify public key
        if (!uECC_valid_public_key(public, uECC_secp256k1())) {
            printf("Failed to find public key.\n");
            return 0;
        }/* else {
            printf("OK.\n", countNumMachines);
        }*/
        finalCounterVerifyPK = getCounter();
        //----------------------------------------------------------------------
        //read signature
        for (countSign = 0; countSign < 64; countSign++) {
            sigReceived[countSign] = lAddrVm[(sizeVm - 64) + countSign];
        }
        //debug signature
        //        printf("uint8_t Signature[2*NUM_ECC_DIGITS] = {");
        //        vli_print(sigReceived, sizeof (sigReceived));
        //        printf("};\n");
        //        printf("\n\n");
        //----------------------------------------------------------------------

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
        //uint8_t tmp[2 * SHA256_BLOCK_SIZE + 64];
        /*SHA256_HashContext ctx = {
            {
             &init_SHA256,
             &update_SHA256,
             &finish_SHA256,
             2 * SHA256_BLOCK_SIZE,
             SHA256_BLOCK_SIZE,
             tmp
            }
        };*/

        initialCountHash = getCounter();
        SHA256_CTX contextHash;
        BYTE buf[SHA256_BLOCK_SIZE];
        sha256_init(&contextHash);
        //sha256_update(&contextHash, hash, sizeHash);
        //for (i = 0; i < (sizeHash); i++) {
        sha256_update(&contextHash, lAddrVm, sizeHash);
        //}
        sha256_final(&contextHash, buf);
        finalCountHash = getCounter();

        //debug hash
        //        printf("uint8_t hash[NUM_ECC_DIGITS] = {");
        //        vli_print(buf, sizeof (buf));
        //        printf("};\n");
        //        printf("\n\n");


        /*tests to detect fail*/
        //public[10]=0xAA;
        //sigReceived[1]=0xAA;
        //buf[30]=0xAA;

        printf("===========================================================\n");
        printf("VM ID: %d\n", countNumMachines);
        printf("VM name: %s\n", VMCONF[countNumMachines].vm_name);
        printf("VM start address: 0x%x\n", lAddrVm);
        printf("VM length:        0x%x (%d)\n", sizeHash, sizeHash);
        printf("VM hash:          ");
        for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
            printf("%02x", buf[i]);
        }
        printf("\n");
        printf("VM signature:     ");
        for (i = 0; i < 64; i++) {
            printf("%02x", sigReceived[i]);
        }
        printf("\n");

        //verify signature
        printf("Verifying signature authenticity for VM %d ... ", countNumMachines);
        initialCountVerifySignature = getCounter();
        if (!uECC_verify(public, buf, sizeof (buf), sigReceived, uECC_secp256k1())) {
            printf("failed.\n\n");
            return 0;
        } else {
            printf("OK.\n\n");
        }
        //    count = hal_lr_rcount();
        finalCountTotal = getCounter();       

        /*
        printf("Count TOTAL:                  %d\n", (finalCountTotal - initialCountTotal));
        printf("Count TOTAL HASh:             %d\n", (finalCountHash-initialCountHash));
        printf("Count TOTAL Verify PK:        %d\n", (finalCounterVerifyPK-initialCountVerifyPK));
        printf("Count TOTAL Verify Signature: %d\n", (finalCountTotal-initialCountVerifySignature));
        */

        finalCountTotal=0;
        initialCountTotal=0;
        finalCountHash=0;
        initialCountHash=0;
    }

    return 1;
}