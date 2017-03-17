hal:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/baikal-t1/eth.c -o $(TOPDIR)apps/$(APP)/eth.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/baikal-t1/platform.c -o $(TOPDIR)apps/$(APP)/platform.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)arch/mips/baikal-t1/crt0.s -o $(TOPDIR)apps/$(APP)/crt.o

