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
#INC_DIRS += $(TOPDIR)../../benchmark/dhrystone/

#Aditional C flags
CFLAGS += -DVIRTUALIZED_IO -DPIC32MZ -DMEM_METHOD=MEM_STATIC -DHAS_PRINTF -DMAIN_HAS_NOARGC -DLITTLE_ENDIAN

PORT_CFLAGS = -O2 -EL
XCFLAGS = -DPERFORMANCE_RUN=1
XLFLAGS = -lrt -static
FLAGS_STR = "$(PORT_CFLAGS) $(XCFLAGS) $(XLFLAGS) $(LFLAGS_END)"
CFLAGS  += $(PORT_CFLAGS) $(IPATH) -DFLAGS_STR=\"$(FLAGS_STR)\"
CFLAGS += -DITERATIONS=0 
CFLAGS += $(XCFLAGS)


#Aditional Libraries
#LIBS +=

#stack size 2048 bytes
STACK_SIZE = 2048

#Include your additional mk files here. 

app:
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_list_join.c -o $(TOPDIR)apps/$(APP)/core_list_join.o
	$(CC) $(CFLAGS) -DFLAGS_STR=\"$(CFLAGS)\" $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_main.c -o $(TOPDIR)apps/$(APP)/core_main.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_matrix.c -o $(TOPDIR)apps/$(APP)/core_matrix.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_state.c -o $(TOPDIR)apps/$(APP)/core_state.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_util.c -o $(TOPDIR)apps/$(APP)/core_util.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)../../benchmark/coremark/core_portme.c -o $(TOPDIR)apps/$(APP)/core_portme.o
	$(CC) $(CFLAGS) $(INC_DIRS) $(TOPDIR)apps/$(APP)/main.c -o $(TOPDIR)apps/$(APP)/main.o
