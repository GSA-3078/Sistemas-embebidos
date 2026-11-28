# 🌡️ Controlador PID de Temperatura con ESP32, HMI Nextion, DS18B20, Foco con Dimmer y Celda Peltier

### 🔬 Sistemas Embebidos | PlatformIO + Arduino Framework + Nextion

![ESP32](https://img.shields.io/badge/ESP32-Microcontrolador-00979D?style=for-the-badge&logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Desarrollo-F5822A?style=for-the-badge&logo=platformio&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-Framework-00878F?style=for-the-badge&logo=arduino&logoColor=white)
![Nextion](https://img.shields.io/badge/Nextion-HMI%202.8%22-FFC107?style=for-the-badge)
![DS18B20](https://img.shields.io/badge/DS18B20-Temperatura-blue?style=for-the-badge)
![Control PID](https://img.shields.io/badge/Control-PID-green?style=for-the-badge)

---

## 📌 1. Descripción del proyecto

El proyecto consiste en el desarrollo de un sistema embebido para la medición y el control automático de temperatura mediante un **controlador PID (Proporcional, Integral y Derivativo), implementado en una tarjeta ESP32**.

El sistema utiliza un sensor digital DS18B20 para adquirir la temperatura del proceso y una **pantalla táctil Nextion NX3224F028 de 2.8 pulgadas** como interfaz hombre-máquina (HMI), desde la cual se visualizan las variables de operación y se modifican los parámetros del controlador.

El calentamiento se realiza con un **foco de 100 W**, cuya potencia se regula mediante un **dimmer accionado por un servomotor**. El sistema incluye además una **celda Peltier** y un **extractor**, controlados mediante módulos de relé.

La acción de control calculada por la ESP32 se transforma en una posición angular que determina el movimiento del servomotor, el cual gira la perilla del dimmer para regular la potencia eléctrica suministrada al foco.

A diferencia del controlador proporcional, el controlador PID incorpora la **acción integral**, que reduce el error en estado estable, y la **acción derivativa**, que anticipa la tendencia de la temperatura y amortigua la respuesta del sistema.

### 🎯 Objetivo general

Desarrollar un sistema de control PID de temperatura basado en ESP32 que permita medir la temperatura con un sensor DS18B20, regular la potencia de un foco mediante un dimmer accionado por un servomotor y configurar la referencia y las ganancias Kp, Ki y Kd desde una HMI Nextion que muestre en tiempo real las variables del proceso y su gráfica de evolución.

### 🎯 Objetivos específicos

- 🌡️ Medir la temperatura del proceso mediante un sensor DS18B20.
- ⚙️ Implementar un controlador PID con anti-windup y derivada sobre la medición.
- 📐 Ajustar las ganancias Kp, Ki y Kd para modificar la respuesta del sistema.
- 🔄 Convertir la acción de control en una posición angular para el servomotor.
- 🔌 Regular la potencia del foco mediante el accionamiento mecánico del dimmer.
- 🖥️ Visualizar la temperatura de proceso (VP), el setpoint (SP), el error y la salida de control en la HMI.
- 📈 Graficar en tiempo real la evolución de la temperatura y del setpoint.
- ✏️ Modificar SP, Kp, Ki y Kd desde la pantalla y aplicarlos al controlador.
- 💾 Conservar los parámetros en memoria no volátil.
- 🛡️ Proteger el sistema ante sobretemperatura y fallos del sensor.

---

## 🧰 2. Materiales y herramientas

| Elemento | Función |
|---|---|
| ESP32 DevKit | Procesamiento y control |
| Pantalla Nextion Discovery NX3224F028 (2.8") | HMI táctil: visualización y configuración |
| Sensor DS18B20 | Medición de temperatura |
| Resistencia de 4.7 kΩ | Pull-up para el DS18B20 |
| Servomotor SG90 o equivalente | Accionamiento mecánico del dimmer |
| Dimmer MY-9892 (2000 W) | Modulación de potencia eléctrica |
| Foco de 100 W (127 V CA) | Actuador de calentamiento |
| Celda Peltier con disipadores y ventiladores (12 V) | Elemento de enfriamiento |
| Extractor de 12 V | Ventilación |
| 2 módulos de relé SRD-05VDC | Control de la Peltier y del extractor |
| Fuente de 12 V | Alimentación de Peltier, ventiladores y extractor |
| Fuente de 5 V | Alimentación del servomotor y de la pantalla |
| Clavija y cableado de 127 V CA | Alimentación del foco |
| PlatformIO | Desarrollo y compilación |

> ⚠️ La conexión del dimmer y del foco involucra tensión de red eléctrica (127 V CA). Se requiere aislamiento, protecciones y una instalación adecuada. El servomotor debe accionar únicamente la perilla aislada del dimmer, sin intervenir directamente en sus conexiones eléctricas.

---

## 🧠 3. Principio de funcionamiento

El sistema opera mediante un controlador PID que calcula la acción de control a partir del error entre la temperatura de referencia y la temperatura medida. La medición y el cálculo se ejecutan cada **1 segundo**.

### 3.1. Cálculo del error

**e(t) = SP - T(t)**

| Variable | Descripción | Unidad |
|---|---|---|
| e(t) | Error de temperatura | °C |
| SP | Setpoint (temperatura de referencia) | °C |
| T(t) | Temperatura medida (VP) | °C |

Cuando la temperatura medida es inferior a la referencia, el error es positivo y el controlador incrementa la potencia de calefacción.

### 3.2. Acciones del controlador PID

**Acción proporcional:**

**P = Kp × e**

**Acción integral:**

**I = I + Ki × e × Ts**

**Acción derivativa (sobre la medición):**

**D = -Kd × (T - Tanterior) / Ts**

**Salida del controlador:**

**u = P + I + D**

| Variable | Descripción |
|---|---|
| Kp | Ganancia proporcional |
| Ki | Ganancia integral |
| Kd | Ganancia derivativa |
| Ts | Periodo de muestreo (aproximadamente 1 s) |
| Tanterior | Temperatura medida en el ciclo anterior |

La derivada se calcula sobre la **medición** y no sobre el error, lo que evita saltos bruscos en la salida cuando el usuario cambia el setpoint.

### 3.3. Saturación y anti-windup

La acción de control se interpreta como un porcentaje de potencia solicitado al dimmer, por lo que se limita a:

**0 % ≤ u ≤ 100 %**

Para evitar que la integral se acumule cuando la salida está saturada (efecto *windup*), se aplican dos medidas:

- **Integración condicional:** la integral deja de acumularse si la salida estaría saturada y el error empuja en el mismo sentido.
- **Limitación del término integral** al rango de 0 a 100 %.

Como el sistema calienta únicamente con el foco, la salida nunca es negativa.

### 3.4. Conversión de la acción de control a posición angular

La acción de control se convierte a un ángulo mediante una relación de calibración:

**θ = θmin + (u/100) × (θmax - θmin)**

| Variable | Descripción | Valor |
|---|---|---|
| θ | Posición angular del servomotor | — |
| θmin | Ángulo correspondiente a 0 % de luz | 55° |
| θmax | Ángulo correspondiente a 100 % de luz | 180° |
| u | Acción de control en porcentaje | 0 a 100 % |

| Acción de control | Posición del servomotor |
|---:|---:|
| 0 % | 55° |
| 25 % | 86° |
| 50 % | 118° |
| 75 % | 149° |
| 100 % | 180° |

Los valores de θmin y θmax dependen del recorrido mecánico de la perilla del dimmer y deben calibrarse experimentalmente. La relación entre el ángulo y la potencia real del foco puede no ser lineal.

### 3.5. Ejemplo de funcionamiento

Considerando SP = 40 °C, Kp = 10 y Ki = Kd = 0:

Si la temperatura medida es 37 °C:

**e = 40 - 37 = 3 °C**

**u = 10 × 3 = 30 %**

**θ = 55 + 0.30 × (180 - 55) ≈ 93°**

Cuando la temperatura alcanza 39 °C:

**e = 40 - 39 = 1 °C**

**u = 10 × 1 = 10 %**

**θ = 55 + 0.10 × (180 - 55) ≈ 68°**

Al activar la acción integral (Ki > 0), la salida sigue aumentando mientras exista error, lo que permite eliminar la diferencia que el control proporcional dejaría en estado estable.

### 3.6. Valores iniciales y límites

| Parámetro | Valor inicial | Mínimo | Máximo |
|---|---|---|---|
| SP | 40 °C | 0 | 75 °C |
| Kp | 10 | 0 | 100 |
| Ki | 0 | 0 | 10 |
| Kd | 0 | 0 | 100 |

Ki y Kd inician en 0 para que el sistema arranque como un controlador proporcional. Se recomienda subirlos gradualmente desde la pantalla durante la sintonización.

---

## 🖥️ 4. Interfaz HMI: Pantalla Nextion

La pantalla Nextion se comunica con la ESP32 mediante el **puerto UART2 a 9600 baudios**. El baud debe coincidir con `baud=9600` en el `Program.s` del proyecto de la pantalla.

### 4.1. Variables mostradas en tiempo real

| Componente | Contenido | Escala enviada |
|---|---|---|
| `x_vp` | Temperatura de proceso (VP) | T × 10 |
| `x_sp` | Setpoint (SP) | SP × 10 |
| `x_err` | Error | e × 10 |
| `n_out` | Salida de control | 0 a 100 % |
| `w_graf` | Gráfica: canal 0 = VP, canal 1 = SP | Rango 20 a 50 °C |
| `t_estado` | Estado: `OK`, `SOBRETEMP` o `ERROR SENSOR` | Texto |

Una distribución propuesta de la pantalla de monitoreo es:

```text
VP: 30.5 C        SP: 40.0 C
E:   9.5 C        OUT: 47 %
┌──────────────────────────────┐
│  gráfica VP / SP vs tiempo   │
└──────────────────────────────┘
```

La gráfica conserva un historial de **120 puntos** (1 punto por segundo). Al volver a la pantalla de monitoreo, la ESP32 **redibuja la gráfica** con el historial guardado, sin bloquear el control.

### 4.2. Configuración de parámetros

Desde la pantalla de configuración el usuario puede modificar **SP, Kp, Ki y Kd** y pulsar **Aplicar**. Los valores se envían a la ESP32, que los valida, los limita a los rangos permitidos y los guarda en memoria no volátil (`Preferences`).

| Componente | Parámetro | Escala |
|---|---|---|
| `cx_sp` | SP | × 10 |
| `cx_kp` | Kp | × 10 |
| `cx_ki` | Ki | × 100 |
| `cx_kd` | Kd | × 10 |

Al arrancar, la ESP32 envía a la pantalla los valores guardados (dos veces, por si la pantalla inicia después).

Al aplicar nuevos parámetros, el término integral se reinicia para evitar saltos del servomotor.

### 4.3. Protocolo pantalla → ESP32

| Byte recibido | Acción |
|---|---|
| `0x52` | Se entró a la pantalla de monitoreo: se redibuja la gráfica |
| `0x55` + 16 bytes | **Aplicar:** SP, Kp, Ki y Kd como enteros de 32 bits (little-endian) |

---

## 🔌 5. Conexiones del sistema

Las siguientes conexiones corresponden a una ESP32 DevKit de 30 pines.

### 5.1. Sensor DS18B20

| Sensor | ESP32 |
|---|---|
| DATA | GPIO 4 |
| VCC | 3.3 V |
| GND | GND |

Se requiere una resistencia pull-up de 4.7 kΩ entre DATA y VCC.

### 5.2. Pantalla Nextion

| Nextion | ESP32 |
|---|---|
| TX | GPIO 16 (RX2) |
| RX | GPIO 17 (TX2) |
| 5V | Fuente de 5 V |
| GND | GND común |

El TX de la pantalla se conecta al RX de la ESP32 y el RX de la pantalla al TX de la ESP32.

### 5.3. Servomotor

| Cable del servomotor | Conexión |
|---|---|
| Rojo | Fuente externa de 5 V |
| Marrón/Negro | GND |
| Naranja/Amarillo | GPIO 18 |

**Consideraciones importantes:**

- Utilizar una fuente externa de 5 V con capacidad suficiente para el servomotor.
- Conectar el GND de la fuente externa con el GND de la ESP32.
- No alimentar el servomotor desde un GPIO.
- Verificar que el servomotor no exceda su recorrido mecánico.
- Mantener los límites angulares (55° a 180°) para evitar daños en el mecanismo.

### 5.4. Módulos de relé

| Relé | Señal IN | Carga |
|---|---|---|
| Peltier | GPIO 19 | Celda Peltier y sus ventiladores (12 V) |
| Extractor | GPIO 23 | Extractor (12 V) |

- Los módulos de relé son **activos en LOW** (LOW = encendido, HIGH = apagado). Si tus módulos funcionan al revés, cambia `RELAY_ACTIVO` a `HIGH` en el código.
- El terminal **COM** de cada relé se conecta al positivo de 12 V y el terminal **NO** a la carga.
- La Peltier y sus dos ventiladores se conectan en paralelo.
- Alimentar el VCC de los relés con 5 V y unir su GND con el de la ESP32.

### 5.5. Dimmer y foco

La conexión funcional es:

**ESP32 → Servomotor → Perilla del dimmer → Foco de 100 W**

El dimmer debe ser compatible con el tipo de carga y con la potencia del foco. Su conexión eléctrica (127 V CA, fase y neutro) debe realizarse de forma independiente al circuito de baja tensión de la ESP32, con aislamiento y protecciones adecuadas.

---

## 💻 6. Estructura del proyecto en PlatformIO

```text
Controlador_PID_HMI_Nextion/
├── include/
├── lib/
├── src/
│   └── main.cpp
├── nextion/
│   └── proyecto.HMI
├── test/
├── platformio.ini
├── .gitignore
└── README.md
```

### 📄 Archivo platformio.ini

Configuración de referencia:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

lib_deps =
    paulstoffregen/OneWire
    milesburton/DallasTemperature
    madhephaestus/ESP32Servo
```

La biblioteca `Preferences` ya forma parte del framework de Arduino para ESP32, por lo que no requiere instalación adicional.

### 💻 Código fuente (src/main.cpp)

El programa está organizado en los siguientes bloques:

| Bloque | Funciones | Descripción |
|---|---|---|
| 🖥️ HMI Nextion | `nxCmd`, `nxNumero`, `nxTexto`, `nxPunto`, `leerNextion`, `leerParametros` | Envío de datos a la pantalla y recepción de los parámetros |
| 📈 Gráfica | `aPixeles`, `guardarHistoria`, `iniciarRedibujo`, `pasoRedibujo` | Historial de 120 puntos y redibujo sin bloquear el control |
| ⚙️ Control PID | `pasoPID`, `reiniciarPID`, `aplicarServo` | Cálculo de P, I y D, anti-windup y conversión a ángulo |
| 🌡️ Sensor | `leerSensor`, `falloSensor` | Lectura no bloqueante del DS18B20 y protección ante fallos |
| 💾 Memoria | `guardarConfiguracion` | Guardado de SP, Kp, Ki y Kd con `Preferences` |

```cpp
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>
#include <Preferences.h>

#define PIN_DS18B20      4
#define PIN_SERVO        18
#define RELAY_PELTIER    19
#define RELAY_EXTRACTOR  23
#define NEXTION_RX       16
#define NEXTION_TX       17

#define RELAY_ACTIVO   LOW
#define RELAY_INACTIVO (!RELAY_ACTIVO)
const bool EXTRACTOR_SIEMPRE_ON = true;

const unsigned long NEXTION_BAUD = 9600;

const int ANGULO_MIN = 55;
const int ANGULO_MAX = 180;

const float TEMP_SEGURIDAD = 80.0;
const float SP_MAX = 75.0;
const float KP_MAX = 100.0;
const float KI_MAX = 10.0;
const float KD_MAX = 100.0;

const int   ID_GRAFICA   = 8;
const int   ALTO_GRAFICA = 135;
const float T_MIN_G      = 20.0;
const float T_MAX_G      = 50.0;
const int   CICLOS_POR_PUNTO = 1;
const int   HIST_N       = 120;

uint8_t histT[HIST_N];
uint8_t histS[HIST_N];
unsigned long totalPuntos = 0;
bool redibujando = false;
unsigned long redibujoSig = 0;
int contadorPunto = 0;

OneWire oneWire(PIN_DS18B20);
DallasTemperature sensores(&oneWire);
Servo dimmer;
Preferences prefs;

float setpoint = 40.0;
float kp = 10.0;
float ki = 0.0;
float kd = 0.0;

float actual = 0.0;
float error  = 0.0;
float termP = 0.0, termI = 0.0, termD = 0.0;
float u = 0.0;
int   angulo = ANGULO_MIN;

float tempAnterior = 0.0;
bool  hayTempAnterior = false;
unsigned long tUltimoPID = 0;
bool  sensorOK = false;
bool  sobretemp = false;

const unsigned long PERIODO_PID_MS = 1000;
const unsigned long ESPERA_CONVERSION_MS = 800;
unsigned long tPeticion = 0;
bool esperando = false;

char estadoActual[16] = "";

bool paramsEnviados1 = false;
bool paramsEnviados2 = false;

void nxCmd(const char* cmd) {
  Serial2.print(cmd);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
  Serial2.write(0xFF);
}

void nxNumero(const char* nombre, long valor) {
  char buf[40];
  snprintf(buf, sizeof(buf), "%s.val=%ld", nombre, valor);
  nxCmd(buf);
}

void nxTexto(const char* nombre, const char* texto) {
  char buf[48];
  snprintf(buf, sizeof(buf), "%s.txt=\"%s\"", nombre, texto);
  nxCmd(buf);
}

void nxPunto(int canal, int y) {
  char buf[32];
  snprintf(buf, sizeof(buf), "add %d,%d,%d", ID_GRAFICA, canal, y);
  nxCmd(buf);
}

void setEstado(const char* txt) {
  if (strcmp(estadoActual, txt) == 0) return;
  strncpy(estadoActual, txt, sizeof(estadoActual) - 1);
  estadoActual[sizeof(estadoActual) - 1] = '\0';
  nxTexto("t_estado", estadoActual);
}

void enviarParametrosAPantalla() {
  nxNumero("cx_sp", lround(setpoint * 10));
  nxNumero("cx_kp", lround(kp * 10));
  nxNumero("cx_ki", lround(ki * 100));
  nxNumero("cx_kd", lround(kd * 10));
}

int aPixeles(float valorC) {
  int y = (int)((valorC - T_MIN_G) * ALTO_GRAFICA / (T_MAX_G - T_MIN_G));
  return constrain(y, 0, ALTO_GRAFICA - 1);
}

void guardarHistoria(int yTemp, int ySet) {
  int k = totalPuntos % HIST_N;
  histT[k] = (uint8_t)yTemp;
  histS[k] = (uint8_t)ySet;
  totalPuntos++;
}

void iniciarRedibujo() {
  char buf[24];
  snprintf(buf, sizeof(buf), "cle %d,255", ID_GRAFICA);
  nxCmd(buf);
  redibujoSig = (totalPuntos > HIST_N) ? totalPuntos - HIST_N : 0;
  redibujando = true;
  estadoActual[0] = '\0';
  setEstado(sensorOK ? (sobretemp ? "SOBRETEMP" : "OK") : "ERROR SENSOR");
}

void pasoRedibujo() {
  if (!redibujando) return;
  int enviados = 0;
  while (enviados < 6 && Serial2.availableForWrite() >= 200) {
    unsigned long minimo = (totalPuntos > HIST_N) ? totalPuntos - HIST_N : 0;
    if (redibujoSig < minimo) redibujoSig = minimo;
    if (redibujoSig >= totalPuntos) { redibujando = false; break; }
    int k = redibujoSig % HIST_N;
    nxPunto(0, histT[k]);
    nxPunto(1, histS[k]);
    redibujoSig++;
    enviados++;
  }
}

void guardarConfiguracion() {
  prefs.begin("horno", false);
  prefs.putFloat("sp", setpoint);
  prefs.putFloat("kp", kp);
  prefs.putFloat("ki", ki);
  prefs.putFloat("kd", kd);
  prefs.end();
}

void leerParametros() {
  byte d[16];
  int n = 0;
  unsigned long t0 = millis();
  while (n < 16 && millis() - t0 < 200) {
    if (Serial2.available()) d[n++] = Serial2.read();
  }
  if (n < 16) return;

  int32_t v[4];
  for (int i = 0; i < 4; i++) {
    v[i] = (int32_t)((uint32_t)d[4 * i] |
                     ((uint32_t)d[4 * i + 1] << 8) |
                     ((uint32_t)d[4 * i + 2] << 16) |
                     ((uint32_t)d[4 * i + 3] << 24));
  }

  setpoint = constrain(v[0] / 10.0,  0.0, SP_MAX);
  kp       = constrain(v[1] / 10.0,  0.0, KP_MAX);
  ki       = constrain(v[2] / 100.0, 0.0, KI_MAX);
  kd       = constrain(v[3] / 10.0,  0.0, KD_MAX);
  termI = 0.0;
  guardarConfiguracion();

  Serial.print("APLICADO -> SP:"); Serial.print(setpoint, 1);
  Serial.print(" Kp:"); Serial.print(kp, 1);
  Serial.print(" Ki:"); Serial.print(ki, 2);
  Serial.print(" Kd:"); Serial.println(kd, 1);
}

void leerNextion() {
  while (Serial2.available()) {
    byte b = Serial2.read();
    if (b == 0x52)      iniciarRedibujo();
    else if (b == 0x55) leerParametros();
  }
}

void reiniciarPID() {
  termP = 0.0; termI = 0.0; termD = 0.0;
  hayTempAnterior = false;
}

void aplicarServo() {
  float grados = ANGULO_MIN + (u / 100.0) * (ANGULO_MAX - ANGULO_MIN);
  angulo = constrain((int)(grados + 0.5), ANGULO_MIN, ANGULO_MAX);
  dimmer.write(angulo);
}

void enviarPantalla() {
  nxNumero("x_vp",  lround(actual * 10));
  nxNumero("x_sp",  lround(setpoint * 10));
  nxNumero("x_err", lround(error * 10));
  nxNumero("n_out", lround(u));

  contadorPunto++;
  if (contadorPunto >= CICLOS_POR_PUNTO) {
    contadorPunto = 0;
    int yT = aPixeles(actual);
    int yS = aPixeles(setpoint);
    guardarHistoria(yT, yS);
    if (!redibujando) {
      nxPunto(0, yT);
      nxPunto(1, yS);
    }
  }
}

void falloSensor() {
  sensorOK = false;
  reiniciarPID();
  u = 0.0;
  aplicarServo();
  nxNumero("n_out", 0);
  setEstado("ERROR SENSOR");
  Serial.println("ERROR: DS18B20 no responde, foco apagado");
}

void pasoPID(float t) {
  unsigned long ahora = millis();
  float Ts = (tUltimoPID == 0) ? 1.0 : (ahora - tUltimoPID) / 1000.0;
  tUltimoPID = ahora;
  if (Ts <= 0.0) Ts = 1.0;

  actual = t;
  error = setpoint - actual;

  if (sobretemp) { if (actual <= TEMP_SEGURIDAD - 2.0) sobretemp = false; }
  else if (actual >= TEMP_SEGURIDAD) sobretemp = true;

  if (sobretemp) {
    reiniciarPID();
    u = 0.0;
    setEstado("SOBRETEMP");
  } else {
    setEstado("OK");

    termP = kp * error;

    if (hayTempAnterior) termD = -kd * (actual - tempAnterior) / Ts;
    else                 termD = 0.0;
    tempAnterior = actual;
    hayTempAnterior = true;

    float integralNueva = termI + ki * error * Ts;
    float uPrueba = termP + integralNueva + termD;
    bool satArriba = (uPrueba > 100.0 && error > 0);
    bool satAbajo  = (uPrueba < 0.0   && error < 0);
    if (!satArriba && !satAbajo) termI = integralNueva;
    termI = constrain(termI, 0.0, 100.0);

    u = constrain(termP + termI + termD, 0.0, 100.0);
  }

  aplicarServo();
  enviarPantalla();

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

void leerSensor() {
  unsigned long ahora = millis();

  if (!esperando && ahora - tPeticion >= PERIODO_PID_MS) {
    sensores.requestTemperatures();
    tPeticion = ahora;
    esperando = true;
  }

  if (esperando && ahora - tPeticion >= ESPERA_CONVERSION_MS) {
    esperando = false;
    float t = sensores.getTempCByIndex(0);
    if (t == DEVICE_DISCONNECTED_C || t < -50 || t > 125) {
      falloSensor();
    } else {
      sensorOK = true;
      pasoPID(t);
    }
  }
}

void setup() {
  Serial.begin(115200);

  digitalWrite(RELAY_PELTIER, RELAY_ACTIVO);
  digitalWrite(RELAY_EXTRACTOR, EXTRACTOR_SIEMPRE_ON ? RELAY_ACTIVO : RELAY_INACTIVO);
  pinMode(RELAY_PELTIER, OUTPUT);
  pinMode(RELAY_EXTRACTOR, OUTPUT);

  dimmer.setPeriodHertz(50);
  dimmer.attach(PIN_SERVO, 500, 2400);
  dimmer.write(ANGULO_MIN);

  Serial2.setTxBufferSize(512);
  Serial2.begin(NEXTION_BAUD, SERIAL_8N1, NEXTION_RX, NEXTION_TX);

  sensores.begin();
  sensores.setResolution(12);
  sensores.setWaitForConversion(false);

  prefs.begin("horno", false);
  setpoint = constrain(prefs.getFloat("sp", 40.0), 0.0, SP_MAX);
  kp       = constrain(prefs.getFloat("kp", 10.0), 0.0, KP_MAX);
  ki       = constrain(prefs.getFloat("ki", 0.0),  0.0, KI_MAX);
  kd       = constrain(prefs.getFloat("kd", 0.0),  0.0, KD_MAX);
  prefs.end();

  Serial.println("=== Control PID del horno (ESP32 + Nextion) ===");
  Serial.print("SP:"); Serial.print(setpoint, 1);
  Serial.print(" Kp:"); Serial.print(kp, 1);
  Serial.print(" Ki:"); Serial.print(ki, 2);
  Serial.print(" Kd:"); Serial.println(kd, 1);
  Serial.println("t_s,SP,T,E,P,I,D,U,Servo");
}

void loop() {
  leerNextion();
  leerSensor();
  pasoRedibujo();

  if (!paramsEnviados1 && millis() > 2000) { enviarParametrosAPantalla(); paramsEnviados1 = true; }
  if (!paramsEnviados2 && millis() > 6000) { enviarParametrosAPantalla(); paramsEnviados2 = true; }
}
```

---

## ▶️ 7. Compilación y puesta en marcha

### Paso 1. Abrir el proyecto

Abre la carpeta del proyecto en Visual Studio Code con PlatformIO instalado.

### Paso 2. Cargar el proyecto en la pantalla

Carga el archivo `.HMI` en la Nextion, ya sea desde una tarjeta microSD o desde el editor Nextion. Verifica que los nombres de los componentes coincidan con los de la sección 4.

### Paso 3. Compilar y cargar

```bash
pio run --target upload
```

### Paso 4. Abrir el monitor serie

```bash
pio device monitor
```

### Paso 5. Verificar el funcionamiento

1. Comprueba que la pantalla muestre VP, SP, error y salida.
2. Modifica el SP desde la pantalla de configuración y pulsa Aplicar.
3. Observa cómo el servomotor mueve la perilla del dimmer.
4. Revisa la gráfica de evolución de la temperatura.

### 📊 Registro por puerto serie

Cada segundo se imprime una línea en formato CSV a 115200 baudios, útil para analizar la respuesta en Excel:

```text
t_s,SP,T,E,P,I,D,U,Servo
```

| Campo | Descripción |
|---|---|
| t_s | Tiempo desde el arranque (s) |
| SP | Setpoint (°C) |
| T | Temperatura medida (°C) |
| E | Error (°C) |
| P, I, D | Aporte de cada acción de control |
| U | Salida de control (%) |
| Servo | Ángulo del servomotor (°) |

---

## 🧪 8. Pruebas funcionales

| Prueba | Procedimiento | Resultado esperado |
|---|---|---|
| Inicialización | Encender el sistema | Se inicializan sensor, servomotor y pantalla |
| Lectura | Observar la pantalla | Se actualizan VP, SP, error y salida |
| Cambio de SP | Aplicar un nuevo setpoint desde la HMI | Se actualiza el SP y el control responde |
| Cambio de Kp, Ki y Kd | Aplicar nuevas ganancias | Los parámetros se actualizan y se guardan |
| Persistencia | Reiniciar la ESP32 | Se conservan los parámetros guardados |
| Error positivo | Temperatura inferior al SP | La salida es positiva y el servomotor se mueve |
| Error cero | Temperatura igual al SP | La acción proporcional es cero |
| Saturación | Aplicar un error elevado | La salida queda limitada al 100 % |
| Anti-windup | Mantener la salida saturada y luego acercarse al SP | La integral no se dispara |
| Gráfica | Cambiar de pantalla y regresar | Se redibuja el historial |
| Valores fuera de rango | Introducir un valor mayor al máximo | Se limita al máximo permitido |
| Sobretemperatura | Alcanzar 80 °C | El foco se apaga y el estado indica `SOBRETEMP` |
| Fallo del sensor | Desconectar el DS18B20 | El foco se apaga y el estado indica `ERROR SENSOR` |

Los resultados deben comprobarse experimentalmente. Esta tabla representa los criterios de aceptación del proyecto.

---

## 🛡️ 9. Protecciones

| Situación | Respuesta del sistema |
|---|---|
| ❌ DS18B20 desconectado o lectura inválida | Servomotor en el ángulo mínimo (foco apagado), PID reiniciado y estado `ERROR SENSOR` |
| 🔥 Temperatura ≥ 80 °C | Foco apagado y estado `SOBRETEMP`; se reanuda al bajar a 78 °C (histéresis de 2 °C) |
| 🔢 Parámetros fuera de rango | Se limitan a los máximos permitidos |
| 🔁 Cambio de parámetros | Se reinicia la integral para evitar saltos del servomotor |

---

## 📊 10. Conceptos de ingeniería aplicados

El desarrollo del proyecto permite estudiar los siguientes conceptos:

- Sistemas embebidos basados en microcontroladores.
- Adquisición de variables físicas mediante sensores digitales (OneWire).
- Lectura no bloqueante de sensores.
- Interfaces hombre-máquina (HMI) y comunicación serie UART.
- Control automático PID.
- Acciones proporcional, integral y derivativa.
- Saturación de la acción de control y anti-windup.
- Derivada sobre la medición.
- Conversión de señales de control a posiciones angulares.
- Accionamiento de mecanismos mediante servomotores.
- Regulación de potencia mediante dimmers.
- Control de cargas mediante módulos de relé.
- Almacenamiento de parámetros en memoria no volátil.
- Respuesta dinámica de sistemas térmicos.
- Sintonización de controladores.

### 🚀 Posibles mejoras

- Controlar la Peltier según la acción de control para enfriar activamente (control en ambos sentidos).
- Implementar autosintonización del PID.
- Incorporar un cruce por cero para controlar el foco con TRIAC en lugar del servomotor.
- Registrar los datos directamente en una tarjeta SD.
- Implementar comunicación inalámbrica para monitoreo remoto.
- Identificar experimentalmente el modelo dinámico del proceso térmico.
- Comparar la respuesta de los controladores ON/OFF, P y PID.

---

## 📈 11. Análisis del comportamiento del controlador

Se recomienda evaluar el sistema variando cada ganancia de forma independiente.

| Ganancia | Efecto al aumentarla | Riesgo si es excesiva |
|---|---|---|
| Kp | Respuesta más rápida y menor error | Sobreimpulso y oscilaciones |
| Ki | Elimina el error en estado estable | Sobreimpulso y oscilaciones lentas |
| Kd | Amortigua la respuesta y reduce el sobreimpulso | Sensibilidad al ruido de la medición |

### 🔧 Procedimiento sugerido de sintonización

1. Iniciar con Ki = 0 y Kd = 0 y ajustar Kp hasta obtener una respuesta rápida sin oscilar demasiado.
2. Incrementar Ki gradualmente para eliminar el error en estado estable.
3. Incrementar Kd para reducir el sobreimpulso.
4. Registrar la respuesta con el CSV del puerto serie y comparar entre pruebas.

Se propone registrar y graficar las siguientes variables:

- Temperatura de referencia (SP).
- Temperatura medida (T).
- Error de temperatura.
- Aporte de las acciones P, I y D.
- Acción de control (U).
- Posición angular del servomotor.

**Nota:** la respuesta real dependerá de la dinámica térmica, de la relación entre la posición angular y la potencia del dimmer, y de las características del foco.

---

## 📚 12. Resultado esperado

Al finalizar el proyecto se espera disponer de un sistema de control PID de temperatura que permita:

- Medir y visualizar la temperatura en tiempo real en una HMI Nextion.
- Graficar la evolución de la temperatura y del setpoint.
- Configurar SP, Kp, Ki y Kd desde la pantalla.
- Calcular automáticamente el error y las acciones P, I y D.
- Convertir la acción de control en una posición angular.
- Regular mecánicamente el dimmer mediante un servomotor.
- Modificar la potencia suministrada al foco.
- Proteger el sistema ante sobretemperatura y fallos del sensor.
- Analizar la respuesta del proceso térmico.

El proyecto constituye una práctica integradora para la asignatura de Sistemas Embebidos, relacionando adquisición de datos, programación de microcontroladores, control automático PID, interfaces HMI, electrónica de potencia y accionamiento electromecánico.

---
