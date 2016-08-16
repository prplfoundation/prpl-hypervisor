
lib:
	$(CC) $(CFLAGS) $(INC_DIRS) \
	        $(TOPDIR)lib/libc.c \
	        $(TOPDIR)lib/malloc.c \
	        $(TOPDIR)lib/network.c 


