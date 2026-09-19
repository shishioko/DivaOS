#!/bin/bash

DISK_SIZE=2G
EFI_PARTITION_SIZE=64M
DATA_PARTITION_SIZE=1G

#Initialize Script
set -e
shopt -s nullglob

#Create required directories
rm -rf ./{Build,Artifacts}/*
mkdir -p ./{Source,Build,Artifacts}

cd ./Build/

#Compile and link kernel
g++ -mcmodel=kernel -ffreestanding -fno-exceptions -fno-rtti -fno-pic -fno-pie -mno-red-zone -c -I ../Source/x86_64/Kernel/ $(printf -- '-I %s ' ../Source/{x86_64,Any}/{Kernel,Shared}/) ../Source/{x86_64,Any}/{Kernel,Shared}/*.cpp
ld  -m elf_x86_64 -T ../Scripts/Kernel.ld -Map=./Kernel.map --oformat elf64-x86-64 -o ./Kernel.elf ./*.o
objcopy -O binary ./Kernel.elf ./Kernel.bin
rm -f ./*.o

#Assemble and link loader
g++ -mcmodel=small -ffreestanding -fno-exceptions -fno-rtti -fno-pic -fno-pie -mno-red-zone -c -I ../Source/x86_64/Loader/ $(printf -- '-I %s ' ../Source/{x86_64,Any}/{Loader,Shared}/) ../Source/{x86_64,Any}/{Loader,Shared}/*.cpp
nasm -f elf64 -o ./Loader.asm.o $(printf -- '-I %s ' ../Source/{x86_64,Any}/{Loader,Shared}/) ../Source/Any/Loader/Main.asm
ld -m elf_x86_64 -T ../Scripts/Loader.ld -Map=./Loader.map --oformat binary -o ./Loader.bin ./*.o
rm -f ./*.o

#Produce EFI Partition
{
    #Collect files for EFI partition
    mkdir -p ./EFI.fs
    cp ./Kernel.elf ./EFI.fs/Kernel.elf
    #Build EFI partition
    EFI_PARTITION_RESERVED=$(( (( $(stat -c%s "./Loader.bin") + 511) / 512) + 3 ))
    if [ $EFI_PARTITION_RESERVED -lt 32 ]; then
        EFI_PARTITION_RESERVED=32
    fi
    truncate -s $EFI_PARTITION_SIZE ./EFI.img
    mkfs.vfat -F 32 -R $EFI_PARTITION_RESERVED -n "DIVAOS_EFI" ./EFI.img >/dev/null
    dd if=./Loader.bin of=./EFI.img bs=512 skip=1 seek=3 count=$EFI_PARTITION_RESERVED conv=notrunc status=none
    dd if=./Loader.bin of=./EFI.img bs=1 count=3 conv=notrunc status=none
    dd if=./Loader.bin of=./EFI.img bs=1 skip=90 seek=90 count=422 conv=notrunc status=none
    mcopy -i ./EFI.img -s ./EFI.fs/* ::/
}

#Produce Disk Image
truncate -s $DISK_SIZE ./Disk.img
dd if=./EFI.img of=./Disk.img bs=512 seek=2048 conv=notrunc status=none
dd if=/usr/lib/syslinux/mbr/mbr.bin of=./Disk.img bs=446 count=1 conv=notrunc status=none
echo ",,0x0C,*" | sfdisk ./Disk.img > /dev/null
mv ./Disk.img ../Artifacts/Disk.img

cd ..