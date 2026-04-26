#include "gini.h"

extern int asm_float_to_int(float val);
extern int asm_gini_index(float val);

int float_to_int(float val) {
    return asm_float_to_int(val);
}

int gini_index(float val) {
    return asm_gini_index(val);
}
