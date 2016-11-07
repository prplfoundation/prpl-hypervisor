
lib:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)lib/libc.c -o $(TOPDIR)apps/$(APP)/libc.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)lib/malloc.c -o $(TOPDIR)apps/$(APP)/malloc.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)lib/network.c -o $(TOPDIR)apps/$(APP)/network.o


