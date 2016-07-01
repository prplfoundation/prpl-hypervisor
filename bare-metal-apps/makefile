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

all:
	$(AS_MIPS) -o build/crt0.o lib/crt0.s
	$(GCC_MIPS) -o build/libc.o lib/libc.c
	$(GCC_MIPS) -o build/platform.o lib/platform.c
	$(GCC_MIPS) -o build/$(APP_NAME).o apps/$(APP_NAME).c
	$(GCC_MIPS) -o build/network.o lib/network.c
	$(LD_MIPS) -Tlib/pic32mz.ld -Map build/$(APP_NAME).map -N -o build/$(APP_NAME).elf \
		build/crt0.o build/network.o build/libc.o build/platform.o build/$(APP_NAME).o
	$(DUMP_MIPS) --disassemble --reloc build/$(APP_NAME).elf > build/$(APP_NAME).lst
	$(DUMP_MIPS) -h build/$(APP_NAME).elf > build/$(APP_NAME).sec
	$(DUMP_MIPS) -s build/$(APP_NAME).elf > build/$(APP_NAME).cnt
	$(OBJ_MIPS) -O binary build/$(APP_NAME).elf build/$(APP_NAME).bin
	$(SIZE_MIPS) build/$(APP_NAME).elf

clean:
	-rm -rf *~ *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *.srec *.hex *.elf
	-rm -rf app/*~
	-rm -rf include/*~
	-rm -rf lib/*~
	-rm -rf build/*~ build/*.o build/*.axf build/*.map build/*.lst build/*.sec build/*.cnt build/*.txt build/*.bin build/*.srec build/*.hex build/*.elf

