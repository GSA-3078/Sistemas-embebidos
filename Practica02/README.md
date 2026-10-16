# 💧 Sistemas Embebidos – Medidor y control automático de nivel de agua con ESP32

![ESP32](https://img.shields.io/badge/Microcontrolador-ESP32-blue?logo=espressif)
![Arduino](https://img.shields.io/badge/Framework-Arduino-teal?logo=arduino)
![C++](https://img.shields.io/badge/Lenguaje-C%2B%2B-blue?logo=cplusplus)
![LCD](https://img.shields.io/badge/Display-LCD%2016x2-green)
![Sensor](https://img.shields.io/badge/Sensor-Ultras%C3%B3nico-purple)

---

## 📚 1. Introducción

En esta práctica se desarrolla un sistema de medición y control automático del nivel de agua utilizando un microcontrolador **ESP32**, un sensor ultrasónico, una pantalla LCD de 16×2 con comunicación I2C, botones pulsadores, relevadores y bombas de diafragma.

El sistema mide la distancia entre el sensor y la superficie del agua para calcular el nivel de llenado del recipiente. A partir de esta medición, estima el volumen de agua en litros y muestra los resultados en la pantalla LCD.

Además, permite establecer límites de nivel mínimo y máximo para controlar automáticamente las operaciones de llenado y vaciado mediante dos salidas de relevador.

El propósito principal es implementar un sistema que permita **supervisar el nivel de agua y automatizar su llenado y desllenado**, reduciendo la necesidad de intervención manual.

---

## 🎯 2. Objetivo

Desarrollar un sistema automatizado de medición y control de nivel de agua mediante un ESP32, integrando sensores, actuadores y una interfaz de visualización.

Los objetivos específicos son:

- Medir la distancia entre el sensor ultrasónico y la superficie del agua.
- Calcular el nivel de agua en centímetros y porcentaje.
- Estimar el volumen de agua contenido en el recipiente.
- Visualizar el nivel, el volumen y los límites de operación en una pantalla LCD.
- Ajustar los niveles mínimo y máximo mediante botones.
- Controlar dos relevadores para gestionar las operaciones de llenado y vaciado.

---

## 🧰 3. Materiales y componentes

| Componente | Función |
|---|---|
| ESP32 | Procesamiento de las mediciones y control del sistema |
| Sensor ultrasónico | Medición de la distancia hasta la superficie del agua |
| Pantalla LCD 16×2 I2C | Visualización de nivel, volumen y límites |
| 3 botones pulsadores | Ajuste de límites y selección de modo |
| 2 relevadores | Conmutación de las bombas |
| 2 bombas de diafragma | Movimiento del agua para llenar o vaciar el recipiente |
| Recipiente cilíndrico | Almacenamiento del agua |
| Fuente de alimentación | Suministro eléctrico para el sistema y las cargas |
| Cables de conexión | Interconexión de los componentes |

**Nota:** Las características de las bombas, los relevadores y las fuentes de alimentación deben ser compatibles entre sí. El ESP32 únicamente proporciona las señales de control; no debe alimentar directamente las bombas.

---

## 🔌 4. Asignación de pines

La siguiente tabla muestra la configuración establecida en el código.

| Componente | Pin ESP32 | Configuración |
|---|---|---|
| Sensor ultrasónico TRIG | GPIO 27 | Salida |
| Sensor ultrasónico ECHO | GPIO 26 | Entrada |
| Botón SW1 | GPIO 33 | Entrada con `INPUT_PULLUP` |
| Botón SW2 | GPIO 32 | Entrada con `INPUT_PULLUP` |
| Botón SWM | GPIO 25 | Entrada con `INPUT_PULLUP` |
| Relevador 1 | GPIO 12 | Salida |
| Relevador 2 | GPIO 14 | Salida |
| LCD SDA | GPIO 21 | I2C |
| LCD SCL | GPIO 22 | I2C |

### Configuración de la pantalla LCD

La pantalla utiliza la dirección I2C `0x27` y una configuración de 16 columnas por 2 filas.

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

La dirección I2C debe coincidir con la dirección real del módulo LCD utilizado.

### Funcionamiento de los botones

- **SW1:** incrementa el límite superior de nivel cuando se encuentra activo el modo de ajuste.
- **SW2:** incrementa el límite inferior y lo reinicia a cero cuando alcanza o supera el límite superior.
- **SWM:** habilita el ajuste de los límites mientras permanece pulsado.

Los tres botones utilizan `INPUT_PULLUP`, por lo que el estado `LOW` indica que el botón está pulsado.

---

## ⚙️ 5. Principio de funcionamiento

El sistema trabaja mediante un ciclo continuo de medición, cálculo, visualización y control.

1. El sensor ultrasónico emite un pulso para medir la distancia hasta la superficie del agua.
2. El ESP32 convierte la duración del eco en una distancia expresada en centímetros.
3. La distancia se limita al intervalo de medición definido en el programa.
4. Se calcula el nivel de agua y se estima el volumen en litros.
5. Los datos se muestran en la pantalla LCD.
6. El nivel calculado se compara con los límites mínimo y máximo.
7. Dependiendo del nivel, se activa la salida correspondiente para controlar las bombas.

### Diagrama general del sistema

```text
            SENSOR ULTRASÓNICO
                    |
                    v
                  ESP32
                    |
          +---------+---------+
          |                   |
          v                   v
    Cálculo de nivel     Control de botones
    y volumen            y límites
          |                   |
          v                   v
       LCD 16×2          Lógica de control
                              |
                    +---------+---------+
                    |                   |
                    v                   v
                RELEVADOR 1         RELEVADOR 2
                    |                   |
                    v                   v
              Bomba de llenado    Bomba de vaciado
```

**Importante:** El diagrama representa la función prevista de las dos bombas. La correspondencia entre cada relevador y cada bomba debe verificarse en el cableado físico.

---

## 📏 6. Medición de distancia y nivel de agua

Para obtener la distancia, el programa genera un pulso de activación en el pin TRIG y mide la duración del eco recibido en el pin ECHO.

```cpp
digitalWrite(trig, 0);
delayMicroseconds(2);

digitalWrite(trig, 1);
delayMicroseconds(10);

digitalWrite(trig, 0);

duracion = pulseIn(echo, 1);
distancia = duracion / 58.2;
```

La duración del pulso se convierte a centímetros mediante la constante `58.2`, utilizada como aproximación para un sensor ultrasónico convencional.

### Límites de medición

El código restringe la distancia calculada al intervalo de 6.5 a 23 centímetros:

```cpp
if (distancia > 23.0) {
    distancia = 23.0;
}

if (distancia < 6.5) {
    distancia = 6.5;
}
```

Esto establece un intervalo de medición de 16.5 cm.

### Cálculo del nivel

La altura de agua se obtiene restando la distancia medida a la referencia de 23 cm:

```cpp
float nivel_cm = 23.0 - distancia;
```

El porcentaje de llenado se calcula de la siguiente manera:

```cpp
float AL = (nivel_cm / 16.5) * 100.0;
```

Donde:

- `nivel_cm`: altura estimada del agua en centímetros.
- `AL`: porcentaje calculado de llenado.
- `23.0`: referencia de distancia para el nivel mínimo.
- `16.5`: intervalo de altura utilizado para calcular el porcentaje.

Por lo tanto, una distancia mayor representa un nivel de agua menor, mientras que una distancia menor representa un nivel de agua mayor.

---

## 🧮 7. Cálculo del volumen en litros

El recipiente se considera cilíndrico y el programa utiliza un radio de 6 cm:

```cpp
float radio = 6.0;
```

Para calcular el volumen de agua se utiliza la fórmula del volumen de un cilindro:

\[
V=\pi r^2h
\]

Donde:

- \(V\): volumen de agua en centímetros cúbicos.
- \(r\): radio del recipiente.
- \(h\): altura del agua.
- \(\pi\): constante matemática aproximada como 3.14159.

El cálculo implementado es:

```cpp
float volumen_cm3 = 3.14159 * (radio * radio) * nivel_cm;
float litros = volumen_cm3 / 1000.0;
```

La conversión a litros se realiza dividiendo el volumen en centímetros cúbicos entre 1000, ya que un litro equivale a 1000 cm³.

**Consideración:** El volumen mostrado es una estimación matemática. Su precisión depende de que el recipiente tenga una geometría cilíndrica, un radio interior cercano a 6 cm y una referencia de altura correctamente calibrada.

---

## 🖥️ 8. Visualización en la pantalla LCD

La pantalla LCD presenta la información en dos filas.

### Primera fila

```text
LL:20% AL:50%
```

- `LL`: límite inferior de llenado.
- `AL`: nivel actual calculado.

### Segunda fila

```text
HL:80% L:0.90
```

- `HL`: límite superior de llenado.
- `L`: volumen estimado en litros.

Los valores anteriores son únicamente un ejemplo ilustrativo; las lecturas reales dependen de la distancia medida y de los límites configurados.

El programa utiliza las siguientes instrucciones para posicionar y mostrar la información:

```cpp
lcd.setCursor(0, 0);
lcd.print("LL:");

lcd.setCursor(0, 1);
lcd.print("HL:");
```

La función `setCursor()` determina la posición inicial del texto, mientras que `lcd.print()` envía los valores y mensajes a la pantalla.

---

## 🎛️ 9. Ajuste de los límites de nivel

El sistema utiliza dos variables para establecer los límites de operación:

```cpp
int LL = 20;
int HL = 80;
```

- `LL`: límite inferior inicial del 20 %.
- `HL`: límite superior inicial del 80 %.

Estos valores pueden modificarse mediante los botones SW1 y SW2 mientras SWM se encuentra pulsado.

### Ajuste del límite superior

Cada pulsación detectada de SW1 incrementa el límite superior en una unidad.

```cpp
HL = HL + 1;

if (HL > 100) {
    HL = LL + 1;
}
```

Cuando el límite superior supera el 100 %, el programa lo establece en `LL + 1`.

### Ajuste del límite inferior

Cada pulsación detectada de SW2 incrementa el límite inferior.

```cpp
LL = LL + 1;

if (LL >= HL) {
    LL = 0;
}
```

Cuando el límite inferior alcanza o supera el límite superior, vuelve a cero.

Los límites se mantienen en las variables durante la ejecución del programa. El código no incorpora almacenamiento permanente de los ajustes, por lo que estos regresan a sus valores iniciales después de reiniciar el ESP32.

---

## 🔄 10. Control automático de llenado y vaciado

La variable `llenando` determina cuál de las dos salidas se activa:

```cpp
int llenando = 1;
```

El programa comienza con el estado de llenado seleccionado.

### Lógica de control

```cpp
if (AL <= LL) {
    llenando = 1;
}
else if (AL >= HL) {
    llenando = 0;
}
```

El funcionamiento es el siguiente:

- Cuando el nivel actual es menor o igual al límite inferior, se selecciona el estado de llenado.
- Cuando el nivel actual es mayor o igual al límite superior, se selecciona el estado de vaciado.
- Cuando el nivel se encuentra entre ambos límites, se conserva el estado anterior.

Este comportamiento crea una **banda de histéresis**, que evita cambiar de estado continuamente cuando el nivel se encuentra entre los dos límites.

### Control de los relevadores

```cpp
if (llenando == 1) {
    digitalWrite(rele1, HIGH);
    digitalWrite(rele2, LOW);
}
else {
    digitalWrite(rele1, LOW);
    digitalWrite(rele2, HIGH);
}
```

En el estado de llenado, el relevador 1 recibe `HIGH` y el relevador 2 recibe `LOW`. En el estado de vaciado, las señales se invierten.

La correspondencia entre las salidas y las bombas depende del cableado realizado. Asimismo, este código supone relevadores activos en nivel `HIGH`; si los módulos son activos en `LOW`, será necesario adaptar la lógica.

---

## 💻 11. Código fuente

El código principal integra la lectura del sensor ultrasónico, el cálculo del nivel, la estimación del volumen, la visualización en pantalla y el control de las salidas.

Las librerías utilizadas son:

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```

- `Wire.h`: permite la comunicación I2C utilizada por la pantalla.
- `LiquidCrystal_I2C.h`: facilita el control de la pantalla LCD.

El programa completo se encuentra en el archivo fuente de esta práctica. Para reproducir el comportamiento del sistema, es necesario conservar la configuración de pines, los límites iniciales y la lógica de control descrita en este documento.

---

## ⚠️ 12. Consideraciones técnicas y posibles mejoras

### Medición ultrasónica

La instrucción `pulseIn(echo, 1)` no especifica un tiempo límite de espera. Si no se recibe el eco, la medición puede tardar más de lo esperado y generar una lectura de cero. Como el programa limita después la distancia mínima a 6.5 cm, una lectura fallida podría interpretarse incorrectamente como un nivel de agua elevado.

Se recomienda incorporar un tiempo de espera y validar la lectura antes de utilizarla para controlar las bombas.

### Protección de las bombas

El código conmuta las dos salidas de relevador en función del estado de llenado o vaciado. Antes de utilizar el sistema físicamente, se debe comprobar que:

- Las bombas estén conectadas a la salida correcta.
- Los relevadores sean compatibles con el voltaje y la corriente de las bombas.
- Las bombas no funcionen en seco si el fabricante no lo permite.
- El recipiente y las conexiones hidráulicas no presenten fugas.
- Existan protecciones contra desbordamiento y funcionamiento anormal.

### Seguridad eléctrica

El agua y la electricidad requieren precauciones adicionales. Se deben mantener las conexiones eléctricas protegidas de la humedad y utilizar fuentes, aislamiento y protecciones apropiadas.

### Mejoras propuestas

- Añadir validación de lecturas del sensor ultrasónico.
- Incorporar alarmas por nivel demasiado alto o demasiado bajo.
- Agregar un modo manual para accionar las bombas.
- Guardar los límites configurados en memoria no volátil.
- Implementar una condición de seguridad ante fallas del sensor.
- Mostrar mensajes de error en la pantalla LCD.

---

## 🧠 13. Conclusión

Esta práctica integra medición ultrasónica, procesamiento de datos, visualización y control automático mediante un ESP32.

A partir de la distancia medida hasta la superficie del agua, el sistema estima el nivel de llenado y el volumen contenido en un recipiente cilíndrico. Posteriormente, compara el porcentaje de nivel con límites configurables para seleccionar el estado de llenado o vaciado.

El proyecto permite aplicar conceptos de sistemas embebidos, sensores, actuadores, comunicación I2C, entradas digitales y control mediante relevadores.

**El resultado es un prototipo de monitoreo y control de nivel de agua que utiliza una misma plataforma para medir, visualizar y gestionar automáticamente el movimiento del agua.**

---

## 🛠️ 14. Tecnologías utilizadas

- ESP32.
- Arduino Framework y C++.
- Sensor ultrasónico.
- Pantalla LCD 16×2 con I2C.
- Librería `Wire.h`.
- Librería `LiquidCrystal_I2C.h`.
- Botones pulsadores.
- Relevadores.
- Bombas de diafragma.

---

**Sistemas Embebidos | ESP32 · Sensores · Actuadores · Automatización**

*Medición → Cálculo → Visualización → Control automático*
