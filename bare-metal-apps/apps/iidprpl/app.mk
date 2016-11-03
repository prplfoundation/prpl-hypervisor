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
#INC_DIRS += 

#Aditional C flags
CFLAGS += -DVIRTUALIZED_IO

#Aditional Libraries
LIBS += -L../../apps/iidprpl/ -liidprplpuf

#default stack size 512 bytes
STACK_SIZE = 512

# The PUF VM needs a specific linker script. 
#This section substitute the original pic32mz.ld linker by a modified one (pic32mz.ld.puf). 
config_linker:
	cp ../../apps/$(APP)/pic32mz.ld.puf ../../apps/$(APP)/pic32mz.ld
	sed -i -e 's/FLASH_SIZE/$(FLASH_SIZE)/' ../../apps/$(APP)/pic32mz.ld
	sed -i -e 's/RAM_SIZE/$(RAM_SIZE)/' ../../apps/$(APP)/pic32mz.ld
	sed -i -e 's/STACK_SIZE/$(STACK_SIZE)/' ../../apps/$(APP)/pic32mz.ld
	

app: config_linker
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/$(APP).c -o $(TOPDIR)apps/$(APP)/$(APP).o

