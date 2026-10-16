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
