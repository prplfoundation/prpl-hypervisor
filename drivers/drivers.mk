#
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

DRIVER_INCLUDE=-I$(TOPDIR)drivers/include/

ifeq ($(CONFIG_INTERVMCOMM_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/inter-vm-comm.c
endif 

ifeq ($(CONFIG_PIC32MZ_USB_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-usb.c
endif 

ifeq ($(CONFIG_VIRTUAL_IO_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/virtual-io.c
endif 

ifeq ($(CONFIG_INTERRUPT_REDIRECT_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-interrupt-redirect.c
endif 

ifeq ($(CONFIG_PUF_FLASH_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-puf-flash.c
endif 

ifeq ($(CONFIG_TIMER_TEST_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-timer-test.c
endif 

ifeq ($(CONFIG_PERFORMANCE_COUNTER_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/performance-counter.c
endif 

ifeq ($(CONFIG_INTERRUPT_LATENCY_TEST_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-interrupt-latency-test.c
endif 

ifeq ($(CONFIG_PIC32MZ_ETHERNET_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-ethernet.c
endif 

ifeq ($(CONFIG_PIC32MZ_ETHERNET_SHARED_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/pic32mz-ethernet-shared.c
endif 


ifeq ($(CONFIG_BAIKALT1_UART_DRV),yes)
	DRIVERS_LIST += $(TOPDIR)drivers/t1-baikal-UART-test.c
endif 


drivers:
	$(CC) $(CFLAGS) $(INC_DIRS) $(DRIVER_INCLUDE) $(DRIVERS_LIST)
	     
