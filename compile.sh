#!/bin/bash
nasm -i ./Source ./Source/Main.asm -f bin -o ./Disk.bin
qemu-system-x86_64 -accel tcg -cpu core2duo -m 1024 -drive format=raw,file=Disk.bin -smp 8 -vga std -monitor stdio -no-reboot -no-shutdown -d unimp,guest_errors