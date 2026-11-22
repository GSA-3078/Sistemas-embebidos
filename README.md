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

Las prácticas van de lo sencillo a lo complejo: primero se controlan cargas simples (un motor, un foco) con transistores y relés, después se construyen sistemas de control de temperatura con sensores, pantalla LCD y teclado, hasta llegar a un **control PID** de un horno.

Cada práctica tiene su propia carpeta con su código, su configuración de PlatformIO, su diagrama de conexiones y su documentación.

```text
                    SISTEMAS EMBEBIDOS
                            │
                            ▼
        ┌───────────────────┴───────────────────┐
        │                                       │
        ▼                                       ▼
  Salidas y potencia                   Control y sensores
  (transistor, relé)           (temperatura, nivel, teclado)
        │                                       │
        ▼                                       ▼
   Motor · Foco             ON/OFF ──► P ──► PID · Dimmer
```

---

## 🎯 2. Objetivo

Aplicar los fundamentos de los sistemas embebidos mediante prácticas que permitan al estudiante:

1. 🔌 Controlar cargas con transistores y módulos de relé.
2. 🌡️ Leer sensores analógicos y digitales (LM35, DS18B20, PIR).
3. 🖥️ Mostrar información en una pantalla LCD 16x2 con I2C.
4. ⌨️ Capturar datos con botones y con un teclado matricial 4x4.
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

> 💡 Si todavía no tienes el entorno listo, consulta la carpeta `00_Instalacion_Entorno`.

---

## 🧪 4. Lista de prácticas

| # | Carpeta | Práctica | Componentes principales |
|:-:|---|---|---|
| 00 | `00_Instalacion_Entorno` | Instalación y configuración del entorno | VS Code, PlatformIO, GitHub |
| 01 | `01_Motor_Transistor` | Control de un motor con transistor | Motor, transistor, 2 botones |
| 02 | `02_Rele_Foco` | Control de un foco con relé | Relé, foco, 2 botones |
| 03 | `03_Rele_Fisico` | Relé con el módulo físico | Módulo de relé, foco |
| 04 | `04_Rele_2N2222` | Activación del relé con transistor 2N2222 | 2N2222, módulo de relé |
| 05 | `05_Control_Nivel_Bombas` | Control de nivel con bombas de diafragma | 2 bombas, 2 relés, LCD, 3 botones |
| 06 | `06_Horno_ONOFF_DS18B20` | Horno ON/OFF con DS18B20 | DS18B20, foco, Peltier, extractor, relés, LCD |
| 07 | `07_Horno_Peltier` | Horno con Peltier, disipadores y ventiladores | Peltier, 2 ventiladores, relés, 12 V |
| 08 | `08_Dimmer_Servo` | Dimmer controlado con servo | Servo, dimmer MY-989, foco, LCD, 2 botones |
| 09 | `09_Horno_LM35_Potenciometros` | Horno con LM35 y límites por potenciómetro | LM35, 2 potenciómetros, relés, LCD |
| 10 | `10_Horno_LM35_Teclado` | Horno con LM35 y límites por teclado | LM35, teclado 4x4, relés, LCD, `Preferences` |
| 11 | `11_Animatronico` | Animatrónico que grita | ESP32-C3, PIR, 3 servos, DFPlayer Mini, PAM8403 |
| 12 | `12_Horno_Control_P` | Horno con control proporcional | DS18B20, teclado, servo + dimmer, LCD |
| 13 | `13_Horno_Control_PID` | Horno con control PID | DS18B20, teclado, servo + dimmer, LCD |

> 📝 Ajusta los nombres y el orden de la tabla a como tengas numeradas tus prácticas.

---

## 📦 5. Librerías por práctica

Cada práctica declara sus librerías en su `platformio.ini`, dentro de `lib_deps`. Al clonar el proyecto, PlatformIO las instala automáticamente.

| Librería | Se usa en |
|---|---|
| `marcoschwartz/LiquidCrystal_I2C` | Prácticas con LCD (05, 06, 07, 08, 09, 10, 12, 13) |
| `chris--a/Keypad` | Teclado 4x4 (10, 12, 13) |
| `madhephaestus/ESP32Servo` | Servomotores (08, 11, 12, 13) |
| `paulstoffregen/OneWire` | DS18B20 (06, 07, 12, 13) |
| `milesburton/DallasTemperature` | DS18B20 (06, 07, 12, 13) |
| `dfrobot/DFRobotDFPlayerMini` | Audio (11) |

`Wire`, `Preferences` y `Arduino.h` ya vienen incluidas en el framework y no se declaran.

**Ejemplo de `platformio.ini`** (horno con control PID):

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

> ⚠️ La práctica del animatrónico usa un **ESP32-C3**, por lo que su `platformio.ini` cambia de placa (`esp32-c3-devkitm-1`) y usa otras banderas de compilación. Revisa el de su carpeta.

---

## 📁 6. Estructura del repositorio

```text
Sistemas_Embebidos/
│
├── README.md
├── .gitignore
│
├── 00_Instalacion_Entorno/
│   └── README.md
│
├── 01_Motor_Transistor/
│   ├── README.md
│   ├── platformio.ini
│   ├── diagrama_conexiones.png
│   └── src/
│       └── main.cpp
│
├── 02_Rele_Foco/
│   └── ...
│
├── ...
│
└── 13_Horno_Control_PID/
    ├── README.md
    ├── platformio.ini
    ├── diagrama_conexiones.png
    └── src/
        └── main.cpp
```

### 📄 Contenido de cada práctica

| Archivo | Función |
|---|---|
| `README.md` | Documentación de la práctica |
| `platformio.ini` | Placa, framework y librerías |
| `src/main.cpp` | Código del programa |
| `diagrama_conexiones.png` | Esquema de conexiones |

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

| Etapa | Práctica | Estrategia | Actuador |
|---|---|---|---|
| 1 | 06 y 07 | ON/OFF con histéresis | Relés |
| 2 | 09 y 10 | ON/OFF con límites ajustables (potenciómetros y teclado) | Relés |
| 3 | 12 | Control **proporcional (P)** | Servo + dimmer |
| 4 | 13 | Control **PID** con anti-windup | Servo + dimmer |

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
* [ ] Diagrama de conexiones incluido.
* [ ] Probada en hardware físico.
* [ ] Sin archivos `.pio/` ni `.vscode/` en el repositorio.

---

## 🧠 13. Competencia desarrollada

Al terminar las prácticas, el estudiante será capaz de:

> **Diseñar, programar, documentar y probar sistemas embebidos basados en ESP32 que integren sensores, actuadores, interfaces de usuario y estrategias de control, aplicando buenas prácticas de seguridad y control de versiones.**

---

## 👨‍🏫 Sistemas Embebidos

**ESP32 · PlatformIO · Arduino · GitHub**

**Autor:** _(escribe aquí tu nombre y grupo)_

**Licencia:** proyecto con fines educativos bajo licencia **MIT**.
