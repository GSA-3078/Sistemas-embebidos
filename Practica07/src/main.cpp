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
