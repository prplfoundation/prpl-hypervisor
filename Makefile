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


# Setup PIC32MZEF core configuration to be used with pic32mzef_boot__v0_1.hex
ABI=32
ENDIAN=EL
MEMORY_SIZE=512K	# Memory available on the ChipKit Wifire PIC32 Board
ELF_ENTRY=0xbfc00000	# Where PC will be set when ELF is loaded- boot vector
APP_START=0x80001000	# Where program code will be located - boot ROM will
			# jump here once it has run

include ${MIPS_ELF_ROOT}/share/mips/rules/mipshal.mk

BASE=./
HAL=$(BASE)hal
MICROCHIP=$(HAL)/microchip

#CFLAGS += -g -O1
CFLAGS=-O0 -g -EL -mips32r2 -Wa,-mvirt  -G 0 -fno-builtin -c -I $(COMMON) -I $(SEAD3) -I include/ -I $(HAL)/include/ -I$(COMMON) 
LDFLAGS += -g

OBJS = kernel.o \
dispatcher.o \
hypercall.o \
libc.o \
linkedlist.o \
malloc.o \
vm.o \
scheduler.o \
$(HAL)/hal.o \
$(HAL)/tlb.o \
$(HAL)/vcpu.o \
$(MICROCHIP)/boot.o \
$(MICROCHIP)/uart.o \
$(MICROCHIP)/chipset.o 

APP = prplHypervisor.elf
BIN = prplHypervisor


all: $(APP)

$(APP): $(OBJS)
	$(LD) $(LDFLAGS)  $^ -o $@
	$(OBJDUMP) -dr $(APP) > $(BIN).dasm

%.o: %.c 
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@
	
.PHONY: clean
clean:
	rm -f $(APP) $(OBJS)
	rm -f $(BIN).dasm
