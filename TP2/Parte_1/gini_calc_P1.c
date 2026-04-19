#include <stdio.h>

int convertir_gini(double valor_gini) {
    int gini_entero = (int)valor_gini;   // truncamiento: 42.9 → 42
    return gini_entero + 1;
}