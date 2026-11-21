# 🔥 Práctica: Horno con control de temperatura (Foco + Peltier + Extractor) con ESP32, LM35 y Potenciómetros

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue)
![Arduino](https://img.shields.io/badge/Arduino-C%2FC%2B%2B-00979D)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange)
![Relay](https://img.shields.io/badge/M%C3%B3dulos-Rel%C3%A9-red)

## 📋 Descripción

En esta práctica se desarrolla un sistema embebido basado en **ESP32** que mantiene la temperatura de un horno dentro de un rango configurable. La temperatura se mide con un sensor analógico **LM35** y se muestra en una pantalla **LCD 16×2 con interfaz I2C**.

Los límites de temperatura (**máximo y mínimo**) se ajustan en tiempo real con **dos potenciómetros**, sin necesidad de botones.

El sistema alterna entre dos modos:

* 🔥 **Calentando:** se enciende un **foco** (127 V CA) para subir la temperatura.
* ❄️ **Enfriando:** se enciende una **celda Peltier** con sus dos ventiladores y un **extractor** para bajar la temperatura.

Los actuadores se manejan con **módulos de relé** (sin transistores).

La práctica integra:

* 🌡️ Adquisición de temperatura con un sensor analógico.
* 📈 Lectura de entradas analógicas (ADC de 12 bits).
* 🧮 Filtro de promedio móvil.
* 🎚️ Ajuste de parámetros con potenciómetros.
* 📟 Comunicación **I2C**.
* ⚡ Control de cargas de CA y CD mediante relés.
* 🔁 Control de temperatura por histéresis (ON/OFF).
* 🖥️ Visualización de datos en LCD y Monitor Serial.
* ⚙️ Programación no bloqueante de ESP32 utilizando C/C++.

> ⚠️ **ADVERTENCIA DE SEGURIDAD:** el foco trabaja con **127 V de corriente alterna** y el sistema genera **calor**. Arme y modifique el circuito siempre **desconectado**, utilice cable y portalámparas que soporten temperatura, y deje las conexiones de 127 V dentro de una caja cerrada.

---

## 🎯 Objetivo

Implementar un sistema de control de temperatura por histéresis utilizando un ESP32, un sensor LM35, dos potenciómetros, tres módulos de relé, una celda Peltier, un foco y una pantalla LCD 16×2 con interfaz I2C.

Al finalizar la práctica, el estudiante será capaz de:

1. Conectar un sensor LM35 al ESP32 y convertir su voltaje a grados Celsius.
2. Leer entradas analógicas con el ADC del ESP32.
3. Aplicar un filtro de promedio móvil para reducir el ruido de la medición.
4. Ajustar parámetros del sistema mediante potenciómetros.
5. Controlar cargas de CA y CD mediante módulos de relé.
6. Implementar un control de temperatura por histéresis con un límite máximo y uno mínimo.
7. Mostrar información en el LCD y en el Monitor Serial.
8. Detectar y manejar fallas del sensor.
9. Alimentar cargas de 12 V con una fuente externa a través de relés.

---

# 🔄 Cambios respecto a la práctica con DS18B20

| Elemento                  | Versión anterior              | Esta práctica                    |
| ------------------------- | ----------------------------- | -------------------------------- |
| Sensor de temperatura     | DS18B20 (digital, OneWire)    | **LM35 (analógico)**             |
| Ajuste de Max y Min       | 3 botones                     | **2 potenciómetros**             |
| Librerías                 | OneWire, DallasTemperature    | Solo `LiquidCrystal_I2C`         |
| Resistencia pull-up 4.7 kΩ | Necesaria                    | No se usa                        |
| Filtro de lectura         | No                            | **Promedio móvil de 5 lecturas** |
| Lectura del sensor        | Cada 800 ms                   | Cada 100 ms                      |

---

# 🧰 Material requerido

## Hardware

| Componente                                  | Cantidad |
| ------------------------------------------- | -------: |
| ESP32 DevKit                                |        1 |
| Sensor de temperatura LM35                  |        1 |
| Potenciómetro 10 kΩ lineal                  |        2 |
| LCD 16×2 I2C                                |        1 |
| Módulo de relé de 1 canal (5 V)             |        3 |
| Celda Peltier 12 V (TEC1-12706 o similar)   |        1 |
| Disipador de aluminio                       |        2 |
| Ventilador 12 V para disipador              |        2 |
| Ventilador extractor 12 V                   |        1 |
| Foco incandescente                          |        1 |
| Portalámparas resistente al calor           |        1 |
| Fuente de 12 V CD (mínimo 8 A)              |        1 |
| Clavija con cable (127 V CA)                |        1 |
| Protoboard                                  |        1 |
| Cables Dupont y cable calibre grueso        |   Varios |
| Cable USB                                   |        1 |

---

# 🔌 Conexiones

## 🌡️ Sensor LM35

El LM35 entrega **10 mV por cada °C** en su pin de salida (Vout). Por ejemplo, 250 mV equivalen a 25 °C.

Visto de frente (con la cara plana hacia usted y las patas hacia abajo):

```text
      ┌─────┐
      │LM35 │
      └─┬─┬─┬┘
        │ │ │
       +Vs Vout GND
```

| LM35        | ESP32          |
| ----------- | -------------- |
| +Vs         | VIN (5 V)      |
| Vout        | GPIO 34        |
| GND         | GND            |

> ⚠️ **Nota:** el LM35 se alimenta con **5 V**. Con 3.3 V trabaja fuera de su rango recomendado y la medición no es confiable. Con 5 V su salida llega como máximo a 1.5 V (150 °C), que es un voltaje seguro para el ADC del ESP32. Opcionalmente, coloque un capacitor de 0.1 µF entre Vout y GND para reducir el ruido.

---

## 🎚️ Potenciómetros

Cada potenciómetro actúa como un divisor de voltaje: sus extremos van a 3.3 V y GND, y la terminal central (cursor) va al GPIO.

| Potenciómetro | Extremo 1 | Terminal central | Extremo 2 | Función                      |
| ------------- | --------- | ---------------- | --------- | ---------------------------- |
| POT MÁX       | 3.3 V     | GPIO 32          | GND       | Temperatura máxima (5–99 °C) |
| POT MÍN       | 3.3 V     | GPIO 33          | GND       | Temperatura mínima (0–94 °C) |

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

> ⚠️ **Importante:** alimente los potenciómetros con **3.3 V** y no con 5 V. Los pines del ESP32 soportan como máximo 3.3 V.

---

## ⚡ Módulos de relé

Los tres módulos se alimentan con **5 V (VIN) y GND** del ESP32 y se activan con nivel **LOW** (`LOW` = encendido, `HIGH` = apagado).

| Módulo de relé | Pin IN del ESP32 | Carga que controla                         |
| -------------- | ---------------- | ------------------------------------------ |
| Relé foco      | GPIO 26          | Foco (127 V CA)                            |
| Relé Peltier   | GPIO 27          | Celda Peltier + 2 ventiladores (12 V CD)   |
| Relé extractor | GPIO 13          | Extractor (12 V CD)                        |

### Cargas de 12 V (Peltier y extractor)

Los **COM de los relés de la Peltier y del extractor van unidos** al positivo (+) de la fuente de 12 V. Cada carga se conecta al **NO** de su relé, y su negativo regresa al negativo (−) de la fuente.

| Origen                                   | Destino                                     |
| ---------------------------------------- | ------------------------------------------- |
| Fuente 12 V (+)                          | COM del relé Peltier y COM del relé extractor |
| NO del relé Peltier                      | (+) de la Peltier y de sus 2 ventiladores   |
| NO del relé extractor                    | (+) del extractor                           |
| (−) de Peltier, ventiladores y extractor | Fuente 12 V (−)                             |

La celda Peltier y sus **dos ventiladores** (uno por disipador: lado frío y lado caliente) van **en paralelo**, por lo que se encienden y apagan juntos con el mismo relé.

### Foco (127 V CA)

| Origen                          | Destino                                   |
| ------------------------------- | ----------------------------------------- |
| Clavija, fase (cable negro)     | COM del relé del foco                     |
| NO del relé del foco            | Foco                                      |
| Clavija, neutro (cable blanco)  | Foco (directo, sin pasar por el relé)     |

> ⚠️ Revise la serigrafía de sus módulos, ya que el orden de las terminales (NO, COM, NC) puede variar entre fabricantes.

---

## 📟 LCD 16×2 I2C

El LCD utiliza el protocolo **I2C**.

En el ESP32 se utilizan los pines por defecto:

* **SDA → GPIO 21**
* **SCL → GPIO 22**

| LCD I2C | ESP32     |
| ------- | --------- |
| GND     | GND       |
| VCC     | VIN / 5 V |
| SDA     | GPIO 21   |
| SCL     | GPIO 22   |

> ⚠️ **Nota:** el código no define estos pines porque la librería `Wire` usa GPIO 21 (SDA) y GPIO 22 (SCL) por defecto en el ESP32.

---

## 📍 Resumen de pines

| GPIO | Función                      | Tipo              |
| ---- | ---------------------------- | ----------------- |
| 34   | Salida del LM35              | Entrada analógica |
| 32   | Potenciómetro de máximo      | Entrada analógica |
| 33   | Potenciómetro de mínimo      | Entrada analógica |
| 26   | Relé del foco                | Salida digital    |
| 27   | Relé de la Peltier           | Salida digital    |
| 13   | Relé del extractor           | Salida digital    |
| 21   | SDA (LCD)                    | I2C               |
| 22   | SCL (LCD)                    | I2C               |

---

# 🔗 Conexión completa

```text
                          ESP32
                  ┌─────────────────┐
                  │                 │
                  │        GPIO 34  ├──── Vout  LM35 (+Vs a VIN, GND a GND)
                  │                 │
                  │        GPIO 32  ├──── Cursor POT MÁX (extremos 3V3 y GND)
                  │        GPIO 33  ├──── Cursor POT MÍN (extremos 3V3 y GND)
                  │                 │
                  │        GPIO 26  ├──── IN  Relé FOCO
                  │        GPIO 27  ├──── IN  Relé PELTIER
                  │        GPIO 13  ├──── IN  Relé EXTRACTOR
                  │                 │
                  │        GPIO 21  ├──── SDA  (LCD)
                  │        GPIO 22  ├──── SCL  (LCD)
                  │                 │
                  │            3V3  ├──── Extremos de los 2 potenciómetros
                  │            VIN  ├──── VCC (LCD, LM35 y 3 relés)
                  │            GND  ├──── GND (todo)
                  └─────────────────┘

  Lado de potencia (los relés solo conmutan estas cargas):

  Clavija 127 V CA ──► [Relé FOCO] ──────────► FOCO

  Fuente 12 V (+) ──┬─► [Relé PELTIER]   ──► Peltier + 2 ventiladores ──┐
                    └─► [Relé EXTRACTOR] ──► Extractor ─────────────────┤
  Fuente 12 V (−) ◄───────────────────────────────────────────────────┘
```

---

# 📚 Librerías

## Arduino IDE

Instalar desde el **Administrador de Bibliotecas**:

* `LiquidCrystal_I2C`
* `Wire` (incluida por defecto)

## PlatformIO

En el archivo `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 9600

lib_deps =
    marcoschwartz/LiquidCrystal_I2C
```

> En PlatformIO agregue `#include <Arduino.h>` como primera línea de `src/main.cpp`.

---

# 💻 Código

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

---

# ⚙️ Funcionamiento del código

## Conversión del LM35

```text
Temperatura (°C) = Voltaje (mV) / 10
```

Ejemplo: si `analogReadMilliVolts()` entrega 250 mV, la temperatura es 25.0 °C.

## Filtro de promedio móvil

El ADC del ESP32 tiene ruido, por lo que el código promedia las **últimas 5 lecturas** (una cada 100 ms):

```text
Temperatura filtrada = (T0 + T1 + T2 + T3 + T4) / 5
```

En la primera lectura, las cinco variables se llenan con el mismo valor para no promediar ceros.

## Ajuste con potenciómetros

La función `map()` convierte la lectura del ADC (0–4095) a grados:

| Potenciómetro | Lectura ADC | Temperatura |
| ------------- | ----------- | ----------- |
| POT MÁX       | 0 – 4095    | 5 – 99 °C   |
| POT MÍN       | 0 – 4095    | 0 – 94 °C   |

Después, el código **protege el margen**: `tempMax` siempre será al menos 5 °C mayor que `tempMin`.

## Estados del sistema

Los relés se activan con `LOW`.

| Estado                        | Foco | Peltier + ventiladores | Extractor          |
| ----------------------------- | :--: | :--------------------: | :----------------: |
| Leyendo sensor (inicio)       | OFF  | OFF                    | sin cambio         |
| **Calentando** (`Calor`)      | ON   | OFF                    | OFF                |
| **Enfriando** (`Frio`)        | OFF  | ON                     | ON                 |
| Pausa entre cambios (1 s)     | OFF  | OFF                    | conserva el último |
| **Error de sensor**           | OFF  | OFF                    | ON                 |

## Control por histéresis

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

* Si `tempActual >= tempMax` mientras calienta → cambia a **Frío**.
* Si `tempActual <= tempMin` mientras enfría → cambia a **Calor**.
* Entre ambos valores se mantiene el modo actual, lo que evita encender y apagar los relés constantemente.

## Partes importantes del código

| Parte del código                     | Función                                                                   |
| ------------------------------------ | ------------------------------------------------------------------------- |
| `analogReadResolution(12)`           | Configura el ADC a 12 bits (0–4095)                                       |
| `analogReadMilliVolts(PIN_LM35)`     | Lee el voltaje en mV con la calibración del ESP32                         |
| `intervaloSensor = 100`              | Lee el sensor cada 100 ms                                                 |
| `milivoltios > 1500`                 | Si el voltaje supera 1.5 V (150 °C) se considera falla del sensor         |
| `margen = 5`                         | Diferencia mínima de 5 °C entre Max y Min                                 |
| `pausaCambio = 1000`                 | Pausa de 1 s con foco y Peltier apagados al cambiar de modo               |
| `porcentaje`                         | Avance de la temperatura entre Min y Max (0–100 %)                        |
| `intervaloLCD = 300`                 | Actualiza la pantalla cada 300 ms sin parpadeo                            |

---

# 🖥️ Resultado esperado

## LCD

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

Al iniciar:

```text
┌────────────────┐
│Max:60 Min:30   │
│Leyendo LM35    │
└────────────────┘
```

Si falla el sensor:

```text
┌────────────────┐
│Max:60 Min:30   │
│Error LM35      │
└────────────────┘
```

## Monitor Serial (9600 baudios)

```text
LM35: 25.3 C | Filtrada: 25.1 C | Max: 60 | Min: 30
LM35: 25.4 C | Filtrada: 25.2 C | Max: 60 | Min: 30
LM35: 25.4 C | Filtrada: 25.3 C | Max: 60 | Min: 30
```

---

# 🛠️ Procedimiento

1. Arme primero el lado de baja tensión: ESP32, LM35, potenciómetros, LCD y entradas de los relés.
2. Conecte los **12 V**: fuente a los COM de Peltier y extractor, cargas a los NO y negativos a la fuente.
3. Conecte el **foco** con la clavija **desconectada**.
4. Coloque el LM35 dentro del horno, sin que toque el foco ni la Peltier directamente.
5. Cargue el código al ESP32.
6. Conecte el ESP32 por USB y verifique que la LCD muestre la temperatura.
7. Gire los potenciómetros y confirme que Max y Min cambien en la pantalla.
8. Conecte la fuente de 12 V y por último la clavija del foco.
9. Observe el cambio entre `Calor` y `Frio` al llegar a los límites.

---

# 🔎 Dirección I2C del LCD

La dirección más común de los módulos LCD I2C es:

```cpp
0x27
```

Sin embargo, algunos módulos pueden utilizar:

```cpp
0x3F
```

Si el LCD está correctamente conectado pero no muestra información, pruebe cambiar:

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

por:

```cpp
LiquidCrystal_I2C lcd(0x3F, 16, 2);
```

---

# 🔍 Escáner I2C

Para identificar automáticamente la dirección del LCD se puede utilizar el siguiente programa:

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

Ejemplo de resultado:

```text
Escaner I2C

Dispositivo encontrado en 0x27
```

---

# ⚠️ Notas importantes

| Tema                    | Descripción                                                                                                                                      |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| Lógica de los relés     | El código está hecho para módulos que se activan con **LOW**. Si los suyos se activan con HIGH, invierta los niveles.                            |
| Alimentación del LM35   | Debe ir a **5 V (VIN)**. Con 3.3 V la medición no es confiable.                                                                                  |
| Alimentación de los pots | Extremos a **3.3 V** y GND. Nunca a 5 V, porque dañaría el pin del ESP32.                                                                       |
| Pines analógicos        | GPIO 32, 33 y 34 pertenecen al ADC1, que funciona sin problemas aunque se use WiFi.                                                              |
| Precisión del ADC       | El ADC del ESP32 no es perfectamente lineal. Compare con un termómetro y ajuste la lectura si es necesario.                                     |
| Cable del sensor        | Un cable largo capta ruido. Use cable corto o blindado, y el capacitor de 0.1 µF recomendado.                                                    |
| Corriente de la fuente  | Una Peltier TEC1-12706 puede consumir hasta unos 6 A, más 3 ventiladores. Use una fuente de **12 V de 8 A o más**.                               |
| Contactos del relé      | Verifique que los contactos del módulo soporten la corriente de la Peltier (los SRD-05VDC son de 10 A).                                          |
| Disipación              | El lado caliente de la Peltier **siempre** debe tener disipador y ventilador. Si falla, la celda se sobrecalienta y se daña rápidamente.         |
| Polaridad de la Peltier | Si el lado que calienta queda hacia el interior del horno, invierta los cables de la celda (rojo = +, negro = −).                                |
| Temperatura del sensor  | El LM35 en encapsulado TO-92 soporta aproximadamente hasta 100–150 °C según el modelo. No configure `tempMax` por encima de lo que soporta.      |
| Aislamiento             | El foco y su portalámparas deben soportar el calor del horno; mantenga los cables de 127 V lejos de las partes calientes.                        |
| Seguridad               | Trabaje siempre desconectado y deje las conexiones de 127 V dentro de una caja cerrada.                                                          |

---

# ⚠️ Problemas frecuentes

| Problema                                   | Posible solución                                                        |
| ------------------------------------------ | ----------------------------------------------------------------------- |
| LCD no muestra nada                        | Verificar alimentación y conexiones SDA/SCL                             |
| LCD iluminado pero sin texto               | Revisar dirección I2C (`0x27` o `0x3F`)                                 |
| LCD muestra cuadros                        | Ajustar el contraste con el potenciómetro del módulo                    |
| Aparece `Error LM35`                       | Revisar conexiones del LM35; el voltaje de salida pasó de 1.5 V         |
| Temperatura muy alta o muy baja            | Revisar que +Vs y GND del LM35 no estén invertidos                      |
| La temperatura fluctúa mucho               | Agregar capacitor de 0.1 µF, acortar el cable o aumentar el filtro      |
| Max o Min cambian solos ±1                 | Ruido del ADC; agregar un promedio a la lectura de los potenciómetros   |
| Al girar un pot no cambia el valor         | Revisar que el cursor (centro) vaya al GPIO y los extremos a 3V3 y GND  |
| El valor se mueve al revés                 | Invierta los extremos del potenciómetro                                 |
| Los relés funcionan al revés               | Su módulo se activa con HIGH; invierta `HIGH` y `LOW` en el código      |
| El foco no enciende                        | Revisar COM/NO del relé y que el neutro llegue directo al foco          |
| La Peltier no enfría                       | Revisar la fuente de 12 V, el COM unido y la polaridad                  |
| Se reinicia el ESP32 al activar un relé    | Fuente insuficiente o GND no común; revisar alimentación                |
| El extractor se queda encendido            | Está en modo Frío o hay error de sensor (se enciende por seguridad)     |

---

# 🧠 Conceptos aprendidos

### Sensor analógico → ADC

```text
LM35 ──► Voltaje (10 mV/°C) ──► ADC 12 bits ──► mV ──► °C
```

### Filtrado de señales

El promedio móvil suaviza el ruido de la lectura: mientras más muestras se promedian, más estable es la medición, pero más lenta es su respuesta.

### Divisor de voltaje con potenciómetro

```text
3.3 V ──┐
        │
      [Pot]──── Cursor ──► ADC (0 a 3.3 V → 0 a 4095)
        │
GND ────┘
```

### Control por histéresis (ON/OFF)

```text
SENSOR ──► ESP32 ──► ¿T ≥ Max? ──► Enfriar
                 └─► ¿T ≤ Min? ──► Calentar
```

### Control de cargas con relés

```text
ESP32 (3.3 V, GPIO) ──► Módulo de relé (aislado) ──► Carga de 12 V CD o 127 V CA
```

### Lectura no bloqueante

El programa no usa `delay()`. Utiliza `millis()` para leer el sensor, refrescar la LCD y manejar la pausa de seguridad al mismo tiempo, sin que una tarea detenga a las demás.

### Efecto Peltier

```text
Corriente ──► Peltier ──► Un lado se enfría ❄️  / El otro lado se calienta 🔥
```

El lado caliente necesita disipador y ventilador para evacuar el calor.

Por lo tanto:

```text
     VARIABLE FÍSICA
      Temperatura
           │
           ▼
         LM35
           │
      Voltaje analógico
           ▼
         ESP32 ◄──── Potenciómetros (Max / Min)
           │
     ┌─────┼───────────┐
     │     │           │
    I2C  Relé foco   Relés Peltier / Extractor
     │     │           │
     ▼     ▼           ▼
    LCD   Calor        Frío
```

---

# 📊 Flujo del programa

```text
        INICIO
           │
           ▼
  Configurar pines y relés (OFF)
           │
           ▼
   Configurar ADC y LCD
           │
           ▼
  ┌──► Leer potenciómetros → Max y Min
  │        │
  │        ▼
  │   Aplicar margen mínimo de 5 °C
  │        │
  │        ▼
  │   ¿Pasaron 100 ms?
  │        │ SÍ
  │        ▼
  │   Leer LM35 (mV)
  │        │
  │    ┌───┴─────────────┐
  │    │                 │
  │  ¿Voltaje          Voltaje
  │  > 1500 mV?         válido
  │    │ SÍ              │
  │    ▼                 ▼
  │  ERROR:         Filtro de promedio (5)
  │  foco y Peltier      │
  │  OFF, extractor      ▼
  │  ON             Calcular %
  │    │                 │
  │    │            ¿T ≥ Max → FRÍO?
  │    │            ¿T ≤ Min → CALOR?
  │    └────────┬────────┘
  │             ▼
  │     ¿En pausa (1 s)?
  │      SÍ → Foco y Peltier OFF
  │      NO → Aplicar estado (Calor / Frío)
  │             │
  │             ▼
  │      Actualizar LCD (300 ms)
  │             │
  └─────────────┘
```

---

# 📝 Actividad para el estudiante

Modifique el programa para implementar las siguientes funciones:

### Nivel 1 — Básico

1. Cambiar el margen mínimo entre Max y Min (`margen`).
2. Cambiar el intervalo de lectura del sensor (`intervaloSensor`).
3. Modificar el filtro para promediar 10 lecturas en lugar de 5.

### Nivel 2 — Intermedio

4. Calibrar el LM35 comparándolo con un termómetro y agregar un valor de corrección (offset).
5. Agregar un filtro de promedio a la lectura de los potenciómetros para que Max y Min no fluctúen.
6. Agregar un LED que indique el modo (rojo = Calor, azul = Frío) y un buzzer que suene en caso de error.

### Nivel 3 — Integración

7. Guardar `tempMax` y `tempMin` en memoria (`Preferences`) para recuperarlos al reiniciar.
8. Agregar un temporizador que apague el sistema después de un tiempo configurable.
9. Reemplazar la histéresis por un control **PID** con salida PWM para la Peltier (con un módulo de potencia adecuado en lugar del relé).
10. Graficar la temperatura medida y la filtrada en el Serial Plotter.

---

# 📈 Extensión: Sistema de control avanzado

Como extensión de la práctica, el sistema puede evolucionar de un control **ON/OFF** a un control **proporcional**:

```text
              LM35
                 │
                 ▼
            ESP32 (PID)
                 │
        ┌────────┴────────┐
        │                 │
        ▼                 ▼
      LCD            Salida PWM
                          │
                   ┌──────┴──────┐
                   ▼             ▼
                Calentador     Peltier
```

Esto permite posteriormente implementar aplicaciones de:

* 🌡️ Incubadoras
* 🔥 Hornos de secado
* ❄️ Mini refrigeradores
* 🧪 Cámaras de temperatura controlada
* 🏠 Automatización del hogar

---

# 📌 Competencias desarrolladas

* Programación de microcontroladores.
* Adquisición de variables físicas con sensores analógicos.
* Uso del convertidor analógico-digital (ADC).
* Filtrado digital de señales.
* Comunicación I2C.
* Control de actuadores mediante relés.
* Control por histéresis.
* Programación no bloqueante con `millis()`.
* Manejo de errores y seguridad del sistema.
* Manejo de displays.
* Programación C/C++.
* Seguridad en instalaciones eléctricas.
* Integración sensor–controlador–actuador–interfaz.
