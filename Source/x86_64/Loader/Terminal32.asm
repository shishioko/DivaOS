[bits 32]

Loader.Terminal32.Print: ;[esi: stringStart] -> Print -> [esi: stringEnd]
    pushfd
    push eax
    push ecx
    push edi
    mov edi, [Loader.Terminal32.Address]
    mov ecx, 80
    cld
    lodsb
    mov ah, al
    Loader.Terminal32.Print.Loop:
        lodsb
        stosw
        dec ecx
        cmp al, 0x00
        jnz Loader.Terminal32.Print.Loop
    add edi, ecx
    add edi, ecx
    mov [Loader.Terminal32.Address], edi
    pop edi
    pop ecx
    pop eax
    popfd
    ret

Loader.Terminal32.Crash: ;[esi: stringStart] -> Print -> []
    call Loader.Terminal32.Print
    cli
    Loader.Terminal32.Crash.Loop:
    hlt
    jmp Loader.Terminal32.Crash.Loop

Loader.Terminal32.Address: dd 0x000B8000