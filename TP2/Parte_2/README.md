# TP2 — Parte 2

## Requisitos previos

```bash
sudo apt update
sudo apt install gcc build-essential nasm python3 python3-pip
pip install requests
```

---

## Compilación


```bash
# 1. Ensamblar gini_asm.s → gini_asm.o
as --64 -g -o gini_asm.o gini_asm.s

# 2. Compilar gini.c → gini.o
gcc -c -fPIC -g3 -o gini.o gini.c

# 3. Linkear ambos objetos → libgini.so
gcc -shared -o libgini.so gini.o gini_asm.o
```

Verificar que ambos símbolos fueron exportados correctamente (`T` = código en `.text`):

```bash
nm libgini.so | grep -E "convertir_gini|convertir_gini_asm"
```

Salida esperada:
```
000000000000XXXX T convertir_gini
000000000000XXXX T convertir_gini_asm
```

---

## Ejecución

```bash
python3 api_gini_P2.py
```

Salida esperada:
```
Valor GINI float: 42.4
GINI convertido a entero: 43
```
