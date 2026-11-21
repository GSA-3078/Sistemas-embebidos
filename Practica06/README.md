# 🌡️ Control de temperatura con teclado 4x4 (ESP32 + LM35)

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue?logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange?logo=platformio)
![Sensor](https://img.shields.io/badge/Sensor-LM35-green)
![Relay](https://img.shields.io/badge/Relay-Active%20LOW-red)
![License](https://img.shields.io/badge/License-MIT-lightgrey)

---

## 1. 📖 Descripción

Horno de control **ON/OFF con histéresis** hecho con un **ESP32**, un sensor **LM35**, una pantalla **LCD 16x2 I2C**, un **teclado matricial 4x4** y **3 módulos de relé**.

Los límites de temperatura (**MAX** y **MIN**) se capturan desde el teclado y se guardan en la memoria flash (`Preferences`/NVS), así que se conservan al apagar el equipo.

- Cuando la temperatura baja a **MIN**, se enciende el **foco** (calienta).
- Cuando sube a **MAX**, se apaga el foco y se encienden la **celda Peltier** y el **extractor** (enfrían).

> ⚠️ **Seguridad:** el foco trabaja con **127 V CA**. Arma y modifica el circuito siempre **desconectado** y deja las conexiones dentro de una caja cerrada.

---

## 2. 🎯 Objetivos

- Leer temperatura con el LM35 usando `analogReadMilliVolts`.
- Filtrar la lectura con un promedio de 5 muestras.
- Controlar 3 salidas con relés activos en LOW.
- Capturar los límites con un teclado 4x4 sin bloquear el programa (`millis()`).
- Guardar la configuración en memoria no volátil.
- Detectar fallas del sensor y apagar las salidas de forma segura.

---

## 3. 🧰 Materiales

| Cantidad | Componente | Notas |
|---|---|---|
| 1 | ESP32 DevKit (30 pines) | |
| 1 | Sensor LM35 | 10 mV/°C, alimentado a 5 V |
| 1 | Teclado matricial 4x4 | 8 pines |
| 1 | LCD 16x2 con módulo I2C | Dirección `0x27` |
| 3 | Módulos de relé SRD-05VDC | Activos en LOW |
| 1 | Celda Peltier + 2 disipadores + 2 ventiladores | 12 V, en paralelo |
| 1 | Ventilador extractor | 12 V |
| 1 | Foco incandescente + clavija | 127 V CA |
| 1 | Fuente de 12 V (≥ 8 A) | Para Peltier y ventiladores |
| — | Cables y jumpers | |

---

## 4. ⚙️ Principio de funcionamiento

### Lectura del sensor

El LM35 entrega 10 mV por cada °C:

```
T (°C) = milivoltios / 10
```

Se promedian las últimas 5 lecturas (una cada 100 ms) para reducir el ruido. Si el voltaje supera 1500 mV se considera una lectura inválida; tras 3 errores seguidos se pasa a `ERROR_SENSOR`.

### Control con histéresis

| Condición | Estado | Foco | Peltier | Extractor |
|---|---|---|---|---|
| T ≤ MIN | CALENTANDO | ON | OFF | OFF |
| T ≥ MAX | ENFRIANDO | OFF | ON | ON |
| Entre MIN y MAX | Mantiene el estado anterior | — | — | — |
| Sensor con falla | ERROR_SENSOR | OFF | OFF | ON |

Entre un cambio de estado y otro hay una pausa de **1 s** con foco y Peltier apagados, para no tenerlos encendidos al mismo tiempo.

El porcentaje que se muestra en la LCD es:

```
% = (T - MIN) * 100 / (MAX - MIN)     (limitado de 0 a 100)
```

### Máquina de estados

`INICIANDO` → `CALENTANDO` ⇄ `ENFRIANDO`  (y `ERROR_SENSOR` si falla el LM35)

---

## 5. ⌨️ Interfaz de usuario (teclado)

| Tecla | Función |
|---|---|
| `A` | Capturar nuevo **MAX** |
| `B` | Capturar nuevo **MIN** |
| `0`–`9` | Escribir el valor (hasta 2 dígitos) |
| `#` | Confirmar y guardar |
| `*` | Borrar un dígito (si no hay dígitos, cancela) |
| `C`, `D` | Sin función |

Reglas de validación:

- Rango permitido: **0 a 99 °C**.
- **MAX ≥ MIN + 5 °C** (margen mínimo).
- Si pasan **10 s** sin presionar teclas, la captura se cancela.
- Valores iniciales: MAX = 60 °C, MIN = 30 °C.

---

## 6. 🖥️ Visualización en LCD

**Pantalla normal**

```
Max:60° Min:30°
45.0° 50% Calor
```

**Captura de valor**

```
Nuevo MAX: 7_
#:OK  *:Borrar
```

**Mensajes (2 s)**: `MAX = 70° / Guardado`, `Fuera de rango / Usa 0 a 99`, `Valor no valido / MAX >= 35°`, `Captura / cancelada`, `Error LM35` en caso de falla del sensor.

---

## 7. 🔌 Conexiones

### ESP32

| Elemento | Pin del ESP32 |
|---|---|
| LM35, salida | GPIO34 |
| LM35, alimentación | VIN (5 V) |
| LM35, tierra | GND |
| Teclado, filas R1–R4 | GPIO13, 14, 27, 26 |
| Teclado, columnas C1–C4 | GPIO25, 33, 32, 23 |
| Relé foco, IN | GPIO4 |
| Relé Peltier, IN | GPIO18 |
| Relé extractor, IN | GPIO19 |
| Relés, VCC / GND | VIN (5 V) / GND |
| LCD I2C, SDA | GPIO21 |
| LCD I2C, SCL | GPIO22 |
| LCD I2C, VCC / GND | VIN (5 V) / GND |

Los pines de la LCD no aparecen en el código por separado porque son los predeterminados de `Wire` en el ESP32 (GPIO21 y GPIO22), aunque el código los define para `Wire.begin()`.

### Lado de potencia

| Relé | Terminal | Conexión |
|---|---|---|
| Extractor | COM | + de la fuente de 12 V |
| Extractor | NO | + del extractor |
| Peltier | COM | + de la fuente de 12 V (unido al COM del extractor) |
| Peltier | NO | + del Peltier y de sus 2 ventiladores (en paralelo) |
| Foco | COM | Fase de la clavija (cable negro) |
| Foco | NO | Una terminal del foco |
| — | Neutro de la clavija | Directo a la otra terminal del foco |
| — | Negativo de la fuente | A los − del extractor, Peltier y ventiladores |

> Revisa la serigrafía de tus módulos de relé: el orden de NO, COM y NC puede cambiar según el fabricante.

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
monitor_speed = 9600
lib_deps =
    marcoschwartz/LiquidCrystal_I2C
    chris--a/Keypad
```

`Wire` y `Preferences` ya vienen incluidas en el framework.

---

## 9. 🚀 Compilación y carga

1. Abre la carpeta del proyecto en VS Code con PlatformIO.
2. Verifica que `platformio.ini` tenga las librerías.
3. Compila con **Build** (✔) y carga con **Upload** (→) con el ESP32 conectado por USB.
4. Abre el **Monitor Serial** a 9600 baudios.

> Esta práctica no se simula en Wokwi porque no incluye el sensor LM35.

---

## 10. 💻 Código

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Preferences.h>

#define PIN_LM35          34

#define RELAY_FOCO        4
#define RELAY_PELTIER     18
#define RELAY_EXTRACTOR   19

#define LCD_SDA           21
#define LCD_SCL           22

#define RELAY_ACTIVO   LOW
#define RELAY_INACTIVO (!RELAY_ACTIVO)

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

const int TEMP_MAX_INICIAL = 60;
const int TEMP_MIN_INICIAL = 30;
const int LIMITE_INFERIOR  = 0;
const int LIMITE_SUPERIOR  = 99;
const int MARGEN_MINIMO    = 5;

const unsigned long PAUSA_CAMBIO_MS = 1000;
const int MAX_ERRORES_SENSOR = 3;
const unsigned long TIEMPO_CAPTURA_MS = 10000;
const unsigned long TIEMPO_MENSAJE_MS = 2000;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences prefs;

enum EstadoCiclo { INICIANDO, CALENTANDO, ENFRIANDO, ERROR_SENSOR };
EstadoCiclo estado = INICIANDO;

int tempMax = TEMP_MAX_INICIAL;
int tempMin = TEMP_MIN_INICIAL;

float tempActual    = 0.0;
float porcentaje    = 0.0;
int   erroresSensor = 0;
bool  hayLectura    = false;

unsigned long momentoCambioEstado = 0;

unsigned long momentoLectura = 0;
const unsigned long INTERVALO_SENSOR_MS = 100;
const int MUESTRAS_FILTRO = 5;
float muestras[MUESTRAS_FILTRO];
int indiceMuestra = 0;
bool filtroListo = false;
const int MV_MAXIMO_VALIDO = 1500;

enum Pantalla { NORMAL, CAPTURANDO, MENSAJE };
Pantalla pantalla = NORMAL;
bool capturandoMax = true;
char digitos[3] = "";
unsigned long ultimaTecla = 0;
char mensaje1[17] = "";
char mensaje2[17] = "";
unsigned long momentoMensaje = 0;

unsigned long ultimaActualizacionLCD = 0;
const unsigned long INTERVALO_LCD_MS = 300;

const char GRADO = (char)223;

void manejarTeclado();
void iniciarCaptura(bool esMax);
void confirmarCaptura();
void mostrarMensaje(const char* l1, const char* l2);
void validarLimites();
void guardarConfiguracion();
void leerSensor();
void actualizarCiclo();
void cambiarEstado(EstadoCiclo nuevo);
void aplicarSalidas();
void apagarFocoYPeltier();
void actualizarExtractor();
const char* nombreEstado();
void actualizarLCD();
void imprimirLinea(int fila, const char* texto);


void setup() {
  Serial.begin(9600);

  pinMode(RELAY_FOCO, OUTPUT);
  pinMode(RELAY_PELTIER, OUTPUT);
  pinMode(RELAY_EXTRACTOR, OUTPUT);
  apagarFocoYPeltier();
  digitalWrite(RELAY_EXTRACTOR, RELAY_INACTIVO);

  teclado.setDebounceTime(50);

  delay(500);
  Wire.begin(LCD_SDA, LCD_SCL);
  Wire.setClock(100000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_LM35, ADC_11db);
  pinMode(PIN_LM35, INPUT);

  prefs.begin("tempTeclado", false);
  tempMax = prefs.getInt("tempMax", TEMP_MAX_INICIAL);
  tempMin = prefs.getInt("tempMin", TEMP_MIN_INICIAL);
  prefs.end();
  validarLimites();

  Serial.println("=== Control de temperatura con teclado ===");
  Serial.println("A + numero + # = MAX | B + numero + # = MIN | * = borrar");
  Serial.print("MAX: "); Serial.print(tempMax); Serial.print(" C  MIN: "); Serial.print(tempMin); Serial.println(" C");

  delay(1000);
  lcd.clear();
}

void loop() {
  manejarTeclado();

  if (millis() - momentoLectura >= INTERVALO_SENSOR_MS) {
    momentoLectura = millis();
    leerSensor();
    actualizarCiclo();
  }

  if (millis() - ultimaActualizacionLCD >= INTERVALO_LCD_MS) {
    ultimaActualizacionLCD = millis();
    actualizarLCD();
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
  Serial.print("Tecla: "); Serial.println(tecla);

  if (tecla == 'A') {
    iniciarCaptura(true);
    return;
  }
  if (tecla == 'B') {
    iniciarCaptura(false);
    return;
  }

  if (pantalla != CAPTURANDO) return;

  int n = strlen(digitos);
  if (tecla >= '0' && tecla <= '9') {
    if (n < 2) {
      digitos[n] = tecla;
      digitos[n + 1] = '\0';
    }
  } else if (tecla == '*') {
    if (n > 0) digitos[n - 1] = '\0';
    else mostrarMensaje("Captura", "cancelada");
  } else if (tecla == '#') {
    confirmarCaptura();
  }
}

void iniciarCaptura(bool esMax) {
  capturandoMax = esMax;
  digitos[0] = '\0';
  pantalla = CAPTURANDO;
  ultimaTecla = millis();
}

void confirmarCaptura() {
  if (strlen(digitos) == 0) {
    mostrarMensaje("Escribe un", "numero primero");
    return;
  }

  int valor = atoi(digitos);
  int nuevoMax = capturandoMax ? valor : tempMax;
  int nuevoMin = capturandoMax ? tempMin : valor;

  if (valor < LIMITE_INFERIOR || valor > LIMITE_SUPERIOR) {
    mostrarMensaje("Fuera de rango", "Usa 0 a 99");
    return;
  }
  if (nuevoMax < nuevoMin + MARGEN_MINIMO) {
    char l2[24];
    if (capturandoMax) snprintf(l2, sizeof(l2), "MAX >= %d%c", tempMin + MARGEN_MINIMO, GRADO);
    else               snprintf(l2, sizeof(l2), "MIN <= %d%c", tempMax - MARGEN_MINIMO, GRADO);
    mostrarMensaje("Valor no valido", l2);
    return;
  }

  tempMax = nuevoMax;
  tempMin = nuevoMin;
  guardarConfiguracion();

  char l1[24];
  snprintf(l1, sizeof(l1), "%s = %d%c", capturandoMax ? "MAX" : "MIN", valor, GRADO);
  mostrarMensaje(l1, "Guardado");
}

void mostrarMensaje(const char* l1, const char* l2) {
  strncpy(mensaje1, l1, 16); mensaje1[16] = '\0';
  strncpy(mensaje2, l2, 16); mensaje2[16] = '\0';
  pantalla = MENSAJE;
  momentoMensaje = millis();
  Serial.print("LCD: "); Serial.print(mensaje1); Serial.print(" / "); Serial.println(mensaje2);
}

void validarLimites() {
  tempMax = constrain(tempMax, LIMITE_INFERIOR + MARGEN_MINIMO, LIMITE_SUPERIOR);
  tempMin = constrain(tempMin, LIMITE_INFERIOR, tempMax - MARGEN_MINIMO);
}

void guardarConfiguracion() {
  prefs.begin("tempTeclado", false);
  prefs.putInt("tempMax", tempMax);
  prefs.putInt("tempMin", tempMin);
  prefs.end();
  Serial.print("MAX: "); Serial.print(tempMax); Serial.print(" C  MIN: "); Serial.print(tempMin); Serial.println(" C");
}

void leerSensor() {
  int milivoltios = analogReadMilliVolts(PIN_LM35);

  if (milivoltios > MV_MAXIMO_VALIDO) {
    erroresSensor++;
    filtroListo = false;
    Serial.println("Error: revisar LM35");
    return;
  }

  float lectura = milivoltios / 10.0;

  if (!filtroListo) {
    for (int i = 0; i < MUESTRAS_FILTRO; i++) muestras[i] = lectura;
    filtroListo = true;
  } else {
    muestras[indiceMuestra] = lectura;
  }
  indiceMuestra = (indiceMuestra + 1) % MUESTRAS_FILTRO;
  float suma = 0;
  for (int i = 0; i < MUESTRAS_FILTRO; i++) suma += muestras[i];
  float t = suma / MUESTRAS_FILTRO;

  erroresSensor = 0;
  hayLectura = true;
  tempActual = t;

  porcentaje = (tempActual - tempMin) * 100.0 / (tempMax - tempMin);
  porcentaje = constrain(porcentaje, 0.0, 100.0);

  Serial.print("LM35: ");        Serial.print(lectura, 1);
  Serial.print(" C | Filtrada: "); Serial.print(tempActual, 1);
  Serial.print(" C | ");         Serial.print(porcentaje, 0);
  Serial.print(" % | Estado: "); Serial.println(nombreEstado());
}

void actualizarCiclo() {
  if (erroresSensor >= MAX_ERRORES_SENSOR) {
    if (estado != ERROR_SENSOR) cambiarEstado(ERROR_SENSOR);
    apagarFocoYPeltier();
    actualizarExtractor();
    return;
  }

  if (!hayLectura) return;

  switch (estado) {
    case INICIANDO:
    case ERROR_SENSOR:
      if (erroresSensor == 0) {
        cambiarEstado(tempActual >= tempMax ? ENFRIANDO : CALENTANDO);
      }
      break;

    case CALENTANDO:
      if (tempActual >= tempMax) cambiarEstado(ENFRIANDO);
      break;

    case ENFRIANDO:
      if (tempActual <= tempMin) cambiarEstado(CALENTANDO);
      break;
  }

  aplicarSalidas();
}

void cambiarEstado(EstadoCiclo nuevo) {
  estado = nuevo;
  momentoCambioEstado = millis();
  apagarFocoYPeltier();
  actualizarExtractor();
  Serial.print(">>> Cambio de estado: "); Serial.println(nombreEstado());
}

void actualizarExtractor() {
  bool encender = (estado == ENFRIANDO || estado == ERROR_SENSOR);
  digitalWrite(RELAY_EXTRACTOR, encender ? RELAY_ACTIVO : RELAY_INACTIVO);
}

void aplicarSalidas() {
  actualizarExtractor();

  if (millis() - momentoCambioEstado < PAUSA_CAMBIO_MS) {
    apagarFocoYPeltier();
    return;
  }

  if (estado == CALENTANDO) {
    digitalWrite(RELAY_PELTIER, RELAY_INACTIVO);
    digitalWrite(RELAY_FOCO,    RELAY_ACTIVO);
  } else if (estado == ENFRIANDO) {
    digitalWrite(RELAY_FOCO,    RELAY_INACTIVO);
    digitalWrite(RELAY_PELTIER, RELAY_ACTIVO);
  } else {
    apagarFocoYPeltier();
  }
}

void apagarFocoYPeltier() {
  digitalWrite(RELAY_FOCO,    RELAY_INACTIVO);
  digitalWrite(RELAY_PELTIER, RELAY_INACTIVO);
}

const char* nombreEstado() {
  switch (estado) {
    case CALENTANDO:   return "CALENTANDO";
    case ENFRIANDO:    return "ENFRIANDO";
    case ERROR_SENSOR: return "ERROR SENSOR";
    default:           return "INICIANDO";
  }
}

void actualizarLCD() {
  char linea[17];

  if (pantalla == CAPTURANDO) {
    snprintf(linea, sizeof(linea), "Nuevo %s: %s%s",
             capturandoMax ? "MAX" : "MIN", digitos, strlen(digitos) < 2 ? "_" : "");
    imprimirLinea(0, linea);
    imprimirLinea(1, "#:OK  *:Borrar");
    return;
  }

  if (pantalla == MENSAJE) {
    imprimirLinea(0, mensaje1);
    imprimirLinea(1, mensaje2);
    return;
  }

  snprintf(linea, sizeof(linea), "Max:%2d%c Min:%2d%c", tempMax, GRADO, tempMin, GRADO);
  imprimirLinea(0, linea);

  if (estado == ERROR_SENSOR) {
    imprimirLinea(1, "Error LM35");
  } else if (!hayLectura) {
    imprimirLinea(1, "Leyendo LM35");
  } else {
    const char* textoEstado;
    if (estado == CALENTANDO)     textoEstado = "Calor";
    else if (estado == ENFRIANDO) textoEstado = "Frio";
    else                          textoEstado = "";

    snprintf(linea, sizeof(linea), "%4.1f%c %3d%% %s",
             tempActual, GRADO, (int)(porcentaje + 0.5), textoEstado);
    imprimirLinea(1, linea);
  }
}


void imprimirLinea(int fila, const char* texto) {
  lcd.setCursor(0, fila);
  int n = 0;
  while (texto[n] != '\0' && n < 16) {
    lcd.print(texto[n]);
    n++;
  }
  while (n < 16) {
    lcd.print(' ');
    n++;
  }
}
```

---

## 11. 🧪 Pruebas funcionales

| Prueba | Acción | Resultado esperado |
|---|---|---|
| Arranque | Energizar | LCD muestra `Iniciando...` y luego MAX/MIN guardados |
| Cambiar MAX | `A`, `7`, `0`, `#` | `MAX = 70° / Guardado` |
| Cambiar MIN | `B`, `4`, `0`, `#` | `MIN = 40° / Guardado` |
| Valor fuera de rango | Escribir un valor inválido | `Valor no valido` |
| Cancelar | `A` y luego `*` sin dígitos | `Captura cancelada` |
| Persistencia | Reiniciar el ESP32 | Conserva MAX y MIN |
| Calentar | T ≤ MIN | Se enciende el foco |
| Enfriar | T ≥ MAX | Se apaga el foco; Peltier y extractor encienden |
| Falla del sensor | Desconectar el LM35 | `Error LM35`, foco y Peltier apagados |

---

## 12. 🛠️ Solución de problemas

| Problema | Posible causa y solución |
|---|---|
| LCD sin texto | Ajusta el contraste del módulo I2C |
| LCD no responde | Cambia `0x27` por `0x3F` |
| Teclas incorrectas o dobles | Revisa el orden filas/columnas y el cableado |
| Temperatura errática | Alimenta el LM35 con 5 V, mantén el cable corto y usa GPIO34 |
| Marca error de sensor | Revisa las 3 patas del LM35 (+5 V, salida, GND) |
| Relé funciona al revés | Los módulos son activos en LOW; revisa `RELAY_ACTIVO` |
| Se reinicia al encender el Peltier | Fuente insuficiente; usa una de 12 V ≥ 8 A |

---

## 13. 📚 Conceptos aplicados

- Histéresis en control ON/OFF.
- Lectura analógica con `analogReadMilliVolts`.
- Filtro de promedio móvil.
- Teclado matricial con la librería `Keypad`.
- Memoria no volátil con `Preferences`.
- Máquina de estados y programación no bloqueante con `millis()`.
- Control de cargas de CA y CD con módulos de relé.

---

## 14. 🔧 Mejoras posibles

- Control PID en lugar de ON/OFF.
- Sensor adicional para promediar o detectar fallas.
- Registro de datos por Wi-Fi.
- Uso del módulo de relé de estado sólido para el foco.

---

## 15. ✅ Resultado esperado

El horno mantiene la temperatura entre MIN y MAX, los límites se ajustan desde el teclado y se conservan al reiniciar. Si el sensor falla, las salidas de calor y frío se apagan y la LCD lo indica.
