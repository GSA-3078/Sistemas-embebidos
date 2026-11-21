# 💡 Práctica: Control de intensidad de un foco con Dimmer MY-989, Servo y LCD 16×2 I2C

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue)
![Arduino](https://img.shields.io/badge/Arduino-C%2FC%2B%2B-00979D)
![PlatformIO](https://img.shields.io/badge/PlatformIO-IDE-orange)
![Servo](https://img.shields.io/badge/Servo-0%C2%B0%20a%20180%C2%B0-green)

## 📋 Descripción

En esta práctica se desarrolla un sistema embebido basado en **ESP32** capaz de regular la intensidad luminosa de un foco mediante un **servomotor** acoplado a la perilla de un **dimmer MY-989 (MY-9892)**.

Con dos botones se controla el ángulo del servo de 5° en 5°. El servo gira la perilla del dimmer, que modifica el voltaje que llega al foco. El ángulo y el porcentaje de luz se muestran en una pantalla **LCD 16×2 con interfaz I2C** y en el Monitor Serial.

La práctica integra:

* 🎚️ Control de posición de un servomotor.
* 🔘 Lectura de botones con `INPUT_PULLUP`.
* 📟 Comunicación **I2C**.
* 🖥️ Visualización de datos en LCD.
* 🖥️ Monitor Serial.
* 💡 Regulación de potencia en corriente alterna mediante un dimmer.
* ⚙️ Programación de ESP32 utilizando C/C++.

> ⚠️ **ADVERTENCIA DE SEGURIDAD:** el foco y el dimmer trabajan con **127 V de corriente alterna**, que pueden causar descargas peligrosas. Arme y modifique el circuito siempre **desconectado**, utilice cable adecuado y deje el dimmer y sus conexiones dentro de una caja cerrada.

---

## 🎯 Objetivo

Implementar un sistema de control de intensidad luminosa utilizando un ESP32, un servomotor, un dimmer MY-989 y una pantalla LCD 16×2 con interfaz I2C.

Al finalizar la práctica, el estudiante será capaz de:

1. Conectar y controlar un servomotor con el ESP32.
2. Leer botones utilizando resistencias pull-up internas.
3. Convertir un ángulo a un porcentaje mediante la función `map()`.
4. Configurar una pantalla LCD mediante I2C.
5. Mostrar información en el LCD y en el Monitor Serial.
6. Conectar un dimmer de corriente alterna a un foco de forma segura.
7. Identificar errores de conexión o comunicación con los dispositivos.

---

# 🧰 Material requerido

## Hardware

| Componente                       | Cantidad |
| -------------------------------- | -------: |
| ESP32 DevKit                     |        1 |
| Servomotor (SG90 / MG90S)        |        1 |
| Dimmer MY-989 (MY-9892) 2000 W   |        1 |
| Acople servo-perilla             |        1 |
| LCD 16×2 I2C                     |        1 |
| Botones (push button)            |        2 |
| Foco incandescente               |        1 |
| Portalámparas                    |        1 |
| Clavija con cable (127 V CA)     |        1 |
| Caja para el dimmer              |        1 |
| Protoboard                       |        1 |
| Cables Dupont                    |   Varios |
| Cable USB                        |        1 |

---

# 🔌 Diagrama de conexión

![Diagrama de conexión](diagrama_dimmer_servo_esp32.png)

---

## 🔘 Botones

Los botones se configuran con `INPUT_PULLUP`, por lo que un lado va al GPIO y el otro a GND. No requieren resistencias externas.

| Botón  | ESP32   | Función        |
| ------ | ------- | -------------- |
| SUBIR  | GPIO 32 | +5 grados      |
| BAJAR  | GPIO 33 | −5 grados      |

---

## 🎚️ Servomotor

| Servo                  | ESP32     |
| ---------------------- | --------- |
| Señal (naranja/amarillo) | GPIO 25 |
| VCC (rojo)             | VIN / 5 V |
| GND (café/negro)       | GND       |

> ⚠️ **Nota:** el servo se alimenta con 5 V. Si el servo vibra, se reinicia el ESP32 o la LCD parpadea al moverse, utilice una fuente externa de 5 V y **una su GND con el GND del ESP32**.

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

## 💡 Dimmer MY-989 y foco (127 V CA)

El dimmer se coloca **en serie** entre la clavija y el foco. El ESP32 **no se conecta eléctricamente** al dimmer: solo el servo gira mecánicamente su perilla.

| Origen                          | Destino                       |
| ------------------------------- | ----------------------------- |
| Clavija, fase (cable negro)     | Dimmer, terminal de ENTRADA   |
| Clavija, neutro (cable blanco)  | Dimmer, terminal de ENTRADA   |
| Dimmer, terminales de SALIDA    | Foco                          |
| Eje del servo                   | Perilla del dimmer (acople)   |

```text
 Clavija 127 V CA
   │ fase (negro)      │ neutro (blanco)
   ▼                   ▼
┌───────────────────────────┐
│    DIMMER MY-989          │◄──── Perilla ◄── Acople ◄── SERVO
│  ENTRADA  │   SALIDA      │
└───────────────┬───────────┘
                │
                ▼
              FOCO
```

> ⚠️ Revise la serigrafía (marcas impresas) de su módulo, ya que el orden de las terminales puede variar entre fabricantes.

---

# 🔗 Conexión completa

```text
                         ESP32
                 ┌─────────────────┐
                 │                 │
                 │        GPIO 32  ├──────────── SUBIR ──── GND
                 │        GPIO 33  ├──────────── BAJAR ──── GND
                 │                 │
                 │        GPIO 25  ├──────────── SEÑAL SERVO
                 │                 │
                 │        GPIO 21  ├──────────── SDA
                 │        GPIO 22  ├──────────── SCL
                 │                 │
                 │            VIN  ├──────────── VCC (Servo y LCD)
                 │            GND  ├──────────── GND (Servo y LCD)
                 │                 │
                 └─────────────────┘

                 SERVO ──(acople)──► PERILLA DIMMER ──► FOCO
                                          ▲
                                   Clavija 127 V CA
```

---

# 📚 Librerías

## Arduino IDE

Instalar desde el **Administrador de Bibliotecas**:

* `LiquidCrystal_I2C`
* `ESP32Servo`
* `Wire` (incluida por defecto)

## PlatformIO

En el archivo `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

lib_deps =
    madhephaestus/ESP32Servo
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
#include <ESP32Servo.h>  

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo miServo;

int btnSubir = 32;
int btnBajar = 33;
int pinServo = 25;

int angulo = 0;       
int porcentaje = 0;   

void setup() {
  Serial.begin(115200);

  pinMode(btnSubir, INPUT_PULLUP);
  pinMode(btnBajar, INPUT_PULLUP);

  miServo.attach(pinServo);
  miServo.write(angulo); 

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Grados: ");
  lcd.setCursor(0, 1);
  lcd.print("Luz: ");
  
  actualizarPantalla();
}

void loop() {
  int huboCambio = 0; 

  if (digitalRead(btnSubir) == LOW) {
    angulo = angulo + 5;
    if (angulo > 180) { angulo = 180; } // Límite máximo
    
    huboCambio = 1;
    delay(150); 
  }

  else if (digitalRead(btnBajar) == LOW) { 
    angulo = angulo - 5;
    if (angulo < 0) { angulo = 0; } // Límite mínimo
    
    huboCambio = 1;
    delay(150); // Velocidad de bajada
  }

  if (huboCambio == 1) {
    // Movemos el motor al nuevo ángulo
    miServo.write(angulo);
     
    porcentaje = map(angulo, 0, 180, 0, 100);
    
    actualizarPantalla();
  }

  delay(10); 
}

void actualizarPantalla() {
  // Monitor Serial
  Serial.print("Angulo: "); Serial.print(angulo); Serial.print(" grados");
  Serial.print("  |  Iluminacion: "); Serial.print(porcentaje); Serial.println("%");

  lcd.setCursor(8, 0);
  lcd.print(angulo); 
  lcd.print("  "); 
  
  lcd.setCursor(5, 1);
  lcd.print(porcentaje);
  lcd.print("%   ");
}
```

---

# ⚙️ Funcionamiento del código

| Parte del código                     | Función                                                        |
| ------------------------------------ | -------------------------------------------------------------- |
| `pinMode(..., INPUT_PULLUP)`         | Activa la resistencia pull-up interna de los botones           |
| `miServo.attach(pinServo)`           | Asocia el servo al GPIO 25                                     |
| `miServo.write(angulo)`              | Mueve el servo al ángulo indicado                              |
| `digitalRead(btnSubir) == LOW`       | Detecta que el botón fue presionado                            |
| `map(angulo, 0, 180, 0, 100)`        | Convierte el ángulo (0°–180°) a porcentaje de luz (0%–100%)    |
| `actualizarPantalla()`               | Actualiza el LCD y el Monitor Serial solo cuando hay cambios   |
| `delay(150)`                         | Controla la velocidad de cambio y evita rebotes                |

---

# 🖥️ Resultado esperado

## LCD

```text
┌────────────────┐
│Grados: 90      │
│Luz: 50%        │
└────────────────┘
```

## Monitor Serial

```text
Angulo: 85 grados  |  Iluminacion: 47%
Angulo: 90 grados  |  Iluminacion: 50%
Angulo: 95 grados  |  Iluminacion: 53%
Angulo: 100 grados  |  Iluminacion: 56%
```

## Relación ángulo - luz

| Ángulo del servo | Luz (%) |
| ---------------: | ------: |
|               0° |      0% |
|              45° |     25% |
|              90° |     50% |
|             135° |     75% |
|             180° |    100% |

---

# 🛠️ Procedimiento

1. Arme el circuito del ESP32, los botones, el servo y la LCD con la clavija **desconectada**.
2. Acople el eje del servo a la perilla del dimmer y verifique que gire sin forzarse.
3. Conecte el foco a las terminales de SALIDA del dimmer y la clavija a las de ENTRADA.
4. Cargue el código al ESP32 desde Arduino IDE o PlatformIO.
5. Coloque la perilla del dimmer en su posición inicial (mínimo).
6. Conecte el ESP32 por USB y después la clavija a la corriente.
7. Use los botones SUBIR y BAJAR para regular la luz y observe la LCD.

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

| Tema                 | Descripción                                                                                                                                         |
| -------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| Rango de la perilla  | El servo gira solo 180°, pero la perilla de un dimmer suele girar 270° o más. Puede que no llegue al brillo máximo ni al apagado total.             |
| Tipo de foco         | El dimmer es para focos **incandescentes**. Los focos LED o ahorradores pueden parpadear o no regular, salvo que sean regulables.                    |
| Potencia máxima      | 2000 W a 220 V y aproximadamente 1154 W a 127 V. No exceda ese límite.                                                                               |
| Alimentación         | Si el servo reinicia el ESP32, use una fuente externa de 5 V con GND común.                                                                          |
| Seguridad            | Trabaje siempre desconectado y con el dimmer dentro de una caja cerrada.                                                                             |

---

# ⚠️ Problemas frecuentes

| Problema                          | Posible solución                                                  |
| --------------------------------- | ----------------------------------------------------------------- |
| LCD no muestra nada               | Verificar alimentación y conexiones SDA/SCL                       |
| LCD iluminado pero sin texto      | Revisar dirección I2C (`0x27` o `0x3F`)                           |
| LCD muestra cuadros               | Ajustar el contraste con el potenciómetro del módulo              |
| El servo no se mueve              | Revisar señal en GPIO 25, alimentación de 5 V y GND común         |
| El servo vibra o tiembla          | Usar fuente externa de 5 V y unir los GND                         |
| El ESP32 se reinicia al mover     | El servo consume mucha corriente, usar fuente externa             |
| Los botones no responden          | Verificar que un lado vaya al GPIO y el otro a GND                |
| El foco no cambia de brillo       | Revisar conexión del dimmer y que el foco sea regulable           |
| El foco parpadea                  | Foco LED no regulable, usar foco incandescente                    |
| Monitor Serial vacío              | Revisar `monitor_speed` (115200)                                  |
| Caracteres incorrectos            | Revisar configuración del LCD                                     |

---

# 🧠 Conceptos aprendidos

Esta práctica integra varios conceptos de sistemas embebidos y control:

### Control de posición con servo

```text
ESP32
  │
  │ Señal PWM (GPIO 25)
  ▼
SERVO ──► Perilla del dimmer
```

### Regulación de potencia AC

```text
Clavija 127 V CA
       │
       ▼
   DIMMER (triac / SCR)
       │
       ▼
     FOCO
```

El dimmer recorta la onda senoidal modificando el **ángulo de disparo** del triac. Al reducir el valor eficaz del voltaje, disminuye la potencia entregada al foco.

### LCD → I2C

```text
ESP32
  │
  │ I2C
  ├── SDA
  └── SCL
       │
       ▼
   LCD 16×2
```

Por lo tanto:

```text
   BOTONES (SUBIR / BAJAR)
            │
            ▼
          ESP32
            │
     ┌──────┴──────┐
     │             │
    PWM           I2C
     │             │
     ▼             ▼
   SERVO        LCD 16×2
     │
     ▼
   DIMMER
     │
     ▼
    FOCO (intensidad luminosa)
```

---

# 📊 Flujo del programa

```text
        INICIO
           │
           ▼
   Inicializar Serial
           │
           ▼
  Configurar botones (PULLUP)
           │
           ▼
   Inicializar Servo (0°)
           │
           ▼
     Inicializar LCD
           │
           ▼
  ┌──► Leer botones
  │        │
  │    ┌───┴────────────┐
  │    │                │
  │  ¿SUBIR?          ¿BAJAR?
  │    │                │
  │    ▼                ▼
  │  ángulo + 5      ángulo − 5
  │    │                │
  │    └───────┬────────┘
  │            ▼
  │     Limitar 0° a 180°
  │            │
  │            ▼
  │     Mover servo
  │            │
  │            ▼
  │   Calcular porcentaje (map)
  │            │
  │            ▼
  │   Mostrar LCD y Serial
  │            │
  └────────────┘
```

---

# 📝 Actividad para el estudiante

Modifique el programa para implementar las siguientes funciones:

### Nivel 1 — Básico

1. Cambiar el incremento del servo de 5° a 10°.
2. Cambiar la velocidad de cambio modificando el `delay(150)`.
3. Mostrar en el LCD el mensaje `Luz: 0%` al iniciar.

### Nivel 2 — Intermedio

4. Limitar el rango del servo para que coincida con el giro real de la perilla:

```cpp
int anguloMin = 0;
int anguloMax = 150;
```

5. Mostrar el texto `APAGADO` en el LCD cuando el porcentaje sea 0%:

```text
Grados: 0
APAGADO
```

6. Mostrar una barra de progreso en el LCD según el porcentaje de luz.

### Nivel 3 — Integración

7. Agregar un tercer botón que lleve la luz directamente a 0%, 50% o 100%.
8. Agregar un LED indicador que se encienda cuando la luz esté al máximo.
9. Guardar el último ángulo en memoria (`Preferences`) para recuperarlo al reiniciar.
10. Registrar las mediciones mediante el Monitor Serial.

---

# 📈 Extensión: Sistema de control

Como extensión de la práctica, el sistema puede evolucionar de un control **manual** a un control **automático en lazo cerrado**:

```text
          Sensor de luz (LDR / BH1750)
                    │
                    ▼
                  ESP32
                    │
          ┌─────────┴─────────┐
          │                   │
          ▼                   ▼
         LCD               Servo
                              │
                              ▼
                           Dimmer
                              │
                              ▼
                            Foco
                              │
                              └──► Luz medida (retroalimentación)
```

Esto permite posteriormente implementar aplicaciones de:

* 💡 Control de iluminación constante
* 🌡️ Control de temperatura con resistencias calefactoras
* 🌀 Control de velocidad de ventiladores
* 🏠 Automatización del hogar

---

# 📌 Competencias desarrolladas

* Programación de microcontroladores.
* Control de actuadores (servomotores).
* Lectura de entradas digitales con pull-up.
* Comunicación I2C.
* Manejo de displays.
* Regulación de potencia en corriente alterna.
* Programación C/C++.
* Depuración mediante Monitor Serial.
* Seguridad en instalaciones eléctricas.
* Integración sensor–controlador–actuador–interfaz.

---
