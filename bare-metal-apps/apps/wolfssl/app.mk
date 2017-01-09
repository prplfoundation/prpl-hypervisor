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


# Define your additional include paths
INC_DIRS += -I ../../../../picotcp/build/include/ -I  $(TOPDIR)../../wolfssl/ -I $(TOPDIR)../../picotcp-modules/libhttps/build/include

#Aditional C flags
CFLAGS += -DPIC32  -DPICOTCP -DLIBHTTPS_USE_WOLFSSL

#Aditional Libraries
LIBS += ../../../../picotcp-modules/libhttps/build/lib/libhttps.a ../../../../picotcp/build/lib/libpicotcp.a ../../../../wolfssl/build/libwolfssl.a 

#default stack size 512 bytes
STACK_SIZE = 8192

#Include your additional mk files here. 

wolf_clone:
	if [ ! -d "$(TOPDIR)../../wolfssl" ]; then \
		git clone https://github.com/wolfSSL/wolfssl.git $(TOPDIR)../../wolfssl; \
	fi
	
wolf_compile:
	if [ ! -f "$(TOPDIR)../../wolfssl/.compiled" ]; then \
		cp $(TOPDIR)apps/$(APP)/wolfssl.mk $(TOPDIR)/../../wolfssl/Makefile; \
		make -C ../../../../wolfssl/ clean; \
		make -C ../../../../wolfssl/; \
		touch $(TOPDIR)/../../wolfssl/.compiled; \
	fi

picotcp_modules_clone:
	if [ ! -d "$(TOPDIR)../../picotcp-modules" ]; then \
		git clone https://github.com/tass-belgium/picotcp-modules.git  $(TOPDIR)../../picotcp-modules; \
	fi

picotcp_modules_compile:
	if [ ! -f "$(TOPDIR)../../picotcp-modules/libhttps/.compiled" ]; then \
		mkdir -p $(TOPDIR)../../picotcp-modules/libhttps/build/include; \
		mkdir -p $(TOPDIR)../../picotcp-modules/libhttps/build/lib; \
		make -C $(TOPDIR)../../picotcp-modules/libhttps clean; \
		make -C $(TOPDIR)../../picotcp-modules/libhttps CROSS_COMPILE=mips-mti-elf- PLATFORM_CFLAGS="-EL -Os -c -Wa,-mvirt -mips32r5 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -DLIBHTTPS_USE_WOLFSSL -DARCH=pic32 -I../../picotcp/build/include -DNO_WRITEV -I../../wolfssl/"  CC=mips-mti-elf-gcc PREFIX=./build; \
		touch $(TOPDIR)../../picotcp-modules/libhttps/.compiled; \
	fi

picotcp_clone:
	if [ ! -d "$(TOPDIR)../../picotcp" ]; then \
		git clone https://github.com/crmoratelli/picotcp.git $(TOPDIR)../../picotcp; \
	fi

picotcp_compile:
	if [ ! -f "$(TOPDIR)../../picotcp/.compiled" ]; then \
		make -C $(TOPDIR)../../picotcp clean; \
		make -C $(TOPDIR)../../picotcp CROSS_COMPILE=mips-mti-elf- PLATFORM_CFLAGS="-EL -Os -c -Wa,-mvirt -mips32r5 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0" \
		DHCP_SERVER=0 DHCP_CLIENT=0 SLAACV4=0 TFTP=0 AODV=0 IPV6=0 NAT=0 PING=1 ICMP4=1 DNS_CLIENT=0 DNS=0 MDNS=0 DNS_SD=0 SNTP_CLIENT=0 PPP=0 MCAST=1 MLD=0 IPFILTER=0 \
		ARCH=pic32 ICMP6=0 WOLFSSL=1 PREFIX=./build; \
		touch $(TOPDIR)../../picotcp/.compiled; \
	fi
	
app: picotcp_clone picotcp_compile picotcp_modules_clone picotcp_modules_compile wolf_clone wolf_compile
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/wolfssl_app.c -o $(TOPDIR)apps/$(APP)/wolfssl_app.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/web_files.c -o $(TOPDIR)apps/$(APP)/web_files.o

