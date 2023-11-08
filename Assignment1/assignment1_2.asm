bits 64 
default rel

segment .data

format_int_in db "%d",0
format_int_out db "%d",0xa,0
format_t_in db "%Lf",0
format_t_out db "%Lf",0xa,0
format_long_in db "%lld",0
format_long_out db "%lld",0xa,0
num1 resw 1
num2 resd 1
num3 resq 1
num4 rest 1


segment .text
global main
extern printf
extern scanf
extern ExitProcess

main:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    xor rdx,rdx
    lea rdx,[num1]
    lea rcx,[format_int_in]
    call scanf

    xor rdx,rdx
    lea rdx,[num2]
    lea rcx,[format_int_in]
    call scanf

    xor rdx,rdx
    lea rdx,[num3]
    lea rcx,[format_long_in]
    call scanf

    xor rdx,rdx
    lea rdx,[num4]
    lea rcx,[format_t_in]
    call scanf

    xor rdx,rdx
    lea rcx,[format_int_out]
    movzx rdx,word [num1]           
    call printf

    xor rdx,rdx
    lea rcx,[format_int_out]
    mov rdx,[num2] 
    call printf

    xor rdx,rdx
    lea rcx,[format_long_out]
    mov rdx,[num3]
    call printf

    xor rdx,rdx
    lea rcx,[format_t_out]
    mov rdx,[num4]
    call printf

    xor rax,rax
    call ExitProcess