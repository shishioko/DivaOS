ENTRY(_start)

SECTIONS
{
    . = 0x7C00;
    
    .vbr : {
        *(.vbr)
    }
    .text : {
        *(.Loader)
    }
    .rodata : {
        *(.Loader)
    }
    .data : {
        *(.Loader)
    }
    .bss : {
        *(.Loader)
    }

    .padding : 
    {
        . = 0x8000 + 0x10000 - 1; 
        BYTE(0);
    }
}