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
#This code was written by Sergio Johann at Embedded System Group (GSE) at PUCRS/Brazil.

# this is stuff specific to this architecture
ARCH_DIR = $(SRC_DIR)/arch/$(ARCH)
INC_DIRS  = -I $(ARCH_DIR)/include

F_CLK=200000000
TIME_SLICE=0

#remove unreferenced functions
CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

# this is stuff used everywhere - compiler and flags should be declared (ASFLAGS, CFLAGS, LDFLAGS, LINKER_SCRIPT, CC, AS, LD, DUMP, READ, OBJ and SIZE).
# remember the kernel, as well as the application, will be compiled using the *same* compiler and flags!
ASFLAGS = -EL -mips32r2 -mvirt 
CFLAGS = -Wall -EL -O2 -c -mips32r2 -mno-check-zero-division -ffreestanding -nostdlib -fomit-frame-pointer -G 0 $(INC_DIRS) -DCPU_SPEED=${F_CLK} -DTIME_SLICE=${TIME_SLICE} -DLITTLE_ENDIAN $(CFLAGS_STRIP) -DKERN_VER=\"$(KERNEL_VER)\"
LDFLAGS = #$(LDFLAGS_STRIP)
LINKER_SCRIPT =pic32mz.ld

CC = mips-mti-elf-gcc
AS = mips-mti-elf-as
LD = mips-mti-elf-ld 
DUMP = mips-mti-elf-objdump
READ = mips-mti-elf-readelf
OBJ = mips-mti-elf-objcopy
SIZE = mips-mti-elf-size

hal:
	$(AS) $(ASFLAGS) -o crt0.o $(ARCH_DIR)/boot/crt0.s
	$(CC) $(CFLAGS) \
		$(ARCH_DIR)/drivers/interrupt.c \
		$(ARCH_DIR)/drivers/hal.c 
