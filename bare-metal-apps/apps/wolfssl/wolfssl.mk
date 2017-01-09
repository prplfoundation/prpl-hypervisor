#Copyright (c) 2016, prpl Foundation
#
#Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
#fee is hereby granted, provided that the above copyright notice and this permission notice appear 
#in all copies.
#
#THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
#INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
#FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
#LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
#ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

PREFIX?=$(PWD)/../../../../wolfssl/build
PICOTCP=$(PWD)/../../../../picotcp/build/include
CC=gcc
CROSS_COMPILE?=mips-mti-elf-
CFLAGS+=-I. -Iwolfssl -I$(PICOTCP) 

#prpl flags
CFLAGS+= -EL -O2 -mtune=m14k -mips32r2
CFLAGS+= -Wa,-mvirt
CFLAGS+= -mno-check-zero-division -msoft-float -fshort-double
CFLAGS+= -c -ffreestanding -nostdlib -fomit-frame-pointer -G 0

CXX_FILES = \
src/ssl.c \
src/tls.c \
src/internal.c \
src/keys.c \
src/io.c \
wolfcrypt/src/camellia.c \
wolfcrypt/src/dh.c \
wolfcrypt/src/ripemd.c \
wolfcrypt/src/aes.c \
wolfcrypt/src/md4.c \
wolfcrypt/src/coding.c \
wolfcrypt/src/curve25519.c \
wolfcrypt/src/ecc_fp.c \
wolfcrypt/src/dsa.c \
wolfcrypt/src/sha256.c \
wolfcrypt/src/rsa.c \
wolfcrypt/src/srp.c \
wolfcrypt/src/hash.c \
wolfcrypt/src/asm.c \
wolfcrypt/src/blake2b.c \
wolfcrypt/src/logging.c \
wolfcrypt/src/ed25519.c \
wolfcrypt/src/wolfmath.c  \
wolfcrypt/src/ecc.c \
wolfcrypt/src/pkcs7.c \
wolfcrypt/src/arc4.c \
wolfcrypt/src/compress.c \
wolfcrypt/src/md5.c \
wolfcrypt/src/misc.c \
wolfcrypt/src/rabbit.c \
wolfcrypt/src/wc_encrypt.c  \
wolfcrypt/src/idea.c \
wolfcrypt/src/cmac.c \
wolfcrypt/src/integer.c \
wolfcrypt/src/chacha20_poly1305.c \
wolfcrypt/src/poly1305.c \
wolfcrypt/src/pkcs12.c \
wolfcrypt/src/des3.c \
wolfcrypt/src/error.c \
wolfcrypt/src/md2.c \
wolfcrypt/src/fe_low_mem.c \
wolfcrypt/src/fe_operations.c \
wolfcrypt/src/ge_low_mem.c \
wolfcrypt/src/async.c \
wolfcrypt/src/pwdbased.c \
wolfcrypt/src/hmac.c \
wolfcrypt/src/sha512.c \
wolfcrypt/src/signature.c \
wolfcrypt/src/wolfevent.c \
wolfcrypt/src/asn.c \
wolfcrypt/src/hc128.c \
wolfcrypt/src/ge_operations.c \
wolfcrypt/src/tfm.c \
wolfcrypt/src/wc_port.c \
wolfcrypt/src/sha.c \
wolfcrypt/src/memory.c \
wolfcrypt/src/random.c \
wolfcrypt/src/chacha.c 

       
# Translate to .o object files
OBJS:= $(patsubst %.c,%.o,$(CXX_FILES))
#OBJS:= $(patsubst wolfcrypt/src/%.c,$(PREFIX)/ssl/wolfcrypt/%.o,$(OBJ_FILES))

all: $(PREFIX)/libwolfssl.a

%.o: %.c
	$(CROSS_COMPILE)$(CC) -c $(CFLAGS) -o $@ $<

$(PREFIX)/libwolfssl.a: $(OBJS)
	@mkdir -p $(PREFIX)
	#@cp wolfssl/*.h  $(PREFIX)/include/wolfssl
	#@cp wolfssl/wolfcrypt/*.h  $(PREFIX)/include/wolfssl/wolfcrypt
	#@cp -r wolfssl/openssl/ $(PREFIX)/include/
	@$(CROSS_COMPILE)ar cru $@ $(OBJS)
	@$(CROSS_COMPILE)ranlib $@

clean:
	rm -f $(OBJS)
