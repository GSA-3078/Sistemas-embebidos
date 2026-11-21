# 🔥 Práctica: Horno con control de temperatura (Foco + Peltier + Extractor) con ESP32, DS18B20 y LCD 16×2 I2C

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue)
![Arduino](https://img.shields.io/badge/Arduino-C%2FC%2B%2B-00979D)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange)
![Relay](https://img.shields.io/badge/M%C3%B3dulos-Rel%C3%A9-red)

## 📋 Descripción

En esta práctica se desarrolla un sistema embebido basado en **ESP32** que mantiene la temperatura de un horno dentro de un rango configurable. La temperatura se mide con un sensor digital **DS18B20** y se muestra en una pantalla **LCD 16×2 con interfaz I2C**.

El sistema alterna entre dos modos:

* 🔥 **Calentando:** se enciende un **foco** (127 V CA) para subir la temperatura.
* ❄️ **Enfriando:** se enciende una **celda Peltier** con sus dos ventiladores y un **extractor** para bajar la temperatura.

Los actuadores se manejan con **módulos de relé** (sin transistores) y la temperatura máxima y mínima se ajustan con **3 botones**.

La práctica integra:

* 🌡️ Adquisición de temperatura.
* 🔌 Comunicación digital **OneWire**.
* 📟 Comunicación **I2C**.
* 🔘 Lectura de botones con antirrebote.
* ⚡ Control de cargas de CA y CD mediante relés.
* 🔁 Control de temperatura por histéresis (ON/OFF).
* 🖥️ Visualización de datos en LCD.
* ⚙️ Programación no bloqueante de ESP32 utilizando C/C++.

> ⚠️ **ADVERTENCIA DE SEGURIDAD:** el foco trabaja con **127 V de corriente alterna** y el sistema genera **calor**. Arme y modifique el circuito siempre **desconectado**, utilice cable y portalámparas que soporten temperatura, y deje las conexiones de 127 V dentro de una caja cerrada.

---

## 🎯 Objetivo

Implementar un sistema de control de temperatura por histéresis utilizando un ESP32, un sensor DS18B20, tres módulos de relé, una celda Peltier, un foco y una pantalla LCD 16×2 con interfaz I2C.

Al finalizar la práctica, el estudiante será capaz de:

1. Conectar un sensor DS18B20 al ESP32 y leerlo sin bloquear el programa.
2. Controlar cargas de CA y CD mediante módulos de relé.
3. Implementar un control de temperatura por histéresis con un límite máximo y uno mínimo.
4. Configurar los límites mediante botones con antirrebote.
5. Mostrar información en el LCD.
6. Detectar y manejar fallas del sensor.
7. Alimentar cargas de 12 V con una fuente externa a través de relés.
8. Identificar errores de conexión o comunicación con los dispositivos.

---

# 🧰 Material requerido

## Hardware

| Componente                                  | Cantidad |
| ------------------------------------------- | -------: |
| ESP32 DevKit                                |        1 |
| Sensor DS18B20 (sonda)                      |        1 |
| LCD 16×2 I2C                                |        1 |
| Resistencia 4.7 kΩ                          |        1 |
| Módulo de relé de 1 canal (5 V)             |        3 |
| Celda Peltier 12 V (TEC1-12706 o similar)   |        1 |
| Disipador de aluminio                       |        2 |
| Ventilador 12 V para disipador              |        2 |
| Ventilador extractor 12 V                   |        1 |
| Foco incandescente                          |        1 |
| Portalámparas resistente al calor           |        1 |
| Fuente de 12 V CD (mínimo 8 A)              |        1 |
| Clavija con cable (127 V CA)                |        1 |
| Botones (push button)                       |        3 |
| Protoboard                                  |        1 |
| Cables Dupont y cable calibre grueso        |   Varios |
| Cable USB                                   |        1 |

---

# 🔌 Diagrama de conexión

![Diagrama de conexión](diagrama_horno_peltier_esp32.png)

---

## 🌡️ DS18B20

El DS18B20 utiliza el protocolo **OneWire**, por lo que solamente requiere una línea de datos para comunicarse con el ESP32.

| DS18B20            | ESP32  |
| ------------------ | ------ |
| VCC (rojo)         | 3.3 V  |
| DATA (amarillo)    | GPIO 4 |
| GND (negro)        | GND    |

Se debe colocar una resistencia de **4.7 kΩ entre DATA y VCC (3.3 V)**.

```text
                  3.3 V
                    │
                  4.7 kΩ
                    │
                    ├──────── DATA
                    │          │
ESP32 GPIO 4 ───────┘       DS18B20
                               │
ESP32 3.3 V ───────────────── VCC
ESP32 GND ─────────────────── GND
```

---

## 🔘 Botones

Los botones se configuran con `INPUT_PULLUP`, por lo que un lado va al GPIO y el otro a GND. No requieren resistencias externas.

| Botón | ESP32   | Función                                   |
| ----- | ------- | ----------------------------------------- |
| MODO  | GPIO 32 | Cambia entre editar el máximo y el mínimo |
| SUBIR | GPIO 33 | +1 al valor que se está editando          |
| BAJAR | GPIO 25 | −1 al valor que se está editando          |

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

| Origen                                  | Destino                                |
| --------------------------------------- | -------------------------------------- |
| Fuente 12 V (+)                         | COM del relé Peltier y COM del extractor |
| NO del relé Peltier                     | (+) de la Peltier y de sus 2 ventiladores |
| NO del relé extractor                   | (+) del extractor                      |
| (−) de Peltier, ventiladores y extractor | Fuente 12 V (−)                       |

La celda Peltier y sus **dos ventiladores** (uno por disipador: lado frío y lado caliente) van **en paralelo**, por lo que se encienden y apagan juntos con el mismo relé.

### Foco (127 V CA)

| Origen                          | Destino                         |
| ------------------------------- | ------------------------------- |
| Clavija, fase (cable negro)     | COM del relé del foco           |
| NO del relé del foco            | Foco                            |
| Clavija, neutro (cable blanco)  | Foco (directo, sin pasar por el relé) |

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

# 🔗 Conexión completa

```text
                          ESP32
                  ┌─────────────────┐
                  │                 │
                  │            3.3V ├───────┐
                  │         GPIO 4  ├───────┼──── DATA
                  │                 │     [4.7kΩ]       DS18B20
                  │            GND  ├───────┼──── GND
                  │                 │
                  │        GPIO 32  ├──── MODO  ──── GND
                  │        GPIO 33  ├──── SUBIR ──── GND
                  │        GPIO 25  ├──── BAJAR ──── GND
                  │                 │
                  │        GPIO 26  ├──── IN  Relé FOCO
                  │        GPIO 27  ├──── IN  Relé PELTIER
                  │        GPIO 13  ├──── IN  Relé EXTRACTOR
                  │                 │
                  │        GPIO 21  ├──── SDA  (LCD)
                  │        GPIO 22  ├──── SCL  (LCD)
                  │                 │
                  │            VIN  ├──── VCC (LCD y 3 relés)
                  │            GND  ├──── GND (LCD y 3 relés)
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

* `OneWire`
* `DallasTemperature`
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
    paulstoffregen/OneWire
    milesburton/DallasTemperature
    marcoschwartz/LiquidCrystal_I2C
```

---

# 💻 Código

Archivo:

```text
src/main.cpp
```

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int SENSOR_PIN = 4;
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensor(&oneWire);

int BTN_MODO = 32;
int BTN_SUBIR = 33;
int BTN_BAJAR = 25;

int rele_foco = 26;
int rele_peltier = 27;
int rele_extractor = 13;

int tempMax = 60;
int tempMin = 30;
float tempActual = 0.0;
float porcentaje = 0.0;

int modoEdicion = 1;
int calentando = 1;

int margen = 5;

int anteriorModo = HIGH;
int anteriorSubir = HIGH;
int anteriorBajar = HIGH;

unsigned long tiempoModo = 0;
unsigned long tiempoSubir = 0;
unsigned long tiempoBajar = 0;
unsigned long antirrebote = 200;

unsigned long tiempoSensor = 0;
unsigned long intervaloSensor = 800;
int esperandoSensor = 0;

int erroresSensor = 0;
int maxErrores = 3;
int sensorOK = 0;

int inicioListo = 0;
int errorSensor = 0;

unsigned long tiempoCambio = 0;
unsigned long pausaCambio = 1000;
int enPausa = 0;

unsigned long tiempoLCD = 0;
unsigned long intervaloLCD = 300;

void setup() {
  pinMode(BTN_MODO, INPUT_PULLUP);
  pinMode(BTN_SUBIR, INPUT_PULLUP);
  pinMode(BTN_BAJAR, INPUT_PULLUP);

  pinMode(rele_foco, OUTPUT);
  pinMode(rele_peltier, OUTPUT);
  pinMode(rele_extractor, OUTPUT);

  digitalWrite(rele_foco, HIGH);
  digitalWrite(rele_peltier, HIGH);
  digitalWrite(rele_extractor, HIGH);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(9600);

  sensor.begin();
  sensor.setWaitForConversion(false);

  sensor.requestTemperatures();
  tiempoSensor = millis();
  esperandoSensor = 1;
}

void loop() {

  int lecturaModo = digitalRead(BTN_MODO);

  if (lecturaModo == LOW && anteriorModo == HIGH &&
      millis() - tiempoModo >= antirrebote) {

    if (modoEdicion == 1) {
      modoEdicion = 0;
    } else {
      modoEdicion = 1;
    }

    tiempoModo = millis();
  }

  anteriorModo = lecturaModo;

  int lecturaSubir = digitalRead(BTN_SUBIR);

  if (lecturaSubir == LOW && anteriorSubir == HIGH &&
      millis() - tiempoSubir >= antirrebote) {

    if (modoEdicion == 1) {
      tempMax = tempMax + 1;

      if (tempMax > 99) {
        tempMax = 99;
      }

      if (tempMax < tempMin + margen) {
        tempMax = tempMin + margen;
      }
    } else {
      tempMin = tempMin + 1;

      if (tempMin > tempMax - margen) {
        tempMin = tempMax - margen;
      }
    }

    tiempoSubir = millis();
  }

  anteriorSubir = lecturaSubir;

  int lecturaBajar = digitalRead(BTN_BAJAR);

  if (lecturaBajar == LOW && anteriorBajar == HIGH &&
      millis() - tiempoBajar >= antirrebote) {

    if (modoEdicion == 1) {
      tempMax = tempMax - 1;

      if (tempMax < tempMin + margen) {
        tempMax = tempMin + margen;
      }
    } else {
      tempMin = tempMin - 1;

      if (tempMin < 0) {
        tempMin = 0;
      }
    }

    tiempoBajar = millis();
  }

  anteriorBajar = lecturaBajar;

  if (esperandoSensor == 1 &&
      millis() - tiempoSensor >= intervaloSensor) {

    float lectura = sensor.getTempCByIndex(0);

    if (lectura == DEVICE_DISCONNECTED_C || lectura == 85.0 || lectura < -55.0 || lectura > 125.0) {

      erroresSensor = erroresSensor + 1;

      Serial.println("Error leyendo el sensor");

      if (erroresSensor >= maxErrores) {
        errorSensor = 1;
        sensorOK = 0;
        inicioListo = 0;
        enPausa = 0;

        digitalWrite(rele_foco, HIGH);
        digitalWrite(rele_peltier, HIGH);

        digitalWrite(rele_extractor, LOW);
      }

    }
    else {

      erroresSensor = 0;
      sensorOK = 1;
      tempActual = lectura;

      if (errorSensor == 1) {
        errorSensor = 0;
        inicioListo = 0;
      }

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

      porcentaje = ((tempActual - tempMin) /
                    (tempMax - tempMin)) * 100.0;

      if (porcentaje > 100.0) {
        porcentaje = 100.0;
      }

      if (porcentaje < 0.0) {
        porcentaje = 0.0;
      }

      if (inicioListo == 1 && errorSensor == 0) {

        if (calentando == 1 && tempActual >= tempMax) {

          calentando = 0;
          enPausa = 1;
          tiempoCambio = millis();

          digitalWrite(rele_foco, HIGH);
          digitalWrite(rele_peltier, HIGH);
          digitalWrite(rele_extractor, LOW);
        }

        else if (calentando == 0 && tempActual <= tempMin) {

          calentando = 1;
          enPausa = 1;
          tiempoCambio = millis();

          digitalWrite(rele_foco, HIGH);
          digitalWrite(rele_peltier, HIGH);
          digitalWrite(rele_extractor, HIGH);
        }
      }
    }

    sensor.requestTemperatures();
    tiempoSensor = millis();
    esperandoSensor = 1;
  }

  if (enPausa == 1 && millis() - tiempoCambio >= pausaCambio) {

    enPausa = 0;
  }

  if (errorSensor == 1 || sensorOK == 0 || inicioListo == 0) {

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
  
  if (millis() - tiempoLCD >= intervaloLCD) {

    tiempoLCD = millis();

    lcd.setCursor(0, 0);

    if (modoEdicion == 1) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }

    lcd.print("Max:");
    lcd.print(tempMax);
    lcd.print(" ");

    if (modoEdicion == 0) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }

    lcd.print("Min:");
    lcd.print(tempMin);
    lcd.print(" ");

    lcd.setCursor(0, 1);

    if (errorSensor == 1) {
      lcd.print("Error sensor    ");
    }

    else if (sensorOK == 0 || inicioListo == 0) {
      lcd.print("Leyendo sensor  ");
    }

    else {

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

## Estados del sistema

Los relés se activan con `LOW`.

| Estado                        | Foco | Peltier + ventiladores | Extractor |
| ----------------------------- | :--: | :--------------------: | :-------: |
| Leyendo sensor (inicio)       | OFF  | OFF                    | sin cambio |
| **Calentando** (`Calor`)      | ON   | OFF                    | OFF       |
| **Enfriando** (`Frio`)        | OFF  | ON                     | ON        |
| Pausa entre cambios (1 s)     | OFF  | OFF                    | conserva el último |
| **Error de sensor**           | OFF  | OFF                    | ON        |

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

| Parte del código                   | Función                                                                 |
| ---------------------------------- | ----------------------------------------------------------------------- |
| `sensor.setWaitForConversion(false)` | Lectura no bloqueante del DS18B20                                     |
| `intervaloSensor = 800`            | Lee la temperatura cada 800 ms                                          |
| `antirrebote = 200`                | Evita que un solo pulso se cuente varias veces                          |
| `margen = 5`                       | Diferencia mínima de 5 °C entre Max y Min                               |
| `pausaCambio = 1000`               | Pausa de 1 s con foco y Peltier apagados al cambiar de modo             |
| `maxErrores = 3`                   | Tras 3 lecturas fallidas seguidas se declara error de sensor            |
| `lectura == 85.0`                  | Valor que entrega el DS18B20 cuando no completó la conversión           |
| `porcentaje`                       | Avance de la temperatura entre Min y Max (0–100 %)                      |
| `intervaloLCD = 300`               | Actualiza la pantalla cada 300 ms sin parpadeo                          |

---

# 🎛️ Uso de los botones

| Botón | Acción                                                                  |
| ----- | ----------------------------------------------------------------------- |
| MODO  | Alterna entre editar **Max** y **Min**. El símbolo `>` indica cuál se edita |
| SUBIR | Aumenta en 1 °C el valor marcado                                        |
| BAJAR | Disminuye en 1 °C el valor marcado                                      |

Límites de configuración:

* `tempMax`: máximo **99 °C**, y siempre al menos 5 °C mayor que `tempMin`.
* `tempMin`: mínimo **0 °C**, y siempre al menos 5 °C menor que `tempMax`.

---

# 🖥️ Resultado esperado

## LCD

Editando el máximo, calentando:

```text
┌────────────────┐
│>Max:60  Min:30 │
│45.0C 50% Calor │
└────────────────┘
```

Editando el mínimo, enfriando:

```text
┌────────────────┐
│ Max:60 >Min:30 │
│58.5C 95% Frio  │
└────────────────┘
```

Al iniciar:

```text
┌────────────────┐
│>Max:60  Min:30 │
│Leyendo sensor  │
└────────────────┘
```

Si falla el sensor:

```text
┌────────────────┐
│>Max:60  Min:30 │
│Error sensor    │
└────────────────┘
```

## Monitor Serial (9600 baudios)

El código solo escribe en el Monitor Serial cuando hay un error:

```text
Error leyendo el sensor
Error leyendo el sensor
Error leyendo el sensor
```

---

# 🛠️ Procedimiento

1. Arme primero el lado de baja tensión: ESP32, botones, LCD, DS18B20 y entradas de los relés.
2. Conecte los **12 V**: fuente a los COM de Peltier y extractor, cargas a los NO y negativos a la fuente.
3. Conecte el **foco** con la clavija **desconectada**.
4. Coloque el sensor dentro del horno, sin que toque el foco ni la Peltier directamente.
5. Cargue el código al ESP32.
6. Conecte el ESP32 por USB y verifique que la LCD muestre la temperatura.
7. Conecte la fuente de 12 V y por último la clavija del foco.
8. Ajuste Max y Min con los botones y observe el cambio entre `Calor` y `Frio`.

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
| Corriente de la fuente  | Una Peltier TEC1-12706 puede consumir hasta unos 6 A, más 3 ventiladores. Use una fuente de **12 V de 8 A o más**.                               |
| Contactos del relé      | Verifique que los contactos del módulo soporten la corriente de la Peltier (los SRD-05VDC son de 10 A).                                          |
| Disipación              | El lado caliente de la Peltier **siempre** debe tener disipador y ventilador. Si falla, la celda se sobrecalienta y se daña rápidamente.         |
| Polaridad de la Peltier | Si el lado que calienta queda hacia el interior del horno, invierta los cables de la celda (rojo = +, negro = −).                                |
| Temperatura del sensor  | La sonda DS18B20 y su cable tienen un límite de temperatura (normalmente cerca de 100–125 °C). No configure `tempMax` por encima de lo que soporta. |
| Aislamiento             | El foco y su portalámparas deben soportar el calor del horno; mantenga los cables de 127 V lejos de las partes calientes.                        |
| Seguridad               | Trabaje siempre desconectado y deje las conexiones de 127 V dentro de una caja cerrada.                                                          |

---

# ⚠️ Problemas frecuentes

| Problema                                   | Posible solución                                                       |
| ------------------------------------------ | ---------------------------------------------------------------------- |
| LCD no muestra nada                        | Verificar alimentación y conexiones SDA/SCL                            |
| LCD iluminado pero sin texto               | Revisar dirección I2C (`0x27` o `0x3F`)                                |
| LCD muestra cuadros                        | Ajustar el contraste con el potenciómetro del módulo                   |
| Aparece `Error sensor`                     | Revisar DATA del DS18B20 y la resistencia de 4.7 kΩ                    |
| Temperatura `-127 °C` o `85 °C`            | Sensor no detectado o conversión incompleta                            |
| Los relés funcionan al revés               | Su módulo se activa con HIGH; invierta `HIGH` y `LOW` en el código     |
| El foco no enciende                        | Revisar COM/NO del relé y que el neutro llegue directo al foco         |
| La Peltier no enfría                       | Revisar la fuente de 12 V, el COM unido y la polaridad                 |
| Se reinicia el ESP32 al activar un relé    | Fuente insuficiente o GND no común; revisar alimentación               |
| Los botones no responden                   | Verificar que un lado vaya al GPIO y el otro a GND                     |
| No puedo subir/bajar más el valor          | Se alcanzó el límite (99 °C, 0 °C o margen de 5 °C)                    |
| El extractor se queda encendido            | Está en modo Frío o hay error de sensor (se enciende por seguridad)    |

---

# 🧠 Conceptos aprendidos

### Control por histéresis (ON/OFF)

```text
SENSOR ──► ESP32 ──► ¿T ≥ Max? ──► Enfriar
                 └─► ¿T ≤ Min? ──► Calentar
```

### Control de cargas con relés

```text
ESP32 (3.3 V, GPIO) ──► Módulo de relé (aislado) ──► Carga de 12 V CD o 127 V CA
```

El módulo de relé permite que una señal de baja potencia del ESP32 controle cargas de mayor voltaje y corriente, manteniendo aislado el circuito de control.

### Lectura no bloqueante

El programa no usa `delay()`. Utiliza `millis()` para leer el sensor, refrescar la LCD y detectar botones al mismo tiempo, sin que una tarea detenga a las demás.

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
        DS18B20
           │
        OneWire
           ▼
         ESP32 ◄──── Botones (Max / Min)
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
   Inicializar LCD y DS18B20
           │
           ▼
  ┌──► Leer botones (con antirrebote)
  │        │
  │        ▼
  │   ¿Pasaron 800 ms?
  │        │ SÍ
  │        ▼
  │   Leer temperatura
  │        │
  │    ┌───┴─────────────┐
  │    │                 │
  │  ¿Lectura          Lectura
  │   inválida?         válida
  │    │                 │
  │    ▼                 ▼
  │  ¿3 errores?     Calcular %
  │    │ SÍ              │
  │    ▼                 ▼
  │  ERROR:         ¿T ≥ Max → FRÍO?
  │  foco y Peltier ¿T ≤ Min → CALOR?
  │  OFF, extractor      │
  │  ON                  │
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

1. Cambiar la temperatura máxima y mínima iniciales (`tempMax`, `tempMin`).
2. Cambiar el margen mínimo entre Max y Min (`margen`).
3. Mostrar la temperatura actual también en el Monitor Serial.

### Nivel 2 — Intermedio

4. Cambiar la pausa entre modos (`pausaCambio`) y analizar su efecto.
5. Agregar un LED que indique el modo (rojo = Calor, azul = Frío).
6. Agregar un buzzer que suene cuando ocurra un error de sensor.

### Nivel 3 — Integración

7. Guardar `tempMax` y `tempMin` en memoria (`Preferences`) para recuperarlos al reiniciar.
8. Agregar un temporizador que apague el sistema después de un tiempo configurable.
9. Reemplazar la histéresis por un control **PID** con salida PWM para la Peltier (con un módulo de potencia adecuado en lugar del relé).
10. Registrar la temperatura en el tiempo y graficarla en el Serial Plotter.

---

# 📈 Extensión: Sistema de control avanzado

Como extensión de la práctica, el sistema puede evolucionar de un control **ON/OFF** a un control **proporcional**:

```text
             DS18B20
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
* Adquisición de variables físicas.
* Comunicación OneWire.
* Comunicación I2C.
* Control de actuadores mediante relés.
* Control por histéresis.
* Programación no bloqueante con `millis()`.
* Manejo de errores y seguridad del sistema.
* Manejo de displays.
* Programación C/C++.
* Seguridad en instalaciones eléctricas.
* Integración sensor–controlador–actuador–interfaz.
