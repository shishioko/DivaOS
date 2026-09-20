ENTRY(_start)

SECTIONS
{
    . = 0xFFFFFFFF80000000;

    . = ALIGN(0x1000); 
    .text : {
        *(.text)
        *(.text.*)
    }

    . = ALIGN(0x1000); 
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }

    . = ALIGN(0x1000); 
    .data : {
        *(.data)
        *(.data.*)
    }

    . = ALIGN(0x1000); 
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    . = ALIGN(0x1000); 
    .eh_frame : {
        *(.eh_frame)
    }
}