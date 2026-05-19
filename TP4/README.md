# Sistemas de Computación - Trabajo Práctico N° 4

Módulos de kernel y llamadas a sistema

**Nombres**  
_Baccino, Luca; Painenao, Juan Manuel; Alejandro R. Stangaferro;_  
**Claude's Interns**

**Facultad de Ciencias Exactas, Físicas y Naturales**  
**Sistemas de Computación**
**Profesores**
_Javier A. Jorge; Miguel A. Solinas;_
**2026**

# Punto 5 — ¿Qué diferencia existe entre un módulo y un programa?

Existen tres diferencias fundamentales entre un módulo del kernel y un programa de usuario convencional:

* **Espacio de ejecución:** Los programas se ejecutan como procesos independientes dentro del **espacio de usuario** (*user space*), el cual tiene permisos restringidos e indirectos. Por el contrario, los módulos operan directamente en el **espacio del kernel** (*kernel space*), teniendo acceso privilegiado y directo a todos los recursos de hardware y a la memoria física del sistema.
* **Propósito y ciclo de vida:** Mientras que un programa es una aplicación diseñada para cumplir una tarea específica ejecutándose de principio a fin, un módulo es un fragmento de código diseñado para cargarse (ej. `insmod`) y descargarse (ej. `rmmod`) en el kernel de forma dinámica. Su función principal es extender la funcionalidad del sistema operativo sobre la marcha (como agregar un nuevo controlador de dispositivo) sin necesidad de compilar un kernel monolítico inmenso ni tener que reiniciar la máquina.
* **Impacto de los errores (Criticidad):** En un programa convencional, si se comete un error grave de acceso a memoria, el sistema operativo aísla el fallo, detiene únicamente ese proceso (generando un *segmentation fault*) y el resto del equipo sigue funcionando. En un módulo, al operar en el nivel de máximo privilegio, un error similar (como el uso de un puntero salvaje) no puede ser contenido; puede corromper estructuras de datos críticas de otros procesos, dañar el sistema de archivos o provocar un colapso total del sistema operativo (*Kernel Panic*), lo que obliga a reiniciar físicamente el equipo.

# Punto 6 — ¿Cómo ver las llamadas al sistema que realiza un hello world en C?

Con la herramienta **`strace`**, que intercepta y muestra todas las syscalls que ejecuta un proceso.

```bash
gcc hello.c -o hello
strace ./hello              # imprime el trace por pantalla
strace -o trace.txt ./hello # guarda el trace en un archivo
strace -c ./hello           # muestra un resumen con conteo de syscalls
```

## Evidencia

Programa usado:

```c
#include <stdio.h>
int main(void) {
    printf("Hola mundo desde TP4\n");
    return 0;
}
```

Resumen obtenido con `strace -c ./hello`:

```
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
 22.64    0.000036          12         3           brk
 20.75    0.000033          33         1           write
 17.61    0.000028          28         1           prlimit64
 16.98    0.000027          27         1           munmap
 11.32    0.000018           6         3           fstat
 10.69    0.000017          17         1           getrandom
  0.00    0.000000           0         1           read
  0.00    0.000000           0         2           close
  0.00    0.000000           0         8           mmap
  0.00    0.000000           0         3           mprotect
  0.00    0.000000           0         2           pread64
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         1           arch_prctl
  0.00    0.000000           0         1           set_tid_address
  0.00    0.000000           0         2           openat
  0.00    0.000000           0         1           set_robust_list
  0.00    0.000000           0         1           rseq
------ ----------- ----------- --------- --------- ----------------
100.00    0.000159           4        34         1 total
```

Un simple "hola mundo" generó **34 syscalls**, de las cuales sólo una (`write`) corresponde al `printf`. El resto son del cargador dinámico y la inicialización de la libc.

# Punto 7 — ¿Qué es un segmentation fault? ¿Cómo lo maneja el kernel y cómo lo hace un programa?

## ¿Qué es?

Una **violación de acceso a memoria**: el programa intentó leer o escribir en una dirección virtual a la que no tiene permiso (memoria no mapeada, un puntero `NULL`, una región de sólo lectura, o fuera de los segmentos asignados al proceso).

## Cómo lo maneja el kernel

1. La **MMU** (hardware del CPU) detecta el acceso ilegal y genera una excepción de tipo *page fault*.
2. El handler de page fault del kernel evalúa si el acceso es legítimo (página válida pero no presente en RAM → la trae del swap) o realmente inválido.
3. Si es inválido, el kernel envía al proceso la señal **`SIGSEGV`** (signal 11).

## Cómo lo maneja el programa

- Por defecto, el manejador de `SIGSEGV` **termina el proceso** y genera un *core dump* (si `ulimit -c` lo permite) para depuración con `gdb`.
- El programa puede **capturar** la señal con `signal()` o `sigaction()` y manejarla, aunque no es buena práctica porque tras un segfault el estado de la memoria queda inconsistente.

## Evidencia

Programa usado:

```c
#include <stdio.h>

int main(void) {
    int *p = NULL;
    printf("Voy a derefenciar un puntero NULL...\n");
    *p = 42;
    printf("Esta linea nunca se ejecuta\n");
    return 0;
}
```

Ejecución:

```
$ ./segfault
Voy a derefenciar un puntero NULL...
Segmentation fault (core dumped)
$ echo "Codigo de salida: $?"
Codigo de salida: 139
```

El código de salida **139 = 128 + 11**, donde 11 es el número de `SIGSEGV`. Es la convención de bash para indicar que el proceso fue terminado por una señal.

## Diferencia con un módulo de kernel

Si un **módulo del kernel** comete un acceso ilegal, no hay un proceso "padre" que lo termine. El resultado es un **Kernel Oops** (queda registrado en `dmesg` con stack trace y el kernel sigue marcado como *tainted*) o, en casos graves, un **Kernel Panic** que congela el sistema.
