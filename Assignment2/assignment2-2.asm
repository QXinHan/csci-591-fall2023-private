bits 64
default rel

section .data
format_lld_out db "%lld",0xa,0xd,0
format_lld_in db "%lld",0
less_than db "Comparison result: -1",0
greater_than db "Comparison result: 1",0
equal_with db "Comparison result: 0",0
section .bss
num1 resq 1
num2 resq 1

section .text
global main
extern scanf
extern printf
extern ExitProcess
main:
    push rbp
    mov rbp,rsp
    sub rsp,32

    lea rcx,[format_lld_in]
    lea rdx,[num1]
    call scanf

    lea rcx,[format_lld_in]
    lea rdx,[num2]
    call scanf

    mov rax,[num1]
    cmp rax,[num2] ;can not compare register with register!!!
    je equal
    jl less

    lea rcx,[greater_than]
    call printf

    xor rax,rax
    call ExitProcess

equal:
 lea rcx,[equal_with]
 call printf
 xor rax,rax
 call ExitProcess
 
 less:
 lea rcx,[less_than]
 call printf
 xor rax,rax
 call ExitProcess
