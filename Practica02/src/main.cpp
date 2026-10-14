#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

int trig = 27;
int echo = 26;
long duracion;
float distancia;

int SW1 = 33;
int SW2 = 32;
int SWM = 25;

int rele1 = 12;
int rele2 = 14;

int LL = 20;
int HL = 80;

int llenando = 1;

float radio = 6.0;

void setup() {
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SWM, INPUT_PULLUP);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.begin(9600);
}

void loop() {
  //LÓGICA DE BOTONES
  if (digitalRead(SWM) == LOW) {

    if (digitalRead(SW1) == LOW) {
      HL = HL + 1;
      if (HL > 100) {
        HL = LL + 1;
      }
      delay(150);
    }

    if (digitalRead(SW2) == LOW) {
      LL = LL + 1;
      if (LL >= HL) {
        LL = 0;
      }
      delay(150);
    }
  }

//LECTURA DEL SENSOR ULTRASÓNICO
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);

  duracion = pulseIn(echo, 1);
  distancia = duracion / 58.2;

  if (distancia > 23.0) {
    distancia = 23.0;
  }
  if (distancia < 6.5) {
    distancia = 6.5;
  }

//CÁLCULOS
  float nivel_cm = 23.0 - distancia;
  float AL = (nivel_cm / 16.5) * 100.0;

  float volumen_cm3 = 3.14159 * (radio * radio) * nivel_cm;
  float litros = volumen_cm3 / 1000.0;

//PANTALLA LCD
  lcd.setCursor(0, 0);
  lcd.print("LL:"); 
  lcd.print(LL); 
  lcd.print("% AL:"); 
  lcd.print(AL, 0); 
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("HL:"); 
  lcd.print(HL); 
  lcd.print("% L:"); 
  lcd.print(litros, 2); 
  lcd.print("   ");

//CONTROL DE LOS RELES

  if (AL <= LL) {
    llenando = 1; 
  }
  else 
  if (AL >= HL) {
    llenando = 0;
  }

  if (llenando == 1) {
    digitalWrite(rele1, HIGH); 
    digitalWrite(rele2, LOW);
  }
  else {
    digitalWrite(rele1, LOW);  
    digitalWrite(rele2, HIGH); 
  }

  delay(50);
}
