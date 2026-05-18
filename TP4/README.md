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

### 5. ¿Qué diferencia existe entre un módulo y un programa?

Existen tres diferencias fundamentales entre un módulo del kernel y un programa de usuario convencional:

* **Espacio de ejecución:** Los programas se ejecutan como procesos independientes dentro del **espacio de usuario** (*user space*), el cual tiene permisos restringidos e indirectos. Por el contrario, los módulos operan directamente en el **espacio del kernel** (*kernel space*), teniendo acceso privilegiado y directo a todos los recursos de hardware y a la memoria física del sistema.
* **Propósito y ciclo de vida:** Mientras que un programa es una aplicación diseñada para cumplir una tarea específica ejecutándose de principio a fin, un módulo es un fragmento de código diseñado para cargarse (ej. `insmod`) y descargarse (ej. `rmmod`) en el kernel de forma dinámica. Su función principal es extender la funcionalidad del sistema operativo sobre la marcha (como agregar un nuevo controlador de dispositivo) sin necesidad de compilar un kernel monolítico inmenso ni tener que reiniciar la máquina.
* **Impacto de los errores (Criticidad):** En un programa convencional, si se comete un error grave de acceso a memoria, el sistema operativo aísla el fallo, detiene únicamente ese proceso (generando un *segmentation fault*) y el resto del equipo sigue funcionando. En un módulo, al operar en el nivel de máximo privilegio, un error similar (como el uso de un puntero salvaje) no puede ser contenido; puede corromper estructuras de datos críticas de otros procesos, dañar el sistema de archivos o provocar un colapso total del sistema operativo (*Kernel Panic*), lo que obliga a reiniciar físicamente el equipo.

# Punto 6 — Llamadas al sistema de un "hello world" en C

## ¿Cómo ver las llamadas al sistema que realiza un programa?

Para observar las **llamadas al sistema (syscalls)** que ejecuta un programa de usuario se utiliza la herramienta **`strace`**, que intercepta y registra cada syscall realizada por el proceso junto con sus argumentos y valor de retorno. Es la forma estándar en Linux de depurar interacciones entre un programa y el kernel.

Las opciones más útiles son:

- `strace ./programa` → muestra todas las syscalls por pantalla.
- `strace -o archivo.txt ./programa` → guarda la salida en un archivo.
- `strace -c ./programa` → muestra un resumen estadístico (cantidad de llamadas, tiempo, errores por syscall).
- `strace -e openat ./programa` → filtra por una syscall específica.

## Procedimiento realizado

Se creó un programa mínimo en C que imprime un mensaje por pantalla:

```c
#include <stdio.h>
int main(void) {
    printf("Hola mundo desde TP4\n");
    return 0;
}
```

Se compiló y se trazó:

```bash
gcc hello.c -o hello
strace -o trace_completo.txt ./hello
strace -c -o trace_resumen.txt ./hello
```

## Salida — Trace completo (primeras 30 líneas)

```
execve("./hello", ["./hello"], 0x7ffc266665b0 /* 28 vars */) = 0
brk(NULL)                               = 0x5a167e93e000
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7d5387a80000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/etc/ld.so.cache", O_RDONLY|O_CLOEXEC) = 3
fstat(3, {st_mode=S_IFREG|0644, st_size=32867, ...}) = 0
mmap(NULL, 32867, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7d5387a77000
close(3)                                = 0
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libc.so.6", O_RDONLY|O_CLOEXEC) = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0\220\243\2\0\0\0\0\0"..., 832) = 832
pread64(3, "...", 784, 64) = 784
fstat(3, {st_mode=S_IFREG|0755, st_size=2125328, ...}) = 0
pread64(3, "...", 784, 64) = 784
mmap(NULL, 2170256, PROT_READ, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7d5387800000
mmap(0x7d5387828000, 1605632, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x28000) = 0x7d5387828000
mmap(0x7d53879b0000, 323584, PROT_READ, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1b0000) = 0x7d53879b0000
mmap(0x7d53879ff000, 24576, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_DENYWRITE, 3, 0x1fe000) = 0x7d53879ff000
mmap(0x7d5387a05000, 52624, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_FIXED|MAP_ANONYMOUS, -1, 0) = 0x7d5387a05000
close(3)                                = 0
mmap(NULL, 12288, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7d5387a74000
arch_prctl(ARCH_SET_FS, 0x7d5387a74740) = 0
set_tid_address(0x7d5387a74a10)         = 2514
set_robust_list(0x7d5387a74a20, 24)     = 0
rseq(0x7d5387a75060, 0x20, 0, 0x53053053) = 0
mprotect(0x7d53879ff000, 16384, PROT_READ) = 0
mprotect(0x5a167463d000, 4096, PROT_READ) = 0
mprotect(0x7d5387ab8000, 8192, PROT_READ) = 0
prlimit64(0, RLIMIT_STACK, NULL, {rlim_cur=8192*1024, rlim_max=RLIM64_INFINITY}) = 0
munmap(0x7d5387a77000, 32867)           = 0
fstat(1, {st_mode=S_IFCHR|0620, st_rdev=makedev(0x88, 0), ...}) = 0
```

## Salida — Resumen estadístico (`strace -c`)

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

## Análisis de las syscalls observadas

Aunque el programa en C parece trivial — una sola línea de `printf` —, su ejecución implicó **34 llamadas al sistema** distintas. Esto evidencia que un programa de usuario nunca interactúa directamente con el hardware: cada operación "simple" se traduce en pedidos al kernel.

Las syscalls observadas pueden agruparse en cuatro fases:

**1. Carga del proceso**
- `execve` — el shell le pide al kernel que reemplace su imagen por la del binario `./hello`. Es la syscall que efectivamente "inicia" el programa.

**2. Configuración del cargador dinámico**
- `access("/etc/ld.so.preload")` — verifica si hay librerías a precargar (en este caso no, retorna `ENOENT`).
- `openat`, `fstat`, `mmap`, `close` sobre `/etc/ld.so.cache` — abre el caché del enlazador dinámico para localizar librerías.
- `openat` sobre `/lib/x86_64-linux-gnu/libc.so.6` — abre la **librería estándar de C** y la mapea en memoria con `mmap` (varias regiones: código ejecutable, datos de sólo lectura, datos modificables).

**3. Inicialización del entorno de ejecución**
- `arch_prctl(ARCH_SET_FS, ...)` — configura el registro `FS` para acceso a Thread Local Storage.
- `set_tid_address`, `set_robust_list`, `rseq` — configuran estructuras del kernel asociadas al hilo (necesarias para señales, mutexes, etc.).
- `mprotect` (varias veces) — ajusta los permisos de páginas de memoria (típicamente RELRO: marca como sólo lectura zonas que ya no deben modificarse).
- `brk` — ajusta el tamaño del heap.
- `getrandom` — obtiene aleatoriedad para mitigaciones de seguridad como ASLR/Stack Canaries.
- `prlimit64` — consulta el límite de tamaño de stack.

**4. Ejecución del `main()` y finalización**
- `fstat(1, ...)` — la libc verifica el tipo del descriptor 1 (stdout) para decidir si bufferizar.
- `write(1, "Hola mundo desde TP4\n", 21)` — **la única syscall del `printf`**: vuelca el texto a la salida estándar.
- `munmap` — libera memoria mapeada que ya no necesita.
- `exit_group` — termina todos los hilos del proceso y devuelve el control al kernel.

## Observaciones

- De las 34 syscalls, **sólo una corresponde realmente a lo que el programador escribió** (`write`, invocada por `printf`). Las otras 33 son trabajo "invisible" del cargador dinámico y la inicialización de la libc.
- La syscall `write` consumió **el 20,75% del tiempo total** a pesar de ser una sola, porque escribir a una terminal involucra al driver de TTY.
- El error reportado (`1 errors` en `access`) es esperado: se intentó abrir `/etc/ld.so.preload`, que normalmente no existe.
- Este experimento muestra concretamente la frontera entre **espacio de usuario** y **espacio del kernel**: cada `syscall` listada es un cruce de esa frontera.
