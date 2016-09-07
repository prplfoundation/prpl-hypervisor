#Copyright (c) 2016, prpl Foundation

#Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
#fee is hereby granted, provided that the above copyright notice and this permission notice appear 
#in all copies.

#THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
#INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
#FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
#LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
#ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

#!/bin/sh

#hypervisor base address
BASE_ADDR=0x9d000000

# Determine the list of the VM's flash start addresses.
NUM_VM=0
for i in $*; do 
    START_ADDR=$(awk '/'$i'/{print $4}' include/vms.info)
    address_array[$NUM_VM]=$(echo $(($START_ADDR)))
    ((NUM_VM++))
    
done 

# Determines the hypervisor padding based on the start address of the first VM.  
AUX=$(echo $(($BASE_ADDR)))
PADDING=$(expr ${address_array[0]} - $AUX)

#increment to the first VM address
BASE_ADDR=$(expr $AUX + $PADDING)

# fill the hypervisor bin file to the padding
dd if=prplHypervisor.bin of=/tmp/prplHypervisor.bin bs=$PADDING conv=sync

#padding of the VM's
COUNT=1
for i in $*; do 
    # Determines the VM's padding size based on the start address of the next VM, or 
    #use the size of the VM in case of the last one
    if [ $COUNT -lt $NUM_VM ]; then 
        PADDING=$(expr ${address_array[$COUNT]} - $BASE_ADDR)
    else
        flash_size=$(awk '/'$i'/{print $2}' include/vms.info)
        PADDING=$(echo $(($flash_size)))
    fi
    
    #padding the VM's bin to its maximum size
    dd if=../../bare-metal-apps/apps/$i/$i.bin of=/tmp/$i.bin bs=$PADDING conv=sync
    
    ((COUNT++))
    BASE_ADDR=$(expr $BASE_ADDR + $PADDING)
done

rm -rf /tmp/tmp.bin

cat /tmp/prplHypervisor.bin > /tmp/firmware.bin 


for i in $*; do 
    # concatenate all binary files to the firmware.bin file
    cat /tmp/firmware.bin  /tmp/$i.bin >> /tmp/tmp.bin 
    mv /tmp/tmp.bin /tmp/firmware.bin
done

srec_cat /tmp/firmware.bin -binary -offset 0x1d000000 -o firmware.hex -intel -line-length=44
