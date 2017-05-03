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


URL_MPLABIPE=http://www.microchip.com/mplabx-ide-linux-installer
URL_CODESCAPE=https://community.imgtec.com/?do-download=linux-x64-mti-bare-metal-2016-05-03
MPLABIPE=MPLABX-v3.51-linux-installer.tar
CODESCAPE=Codescape-Bare-Metal.tar.gz
DOWNLOAD_DIR=~/Downloads/hyper_files

download_mplab(){
	echo "************************************************************************************"
	echo "Downloading the Michochip IDE. After download follow the instructions on the screen."
	echo "************************************************************************************"
	echo
	wget -O $MPLABIPE $URL_MPLABIPE
}

download_codescape(){
	echo "************************************************************************************"
	echo "Downloading the Linux x64 MTI Bare Metal 2016.05-03. After download follow the "
	echo "instructions on the screen."
	echo "************************************************************************************"
	echo
	wget -O $CODESCAPE $URL_CODESCAPE
}

FILE="/tmp/out.$$"
GREP="/bin/grep"
# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root." 1>&2
   exit 1
fi

dpkg --add-architecture i386
apt-get update
apt-get --yes --force-yes install gcc-multilib
apt-get --yes --force-yes install libc6:i386 libncurses5:i386 libstdc++6:i386
apt-get --yes --force-yes install libexpat1-dev:i386
apt-get --yes --force-yes install libx11-dev:i386
apt-get --yes --force-yes install libxext-dev:i386 
apt-get --yes --force-yes purge modemmanager
apt-get --yes --force-yes install srecord
apt-get --yes --force-yes install libconfig-dev

mkdir -p $DOWNLOAD_DIR
pushd $DOWNLOAD_DIR

if [ ! -f "$MPLABIPE" ]; then
	download_mplab;
fi;

if [ ! -f "$CODESCAPE" ]; then
	download_codescape;
fi;

tar -xvf MPLABX-v3.51-linux-installer.tar
./MPLABX-v3.51-linux-installer.sh

tar -zxvf Codescape-Bare-Metal.tar.gz 
mv mips-mti-elf /opt

popd

echo  >> ~/.profile
echo "export PATH=\"\$PATH:\"/opt/mips-mti-elf/2016.05-03/bin\"\"" >> ~/.profile
echo  >> ~/.profile

echo "***********************************************************"
echo "You need to logout to the changes on you PATH make effect. "
echo "***********************************************************"

