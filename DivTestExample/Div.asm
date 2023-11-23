;correctly print

bits 64
 default rel

section .data
ff db "%d %d",0xd,0xa,0

 section .text
global main
extern ExitProcess
extern printf
extern _CRT_INIT

main:
    push rbp
    mov rbp,rsp
    sub rsp,32
    
    mov rax,15
    mov rbx,3
    xor rdx,rdx
    div rbx
    lea rcx,[ff]
    mov r8,rdx
    mov rdx,rax
    call printf


    xor rax,rax
    call ExitProcess
    