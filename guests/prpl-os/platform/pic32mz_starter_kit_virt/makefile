APP = app/$(APPLICATION)
ARCH = mips/pic32mz_starter-kit-virt

SERIAL_BAUD=115200
SERIAL_DEVICE=/dev/ttyACM0

CPU_ARCH = \"$(ARCH)\"
MAX_TASKS = 30
HEAP_SIZE = 32768

SRC_DIR = $(CURDIR)/../..

include $(SRC_DIR)/arch/$(ARCH)/arch.mak
include $(SRC_DIR)/lib/lib.mak
include $(SRC_DIR)/sys/kernel.mak
include $(SRC_DIR)/$(APP)/app.mak

INC_DIRS += -I $(SRC_DIR)/lib/include -I $(SRC_DIR)/sys/include
CFLAGS += -DCPU_ID=0 -DCPU_CORES=1 -DCPU_ARCH=$(CPU_ARCH) -DMAX_TASKS=$(MAX_TASKS) -DHEAP_SIZE=$(HEAP_SIZE) -DTERM_BAUD=$(SERIAL_BAUD)

image: hal libc kernel app
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -o $(SRC_DIR)/$(APP)/$(APPLICATION).elf *.o
	$(DUMP) --disassemble --reloc $(SRC_DIR)/$(APP)/$(APPLICATION).elf > $(SRC_DIR)/$(APP)/$(APPLICATION).lst
	$(DUMP) -h $(SRC_DIR)/$(APP)/$(APPLICATION).elf > $(SRC_DIR)/$(APP)/$(APPLICATION).sec
	$(DUMP) -s $(SRC_DIR)/$(APP)/$(APPLICATION).elf > $(SRC_DIR)/$(APP)/$(APPLICATION).cnt
	$(OBJ) -O binary $(SRC_DIR)/$(APP)/$(APPLICATION).elf $(SRC_DIR)/$(APP)/$(APPLICATION).bin
	$(OBJ) -O ihex --change-addresses=0x80000000 $(SRC_DIR)/$(APP)/$(APPLICATION).elf $(SRC_DIR)/$(APP)/$(APPLICATION).hex
	$(SIZE) $(SRC_DIR)/$(APP)/$(APPLICATION).elf
	hexdump -v -e '4/1 "%02x" "\n"' $(SRC_DIR)/$(APP)/$(APPLICATION).bin > $(SRC_DIR)/$(APP)/$(APPLICATION).txt

clean:
	rm -rf *.o *~ *.elf *.bin *.cnt *.lst *.sec *.txt *.hex
	rm -rf $(SRC_DIR)/$(APP)/*.elf
	rm -rf $(SRC_DIR)/$(APP)/*.bin
	rm -rf $(SRC_DIR)/$(APP)/*.cnt
	rm -rf $(SRC_DIR)/$(APP)/*.lst
	rm -rf $(SRC_DIR)/$(APP)/*.sec
	rm -rf $(SRC_DIR)/$(APP)/*.txt
	rm -rf $(SRC_DIR)/$(APP)/*.hex

