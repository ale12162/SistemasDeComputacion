#include <stdint.h>   // para uint64_t
#include <string.h>   // para memcpy

/* La rutina ASM no declara argumentos porque los recibe por el stack manualmente */
extern long convertir_gini_asm(void);

/* Esta es la función que Python llama vía ctypes.
 * Recibe el double por %xmm0 (ABI normal),
 * lo pushea al stack manualmente, llama al ASM,
 * y devuelve el resultado entero. */
long convertir_gini(double valor) {
    long resultado;

    /* Reinterpretamos los 64 bits del double como entero
     * SIN conversión numérica. 42.9 no se convierte a 42,
     * los bytes 0x4045733333333333 quedan intactos. */
    uint64_t bits;
    memcpy(&bits, &valor, 8);

    __asm__ (
        "pushq %[v]\n\t"           /* pusheamos el double (como bits enteros) al stack  */
        "call convertir_gini_asm\n\t"
        "addq $8, %%rsp\n\t"       /* limpiamos la pila: 1 argumento x 8 bytes          */
        : "=a" (resultado)         /* output: el resultado queda en %rax                */
        : [v] "r" (bits)           /* input: bits va a cualquier registro general        */
        : "memory", "rcx", "rdx",
          "rsi", "rdi", "r8", "r9",
          "r10", "r11"             /* clobbers: registros que call puede modificar       */
    );

    return resultado;
}