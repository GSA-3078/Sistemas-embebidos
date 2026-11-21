# 🔥 Horno con Control de Temperatura con ESP32

### 🌡️ Control por histéresis con LM35, potenciómetros, relés, celda Peltier y LCD 16×2 mediante I²C

![ESP32](https://img.shields.io/badge/ESP32-Microcontrolador-E7352C?style=for-the-badge&logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange?style=for-the-badge&logo=platformio&logoColor=white)
![Arduino](https://img.shields.io/badge/Framework-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![Relay](https://img.shields.io/badge/M%C3%B3dulos-Rel%C3%A9-red?style=for-the-badge)
![License](https://img.shields.io/badge/Licencia-Educativa-blue?style=for-the-badge)

## 📌 Descripción del proyecto

Este proyecto consiste en desarrollar un sistema de control de temperatura para un horno utilizando una tarjeta **ESP32**, un sensor analógico **LM35**, dos potenciómetros y tres módulos de relé.

La temperatura se mide con el LM35 y se mantiene entre un límite **máximo** y uno **mínimo**, que se ajustan en tiempo real con los potenciómetros. Cuando la temperatura baja al mínimo, un **foco** de 127 V CA calienta el horno. Cuando llega al máximo, se enciende una **celda Peltier** con sus dos ventiladores y un **extractor** para enfriarlo.

Los límites, la temperatura y el modo de operación se muestran en una pantalla LCD de 16 columnas por 2 filas, utilizando el protocolo de comunicación **I²C**. Además, las lecturas se envían al monitor serie para su supervisión.

> ⚠️ **ADVERTENCIA DE SEGURIDAD:** el foco trabaja con **127 V de corriente alterna** y el sistema genera **calor**. Arme y modifique el circuito siempre **desconectado**, utilice cable y portalámparas que soporten temperatura, y deje las conexiones de 127 V dentro de una caja cerrada.

## 🎯 Objetivo general

Implementar un sistema embebido basado en ESP32 capaz de medir la temperatura de un horno con un sensor LM35 y controlarla por histéresis, activando un foco, una celda Peltier y un extractor mediante módulos de relé, con límites ajustables por potenciómetros y visualización en una pantalla LCD con comunicación I²C.

### 🎓 Objetivos específicos

* 🔌 Configurar entradas analógicas de la ESP32 para adquirir señales eléctricas.
* 🌡️ Convertir el voltaje de salida del LM35 en grados Celsius.
* 🧮 Aplicar un filtro de promedio móvil para reducir el ruido de la medición.
* 🎚️ Ajustar los límites de temperatura mediante dos potenciómetros.
* 🔁 Implementar un control de temperatura por histéresis.
* ⚡ Controlar cargas de CA y CD mediante módulos de relé.
* 🛡️ Detectar y manejar fallas del sensor de forma segura.
* 🖥️ Implementar la comunicación I²C para visualizar los datos en una pantalla LCD.
* 📡 Enviar las mediciones al monitor serie para su supervisión.

## 🧰 Materiales y herramientas

| Componente                                   | Función                                           |
| -------------------------------------------- | ------------------------------------------------- |
| 🔴 ESP32 DevKit V1                           | Adquisición, procesamiento y control              |
| 🌡️ Sensor LM35                              | Medición de la temperatura del horno              |
| 🎚️ Potenciómetro de 10 kΩ (×2)              | Ajuste de la temperatura máxima y mínima          |
| 🖥️ LCD 16×2 con módulo I²C                  | Visualización de las variables                    |
| ⚡ Módulo de relé de 1 canal, 5 V (×3)       | Activación de foco, Peltier y extractor           |
| ❄️ Celda Peltier 12 V (TEC1-12706)          | Enfriamiento del horno                            |
| 🧊 Disipador de aluminio (×2)                | Disipación del lado frío y del lado caliente      |
| 🌀 Ventilador de 12 V para disipador (×2)    | Ventilación de los disipadores de la Peltier      |
| 🌀 Ventilador extractor de 12 V              | Extracción del aire caliente                      |
| 💡 Foco incandescente con portalámparas      | Calentamiento del horno                           |
| 🔋 Fuente de 12 V CD (mínimo 8 A)            | Alimentación de la Peltier y los ventiladores     |
| 🔌 Clavija con cable de 127 V CA             | Alimentación del foco                             |
| 🔗 Cables y protoboard                       | Interconexión de los componentes                  |
| 💻 Visual Studio Code                        | Entorno de desarrollo                             |
| ⚙️ PlatformIO                                | Compilación y gestión del proyecto                |

## 🔌 Diagrama de conexiones

### 📍 Asignación de pines de la ESP32

| Dispositivo                     | Pin del dispositivo | Pin ESP32 |
| ------------------------------- | ------------------- | --------- |
| Sensor LM35                     | Vout                | GPIO 34   |
| Potenciómetro de máximo         | Terminal central    | GPIO 32   |
| Potenciómetro de mínimo         | Terminal central    | GPIO 33   |
| Relé del foco                   | IN                  | GPIO 26   |
| Relé de la Peltier              | IN                  | GPIO 27   |
| Relé del extractor              | IN                  | GPIO 13   |
| LCD I²C                         | SDA                 | GPIO 21   |
| LCD I²C                         | SCL                 | GPIO 22   |
| Potenciómetros                  | Extremos            | 3.3 V y GND |
| LM35, LCD y módulos de relé     | Alimentación        | VIN (5 V) |
| Todos los dispositivos          | Tierra              | GND común |

**⚠️ Importante:** conecta cada potenciómetro entre **3.3 V** y GND, utilizando su terminal central como salida analógica. El LM35 se alimenta con **5 V**.

### 🌡️ Conexión del sensor LM35

Visto de frente (cara plana hacia ti y patas hacia abajo):

```text
      ┌─────┐
      │LM35 │
      └─┬─┬─┬┘
        │ │ │
       +Vs Vout GND
```

| LM35 | ESP32     |
| ---- | --------- |
| +Vs  | VIN (5 V) |
| Vout | GPIO 34   |
| GND  | GND       |

> ⚠️ Con 3.3 V el LM35 trabaja fuera de su rango recomendado y la medición no es confiable. Con 5 V, su salida máxima de 1.5 V (150 °C) es segura para el ADC. Opcionalmente, coloca un capacitor de 0.1 µF entre Vout y GND para reducir el ruido.

### 🎚️ Conexión de los potenciómetros

```text
      3.3 V
        │
      ┌─┴─┐
      │   │
      │   ├──────── GPIO (32 o 33)
      │   │  cursor
      └─┬─┘
        │
       GND
```

### ⚡ Conexión de los módulos de relé

Los tres módulos se alimentan con 5 V (VIN) y GND, y se activan con nivel **LOW** (`LOW` = encendido, `HIGH` = apagado).

| Módulo de relé | Carga que controla                       |
| -------------- | ---------------------------------------- |
| Relé foco      | Foco (127 V CA)                          |
| Relé Peltier   | Celda Peltier + 2 ventiladores (12 V CD) |
| Relé extractor | Extractor (12 V CD)                      |

**Cargas de 12 V (Peltier y extractor):** los **COM de ambos relés van unidos** al positivo (+) de la fuente. Cada carga se conecta al **NO** de su relé y su negativo regresa al negativo (−) de la fuente. La Peltier y sus dos ventiladores (uno por disipador) van **en paralelo**.

| Origen                                   | Destino                                       |
| ---------------------------------------- | --------------------------------------------- |
| Fuente 12 V (+)                          | COM del relé Peltier y COM del relé extractor |
| NO del relé Peltier                      | (+) de la Peltier y de sus 2 ventiladores     |
| NO del relé extractor                    | (+) del extractor                             |
| (−) de Peltier, ventiladores y extractor | Fuente 12 V (−)                               |

**Foco (127 V CA):**

| Origen                         | Destino                               |
| ------------------------------ | ------------------------------------- |
| Clavija, fase (cable negro)    | COM del relé del foco                 |
| NO del relé del foco           | Foco                                  |
| Clavija, neutro (cable blanco) | Foco (directo, sin pasar por el relé) |

> ⚠️ Revisa la serigrafía de tus módulos, ya que el orden de las terminales (NO, COM, NC) puede variar entre fabricantes.

### 🔗 Esquema general

```text
                          ESP32
                  ┌─────────────────┐
                  │                 │
                  │        GPIO 34  ├──── Vout  LM35 (+Vs a VIN, GND a GND)
                  │        GPIO 32  ├──── Cursor POT MÁX
                  │        GPIO 33  ├──── Cursor POT MÍN
                  │                 │
                  │        GPIO 26  ├──── IN  Relé FOCO
                  │        GPIO 27  ├──── IN  Relé PELTIER
                  │        GPIO 13  ├──── IN  Relé EXTRACTOR
                  │                 │
                  │        GPIO 21  ├──── SDA  (LCD)
                  │        GPIO 22  ├──── SCL  (LCD)
                  │                 │
                  │            3V3  ├──── Extremos de los potenciómetros
                  │            VIN  ├──── VCC (LCD, LM35 y relés)
                  │            GND  ├──── GND (todo)
                  └─────────────────┘

  Clavija 127 V CA ──► [Relé FOCO] ──────────► FOCO

  Fuente 12 V (+) ──┬─► [Relé PELTIER]   ──► Peltier + 2 ventiladores ──┐
                    └─► [Relé EXTRACTOR] ──► Extractor ─────────────────┤
  Fuente 12 V (−) ◄───────────────────────────────────────────────────┘
```

## ⚙️ Fundamento teórico

### 1. 🎚️ Conversión analógica a digital

La ESP32 utiliza su convertidor analógico-digital (ADC) para leer el LM35 y los potenciómetros. Con una resolución de 12 bits, las lecturas están entre 0 y 4095.

$$
V_{\text{in}}=\frac{ADC}{4095}\,V_{\text{ref}}
$$

Donde:

* \(V_{\text{in}}\): voltaje estimado de entrada.
* \(ADC\): lectura digital obtenida.
* \(V_{\text{ref}}\): voltaje de referencia nominal (3.3 V).

**Nota:** el programa usa `analogReadMilliVolts()`, que aplica la calibración de fábrica del ESP32 y reduce el error de la conversión nominal.

### 2. 🌡️ Medición de temperatura con LM35

El LM35 entrega una salida lineal de **10 mV por cada grado Celsius**:

$$
T_{^\circ C}=\frac{V_{\text{out}}\,[\text{mV}]}{10}
$$

Ejemplo: 250 mV equivalen a 25.0 °C.

### 3. 🧮 Filtro de promedio móvil

Para reducir el ruido del ADC, el programa promedia las últimas cinco lecturas, tomadas cada 100 ms:

$$
T_{\text{filtrada}}=\frac{T_k+T_{k-1}+T_{k-2}+T_{k-3}+T_{k-4}}{5}
$$

En la primera lectura, las cinco variables se llenan con el mismo valor para evitar promediar ceros.

### 4. 🎚️ Ajuste de los límites con potenciómetros

La función `map()` convierte la lectura del ADC en grados:

| Potenciómetro | Lectura ADC | Temperatura |
| ------------- | ----------- | ----------- |
| Máximo        | 0 – 4095    | 5 – 99 °C   |
| Mínimo        | 0 – 4095    | 0 – 94 °C   |

El programa mantiene siempre una diferencia mínima de **5 °C** entre ambos límites:

$$
T_{max}\geq T_{min}+5
$$

### 5. 🔁 Control por histéresis

El sistema alterna entre dos modos y mantiene el actual mientras la temperatura esté entre los límites:

```text
Temperatura
    ▲
Max ├─ ─ ─ ─ ─ ─ ─ ┌──────┐ ─ ─ ─  ← Al llegar a Max: apaga foco, enciende Peltier
    │            ╱         ╲
    │          ╱             ╲
Min ├─ ─ ─ ─ ╱─ ─ ─ ─ ─ ─ ─ ─ ╲ ─ ─  ← Al llegar a Min: apaga Peltier, enciende foco
    │      ╱
    └──────────────────────────────► Tiempo
      CALOR      FRÍO       CALOR
```

* Si \(T \geq T_{max}\) mientras calienta, cambia a **Frío**.
* Si \(T \leq T_{min}\) mientras enfría, cambia a **Calor**.

Esto evita que los relés se enciendan y apaguen constantemente.

### 6. 📊 Porcentaje de avance

La segunda línea de la LCD muestra qué tan cerca está la temperatura del rango:

$$
P=\frac{T-T_{min}}{T_{max}-T_{min}}\times 100
$$

El resultado se limita entre 0 % y 100 %.

### 7. ⚡ Estados del sistema

Los relés se activan con `LOW`.

| Estado                    | Foco | Peltier + ventiladores | Extractor          |
| ------------------------- | :--: | :--------------------: | :----------------: |
| Leyendo sensor (inicio)   | OFF  | OFF                    | sin cambio         |
| **Calentando** (`Calor`)  | ON   | OFF                    | OFF                |
| **Enfriando** (`Frio`)    | OFF  | ON                     | ON                 |
| Pausa entre cambios (1 s) | OFF  | OFF                    | conserva el último |
| **Error de sensor**       | OFF  | OFF                    | ON                 |

La pausa de 1 s evita que el foco y la Peltier funcionen al mismo tiempo al cambiar de modo. Si el voltaje del LM35 supera 1.5 V (150 °C), se detecta una falla, se apagan el foco y la Peltier, y se enciende el extractor por seguridad.

## 🖥️ Visualización de datos

La pantalla LCD muestra los límites configurados, la temperatura medida, el porcentaje y el modo actual.

Calentando:

```text
┌────────────────┐
│Max:60 Min:30   │
│45.0C 50% Calor │
└────────────────┘
```

Enfriando:

```text
┌────────────────┐
│Max:60 Min:30   │
│58.5C 95% Frio  │
└────────────────┘
```

Al iniciar o si falla el sensor:

```text
┌────────────────┐      ┌────────────────┐
│Max:60 Min:30   │      │Max:60 Min:30   │
│Leyendo LM35    │      │Error LM35      │
└────────────────┘      └────────────────┘
```

Las lecturas también se envían al monitor serie a **9600 baudios**:

```text
LM35: 25.3 C | Filtrada: 25.1 C | Max: 60 | Min: 30
LM35: 25.4 C | Filtrada: 25.2 C | Max: 60 | Min: 30
```

### 📊 Variables monitoreadas

| Variable                | Unidad | Origen                 |
| ----------------------- | ------ | ---------------------- |
| Temperatura medida      | °C     | GPIO 34 (LM35)         |
| Temperatura filtrada    | °C     | Cálculo (promedio de 5) |
| Temperatura máxima      | °C     | GPIO 32 (potenciómetro) |
| Temperatura mínima      | °C     | GPIO 33 (potenciómetro) |
| Porcentaje de avance    | %      | Cálculo                |

*Los valores mostrados son únicamente ejemplos; no representan mediciones experimentales.*

## 💻 Código

Archivo:

```text
src/main.cpp
```

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines
int PIN_LM35 = 34;
int POT_MAX = 32;
int POT_MIN = 33;

int rele_foco = 26;
int rele_peltier = 27;
int rele_extractor = 13;

// Temperaturas
int tempMax = 60;
int tempMin = 30;

float tempActual = 0.0;
float porcentaje = 0.0;

int margen = 5;

// Filtro de promedio movil
float tk = 0;
float tk_1 = 0;
float tk_2 = 0;
float tk_3 = 0;
float tk_4 = 0;

float tk_filtrada = 0;

int filtroListo = 0;

// Estados del sistema
int calentando = 1;
int inicioListo = 0;
int sensorOK = 0;
int errorSensor = 0;
int enPausa = 0;

// Tiempos
unsigned long tiempoSensor = 0;
unsigned long intervaloSensor = 100;

unsigned long tiempoCambio = 0;
unsigned long pausaCambio = 1000;

unsigned long tiempoLCD = 0;
unsigned long intervaloLCD = 300;

void setup() {

  pinMode(PIN_LM35, INPUT);
  pinMode(POT_MAX, INPUT);
  pinMode(POT_MIN, INPUT);

  pinMode(rele_foco, OUTPUT);
  pinMode(rele_peltier, OUTPUT);
  pinMode(rele_extractor, OUTPUT);

  // Apagar relevadores (activos en LOW)
  digitalWrite(rele_foco, HIGH);
  digitalWrite(rele_peltier, HIGH);
  digitalWrite(rele_extractor, HIGH);

  analogReadResolution(12);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(9600);
}

void loop() {

  // --------------------------------
  // 1. LEER POTENCIOMETROS
  // --------------------------------

  int lecturaMax = analogRead(POT_MAX);
  int lecturaMin = analogRead(POT_MIN);

  tempMax = map(lecturaMax, 0, 4095, 5, 99);
  tempMin = map(lecturaMin, 0, 4095, 0, 94);

  // Mantener margen minimo de 5 grados
  if (tempMax < tempMin + margen) {
    tempMax = tempMin + margen;
  }

  if (tempMax > 99) {
    tempMax = 99;
  }

  if (tempMin > tempMax - margen) {
    tempMin = tempMax - margen;
  }

  if (tempMin < 0) {
    tempMin = 0;
  }

  // --------------------------------
  // 2. LEER LM35 Y APLICAR FILTRO
  // --------------------------------

  if (millis() - tiempoSensor >= intervaloSensor) {

    tiempoSensor = millis();

    int milivoltios = analogReadMilliVolts(PIN_LM35);

    // LM35: aproximadamente 10 mV por grado Celsius
    float lectura = milivoltios / 10.0;

    // Validar voltaje del sensor
    if (milivoltios > 1500) {

      errorSensor = 1;
      sensorOK = 0;
      inicioListo = 0;
      enPausa = 0;
      filtroListo = 0;

      digitalWrite(rele_foco, HIGH);
      digitalWrite(rele_peltier, HIGH);
      digitalWrite(rele_extractor, LOW);

      Serial.println("Error: revisar LM35");

    } else {

      // Primera lectura: llenar el filtro
      // para evitar promediar valores iniciales de cero
      if (filtroListo == 0) {

        tk = lectura;
        tk_1 = lectura;
        tk_2 = lectura;
        tk_3 = lectura;
        tk_4 = lectura;

        tk_filtrada = lectura;

        filtroListo = 1;

      } else {

        // Nueva lectura
        tk = lectura;

        // Promedio movil de cinco lecturas
        tk_filtrada = (tk + tk_1 + tk_2 +
                       tk_3 + tk_4) / 5.0;

        // Desplazar lecturas anteriores
        tk_4 = tk_3;
        tk_3 = tk_2;
        tk_2 = tk_1;
        tk_1 = tk;
      }

      tempActual = tk_filtrada;

      errorSensor = 0;
      sensorOK = 1;

      // --------------------------------
      // 3. INICIALIZAR CONTROL
      // --------------------------------

      if (inicioListo == 0) {

        if (tempActual >= tempMax) {
          calentando = 0;
        } else {
          calentando = 1;
        }

        inicioListo = 1;
        enPausa = 1;
        tiempoCambio = millis();

        digitalWrite(rele_foco, HIGH);
        digitalWrite(rele_peltier, HIGH);

        if (calentando == 0) {
          digitalWrite(rele_extractor, LOW);
        } else {
          digitalWrite(rele_extractor, HIGH);
        }
      }

      // --------------------------------
      // 4. CALCULAR PORCENTAJE
      // --------------------------------

      porcentaje = ((tempActual - tempMin) /
                    (tempMax - tempMin)) * 100.0;

      if (porcentaje > 100.0) {
        porcentaje = 100.0;
      }

      if (porcentaje < 0.0) {
        porcentaje = 0.0;
      }

      // --------------------------------
      // 5. CAMBIO DE ESTADO
      // --------------------------------

      if (inicioListo == 1 && errorSensor == 0) {

        // De calentamiento a enfriamiento
        if (calentando == 1 && tempActual >= tempMax) {

          calentando = 0;
          enPausa = 1;
          tiempoCambio = millis();

          digitalWrite(rele_foco, HIGH);
          digitalWrite(rele_peltier, HIGH);
          digitalWrite(rele_extractor, LOW);
        }

        // De enfriamiento a calentamiento
        else if (calentando == 0 && tempActual <= tempMin) {

          calentando = 1;
          enPausa = 1;
          tiempoCambio = millis();

          digitalWrite(rele_foco, HIGH);
          digitalWrite(rele_peltier, HIGH);
          digitalWrite(rele_extractor, HIGH);
        }
      }

      // Monitor serial
      Serial.print("LM35: ");
      Serial.print(lectura, 1);

      Serial.print(" C | Filtrada: ");
      Serial.print(tempActual, 1);

      Serial.print(" C | Max: ");
      Serial.print(tempMax);

      Serial.print(" | Min: ");
      Serial.println(tempMin);
    }
  }

  // --------------------------------
  // 6. PAUSA DE SEGURIDAD
  // --------------------------------

  if (enPausa == 1 &&
      millis() - tiempoCambio >= pausaCambio) {

    enPausa = 0;
  }

  // --------------------------------
  // 7. CONTROL DE RELEVADORES
  // --------------------------------

  if (errorSensor == 1 ||
      sensorOK == 0 ||
      inicioListo == 0) {

    digitalWrite(rele_foco, HIGH);
    digitalWrite(rele_peltier, HIGH);

  } else if (enPausa == 1) {

    digitalWrite(rele_foco, HIGH);
    digitalWrite(rele_peltier, HIGH);

  } else {

    if (calentando == 1) {

      digitalWrite(rele_peltier, HIGH);
      digitalWrite(rele_foco, LOW);
      digitalWrite(rele_extractor, HIGH);

    } else {

      digitalWrite(rele_foco, HIGH);
      digitalWrite(rele_peltier, LOW);
      digitalWrite(rele_extractor, LOW);
    }
  }

  // --------------------------------
  // 8. ACTUALIZAR LCD
  // --------------------------------

  if (millis() - tiempoLCD >= intervaloLCD) {

    tiempoLCD = millis();

    lcd.setCursor(0, 0);

    lcd.print("Max:");
    lcd.print(tempMax);
    lcd.print(" Min:");
    lcd.print(tempMin);
    lcd.print("   ");

    lcd.setCursor(0, 1);

    if (errorSensor == 1) {

      lcd.print("Error LM35      ");

    } else if (sensorOK == 0 || inicioListo == 0) {

      lcd.print("Leyendo LM35    ");

    } else {

      lcd.print(tempActual, 1);
      lcd.print("C ");
      lcd.print(porcentaje, 0);
      lcd.print("% ");

      if (calentando == 1) {
        lcd.print("Calor");
      } else {
        lcd.print("Frio ");
      }
    }
  }
}
```

## 🚀 Instalación y ejecución

### 1. 💻 Preparar el entorno

Instala las siguientes herramientas:

* [Visual Studio Code](https://code.visualstudio.com/)
* [Extensión PlatformIO IDE](https://platformio.org/install/ide?install=vscode)

### 2. 📁 Crear el proyecto

En PlatformIO:

1. Selecciona **New Project**.
2. Elige la tarjeta **ESP32 DevKit V1** (`esp32dev`).
3. Selecciona el framework **Arduino**.
4. Define el nombre del proyecto.
5. Espera a que se genere la estructura de archivos.

### 3. 📦 Instalar la biblioteca LCD

Agrega la siguiente dependencia en `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 9600

lib_deps =
    marcoschwartz/LiquidCrystal_I2C
```

> La biblioteca `Wire` ya viene incluida con el framework, por lo que no se agrega.

### 4. 📝 Agregar el programa

Copia el código del sistema en `src/main.cpp` y agrega `#include <Arduino.h>` como primera línea.

### 5. 🔧 Armar y probar el circuito

1. Arma primero el lado de baja tensión: ESP32, LM35, potenciómetros, LCD y entradas de los relés.
2. Conecta los 12 V: fuente a los COM de Peltier y extractor, cargas a los NO y negativos a la fuente.
3. Conecta el foco con la clavija **desconectada**.
4. Coloca el LM35 dentro del horno, sin que toque el foco ni la Peltier directamente.
5. Carga el código y conecta el ESP32 por USB.
6. Verifica la LCD y gira los potenciómetros para comprobar los límites.
7. Conecta la fuente de 12 V y, por último, la clavija del foco.

## 🔎 Dirección I²C del LCD

La dirección más común de los módulos LCD I²C es `0x27`, aunque algunos usan `0x3F`. Si el LCD está conectado pero no muestra información, cambia:

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

por:

```cpp
LiquidCrystal_I2C lcd(0x3F, 16, 2);
```

Para identificarla automáticamente, usa este escáner I²C:

```cpp
#include <Arduino.h>
#include <Wire.h>

void setup()
{
    Serial.begin(115200);

    Wire.begin(21, 22);

    Serial.println("Escaner I2C");

    for (byte address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);

        byte error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("Dispositivo encontrado en 0x");

            if (address < 16)
                Serial.print("0");

            Serial.println(address, HEX);
        }
    }
}

void loop()
{
}
```

## 🧪 Pruebas de funcionamiento

| Prueba | Acción                                      | Resultado esperado                                         |
| ------ | ------------------------------------------- | ---------------------------------------------------------- |
| 1️⃣    | Girar el potenciómetro de máximo            | Cambia `Max` en la LCD (5 a 99)                            |
| 2️⃣    | Girar el potenciómetro de mínimo            | Cambia `Min` en la LCD (0 a 94)                            |
| 3️⃣    | Acercar Max y Min a menos de 5 °C           | Se mantiene una diferencia de 5 °C entre ambos             |
| 4️⃣    | Calentar el LM35 con la mano                | Aumenta la temperatura mostrada                            |
| 5️⃣    | Temperatura menor que Max                   | Modo `Calor`: foco encendido                               |
| 6️⃣    | Temperatura igual o mayor que Max           | Pausa de 1 s y modo `Frio`: Peltier y extractor encendidos |
| 7️⃣    | Temperatura igual o menor que Min           | Pausa de 1 s y regreso al modo `Calor`                     |
| 8️⃣    | Desconectar el pin de salida del LM35       | Puede aparecer `Error LM35` y se enciende el extractor     |
| 9️⃣    | Abrir el monitor serie a 9600 baudios       | Se muestran las mediciones cada 100 ms                     |

## 🛠️ Solución de problemas

| Problema                              | Posible causa                                     | Solución                                                  |
| ------------------------------------- | ------------------------------------------------- | --------------------------------------------------------- |
| LCD sin texto                         | Dirección I²C incorrecta                          | Verificar si la dirección es `0x27` o `0x3F`              |
| LCD con cuadros                       | Contraste mal ajustado                            | Girar el potenciómetro del módulo I²C                     |
| Aparece `Error LM35`                  | Voltaje de salida mayor a 1.5 V                   | Revisar conexiones del LM35                               |
| Temperatura muy alta o muy baja       | +Vs y GND del LM35 invertidos                     | Verificar el pinout con la cara plana hacia ti            |
| Temperatura inestable                 | Ruido en el ADC o cable largo                     | Agregar capacitor de 0.1 µF y acortar el cable            |
| Max o Min cambian solos ±1            | Ruido del ADC en los potenciómetros               | Promediar la lectura de los potenciómetros                |
| El potenciómetro no cambia el valor   | Cursor mal conectado                              | Cursor al GPIO y extremos a 3.3 V y GND                   |
| El valor se mueve al revés            | Extremos del potenciómetro invertidos             | Intercambiar 3.3 V y GND                                  |
| Los relés funcionan al revés          | El módulo se activa con HIGH                      | Invertir `HIGH` y `LOW` en el código                      |
| El foco no enciende                   | Conexión incorrecta COM/NO o neutro               | Revisar el relé y que el neutro llegue directo al foco    |
| La Peltier no enfría                  | Fuente, COM o polaridad incorrectos               | Revisar los 12 V, el COM unido y invertir cables si hace falta |
| Se reinicia el ESP32 al activar relés | Fuente insuficiente o GND no común                | Revisar alimentación y unir las tierras                   |
| No aparece información serie          | Puerto o velocidad incorrectos                    | Configurar 9600 baudios                                   |
| No compila el proyecto                | Biblioteca ausente o falta `Arduino.h`            | Revisar `platformio.ini` y agregar `#include <Arduino.h>` |

## ⚠️ Notas importantes

* **Lógica de los relés:** el código está hecho para módulos que se activan con **LOW**.
* **Corriente de la fuente:** una Peltier TEC1-12706 puede consumir hasta unos 6 A, más tres ventiladores. Usa una fuente de **12 V de 8 A o más** y verifica que los contactos del relé soporten esa corriente.
* **Disipación:** el lado caliente de la Peltier **siempre** debe tener disipador y ventilador. Si falla, la celda se sobrecalienta y se daña rápidamente.
* **Polaridad de la Peltier:** si el lado que calienta queda hacia el interior del horno, invierte los cables de la celda.
* **Límite del sensor:** el LM35 soporta aproximadamente entre 100 y 150 °C según el modelo. No configures `tempMax` por encima de lo que soporta.
* **Precisión:** el ADC del ESP32 no es perfectamente lineal. Compara con un termómetro de referencia y calibra si es necesario.
* **Aislamiento:** el foco y su portalámparas deben soportar el calor, y los cables de 127 V deben mantenerse lejos de las partes calientes.

## 📚 Competencias desarrolladas

* ⚡ Electrónica analógica y divisores de voltaje.
* 🔌 Configuración de entradas ADC en microcontroladores.
* 🌡️ Instrumentación y medición de temperatura.
* 🧮 Filtrado digital de señales.
* 🔁 Control por histéresis.
* ⚡ Control de cargas mediante relés.
* 🔗 Comunicación serial e interfaz I²C.
* ⏱️ Programación no bloqueante con `millis()`.
* 🛡️ Manejo de fallas y seguridad del sistema.
* 💻 Programación de sistemas embebidos en C++.

## 🔮 Posibles mejoras

* 📈 Graficar la temperatura medida y la filtrada en el Serial Plotter.
* 🎛️ Reemplazar la histéresis por un control PID con salida PWM.
* 💾 Guardar `tempMax` y `tempMin` en memoria (`Preferences`) para recuperarlos al reiniciar.
* 🚨 Agregar un LED y un buzzer para indicar el modo y las alarmas.
* ⏲️ Incorporar un temporizador que apague el sistema después de un tiempo configurable.
* 📏 Calibrar el LM35 con un termómetro de referencia (offset).
* 📲 Transmitir las mediciones por Bluetooth o Wi-Fi.
* 💾 Registrar las lecturas en archivos CSV.
