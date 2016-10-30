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

#Input CFG file for VMs configuration
CFG_FILE = ../cfg/sample-ping-pong.cfg

# CROSS Compiler 
CROSS_COMPILER = mips-mti-elf-

# Optional Device Drivers
CONFIG_INTERVMCOMM_DRV = yes
CONFIG_PIC32MZ_USB_DRV = yes
CONFIG_PIC32MZ_ETHERNET_DRV = yes
CONFIG_VIRTUAL_IO_DRV = yes
CONFIG_INTERRUPT_REDIRECT_DRV = yes

# Platform info
CFLAGS = -DCPU_ID="M5150"
CFLAGS += -DCPU_ARCH="Microchip Starter Kit"
CFLAGS += -DCPU_FREQ=200000000


########################################################################################################
# SHOULD NOT BE NEEDED TO MODIFY ANYTHING FROM WHERE. UNLESS YOU ARE ADDING NEW HYPERVISOR SOURCE FILES
# OR PORTING TO A NEW PLATFORM.
########################################################################################################
APP_LIST = ""

SHELL := /bin/bash

# Setup PIC32MZEF core configuration to be used with Microchip_UART.hex bootloader
BAUDRATE=115200
F_CLK=200000000
SERIAL_DEV=/dev/ttyACM0

TOPDIR=../../

PLATFORM_DIR=platform/pic32mz_starter_kit/

VERSION:=$(shell $(TOPDIR)/scripts/genversion.sh)

LINKER_SCRIPT = ../../arch/mips/pic32mz/pic32mz.ld

### GCC flags configuration: processor tunning ###
CFLAGS += -EL -O2 -mips32r5 -mmicromips -Wall
#MIPZ VZ support
CFLAGS += -Wa,-mvirt 
#flointing pointing options
CFLAGS +=  -mno-check-zero-division -msoft-float -fshort-double
#General options
CFLAGS += -c -ffreestanding -nostdlib -fomit-frame-pointer -G 0
#Additional features flags
CFLAGS += -DCPU_SPEED=$(F_CLK) -DHYPVERSION=${VERSION}
# LD flags
LDFLAGS = -EL  

### Include dirs ###
INC_DIRS = -I$(TOPDIR)arch/mips/pic32mz/include \
           -I$(TOPDIR)arch/mips/common/include \
           -I$(TOPDIR)/platform/include \
           -I$(TOPDIR)sys/lib/include \
           -I$(TOPDIR)sys/kernel/include \
           -I$(TOPDIR)platform/pic32mz_starter_kit/include \
           -I$(TOPDIR)/include

BUILDDIR=build/hypervisor
 
#Configure CROSS COMPILER
AS = $(CROSS_COMPILER)as 
LD = $(CROSS_COMPILER)ld 
OBJDUMP = $(CROSS_COMPILER)objdump
READELF = $(CROSS_COMPILER)readelf
OBJCOPY = $(CROSS_COMPILER)objcopy
SIZE = $(CROSS_COMPILER)size
CC= $(CROSS_COMPILER)gcc
STRIP = $(CROSS_COMPILER)strip

APP = prplHypervisor

all: config_vms lib kernel common pic32mz platform drivers $(APP) bare_apps generate_firmware

include $(TOPDIR)sys/sys.mk
include $(TOPDIR)arch/mips/common/common.mk
include $(TOPDIR)arch/mips/pic32mz/pic32mz.mk
include board/board.mk
include $(TOPDIR)/drivers/drivers.mk

ifeq ($(CONFIG_PIC32MZ_ETHERNET_DRV),yes)
    OBJ = ../../bin/pic32mz-ethernet.o
endif
    

$(APP): 
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -Map $(APP).map -N -o $(APP).elf *.o $(OBJ)
	$(STRIP) $(APP).elf
	$(OBJDUMP) --disassemble --reloc $(APP).elf > $(APP).lst
	$(OBJDUMP) -h $(APP).elf > $(APP).sec
	$(OBJDUMP) -s $(APP).elf > $(APP).cnt
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(OBJCOPY) -O ihex --change-addresses=0x80000000 $(APP).elf $(BIN).hex
	$(SIZE) $(APP).elf

serial:
	stty ${BAUDRATE} raw cs8 -hupcl -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEV}

load: serial
	../../bin/pic32prog -S -d ${SERIAL_DEV} firmware.hex

bare_apps:
	for i in $(APP_LIST) ; do \
		$(MAKE) -C ../../bare-metal-apps/platform/pic32mz_starter_kit APP=$$i CLEAN=1 clean; \
		$(MAKE) -C ../../bare-metal-apps/platform/pic32mz_starter_kit APP=$$i CROSS_COMPILER=$(CROSS_COMPILER) F_CLK=$(F_CLK) FLASH_SIZE=`awk  '$$1=="'$$i'" {print $$2}' include/vms.info` RAM_SIZE=`awk '$$1=="'$$i'" {print $$3}' include/vms.info` \
	;done

generate_firmware:
	../../scripts/genhex.sh $(APP_LIST)

make_genconf:
	gcc -o genconf ../cfg_reader/genconf.c -lconfig
	
config_vms: make_genconf
	#execute first and exit in case of errors
	./genconf $(CFG_FILE) || (exit 1)
	#execute and export to a makefile variable the ouput of the script
	$(eval APP_LIST:=$(shell ./genconf $(CFG_FILE) || (exit 1)))

	
clean: config_vms
	rm -f $(APP).* 
	rm -f *.o
	rm -f firmware.hex
	rm -rf include
	rm -f genconf
	
	for i in $(APP_LIST) ; do \
		$(MAKE) -C ../../bare-metal-apps/platform/pic32mz_starter_kit APP=$$i CLEAN=1 clean \
	;done

