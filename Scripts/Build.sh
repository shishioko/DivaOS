#!/bin/bash
nasm -i ./Source/x86_64/ -i ./Source/Any/ ./Source/Any/Boot.asm -f bin -o ./Artifacts/Disk.bin