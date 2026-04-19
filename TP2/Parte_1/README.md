# TP2 — PARTE 1

## Requisitos previos

### Dependencias del sistema

```bash
sudo apt update
sudo apt install gcc build-essential python3 python3-pip
```

gcc: compilador de C, necesario para generar la shared library.
build-essential: incluye herramientas de compilación básicas (make, ld, etc.).
python3 y pip: intérprete Python y gestor de paquetes.

### Dependencias de Python

```bash
pip install requests
```

  requests: biblioteca para hacer llamadas HTTP a la API REST del Banco Mundial.

---

## Compilación de la shared library

Desde la raíz del repositorio, compilar `gini_calc.c` como librería dinámica:

```bash
gcc -shared -fPIC -o libgini.so gini_calc.c
```

Para compilar con símbolos de debug (útil para inspeccionar con GDB):

```bash
gcc -shared -fPIC -g3 -o libgini.so gini_calc.c
```

### Verificar que la compilación fue exitosa

```bash
# Confirmar que el archivo fue generado
ls -lh libgini.so

# Verificar que el símbolo convertir_gini fue exportado correctamente
nm libgini.so | grep convertir_gini

# Ver información del archivo binario generado
file libgini.so
```
---

## Ejecución

Con la librería compilada en el mismo directorio, ejecutar:

```bash
python3 api_gini_P1.py
```
