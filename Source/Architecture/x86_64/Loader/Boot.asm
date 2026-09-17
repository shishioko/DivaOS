[segment MBR start=0x0000000000000000 vstart=0x0000000000007C00]
[bits 16]
MBR:
MBR.Code:
    ;Reset some registers
    xor ax, ax
    mov ds, ax
    mov di, ax
    mov [MBR.Code.Disk], dl ;Save bootdisk id
    sti ;Enable interrupts
    ;Set video mode
    mov ax, 0x0003
    int 0x10
    ;Load kernel (127 sectors / 64KiB to 0x8000)
    mov ax, 0x0800
    mov es, ax
    mov ax, 0x027F
    mov bx, cx
    mov cl, 0x02
    int 0x13
    cli ;Disable interrupts
    ;Execute bootloader if successful
    jnc MBR.Code.RunBootloader
    ;Show error message otherwise
    mov si, MBR.Code.Message.NoDisk
    jmp MBR.Code.Error
    MBR.Code.RunBootloader:
    jmp 0x0000:Loader.Entry16

    MBR.Code.Error: 
        mov ax, 0xB800
        mov es, ax
        xor ah, ah
        mov di, ax
        cld
        lodsb
        mov ah, al
        MBR.Code.Error.Loop:
            lodsb
            stosw
            cmp al, 0x00
            jnz MBR.Code.Error.Loop
        cli
        MBR.Code.Error.Halt:
            hlt
        jmp MBR.Code.Error.Halt

    MBR.Code.Disk: db 0x00

    MBR.Code.Message.NoDisk: db 0x0F, "Unable to load bootloader!", 0x00
times 0x01BE - ($ - MBR.Code) db 00
MBR.Partitions:
    db 0x80 ;Bootable
    db 0x00 ;Starting Head
    dw 0x01 + (0x0000 << 0x06) ;Starting Sector and Cylinder
    db 0xD3 ;System ID = ETFS (ESPIOS Tree File System)
    db 0x00 ;Ending Head
    dw 0x02 + (0x0000 << 0x06) ;Ending Sector and Cylinder
    dd 0x00000001 ;LBA Start (?)
    dd 0x00000001 ;Total Sectors

    db 0x00 ;Not Bootable
    db 0x00 ;Starting Head
    dw 0x00 + (0x0000 << 0x06) ;Starting Sector and Cylinder
    db 0x00 ;System ID
    db 0x00 ;Ending Head
    dw 0x00 + (0x0000 << 0x06) ;Ending Sector and Cylinder
    dd 0x00000000 ;LBA Start (?)
    dd 0x00000000 ;Total Sectors

    db 0x00 ;Not Bootable
    db 0x00 ;Starting Head
    dw 0x00 + (0x0000 << 0x06) ;Starting Sector and Cylinder
    db 0x00 ;System ID
    db 0x00 ;Ending Head
    dw 0x00 + (0x0000 << 0x06) ;Ending Sector and Cylinder
    dd 0x00000000 ;LBA Start (?)
    dd 0x00000000 ;Total Sectors

    db 0x00 ;Not Bootable
    db 0x00 ;Starting Head
    dw 0x00 + (0x0000 << 0x06) ;Starting Sector and Cylinder
    db 0x00 ;System ID
    db 0x00 ;Ending Head
    dw 0x00 + (0x0000 << 0x06) ;Ending Sector and Cylinder
    dd 0x00000000 ;LBA Start (?)
    dd 0x00000000 ;Total Sectors

    ;Boot flag
    db 0x55, 0xAA

[segment Loader start=0x0000000000000200 vstart=0x0000000000008000]
[bits 16]
Loader.Entry16:
    ;Enable address bit 20
    mov dx, 0x0092
    in al, dx
    or al, 0x02
    out dx, al

    ;Check for protected mode support
    Loader.Entry16.CheckProtectedMode:
    pushf
    pushf
    pop ax
    xor ah, 0xF0
    push ax
    popf
    pushf
    pop ax
    pop bx
    push bx
    xor ah, bh
    popf
    and ah, 0xF0
    test ah, ah
    ;Show error message otherwise
    jnz Loader.Entry16.Load32
    mov si, Loader.Message.NoProtectedMode
    jmp MBR.Code.Error

    ;Set up protected mode
    Loader.Entry16.Load32:
    lgdt [Loader.GDTR]
    mov eax, cr0
    or al, 0x01
    mov cr0, eax
    ;Enter protected mode
    jmp Loader.GDTR.Code32:Loader.Entry32

[bits 32]
%include "Loader/Terminal32.asm"
Loader.Entry32:
    ;Initialize execution
    mov esp, 0x00007000
    mov ax, Loader.GDTR.Data32
    mov ds, ax
    mov ss, ax
    mov gs, ax
    mov es, ax
    mov fs, ax

    ;Check for cpuid support
    Loader.Entry32.CheckCpuid:
    pushfd
    pushfd
    xor dword [ss:esp], 0x00200000 
    popfd
    pushfd
    pop eax
    xor eax, [ss:esp]
    popfd
    and eax,0x00200000
    test eax, eax
    jnz Loader.Entry32.CheckLongMode
    ;Show error message otherwise
    mov esi, Loader.Message.NoCpuid
    jmp Loader.Terminal32.Crash

    ;Check for long mode support
    Loader.Entry32.CheckLongMode:
    mov eax, 0x80000001
    cpuid
    bswap edx
    test dl, 0x20
    jnz Loader.Entry32.CheckMSR
    ;Show error message otherwise
    mov esi, Loader.Message.NoLongMode
    jmp Loader.Terminal32.Crash

    ;Check for MSR support
    Loader.Entry32.CheckMSR:
    xor eax, eax
    inc eax
    cpuid 
    test dh, 0x20
    jnz Loader.Entry32.Load64
    ;Show error message otherwise
    jmp Loader.Terminal32.Crash
    mov esi, Loader.Message.NoMSR

    ;Set up long mode
    Loader.Entry32.Load64:

    ;Enable PSA & PSE
    mov eax, cr4
    or al, 0x20
    or al, 0x10
    mov cr4, eax

    ;Set Page Table Structure Location
    mov edx, Loader.Paging.PML4
    mov cr3, edx

    ;Enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or ah, 0x01 
    wrmsr 

    ;TODO: set CR4.LA57 if applicable

    ;Enable paging
    mov eax, cr0 
    or eax, 0x80000000
    mov cr0, eax

    ;Enter long mode
    jmp Loader.GDTR.Code64:Loader.Entry64

[bits 64]
Loader.Entry64:
    mov ax, Loader.GDTR.Data64
    mov ds, ax

    mov al, "F"
    mov ah, 0x0F
    mov [0x00000000000B8000], ax
    jmp $

align 4096, db 0x00

Loader.Paging.PML4:
    dq Loader.Paging.PML3 + 00000011b ;R/W, Present
    times 511 dq 0x0000000000000000
Loader.Paging.PML3:
    dq Loader.Paging.PML2 + 00000011b ;R/W, Present
    times 511 dq 0x0000000000000000
Loader.Paging.PML2:
    dq 0x0000000000000000 + 10000011b ;2MB Pages, R/W, Present
    times 511 dq 0x0000000000000000

Loader.GDTR:
    dw 0x003f ;Size including nulldescriptor - 1
    dq $ ;Base Address including nulldescriptor

    Loader.GDTR.Code32 equ $ - Loader.GDTR - 2
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10011010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1100 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    Loader.GDTR.Data32 equ $ - Loader.GDTR - 2
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10010010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1100 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    Loader.GDTR.Code64 equ $ - Loader.GDTR - 2
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10011010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1110 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    Loader.GDTR.Data64 equ $ - Loader.GDTR - 2
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10010010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1110 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

Loader.Message.NoProtectedMode: db 0x0F, "Protected mode not supported!", 0x00
Loader.Message.NoCpuid: db 0x0F, "CPUID not supported!", 0x00
Loader.Message.NoLongMode: db 0x0F, "Long mode not supported!", 0x00
Loader.Message.NoMSR: db 0x0F, "MSRs not supported!", 0x00

Loader.Padding:
times 0x80000-0x200-($-$$) db 0x00 ;Padding