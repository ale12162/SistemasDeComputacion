// main_test.c
#include <stdio.h>

extern long convertir_gini(double valor);

int main(void) {
    double gini = 42.9;
    long resultado = convertir_gini(gini);
    printf("GINI convertido: %ld\n", resultado);
    return 0;
}