#include <asm/unistd_64.h>
    .intel_syntax noprefix
    .global _start 
    .text


_start:
    push r12
    push r13 
    push r14
    push r15

    mov	  rax, __NR_brk // - номер brk
    mov	  rdi, 0 //
    syscall 

    mov	  rbx, rax // - start addr
    //add   rbx, 100 

    //mov	  rax, __NR_brk
    //mov	  rdi, rbx	
    //syscall
 
    //mov   rbx, rax 
    //sub   rbx, 100

    //sub   rsp, 100
    //mov   rbx, rsp 
    mov    r13, 1 // max_size = 1
    xor	   r12, r12

//////////////////////////////////////
//// Allocatioin 
    add    rbx, r13  
    
    mov	   rax, __NR_brk
    mov    rdi, rbx
    syscall

    mov    rbx, rax
    sub    rbx, r13 
 
    mov    r14, r13

    jmp loopReadBegin
   
////////////////////////////////////////
    Realloc:

    add    rbx, r13  
    
    mov	   rax, __NR_brk
    mov    rdi, rbx
    syscall

    mov    rbx, rax
    sub    rbx, r13 
 
    mov    r14, r13

    add    r13, r13

    jmp loopReadBegin
////////////////////////////////////////


/////////////////////////////////////////
    loopReadBegin: 

    mov   eax, __NR_read // - номер read
    mov   rdi, 0 // 0 - файл дескриптор stdin
    mov   rsi, rbx // - указатель на buff
    mov	  rdx, r14 // - сколько символов считать надо
    syscall

    cmp   rax, 0 // если все считали выходим
    je loopReadEnd

   
    add   rbx, rax // ptr_to_read += сколько считалт
    add   r12, rax // size += кол-во считанных
    sub   r14, rax // считать надо max_size - curr_size = size - delta

    cmp   r12, r13 // if (size == max_size)
    je    Realloc 

    jmp loopReadBegin
/////////////////////////////////////////

    loopReadEnd:

    mov   r15, r12
    sub   r15, 0
////////////////////////////////////////
    mov   r11, rbx
    sub    r11, 1
    findEnter:
    movb   al, [rbx]

    cmp	   al, 10
    je EnterExist

    cmp    r15, 0
    je printString

    sub    rbx, 1
    sub    r15, 1

    jmp findEnter

    EnterExist:
    mov    r13, rbx // запоминаем позицию

    cmp    r11, r13 
    jne startFromLast
    
    sub    rbx, 1
    sub    r15, 1
    sub    r12, 1

    startFromLast:
////////////////////////////////////////
    loopIter:
    movb   al, [rbx]

    cmp	   al, 10
    je printString

    cmp r15, 0
    je printStringWithEnter

    printStringRet:        
 
    cmp r12, 0
    je progEnd

    sub    rbx, 1
    sub    r15, 1

    jmp loopIter
//////////////////////////////////////// 
    

    printString:
    // сколько нужно вывести:
    mov   rdx, r12
    sub   rdx, r15
    mov   rsi, rbx // - указатель на buff

    printLoopBegin:

    mov   rax, __NR_write // - номер write 
    mov   rdi, 1 // 0 - файл дескриптор stdout
    syscall

    cmp rax, 0 // если уже все вывели
    je printLoopEnd
  
    add   rsi, rax // указатель на читаемый последний символ
    sub   rdx, rax // осталось считать 
    jmp printLoopBegin

    printLoopEnd: 
  
    mov   r12, r15

    jmp printStringRet

///////////////////////////////////////

    printStringWithEnter:
    mov   rdx, 1
    mov   rsi, r13
    mov   rax, __NR_write 
    mov   rdi, 1
    syscall
    jmp printString

////////////////////////////////////////


    progEnd:
    pop   r15
    pop   r14
    pop   r13
    pop   r12
    mov	  eax, __NR_exit
    mov	  edi, 0 
    syscall
