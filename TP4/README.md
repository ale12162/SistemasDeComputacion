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
