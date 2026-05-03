# Trabajo Práctico 1 — Exploración del entorno UEFI y la Shell

**Objetivo:** explorar cómo UEFI abstrae el hardware y gestiona la configuración del sistema antes de cargar el sistema operativo, usando la UEFI Shell sobre QEMU + OVMF.

A diferencia del BIOS Legacy que simplemente leía el primer sector de un disco (MBR), UEFI es un entorno completo con su propio gestor de memoria, red y consola. 

---

## 1.1 Arranque en el entorno virtual

**Comando:**

```
qemu-system-x86_64 -m 512 -bios /usr/share/ovmf/OVMF.fd -net none
```

- `-m 512`: asigna 512 MB de RAM a la VM.
- `-bios /usr/share/ovmf/OVMF.fd`: en lugar de la BIOS legacy de QEMU, carga el firmware UEFI provisto por el paquete `ovmf`.
- `-net none`: deshabilita la placa de red virtual (no la necesitamos para el TP)

![Entorno virtual](img/11_uefi_shell.png)

---

## 1.2 Exploración de Dispositivos (Handles y Protocolos)

UEFI no usa letras de unidad fijas (como `C:` en Windows) ni interrupciones predefinidas (como `int 0x13` para disco en BIOS). En su lugar mantiene una base de datos interna de handles que agrupan protocolos.

### Comandos a ejecutar dentro de la UEFI Shell

```
Shell> map
Shell> FS0:
FS0:\> ls
FS0:\> exit             (volvemos al prompt Shell> )
Shell> dh -b
```

#### `map`

Lista los **mappings** entre nombres lógicos y los protocolos/dispositivos detectados. Si corremos este comando, vemos lo siguiente:

![1.2](img/1.2.png)

Las cadenas largas (`PciRoot(0x0)/Pci(...)/Ata(...)`) son **device paths**: una representación de cómo llegar al dispositivo desde la raíz del bus PCI.

#### `FS0:` y `ls`

`FS0:` cambia el "directorio actual" al primer file system detectado (típicamente la partición ESP de un disco real, o el ramdisk de OVMF en QEMU). `ls` lista los archivos y directorios.

En nuestro caso el comando `FS0:` muestra un error ya que no encuentra el system file.

#### `dh -b`

`dh` (= _dump handle_) lista todos los handles activos del sistema y los protocolos que cada uno implementa. La opción `-b` hace _break_ cada pantalla (hay decenas de handles, sin esto sale todo de corrido).

Corriendo esta linea de comando nos devuelve lo siguiente:

![dh -b](img/dh.png)

##### Cómo se lee cada línea

El formato es:

```
<numero_de_handle>: <protocolo_1>  <protocolo_2>  <protocolo_3> …
```

El número de la izquierda (`01:`, `02:`, `03:`, …) es el **ID del handle**. Todo lo que viene después son los **protocolos que ese handle implementa**, separados por espacios. Los protocolos pueden aparecer de tres formas:

1. **Nombre simbólico amigable** — como `LoadedImage`, `Decompress`, `FirmwareVolume2`, `SmartCardReader`. Son protocolos estándar de UEFI cuyo GUID está en el diccionario interno de la Shell, así que se traduce a un nombre legible.
2. **GUID crudo de 128 bits** — como `EE4E5898-3914-4259-9D6E-DC7BD79403CF`. El protocolo existe pero la Shell no tiene su nombre amigable cargado (típicamente protocolos vendor-specific o de revisiones nuevas).
3. **Nombre con paréntesis** — como `LoadedImage(DxeCore)` o `DevicePath(..F8EB-…))`. El paréntesis es **metadato extra del protocolo**, no otro protocolo. Por ejemplo `LoadedImage(DxeCore)` significa "este handle expone `LoadedImage`, y la imagen cargada se llama `DxeCore.efi`".

---

### Pregunta de Razonamiento 1

> Al ejecutar `map` y `dh`, vemos protocolos e identificadores en lugar de puertos de hardware fijos. ¿Cuál es la ventaja de seguridad y compatibilidad de este modelo frente al antiguo BIOS?

**Respuesta:**

En el modelo BIOS, el código que quería usar el hardware tenía dos caminos: o llamaba interrupciones predefinidas (`int 0x10` para video, `int 0x13` para disco) que asumían comportamientos fijos del firmware, o tocaba directamente puertos de I/O y direcciones de memoria conocidas (`0x3F8` para serie, `0xB8000` para texto VGA). Esto trajo dos problemas que UEFI resuelve con el modelo de handles + protocolos.

**Compatibilidad.** El BIOS fijaba la convención del PC IBM original: discos por `int 0x13`, hasta 4 particiones primarias, geometría CHS, MBR de 512 bytes, etc. Cuando aparecieron tecnologías nuevas (USB, SATA, NVMe, GPT, Secure Boot) la BIOS tuvo que ir parchando con módulos como CSM y limitaciones cada vez más artificiales. UEFI, en cambio, define una interfaz abstracta (el protocolo) y deja el cómo implementarla al driver del hardware concreto. Un disco SATA, uno NVMe y uno USB exponen el mismo `EFI_BLOCK_IO_PROTOCOL`: el bootloader y el SO los consumen igual sin saber qué hay debajo. Soportar un nuevo bus es escribir un driver que produzca ese protocolo, no rehacer la convención. 

**Seguridad.**
**No hay acceso directo al hardware.** Un programa UEFI no toca puertos: tiene que pedirle al firmware un handle y usar el protocolo. El firmware media todos los accesos y puede aplicar políticas (rechazar, auditar, etc.), esto facilita un desarrollo seguro y sin problemas con el hardware.

---

## 1.3 Análisis de Variables Globales (NVRAM)

La fase BDS (Boot Device Selection) decide qué cargar basándose en variables no volátiles.

### Comandos

```
Shell> dmpstore
Shell> set TestSeguridad "Hola UEFI"
Shell> set -v
```

![1.3](img/1.3.png)

#### `dmpstore`

Vuelca todas las variables UEFI accesibles desde la Shell.

Cada entrada de `dmpstore` muestra GUID + nombre + atributos + dump hexadecimal del contenido. Los `Boot####` son estructuras `EFI_LOAD_OPTION` que contienen la descripción legible y el device path del archivo `.efi` a ejecutar.

#### `set TestSeguridad "Hola UEFI"`

Crea una variable de la Shell llamada `TestSeguridad`. Es una variable temporal que vive solo mientras la Shell está corriendo. Se usa para mostrar cómo se manipulan variables desde la consola.

#### `set -v`

Lista todas las variables del entorno de la Shell (las creadas con `set`).

---

### Pregunta de Razonamiento 2

> Observando las variables `Boot####` y `BootOrder`, ¿cómo determina el Boot Manager la secuencia de arranque?

**Respuesta:**

El Boot Manager, en cada arranque, hace lo siguiente:

1. **Lee `BootOrder`** desde NVRAM.
2. **Itera** los IDs en ese orden.
3. Para cada ID, **lee la `Boot####`** correspondiente y resuelve el device path: el firmware recorre handles/protocolos para localizar el archivo ejecutable `.efi` indicado.
4. Si el archivo existe y supera Secure Boot (firma válida cuando está activado), **lo carga y ejecuta**. La entrada usada queda registrada en `BootCurrent`.
5. Si el archivo no se encuentra o falla, **pasa al siguiente ID** de `BootOrder`.

---

## 1.4 Footprinting de Memoria y Hardware

Antes de entregar el control al SO, el firmware ya descubrió todo el hardware y armó una "foto" del sistema. Esta sección consulta tres vistas de esa foto: el mapa de memoria, los dispositivos PCI y los drivers cargados.

### Comandos

```
Shell> memmap -b
Shell> pci -b
Shell> drivers -b
```

(`-b` hace pausa cada pantalla. La salida real es larga.)

![1.4.1](img/1.4.1.png)

![1.3](img/1.4.2.png)

![1.3](img/1.4.3.png)

---

### Pregunta de Razonamiento 3

> En el mapa de memoria (`memmap`), existen regiones marcadas como `RuntimeServicesCode`. ¿Por qué estas áreas son un objetivo principal para los desarrolladores de malware (Bootkits)?

**Respuesta:**

- **Runtime Services**: siguen disponibles después de `ExitBootServices`, durante toda la ejecución del SO.

El código que implementa esos Runtime Services vive precisamente en las regiones marcadas `RuntimeServicesCode` del `memmap`. El SO mapea esa memoria en su espacio virtual, y cuando necesita una de esas funciones, salta al firmware. Es decir: es código del firmware ejecutándose dentro del contexto del SO, con privilegio máximo, mientras el sistema está corriendo.

Eso lo vuelve un objetivo para los desarrolladores de malware:

1. **Persistencia inmune al disco.** El código de los Runtime Services se carga desde la SPI flash de la motherboard en cada arranque. Reformatear el disco, reinstalar Windows, cambiar la SSD: nada de eso lo borra. Solo borra el bootkit reflashear el firmware.
2. **Privilegio máximo y previo al SO.** Ejecuta antes que cualquier antivirus o EDR. Para el momento en que el SO terminó de cargar, el bootkit ya tomó las decisiones que quería tomar (ej. desactivar verificaciones, modificar el kernel en vuelo, plantar drivers).
3. **Invisibilidad para el SO.** Los antivirus convencionales escanean el disco y la RAM "de su lado"; muy pocos inspeccionan las páginas de `RuntimeServicesCode`, porque están marcadas como pertenecientes al firmware y son código que el SO no controla.
4. **Sobreviven a reinstalaciones del SO.** El bootkit modifica la imagen del firmware en SPI flash; en cada arranque siguiente, el firmware vuelve a copiarse a esas páginas de `RuntimeServicesCode` ya infectadas. El SO ve un sistema "limpio", pero el firmware no lo es.
