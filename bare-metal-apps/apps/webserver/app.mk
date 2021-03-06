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
INC_DIRS += -I ../../../../picotcp/build/include/ -I $(TOPDIR)../../picotcp-modules/libhttp/

#Aditional C flags
CFLAGS += -DPICOTCP -DPIC32

#Aditional Libraries
LIBS += ../../../../picotcp/build/lib/libpicotcp.a 

#default stack size 512 bytes
STACK_SIZE = 4096

#Include your additional mk files here. 

picotcp_modules_clone:
	if [ ! -d "$(TOPDIR)../../picotcp-modules" ]; then \
		git clone https://github.com/tass-belgium/picotcp-modules.git  $(TOPDIR)../../picotcp-modules; \
	fi

picotcp_modules_compile:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../picotcp-modules/libhttp/pico_http_server.c -o $(TOPDIR)apps/$(APP)/pico_http_server.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../picotcp-modules/libhttp/pico_http_util.c -o $(TOPDIR)apps/$(APP)/pico_http_util.o 


picotcp_clone:
	if [ ! -d "$(TOPDIR)../../picotcp" ]; then \
		git clone https://github.com/crmoratelli/picotcp.git $(TOPDIR)../../picotcp; \
	fi

picotcp_compile:
	if [ ! -f "$(TOPDIR)../../picotcp/.compiled" ]; then \
		make -C $(TOPDIR)../../picotcp clean; \
		make -C $(TOPDIR)../../picotcp CROSS_COMPILE=mips-mti-elf- PLATFORM_CFLAGS="-EL -Os -c -Wa,-mvirt -mips32r5 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0 -DPIC32" \
		DHCP_SERVER=0 DHCP_CLIENT=0 SLAACV4=0 TFTP=0 AODV=0 IPV6=0 NAT=0 PING=1 ICMP4=1 DNS_CLIENT=0 DNS=0 MDNS=0 DNS_SD=0 SNTP_CLIENT=0 PPP=0 MCAST=1 MLD=0 IPFILTER=0 \
		ARCH=pic32 ICMP6=0 WOLFSSL=1 PREFIX=./build; \
		touch $(TOPDIR)../../picotcp/.compiled; \
	fi
	
app: picotcp_clone picotcp_compile picotcp_modules_clone picotcp_modules_compile
	@./$(TOPDIR)apps/$(APP)/html-sources/make-Cfiles.py 
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/webserver.c -o $(TOPDIR)apps/$(APP)/webserver.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/web_files.c -o $(TOPDIR)apps/$(APP)/web_files.o

