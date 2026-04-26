#include <stdio.h>
#include "gini.h"

int main() {
    float val = 42.3f;

    int truncado = float_to_int(val);
    int indice   = gini_index(val);

    printf("float:        %.2f\n", val);
    printf("float_to_int: %d\n", truncado);
    printf("gini_index:   %d\n", indice);

    return 0;
}
