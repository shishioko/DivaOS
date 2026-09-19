ENTRY(_start)

SECTIONS
{
    . = 0x7C00;
    
    .vbr : {
        *(.vbr)
        *(.vbr.*)
    }
    .text : {
        *(.text)
        *(.text.*)
    }
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    .data : {
        *(.data)
        *(.data.*)
    }
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    .bss.loadermemory : {
        *(.bss.loadermemory)
    }

    .padding : 
    {
        . = 0x8000 + 0x10000 - 1; 
        BYTE(0);
    }
}