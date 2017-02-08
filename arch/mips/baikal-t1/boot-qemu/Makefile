#Copyright (c) 2016, prpl Foundation

#Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
#fee is hereby granted, provided that the above copyright notice and this permission notice appear 
#in all copies.

#THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
#INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
#FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
#LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
#ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

# Based on the Baikal Electronics bootloader for P5600 core. 


# BAIKAL-T P5600 CORE
BOOT_BASE	:= 0x1FC00000
SRAM_BASE	:= 0x1A400000
UART_BASE	:= 0x1F04A000
GIC_BASE	:= 0x1BDC0000
CPC_BASE	:= 0x1BDE0000
GCR_BASE	:= 0x1FBF8000

PHYS_BASE	:= 0x00000000
KSEG0_BASE	:= 0x80000000
KSEG1_BASE	:= 0xA0000000

CNTFRQ		:= 24000000

INITRD_FLAGS	:= -DUSE_INITRD
CPPFLAGS	+= $(INITRD_FLAGS)
CPPFLAGS	+= -EL -mlong-calls -mxgot -Wall -O2 -mno-mips16 -mabi=32 -mno-micromips -mips32r2
CPPFLAGS	+= -D_ASSEMBLER_

BOOTLOADER	:= boot.S
KERNEL		:= prplHypervisor.bin
KERNEL_OFFSET	:= 0x00000000
KERNEL_ENTRY	:= 0x00001000
LD_SCRIPT	:= model.lds.S
IMAGE		:= prplHypervisor.elf


DTB		:= baikal.dtb
DTB_OFFSET	:= 0x00F00000

CC		:= $(CROSS_COMPILER)gcc
LD		:= $(CROSS_COMPILER)ld

all: $(IMAGE)
	mv prplHypervisor.elf ../../../../platform/baikal_t1_board

clean:
	rm -f $(IMAGE) boot.o uart.o model.lds

$(IMAGE): boot.o model.lds $(KERNEL) $(FILESYSTEM)
	$(LD) -o $@ --script=model.lds

boot.o: $(BOOTLOADER) Makefile
	$(CC) $(CPPFLAGS) \
		-DKERNEL_ENTRY=$(KERNEL_ENTRY) \
		-DDTB_OFFSET=$(DTB_OFFSET) \
		-DSRAM_BASE=$(SRAM_BASE) \
		-DUART_BASE=$(UART_BASE) \
		-DGIC_BASE=$(GIC_BASE) \
		-DGCR_BASE=$(GCR_BASE) \
		-DCPC_BASE=$(CPC_BASE) \
		-c -o $@ $(BOOTLOADER)

model.lds: $(LD_SCRIPT) Makefile 
	$(CC) $(CPPFLAGS) \
		-DBOOT_BASE=$(BOOT_BASE) \
		-DKERNEL_OFFSET=$(KERNEL_OFFSET) \
		-DDTB_OFFSET=$(DTB_OFFSET) \
		-DFS_OFFSET=$(FS_OFFSET) \
		-DKERNEL=$(KERNEL) \
		-E -P -C -o $@ $<

	

# The filesystem archive might not exist if INITRD is not being used
.PHONY: all clean $(FILESYSTEM)
