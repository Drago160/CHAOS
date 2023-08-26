#include <asm/unistd_64.h>
    .intel_syntax noprefix
    .global _start 
    .text

_start:
    push r12
    sub rsp, 100 

    loopBegin: 
    mov   eax, __NR_read // - номер read
    mov   edi, 0 // 0 - файл дескриптор stdin
    mov   rsi, rsp // - указатель на buff
    mov   edx, 100 
    syscall

    cmp   rax, 0	
    je loopEnd
    
    xor   rdx, rdx

    mov   r12, rax 
    mov   rsi, rsp // указатель на buff 
    WriteBegin:

    mov   eax, __NR_write  // - номер write
    mov   edi, 1  // 1 - файловый дескриптор stdout

    mov   rdx, r12  // количество байт в выводе
    syscall       // системный вызов Linux
    
    add   rsi, rax
    sub	  r12, rax 

    cmp   r12, 0
    jne WriteBegin
    WriteEnd:
    

    jmp loopBegin

    loopEnd:

    pop r12
    add rsp, 100
    mov	  eax, __NR_exit
    mov	  edi, 0 
    syscall
    .data
hello:
    .string "Hello"
hello_ptr:
    .quad   hello
