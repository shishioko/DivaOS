%include "/VBR.asm"
[section .text]
[bits 16]
Loader.Entry16:
    ;Enable address bit 20
    mov dx, 0x0092
    in al, dx
    or al, 0x02
    out dx, al

    ;extern TestPrint
    ;call TestPrint

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
    jnz Loader.Entry16.Load32
    ;Show error message otherwise
    mov esi, Loader.Message.NoProtectedMode
    jmp VBR.Code.Crash

    ;Set up protected mode
    Loader.Entry16.Load32:
    lgdt [Loader.GDTR]
    mov eax, cr0
    or al, 0x01
    mov cr0, eax
    ;Enter protected mode
    jmp Loader.GDTR.Code32:Loader.Entry32

[bits 32]
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
    jmp Loader.Entry32.Crash

    ;Check for long mode support
    Loader.Entry32.CheckLongMode:
    mov eax, 0x80000001
    cpuid
    bswap edx
    test dl, 0x20
    jnz Loader.Entry32.CheckMSR
    ;Show error message otherwise
    mov esi, Loader.Message.NoLongMode
    jmp Loader.Entry32.Crash

    ;Check for MSR support
    Loader.Entry32.CheckMSR:
    xor eax, eax
    inc eax
    cpuid 
    test dh, 0x20
    jnz Loader.Entry32.Load64
    ;Show error message otherwise
    mov esi, Loader.Message.NoMSR
    jmp Loader.Entry32.Crash

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

    Loader.Entry32.Crash:
        mov edi, 0x000B8000
        mov ecx, 80
        cld
        mov ah, 0x0F
        Loader.Entry32.Print.Loop:
            lodsb
            stosw
            dec ecx
            cmp al, 0x00
            jnz Loader.Entry32.Print.Loop
        add edi, ecx
        add edi, ecx
        cli
        hlt
        jmp $
[bits 64]
Loader.Entry64:
    ;Initialize registers
    ;mov rsp, 0x00007000
    mov ax, Loader.GDTR.Data64
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    mov rbx, Main
    call rbx

    lea rdi, [rel Loader.Message.NoProtectedMode]
    call DivaOS.Loader.Terminal.Write_t8p

    jmp $

    extern Main
    extern DivaOS.Loader.Terminal.Write_t8p

[section .rodata]

align 4096, db 0x00

Loader.Paging.PML4:
    dq Loader.Paging.PML3.Low + 00000011b ;R/W, Present
    times 510 dq 0x0000000000000000
    dq Loader.Paging.PML3.High + 00000011b ;R/W, Present
Loader.Paging.PML3.Low:
    dq Loader.Paging.PML2.Low + 00000011b ;R/W, Present
    times 511 dq 0x0000000000000000
Loader.Paging.PML2.Low:
    dq 0x0000000000000000 + 10000011b ;2MB Pages, R/W, Present
    times 511 dq 0x0000000000000000
Loader.Paging.PML3.High:
    times 510 dq 0x0000000000000000
    dq Loader.Paging.PML2.High + 00000011b ;R/W, Present
    times 1 dq 0x0000000000000000
Loader.Paging.PML2.High:
    dq Loader.Paging.PML1.High + 00000011b ;R/W, Present
    times 511 dq 0x0000000000000000
Loader.Paging.PML1.High:
    %push
    %assign Offset 0x0000000000018000
    %rep 512
        dq Offset + 00000011b ; R/W, Present
        %assign Offset Offset + 0x1000
    %endrep
    %pop

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

Loader.Message.NoProtectedMode: db "Protected mode not supported!", 0x00
Loader.Message.NoCpuid: db "CPUID not supported!", 0x00
Loader.Message.NoLongMode: db "Long mode not supported!", 0x00
Loader.Message.NoMSR: db "MSRs not supported!", 0x00