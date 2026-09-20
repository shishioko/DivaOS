%include "/VBR.asm"
%include "/Memory/Map.asm"

[section .text]
[bits 16]
; Entry point of the stage 2 bootloader
; Initializes and enters protected mode
DivaOS.Loader.Loader.Entry16:
    ;Initialize Stack
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0x7000

    ;Enable address bit 20
    mov dx, 0x0092
    in al, dx
    or al, 0x02
    out dx, al

    ;Check for protected mode support
    DivaOS.Loader.Loader.Entry16.CheckProtectedMode:
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
    jnz DivaOS.Loader.Loader.Entry16.MemoryMap
    ;Show error message otherwise
    mov esi, DivaOS.Loader.Loader.Message.NoProtectedMode
    jmp VBR.Code.Crash

    ;Map memory
    DivaOS.Loader.Loader.Entry16.MemoryMap:
    call DivaOS.Loader.Memory.Map.Build
    jmp DivaOS.Loader.Loader.Entry16.Load32

    ;Set up protected mode
    DivaOS.Loader.Loader.Entry16.Load32:
    lgdt [DivaOS.Loader.Loader.GDTR]
    mov eax, cr0
    or al, 0x01
    mov cr0, eax
    ;Enter protected mode
    jmp DivaOS.Loader.Loader.GDT.Code32:DivaOS.Loader.Loader.Entry32

[bits 32]
; Initializes and enters long mode
DivaOS.Loader.Loader.Entry32:
    ;Initialize execution
    mov ax, DivaOS.Loader.Loader.GDT.Data32
    mov ds, ax
    mov ss, ax
    mov gs, ax
    mov es, ax
    mov fs, ax

    ;Check for cpuid support
    DivaOS.Loader.Loader.Entry32.CheckCpuid:
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
    jnz DivaOS.Loader.Loader.Entry32.CheckLongMode
    ;Show error message otherwise
    mov esi, DivaOS.Loader.Loader.Message.NoCpuid
    jmp DivaOS.Loader.Loader.Entry32.Crash

    ;Check for long mode support
    DivaOS.Loader.Loader.Entry32.CheckLongMode:
    mov eax, 0x80000001
    cpuid
    bswap edx
    test dl, 0x20
    jnz DivaOS.Loader.Loader.Entry32.CheckMSR
    ;Show error message otherwise
    mov esi, DivaOS.Loader.Loader.Message.NoLongMode
    jmp DivaOS.Loader.Loader.Entry32.Crash

    ;Check for MSR support
    DivaOS.Loader.Loader.Entry32.CheckMSR:
    xor eax, eax
    inc eax
    cpuid 
    test dh, 0x20
    jnz DivaOS.Loader.Loader.Entry32.Load64
    ;Show error message otherwise
    mov esi, DivaOS.Loader.Loader.Message.NoMSR
    jmp DivaOS.Loader.Loader.Entry32.Crash

    ;Set up long mode
    DivaOS.Loader.Loader.Entry32.Load64:

    ;Enable PSA & PSE
    mov eax, cr4
    or al, 0x20
    or al, 0x10
    mov cr4, eax

    ;Set Page Table Structure Location
    mov edx, DivaOS.Loader.Loader.Paging.PML4
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
    jmp DivaOS.Loader.Loader.GDT.Code64:DivaOS.Loader.Loader.Entry64

    ; Prints an error message and halts execution
    ; Input: esi = Pointer to messaage to be printed
    DivaOS.Loader.Loader.Entry32.Crash:
        mov edi, 0x000B8000
        mov ecx, 80
        cld
        mov ah, 0x0F ;Set print color
        ;Iterate string content
        DivaOS.Loader.Loader.Entry32.Crash.Loop:
            lodsb
            stosw
            dec ecx
            cmp al, 0x00 ;Check for null terminator
            jnz DivaOS.Loader.Loader.Entry32.Crash.Loop
        ;Halt execution
        cli
        hlt
        jmp $
[bits 64]
; Initializes and enters C++ provided code
DivaOS.Loader.Loader.Entry64:
    ;Initialize registers
    mov ax, DivaOS.Loader.Loader.GDT.Data64
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    ;Call C++ code
    mov rbx, Main
    call rbx

    ;Print message in case of unexpected return
    lea rdi, [rel DivaOS.Loader.Loader.Message.LoaderReturn]
    call DivaOS.Loader.Peripherals.Terminal.Write_t8p

    ;Halt execution
    cli
    hlt
    jmp $

    extern Main
    extern DivaOS.Loader.Peripherals.Terminal.Write_t8p

[section .rodata]

; Baked identity mapped page tables for entering long mode
align 4096, db 0x00
DivaOS.Loader.Loader.Paging:
    DivaOS.Loader.Loader.Paging.PML4:
        dq DivaOS.Loader.Loader.Paging.PML3 + 00000011b ;R/W, Present
        times 511 dq 0x0000000000000000
    DivaOS.Loader.Loader.Paging.PML3:
        dq DivaOS.Loader.Loader.Paging.PML2 + 00000011b ;R/W, Present
        times 511 dq 0x0000000000000000
    DivaOS.Loader.Loader.Paging.PML2:
        dq DivaOS.Loader.Loader.Paging.PML1 + 00000011b ;R/W, Present
        times 511 dq 0x0000000000000000
    DivaOS.Loader.Loader.Paging.PML1:
        ;Could use 2MB Pages but that would mess with caching, apparently
        %push
        %assign Offset 0x0000000000000000
        %rep 512
            dq Offset + 00000011b ; R/W, Present
            %assign Offset Offset + 0x1000
        %endrep
        %pop
DivaOS.Loader.Loader.GDTR:
    dw (DivaOS.Loader.Loader.GDT.End - 1) ;Size including nulldescriptor - 1
    dq DivaOS.Loader.Loader.GDT ;Base Address including nulldescriptor
DivaOS.Loader.Loader.GDT:
    DivaOS.Loader.Loader.GDT.Null equ $ - DivaOS.Loader.Loader.GDT
        dq 0x0000000000000000
    DivaOS.Loader.Loader.GDT.Code32 equ $ - DivaOS.Loader.Loader.GDT
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10011010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1100 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    DivaOS.Loader.Loader.GDT.Data32 equ $ - DivaOS.Loader.Loader.GDT
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10010010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1100 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    DivaOS.Loader.Loader.GDT.Code64 equ $ - DivaOS.Loader.Loader.GDT
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10011010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1010 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______

    DivaOS.Loader.Loader.GDT.Data64 equ $ - DivaOS.Loader.Loader.GDT
        dw 0xFFFF ;Size: 0x_0000
        dw 0x0000 ;Start: 0x____0000
        db 0x00 ;Start: 0x__00____
        db 0b10010010 ;Permissions and Type: Present, Privileges(2), NotSystem, Type(Ex, Di, RW), Accessed
        db 0xF + (0b1010 << 4) ;Size: 0x0____ and Additional: Size*=(0=1,1=4096), ProtectedMode, LongMode, Reserved
        db 0x00 ;Start: 0x00______
    DivaOS.Loader.Loader.GDT.End equ $ - DivaOS.Loader.Loader.GDT

DivaOS.Loader.Loader.Message.NoProtectedMode: db "Protected mode not supported!", 0x00
DivaOS.Loader.Loader.Message.NoCpuid: db "CPUID not supported!", 0x00
DivaOS.Loader.Loader.Message.NoLongMode: db "Long mode not supported!", 0x00
DivaOS.Loader.Loader.Message.NoMSR: db "MSRs not supported!", 0x00
DivaOS.Loader.Loader.Message.LoaderReturn: db "Loader exited!", 0x00