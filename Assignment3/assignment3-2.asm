; the program has a function to print out all ASCII
; character set.



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
   ;the decimal value from 32 to 126 can be printed out on the screen
loop_start:
    mov [temp],rcx
    push rcx
    
    push rbp
    mov rbp, rsp
    sub rsp, 32 ;need to allocate the stack space for the function "printf"
    ;print the ascii by "%c"
    mov rdx,qword [temp]
    mov rcx,ch_out
    call printf
    add rsp, 32
    pop rbp

    ;if the rcx is 32 then end the loop and the function
    cmp qword [temp],33
    je func_end

    pop rcx
    loop loop_start
    
func_end:
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