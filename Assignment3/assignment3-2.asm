bits 64
default rel

section .data
ch_out db "%c ",0


section .text
global main
extern printf
extern ExitProcess

function:
    push rbp
    mov rbp,rsp
    sub rsp,32

    mov rcx,32
loop_start:
    push rcx

    mov rdx,rcx
    lea rcx,[ch_out]
    call printf

    pop rcx
    add rcx

    cmp rcx,127
    je func_end

loop loop_start

func_end:
    ret

main:
    push rbp
    mov rbp,rsp
    sub rsp,32

    call function

    xor rax,rax
    call ExitProcess