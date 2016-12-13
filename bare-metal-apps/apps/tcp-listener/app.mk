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
INC_DIRS += -I ../../../../picotcp/build/include/

#Aditional C flags
CFLAGS += -DPICOTCP

#Aditional Libraries
LIBS += ../../../../picotcp/build/lib/libpicotcp.a

#default stack size 512 bytes
STACK_SIZE = 4096

#include your additional mk files. 
picotcp_clone:
	if [ ! -d "$(TOPDIR)../../picotcp" ]; then \
		git clone https://github.com/crmoratelli/picotcp.git $(TOPDIR)../../picotcp; \
	fi

picotcp_compile:
	if [ ! -f "$(TOPDIR)../../picotcp/.compiled" ]; then \
		make -C $(TOPDIR)../../picotcp CROSS_COMPILE=mips-mti-elf- PLATFORM_CFLAGS="-EL -Os -c -Wa,-mvirt -mips32r5 -mtune=m14k -mno-check-zero-division -msoft-float -fshort-double -ffreestanding -nostdlib -fomit-frame-pointer -G 0" \
		DHCP_SERVER=0 DHCP_CLIENT=0 SLAACV4=0 TFTP=0 AODV=0 IPV6=0 NAT=0 PING=1 ICMP4=1 DNS_CLIENT=0 DNS=0 MDNS=0 DNS_SD=0 SNTP_CLIENT=0 PPP=0 MCAST=1 MLD=0 IPFILTER=0 ARCH=pic32 ICMP6=0 PREFIX=./build; \
		touch $(TOPDIR)../../picotcp/.compiled; \
	fi

app: picotcp_clone picotcp_compile
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/$(APP).c -o $(TOPDIR)apps/$(APP)/$(APP).o

