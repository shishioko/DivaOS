[section .text]
[bits 16]

DivaOS.Loader.MemoryMap.Map:
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
    mov edi, [DivaOS.Loader.LoaderMemory.Offset]
    add edi, 4
    mov [DivaOS.Loader.MemoryMap.Start], edi
    DivaOS.Loader.MemoryMap.Map.Iterate:
        mov ecx, 20
        mov eax, 0xE820
        add edi, 4
        mov [edi-4], dword 0
        sti
        int 0x15
        cli
        jc DivaOS.Loader.MemoryMap.Map.Error 
        mov [edi-4], ecx
        add edi, ecx
        cmp ebx, 0
        jnz DivaOS.Loader.MemoryMap.Map.Iterate
    mov [dword DivaOS.Loader.LoaderMemory.Offset], edi

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
    DivaOS.Loader.MemoryMap.Map.Error:
        mov esi, dword DivaOS.Loader.MemoryMap.Message.Error
        jmp VBR.Code.Crash
    
[section .rodata]
DivaOS.Loader.MemoryMap.Message.Error: db "Error while mapping memory!", 0x00

[section .data]
global DivaOS.Loader.MemoryMap.Start
DivaOS.Loader.MemoryMap.Start: dq 0x000000000000