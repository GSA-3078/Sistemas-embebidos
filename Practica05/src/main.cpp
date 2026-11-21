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
