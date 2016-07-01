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

# fill the hypervisor bin file to 64Kb
dd if=prplHypervisor.bin of=/tmp/prplHypervisor.bin bs=64K conv=sync

for i in $*; do 
    ## fill the VMs binary file to 32Kb
    dd if=bare-metal-apps/build/$i.bin of=/tmp/$i.bin bs=32K conv=sync
done

rm -rf /tmp/tmp.bin

cat /tmp/prplHypervisor.bin > /tmp/firmware.bin 


for i in $*; do 
    # concatenate all binary files to the firmware.bin file
    cat /tmp/firmware.bin  /tmp/$i.bin >> /tmp/tmp.bin 
    mv /tmp/tmp.bin /tmp/firmware.bin
done

#convert the binary file to intel hex 
srec_cat /tmp/firmware.bin -binary -offset 0x1d000000 -o firmware.hex -intel -line-length=44
