[section .data]

global DivaOS.Loader.Memory.LoaderMemory.Start
; Holds the initial location of the memory to be used by the bootloader
DivaOS.Loader.Memory.LoaderMemory.Start: dq DivaOS.Loader.Memory.LoaderMemory.Memory.Start
global DivaOS.Loader.Memory.LoaderMemory.Offset
; Holds the current location of the memory to be used by the bootloader
DivaOS.Loader.Memory.LoaderMemory.Offset: dq DivaOS.Loader.Memory.LoaderMemory.Memory.Start
global DivaOS.Loader.Memory.LoaderMemory.End
; Holds the maximum of the location of the memory to be used by the bootloader
DivaOS.Loader.Memory.LoaderMemory.End: dq DivaOS.Loader.Memory.LoaderMemory.Memory.End

[section .loadermemory]

DivaOS.Loader.Memory.LoaderMemory.Memory.Start:
DivaOS.Loader.Memory.LoaderMemory.Memory.End equ 0x9FFFF