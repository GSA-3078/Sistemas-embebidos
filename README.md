# 🚀 Sistemas Embebidos – Prácticas con ESP32

![ESP32](https://img.shields.io/badge/ESP32-DevKit%20V1-blue?logo=espressif)
![ESP32-C3](https://img.shields.io/badge/ESP32--C3-Super%20Mini-red?logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange?logo=platformio)
![Arduino](https://img.shields.io/badge/Framework-Arduino-teal?logo=arduino)
![VS Code](https://img.shields.io/badge/Visual%20Studio%20Code-IDE-blue?logo=visualstudiocode)
![GitHub](https://img.shields.io/badge/GitHub-Version%20Control-black?logo=github)

---

## 📚 1. Introducción

Este repositorio reúne las **prácticas de la asignatura de Sistemas Embebidos**, desarrolladas con **ESP32** y programadas en **C++ con el framework Arduino** usando **PlatformIO** en Visual Studio Code.

Las prácticas van de lo sencillo a lo complejo: primero se controlan cargas simples (un LED, un motor y un foco), después se construyen sistemas de control de nivel y de temperatura con sensores, pantalla LCD y teclado, hasta llegar a un **control PID** de un horno.

Cada práctica tiene su propia carpeta con su código, su configuración de PlatformIO y su documentación.

```text
                    SISTEMAS EMBEBIDOS
                            │
                            ▼
        ┌───────────────────┴───────────────────┐
        │                                       │
        ▼                                       ▼
  Salidas y potencia                   Control y sensores
  (LED, motor, foco, relé)       (agua, temperatura, teclado)
        │                                       │
        ▼                                       ▼
   Cargas simples            ON/OFF ──► P ──► PID · Dimmer
```

---

## 🎯 2. Objetivo

Aplicar los fundamentos de los sistemas embebidos mediante prácticas que permitan al estudiante:

1. 🔌 Controlar cargas con transistores y módulos de relé.
2. 🌡️ Leer sensores analógicos y digitales (LM35, DS18B20, PIR).
3. 🖥️ Mostrar información en una pantalla LCD 16x2 con I2C.
4. ⌨️ Capturar datos con botones, potenciómetros y un teclado matricial 4x4.
5. 💾 Guardar configuración en memoria no volátil (`Preferences`).
6. ⚙️ Controlar servomotores y regular potencia con un dimmer.
7. 🎛️ Implementar control ON/OFF con histéresis, proporcional (P) y PID.
8. 🔊 Reproducir audio con un DFPlayer Mini y un amplificador.
9. 📝 Documentar cada práctica y respaldarla en GitHub.

---

## 🧰 3. Herramientas

| Herramienta | Función |
|---|---|
| Visual Studio Code | Entorno de desarrollo |
| PlatformIO IDE | Compilación, librerías y carga al ESP32 |
| Arduino Framework | Framework de programación |
| Git y GitHub | Control de versiones y respaldo |

---

## 🧪 4. Lista de prácticas

| # | Carpeta | Práctica | Componentes principales |
|:-:|---|---|---|
| 01 | `01_LED_Motor_Foco` | Las tres primeras prácticas con ESP32: LED, motor y foco | LED, motor con transistor, foco con relé, botones |
| 02 | `02_Control_Agua` | Control de agua | 2 bombas de diafragma, 2 relés, LCD, 3 botones |
| 03 | `03_Horno_Botones` | Horno con botones | DS18B20, foco, Peltier, extractor, relés, LCD, botones |
| 04 | `04_Dimmer` | Dimmer controlado con servo | Servo, dimmer MY-989, foco, LCD, 2 botones |
| 05 | `05_Horno_Potenciometros` | Horno con potenciómetros | LM35, 2 potenciómetros, relés, LCD |
| 06 | `06_Horno_Teclado_4x4` | Horno con teclado matricial 4x4 | LM35, teclado 4x4, relés, LCD, `Preferences` |
| 07 | `07_Animatronico` | Animatrónico que grita | ESP32-C3, sensor PIR, 3 servos, DFPlayer Mini, PAM8403, bocinas |
| 08 | `08_Control_P_Horno` | Control P del horno | DS18B20, teclado 4x4, servo + dimmer, LCD |
| 09 | `09_Control_PID_Horno` | Control PID del horno | DS18B20, teclado 4x4, servo + dimmer, LCD |

---

## 📦 5. Librerías por práctica

Cada práctica declara sus librerías en su `platformio.ini`, dentro de `lib_deps`. Al clonar el proyecto, PlatformIO las instala automáticamente.

| Librería | Se usa en |
|---|---|
| `marcoschwartz/LiquidCrystal_I2C` | Prácticas con LCD: 02, 03, 04, 05, 06, 08, 09 |
| `chris--a/Keypad` | Teclado 4x4: 06, 08, 09 |
| `madhephaestus/ESP32Servo` | Servomotores: 04, 07, 08, 09 |
| `paulstoffregen/OneWire` | DS18B20: 03, 08, 09 |
| `milesburton/DallasTemperature` | DS18B20: 03, 08, 09 |
| `dfrobot/DFRobotDFPlayerMini` | Audio: 07 |

`Wire`, `Preferences` y `Arduino.h` ya vienen incluidas en el framework y no se declaran. El LM35 (prácticas 05 y 06) se lee con una entrada analógica y no necesita librería.

**Ejemplo de `platformio.ini`** (práctica 09, control PID):

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
lib_deps =
    marcoschwartz/LiquidCrystal_I2C
    chris--a/Keypad
    milesburton/DallasTemperature
    paulstoffregen/OneWire
    madhephaestus/ESP32Servo
```

> ⚠️ La práctica 07 (animatrónico) usa un **ESP32-C3**, por lo que su `platformio.ini` cambia de placa (`esp32-c3-devkitm-1`) y lleva otras banderas de compilación. Revisa el de su carpeta.

---

## 📁 6. Estructura del repositorio

```text
Sistemas_Embebidos/
│
├── README.md
├── .gitignore
│
├── 01_LED_Motor_Foco/
│   ├── README.md
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
│
├── 02_Control_Agua/
│   ├── README.md
│   ├── platformio.ini
│   └── src/
│       └── main.cpp
│
├── 03_Horno_Botones/
│   └── ...
│
├── 04_Dimmer/
│   └── ...
│
├── 05_Horno_Potenciometros/
│   └── ...
│
├── 06_Horno_Teclado_4x4/
│   └── ...
│
├── 07_Animatronico/
│   └── ...
│
├── 08_Control_P_Horno/
│   └── ...
│
└── 09_Control_PID_Horno/
    ├── README.md
    ├── platformio.ini
    └── src/
        └── main.cpp
```

### 📄 Contenido de cada práctica

| Archivo | Función |
|---|---|
| `README.md` | Documentación de la práctica |
| `platformio.ini` | Placa, framework y librerías |
| `src/main.cpp` | Código del programa |

---

## ▶️ 7. Cómo abrir y cargar una práctica

1. **Clona** el repositorio o descárgalo como ZIP.
2. Abre **VS Code** y selecciona `File → Open Folder`.
3. Abre la **carpeta de la práctica** (no la carpeta general), porque ahí está su `platformio.ini`.
4. Espera a que PlatformIO instale las librerías.
5. Compila con **Build** (✔).
6. Conecta el ESP32 por USB y carga con **Upload** (→).
7. Abre el **Monitor Serial** (🔌) a la velocidad indicada en `monitor_speed`.

```bash
pio run                 # compilar
pio run -t upload       # cargar al ESP32
pio device monitor      # monitor serial
```

---

## 🔌 8. Convenciones usadas

| Tema | Convención |
|---|---|
| Placa principal | ESP32 DevKit de 30 pines (el animatrónico usa ESP32-C3) |
| Módulos de relé | **Activos en LOW** (`LOW` = encendido) |
| LCD I2C | Dirección `0x27`, SDA en GPIO21 y SCL en GPIO22 |
| Alimentación de servos | 5 V, con GND común con el ESP32 |
| Cargas de 12 V | Fuente externa conmutada con módulos de relé |
| Carga de 127 V CA | Solo por contactos de relé o dimmer |
| Teclado 4x4 | Filas en GPIO13, 14, 27, 26 · columnas en GPIO25, 33, 32, 23 |
| Monitor serial | 115200 baudios (salvo que la práctica indique otra velocidad) |

---

## 📈 9. Evolución de las prácticas de temperatura

Las prácticas del horno forman una secuencia que permite comparar estrategias de control:

| Etapa | Práctica | Estrategia | Sensor | Actuador |
|---|---|---|---|---|
| 1 | 03 Horno con botones | ON/OFF con histéresis | DS18B20 | Relés |
| 2 | 05 Horno con potenciómetros | ON/OFF con límites ajustables (potenciómetros) | LM35 | Relés |
| 3 | 06 Horno con teclado 4x4 | ON/OFF con límites ajustables (teclado) | LM35 | Relés |
| 4 | 08 Control P | Control **proporcional (P)** | DS18B20 | Servo + dimmer |
| 5 | 09 Control PID | Control **PID** con anti-windup | DS18B20 | Servo + dimmer |

Con el monitor serial del PID se puede guardar un registro en CSV y graficar la respuesta del sistema en Excel.

---

## ⚠️ 10. Seguridad

Varias prácticas trabajan con **127 V de corriente alterna** (foco, dimmer) y con **12 V de corriente alta** (Peltier y ventiladores):

- 🚫 Arma y modifica los circuitos siempre **desconectados**.
- 📦 Deja las conexiones de 127 V dentro de una **caja cerrada** y aislada.
- 🔥 No dejes el horno funcionando sin supervisión.
- 🧯 Verifica que relés, dimmer y cables soporten la corriente de la carga.
- ⚡ No alimentes servos ni cargas de potencia desde un GPIO.

---

## 🚫 11. Archivos que no se suben

El `.gitignore` excluye los archivos generados automáticamente:

```gitignore
# PlatformIO
.pio/
.piolibdeps/

# Visual Studio Code
.vscode/

# Archivos compilados
*.bin
*.elf
*.hex

# Sistema operativo
.DS_Store
Thumbs.db
```

---

## 📋 12. Checklist antes de entregar una práctica

* [ ] Carpeta con nombre numerado.
* [ ] `src/main.cpp` compila sin errores.
* [ ] `platformio.ini` con las librerías necesarias.
* [ ] `README.md` con descripción, materiales, conexiones y pruebas.
* [ ] Probada en hardware físico.
* [ ] Sin archivos `.pio/` ni `.vscode/` en el repositorio.

---

## 🧠 13. Competencia desarrollada

Al terminar las prácticas, el estudiante será capaz de:

> **Diseñar, programar, documentar y probar sistemas embebidos basados en ESP32 que integren sensores, actuadores, interfaces de usuario y estrategias de control, aplicando buenas prácticas de seguridad y control de versiones.**

---
