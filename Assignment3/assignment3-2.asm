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

;this function is used to print the set of ascii
function:
    push rbp
    mov rbp,rsp
    sub rsp,32

    mov rcx,126
   ;the decimal value from 32 to 126 can display on the screen
loop_start:
    mov [temp],rcx
    push rcx
    ;print the ascii by "%c"
    mov rdx,qword [temp]
    mov rcx,ch_out
    call printf
    ;if the rcx is 32 then end the loop and the function
    cmp qword [temp],32
    je func_end

    pop rcx

loop loop_start

func_end:
    xor rax,rax
    ret

main:
    push rbp
    mov rbp,rsp
    sub rsp,32

    ;call the print function
    call function
    
    ;the program exit
    xor rax,rax
    call ExitProcess