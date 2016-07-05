F_CLK=200000000

CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

GCC_MIPS = mips-mti-elf-gcc -EL -O2 -c -Wa,-mvirt -mips32r2 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I ./include -DCPU_SPEED=${F_CLK} #$(CFLAGS_STRIP)

AS_MIPS = mips-mti-elf-as -EL -mips32r2 -mvirt  -msoft-float
LD_MIPS = mips-mti-elf-ld 
DUMP_MIPS = mips-mti-elf-objdump
READ_MIPS = mips-mti-elf-readelf
OBJ_MIPS = mips-mti-elf-objcopy
SIZE_MIPS = mips-mti-elf-size

all: clean
	$(AS_MIPS) -o build/crt0.o lib/crt0.s
	$(GCC_MIPS) -o build/libc.o lib/libc.c
	$(GCC_MIPS) -o build/platform.o lib/platform.c
	$(GCC_MIPS) -o build/network.o lib/network.c	
	$(GCC_MIPS) -o build/$(APP_NAME).o apps/$(APP_NAME).c
	$(LD_MIPS) build/crt0.o build/libc.o build/platform.o build/$(APP_NAME).o build/network.o -o build/bin/$(APP_NAME).elf -Tlib/pic32mz.ld -Llib/ -liidquiddikey -Map build/$(APP_NAME).map -N
	$(DUMP_MIPS) --disassemble --reloc build/bin/$(APP_NAME).elf > build/bin/$(APP_NAME).lst
	$(DUMP_MIPS) -h build/bin/$(APP_NAME).elf > build/bin/$(APP_NAME).sec
	$(DUMP_MIPS) -s build/bin/$(APP_NAME).elf > build/bin/$(APP_NAME).cnt
	$(OBJ_MIPS) -O binary build/bin/$(APP_NAME).elf build/bin/$(APP_NAME).bin
	$(SIZE_MIPS) build/bin/$(APP_NAME).elf

clean:
	-rm -rf *~ *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *.srec *.hex *.elf
	-rm -rf app/*~
	-rm -rf include/*~
	-rm -rf lib/*~
	-rm -rf build/*~ build/*.o build/*.axf build/*.map build/*.lst build/*.sec build/*.cnt build/*.txt build/*.bin build/*.srec build/*.hex build/*.elf

