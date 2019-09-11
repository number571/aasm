# AASM - Abstract Assembler.
> Use one assembly system calls for different OS's.

> AASM use dialect FASM and translated into the FASM.

#### Arguments:
1. [-i, --input] = input assembly file
2. [-o, --output] = output assembly file

#### AASM extensions:
1. Register: !ax
2. System call: !syscall

#### Supported instructions:
1. mov

#### Supported operations:
1. 10 = exit.
2. 41 = write stdout.

#### AASM code:
```asm
section '.data' writeable
    msg db "hello, world!", 0xA, 0
    len = $-msg

section '.text' executable
_start:
    mov !ax, 41
    push msg
    push len
    !syscall

    mov !ax, 10
    !syscall
```

#### Translation:
```
$ make compile
> gcc aasm.c -o aasm
$ make
> ./aasm -i main.asm
> fasm out.asm
> ld out.o -o out
> ./out
> hello, world!
```

#### FASM code (result):
```asm
format ELF64
public _start

section '.data' writeable
    msg db "hello, world!", 0xA, 0
    len = $-msg

section '.text' executable
_start:
    push rax
    push rbx
    mov eax, 4
    mov ebx, 1
    push msg
    push len
    pop rdx
    pop rcx
    int 0x80
    pop rbx
    pop rax

    push rax
    push rbx
    mov eax, 1
    xor ebx, ebx
    int 0x80
    pop rbx
    pop rax
```

[ AASM from #571 ]
