#!/bin/bash

DISK_SIZE=1G
EFI_PARTITION_SIZE=256M
DATA_PARTITION_SIZE=512M

#Process Parameters
DISK_SIZE_BLOCKS=$(($(echo "$DISK_SIZE" | numfmt --from=iec) / 512))
EFI_PARTITION_SIZE_BLOCKS=$(($(echo "$EFI_PARTITION_SIZE" | numfmt --from=iec) / 512))
DATA_PARTITION_SIZE_BLOCKS=$(($(echo "$DATA_PARTITION_SIZE" | numfmt --from=iec) / 512))

#Initialize Script
set -e
shopt -s nullglob
FLAGS="-O0"
CPP_FLAGS=""
if [[ " $@ " =~ " -optimize " ]]; then
    CPP_FLAGS="$CPP_FLAGS -O3"
fi

#Create required directories
rm -rf ./{Build,Artifacts}/*
mkdir -p ./{Source,Build,Artifacts}

#Compile and link kernel
g++ -m64 -T ./Scripts/Kernel.ld -no-pie -mcmodel=kernel -ffreestanding -fno-exceptions -fno-rtti -fno-pic -fno-pie -mno-red-zone -nostdlib -Wno-pointer-arith -masm=intel $CPP_FLAGS -g3 -o ./Build/DivaOS.elf -I ./Source/x86_64/ $(printf -- '-I %s ' ./Source/{x86_64,Any}/) $(printf -- '-I %s ' ./Source/{x86_64,Any}/) $(find ./Source/{x86_64,Any}/ -name "*.cpp")
objcopy --only-keep-debug ./Build/DivaOS.elf ./Build/Kernel.sym
objcopy --strip-debug ./Build/DivaOS.elf

#Download Limine Bootloader
wget -q --show-progress -P ./Build/ https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz
mkdir ./Build/limine-binary/
tar --strip-components=1 -C ./Build/limine-binary -xf ./Build/limine-binary.tar.gz
cd ./Build/limine-binary/
make
cd ../../

#Produce EFI Partition
{
    #Collect files for EFI partition
    mkdir -p ./Build/EFI.fs/{EFI/BOOT,boot/{limine,DivaOS}}
    cp ./Build/limine-binary/BOOTX64.EFI ./Build/EFI.fs/EFI/BOOT/BOOTX64.EFI
    cp ./Build/limine-binary/BOOTIA32.EFI ./Build/EFI.fs/EFI/BOOT/BOOTIA32.EFI
    cp ./Build/limine-binary/limine-bios.sys ./Build/EFI.fs/boot/limine/limine-bios.sys
    cp ./Build/DivaOS.elf ./Build/EFI.fs/boot/DivaOS/DivaOS.elf
	cat <<- 'EOF' > ./Build/EFI.fs/boot/limine/limine.conf
		timeout: 3

		/DivaOS
		    protocol: limine
		    path: boot():/boot/DivaOS/DivaOS.elf
	EOF
    #Build EFI partition
    truncate -s "$((EFI_PARTITION_SIZE_BLOCKS*512))" ./Build/EFI.img
    /usr/sbin/mkfs.vfat -F 32 -n "DIVA_LIMINE" ./Build/EFI.img >/dev/null
    mcopy -o -i ./Build/EFI.img -s ./Build/EFI.fs/* ::/
}

#Produce Disk Image
truncate -s "$((DISK_SIZE_BLOCKS*512))" ./Build/Disk.img
sgdisk --clear
sgdisk --new=1:2048:4095 --typecode=1:ef02 --change-name=1:"BIOS Boot Partition" ./Build/Disk.img
sgdisk --new=2:4096:+"$EFI_PARTITION_SIZE_BLOCKS" --typecode=2:ef00 --change-name=2:"EFI System Partition" ./Build/Disk.img
./Build/limine-binary/limine bios-install ./Build/Disk.img 1
dd status=none conv=notrunc seek=4096 bs=512 count="$EFI_PARTITION_SIZE_BLOCKS" of=./Build/Disk.img if=./Build/EFI.img 

#Export Artifacts
mv ./Build/Disk.img ./Artifacts/Disk.img