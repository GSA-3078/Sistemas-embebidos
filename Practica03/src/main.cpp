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
