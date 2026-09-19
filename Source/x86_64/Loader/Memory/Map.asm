%include "/Memory/LoaderMemory.asm"

[section .text]
[bits 16]

DivaOS.Loader.Memory.Map.Build:
    pushf
    push eax
    push ebx
    push ecx
    push edx
    push edi

    xor ax, ax
    mov es, ax
    xor ebx, ebx
    mov edx, dword 0x534D4150
    mov edi, [DivaOS.Loader.Memory.LoaderMemory.Offset]
    add edi, 4
    mov [DivaOS.Loader.Memory.Map.Start], edi
    DivaOS.Loader.Memory.Build.Iterate:
        mov ecx, 20
        mov eax, 0xE820
        add edi, 8
        mov [edi-8], dword 0
        mov [edi-4], dword 0
        sti
        int 0x15
        cli
        jc DivaOS.Loader.Memory.Build.Error 
        mov [edi-4], ecx
        add edi, ecx
        cmp ebx, 0
        jnz DivaOS.Loader.Memory.Build.Iterate
    mov [dword DivaOS.Loader.Memory.LoaderMemory.Offset], edi

    ;todo: make a table of pointers (exclude unusable and current area)
    ;todo: sort that table
    ;todo: shrink segments if they overlap with restrictive ones\
    ;todo: shrimk segments to page boundaries

    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popf
    ret
    DivaOS.Loader.Memory.Build.Error:
        mov esi, dword DivaOS.Loader.Memory.Map.Message.Error
        jmp VBR.Code.Crash
    
[section .rodata]
DivaOS.Loader.Memory.Map.Message.Error: db "Error while mapping memory!", 0x00