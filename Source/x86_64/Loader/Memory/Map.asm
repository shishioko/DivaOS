%include "/Memory/LoaderMemory.asm"

[section .text]
[bits 16]

; Fetches and saves the E820 memory map
DivaOS.Loader.Memory.Map.Build:
    ;Save registers
    pushf
    push eax
    push ebx
    push ecx
    push edx
    push edi
    ;Return if memory map has already been fetched
    cmp [DivaOS.Loader.Memory.Map.Start], 0
    jnz DivaOS.Loader.Memory.Map.Build.Return
    ;Initialize registers for interrupt
    xor ax, ax
    mov es, ax
    xor ebx, ebx
    mov edx, dword 0x534D4150
    mov edi, [DivaOS.Loader.Memory.LoaderMemory.Offset]
    ;Save pointer to first entry
    mov [DivaOS.Loader.Memory.Map.Start], edi
    DivaOS.Loader.Memory.Map.Build.Iterate:\
        ;Set registers for next interrupt iteration
        mov ecx, 24
        mov eax, 0xE820
        ;Trigger interrupt
        sti
        int 0x15
        cli
        ;Crash on error
        jc DivaOS.Loader.Memory.Map.Build.Error 
        ;Offset destination pointer for next entry
        add edi, 24
        ;Count entries
        inc [DivaOS.Loader.Memory.Map.Length]
        ;Return if this was the last entry
        cmp ebx, 0
        jnz DivaOS.Loader.Memory.Map.Build.Iterate
    ;Update LoaderMemory offset
    mov [dword DivaOS.Loader.Memory.LoaderMemory.Offset], edi
    DivaOS.Loader.Memory.Map.Build.Return:
    ;Restore registers
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popf
    ret
    DivaOS.Loader.Memory.Map.Build.Error:
        mov esi, dword DivaOS.Loader.Memory.Map.Message.Error
        jmp VBR.Code.Crash
    
[section .rodata]
DivaOS.Loader.Memory.Map.Message.Error: db "Error while mapping memory!", 0x00

[section .data]
global DivaOS.Loader.Memory.Map.Length
; Holds the amount of entries in the fetched memory map
DivaOS.Loader.Memory.Map.Length: dq 0x000000000000
global DivaOS.Loader.Memory.Map.Start
; Holds the pointer to the first entry of the fetched memory map
DivaOS.Loader.Memory.Map.Start: dq 0x000000000000