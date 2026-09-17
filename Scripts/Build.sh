#!/bin/bash

#Create required directories
mkdir ./{Source,Build,Artifacts}

#Assemble and compile code
nasm -f elf64 -o ./Build/Loader.o -i ./Source/x86_64/ -i ./Source/Any/ ./Source/Any/Boot.asm
gcc -ffreestanding -c -o ./Build/Kernel.o -I ./Source/x86_64/ -I ./Source/Any/ ./Source/Any/Main.c

#Produce disk binary
ld -T ./Scripts/Linker.ld --oformat binary -o ./Artifacts/Disk.bin ./Build/Loader.o ./Build/Kernel.o