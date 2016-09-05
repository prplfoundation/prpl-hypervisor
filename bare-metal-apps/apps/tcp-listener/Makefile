APP=tcp-listener

# Define your additional include paths
INC_DIRS = -I ../../../../picotcp/build/include/

#Aditional C flags
CFLAGS = -DPICOTCP -mhard-float
#wolfssl flags
CFLAGS+= -DWOLFSSL_PICOTCP -DFTM_TIMING_RESISTANT -DNO_WRITEV -DNO_ERROR_STRINGS -DSMALL_SESSION_CACHE -DWOLFSSL_SMALL_STACK -DHAVE_TLS_EXTENSIONS -DNO_DSA -DNO_RABBIT -DNO_MD4 -DNO_DES3 -DNO_FILESYSTEM -DUSE_FAST_MATH -DRSA_DECODE_EXTRA -DHAVE_ECC -DHAVE_ALL_CURVES -DHAVE_SUPPORTED_CURVES -DSINGLE_THREADED -DWOLFSSL_USER_IO

#Aditional Libraries
LIBS=../../../../picotcp/build/lib/libpicotcp.a \
	 ../../../../picotcp/build/lib/libwolfssl.a \
	 ~/.local/opt/imgtec/Toolchains/mips-mti-elf/2015.06-05/mips-mti-elf/lib/el/libm.a \
	 ~/.local/opt/imgtec/Toolchains/mips-mti-elf/2015.06-05/mips-mti-elf/lib/el/libc.a 

	 
# Add your targets before the $(APP) target. Your targets should be defined at the .mk files. 
# See app.mk as an example.
all: conf_liker lib hal app $(APP)

#include your additional mk files. 
include app.mk

#default stack size 512 bytes
STACK_SIZE = 4096

######################################################################################
# SHOULD NOT BE NECESSARY TO MODIFY THINGS FROM THIS POINT.                          #       
######################################################################################

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
           -I$(TOPDIR)arch/mips/pic32mz_starter_kit/include/

include $(TOPDIR)lib/lib.mk
include $(TOPDIR)arch/mips/pic32mz_starter_kit/hal.mk

           
#Configure CROSS COMPILER
AS = $(CROSS_COMPILER)as 
LD = $(CROSS_COMPILER)ld 
OBJDUMP = $(CROSS_COMPILER)objdump
READELF = $(CROSS_COMPILER)readelf
OBJCOPY = $(CROSS_COMPILER)objcopy
SIZE = $(CROSS_COMPILER)size
CC= $(CROSS_COMPILER)gcc

LINKER_SCRIPT = pic32mz.ld

### GCC flags configuration: processor tunning ###
CFLAGS += -EL -O2 -mtune=m14k -mips32r2
#MIPZ VZ support
CFLAGS += -Wa,-mvirt
#flointing pointing options
CFLAGS +=  -mno-check-zero-division -fshort-double
#General options
CFLAGS += -c -ffreestanding -nostdlib -fomit-frame-pointer -G 0
#Additional features flags
CFLAGS += $(ETHERNET) -DCPU_SPEED=$(F_CLK)
# AS flags
ASFLAGS += -EL -mvirt -mips32r2
# LD flags
LDFLAGS += -EL

$(APP): 
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -Map $(APP).map -o $(APP).elf *.o -N $(LIBS)
	$(OBJDUMP) --disassemble --reloc $(APP).elf > $(APP).lst
	$(OBJDUMP) -h $(APP).elf > $(APP).sec
	$(OBJDUMP) -s $(APP).elf > $(APP).cnt
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) $(APP).elf

conf_liker:
	cp $(TOPDIR)arch/mips/pic32mz_starter_kit/pic32mz.ld .
	sed -i -e 's/FLASH_SIZE/$(FLASH_SIZE)/' pic32mz.ld
	sed -i -e 's/RAM_SIZE/$(RAM_SIZE)/' pic32mz.ld
	sed -i -e 's/STACK_SIZE/$(STACK_SIZE)/' pic32mz.ld
	
	
	
clean: 
	rm -f *.o *.sec *.lst *.elf *.sec *.cnt *.bin *.map *~ *.ld

