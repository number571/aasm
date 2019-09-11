section '.data' writeable
    msg db "hello, world!", 0xA, 0
    len = $-msg

section '.text' executable
_start:
    mov !ax, 41
    mov ecx, msg
    mov edx, len
    !syscall

    mov !ax, 10
    !syscall
