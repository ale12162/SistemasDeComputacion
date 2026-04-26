section .text

global asm_float_to_int
global asm_gini_index

; int asm_float_to_int(float val)
; val llega en xmm0 (convencion System V AMD64)
; retorno en rax
asm_float_to_int:
    push    rbp
    mov     rbp, rsp

    cvttss2si rax, xmm0     ; convierte float -> int (truncando)

    pop     rbp
    ret

; int asm_gini_index(float val)
; igual que float_to_int pero suma 1 al resultado
asm_gini_index:
    push    rbp
    mov     rbp, rsp

    cvttss2si rax, xmm0     ; convierte float -> int
    add     rax, 1          ; suma 1

    pop     rbp
    ret
