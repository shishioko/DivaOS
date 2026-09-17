#!/bin/bash
nasm -i ./Source/x86_64/ -i ./Source/Any/ ./Source/Any/Main.asm -f bin -o ./Artifacts/Disk.bin