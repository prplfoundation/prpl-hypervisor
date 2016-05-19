#
#Copyright (c) 2016, prpl Foundation
#
#Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
#fee is hereby granted, provided that the above copyright notice and this permission notice appear 
#in all copies.
#
#THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
#INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
#FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
#LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
#ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.




CC=mips-mti-elf-gcc
LD=mips-mti-elf-ld
OD=mips-mti-elf-objdump
OC=mips-mti-elf-objcopy
STRIP=mips-mti-elf-strip
BASE=./
HAL=$(BASE)hal


CFLAGS= -O2 -EL -mips32r2 -Wa,-mvirt -G 0 -fno-builtin -c -I $(COMMON) -I $(SEAD3) -I include/ -I $(HAL)/include/
LDFLAGS_RAM=-Ttext 0x80100000 -e main -EL -nostdlib  -Wl,-Map=prplHypervisor.map

ASOURCES= \
$(HAL)/rw_regs.S


ASOURCES_RAM=$(ASOURCES) 

CSOURCES= \
$(HAL)/hal.c \
$(HAL)/tlb.c \
$(HAL)/uart.c \
$(HAL)/vcpu.c \
vm.c \
linkedlist.c \
kernel.c \
libc.c \
malloc.c \
scheduler.c \
dispatcher.c \
hypercall.c

COBJECTS=$(CSOURCES:.c=.o)
AOBJECTS=$(ASOURCES:.S=.o)

AOBJECTS_SP=$(ASOURCES_SP:.S=.o)
AOBJECTS_RAM=$(ASOURCES_RAM:.S=.o)

SIM_RAM : $(COBJECTS) $(AOBJECTS_RAM)
	$(CC)  $(LDFLAGS_RAM) $(COBJECTS) $(AOBJECTS_RAM) -o prplHypervisor.elf
	$(OD) -d -S -l prplHypervisor.elf > prplHypervisor.dasm
	$(OC) -O srec prplHypervisor.elf prplHypervisor.srec
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
.S.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f prplHypervisor.elf
	rm -f prplHypervisor.srec
	rm -f prplHypervisor.map
	rm -f prplHypervisor.dasm
	rm -f *.o
	rm -f $(HAL)/*.o
		
