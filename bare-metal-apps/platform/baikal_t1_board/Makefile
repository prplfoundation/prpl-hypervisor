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


all: conf_liker app lib hal platform $(APP)

ifndef CLEAN
    ifndef CROSS_COMPILER
        $(error  CROSS_COMPILER not defined)
    endif

    ifndef F_CLK
        $(error  F_CLK not defined)
    endif

    ifndef FLASH_SIZE
        $(error  FLASH_SIZE not defined.)
    endif

    ifndef RAM_SIZE
        $(error  RAM_SIZE not defined.)
    endif
endif

TOPDIR=../../

INC_DIRS += -I$(TOPDIR)/lib/include \
            -I$(TOPDIR)../include \
            -Iinclude \
           -I$(TOPDIR)arch/mips/baikal-t1/include/

include $(TOPDIR)lib/lib.mk
include $(TOPDIR)arch/mips/baikal-t1/hal.mk
include $(TOPDIR)apps/$(APP)/app.mk
include $(TOPDIR)platform/baikal_t1_board/platform.mk
           
#Configure CROSS COMPILER
AS = $(CROSS_COMPILER)as 
LD = $(CROSS_COMPILER)ld 
OBJDUMP = $(CROSS_COMPILER)objdump
READELF = $(CROSS_COMPILER)readelf
OBJCOPY = $(CROSS_COMPILER)objcopy
SIZE = $(CROSS_COMPILER)size
CC= $(CROSS_COMPILER)gcc
STRIP = $(CROSS_COMPILER)strip

LINKER_SCRIPT = ../../apps/$(APP)/baikal.ld

ifeq ($(DEBUG_GUEST), yes)
	CFLAGS += -O0 -g
else
	CFLAGS += -O2
endif

### GCC flags configuration: processor tunning ###
CFLAGS += -EL -mips32r2 -Wall
#MIPZ VZ support
CFLAGS += -Wa,-mvirt
#flointing pointing options
CFLAGS +=  -mno-check-zero-division -msoft-float -fshort-double
#General options
CFLAGS += -c -ffreestanding -nostdlib -fomit-frame-pointer -G 0
#Reserve registers t8 and t9 for hypervisor purposes
#CFLAGS += -ffixed-t8 -ffixed-t9
#Additional features flags
CFLAGS += $(ETHERNET) -DCPU_SPEED=$(F_CLK) -DLITTLE_ENDIAN
# AS flags
ASFLAGS += -EL -mvirt -mips32r5
# LD flags
LDFLAGS += -EL

$(APP): 
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -Map ../../apps/$(APP)/$(APP).map -o ../../apps/$(APP)/$(APP).elf ../../apps/$(APP)/*.o -N $(LIBS)
#	$(STRIP) ../../apps/$(APP)/$(APP).elf
	$(OBJDUMP) -Dz ../../apps/$(APP)/$(APP).elf > ../../apps/$(APP)/$(APP).lst
	$(OBJDUMP) -h ../../apps/$(APP)/$(APP).elf > ../../apps/$(APP)/$(APP).sec
	$(OBJDUMP) -s ../../apps/$(APP)/$(APP).elf > ../../apps/$(APP)/$(APP).cnt
	$(OBJCOPY) -O binary ../../apps/$(APP)/$(APP).elf ../../apps/$(APP)/$(APP).bin
	$(SIZE) ../../apps/$(APP)/$(APP).elf

conf_liker:
	cp $(TOPDIR)arch/mips/baikal-t1/baikal.ld ../../apps/$(APP)/
	sed -i -e 's/RAM_SIZE/$(RAM_SIZE)/' ../../apps/$(APP)/baikal.ld
	sed -i -e 's/STACK_SIZE/$(STACK_SIZE)/' ../../apps/$(APP)/baikal.ld
	
	
	
clean: 
	rm -f ../../apps/$(APP)/*.o ../../apps/$(APP)/*.sec ../../apps/$(APP)/*.lst ../../apps/$(APP)/*.elf 
	rm -f ../../apps/$(APP)/*.cnt ../../apps/$(APP)/*.bin ../../apps/$(APP)/*.map ../../apps/$(APP)/*~ ../../apps/$(APP)/*.ld

