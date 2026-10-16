# ⚡ Sistemas Embebidos – Control de salidas digitales mediante botones con ESP32

![ESP32](https://img.shields.io/badge/Microcontrolador-ESP32-blue?logo=espressif)
![Arduino](https://img.shields.io/badge/Framework-Arduino-teal?logo=arduino)
![C++](https://img.shields.io/badge/Lenguaje-C%2B%2B-blue?logo=cplusplus)
![PlatformIO](https://img.shields.io/badge/IDE-PlatformIO-orange?logo=platformio)

---

## 📚 1. Introducción

En esta práctica se desarrolla un sistema de control de salidas digitales utilizando un microcontrolador **ESP32** y dos botones pulsadores. El sistema permite activar y desactivar un dispositivo mediante entradas digitales, utilizando la programación en C++ con el framework de Arduino.

El mismo código base se utiliza en tres prácticas diferentes, modificando el dispositivo de salida y el circuito electrónico:

1. **Control de un LED:** encendido y apagado mediante dos botones.
2. **Control de un motor DC:** activación y desactivación mediante un transistor.
3. **Control de un foco:** activación y desactivación mediante un módulo relevador.

El objetivo es comprender cómo las entradas digitales permiten controlar diferentes actuadores, conservando la misma lógica de programación.

---

## 🎯 2. Objetivo

Desarrollar un sistema de control mediante botones pulsadores y salidas digitales del ESP32, aplicando los conceptos de lectura de entradas, control de salidas y accionamiento de dispositivos electrónicos.

Al finalizar las prácticas se busca:

- Configurar entradas digitales utilizando `INPUT_PULLUP`.
- Controlar salidas digitales mediante `digitalWrite()`.
- Implementar condiciones para detectar la pulsación de botones.
- Comprender el funcionamiento de las macros en C++.
- Aplicar una misma lógica de control a diferentes dispositivos de salida.

---

## 🧰 3. Materiales y componentes

### Componentes generales

| Componente | Función |
|---|---|
| ESP32 | Ejecuta el programa y procesa las entradas |
| 2 botones pulsadores | Permiten activar y desactivar la salida |
| Cables de conexión | Establecen las conexiones eléctricas |
| Protoboard | Facilita el montaje del circuito |

### Componentes específicos de cada práctica

| Práctica | Elemento de salida | Elementos adicionales |
|---|---|---|
| 1 | LED | Resistencia limitadora de corriente |
| 2 | Motor DC | Transistor de conmutación y circuito de protección adecuado |
| 3 | Foco | Módulo relevador compatible con la carga |

**Nota:** Los componentes de potencia deben seleccionarse de acuerdo con las características eléctricas del motor o del foco. El ESP32 no debe alimentar directamente un motor ni una carga de potencia desde sus GPIO.

---

## 🔌 4. Asignación de pines

El programa utiliza la siguiente configuración:

| Elemento | Pin ESP32 | Configuración |
|---|---|---|
| Salida de control | GPIO 2 | `OUTPUT` |
| Botón 1 | GPIO 21 | `INPUT_PULLUP` |
| Botón 2 | GPIO 19 | `INPUT_PULLUP` |

Los botones utilizan la resistencia interna de elevación del ESP32 mediante `INPUT_PULLUP`. Por este motivo, la lectura normalmente es `HIGH` y cambia a `LOW` cuando se pulsa el botón conectado a tierra (GND).

La salida del GPIO 2 se utiliza como señal de control. Dependiendo de la práctica, esta señal se conecta al LED, a la etapa de control del transistor o a la entrada del módulo relevador.

---

## ⚙️ 5. Funcionamiento del programa

El sistema utiliza dos botones independientes:

- **Botón 1 (GPIO 21):** activa la salida.
- **Botón 2 (GPIO 19):** desactiva la salida.

Cuando se detecta la pulsación de un botón, el ESP32 ejecuta la acción correspondiente y muestra un mensaje en el monitor serial.

### Secuencia de funcionamiento

```text
             INICIO
                |
                v
       Configurar los pines
                |
                v
       Leer el botón 1 y 2
                |
        +-------+-------+
        |               |
        v               v
  ¿Botón 1?        ¿Botón 2?
        |               |
        v               v
  Activar salida   Desactivar salida
        |               |
        v               v
  Mensaje serial   Mensaje serial
        |               |
        +-------+-------+
                |
                v
       Repetir el proceso
```

El programa comprueba continuamente el estado de ambos botones dentro de la función `loop()`, permitiendo responder a las pulsaciones mientras el ESP32 está funcionando.

---

## 💻 6. Código fuente

El siguiente programa corresponde a la lógica de control utilizada en las tres prácticas.

```cpp
#define LED 2
#define SW1 21
#define SW2 19

#define PRENDE_LED digitalWrite(LED,HIGH)
#define APAGA_LED digitalWrite(LED,LOW)
#define PRESSED(x) digitalRead(x)==0

void setup() {

  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  pinMode(LED, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);

}

void loop() {

  if(PRESSED(SW1)){
      PRENDE_LED;
      Serial.println("Boton 1 pulsado");
      delay(100);
  }

  if(PRESSED(SW2)){
      APAGA_LED;
      Serial.println("Boton 2 pulsado");
      delay(100);
  }
}
```

### Descripción de las instrucciones principales

| Instrucción | Función |
|---|---|
| `#define LED 2` | Asigna el GPIO 2 a la salida de control |
| `#define SW1 21` | Identifica el pin del primer botón |
| `#define SW2 19` | Identifica el pin del segundo botón |
| `pinMode()` | Configura los pines como entrada o salida |
| `INPUT_PULLUP` | Activa la resistencia interna de elevación |
| `digitalRead()` | Lee el estado lógico de un botón |
| `digitalWrite()` | Establece el estado de la salida |
| `Serial.println()` | Muestra mensajes en el monitor serial |
| `delay(100)` | Introduce una pausa de 100 milisegundos |

### Uso de macros

El programa define tres macros para simplificar la escritura del código:

- `PRENDE_LED`: establece la salida en `HIGH`.
- `APAGA_LED`: establece la salida en `LOW`.
- `PRESSED(x)`: comprueba si la entrada indicada se encuentra en `LOW`.

La macro `PRESSED(x)` funciona con lógica activa en bajo porque los botones están configurados con `INPUT_PULLUP`.

**Importante:** Las macros conservan sus nombres originales aunque la salida se utilice para controlar un motor o un relevador. En esas prácticas, el GPIO 2 funciona como señal de control del dispositivo correspondiente.

---

## 💡 7. Práctica 1: Control de un LED

### Descripción

En la primera práctica se utiliza un LED como dispositivo de salida. El ESP32 controla su estado mediante el GPIO 2, permitiendo encenderlo y apagarlo con dos botones pulsadores.

### Funcionamiento

1. El sistema inicia configurando los pines.
2. Al pulsar el botón 1, la salida cambia a `HIGH` y el LED enciende.
3. Al pulsar el botón 2, la salida cambia a `LOW` y el LED apaga.
4. Los mensajes de cada pulsación se muestran en el monitor serial.

### Resultado esperado

El LED permanece encendido después de pulsar el botón 1 y apagado después de pulsar el botón 2, hasta que se recibe una nueva orden.

---

## ⚙️ 8. Práctica 2: Control de un motor DC mediante transistor

### Descripción

En la segunda práctica se sustituye el LED por un motor de corriente directa (DC). Para controlar el motor se utiliza un transistor como elemento de conmutación, ya que el GPIO del ESP32 no está diseñado para suministrar directamente la corriente que requiere un motor.

### Funcionamiento

1. El botón 1 activa la señal de control del transistor.
2. El transistor permite el paso de corriente por el motor y este comienza a girar.
3. El botón 2 desactiva la señal de control.
4. El transistor interrumpe la corriente del motor y este deja de recibir alimentación.

### Consideraciones eléctricas

- Utilizar un transistor adecuado para la corriente del motor.
- Alimentar el motor con una fuente apropiada para sus características.
- Compartir GND entre el ESP32 y la etapa de control cuando el circuito lo requiera.
- Incorporar un diodo de protección para absorber los picos inductivos del motor cuando se utilice una etapa de conmutación convencional.
- Evitar conectar el motor directamente al GPIO del ESP32.

### Resultado esperado

El motor se activa con el botón 1 y se desactiva con el botón 2, utilizando el transistor como interfaz entre la señal de control y la carga.

---

## 🔦 9. Práctica 3: Control de un foco mediante relevador

### Descripción

En la tercera práctica se utiliza un módulo relevador para controlar el encendido y apagado de un foco. El relevador permite utilizar una señal de control de baja potencia para accionar sus contactos y conmutar una carga externa compatible.

### Funcionamiento

1. El botón 1 establece la salida del ESP32 en `HIGH`.
2. La señal llega a la entrada de control del módulo relevador.
3. El relevador conmuta sus contactos y el foco cambia de estado.
4. El botón 2 establece la salida en `LOW` y el relevador vuelve al estado correspondiente.

**Nota:** Este comportamiento supone un módulo relevador que se activa con nivel `HIGH`. Algunos módulos son activos en `LOW`, por lo que debe verificarse su funcionamiento antes de utilizar el programa sin modificaciones.

### Consideraciones de seguridad

- Verificar la tensión y la corriente nominales del relevador.
- Utilizar un módulo compatible con los niveles lógicos del ESP32.
- No conectar directamente el foco a los pines del microcontrolador.
- Si se utiliza tensión de red, realizar el montaje con aislamiento, protección y conexiones adecuadas. Evitar manipular conductores energizados.

### Resultado esperado

El foco se enciende y apaga mediante los botones, utilizando el relevador como elemento de conmutación.

---

## 📊 10. Comparación de las tres prácticas

| Característica | LED | Motor DC | Foco con relevador |
|---|---|---|---|
| Señal de control | GPIO 2 | GPIO 2 | GPIO 2 |
| Dispositivo de salida | LED | Motor DC | Foco |
| Interfaz de potencia | No requiere transistor externo en un montaje LED adecuado | Transistor | Relevador |
| Botón 1 | Enciende | Activa el motor | Acciona el relevador |
| Botón 2 | Apaga | Desactiva el motor | Desactiva el relevador |
| Lógica del programa | La misma | La misma | La misma |

La principal diferencia entre las prácticas se encuentra en la etapa de salida. La lectura de los botones y la lógica de control se mantienen, mientras que el circuito electrónico se adapta al dispositivo utilizado.

---

## 🖥️ 11. Monitor serial

El programa utiliza una velocidad de comunicación de **115200 baudios**, configurada mediante:

```cpp
Serial.begin(115200);
```

Cuando se pulsa un botón, se muestra uno de los siguientes mensajes:

```text
Boton 1 pulsado
Boton 2 pulsado
```

Estos mensajes permiten verificar que el ESP32 detecta las pulsaciones y ejecuta las condiciones programadas.

---

## ⚠️ 12. Observaciones y posibles mejoras

El programa cumple con la función básica de activar y desactivar una salida mediante dos botones. Sin embargo, pueden realizarse algunas mejoras:

- Implementar antirrebote para evitar múltiples detecciones causadas por el contacto mecánico de los botones.
- Cambiar los nombres de las macros para que sean generales, por ejemplo, `ACTIVAR_SALIDA` y `DESACTIVAR_SALIDA`.
- Añadir indicadores visuales o mensajes de estado.
- Incorporar una función de parada de emergencia cuando la aplicación lo requiera.
- Adaptar la etapa de potencia a las características eléctricas del actuador.

El `delay(100)` introduce una pausa breve después de cada detección, pero no constituye por sí solo un sistema completo de antirrebote.

---

## 🧠 13. Conclusión

Las tres prácticas permiten comprender cómo un microcontrolador ESP32 puede utilizar entradas digitales para controlar diferentes dispositivos de salida.

Mediante dos botones pulsadores se implementa una lógica sencilla de activación y desactivación, que posteriormente se aplica a un LED, un motor DC controlado por transistor y un foco accionado mediante relevador.

De esta manera, se demuestra que **una misma lógica de programación puede utilizarse para controlar diferentes actuadores**, siempre que se adapte correctamente la interfaz electrónica de salida y se respeten las características eléctricas de cada dispositivo.

---

## 🛠️ 14. Tecnologías utilizadas

- ESP32.
- Arduino Framework.
- Lenguaje C++.
- Entradas digitales con `INPUT_PULLUP`.
- Control de salidas con `digitalWrite()`.
- Comunicación serial.
- Transistor de conmutación.
- Módulo relevador.

---

**Sistemas Embebidos | ESP32 · Arduino · Entradas y salidas digitales**

*Control de entradas → Procesamiento → Activación de actuadores*
