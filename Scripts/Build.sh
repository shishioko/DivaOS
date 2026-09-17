#!/bin/bash

#Create required directories
mkdir -p ./{Source,Build,Artifacts}

#Assemble and compile code
nasm -f elf64 -o ./Build/Loader.o -i ./Source/x86_64/ -i ./Source/Any/ ./Source/Any/Boot.asm
g++ -ffreestanding -fno-exceptions -fno-rtti -c -o ./Build/Kernel.o -I ./Source/x86_64/ -I ./Source/Any/ ./Source/Any/Main.cpp

#Produce disk binary
ld -T ./Scripts/Linker.ld --oformat binary -o ./Artifacts/Disk.bin ./Build/Loader.o ./Build/Kernel.o