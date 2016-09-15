PREFIX?=$(PWD)/../../../picotcp/build
CC=gcc
CROSS_COMPILE?=mips-mti-elf-
CFLAGS+=-I . -I $(PREFIX)/include -DWOLFSSL_PICOTCP -DTFM_TIMING_RESISTANT -DNO_WRITEV -DNO_ERROR_STRINGS -DSMALL_SESSION_CACHE -DWOLFSSL_SMALL_STACK -DHAVE_TLS_EXTENSIONS -DNO_DSA -DNO_RABBIT -DNO_MD4 -DNO_DES3 -DNO_FILESYSTEM -DUSE_FAST_MATH -DRSA_DECODE_EXTRA -DHAVE_ECC -DPICOTCP -DSINGLE_THREADED -DWOLFSSL_USER_IO -DNO_WOLFSSL_SERVER -DNO_DH -DNO_AES -DWOLFSSL_MICROCHIP_PIC32MZ

#prpl flags
CFLAGS+= -EL -O2 -mtune=m14k -mips32r2
CFLAGS+= -Wa,-mvirt
CFLAGS+= -mno-check-zero-division -mhard-float -fshort-double
CFLAGS+= -c -ffreestanding -nostdlib -fomit-frame-pointer -G 0

ASFLAGS+= -EL -mvirt -mips32r2

LDFLAGS+= -EL

# Fetch source files
SRC_DIR=src wolfcrypt/src
CXX_FILES := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))

# Translate to .o object files
OBJS:= $(patsubst %.c,%.o,$(CXX_FILES))
#OBJS:= $(patsubst wolfcrypt/src/%.c,$(PREFIX)/ssl/wolfcrypt/%.o,$(OBJ_FILES))

all: $(PREFIX)/lib/libwolfssl.a

%.o: %.c
	$(CROSS_COMPILE)$(CC) -c $(CFLAGS) -o $@ $<

$(PREFIX)/lib/libwolfssl.a: $(OBJS)
	@mkdir -p $(PREFIX)/include/wolfssl/wolfcrypt
	@cp wolfssl/*.h  $(PREFIX)/include/wolfssl
	@cp wolfssl/wolfcrypt/*.h  $(PREFIX)/include/wolfssl/wolfcrypt
	@cp -r wolfssl/openssl/ $(PREFIX)/include/
	@$(CROSS_COMPILE)ar cru $@ $(OBJS)
	@$(CROSS_COMPILE)ranlib $@

clean:
	rm -f $(OBJS)
