# 🌡️ Horno con control proporcional (P): ESP32, DS18B20, teclado 4x4, dimmer con servo y LCD I2C

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue?logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange?logo=platformio)
![Sensor](https://img.shields.io/badge/Sensor-DS18B20-green)
![Control](https://img.shields.io/badge/Control-Proporcional_(P)-brightgreen)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

---

## 1. 📖 Descripción

Horno con **control proporcional (P)** de temperatura hecho con un **ESP32**, un sensor digital **DS18B20**, un **teclado 4x4**, una **LCD 16x2 I2C**, un **servomotor** acoplado a la perilla de un **dimmer MY-9892 (MY-989)** y **2 módulos de relé** (Peltier y extractor).

A diferencia del control ON/OFF, el foco ya no se prende y se apaga: el ESP32 calcula cuánta potencia necesita según el **error** entre la temperatura deseada (**SP**) y la temperatura medida (**T**), y gira el servo para que el dimmer entregue más o menos voltaje al foco.

La temperatura deseada (SP) y la ganancia (Kp) se capturan desde el teclado y se guardan en la memoria flash (`Preferences`), así que se conservan al apagar el equipo.

> ⚠️ **Seguridad:** el foco y el dimmer trabajan con **127 V de corriente alterna**. Arma y modifica el circuito siempre **desconectado**, y deja el dimmer y las conexiones dentro de una caja cerrada. El servo solo debe mover la perilla del dimmer, sin tocar sus conexiones eléctricas.

---

## 2. 🎯 Objetivos

- Medir la temperatura con un DS18B20 (12 bits) sin bloquear el programa.
- Implementar una ley de control proporcional con saturación de 0 a 100 %.
- Convertir la acción de control en un ángulo para el servo del dimmer.
- Capturar SP y Kp desde un teclado 4x4 y guardarlos en memoria no volátil.
- Mostrar temperatura, referencia, error, salida y ángulo en una LCD.
- Llevar el foco a una condición segura si el sensor falla.

---

## 3. 🧰 Materiales

| Cantidad | Componente | Notas |
|---|---|---|
| 1 | ESP32 DevKit (30 pines) | |
| 1 | Sensor DS18B20 | Digital, 1-Wire |
| 1 | Resistencia de 4.7 kΩ | Pull-up entre DATA y 3.3 V |
| 1 | Teclado matricial 4x4 | 8 pines |
| 1 | LCD 16x2 con módulo I2C | Dirección `0x27` |
| 1 | Servomotor (SG90 o similar) | Con alimentación de 5 V |
| 1 | Dimmer MY-9892 (MY-989) 2000 W | Regulador AC con perilla |
| 1 | Acople servo-perilla | Une el eje del servo con la perilla |
| 2 | Módulos de relé SRD-05VDC | Activos en LOW |
| 1 | Celda Peltier + 2 disipadores + 2 ventiladores | 12 V, en paralelo |
| 1 | Ventilador extractor | 12 V |
| 1 | Foco incandescente + clavija | 127 V CA, compatible con el dimmer |
| 1 | Fuente de 12 V (≥ 8 A) | Peltier, ventiladores y extractor |
| 1 | Fuente de 5 V | Para el servo, con GND común con el ESP32 |
| — | Cables y jumpers | |

---

## 4. ⚙️ Principio de funcionamiento

### 4.1 Error

```
e = SP - T
```

| Variable | Descripción | Unidad |
|---|---|---|
| e | Error de temperatura | °C |
| SP | Temperatura deseada (setpoint) | °C |
| T | Temperatura medida | °C |

### 4.2 Ley de control proporcional

```
u = Kp × e          (limitada a 0 % ≤ u ≤ 100 %)
```

- Error grande y positivo → más potencia al foco.
- Error pequeño → menos potencia.
- Error negativo (T mayor que SP) → `u = 0`, el foco queda apagado.

### 4.3 Conversión a ángulo del servo

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

### 4.4 Ejemplo

Con SP = 40 °C y Kp = 5 %/°C:

| T medida | Error | u | Ángulo |
|---:|---:|---:|---:|
| 30 °C | 10 °C | 50 % | 151° |
| 36 °C | 4 °C | 20 % | 134° |
| 38 °C | 2 °C | 10 % | 128° |
| 40 °C o más | ≤ 0 | 0 % | 122° |

> Un control P puede dejar un **error en estado estable**: cuando T se acerca a SP, la potencia baja y la temperatura puede estabilizarse un poco por debajo del SP.

### 4.5 Lectura del sensor

- Se pide una conversión cada **1 s** y se lee a los **800 ms**, sin usar `delay()`.
- Resolución de **12 bits**.
- Si el sensor no responde o da un valor fuera de −50 a 125 °C, se marca error, `u = 0` y el servo vuelve a `ANGULO_MIN` (foco apagado).

---

## 5. ⌨️ Interfaz de usuario (teclado)

| Tecla | Función |
|---|---|
| `A` | Capturar nuevo **SP** (temperatura deseada) |
| `B` | Capturar nueva **Kp** (ganancia) |
| `0`–`9` | Escribir el valor (hasta 5 caracteres) |
| `*` | Punto decimal |
| `C` | Borrar el último carácter |
| `D` | Cancelar la captura |
| `#` | Confirmar y guardar (en captura) · Cambiar de pantalla (fuera de captura) |

Reglas de validación:

- **SP:** de 0 a 99 °C.
- **Kp:** de 0 a 100 %/°C.
- Si pasan **10 s** sin presionar teclas, la captura se cancela.
- Valores iniciales: SP = 40 °C, Kp = 5 %/°C.
- Al confirmar, el cambio se aplica de inmediato.

---

## 6. 🖥️ Visualización en LCD

**Pantalla 1 (principal)**

```
T:30.5C SP:40.0
E: 9.5C OUT: 47%
```

**Pantalla 2** (se alterna con `#`)

```
Kp:  5.0 %/C
SRV:158 PELT:ON
```

**Captura de valor**

```
Nuevo SP: 45_
#:OK C:Bor D:Sal
```

**Mensajes (2 s):** `SP = 45.0 / Guardado`, `Fuera de rango / Usa 0 a 99`, `Escribe un / numero primero`, `Captura / cancelada`.

Si el sensor falla, la segunda línea muestra `Error DS18B20`.

---

## 7. 🔌 Conexiones

### ESP32

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

### Lado de 12 V (relés)

| Relé | Terminal | Conexión |
|---|---|---|
| Extractor | COM | + de la fuente de 12 V |
| Extractor | NO | + del extractor |
| Peltier | COM | + de la fuente de 12 V (unido al COM del extractor) |
| Peltier | NO | + del Peltier y de sus 2 ventiladores (en paralelo) |
| — | Negativo de la fuente | A los − del extractor, Peltier y ventiladores |

### Lado de 127 V CA (dimmer y foco)

| Origen | Destino |
|---|---|
| Clavija, fase (cable negro) | Dimmer, terminal de ENTRADA |
| Clavija, neutro (cable blanco) | Dimmer, terminal de ENTRADA |
| Dimmer, terminales de SALIDA | Foco (sus dos terminales) |
| Eje del servo | Perilla del dimmer (con el acople) |

> Revisa la serigrafía de tu dimmer y de tus módulos de relé: el orden de las terminales puede cambiar según el fabricante.

---

## 8. 📁 Estructura del proyecto (PlatformIO)

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

## 9. 🚀 Compilación y carga

1. Abre la carpeta del proyecto en VS Code con PlatformIO.
2. Verifica que `platformio.ini` tenga las librerías.
3. Compila con **Build** (✔) y carga con **Upload** (→) con el ESP32 por USB.
4. Abre el **Monitor Serial** a 115200 baudios.
5. Con la clavija **desconectada**, comprueba que el servo gire al cambiar SP o Kp.
6. Conecta la clavija solo cuando el dimmer y el foco estén listos y en su caja.

El monitor serial imprime en cada lectura: `SP`, `T`, `E`, `Kp`, `U` y el ángulo del servo.

---

## 10. 💻 Código

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

float setpoint = 40.0;
float kp       = 5.0;
float actual   = 0.0;
float error    = 0.0;
float u        = 0.0;
int   angulo   = ANGULO_MIN;
bool  sensorOK = false;
bool  lecturaIntentada = false;

const float SP_MAX = 99.0;
const float KP_MAX = 100.0;

unsigned long tPeticion = 0;
bool esperandoLectura = false;
const unsigned long PERIODO_LECTURA_MS   = 1000;
const unsigned long ESPERA_CONVERSION_MS = 800;

enum Pantalla { NORMAL, CAPTURANDO, MENSAJE };
Pantalla pantalla = NORMAL;
bool capturandoSP = true;
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
void iniciarCaptura(bool esSP);
void confirmarCaptura();
void cancelarCaptura();
void mostrarMensaje(const char* l1, const char* l2);
void guardarConfiguracion();
void leerSensor();
void calcularControl();
void actualizarLCD();
void imprimirLinea(int fila, const char* texto);

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

  prefs.begin("controlP", false);
  setpoint = prefs.getFloat("sp", 40.0);
  kp       = prefs.getFloat("kp", 5.0);
  prefs.end();

  Serial.println("=== Control P: foco con dimmer, Peltier siempre ON ===");
  Serial.println("A=SP  B=Kp  #=cambiar pantalla  C=borrar  D=cancelar  *=punto");

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
      u = 0;
      angulo = ANGULO_MIN;
      dimmer.write(angulo);
      Serial.println("Error: DS18B20 no responde");
      return;
    }

    sensorOK = true;
    actual = t;
    calcularControl();
  }
}

void calcularControl() {
  error = setpoint - actual;
  u = kp * error;
  if (u > 100.0) u = 100.0;
  if (u < 0.0)   u = 0.0;

  float grados = ANGULO_MIN + (u / 100.0) * (ANGULO_MAX - ANGULO_MIN);
  angulo = (int)(grados + 0.5);
  dimmer.write(angulo);

  Serial.print("SP:");     Serial.print(setpoint, 1);
  Serial.print(" T:");     Serial.print(actual, 1);
  Serial.print(" E:");     Serial.print(error, 1);
  Serial.print(" Kp:");    Serial.print(kp, 1);
  Serial.print(" U:");     Serial.print(u, 0);
  Serial.print("% Servo:"); Serial.println(angulo);
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

  if (tecla == 'A') { iniciarCaptura(true);  return; }
  if (tecla == 'B') { iniciarCaptura(false); return; }
  if (tecla == 'D') { cancelarCaptura();     return; }

  if (pantalla != CAPTURANDO) {
    if (tecla == '#') {
      paginaLCD = !paginaLCD;
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
    }
  } else if (tecla == 'C') {
    if (n > 0) entrada[n - 1] = '\0';
  } else if (tecla == '#') {
    confirmarCaptura();
  }
}

void iniciarCaptura(bool esSP) {
  capturandoSP = esSP;
  entrada[0] = '\0';
  pantalla = CAPTURANDO;
  ultimaTecla = millis();
}

void cancelarCaptura() {
  if (pantalla == CAPTURANDO) mostrarMensaje("Captura", "cancelada");
  else pantalla = NORMAL;
}

void confirmarCaptura() {
  if (strlen(entrada) == 0) {
    mostrarMensaje("Escribe un", "numero primero");
    return;
  }

  float valor = atof(entrada);
  float limite = capturandoSP ? SP_MAX : KP_MAX;

  if (valor < 0 || valor > limite) {
    char l2[17];
    snprintf(l2, sizeof(l2), "Usa 0 a %.0f", limite);
    mostrarMensaje("Fuera de rango", l2);
    return;
  }

  if (capturandoSP) setpoint = valor;
  else              kp = valor;
  guardarConfiguracion();
  if (sensorOK) calcularControl();

  char l1[17];
  snprintf(l1, sizeof(l1), "%s = %.1f", capturandoSP ? "SP" : "Kp", valor);
  mostrarMensaje(l1, "Guardado");
}

void mostrarMensaje(const char* l1, const char* l2) {
  strncpy(mensaje1, l1, 16); mensaje1[16] = '\0';
  strncpy(mensaje2, l2, 16); mensaje2[16] = '\0';
  pantalla = MENSAJE;
  momentoMensaje = millis();
}

void guardarConfiguracion() {
  prefs.begin("controlP", false);
  prefs.putFloat("sp", setpoint);
  prefs.putFloat("kp", kp);
  prefs.end();
}

void actualizarLCD() {
  char linea[32];

  if (pantalla == CAPTURANDO) {
    snprintf(linea, sizeof(linea), "Nuevo %s: %s_", capturandoSP ? "SP" : "Kp", entrada);
    imprimirLinea(0, linea);
    imprimirLinea(1, "#:OK C:Bor D:Sal");
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
  } else {
    snprintf(linea, sizeof(linea), "Kp:%5.1f %%/C", kp);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "SRV:%3d PELT:ON", angulo);
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

## 11. 🧪 Pruebas funcionales

| Prueba | Acción | Resultado esperado |
|---|---|---|
| Arranque | Energizar | LCD muestra `Iniciando...` y luego `Leyendo DS18B20...` |
| Cambiar SP | `A`, `4`, `5`, `#` | `SP = 45.0 / Guardado` |
| Cambiar Kp | `B`, `8`, `#` | `Kp = 8.0 / Guardado` |
| Decimales | `A`, `3`, `7`, `*`, `5`, `#` | SP = 37.5 |
| Fuera de rango | `A`, `1`, `0`, `0`, `#` | `Fuera de rango` |
| Cancelar | `A` y luego `D` | `Captura cancelada` |
| Cambiar pantalla | `#` fuera de captura | Alterna entre las 2 pantallas |
| Error alto | T muy por debajo de SP | OUT cercano a 100 % y servo en 180° |
| Error pequeño | T cerca de SP | OUT baja y el foco atenúa |
| T mayor que SP | Calentar el sensor | OUT = 0 % y servo en 122° |
| Persistencia | Reiniciar el ESP32 | Conserva SP y Kp |
| Falla del sensor | Desconectar el DS18B20 | `Error DS18B20` y foco apagado |

---

## 12. 🛠️ Solución de problemas

| Problema | Posible causa y solución |
|---|---|
| `Error DS18B20` | Revisa DATA en GPIO4, la resistencia de 4.7 kΩ a 3.3 V y la alimentación |
| Lee −127 °C | Sensor desconectado o sin pull-up |
| LCD sin texto | Ajusta el contraste del módulo I2C |
| LCD no responde | Cambia `0x27` por `0x3F` |
| El foco no apaga del todo | Ajusta `ANGULO_MIN` según tu acople |
| El foco no llega al máximo | Ajusta `ANGULO_MAX` o el acople |
| El servo vibra o se reinicia el ESP32 | Alimenta el servo con 5 V externos y une los GND |
| El foco parpadea | Usa un foco incandescente compatible con el dimmer |
| Teclas incorrectas | Revisa el orden de filas y columnas |
| Relé funciona al revés | Los módulos son activos en LOW (`RELAY_ACTIVO`) |

---

## 13. 📚 Conceptos aplicados

- Control automático proporcional, error y ganancia.
- Saturación de la acción de control.
- Conversión de una señal de control a una posición angular.
- Sensor digital 1-Wire (DS18B20) con lectura no bloqueante.
- Teclado matricial con `Keypad` y entrada de números decimales.
- Memoria no volátil con `Preferences`.
- Accionamiento mecánico de un dimmer con un servomotor.
- Control de cargas de CD con módulos de relé.

---

## 14. 🔧 Mejoras posibles

- Pasar de P a **PI** o **PID** para reducir el error en estado estable.
- Registrar T, error y salida para graficar la respuesta.
- Agregar una alarma por sobretemperatura.
- Calibrar `ANGULO_MIN` y `ANGULO_MAX` desde el teclado.
- Reemplazar el dimmer mecánico por un dimmer electrónico con cruce por cero.

---

## 15. ✅ Resultado esperado

El horno mantiene la temperatura cerca del valor deseado regulando gradualmente la luz del foco: mucha potencia cuando la temperatura está lejos del SP y poca cuando se acerca. SP y Kp se cambian desde el teclado y se conservan al reiniciar. Si el sensor falla, el foco se apaga.
