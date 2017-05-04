#!/bin/bash
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


URL_BAIKAL=http://www.baikalelectronics.com/upload/iblock/35f/bsp_baikal_mips_4_00_08.run
FILE=bsp_baikal_mips_4_00_08.run
DOWNLOAD_DIR=~/baikal-bsp

download_baikal(){
	echo "************************************************************************************"
	echo "Downloading the Baikal BSP.  After download follow the instructions on the screen."
	echo "************************************************************************************"
	echo
	wget -O $FILE $URL_BAIKAL
	
	chmod +x $FILE
	./$FILE
}


# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root." 1>&2
   exit 1
fi

apt-get update
apt-get --yes --force-yes install srecord
apt-get --yes --force-yes install libconfig-dev

mkdir -p $DOWNLOAD_DIR
pushd $DOWNLOAD_DIR

if [ ! -f "$URL_BAIKAL" ]; then
	download_baikal;
fi;


popd

echo  >> ~/.profile
echo "export PATH=\"\$PATH:\"$HOME/baikal-bsp/baikal/usr/x-tools/mipsel-unknown-linux-gnu/bin/\"\"" >> ~/.profile
echo  >> ~/.profile

echo "***********************************************************"
echo "You need to logout to the changes on you PATH make effect. "
echo "***********************************************************"

