ENTRY(_start)

SECTIONS
{
    . = 0x7C00;
    .mbr : {
        *(.MBR)
    }

    .loader 0x8000 : AT(0x7C00 + 0x200) {
        *(.Loader)
    }

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }

    .padding : {
        . = 0x80000 - 1; 
        BYTE(0);
        }
}