[section .data]

global DivaOS.Loader.Memory.LoaderMemory.Start
DivaOS.Loader.Memory.LoaderMemory.Start: dq DivaOS.Loader.Memory.LoaderMemory.Memory.Start
global DivaOS.Loader.Memory.LoaderMemory.Offset
DivaOS.Loader.Memory.LoaderMemory.Offset: dq DivaOS.Loader.Memory.LoaderMemory.Memory.Start
global DivaOS.Loader.Memory.LoaderMemory.End
DivaOS.Loader.Memory.LoaderMemory.End: dq DivaOS.Loader.Memory.LoaderMemory.Memory.End

[section .loadermemory]

DivaOS.Loader.Memory.LoaderMemory.Memory.Start:
DivaOS.Loader.Memory.LoaderMemory.Memory.End equ 0x9FFFF