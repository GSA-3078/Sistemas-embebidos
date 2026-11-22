# 🔥 Horno con control PID: ESP32, DS18B20, teclado 4x4, dimmer con servo y LCD I2C

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue?logo=espressif&style=for-the-badge)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange?logo=platformio&style=for-the-badge)
![Sensor](https://img.shields.io/badge/Sensor-DS18B20-green?style=for-the-badge)
![Control](https://img.shields.io/badge/Control-PID-red?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-lightgrey?style=for-the-badge)

### 🎛️ Control automático de temperatura con anti-windup, derivada sobre la medición y registro de datos por serial

---

## 📌 1. Descripción del proyecto

Este proyecto es un **horno con control PID (Proporcional, Integral y Derivativo)** hecho con un **ESP32**. El sistema mide la temperatura con un sensor digital **DS18B20**, calcula la potencia que necesita el foco y la aplica girando un **servomotor** acoplado a la perilla de un **dimmer MY-9892 (MY-989)**.

Los cuatro parámetros del controlador (**SP, Kp, Ki y Kd**) se capturan con un **teclado 4x4**, se muestran en una **LCD 16x2 I2C** y se guardan en la memoria flash (`Preferences`), por lo que se conservan al apagar el equipo.

Respecto al control P, aquí se agregan:

- ➕ Acción **integral** con protección **anti-windup**.
- 📉 Acción **derivativa** calculada sobre la temperatura medida.
- 🛑 **Paro por sobretemperatura** a 80 °C.
- 📊 **Registro en formato CSV** por el monitor serial para graficar en Excel.
- 🖥️ **Tres pantallas** en la LCD.

> ⚠️ **Seguridad:** el foco y el dimmer trabajan con **127 V de corriente alterna**. Arma y modifica el circuito siempre **desconectado** y deja el dimmer y las conexiones dentro de una caja cerrada. El servo solo debe mover la perilla del dimmer, sin tocar sus conexiones eléctricas.

---

## 🎯 2. Objetivos

- 🌡️ Medir la temperatura con un DS18B20 de 12 bits sin bloquear el programa.
- 🧮 Implementar un controlador PID discreto con saturación de 0 a 100 %.
- 🛡️ Evitar el *integral windup* con integración condicional y límite del término integral.
- 📐 Convertir la salida del controlador en un ángulo para el servo del dimmer.
- ⌨️ Editar SP, Kp, Ki y Kd desde el teclado y guardarlos en memoria no volátil.
- 🖥️ Visualizar temperatura, error, salida, ganancias y términos P, I y D.
- 📈 Registrar los datos por serial para analizar la respuesta del sistema.
- 🔒 Llevar el foco a una condición segura ante falla del sensor o sobretemperatura.

---

## 🧰 3. Materiales

| Cantidad | Componente | Notas |
|---:|---|---|
| 1 | ESP32 DevKit (30 pines) | |
| 1 | Sensor DS18B20 | Digital, 1-Wire |
| 1 | Resistencia de 4.7 kΩ | Pull-up entre DATA y 3.3 V |
| 1 | Teclado matricial 4x4 | 8 pines |
| 1 | LCD 16x2 con módulo I2C | Dirección `0x27` |
| 1 | Servomotor (SG90 o similar) | Alimentado con 5 V externos |
| 1 | Dimmer MY-9892 (MY-989) 2000 W | Regulador AC con perilla |
| 1 | Acople servo-perilla | Une el eje del servo con la perilla |
| 2 | Módulos de relé SRD-05VDC | Activos en LOW |
| 1 | Celda Peltier + 2 disipadores + 2 ventiladores | 12 V, en paralelo |
| 1 | Ventilador extractor | 12 V |
| 1 | Foco incandescente + clavija | 127 V CA, compatible con el dimmer |
| 1 | Fuente de 12 V (≥ 8 A) | Peltier, ventiladores y extractor |
| 1 | Fuente de 5 V | Para el servo, con GND común con el ESP32 |
| — | Cables y jumpers | |

> 💡 Un SG90 puede no tener torque suficiente si la perilla del dimmer está dura. En ese caso usa un servo MG90S o MG996R.

---

## 🧠 4. Principio de funcionamiento

### 4.1 📏 Error

```
e = SP - T
```

| Variable | Descripción | Unidad |
|---|---|---|
| e | Error de temperatura | °C |
| SP | Temperatura deseada (setpoint) | °C |
| T | Temperatura medida | °C |

### 4.2 🅿️ Acción proporcional

```
P = Kp × e
```

Responde de inmediato al tamaño del error. Un Kp muy alto provoca oscilaciones y sobreimpulso.

### 4.3 ➕ Acción integral con anti-windup

```
I = I + Ki × e × Ts
```

Acumula el error con el tiempo y elimina el error permanente que deja el control P. Para evitar el *windup* el programa:

1. **Congela la integral** si la salida ya está saturada y el error la empuja todavía más hacia ese límite.
2. **Limita** el término integral entre 0 y 100 %.
3. **Reinicia** la integral cuando se cambia Ki, cuando falla el sensor o cuando hay sobretemperatura.

### 4.4 📉 Acción derivativa sobre la temperatura

```
D = -Kd × (T - Tanterior) / Ts
```

Se calcula sobre la **temperatura medida** y no sobre el error, para que un cambio brusco del SP no genere un golpe derivativo. Si la temperatura sube rápido, D resta potencia y anticipa el frenado.

### 4.5 🧾 Salida del controlador

```
u = P + I + D          (limitada a 0 % ≤ u ≤ 100 %)
```

- Ts es el tiempo real transcurrido entre cálculos (aprox. **1 s**).
- Si T es mayor que SP, el error es negativo y `u` se limita a 0 %, es decir, el foco se apaga.

### 4.6 📐 Conversión a ángulo del servo

```
ángulo = 122 + (u / 100) × (180 - 122)
```

| Constante | Valor | Significado |
|---|---|---|
| `ANGULO_MIN` | 122° | 0 % de luz |
| `ANGULO_MAX` | 180° | 100 % de luz |

| u | Ángulo del servo |
|---:|---:|
| 0 % | 122° |
| 25 % | 137° |
| 50 % | 151° |
| 75 % | 166° |
| 100 % | 180° |

Estos ángulos dependen de cómo quedó acoplado el servo a tu dimmer. Si el foco no apaga o no llega al máximo, ajusta `ANGULO_MIN` y `ANGULO_MAX`.

### 4.7 🧪 Ejemplo numérico

Con SP = 40 °C, Kp = 5, Ki = 0.1, Kd = 2, Ts = 1 s, una temperatura que pasa de 29.8 °C a 30.0 °C y una integral previa de 12 %:

| Término | Cálculo | Resultado |
|---|---|---:|
| e | 40 − 30.0 | 10.0 °C |
| P | 5 × 10 | 50.0 % |
| I | 12 + 0.1 × 10 × 1 | 13.0 % |
| D | −2 × (30.0 − 29.8) / 1 | −0.4 % |
| **u** | 50 + 13 − 0.4 | **62.6 %** |
| **Ángulo** | 122 + 0.626 × 58 | **158°** |

---

## ⌨️ 5. Interfaz de usuario (teclado)

| Tecla | Función |
|---|---|
| `A` | Capturar **SP** (temperatura deseada) |
| `B` | Capturar **Kp** |
| `C` | Capturar **Ki** |
| `D` | Capturar **Kd** |
| `0`–`9` | Escribir el valor (hasta 5 caracteres) |
| `*` | Punto decimal (si no hay número, escribe `0.`) |
| `#` | Confirmar el valor · con el campo vacío cancela · fuera de captura cambia de pantalla |

### 📋 Reglas de captura

| Parámetro | Rango permitido | Valor inicial |
|---|---|---|
| SP | 0 a 99 °C | 40 |
| Kp | 0 a 100 | 5 |
| Ki | 0 a 10 | 0 |
| Kd | 0 a 100 | 0 |

- Volver a presionar la letra durante una captura **reinicia el número**.
- Si pasan **10 s** sin presionar teclas, la captura se cancela.
- Los avisos duran **2 s**.
- Los valores se guardan al confirmar y se conservan al reiniciar.

---

## 🖥️ 6. Visualización en LCD

La tecla `#` (fuera de captura) cambia entre tres pantallas.

**Pantalla 1: proceso**

```
T:30.5C SP:40.0
E: 9.5C OUT: 47%
```

**Pantalla 2: ganancias y servo**

```
Kp:12.0 Ki:0.80
Kd: 2.0 SER:158
```

**Pantalla 3: términos del PID**

```
P: 30.0 I: 12.5
D: -1.2 U: 41%
```

**Captura de valor**

```
Nuevo Kp: 12_
#:OK #vacio:Sal
```

**Avisos:** `SP = 45.00 / Guardado`, `Fuera de rango / Usa 0 a 99`, `Captura / cancelada`.

Si el sensor falla, la segunda línea muestra `Error DS18B20`.

---

## 🔌 7. Conexiones

### 📟 ESP32

| Elemento | Pin del ESP32 |
|---|---|
| DS18B20, DATA | GPIO4 (4.7 kΩ a 3.3 V) |
| DS18B20, VCC / GND | 3V3 / GND |
| Servo, señal | GPIO18 |
| Servo, alimentación / tierra | 5 V externos / GND común |
| Relé Peltier, IN | GPIO16 |
| Relé extractor, IN | GPIO17 |
| Relés, VCC / GND | VIN (5 V) / GND |
| LCD I2C, SDA / SCL | GPIO21 / GPIO22 |
| LCD I2C, VCC / GND | VIN (5 V) / GND |
| Teclado, filas R1–R4 | GPIO13, 14, 27, 26 |
| Teclado, columnas C1–C4 | GPIO25, 33, 32, 23 |

### ⚡ Lado de 12 V (relés)

| Relé | Terminal | Conexión |
|---|---|---|
| Extractor | COM | + de la fuente de 12 V |
| Extractor | NO | + del extractor |
| Peltier | COM | + de la fuente de 12 V (unido al COM del extractor) |
| Peltier | NO | + del Peltier y de sus 2 ventiladores (en paralelo) |
| — | Negativo de la fuente | A los − del extractor, Peltier y ventiladores |

### 💡 Lado de 127 V CA (dimmer y foco)

| Origen | Destino |
|---|---|
| Clavija, fase (cable negro) | Dimmer, terminal de ENTRADA |
| Clavija, neutro (cable blanco) | Dimmer, terminal de ENTRADA |
| Dimmer, terminales de SALIDA | Foco (sus dos terminales) |
| Eje del servo | Perilla del dimmer (con el acople) |

> 🔎 Revisa la serigrafía de tu dimmer y de tus módulos de relé: el orden de las terminales puede cambiar según el fabricante.

---

## 📁 8. Estructura del proyecto (PlatformIO)

```
proyecto/
├── platformio.ini
└── src/
    └── main.cpp
```

**platformio.ini**

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

`Wire` y `Preferences` ya vienen incluidas en el framework.

---

## 🚀 9. Compilación y carga

1. Abre la carpeta del proyecto en VS Code con PlatformIO.
2. Verifica que `platformio.ini` tenga las librerías.
3. Compila con **Build** (✔) y carga con **Upload** (→) con el ESP32 por USB.
4. Abre el **Monitor Serial** a 115200 baudios.
5. Con la clavija **desconectada**, comprueba que el servo gire al cambiar SP, Kp, Ki o Kd.
6. Conecta la clavija solo cuando el dimmer y el foco estén listos y dentro de su caja.

---

## 📊 10. Registro de datos para Excel

En cada lectura, el monitor serial imprime una línea en formato CSV con este encabezado:

```
t_s,SP,T,E,P,I,D,U,Servo
```

| Columna | Significado |
|---|---|
| `t_s` | Tiempo desde el arranque (s) |
| `SP` | Temperatura deseada (°C) |
| `T` | Temperatura medida (°C) |
| `E` | Error (°C) |
| `P`, `I`, `D` | Términos del controlador (%) |
| `U` | Salida limitada (%) |
| `Servo` | Ángulo del servo (°) |

Copia las líneas del monitor, guárdalas como `.csv` y grafica **T y SP contra el tiempo** para obtener tiempo de establecimiento, sobreimpulso y error en estado estable.

---

## 💻 11. Código

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

#define PIN_DS18B20       4
#define PIN_SERVO         18
#define RELAY_PELTIER     16
#define RELAY_EXTRACTOR   17
#define LCD_SDA           21
#define LCD_SCL           22

#define RELAY_ACTIVO   LOW
#define RELAY_INACTIVO (!RELAY_ACTIVO)

const bool EXTRACTOR_SIEMPRE_ON = true;

const int ANGULO_MIN = 122;
const int ANGULO_MAX = 180;

const float TEMP_SEGURIDAD = 80.0;

const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pinesFilas[FILAS]       = { 13, 14, 27, 26 };
byte pinesColumnas[COLUMNAS] = { 25, 33, 32, 23 };
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensores(&oneWire);
Servo dimmer;
Preferences prefs;

enum Parametro { P_SP = 0, P_KP = 1, P_KI = 2, P_KD = 3 };
const char* nombreParam[4] = { "SP", "Kp", "Ki", "Kd" };
const float limiteParam[4] = { 99.0, 100.0, 10.0, 100.0 };

float setpoint = 40.0;
float kp       = 5.0;
float ki       = 0.0;
float kd       = 0.0;

float actual   = 0.0;
float error    = 0.0;
float termP    = 0.0;
float termI    = 0.0;
float termD    = 0.0;
float u        = 0.0;
int   angulo   = ANGULO_MIN;

float tempAnterior = 0.0;
bool  hayTempAnterior = false;
unsigned long tUltimoCalculo = 0;

bool sensorOK = false;
bool lecturaIntentada = false;

unsigned long tPeticion = 0;
bool esperandoLectura = false;
const unsigned long PERIODO_LECTURA_MS   = 1000;
const unsigned long ESPERA_CONVERSION_MS = 800;

enum Pantalla { NORMAL, CAPTURANDO, MENSAJE };
Pantalla pantalla = NORMAL;
Parametro parametroActual = P_SP;
char entrada[7] = "";
unsigned long ultimaTecla = 0;
char mensaje1[17] = "";
char mensaje2[17] = "";
unsigned long momentoMensaje = 0;
const unsigned long TIEMPO_CAPTURA_MS = 10000;
const unsigned long TIEMPO_MENSAJE_MS = 2000;

unsigned long ultimaLCD = 0;
const unsigned long INTERVALO_LCD_MS = 300;
int paginaLCD = 0;

void manejarTeclado();
void iniciarCaptura(Parametro p);
void confirmarCaptura();
void cancelarCaptura();
void mostrarMensaje(const char* l1, const char* l2);
void guardarConfiguracion();
void leerSensor();
void calcularPID();
void reiniciarPID();
void apagarFoco();
void actualizarLCD();
void imprimirLinea(int fila, const char* texto);
float* punteroParam(Parametro p);

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PELTIER, OUTPUT);
  pinMode(RELAY_EXTRACTOR, OUTPUT);
  digitalWrite(RELAY_PELTIER, RELAY_ACTIVO);
  digitalWrite(RELAY_EXTRACTOR, EXTRACTOR_SIEMPRE_ON ? RELAY_ACTIVO : RELAY_INACTIVO);

  teclado.setDebounceTime(50);

  dimmer.setPeriodHertz(50);
  dimmer.attach(PIN_SERVO, 500, 2400);
  dimmer.write(ANGULO_MIN);

  delay(500);
  Wire.begin(LCD_SDA, LCD_SCL);
  Wire.setClock(100000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Iniciando...");

  sensores.begin();
  sensores.setResolution(12);
  sensores.setWaitForConversion(false);

  prefs.begin("controlPID", false);
  setpoint = prefs.getFloat("sp", 40.0);
  kp       = prefs.getFloat("kp", 5.0);
  ki       = prefs.getFloat("ki", 0.0);
  kd       = prefs.getFloat("kd", 0.0);
  prefs.end();

  Serial.println("=== Control PID: foco con dimmer, Peltier siempre ON ===");
  Serial.println("A=SP B=Kp C=Ki D=Kd  *=punto  #=confirmar / cambiar pantalla");
  Serial.println("t_s,SP,T,E,P,I,D,U,Servo");

  delay(1000);
  lcd.clear();
}

void loop() {
  manejarTeclado();
  leerSensor();

  if (millis() - ultimaLCD >= INTERVALO_LCD_MS) {
    ultimaLCD = millis();
    actualizarLCD();
  }
}

void leerSensor() {
  if (!esperandoLectura && millis() - tPeticion >= PERIODO_LECTURA_MS) {
    sensores.requestTemperatures();
    tPeticion = millis();
    esperandoLectura = true;
  }

  if (esperandoLectura && millis() - tPeticion >= ESPERA_CONVERSION_MS) {
    esperandoLectura = false;
    lecturaIntentada = true;
    float t = sensores.getTempCByIndex(0);

    if (t == DEVICE_DISCONNECTED_C || t < -50 || t > 125) {
      sensorOK = false;
      reiniciarPID();
      apagarFoco();
      Serial.println("Error: DS18B20 no responde");
      return;
    }

    sensorOK = true;
    actual = t;
    calcularPID();
  }
}

void calcularPID() {
  unsigned long ahora = millis();
  float Ts = (tUltimoCalculo == 0) ? 1.0 : (ahora - tUltimoCalculo) / 1000.0;
  tUltimoCalculo = ahora;
  if (Ts <= 0) Ts = 1.0;

  error = setpoint - actual;

  if (actual >= TEMP_SEGURIDAD) {
    reiniciarPID();
    apagarFoco();
    tempAnterior = actual;
    hayTempAnterior = true;
    Serial.println("SOBRETEMPERATURA: foco apagado");
    return;
  }

  termP = kp * error;

  if (hayTempAnterior) termD = -kd * (actual - tempAnterior) / Ts;
  else                 termD = 0.0;
  tempAnterior = actual;
  hayTempAnterior = true;

  float integralNueva = termI + ki * error * Ts;
  float uPrueba = termP + integralNueva + termD;
  bool saturadoArriba = (uPrueba > 100.0 && error > 0);
  bool saturadoAbajo  = (uPrueba < 0.0   && error < 0);
  if (!saturadoArriba && !saturadoAbajo) termI = integralNueva;
  termI = constrain(termI, 0.0, 100.0);

  u = termP + termI + termD;
  u = constrain(u, 0.0, 100.0);

  float grados = ANGULO_MIN + (u / 100.0) * (ANGULO_MAX - ANGULO_MIN);
  angulo = (int)(grados + 0.5);
  dimmer.write(angulo);

  Serial.print(ahora / 1000.0, 1); Serial.print(",");
  Serial.print(setpoint, 1);       Serial.print(",");
  Serial.print(actual, 2);         Serial.print(",");
  Serial.print(error, 2);          Serial.print(",");
  Serial.print(termP, 1);          Serial.print(",");
  Serial.print(termI, 1);          Serial.print(",");
  Serial.print(termD, 1);          Serial.print(",");
  Serial.print(u, 1);              Serial.print(",");
  Serial.println(angulo);
}

void reiniciarPID() {
  termI = 0.0;
  termP = 0.0;
  termD = 0.0;
  hayTempAnterior = false;
}

void apagarFoco() {
  u = 0.0;
  angulo = ANGULO_MIN;
  dimmer.write(angulo);
}

float* punteroParam(Parametro p) {
  switch (p) {
    case P_SP: return &setpoint;
    case P_KP: return &kp;
    case P_KI: return &ki;
    default:   return &kd;
  }
}

void manejarTeclado() {
  if (pantalla == CAPTURANDO && millis() - ultimaTecla > TIEMPO_CAPTURA_MS) {
    mostrarMensaje("Captura", "cancelada");
  }
  if (pantalla == MENSAJE && millis() - momentoMensaje > TIEMPO_MENSAJE_MS) {
    pantalla = NORMAL;
  }

  char tecla = teclado.getKey();
  if (!tecla) return;
  ultimaTecla = millis();

  if (tecla == 'A') { iniciarCaptura(P_SP); return; }
  if (tecla == 'B') { iniciarCaptura(P_KP); return; }
  if (tecla == 'C') { iniciarCaptura(P_KI); return; }
  if (tecla == 'D') { iniciarCaptura(P_KD); return; }

  if (pantalla != CAPTURANDO) {
    if (tecla == '#') {
      paginaLCD = (paginaLCD + 1) % 3;
      pantalla = NORMAL;
    }
    return;
  }

  int n = strlen(entrada);
  if (tecla >= '0' && tecla <= '9') {
    if (n < 5) { entrada[n] = tecla; entrada[n + 1] = '\0'; }
  } else if (tecla == '*') {
    if (n > 0 && n < 5 && strchr(entrada, '.') == NULL) {
      entrada[n] = '.'; entrada[n + 1] = '\0';
    } else if (n == 0) {
      entrada[0] = '0'; entrada[1] = '.'; entrada[2] = '\0';
    }
  } else if (tecla == '#') {
    if (n == 0) cancelarCaptura();
    else        confirmarCaptura();
  }
}

void iniciarCaptura(Parametro p) {
  parametroActual = p;
  entrada[0] = '\0';
  pantalla = CAPTURANDO;
  ultimaTecla = millis();
}

void cancelarCaptura() {
  mostrarMensaje("Captura", "cancelada");
}

void confirmarCaptura() {
  float valor = atof(entrada);
  float limite = limiteParam[parametroActual];

  if (valor < 0 || valor > limite) {
    char l2[17];
    snprintf(l2, sizeof(l2), "Usa 0 a %.0f", limite);
    mostrarMensaje("Fuera de rango", l2);
    return;
  }

  *punteroParam(parametroActual) = valor;
  if (parametroActual == P_KI) termI = 0.0;
  guardarConfiguracion();

  char l1[17];
  snprintf(l1, sizeof(l1), "%s = %.2f", nombreParam[parametroActual], valor);
  mostrarMensaje(l1, "Guardado");
}

void mostrarMensaje(const char* l1, const char* l2) {
  strncpy(mensaje1, l1, 16); mensaje1[16] = '\0';
  strncpy(mensaje2, l2, 16); mensaje2[16] = '\0';
  pantalla = MENSAJE;
  momentoMensaje = millis();
}

void guardarConfiguracion() {
  prefs.begin("controlPID", false);
  prefs.putFloat("sp", setpoint);
  prefs.putFloat("kp", kp);
  prefs.putFloat("ki", ki);
  prefs.putFloat("kd", kd);
  prefs.end();
}

void actualizarLCD() {
  char linea[32];

  if (pantalla == CAPTURANDO) {
    snprintf(linea, sizeof(linea), "Nuevo %s: %s_", nombreParam[parametroActual], entrada);
    imprimirLinea(0, linea);
    imprimirLinea(1, "#:OK #vacio:Sal");
    return;
  }

  if (pantalla == MENSAJE) {
    imprimirLinea(0, mensaje1);
    imprimirLinea(1, mensaje2);
    return;
  }

  if (!lecturaIntentada) {
    imprimirLinea(0, "Leyendo");
    imprimirLinea(1, "DS18B20...");
    return;
  }

  if (!sensorOK) {
    snprintf(linea, sizeof(linea), "SP:%4.1f Kp:%4.1f", setpoint, kp);
    imprimirLinea(0, linea);
    imprimirLinea(1, "Error DS18B20");
    return;
  }

  if (paginaLCD == 0) {
    snprintf(linea, sizeof(linea), "T:%4.1fC SP:%4.1f", actual, setpoint);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "E:%4.1fC OUT:%3.0f%%", error, u);
    imprimirLinea(1, linea);
  } else if (paginaLCD == 1) {
    snprintf(linea, sizeof(linea), "Kp:%4.1f Ki:%4.2f", kp, ki);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "Kd:%4.1f SER:%3d", kd, angulo);
    imprimirLinea(1, linea);
  } else {
    snprintf(linea, sizeof(linea), "P:%5.1f I:%5.1f", termP, termI);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "D:%5.1f U:%3.0f%%", termD, u);
    imprimirLinea(1, linea);
  }
}

void imprimirLinea(int fila, const char* texto) {
  lcd.setCursor(0, fila);
  int n = 0;
  while (texto[n] != '\0' && n < 16) { lcd.print(texto[n]); n++; }
  while (n < 16) { lcd.print(' '); n++; }
}
```

---

## 🎚️ 12. Ajuste de las ganancias

| Parámetro | Efecto principal | Si es demasiado alto |
|---|---|---|
| Kp | Respuesta proporcional al error | Oscilaciones y sobreimpulso |
| Ki | Corrige el error acumulado | Sobreimpulso y oscilaciones lentas |
| Kd | Responde a cambios rápidos | Sensibilidad al ruido del sensor |

**Procedimiento sugerido**

1. Deja **Ki = 0 y Kd = 0** (valores iniciales) y ajusta Kp hasta lograr una respuesta rápida sin oscilar mucho.
2. Sube **Ki** poco a poco para eliminar el error permanente.
3. Agrega **Kd** al final para reducir el sobreimpulso.
4. Haz cambios pequeños y espera a que el sistema se estabilice antes del siguiente.

> 🌡️ El horno tiene mucha inercia térmica: los cambios tardan minutos en verse. Ajusta con paciencia y guarda el CSV de cada prueba para compararlas.

---

## 🧪 13. Pruebas funcionales

| Prueba | Acción | Resultado esperado |
|---|---|---|
| Arranque | Energizar | `Iniciando...` y luego `Leyendo DS18B20...` |
| Cambiar SP | `A`, `4`, `5`, `#` | `SP = 45.00 / Guardado` |
| Cambiar Kp | `B`, `8`, `#` | `Kp = 8.00 / Guardado` |
| Cambiar Ki | `C`, `0`, `*`, `2`, `#` | `Ki = 0.20 / Guardado` |
| Cambiar Kd | `D`, `2`, `#` | `Kd = 2.00 / Guardado` |
| Decimal sin número | `C`, `*`, `5`, `#` | Ki = 0.5 |
| Fuera de rango | `C`, `5`, `0`, `#` | `Fuera de rango / Usa 0 a 10` |
| Cancelar | `A` y luego `#` sin número | `Captura cancelada` |
| Reiniciar número | `A`, `4`, `A` | El campo vuelve a vacío |
| Cambiar pantalla | `#` fuera de captura | Rota entre 3 pantallas |
| Error alto | T muy por debajo de SP | OUT cercano a 100 % |
| Cerca del SP | T se acerca a SP | OUT baja gradualmente |
| T mayor que SP | Calentar el sensor | OUT = 0 % y servo en 122° |
| Anti-windup | Mantener salida saturada y luego acercarse a SP | La integral no se dispara |
| Sobretemperatura | T ≥ 80 °C | Foco apagado y aviso en serial |
| Falla del sensor | Desconectar el DS18B20 | `Error DS18B20` y foco apagado |
| Persistencia | Reiniciar el ESP32 | Conserva SP, Kp, Ki y Kd |

---

## 🛠️ 14. Solución de problemas

| Problema | Posible causa y solución |
|---|---|
| `Error DS18B20` | Revisa DATA en GPIO4, la resistencia de 4.7 kΩ a 3.3 V y la alimentación |
| Lee −127 °C | Sensor desconectado o sin pull-up |
| La temperatura oscila mucho | Baja Kp o Ki; revisa que Kd no sea muy alto |
| No llega al SP | Sube Ki poco a poco o revisa la potencia del foco |
| Sobreimpulso grande | Baja Ki o sube Kd |
| La salida se mueve a saltos | Kd demasiado alto: el sensor tiene ruido |
| LCD sin texto | Ajusta el contraste del módulo I2C |
| LCD no responde | Cambia `0x27` por `0x3F` |
| El foco no apaga del todo | Ajusta `ANGULO_MIN` según tu acople |
| El foco no llega al máximo | Ajusta `ANGULO_MAX` o el acople |
| El servo vibra o se reinicia el ESP32 | Alimenta el servo con 5 V externos y une los GND |
| El foco parpadea | Usa un foco incandescente compatible con el dimmer |
| Teclas incorrectas | Revisa el orden de filas y columnas |
| Relé funciona al revés | Los módulos son activos en LOW (`RELAY_ACTIVO`) |

---

## 📚 15. Conceptos aplicados

- 🎛️ Control automático PID discreto: error, ganancias y tiempo de muestreo.
- 🛡️ Saturación y anti-windup por integración condicional.
- 📉 Derivada sobre la medición para evitar el golpe derivativo.
- 🌡️ Sensor digital 1-Wire (DS18B20) con lectura no bloqueante.
- ⌨️ Teclado matricial con `Keypad` y captura de números decimales.
- 💾 Memoria no volátil con `Preferences`.
- ⚙️ Accionamiento mecánico de un dimmer con un servomotor.
- 🔌 Control de cargas de CD con módulos de relé.
- 📊 Adquisición y análisis de datos de una respuesta térmica.

---

## 🔧 16. Mejoras posibles

- 🤖 Autoajuste de ganancias (por ejemplo, método de relé).
- 🔁 Filtro digital para la lectura de temperatura y para el término derivativo.
- 📶 Monitoreo por Wi-Fi o MQTT.
- 📈 Gráfica en vivo en LabVIEW o en una página web.
- 🧭 Calibración de `ANGULO_MIN` y `ANGULO_MAX` desde el teclado.
- 🔌 Sustituir el dimmer mecánico por un dimmer electrónico con cruce por cero.
- 🔥 Protección térmica independiente del ESP32.

---

## 🏁 17. Resultado esperado

El horno alcanza la temperatura deseada regulando gradualmente la potencia del foco. El término integral elimina el error permanente, la derivada reduce el sobreimpulso y el anti-windup evita que la integral se dispare cuando la salida está saturada. SP, Kp, Ki y Kd se editan desde el teclado y se conservan al reiniciar. Si el sensor falla o la temperatura llega a 80 °C, el foco se apaga.
