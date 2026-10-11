# Sistemas-embebidos
# 📚 1. Introducción

En el estudio de los Sistemas Embebidos modernos, el desarrollo de aplicaciones basadas en el microcontrolador ESP32 requiere un entorno de trabajo sólido, escalable y flexible. Para optimizar el ciclo de desarrollo, se implementa una arquitectura de herramientas integrada que abarca desde la escritura del código y la compilación, hasta la simulación de circuitos y el control de versiones.

Este documento presenta la estructura, componentes y metodología de trabajo con la que se abordarán las prácticas del curso, utilizando un ecosistema compuesto por Visual Studio Code, PlatformIO, Arduino Framework, Wokwi, Git y GitHub.
SISTEMAS EMBEBIDOS (ESP32)

                             │
                             ▼
                    Visual Studio Code
                             │
                    ┌────────┴────────┐
                    │                 │
                    ▼                 ▼
               PlatformIO           Wokwi
                    │                 │
                 Compilar          Simular
                    │                 │
                    └────────┬────────┘
                             ▼
                          ESP32
                             │
                             ▼
                          GitHub
# 🎯 2. Objetivos
Como estudiante de la asignatura, los objetivos al utilizar este entorno de desarrollo son:

- Desarrollar y estructurar proyectos para el microcontrolador ESP32 utilizando C++ y el framework de Arduino.

- Compilar y gestionar dependencias de forma eficiente mediante PlatformIO.

- Validar y simular esquemas electrónicos y código en tiempo real con Wokwi antes de realizar el montaje físico.

- Interactuar con periféricos y sensores (virtuales y físicos) analizando señales e interfaces de comunicación (GPIO, ADC, PWM, I2C, etc.).

- Implementar control de versiones y respaldo continuo de prácticas mediante Git y GitHub.

- Desplegar aplicaciones en hardware real garantizando que el código probado en simulación sea ejecutable directamente en el ESP32 físico.

| Herramienta &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Tipo &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Función Principal en el Flujo de Trabajo |
| :--- | :--- | :--- |
| <br>**Visual Studio Code**<br>&nbsp; | <br>Editor / IDE<br>&nbsp; | <br>Editor de código fuente y centro de integración de extensiones.<br>&nbsp; |
| <br>**PlatformIO IDE**<br>&nbsp; | <br>Extensión / Entorno<br>&nbsp; | <br>Gestión de proyectos, compilación, librerías y carga a placa.<br>&nbsp; |
| <br>**Arduino Framework**<br>&nbsp; | <br>Framework<br>&nbsp; | <br>API de desarrollo para la programación del ESP32 en C/C++.<br>&nbsp; |
| <br>**Wokwi for VS Code**<br>&nbsp; | <br>Simulador<br>&nbsp; | <br>Simulación interactiva de componentes electrónicos y microcontrolador.<br>&nbsp; |
| <br>**Git**<br>&nbsp; | <br>CLI / Herramienta<br>&nbsp; | <br>Control de versiones local para el rastreo de cambios en los proyectos.<br>&nbsp; |
| <br>**GitHub**<br>&nbsp; | <br>Plataforma Web<br>&nbsp; | <br>Respaldo remoto, documentación y entrega de prácticas del curso.<br>&nbsp; |

# 🔄 5. Flujo de Trabajo Metodológico
El proceso de desarrollo de cada práctica sigue una metodología iterativa dividida en tres etapas clave: Desarrollo, Simulación e Implementación Física.

     ┌─────────────┐
     │ src/main.cpp│
     └──────┬──────┘
            │
            ▼
     ┌─────────────┐
     │ PlatformIO  │ ──► [Build / Compilación]
     └──────┬──────┘
            │
            ▼
    firmware.bin / ELF
            │
            ▼
     ┌─────────────┐
     │    Wokwi    │ ──► [Simulación y Validación]
     └──────┬──────┘
            │
            ▼
    ┌───────────────┐
    │  ESP32 Físico │ ──► [Carga e Implementación Real]
    └───────────────┘
    
1. Programación: Edición del código en src/main.cpp e inclusión de dependencias en platformio.ini.

2. Compilación: Verificación del código mediante el comando de compilación de PlatformIO (pio run).

3. Simulación: Ejecución del firmware en Wokwi cargando el circuito definido en diagram.json para verificar señales, displays y lógica de control.

4. Versionado: Subida del código fuente y configuraciones a GitHub omitiendo binarios.

5. Transferencia a Hardware: Carga del binario compilado al ESP32 físico para validación final.

# 🧠 7. Filosofía de Trabajo
Durante el desarrollo de las actividades de la materia, se adopta el siguiente principio operativo:
> **PlatformIO** administra el código y la compilación.  
> **Wokwi** administra la simulación del circuito.  
> **GitHub** administra el control de versiones y el respaldo.

De esta manera, cada solución desarrollada es completamente reproducible, portátil y fácil de documentar antes de realizar pruebas en componentes electrónicos reales.

# 🎓 8. Competencia a Desarrollar
Al hacer uso de este entorno, se adquiere la capacidad de operar un ecosistema profesional de desarrollo para sistemas embebidos, aplicando buenas prácticas de desarrollo de software, simulación previa de hardware y control de versiones, optimizando los tiempos de diseño y minimizando riesgos de daño en componentes físicos.
