F_CLK=200000000

CFLAGS_STRIP = -fdata-sections -ffunction-sections
LDFLAGS_STRIP = --gc-sections

GCC_MIPS = mips-mti-elf-gcc -EL -O2 -c -mips32r2 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -I ./include -DCPU_SPEED=${F_CLK} #$(CFLAGS_STRIP)

AS_MIPS = mips-mti-elf-as -EL -mips32r2 -mvirt  -msoft-float
LD_MIPS = mips-mti-elf-ld 
DUMP_MIPS = mips-mti-elf-objdump
READ_MIPS = mips-mti-elf-readelf
OBJ_MIPS = mips-mti-elf-objcopy
SIZE_MIPS = mips-mti-elf-size

all:
	$(AS_MIPS) -o crt0.o lib/crt0.s
	$(GCC_MIPS) -o libc.o lib/libc.c
	$(GCC_MIPS) -o apps/$(APP_NAME).o apps/$(APP_NAME).c
	$(LD_MIPS) -Tlib/pic32mz.ld -Map apps/$(APP_NAME).map -N -o apps/$(APP_NAME).elf \
		crt0.o libc.o apps/$(APP_NAME).o
	$(DUMP_MIPS) --disassemble --reloc apps/$(APP_NAME).elf > apps/$(APP_NAME).lst
	$(DUMP_MIPS) -h apps/$(APP_NAME).elf > apps/$(APP_NAME).sec
	$(DUMP_MIPS) -s apps/$(APP_NAME).elf > apps/$(APP_NAME).cnt
	$(OBJ_MIPS) -O binary apps/$(APP_NAME).elf apps/$(APP_NAME).bin
	$(SIZE_MIPS) apps/$(APP_NAME).elf

clean:
	-rm -rf *~ *.o *.axf *.map *.lst *.sec *.cnt *.txt *.bin *.srec *.hex *.elf
	-rm -rf app/*~
	-rm -rf include/*~
	-rm -rf lib/*~
	-rm -rf apps/*~ apps/*.o apps/*.axf apps/*.map apps/*.lst apps/*.sec apps/*.cnt apps/*.txt apps/*.bin apps/*.srec apps/*.hex apps/*.elf

