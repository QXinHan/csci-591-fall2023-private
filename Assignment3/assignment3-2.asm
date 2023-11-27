bits 64
default rel

section .data
ch_out db "%c ",0

section .bss
temp resq 1
section .text
global main
extern printf
extern ExitProcess


function:
    push rbp
    mov rbp,rsp
    sub rsp,32

    mov rcx,126
   
loop_start:
    mov [temp],rcx
    push rcx

    mov rdx,qword [temp]
    mov rcx,ch_out
    call printf

    cmp qword [temp],32
    je func_end

    pop rcx

loop loop_start

jmp func_end

func_end:
    ret

main:
    push rbp
    mov rbp,rsp
    sub rsp,32

    call function

    xor rax,rax
    call ExitProcess