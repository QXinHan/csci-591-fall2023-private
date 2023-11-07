bits 64
default rel

segment .data
format_int db "%d"
format_float db "%f",0xa,0
format_double db "%lf",0xa,0
real_number1 dd 1.234 
real_number2 dq 123.456 
real_number3 dt 1.234567e20 

segment .text
global main
extern ExitProcess
extern printf

main:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 32

    lea     rcx,[format_float]
    cvtss2sd  xmm0,dword [real_number1] 
    movq    rdx,xmm0 
    call    printf
    
     lea     rcx,[format_double]
    movsd   xmm0,qword [real_number2]
    movq    rdx,xmm0
    call    printf
	
    xor rcx,rcx
    xor rdx,rdx
    fld tword [real_number3]
    fstp qword [real_number3]

    lea     rcx,[format_double]
    movsd   xmm0,qword [real_number3]
    movq    rdx,xmm0
    call    printf

    xor     rax, rax
    call    ExitProcess