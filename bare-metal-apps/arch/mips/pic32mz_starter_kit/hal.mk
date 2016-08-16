hal:
	$(CC) $(CFLAGS) $(INC_DIRS) \
        	$(TOPDIR)arch/mips/pic32mz_starter_kit/eth.c \
        	$(TOPDIR)arch/mips/pic32mz_starter_kit/platform.c \
        	$(TOPDIR)arch/mips/pic32mz_starter_kit/crt0.s 

