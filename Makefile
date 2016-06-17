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
BAUDRATE=115200
F_CLK=200000000
SERIAL_DEV=/dev/ttyACM0


BASE=./
HAL=$(BASE)hal
MICROCHIP=$(HAL)/microchip

CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections 
LDFLAGS = -Thal/microchip/pic32mz.ld 
#CFLAGS = -EL -O2 -c -mtune=m14k  -mips32r2 -Wa,-mvirt -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I include/ -I $(HAL)/include/ -DCPU_SPEED=${F_CLK} #$(CFLAGS_STRIP)
CFLAGS = -EL -O2 -c -mips32r2 -mtune=m14k -Wa,-mvirt -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I include/ -I $(HAL)/include/ -DCPU_SPEED=${F_CLK}
         

AS_MIPS = mips-mti-elf-as -EL
LD_MIPS = mips-mti-elf-ld #$(LDFLAGS_STRIP)
DUMP_MIPS = mips-mti-elf-objdump
READ_MIPS = mips-mti-elf-readelf
OBJ_MIPS = mips-mti-elf-objcopy
SIZE_MIPS = mips-mti-elf-size
GCC_MIPS= mips-mti-elf-gcc


OBJS = $(MICROCHIP)/boot.o \
kernel.o \
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
$(HAL)/common.o \
$(MICROCHIP)/uart.o \
$(MICROCHIP)/chipset.o 

APP = prplHypervisor.elf
BIN = prplHypervisor


all: $(APP)

$(APP): $(OBJS)
	$(LD_MIPS) $(LDFLAGS)  $^ -o $@
	$(DUMP_MIPS) --disassemble --reloc $(APP) > $(BIN).lst
	$(DUMP_MIPS) -h $(APP) > $(BIN).sec
	$(DUMP_MIPS) -s $(APP) > $(BIN).cnt
	$(OBJ_MIPS) -O binary $(APP) $(BIN).bin
	$(OBJ_MIPS) -O ihex --change-addresses=0x80000000 $(APP) $(BIN).hex
	$(SIZE_MIPS) $(APP)

%.o: %.c 
	$(GCC_MIPS) $(CFLAGS) $(INCLUDES) -c $^ -o $@

%.o: %.S
	$(GCC_MIPS) $(CFLAGS) $(INCLUDES) -c $^ -o $@
	
serial:
	stty ${BAUDRATE} raw cs8 -hupcl -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEV}

load: serial
	./pic32prog -d ${SERIAL_DEV} $(BIN).hex

debug: serial
	cat ${SERIAL_DEV}

clean:
	rm -f $(APP) $(OBJS)
	rm -f $(BIN).lst
	rm -f $(BIN).sec
	rm -f $(BIN).cnt
	rm -f $(BIN).bin
	rm -f $(BIN).hex
