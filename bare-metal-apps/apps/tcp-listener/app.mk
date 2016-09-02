
app: wolfssl
	$(CC) $(CFLAGS) $(INC_DIRS) \
		$(TOPDIR)apps/$(APP)/$(APP).c 

wolfssl:
	@cp wolfssl.mk ../../../../wolfssl/Makefile
	make -C ../../../../wolfssl/ clean
	make -C ../../../../wolfssl/
