#!/bin/sh

dd if=prplHypervisor.bin of=/tmp/prplHypervisor.bin bs=64K conv=sync
dd if=bare-metal-apps/apps/$1.bin of=/tmp/$1.bin bs=64K conv=sync
cat /tmp/prplHypervisor.bin /tmp/$1.bin > /tmp/firmware.bin 
srec_cat /tmp/firmware.bin -binary -offset 0x1d000000 -o firmware.hex -intel -line-length=44
