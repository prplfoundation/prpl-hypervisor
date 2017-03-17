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
#CFG_FILE = cfg/sample-blink.cfg
CFG_FILE = cfg/sample-blink.cfg

# CROSS Compiler 
CROSS_COMPILER = mipsel-unknown-linux-gnu-

# Optional Device Drivers
CONFIG_INTERVMCOMM_DRV = yes

# The following drivers is only for tests/performance purpose. 
#CONFIG_TIMER_TEST_DRV = yes
#CONFIG_PERFORMANCE_COUNTER_DRV = yes
#CONFIG_INTERRUPT_LATENCY_TEST_DRV = yes

# Platform info
CFLAGS = -DCPU_ID="P5600"
CFLAGS += -DCPU_ARCH="Baikal-T1 board."
CFLAGS += -DCPU_FREQ=850000000

# Define DEBUG_GUEST to yes if you desire to compile the guests for debug. The code optimization will be O0.
DEBUG_GUEST = no


########################################################################################################
# SHOULD NOT BE NEEDED TO MODIFY ANYTHING FROM WHERE. UNLESS YOU ARE ADDING NEW HYPERVISOR SOURCE FILES
# OR PORTING TO A NEW PLATFORM.
########################################################################################################
APP_LIST = ""

SHELL := /bin/bash

# Setup Baikal-T1 core configuration used with u-boot.
BAUDRATE=38600
F_CLK=850000000
SERIAL_DEV=/dev/ttyACM0

TOPDIR=../../

PLATFORM_DIR=platform/baikal_t1_board/

VERSION:=$(shell $(TOPDIR)/scripts/genversion.sh)

LINKER_SCRIPT = ../../arch/mips/baikal-t1/baikal.ld

### GCC flags configuration: processor tunning ###
CFLAGS += -EL -O2 -mips32r5 -Wall
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
INC_DIRS = -I$(TOPDIR)arch/mips/baikal-t1/include \
           -I$(TOPDIR)arch/mips/common/include \
           -I$(TOPDIR)/platform/include \
           -I$(TOPDIR)sys/lib/include \
           -I$(TOPDIR)sys/kernel/include \
           -I$(TOPDIR)platform/baikal_t1_board/include \
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

all: config_vms lib kernel common baikal platform drivers $(APP) bare_apps generate_firmware

include $(TOPDIR)sys/sys.mk
include $(TOPDIR)arch/mips/common/common.mk
include $(TOPDIR)arch/mips/baikal-t1/baikal.mk
include board/board.mk
include $(TOPDIR)/drivers/drivers.mk

$(APP): 
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -Map $(APP).map -N -o $(APP).elf *.o $(OBJ)
	$(OBJDUMP) -Dz $(APP).elf > $(APP).lst
	$(OBJDUMP) -h $(APP).elf > $(APP).sec
	$(OBJDUMP) -s $(APP).elf > $(APP).cnt
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) $(APP).elf

bare_apps:
	for i in $(APP_LIST) ; do \
		$(MAKE) -C ../../bare-metal-apps/platform/baikal_t1_board APP=$$i CLEAN=1 clean; \
		$(MAKE) -C ../../bare-metal-apps/platform/baikal_t1_board APP=$$i CROSS_COMPILER=$(CROSS_COMPILER) F_CLK=$(F_CLK) FLASH_SIZE=`awk  '$$1=="'$$i'" {print $$2}' include/vms.info` RAM_SIZE=`awk '$$1=="'$$i'" {print $$3}' include/vms.info` DEBUG_GUEST=$(DEBUG_GUEST) \
	;done

generate_firmware:
	$(shell export BASE_ADDR=0x80000000; ../../scripts/genhex.sh $(APP_LIST))

make_genconf:
	gcc -o genconf ../cfg_reader/genconf-baikal.c -lconfig
	
config_vms: make_genconf
	#execute first and exit in case of errors
	./genconf $(CFG_FILE) baikal-t1 || (exit 1)
	#execute and export to a makefile variable the ouput of the script
	$(eval APP_LIST:=$(shell ./genconf $(CFG_FILE) baikal-t1 || (exit 1)))

	
clean: config_vms
	rm -f $(APP).* 
	rm -f *.o
	rm -f firmware.hex
	rm -f include/vms.info
	rm -f include/config.h
	rm -f genconf
	rm -f firmware.bin
	$(MAKE) -C ../../arch/mips/baikal-t1/boot-qemu clean
	
	for i in $(APP_LIST) ; do \
		$(MAKE) -C ../../bare-metal-apps/platform/baikal_t1_board APP=$$i CLEAN=1 clean \
	;done

