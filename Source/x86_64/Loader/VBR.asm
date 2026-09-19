[section .vbr]
[bits 16]
VBR:
VBR.BPB:
    jmp short VBR.Code
    nop
    times 87 db 0x00
VBR.Code:
    ;Reset some registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov di, ax
    
    ;Save origin partition sector
    mov ax, [si + 8] 
    mov [VBR.Code.Sector], ax
    mov ax, [si + 10] 
    mov [VBR.Code.Sector + 2], ax
    ;Save boot disk id
    mov [VBR.Code.Disk], dl

    sti ;Enable interrupts

    ;Set video mode
    mov ax, 0x0003
    int 0x10
    ;Disable cursor
    mov dx, 0x03D4
    mov al, 0x0A
    out dx, al
    mov dx, 0x03D5
    mov al, 0x20
    out dx, al

    ;Load loader (127 sectors / 64KiB to 0x08000)
    VBR.Code.LoadLoader:
    mov eax, [VBR.Code.Sector] 
    add dword [VBR.DiskAddressPacket.Loader + 8], eax
    adc word [VBR.DiskAddressPacket.Loader + 12], 0
    adc word [VBR.DiskAddressPacket.Loader + 14], 0
    mov dl, [VBR.Code.Disk]
    mov si, VBR.DiskAddressPacket.Loader
    mov ah, 0x42
    int 0x13
    jnc VBR.Code.Load16
    ;Show error message otherwise
    mov si, VBR.Code.Message.DiskReadFail
    jmp VBR.Code.Crash
    VBR.Code.Load16:
    cli ;Disable interrupts
    ;Execute bootloader if successful
    jnc VBR.Code.RunBootloader
    ;Show error message otherwise
    mov si, VBR.Code.Message.NoDisk
    jmp VBR.Code.Crash
    VBR.Code.RunBootloader:
    jmp 0x0000:DivaOS.Loader.Loader.Entry16

    ; Prints an error message and halts execution
    ; Input: si = Pointer to messaage to be printed
    VBR.Code.Crash: 
        mov ax, 0xB800
        mov es, ax
        xor ah, ah
        mov di, ax
        cld
        mov ah, 0x0F ;Set print color
        ;Iterate string content
        VBR.Code.Crash.Loop:
            lodsb
            stosw
            cmp al, 0x00 ;Check for null terminator
            jnz VBR.Code.Crash.Loop
        ;Halt execution
        cli
        hlt
        jmp $

    ; Used to store the disk the program is on, for BIOS interrupt purposes
    VBR.Code.Disk: db 0x00
    ; Used to offset the program is at, for BIOS interrupt purposes
    VBR.Code.Sector: dd 0x00000000

    VBR.DiskAddressPacket.Loader:
        db 0x10
        db 0x00
        dw 0x007F ;Sector Amount ;TODO: make sure loader is actually 64kb max
        dw 0x0000 ;Memory Destination Offset
        dw 0x07E0 ;Memory Destination Segment
        dq 0x0000000000000003 ;Disk Source

    VBR.Code.Message.NoDisk: db "Unable to load bootloader!", 0x00
    VBR.Code.Message.DiskReadFail: db "Disk read failed!", 0x00
times 0x01FE - ($ - VBR) db 00
    ;Boot flag
    db 0x55, 0xAA