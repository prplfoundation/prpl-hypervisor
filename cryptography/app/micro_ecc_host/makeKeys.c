/*********************************************************************
 * Filename: makeKeys
 *********************************************************************/

/*****************************************************************************
 **                HEADERS
 *****************************************************************************/
#include <stdio.h>
//#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include "./include/uECC.h"
#include "./include/sha256.h"
#include <inttypes.h>
#include <stdint.h>
/*****************************************************************************
 **                DEFINES
 *****************************************************************************/

/*****************************************************************************
 **                INTERNAL FUNCTIONS
 *****************************************************************************/
void vli_print(uint8_t *p_vli, unsigned int size);
long file_lenght(FILE *file);

//typedef struct SHA256_HashContext {
//    uECC_HashContext uECC;
//    SHA256_CTX ctx;
//} SHA256_HashContext;
#define NUM_ECC_DIGITS 32

int main(int argc, char *argv[]) {
    int i = 0;
    int count = 1;
    printf("\n#####################################################\n");
    printf("#Aplicacao desenvolvida para gerar chaves publica e privada\n");
    printf("#que serão utilizadas pelo Hypervisor para autenticar VM\n");
    printf("#Project: micro-ecc\n");
    printf("#####################################################\n\n");
    //verify args
    if (argc < 2) {
        printf("\n\rMissing parameters");
        printf("\n\rUsage: %s ./path/fileNameVM", argv[0]);
        printf("\n\rUsage: %s ./tmp/fileNameVM ./tmp/fileNameVM", argv[0]);
        printf("\n\rUsage: %s ./tmp/ping.bin ./tmp/blink.bin ./path/MORE_FILES\n", argv[0]);
        return (EXIT_FAILURE);
    }

    printf("\n\rNum args: (%d)", argc);
    for (; count < argc; count++) {
        printf("\n\rProgram to read: (%s)\n", argv[count]);
        uint8_t private[NUM_ECC_DIGITS];
        uint8_t public[NUM_ECC_DIGITS * 2];
        uint8_t sig[64] = {0}; //sigature - feito com a chave privada do emissor e entregue para que tem a chave publica verificar.
        //generate private/public key
        if (!uECC_make_key(public, private, uECC_secp256k1())) {
            printf("uECC_make_key() failed\n");
            return 1;
        }
        //valida public key
        if (!uECC_valid_public_key(public, uECC_secp256k1())) {
            printf("uECC_valid_public_key() failed\n");
            return 1;
        }
        //        else {
        //            printf("Public Key OK\n");
        //        }
        //show private key
        printf("uint8_t private[NUM_ECC_DIGITS] = {");
        vli_print(private, NUM_ECC_DIGITS);
        printf("};\n");
        printf("\n\n");
        //show public key
        printf("uint8_t public[64] = {");
        vli_print(public, NUM_ECC_DIGITS * 2);
        printf("};\n");
        printf("\n\n");
        /**********************************************************************/
        FILE *binaryVM;
        long fileSize = 0;
        binaryVM = fopen(argv[count], "rb");
//        binaryVM = fopen("blink.bin", "rb");
        if (binaryVM == NULL) {
            printf("\nError openfile\n");
            return 1;
        }
        fileSize = file_lenght(binaryVM);
        printf("\n\rSize of file: %ld \n", fileSize);

        char hashBinarioPing[fileSize];
        int bytes;
        //ler todo arquivo
        fread(hashBinarioPing, 1, fileSize, binaryVM);
        /**********************************************************************/
        //create hmac
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
        //        BYTE text1[] = {"abc"};
        SHA256_CTX contextHash;
        BYTE bufFinalHash[SHA256_BLOCK_SIZE];
        //calculate hash
        sha256_init(&contextHash);
        //	sha256_update(&contextHash, text1, strlen(text1));
        sha256_update(&contextHash, hashBinarioPing, fileSize);
        sha256_final(&contextHash, bufFinalHash);
        printf("uint8_t hash[NUM_ECC_DIGITS] = {");
        vli_print(bufFinalHash, sizeof (bufFinalHash));
        printf("};\n");
        printf("\n\n");
        //sign
        //if (!uECC_sign_deterministic(private, bufFinalHash, sizeof (bufFinalHash), &ctx.uECC, sig, uECC_secp256k1())) {
        if (!uECC_sign(private, bufFinalHash, sizeof (bufFinalHash), sig, uECC_secp256k1())) {
            printf("uECC_sign() failed\n");
            return 1;
        }
        //show sign
        printf("uint8_t sig[NUM_ECC_DIGITS*2] = {");
        vli_print(sig, sizeof (sig));
        printf("};\n");
        printf("\n\n");
        //test signature
        if (!uECC_verify(public, bufFinalHash, sizeof (bufFinalHash), sig, uECC_secp256k1())) {
            printf("uECC_verify() failed\n");
            return 1;
        } else {
            printf("uECC_verify() OK\n");
        }
        //create file
        FILE *fileSecurity;
        const char header[150] = "This file is automatically generated, do not change this.\r\n";
        char strExtension[10] = ".security"; //temporary path
        printf("\n\rNew File: (%s)\n", strcat(argv[count], ".security"));
        fileSecurity = fopen(argv[count], "w+b");
        if (fileSecurity == NULL) {
            printf("\nError create file");
        }
        //save header
        fprintf(fileSecurity, "#This file is automatically generated, do not change this.\r\n");
        fprintf(fileSecurity, "#Line 1: public key.\r\n");
        fprintf(fileSecurity, "#Line 2: sign.\r\n");
        fprintf(fileSecurity, "#Line 3: private key.\r\n");
        fprintf(fileSecurity, "#Line 3: hash.\r\n");
        fprintf(fileSecurity, "###########################################\r\n");

        //public key
        fprintf(fileSecurity, "Public Key: ");
        for (i = 0; i < 64; ++i) {
            fprintf(fileSecurity, "%04u", (unsigned) public[i]);
            printf("%04u", (unsigned) public[i]);
        }
        fprintf(fileSecurity, "\r\n");
        printf("\n");
        fprintf(fileSecurity, "Signature: ");
        //sign
        for (i = 0; i < 64; ++i) {
            fprintf(fileSecurity, "%04u", (unsigned) sig[i]);
            printf("%04u", (unsigned) sig[i]);
        }
        fprintf(fileSecurity, "\r\n");
        printf("\n");
        fprintf(fileSecurity, "Private Key: ");
        //save private key
        for (i = 0; i < 32; ++i) {
            fprintf(fileSecurity, "%04u", (unsigned) private[i]);
            printf("%04u", (unsigned) private[i]);
        }
        fprintf(fileSecurity, "\r\n");
        printf("\n");
        fprintf(fileSecurity, "Hash: ");
        //hash
        for (i = 0; i < 32; ++i) {
            fprintf(fileSecurity, "%04u", bufFinalHash[i]);
            printf("%04u", (unsigned) bufFinalHash[i]);
        }
        fprintf(fileSecurity, "\r\n");
        printf("\n");
        /* close the file */
        fclose(fileSecurity);
        fclose(binaryVM);
        
        //create temp file to concatenate signatura in vm binary
        FILE *fileSecurityTemp;
        strcat(argv[count], ".tmp");
        fileSecurityTemp = fopen(argv[count], "w+b");
        if (fileSecurityTemp == NULL) {
            printf("\nError create file");
        }
        
        fwrite(&public, 1, 64, fileSecurityTemp);
        fwrite(&sig, 1, 64, fileSecurityTemp);
        fflush(fileSecurityTemp);
        fclose(fileSecurityTemp);
    }
    printf("\n\rend");
    return (0);
}

/*
 ** \brief - funcao para mostrar valores em hexadecima
 * \param
 * \return
 */
void vli_print(uint8_t *p_vli, unsigned int size) {
    unsigned i;
    for (i = 0; i < size - 1; ++i) {
        printf("0x%02X, ", (unsigned) p_vli[i]);
    }
    printf("0x%02X", (unsigned) p_vli[i]);
}

long file_lenght(FILE *file) {
    long pos, end;
    pos = ftell(file);
    fseek(file, 0, SEEK_END);
    end = ftell(file);
    fseek(file, pos, SEEK_SET);
    return end;
}
//static void foo(void){
//	return;
//	int a=0;
//}
/********************************END OF FILE***********************************/