hal:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/pic32mz/eth.c -o $(TOPDIR)apps/$(APP)/eth.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/pic32mz/platform.c -o $(TOPDIR)apps/$(APP)/platform.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/pic32mz/crt0.s -o $(TOPDIR)apps/$(APP)/crt.o

