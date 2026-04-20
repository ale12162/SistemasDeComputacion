.section .text
    .globl  convertir_gini_asm       # exportar símbolo para el linker
    .type   convertir_gini_asm, @function

convertir_gini_asm:
    # Prólogo — preparamos el stack frame
    pushq   %rbp
    movq    %rsp, %rbp
    
    # Aquí el mapa de la pila (stack frame) es:
    # 24(%rbp) -> 2do argumento (b)
    # 16(%rbp) -> 1er argumento (a)
    #  8(%rbp) -> Dirección de retorno (puesta por la instrucción 'call')
    #  0(%rbp) -> Valor original de rbp (puesto en el prólogo)
    
    movsd   16(%rbp), %xmm0
    cvttsd2si %xmm0, %rax
    addq    $1, %rax

    # Epílogo — restauramos el estado anterior
    popq    %rbp
    ret

    .size   convertir_gini_asm, .-convertir_gini_asm