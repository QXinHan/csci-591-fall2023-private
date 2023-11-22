bits 64
default rel

section .data
format_lld_out db "%lld",0xa,0xd,0
format_lld_in db "%lld",0
format_d_out db "%d",0xa,0xd,0
format_d_in db "%d",0

section .bss
uninitailizedData1 resd 1
uninitailizedData2 resq 1

section .text
global main
extern printf
extern scanf
extern ExitProcess
main:
    push rbp
    mov rbp,rsp
    sub rsp,32

    xor rdx,rdx
    lea rcx,[format_d_in]
    lea rdx,[uninitailizedData1]
    call scanf

    xor rdx,rdx
    lea rcx,[format_lld_in]
    lea rdx,[uninitailizedData2]
    call scanf

    xor rax,rax
    mov eax,[uninitailizedData1]
    add rax,[uninitailizedData2]
    mov rdx,rax
    lea rcx,[format_lld_out]
    call printf

    xor rax,rax
    mov eax,[uninitailizedData1]
    sub rax,[uninitailizedData2]
    mov rdx,rax
    lea rcx,[format_lld_out]
    call printf


    xor rax,rax
    call ExitProcess