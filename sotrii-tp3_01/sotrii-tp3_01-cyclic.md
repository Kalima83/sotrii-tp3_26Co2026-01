# Configuración de FreeRTOS para Cyclic Scheduling (TP3 - Sistema Gobernado por Tiempo)

## 1. Mapeo de Unidades de Tiempo a FreeRTOS
Para implementar un Planificador Cíclico Determinista (*Cyclic Executive*) sobre FreeRTOS manteniendo la precisión temporal exigida por los tests de garantía, se establece la siguiente equivalencia de base de tiempo:

* **Unidad Básica de Cómputo (1 U):** $100\text{ ms}$.
* **Tick de FreeRTOS (`configTICK_RATE_HZ`):** Configurado a $1000\text{ Hz}$ ($1\text{ ms}$ por tick).
* **Función de Temporización:** `vTaskDelayUntil(&xLastWakeTime, xFrequency)` para evitar deriva temporal (*drift*) entre tramas.
* **Modo de Ejecución:** Cooperativo por trama (*Non-preemptive* dentro de cada frame). Los *Jobs* se despachan secuencialmente desde el *Dispatcher*.

---

## 2. Parámetros de Diseño y Tests de Garantía por Sistema

Las condiciones de diseño para el tamaño de trama ($f$) bajo un Ejecutivo Cíclico son:
1. **Condición 1 ($f \ge C_i$):** El tamaño de trama debe alojar el peor tiempo de ejecución de cada tarea (o de sus sub-trabajos/slices).
2. **Condición 2 ($H \pmod f = 0$):** El marco debe dividir exactamente al Hiperperíodo $H = \text{mcm}(T_1, T_2, \dots, T_n)$.
3. **Condición 3 ($2f - \gcd(T_i, f) \le D_i$):** Garantiza que cada tarea se ejecute completamente antes de su plazo límite ($D_i = T_i$).

---

### Sistema 1

#### Parámetros y Prioridades
| Tarea | $C_i$ (U) | $T_i = D_i$ (U) | Prioridad Teórica |
| :---: | :-------: | :-------------: | :---------------: |
| $T_1$ |     1     |        4        |       Alta        |
| $T_2$ |     2     |        5        |       Media       |
| $T_3$ |     5     |       20        |       Baja        |

#### Test de Garantía
* **Factor de Utilización ($U$):** 
  $$U = \frac{1}{4} + \frac{2}{5} + \frac{5}{20} = 0.25 + 0.40 + 0.25 = 0.90 \quad (90\%) \le 1$$
* **Hiperperíodo ($H$):** $\text{mcm}(4, 5, 20) = 20\text{ U} = 2000\text{ ms}$.
* **Verificación del Tamaño de Trama ($f = 2\text{ U} = 200\text{ ms}$):**
  * **Condición 1:** $f \ge C_i \rightarrow 2 \ge 1$ (para $T_1$) y $2 \ge 2$ (para $T_2$). Para $T_3$ ($C_3=5$), se fracciona en sub-trabajos donde cada uno cumple $C_{3,j} \le 2$. *(Cumple)*.
  * **Condición 2:** $H \pmod f = 20 \pmod 2 = 0$ (10 tramas por ciclo). *(Cumple)*.
  * **Condición 3:**
    * $T_1$: $2(2) - \gcd(4,2) = 4 - 2 = 2 \le 4$ *(Cumple)*.
    * $T_2$: $2(2) - \gcd(5,2) = 4 - 1 = 3 \le 5$ *(Cumple)*.
    * $T_3$: $2(2) - \gcd(20,2) = 4 - 2 = 2 \le 20$ *(Cumple)*.

#### Diagrama de Gantt (Hiperperíodo: 10 Tramas de 2 U)
Trama,Instante (U),Tareas Ejecutadas,Cómputo Usado / f (2 U)
F0,[0  - 2),T1 (1U) + T2 (1U),2 U / 2 U
F1,[2  - 4),T1 (1U) + T3_1 (1U),2 U / 2 U
F2,[4  - 6),T1 (1U) + T2 (1U),2 U / 2 U
F3,[6  - 8),T1 (1U),1 U / 2 U
F4,[8  - 10),T1 (1U) + T2 (1U),2 U / 2 U
F5,[10 - 12),T1 (1U),1 U / 2 U
F6,[12 - 14),T1 (1U) + T2 (1U),2 U / 2 U
F7,[14 - 16),T1 (1U),1 U / 2 U
F8,[16 - 18),T1 (1U) + T2 (1U),2 U / 2 U
F9,[18 - 20),T1 (1U),1 U / 2 U

---

### Sistema 2

#### Parámetros y Prioridades
| Tarea | $C_i$ (U) | $T_i = D_i$ (U) | Prioridad Teórica |
| :---: | :-------: | :-------------: | :---------------: |
| $T_1$ |     1     |        6        |       Alta        |
| $T_2$ |     2     |       10        |       Media       |
| $T_3$ |     2     |       18        |       Baja        |

#### Test de Garantía
* **Factor de Utilización ($U$):** 
  $$U = \frac{1}{6} + \frac{2}{10} + \frac{2}{18} \approx 0.167 + 0.200 + 0.111 = 0.478 \quad (47.8\%) \le 1$$
* **Hiperperíodo ($H$):** $\text{mcm}(6, 10, 18) = 90\text{ U} = 9000\text{ ms}$.
* **Verificación del Tamaño de Trama ($f = 6\text{ U} = 600\text{ ms}$):**
  * **Condición 1:** $f \ge \max(C_i) \rightarrow 6 \ge 2$. *(Cumple sin fraccionamiento)*.
  * **Condición 2:** $H \pmod f = 90 \pmod 6 = 0$ (15 tramas por ciclo). *(Cumple)*.
  * **Condición 3:**
    * $T_1$: $2(6) - \gcd(6,6) = 12 - 6 = 6 \le 6$ *(Cumple)*.
    * $T_2$: $2(6) - \gcd(10,6) = 12 - 2 = 10 \le 10$ *(Cumple)*.
    * $T_3$: $2(6) - \gcd(18,6) = 12 - 6 = 6 \le 18$ *(Cumple)*.

#### Diagrama de Gantt (Muestra Representativa F0 a F5)
TramaInstante (U)Tareas EjecutadasCómputo Usado / f (6 U)F0[0  - 6)T1 (1U) + T2 (2U) + T3 (2U)5 U / 6 UF1[6  - 12)T1 (1U)1 U / 6 UF2[12 - 18)T1 (1U) + T2 (2U)3 U / 6 UF3[18 - 24)T1 (1U) + T3 (2U)3 U / 6 UF4[24 - 30)T1 (1U) + T2 (2U)3 U / 6 UF5[30 - 36)T1 (1U)1 U / 6 U......(Repite patrón armónico)...
---

### Sistema 3

#### Parámetros y Prioridades
| Tarea | $C_i$ (U) | $T_i = D_i$ (U) | Prioridad Teórica |
| :---: | :-------: | :-------------: | :---------------: |
| $T_1$ |     1     |        8        |     Más Alta      |
| $T_2$ |     3     |       15        |       Alta        |
| $T_3$ |     4     |       20        |       Media       |
| $T_4$ |     6     |       22        |       Baja        |

#### Test de Garantía
* **Factor de Utilización ($U$):** 
  $$U = \frac{1}{8} + \frac{3}{15} + \frac{4}{20} + \frac{6}{22} \approx 0.125 + 0.200 + 0.200 + 0.273 = 0.798 \quad (79.8\%) \le 1$$
* **Hiperperíodo ($H$):** $\text{mcm}(8, 15, 20, 22) = 1320\text{ U} = 132000\text{ ms}$.
* **Verificación del Tamaño de Trama ($f = 8\text{ U} = 800\text{ ms}$):**
  * **Condición 1:** $f \ge C_i \rightarrow 8 \ge 6$. *(Cumple)*.
  * **Condición 2:** $H \pmod f = 1320 \pmod 8 = 0$ (165 tramas por ciclo). *(Cumple)*.
  * **Condición 3:**
    * $T_2$ ($T_2=15$): $2(8) - \gcd(15,8) = 16 - 1 = 15 \le 15$ *(Cumple en el límite)*.
    * $T_4$ ($T_4=22$): $2(8) - \gcd(22,8) = 16 - 2 = 14 \le 22$ *(Cumple)*.

#### Estructura de Despacho (Patrón Reducido de 3 Tramas)
Dada la magnitud de $H=1320$, en la práctica se sintetizó la ejecución mediante un ciclo modular `frame % 3` ($24\text{ U}$):

TramaInstante (U)Tareas EjecutadasCómputo Usado / f (8 U)F0[0  - 8)T1 (1U) + T2 (3U) + T3 (4U)8 U / 8 UF1[8  - 16)T1 (1U) + T2 (3U) + T4 (4U)8 U / 8 U  (T4 Slice 1)F2[16 - 24)T1 (1U) + T4 (2U)3 U / 8 U  (T4 Slice 2)
---

### Sistema 4

#### Parámetros y Prioridades
| Tarea | $C_i$ (U) | $T_i = D_i$ (U) | Prioridad Teórica |
| :---: | :-------: | :-------------: | :---------------: |
| $T_1$ |    0.5    |        3        |     Más Alta      |
| $T_2$ |     1     |        6        |       Alta        |
| $T_3$ |     2     |       24        |       Media       |
| $T_4$ |     6     |       24        |       Baja        |

#### Test de Garantía
* **Factor de Utilización ($U$):** 
  $$U = \frac{0.5}{3} + \frac{1}{6} + \frac{2}{24} + \frac{6}{24} = 0.167 + 0.167 + 0.083 + 0.250 = 0.667 \quad (66.7\%) \le 1$$
* **Hiperperíodo ($H$):** $\text{mcm}(3, 6, 24) = 120\text{ U} = 12000\text{ ms}$.
* **Verificación del Tamaño de Trama ($f = 3\text{ U} = 300\text{ ms}$):**
  * **Condición 1:** Para $T_4$ ($C_4=6$), se divide en 3 *slices* de $2\text{ U}$. Para $T_1$, se utiliza `T1_Job_Half()` ($0.5\text{ U}$). Todas las cargas secundarias son $\le 3\text{ U}$. *(Cumple)*.
  * **Condición 2:** $H \pmod f = 120 \pmod 3 = 0$ (40 tramas por ciclo). *(Cumple)*.
  * **Condición 3:**
    * $T_1$: $2(3) - \gcd(3,3) = 6 - 3 = 3 \le 3$ *(Cumple)*.
    * $T_2$: $2(3) - \gcd(6,3) = 6 - 3 = 3 \le 6$ *(Cumple)*.
    * $T_4$: $2(3) - \gcd(24,3) = 6 - 3 = 3 \le 24$ *(Cumple)*.

#### Diagrama de Gantt (Módulo Repetitivo de 8 Tramas / 24 U)
TramaInstante (U)Tareas EjecutadasCómputo Usado / f (3 U)F0[0  - 3)T1 (0.5U) + T2 (1U)1.5 U / 3 UF1[3  - 6)T1 (0.5U) + T3 (2U)2.5 U / 3 UF2[6  - 9)T1 (0.5U) + T2 (1U)1.5 U / 3 UF3[9  - 12)T1 (0.5U) + T4_Slice1 (2U)2.5 U / 3 UF4[12 - 15)T1 (0.5U) + T2 (1U)1.5 U / 3 UF5[15 - 18)T1 (0.5U) + T4_Slice2 (2U)2.5 U / 3 UF6[18 - 21)T1 (0.5U) + T2 (1U)1.5 U / 3 UF7[21 - 24)T1 (0.5U) + T4_Slice3 (2U)2.5 U / 3 U
---

## 3. Configuración de Tareas e Implementación en FreeRTOS

1. **Prioridad del Dispatcher:** Asignada a la máxima prioridad de la aplicación (`tskIDLE_PRIORITY + 1`) para evitar interrupciones de tareas secundarias.
2. **Despacho Cooperativo por Trama:** Cada *Job* se ejecuta de forma secuencial y determinista dentro de un bloque `switch(frame)`.
3. **Control de Deriva:** Sincronizado estrictamente con `vTaskDelayUntil()` en la base del ciclo de despacho.

---

## 4. Conclusiones y Análisis de Implementación

### Determinismo y Eficiencia
El uso de `vTaskDelayUntil()` integrado con un *dispatcher* secuencial garantizó una cadencia de tramas rígida y libre de deriva temporal (*drift*). Al ejecutar los *Jobs* de forma secuencial dentro del contexto de una única tarea sin desafección (*non-preemptive* entre sub-trabajos), se eliminó por completo la sobrecarga por conmutación de contexto (*context switch*) y la necesidad de emplear primitivas de sincronización (semáforos o mutexes) para proteger datos compartidos.

### Optimización por Módulo (Sistema 4)
Para mitigar la sobrecarga en memoria Flash que representa declarar bloques `switch` extensos en sistemas con hiperperíodos grandes ($H=120\text{ U}$, 40 tramas), se aplicó la propiedad de recurrencia mediante la operación `frame % 8`. Esta abstracción permitió empaquetar las 40 tramas en solo 8 casos lógicos repetitivos sin alterar las restricciones temporales.

### Compromisos del Ejecutivo Cíclico (Sistema 3)
El análisis práctico del Sistema 3 puso en evidencia las limitaciones estructurales del Ejecutivo Cíclico cuando se aplican conjuntos de tareas con períodos no armónicos ($8, 15, 20, 22$):
* **Explosión del Hiperperíodo:** Genera un $H = 1320\text{ U}$ (165 tramas), lo que complica la mantenibilidad de la tabla estática.
* **Sobreejecución por Armonización:** La simplificación del dispatcher a un patrón de 3 tramas ($24\text{ U}$)

## 5. Informe de prueba de sistemas

[info] Inicializando App - Test Runner de Sistemas de Tiempo Real
[info] ==================================================
[info]   INICIANDO EVALUACION SECUENCIAL AUTOMATICA (TP3)
[info] ==================================================
[info] 

>>> 1/4. Ejecutando Sistema 1 (H=20, f=2) <<<
[info] --- [Sys1] Frame F0 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys1] Frame F1 ---
[info]   T1 Ejecutando (1 u)
[info]   T3 Ejecutando (5 u)
[info] --- [Sys1] Frame F2 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys1] Frame F3 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys1] Frame F4 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys1] Frame F5 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys1] Frame F6 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys1] Frame F7 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys1] Frame F8 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys1] Frame F9 ---
[info]   T1 Ejecutando (1 u)
[info] === [Sys1] Hiperperiodo Completado ===
[info] 

>>> 2/4. Ejecutando Sistema 2 (H=90, f=6) <<<
[info] --- [Sys2] Frame F0 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys2] Frame F1 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys2] Frame F2 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys2] Frame F3 ---
[info]   T1 Ejecutando (1 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys2] Frame F4 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys2] Frame F5 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys2] Frame F6 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys2] Frame F7 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys2] Frame F8 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys2] Frame F9 ---
[info]   T1 Ejecutando (1 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys2] Frame F10 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] --- [Sys2] Frame F11 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys2] Frame F12 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys2] Frame F13 ---
[info]   T1 Ejecutando (1 u)
[info] --- [Sys2] Frame F14 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (2 u)
[info] === [Sys2] Hiperperiodo Completado ===
[info] 

>>> 3/4. Ejecutando Sistema 3 (H=1320, f=8) <<<
[info] --- [Sys3] Frame F0 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F1 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F2 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F3 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F4 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F5 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F6 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F7 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F8 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F9 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F10 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F11 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F12 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F13 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F14 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F15 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F16 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F17 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F18 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F19 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F20 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F21 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F22 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F23 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F24 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F25 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F26 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F27 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F28 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F29 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F30 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F31 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F32 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F33 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F34 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F35 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F36 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F37 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F38 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F39 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F40 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F41 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F42 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F43 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F44 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F45 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F46 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F47 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F48 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F49 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F50 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F51 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F52 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F53 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F54 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F55 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F56 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F57 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F58 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F59 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F60 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F61 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F62 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F63 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F64 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F65 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F66 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F67 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F68 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F69 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F70 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F71 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F72 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F73 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F74 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F75 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F76 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F77 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F78 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F79 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F80 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F81 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F82 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F83 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F84 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F85 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F86 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F87 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F88 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F89 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F90 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F91 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F92 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F93 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F94 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F95 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F96 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F97 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F98 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F99 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F100 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F101 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F102 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F103 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F104 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F105 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F106 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F107 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F108 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F109 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F110 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F111 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F112 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F113 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F114 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F115 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F116 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F117 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F118 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F119 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F120 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F121 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F122 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F123 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F124 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F125 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F126 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F127 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F128 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F129 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F130 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F131 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F132 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F133 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F134 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F135 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F136 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F137 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F138 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F139 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F140 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F141 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F142 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F143 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F144 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F145 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F146 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F147 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F148 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F149 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F150 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F151 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F152 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F153 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F154 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F155 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F156 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F157 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F158 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F159 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F160 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F161 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys3] Frame F162 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T3 Ejecutando (4 u)
[info] --- [Sys3] Frame F163 ---
[info]   T1 Ejecutando (1 u)
[info]   T2 Ejecutando (3 u)
[info]   T4 Ejecutando (4 u)
[info] --- [Sys3] Frame F164 ---
[info]   T1 Ejecutando (1 u)
[info]   T4 Ejecutando (2 u)
[info] === [Sys3] Hiperperiodo Completado ===
[info] 

>>> 4/4. Ejecutando Sistema 4 (H=120, f=3) <<<
[info] --- [Sys4] Frame F0 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F1 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys4] Frame F2 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F3 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F4 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F5 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F6 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F7 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F8 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F9 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys4] Frame F10 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F11 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F12 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F13 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F14 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F15 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F16 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F17 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys4] Frame F18 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F19 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F20 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F21 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F22 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F23 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F24 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F25 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys4] Frame F26 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F27 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F28 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F29 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F30 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F31 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F32 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F33 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T3 Ejecutando (2 u)
[info] --- [Sys4] Frame F34 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F35 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F36 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F37 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] --- [Sys4] Frame F38 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T2 Ejecutando (1 u)
[info] --- [Sys4] Frame F39 ---
[info]   T1 Ejecutando (0.5 u)
[info]   T4 Ejecutando (2 u)
[info] === [Sys4] Hiperperiodo Completado ===
[info] 

==================================================
[info]   PRUEBAS FINALIZADAS CON EXITO EN TODOS LOS SISTEMAS
[info] ==================================================
