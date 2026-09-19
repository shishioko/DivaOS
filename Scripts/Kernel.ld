ENTRY(_start)

SECTIONS
{
    . = 0xFFFFFFFF80000000;

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
    .eh_frame : {
        *(.eh_frame)
    }
}