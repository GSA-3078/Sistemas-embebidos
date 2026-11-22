# 🎃 Animatrónico que grita con ESP32-C3
### 👻 Sensor de presencia, audio con DFPlayer Mini + PAM8403, ojos LED y servomotores

![ESP32-C3](https://img.shields.io/badge/ESP32--C3-Sistemas_Embebidos-red?style=for-the-badge&logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Desarrollo-orange?style=for-the-badge&logo=platformio&logoColor=white)
![Audio](https://img.shields.io/badge/Audio-DFPlayer_%2B_PAM8403-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-lightgrey?style=for-the-badge)

---

## 🧟 1. Descripción del proyecto

Animatrónico interactivo que **detecta la presencia de una persona** con un sensor PIR y reacciona con un **susto**: reproduce un audio de terror, hace parpadear dos LED que funcionan como ojos y deja listos tres servomotores para el movimiento mecánico.

El audio se reproduce con un módulo **DFPlayer Mini**. En lugar de usar su salida directa de bocina, se utilizan sus salidas **DAC_L y DAC_R**, que se conectan a un amplificador **PAM8403** con **dos bocinas de 3 W a 8 Ω**. Esto da mucho más volumen y mejor calidad que la salida SPK del módulo.

> ⚠️ **Nota:** el programa deja los servos inicializados (`attach`), pero la rutina `susto()` solo controla audio y LED. Para que se muevan hay que agregar `servoX.write(ángulo)` dentro de la secuencia.

---

## 🎯 2. Objetivo general

Diseñar e implementar un animatrónico controlado con un ESP32-C3 que responda a la presencia de personas mediante detección de movimiento, reproducción de audio amplificado, efectos luminosos y servomotores.

---

## ⚙️ 3. Funcionamiento del sistema

1. **⏳ Calentamiento:** al encender, el sistema espera **30 s** para que el sensor PIR se estabilice.
2. **😴 Reposo:** espera una señal HIGH del sensor.
3. **👁️ Detección:** al detectar presencia, imprime `Hay alguien!` por el monitor serial.
4. **🔊 Audio:** el DFPlayer reproduce la pista `0001`.
5. **🔴 Ojos:** durante la duración del audio, los dos LED parpadean de forma alternada (100 ms cada uno).
6. **🌑 Fin:** se apagan los LED y el sistema espera **8 s** antes de aceptar otra detección.

Cada vuelta del ciclo de parpadeo dura 200 ms, por lo que el número de vueltas es `segundosAudio × 5`.

---

## 🧰 4. Materiales

| Cantidad | Componente | Función |
|---:|---|---|
| 1 | ESP32-C3 (tipo Super Mini) | Unidad de control |
| 1 | Sensor PIR (HC-SR501) | Detección de presencia |
| 3 | Servomotores (SG90 o similar) | Movimiento mecánico |
| 2 | LED | Ojos |
| 2 | Resistencias de 220 Ω | Limitar corriente de los LED |
| 1 | Resistencia de 1 kΩ | Protección de la línea RX del DFPlayer |
| 1 | Módulo DFPlayer Mini | Reproductor de audio |
| 1 | Tarjeta microSD | Almacena el audio |
| 1 | Módulo amplificador PAM8403 | Amplificación estéreo 2 × 3 W |
| 2 | Bocinas de 3 W, 8 Ω | Salida de audio |
| 1 | Fuente de 5 V (recomendada ≥ 2 A) | Alimentación de servos y audio |
| — | Cables y protoboard | |

---

## 🔌 5. Arquitectura del sistema

```text
        SENSOR PIR
            │
            ▼
      ┌────────────┐        UART        ┌──────────────┐
      │  ESP32-C3  │───────────────────►│  DFPlayer    │
      │            │                    │  Mini (SD)   │
      └────────────┘                    └──────┬───────┘
        │   │    │                        DAC_L│DAC_R
        ▼   ▼    ▼                            ▼
     LED  LED  3 SERVOS               ┌──────────────┐
     (ojos)                           │   PAM8403    │
                                      └──┬────────┬──┘
                                         ▼        ▼
                                     Bocina L  Bocina R
```

---

## 🔧 6. Asignación de pines

| Dispositivo | Señal | Pin ESP32-C3 |
|---|---|---|
| Sensor PIR | OUT | GPIO3 |
| LED 1 (ojo) | Ánodo, con resistencia de 220 Ω | GPIO7 |
| LED 2 (ojo) | Ánodo, con resistencia de 220 Ω | GPIO10 |
| Servo 1 | Señal | GPIO0 |
| Servo 2 | Señal | GPIO1 |
| Servo 3 | Señal | GPIO6 |
| DFPlayer | TX del módulo → RX del ESP32 | GPIO4 |
| DFPlayer | RX del módulo ← TX del ESP32 (con 1 kΩ) | GPIO5 |

### Alimentación y audio

| Origen | Destino |
|---|---|
| 5 V | VCC del DFPlayer, PAM8403, PIR y servos |
| GND | GND común de todos los módulos y la fuente |
| DFPlayer `DAC_L` | Entrada `L` del PAM8403 |
| DFPlayer `DAC_R` | Entrada `R` del PAM8403 |
| GND del DFPlayer | Entrada `G` del PAM8403 |
| PAM8403 `L+` / `L-` | Bocina izquierda |
| PAM8403 `R+` / `R-` | Bocina derecha |

> No se usan las salidas `SPK1` y `SPK2` del DFPlayer. Revisa la serigrafía de tu PAM8403, porque el orden de sus pines puede variar.

---

## 🔊 7. Preparación del audio

1. Formatea la microSD en **FAT32**.
2. Copia el archivo con nombre numérico: `0001.mp3` (en la raíz o dentro de una carpeta `mp3`).
3. Usa un audio de aproximadamente **9 s**, o ajusta `segundosAudio` en el código a la duración real de tu archivo.
4. Inserta la tarjeta en el DFPlayer antes de energizarlo.

---

## 🧠 8. Parámetros configurables

| Variable | Valor | Descripción |
|---|---|---|
| `volumen` | 20 | Volumen del DFPlayer (0 a 30) |
| `segundosAudio` | 9 | Duración del audio en segundos |
| `espera` | 8000 | Descanso entre sustos (ms) |

---

## 📁 9. Estructura del proyecto (PlatformIO)

```
proyecto/
├── platformio.ini
└── src/
    └── main.cpp
```

**platformio.ini**

```ini
[env:esp32-c3]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
build_flags =
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
lib_deps =
    dfrobot/DFRobotDFPlayerMini
    madhephaestus/ESP32Servo
```

Las dos banderas de `build_flags` activan el monitor serial por el puerto USB nativo del ESP32-C3.

---

## 🚀 10. Compilación y carga

1. Abre la carpeta del proyecto en VS Code con PlatformIO.
2. Compila con **Build** y carga con **Upload**.
3. Si la placa no entra en modo de carga, mantén presionado **BOOT**, conecta el USB y suéltalo.
4. Abre el **Monitor Serial** a 115200 baudios.

---

## 💻 11. Código

```cpp
#include <DFRobotDFPlayerMini.h>
#include <ESP32Servo.h>

HardwareSerial dfSerial(1);
DFRobotDFPlayerMini myDFPlayer;

int dfRX = 4;
int dfTX = 5;
int sensor = 3;
int led1 = 7;
int led2 = 10;
int pinServo1 = 0;
int pinServo2 = 1;
int pinServo3 = 6;

int volumen = 20;
int segundosAudio = 9;
int espera = 8000;

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  Serial.begin(115200);

  pinMode(sensor, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  servo1.attach(pinServo1, 500, 2400);
  servo2.attach(pinServo2, 500, 2400);
  servo3.attach(pinServo3, 500, 2400);

  dfSerial.begin(9600, SERIAL_8N1, dfRX, dfTX);
  delay(1000);

  if (!myDFPlayer.begin(dfSerial)) {
    Serial.println("No se pudo iniciar el DFPlayer");
    while (true) delay(100);
  }

  myDFPlayer.volume(volumen);

  Serial.println("Esperando 30 segundos a que el sensor se caliente...");
  delay(30000);
  Serial.println("Listo!");
}

void loop() {
  if (digitalRead(sensor) == HIGH) {
    Serial.println("Hay alguien!");
    susto();
    delay(espera);
  }
}

void susto() {
  myDFPlayer.play(1);

  int vueltas = segundosAudio * 5;

  for (int i = 0; i < vueltas; i++) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    delay(100);
    digitalWrite(led1, LOW);
    digitalWrite(led2, HIGH);
    delay(100);
  }

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}
```

---

## 🧪 12. Pruebas funcionales

- [ ] Al encender, el monitor muestra `Esperando 30 segundos...` y luego `Listo!`.
- [ ] Si no hay tarjeta SD o el módulo está mal conectado, aparece `No se pudo iniciar el DFPlayer`.
- [ ] Al pasar frente al PIR se imprime `Hay alguien!`.
- [ ] Se reproduce el audio por ambas bocinas.
- [ ] Los LED parpadean de forma alternada durante todo el audio.
- [ ] Después del susto, el sistema espera 8 s antes de volver a activarse.
- [ ] Los servos no se calientan ni se reinician al funcionar juntos.

---

## 🛠️ 13. Solución de problemas

| Problema | Posible causa y solución |
|---|---|
| `No se pudo iniciar el DFPlayer` | Revisa el cruce TX/RX, la microSD (FAT32) y la alimentación |
| No suena nada | Verifica que el archivo se llame `0001.mp3` y que los DAC estén conectados al PAM8403 |
| Ruido o zumbido en las bocinas | Une todos los GND y alimenta los servos con fuente aparte |
| El ESP32 se reinicia al moverse los servos | Usa fuente externa de 5 V con corriente suficiente |
| Volumen bajo | Sube `volumen` (máx. 30) y revisa la alimentación del PAM8403 |
| El PIR se activa solo | Espera el calentamiento y ajusta sus potenciómetros de sensibilidad y tiempo |
| No hay texto en el monitor serial | Confirma las banderas `ARDUINO_USB_CDC_ON_BOOT` y `ARDUINO_USB_MODE` |

---

## ⚠️ 14. Recomendaciones de seguridad

- No alimentes los servos desde un GPIO ni desde el regulador del ESP32 si son varios.
- Comparte GND entre la fuente externa y el ESP32.
- Usa resistencias en serie con los LED y con la línea RX del DFPlayer.
- Asegura mecánicamente los servos para evitar atascos.
- Mantén los cables protegidos dentro de la estructura.

---

## 📚 15. Conceptos aplicados

- Lectura de sensores digitales (PIR).
- Comunicación serial UART con el DFPlayer Mini.
- Salidas DAC del DFPlayer y amplificación con PAM8403.
- Control de servomotores con la librería `ESP32Servo`.
- Control de salidas digitales (LED).
- Temporización con `delay()`.

---

## 💡 16. Posibles mejoras

- Mover los servos durante el audio (boca, cabeza, brazos).
- Sustituir `delay()` por `millis()` para no bloquear el programa.
- Reproducir varios audios al azar.
- Detectar el fin del audio con el pin `BUSY` del DFPlayer en lugar de usar un tiempo fijo.
- Agregar un botón para activar el susto manualmente.

---

## 🏁 17. Resultado esperado

Un animatrónico que detecta la presencia de una persona y responde con un grito amplificado por dos bocinas y ojos LED parpadeantes, listo para ampliarse con movimiento mecánico.

**Autor:** _(escribe aquí tu nombre y grupo)_
