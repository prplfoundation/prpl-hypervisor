hal:
	$(CC) $(CFLAGS) $(INC_DIRS) \
        	$(TOPDIR)arch/mips/pic32mz/eth.c \
        	$(TOPDIR)arch/mips/pic32mz/platform.c \
        	$(TOPDIR)arch/mips/pic32mz/crt0.s 

