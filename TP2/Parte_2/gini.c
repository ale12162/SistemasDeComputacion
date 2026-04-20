#include <stdint.h>   // para uint64_t
#include <string.h>   // para memcpy

extern long convertir_gini_asm(void);


long convertir_gini(double valor) {
    long resultado;

    // Se reinterpreta como entero
    uint64_t bits;
    memcpy(&bits, &valor, 8);

    __asm__ (
        "pushq %[v]\n\t"           
        "call convertir_gini_asm\n\t"
        "addq $8, %%rsp\n\t"       
        : "=a" (resultado)         
        : [v] "r" (bits)           
        : "memory", "rcx", "rdx",
          "rsi", "rdi", "r8", "r9",
          "r10", "r11"             
    );

    return resultado;
}