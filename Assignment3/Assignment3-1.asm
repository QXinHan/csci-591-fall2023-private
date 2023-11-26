;x64里面没有loop这个指令好像
bits 64
default rel

section .data
    msg db "Hello,world!",0
    d_out db "%d",0xa,0

section .text
global main
extern ExitProcess
extern printf


function:
    ;store the stack information
    push rbp
    mov rbp,rsp
    sub rsp,32
    ;msg start address to the rsi
    mov rsi,msg
    ;use a loop to traverse the string
    xor rcx,rcx
    xor rdx,rdx
    mov rcx,100000000
    ;the max length of the string is 2^32-1 about 2.1e9,may be the 1e9 is ok
loop_start:
    cmp byte [rsi],0;if it is the null terminator then jump to loop_end
    je loop_end
    add rdx,1
    add rsi,1
        loop loop_start

loop_end:
   lea rcx,[d_out]
   mov rdx,rdx
   call printf
    ;return to the main function
    xor     rax, rax
    leave
    ret

;the main function
main:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    call function

    xor rax,rax
    call ExitProcess