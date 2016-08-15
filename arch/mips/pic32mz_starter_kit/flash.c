/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

*/

#include <config.h>
#include <types.h>
#include "pic32mz.h"

/* Uses the last page 16Kb of the flash to data persistance */
uint32_t* pageFlash = (uint32_t *)0xbd1fc000;

/*
 * Program Flash write access routines
 */
uint32_t NVMUnlock(uint32_t nvmop){
        uint32_t status;

        // Suspend or Disable all Interrupts
        asm volatile ("di %0" : "=r" (status));
        // Enable Flash Write/Erase Operations and Select
        // Flash operation to perform
        NVMCON = nvmop;
        udelay(10);
        // Write Keys
        NVMKEY = 0x0;
        NVMKEY = 0xAA996655;
        NVMKEY = 0x556699AA;
        // Start the operation using the Set Register
        NVMCONSET = 0x8000;
        // Wait for operation to complete
        while (NVMCON & 0x8000);
        // Restore Interrupts
        if (status & 0x00000001)
                asm volatile ("ei");
        else
                asm volatile ("di");
        // Disable NVM write enable
        NVMCONCLR = 0x0004000;
        // Return WRERR and LVDERR Error Status Bits 
        return (NVMCON & 0x3000);
}

uint32_t NVMErasePage(void *address){
        uint32_t res;

        // Set NVMADDR to the Start Address of page to erase
        NVMADDR = ((uint32_t)address) & 0x1fffffff;
        // Unlock and Erase Page
        res = NVMUnlock(0x4004);

        return res;
}

uint32_t NVMWriteQuad(void *address, void *data){
        uint32_t res;

        // Set NVMADDR to Start Address of row to program
        NVMADDR = ((uint32_t)address) & 0x1fffffff;

        NVMDATA0 = (*((uint32_t *)data));
        // value written to
        NVMDATA1 = (*((uint32_t *)data+1));
        // value written to
        NVMDATA2 = (*((uint32_t *)data+2));
        // value written to
        NVMDATA3 = (*((uint32_t *)data+3));
        
        // Unlock and Write Quad
        res = NVMUnlock(0x4002);

        return res;
}

uint8_t * flash_read1Kbuffer(uint8_t *buffer){
    uint32_t i;
    for(i=0; i<256; i++){
        ((uint32_t *)buffer)[i] = pageFlash[i];
    }
    return buffer;
}

uint8_t * flash_write1Kbuffer(uint8_t *buffer){
    uint32_t i;
    NVMErasePage(pageFlash);
    for(i=0; i<256; i+=4){
        NVMWriteQuad((pageFlash+i), ((uint32_t*)buffer)+i);    
    }
}

