APP = app/cond_test1
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

serial:
	stty ${SERIAL_BAUD} raw cs8 -parenb -crtscts clocal cread ignpar ignbrk -ixon -ixoff -ixany -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke -F ${SERIAL_DEVICE}

load: serial
	./pic32prog -d ${SERIAL_DEVICE} image.hex 

debug: serial
	cat ${SERIAL_DEVICE}

image: hal libc kernel app
	$(LD) $(LDFLAGS) -T$(LINKER_SCRIPT) -o image.elf *.o
	$(DUMP) --disassemble --reloc image.elf > image.lst
	$(DUMP) -h image.elf > image.sec
	$(DUMP) -s image.elf > image.cnt
	$(OBJ) -O binary image.elf image.bin
	$(OBJ) -O ihex --change-addresses=0x80000000 image.elf image.hex
	$(SIZE) image.elf
	hexdump -v -e '4/1 "%02x" "\n"' image.bin > image.txt

clean:
	rm -rf *.o *~ *.elf *.bin *.cnt *.lst *.sec *.txt *.hex

