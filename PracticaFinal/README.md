# 🌡️ Práctica 10: Control PID de Temperatura con HMI Nextion
### 🔥 ESP32 + DS18B20 + Foco con Dimmer + Peltier + Pantalla Nextion NX3224F028

![ESP32](https://img.shields.io/badge/ESP32-DevKit-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange?style=for-the-badge&logo=platformio&logoColor=white)
![Control](https://img.shields.io/badge/Control-PID-blue?style=for-the-badge)
![HMI](https://img.shields.io/badge/HMI-Nextion%202.8%22-yellow?style=for-the-badge)

---

## 📌 Descripción

Esta práctica implementa un **controlador PID de temperatura** en un **ESP32**, con una **HMI táctil Nextion** para visualizar y configurar el sistema en tiempo real.

La temperatura se mide con un sensor **DS18B20**. El calentamiento se realiza con un **foco de 100 W** cuya potencia se regula con un **dímer accionado por un servomotor**, y el sistema incluye una **celda Peltier** con su **extractor**, controlados mediante módulos de relé.

Desde la pantalla Nextion el usuario puede:

- 👁️ Ver la **temperatura de proceso (VP)**, el **setpoint (SP)**, el **error** y la **salida de control**.
- 📈 Ver la **gráfica de evolución** de la temperatura y del setpoint.
- ✏️ Modificar **SP, Kp, Ki y Kd** y aplicarlos al controlador.

---

## 🎯 Objetivos

- Implementar un controlador **PID completo** con anti-windup.
- Medir la temperatura con un **DS18B20** de forma no bloqueante.
- Regular un foco de 127 V CA con un **dímer movido por un servo**.
- Diseñar una **HMI** para monitoreo y configuración en tiempo real.
- Guardar los parámetros en memoria no volátil (**NVS**).

---

# ⚙️ Control PID

El error se define como:

$$
e = SP - T
$$

Las tres acciones del controlador son:

$$
P = K_p \cdot e
$$

$$
I = I + K_i \cdot e \cdot T_s
$$

$$
D = -K_d \cdot \frac{T - T_{anterior}}{T_s}
$$

La salida se limita al rango 0 a 100 %:

$$
u = P + I + D \qquad (0\% \le u \le 100\%)
$$

El ángulo del servo se calcula con:

$$
\theta = \theta_{min} + \frac{u}{100}\,(\theta_{max}-\theta_{min})
$$

| Característica | Descripción |
|---|---|
| 🧠 **Derivada sobre la medición** | Evita saltos bruscos al cambiar el setpoint |
| 🛡️ **Anti-windup** | Integración condicional + limitación de 0 a 100 % |
| ⏱️ **Periodo de control** | 1 s (conversión del DS18B20 de 12 bits) |
| 🔥 **Seguridad** | A 80 °C el foco se apaga (con histéresis de 2 °C) |

---

# 🧰 Componentes

| Componente | Función |
|---|---|
| 🤖 ESP32 DevKit | Microcontrolador principal |
| 🌡️ DS18B20 | Sensor de temperatura digital |
| 💡 Foco 100 W (127 V CA) | Actuador de calentamiento |
| 🎚️ Dímer MY-9892 + servo SG90 | Regulación de potencia del foco |
| ❄️ Celda Peltier + disipadores + ventiladores | Elemento de enfriamiento (12 V) |
| 🌀 Extractor 12 V | Ventilación |
| 🔌 2 módulos de relé SRD-05VDC | Control de Peltier y extractor |
| 🖥️ Nextion Discovery NX3224F028 (2.8") | HMI táctil |
| 🔋 Fuente de 12 V y fuente de 5 V | Alimentación |
| 🧱 Resistencia de 4.7 kΩ | Pull-up del DS18B20 |

---

# 🔌 Conexiones

| Dispositivo | Terminal | ESP32 |
|---|---|---|
| DS18B20 | DATA | GPIO 4 (4.7 kΩ a 3.3 V) |
| Servo (dímer) | Señal | GPIO 18 (5 V externos, GND común) |
| Relé Peltier | IN | GPIO 19 |
| Relé extractor | IN | GPIO 23 |
| Nextion | TX | GPIO 16 (RX2) |
| Nextion | RX | GPIO 17 (TX2) |
| Nextion | 5V / GND | Fuente de 5 V / GND común |

> Los relés son **activos en LOW**. Si tus módulos funcionan al revés, cambia `RELAY_ACTIVO` a `HIGH`.

### ⚠️ Seguridad

- El foco y el dímer trabajan con **127 V CA**. Aísla bien todas las conexiones y no las manipules energizadas.
- Todos los **GND** (ESP32, servo, relés, Nextion, fuentes) deben estar **unidos**.
- El dímer debe estar **calibrado**: `ANGULO_MIN = 55°` (0 % de luz) y `ANGULO_MAX = 180°` (100 %).

---

# 🖥️ HMI Nextion

La pantalla se comunica por **UART2 a 9600 baudios**. El baud debe coincidir con `baud=9600` en el `Program.s` de la Nextion.

## 📤 ESP32 → Pantalla

| Componente | Contenido | Escala |
|---|---|---|
| `x_vp` | Temperatura de proceso | T × 10 |
| `x_sp` | Setpoint | SP × 10 |
| `x_err` | Error | e × 10 |
| `n_out` | Salida de control | 0 a 100 % |
| `w_graf` (id 8) | Gráfica: canal 0 = VP, canal 1 = SP | Altura 135 px |
| `t_estado` | Estado: `OK`, `SOBRETEMP` o `ERROR SENSOR` | Opcional |
| `cx_sp`, `cx_kp`, `cx_ki`, `cx_kd` | Parámetros guardados en la página de configuración | SP × 10, Kp × 10, Ki × 100, Kd × 10 |

La gráfica cubre el rango de **20 °C a 50 °C** y guarda el historial de **120 puntos** (1 punto por segundo).

## 📥 Pantalla → ESP32

| Byte recibido | Acción |
|---|---|
| `0x52` | Se entró a la página del monitor: el ESP32 limpia y **redibuja la gráfica** con el historial |
| `0x55` + 16 bytes | **Aplicar**: recibe SP, Kp, Ki y Kd como enteros de 32 bits (little-endian) |

### Límites de los parámetros

| Parámetro | Mínimo | Máximo | Valor inicial |
|---|---|---|---|
| SP | 0 | 75 °C | 40 °C |
| Kp | 0 | 100 | 10 |
| Ki | 0 | 10 | 0 |
| Kd | 0 | 100 | 0 |

Al aplicar nuevos parámetros, el término integral se reinicia para evitar saltos del servo, y los valores se guardan en memoria (`Preferences`, espacio `horno`).

---

# 📊 Registro por puerto serie

Cada segundo se imprime una línea en formato CSV a **115200 baudios**, útil para graficar en Excel:

```text
t_s,SP,T,E,P,I,D,U,Servo
```

| Campo | Descripción |
|---|---|
| `t_s` | Tiempo desde el arranque (s) |
| `SP` | Setpoint (°C) |
| `T` | Temperatura medida (°C) |
| `E` | Error (°C) |
| `P`, `I`, `D` | Aporte de cada acción |
| `U` | Salida de control (%) |
| `Servo` | Ángulo del servo (°) |

---

# 🚀 Instalación

1. Clona el repositorio y abre la carpeta `Practica10` en **VS Code con PlatformIO**.
2. Copia el `platformio.ini` y el código en `src/main.cpp`.
3. Carga el archivo `.HMI` en la pantalla Nextion (desde la tarjeta microSD o con el editor Nextion).
4. Conecta el ESP32, compila y sube el programa:

```bash
pio run --target upload
pio device monitor
```

---

# 🛡️ Protecciones

| Situación | Respuesta |
|---|---|
| ❌ DS18B20 desconectado o lectura inválida | Foco apagado (servo en el ángulo mínimo), PID reiniciado y estado `ERROR SENSOR` |
| 🔥 Temperatura ≥ 80 °C | Foco apagado y estado `SOBRETEMP`; se reanuda al bajar a 78 °C |
| 🔢 Valor fuera de rango en la HMI | Se limita al máximo permitido |

---

# 📁 Estructura de la práctica

```text
Practica10/
│
├── img/                       → Esquema de conexiones y capturas
├── src/
│   └── main.cpp               → Código del ESP32
├── nextion/
│   └── proyecto.HMI           → Proyecto de la pantalla
├── platformio.ini
└── README.md
```
