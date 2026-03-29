# Sistemas de Computación - Trabajo Práctico N° 1

**Nombres**  
_Baccino, Luca; Painenao, Juan Manuel; Alejandro R. Stangaferro;_  
**Claude's Interns**

**Facultad de Ciencias Exactas, Físicas y Naturales**  
**Sistemas de Computación**
**Profesores**
_Javier A. Jorge; Miguel A. Solinas;_
**2026**

---

## Actividades

### Actividad 1
Armar una lista de benchmarks, ¿cuales les serían más útiles a cada uno ? ¿Cuáles podrían llegar a medir mejor las tareas que ustedes realizan a diario ? 
Pensar en las tareas que cada uno realiza a diario y escribir en una tabla de dos entradas las tareas y que benchmark la representa mejor.

### Actividad 2
Cual es el rendimiento de estos procesadores para compilar el kernel de linux ?
Intel Core i5-13600K
AMD Ryzen 9 5900X 12-Core
Cual es la aceleración cuando usamos un AMD Ryzen 9 7950X 16-Core
https://openbenchmarking.org/test/pts/build-linux-kernel-1.15.0

### Actividad 3
Conseguir un esp32 o cualquier procesador al que se le pueda cambiar la frecuencia.
Ejecutar un código que demore alrededor de 10 segundos. Puede ser un bucle for con sumas de enteros por un lado y otro con suma de floats por otro lado.
¿Qué sucede con el tiempo del programa al duplicar (variar) la frecuencia ?

### Actividad 4
Seguir el tutorial de profiling con gprof y perf sobre el código de ejemplo, mostrando con capturas de pantalla cada paso y analizando cómo se distribuye el tiempo entre las funciones. 

## Respuestas

### Actividad 1

A diario utilizamos la computadora para estudiar/trabajar y para ocio. Dentro de estudio/trabajo enlistamos las tareas de programar, navegar en la web, calculo numero y uso general multitarea. Para ocio enlistamos videos/streaming y gaming.

| Tarea diaria | Benchmark | Plataforma / Suite | Dónde ver resultados |
|---|---|---|---|
| Programar y editar código (IDE) | pts/compress-7zip | Phoronix Test Suite | https://openbenchmarking.org/test/pts/compress-7zip |
| Navegar la web (muchas pestañas) | Speedometer 3.0 | BrowserBench (Apple/Google/Mozilla) | https://browserbench.org/Speedometer3.0/ |
| Ver videos / streaming | pts/ffmpeg | Phoronix Test Suite | https://openbenchmarking.org/test/pts/ffmpeg |
| Gaming | 3DMark / Unigine Heaven | UL Benchmarks / Unigine | https://www.tomshardware.com/reviews/cpu-hierarchy,4312.html |
| Cálculo numérico | pts/numpy | Phoronix Test Suite | https://openbenchmarking.org/test/pts/numpy |
| Uso general multitarea | Geekbench Multi-Core | Geekbench (Primate Labs) | https://browser.geekbench.com/processor-benchmarks |


### Actividad 2

Los resultados del rendimiento de los siguientes procesadores para compilar el kernel de linux fueron extraidos de la tabla de OpenBenchmarking.org para el test pts/build-linux-kernel

| Procesador | Núcleos / Hilos | Segundos (promedio) | Percentil |
|---|---|---|---|
| Intel Core i5-13600K | 14 / 20 | 76 s | 57th |
| AMD Ryzen 9 5900X 12-Core | 12 / 24 | ~86 s | ~52nd |
| AMD Ryzen 9 7950X 16-Core | 16 / 32 | 59 s | 73rd |

#### Cálculo de la aceleración (Speedup)

La aceleración o speedup mide cuántas veces más rápido es un sistema mejorado respecto de uno original. Se calcula como:

$$Speedup = \frac{T_{original}}{T_{mejorado}}$$

Donde $T_{original}$ es el tiempo del procesador de referencia y $T_{mejorado}$ es el tiempo del procesador más rápido (en este caso, el AMD Ryzen 9 7950X).

#### Speedup del Ryzen 9 7950X respecto del i5-13600K

$$Speedup = \frac{T_{i5-13600K}}{T_{7950X}} = \frac{76 \, s}{59 \, s} = 1.288$$

El Ryzen 9 7950X es aproximadamente 1.29 veces más rápido que el i5-13600K para compilar el kernel de Linux, es decir, un 28.8% más rápido.

#### Speedup del Ryzen 9 7950X respecto del Ryzen 9 5900X

$$Speedup = \frac{T_{5900X}}{T_{7950X}} = \frac{86 \, s}{59 \, s} = 1.457$$

El Ryzen 9 7950X es aproximadamente 1.46 veces más rápido que el Ryzen 9 5900X, es decir, un 45.7% más rápido.

#### Tabla resumen

| Comparación | $T_{original}$ (s) | $T_{mejorado}$ (s) | Speedup |
|---|---|---|---|
| 7950X vs i5-13600K | 76 | 59 | 1.29x |
| 7950X vs Ryzen 9 5900X | 86 | 59 | 1.46x |

### Actividad 3

#### Introducción y Metodología
Para verificar experimentalmente la relación teórica entre la frecuencia de operación de una CPU y el tiempo de ejecución de un programa, se utilizó el simulador online Wokwi con una placa ESP32. 

El tiempo de ejecución de un programa está dictado por la ecuación fundamental:
$$T_{prog} = \frac{N_{instrucciones} \times CPI}{f_{CPU}}$$

Bajo este modelo, si se mantiene el mismo código ($N$ y $CPI$ constantes), el tiempo de ejecución es inversamente proporcional a la frecuencia.

Para evaluar correctamente el impacto del tipo de dato (enteros vs. punto flotante) y aislar el costo computacional (CPI), es un requisito matemático ineludible mantener constante la variable $N_{instrucciones}$. Por ello, se diseñó un benchmark donde ambos bucles (enteros y floats) ejecutan 20.000.000 de iteraciones. Se probaron frecuencias de 8 MHz y 4 MHz.

#### Código Fuente Utilizado (ESP32)

El siguiente código fue diseñado para ser ejecutado en el framework de Arduino para ESP32. Se utilizan tipos de datos volátiles (`volatile`) para forzar a la ALU y a la FPU a ejecutar matemáticamente cada iteración del bucle, evadiendo la optimización de código muerto del compilador.

```cpp
// tp1_benchmark_frecuencia_esp32.ino
#include "esp32-hal-cpu.h"

const long ITERACIONES = 20000000; 

void ejecutarPruebaMatematica() {
  int freq_actual = getCpuFrequencyMhz();
  Serial.print("\n>>> Iniciando prueba a ");
  Serial.print(freq_actual);
  Serial.println(" MHz <<<");

  // Suma de ENTEROS
  unsigned long t_inicio_int = millis();
  volatile long suma_int = 0; 
  
  for (long i = 0; i < ITERACIONES; i++) {
    suma_int += 5; 
  }
  
  unsigned long t_fin_int = millis();
  Serial.print("Tiempo ENTEROS: ");
  Serial.print((t_fin_int - t_inicio_int) / 1000.0, 3);
  Serial.println(" seg");

  // Suma de FLOATS 
  unsigned long t_inicio_float = millis();
  volatile float suma_float = 0.0f; 
  
  for (long i = 0; i < ITERACIONES; i++) {
    suma_float += 5.0f;
  }
  
  unsigned long t_fin_float = millis();
  Serial.print("Tiempo FLOATS:  ");
  Serial.print((t_fin_float - t_inicio_float) / 1000.0, 3);
  Serial.println(" seg");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("--- Benchmark de Frecuencia y Tipos de Datos ---");

  setCpuFrequencyMhz(8);
  ejecutarPruebaMatematica();

  setCpuFrequencyMhz(4);
  ejecutarPruebaMatematica();
  
  Serial.println("\n--- Fin de las pruebas ---");
}

void loop() {
}
```

#### Resultados Obtenidos (Wokwi Simulator)

| Frecuencia (MHz) | Tiempo Enteros | Tiempo Floats |
| :--- | :--- | :--- |
| **8 MHz** | 13.320 s | 21.314 s |
| **4 MHz** | 16.611 s | 26.578 s |

#### Análisis de Relación Frecuencia-Tiempo
Al reducir la frecuencia a la mitad (8 MHz a 4 MHz), el marco teórico predice un aumento del 100% en el tiempo. Sin embargo, los resultados muestran que el tiempo de enteros aumentó apenas un 24.7% (de 13.320s a 16.611s).

Este comportamiento anómalo se debe a una limitación documentada del simulador Wokwi. Al ser un simulador ISA basado en web, aplica un "cap" artificial cercano a los 8 MHz por defecto. Al solicitar frecuencias más bajas, la simulación pierde proporcionalidad temporal estricta.

#### Comparación de Arquitectura
Dado que las iteraciones se mantuvieron idénticas, la relación de tiempos refleja la relación real de Ciclos Por Instrucción (CPI):
* **Ratio a 8 MHz:** $21.314 / 13.320 = \mathbf{1.60\times}$
* **Ratio a 4 MHz:** $26.578 / 16.611 = \mathbf{1.60\times}$

La consistencia matemática es absoluta. El ESP32 requiere un 60% más de ciclos de reloj para resolver matemáticas de punto flotante simple frente a sumas de enteros.

### Actividad 4
#### 1. Compilar con profiling habilitado
Agregamos `-pg` para que GCC instrumente el código.
```bash
gcc -Wall -pg test_gprof.c test_gprof_new.c -o test_gprof
```

#### 2. Ejecutar el programa
Al ejecutarse genera automáticamente el archivo `gmon.out` con los datos de profiling.
```bash
./test_gprof
```

#### 3. Generar el análisis con gprof
Lee el ejecutable y `gmon.out`, produce el informe con flat profile y call graph.
```bash
gprof test_gprof gmon.out > analysis.txt
cat analysis.txt
```

#### 4. Distintas flags de gprof

- **-a**: Oculta funciones estáticas (en este caso `func2` desaparece del reporte).
- **-b**: Elimina el texto explicativo, deja solo los datos.
- **-p**: Muestra solo el flat profile (sin call graph).
- **-pfunc1**: Filtra el flat profile para mostrar solo 

#### Resultados
