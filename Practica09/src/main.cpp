#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP32Servo.h>

#define PIN_DS18B20       4
#define PIN_SERVO         18
#define RELAY_PELTIER     16
#define RELAY_EXTRACTOR   17
#define LCD_SDA           21
#define LCD_SCL           22

#define RELAY_ACTIVO   LOW
#define RELAY_INACTIVO (!RELAY_ACTIVO)

const bool EXTRACTOR_SIEMPRE_ON = true;

const int ANGULO_MIN = 122;
const int ANGULO_MAX = 180;

const float TEMP_SEGURIDAD = 80.0;

const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pinesFilas[FILAS]       = { 13, 14, 27, 26 };
byte pinesColumnas[COLUMNAS] = { 25, 33, 32, 23 };
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(PIN_DS18B20);
DallasTemperature sensores(&oneWire);
Servo dimmer;
Preferences prefs;

enum Parametro { P_SP = 0, P_KP = 1, P_KI = 2, P_KD = 3 };
const char* nombreParam[4] = { "SP", "Kp", "Ki", "Kd" };
const float limiteParam[4] = { 99.0, 100.0, 10.0, 100.0 };

float setpoint = 40.0;
float kp       = 5.0;
float ki       = 0.0;
float kd       = 0.0;

float actual   = 0.0;
float error    = 0.0;
float termP    = 0.0;
float termI    = 0.0;
float termD    = 0.0;
float u        = 0.0;
int   angulo   = ANGULO_MIN;

float tempAnterior = 0.0;
bool  hayTempAnterior = false;
unsigned long tUltimoCalculo = 0;

bool sensorOK = false;
bool lecturaIntentada = false;

unsigned long tPeticion = 0;
bool esperandoLectura = false;
const unsigned long PERIODO_LECTURA_MS   = 1000;
const unsigned long ESPERA_CONVERSION_MS = 800;

enum Pantalla { NORMAL, CAPTURANDO, MENSAJE };
Pantalla pantalla = NORMAL;
Parametro parametroActual = P_SP;
char entrada[7] = "";
unsigned long ultimaTecla = 0;
char mensaje1[17] = "";
char mensaje2[17] = "";
unsigned long momentoMensaje = 0;
const unsigned long TIEMPO_CAPTURA_MS = 10000;
const unsigned long TIEMPO_MENSAJE_MS = 2000;

unsigned long ultimaLCD = 0;
const unsigned long INTERVALO_LCD_MS = 300;
int paginaLCD = 0;

void manejarTeclado();
void iniciarCaptura(Parametro p);
void confirmarCaptura();
void cancelarCaptura();
void mostrarMensaje(const char* l1, const char* l2);
void guardarConfiguracion();
void leerSensor();
void calcularPID();
void reiniciarPID();
void apagarFoco();
void actualizarLCD();
void imprimirLinea(int fila, const char* texto);
float* punteroParam(Parametro p);

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PELTIER, OUTPUT);
  pinMode(RELAY_EXTRACTOR, OUTPUT);
  digitalWrite(RELAY_PELTIER, RELAY_ACTIVO);
  digitalWrite(RELAY_EXTRACTOR, EXTRACTOR_SIEMPRE_ON ? RELAY_ACTIVO : RELAY_INACTIVO);

  teclado.setDebounceTime(50);

  dimmer.setPeriodHertz(50);
  dimmer.attach(PIN_SERVO, 500, 2400);
  dimmer.write(ANGULO_MIN);

  delay(500);
  Wire.begin(LCD_SDA, LCD_SCL);
  Wire.setClock(100000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Iniciando...");

  sensores.begin();
  sensores.setResolution(12);
  sensores.setWaitForConversion(false);

  prefs.begin("controlPID", false);
  setpoint = prefs.getFloat("sp", 40.0);
  kp       = prefs.getFloat("kp", 5.0);
  ki       = prefs.getFloat("ki", 0.0);
  kd       = prefs.getFloat("kd", 0.0);
  prefs.end();

  Serial.println("=== Control PID: foco con dimmer, Peltier siempre ON ===");
  Serial.println("A=SP B=Kp C=Ki D=Kd  *=punto  #=confirmar / cambiar pantalla");
  Serial.println("t_s,SP,T,E,P,I,D,U,Servo");

  delay(1000);
  lcd.clear();
}

void loop() {
  manejarTeclado();
  leerSensor();

  if (millis() - ultimaLCD >= INTERVALO_LCD_MS) {
    ultimaLCD = millis();
    actualizarLCD();
  }
}

void leerSensor() {
  if (!esperandoLectura && millis() - tPeticion >= PERIODO_LECTURA_MS) {
    sensores.requestTemperatures();
    tPeticion = millis();
    esperandoLectura = true;
  }

  if (esperandoLectura && millis() - tPeticion >= ESPERA_CONVERSION_MS) {
    esperandoLectura = false;
    lecturaIntentada = true;
    float t = sensores.getTempCByIndex(0);

    if (t == DEVICE_DISCONNECTED_C || t < -50 || t > 125) {
      sensorOK = false;
      reiniciarPID();
      apagarFoco();
      Serial.println("Error: DS18B20 no responde");
      return;
    }

    sensorOK = true;
    actual = t;
    calcularPID();
  }
}

void calcularPID() {
  unsigned long ahora = millis();
  float Ts = (tUltimoCalculo == 0) ? 1.0 : (ahora - tUltimoCalculo) / 1000.0;
  tUltimoCalculo = ahora;
  if (Ts <= 0) Ts = 1.0;

  error = setpoint - actual;

  if (actual >= TEMP_SEGURIDAD) {
    reiniciarPID();
    apagarFoco();
    tempAnterior = actual;
    hayTempAnterior = true;
    Serial.println("SOBRETEMPERATURA: foco apagado");
    return;
  }

  termP = kp * error;

  if (hayTempAnterior) termD = -kd * (actual - tempAnterior) / Ts;
  else                 termD = 0.0;
  tempAnterior = actual;
  hayTempAnterior = true;

  float integralNueva = termI + ki * error * Ts;
  float uPrueba = termP + integralNueva + termD;
  bool saturadoArriba = (uPrueba > 100.0 && error > 0);
  bool saturadoAbajo  = (uPrueba < 0.0   && error < 0);
  if (!saturadoArriba && !saturadoAbajo) termI = integralNueva;
  termI = constrain(termI, 0.0, 100.0);

  u = termP + termI + termD;
  u = constrain(u, 0.0, 100.0);

  float grados = ANGULO_MIN + (u / 100.0) * (ANGULO_MAX - ANGULO_MIN);
  angulo = (int)(grados + 0.5);
  dimmer.write(angulo);

  Serial.print(ahora / 1000.0, 1); Serial.print(",");
  Serial.print(setpoint, 1);       Serial.print(",");
  Serial.print(actual, 2);         Serial.print(",");
  Serial.print(error, 2);          Serial.print(",");
  Serial.print(termP, 1);          Serial.print(",");
  Serial.print(termI, 1);          Serial.print(",");
  Serial.print(termD, 1);          Serial.print(",");
  Serial.print(u, 1);              Serial.print(",");
  Serial.println(angulo);
}

void reiniciarPID() {
  termI = 0.0;
  termP = 0.0;
  termD = 0.0;
  hayTempAnterior = false;
}

void apagarFoco() {
  u = 0.0;
  angulo = ANGULO_MIN;
  dimmer.write(angulo);
}

float* punteroParam(Parametro p) {
  switch (p) {
    case P_SP: return &setpoint;
    case P_KP: return &kp;
    case P_KI: return &ki;
    default:   return &kd;
  }
}

void manejarTeclado() {
  if (pantalla == CAPTURANDO && millis() - ultimaTecla > TIEMPO_CAPTURA_MS) {
    mostrarMensaje("Captura", "cancelada");
  }
  if (pantalla == MENSAJE && millis() - momentoMensaje > TIEMPO_MENSAJE_MS) {
    pantalla = NORMAL;
  }

  char tecla = teclado.getKey();
  if (!tecla) return;
  ultimaTecla = millis();

  if (tecla == 'A') { iniciarCaptura(P_SP); return; }
  if (tecla == 'B') { iniciarCaptura(P_KP); return; }
  if (tecla == 'C') { iniciarCaptura(P_KI); return; }
  if (tecla == 'D') { iniciarCaptura(P_KD); return; }

  if (pantalla != CAPTURANDO) {
    if (tecla == '#') {
      paginaLCD = (paginaLCD + 1) % 3;
      pantalla = NORMAL;
    }
    return;
  }

  int n = strlen(entrada);
  if (tecla >= '0' && tecla <= '9') {
    if (n < 5) { entrada[n] = tecla; entrada[n + 1] = '\0'; }
  } else if (tecla == '*') {
    if (n > 0 && n < 5 && strchr(entrada, '.') == NULL) {
      entrada[n] = '.'; entrada[n + 1] = '\0';
    } else if (n == 0) {
      entrada[0] = '0'; entrada[1] = '.'; entrada[2] = '\0';
    }
  } else if (tecla == '#') {
    if (n == 0) cancelarCaptura();
    else        confirmarCaptura();
  }
}

void iniciarCaptura(Parametro p) {
  parametroActual = p;
  entrada[0] = '\0';
  pantalla = CAPTURANDO;
  ultimaTecla = millis();
}

void cancelarCaptura() {
  mostrarMensaje("Captura", "cancelada");
}

void confirmarCaptura() {
  float valor = atof(entrada);
  float limite = limiteParam[parametroActual];

  if (valor < 0 || valor > limite) {
    char l2[17];
    snprintf(l2, sizeof(l2), "Usa 0 a %.0f", limite);
    mostrarMensaje("Fuera de rango", l2);
    return;
  }

  *punteroParam(parametroActual) = valor;
  if (parametroActual == P_KI) termI = 0.0;
  guardarConfiguracion();

  char l1[17];
  snprintf(l1, sizeof(l1), "%s = %.2f", nombreParam[parametroActual], valor);
  mostrarMensaje(l1, "Guardado");
}

void mostrarMensaje(const char* l1, const char* l2) {
  strncpy(mensaje1, l1, 16); mensaje1[16] = '\0';
  strncpy(mensaje2, l2, 16); mensaje2[16] = '\0';
  pantalla = MENSAJE;
  momentoMensaje = millis();
}

void guardarConfiguracion() {
  prefs.begin("controlPID", false);
  prefs.putFloat("sp", setpoint);
  prefs.putFloat("kp", kp);
  prefs.putFloat("ki", ki);
  prefs.putFloat("kd", kd);
  prefs.end();
}

void actualizarLCD() {
  char linea[32];

  if (pantalla == CAPTURANDO) {
    snprintf(linea, sizeof(linea), "Nuevo %s: %s_", nombreParam[parametroActual], entrada);
    imprimirLinea(0, linea);
    imprimirLinea(1, "#:OK #vacio:Sal");
    return;
  }

  if (pantalla == MENSAJE) {
    imprimirLinea(0, mensaje1);
    imprimirLinea(1, mensaje2);
    return;
  }

  if (!lecturaIntentada) {
    imprimirLinea(0, "Leyendo");
    imprimirLinea(1, "DS18B20...");
    return;
  }

  if (!sensorOK) {
    snprintf(linea, sizeof(linea), "SP:%4.1f Kp:%4.1f", setpoint, kp);
    imprimirLinea(0, linea);
    imprimirLinea(1, "Error DS18B20");
    return;
  }

  if (paginaLCD == 0) {
    snprintf(linea, sizeof(linea), "T:%4.1fC SP:%4.1f", actual, setpoint);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "E:%4.1fC OUT:%3.0f%%", error, u);
    imprimirLinea(1, linea);
  } else if (paginaLCD == 1) {
    snprintf(linea, sizeof(linea), "Kp:%4.1f Ki:%4.2f", kp, ki);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "Kd:%4.1f SER:%3d", kd, angulo);
    imprimirLinea(1, linea);
  } else {
    snprintf(linea, sizeof(linea), "P:%5.1f I:%5.1f", termP, termI);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "D:%5.1f U:%3.0f%%", termD, u);
    imprimirLinea(1, linea);
  }
}

void imprimirLinea(int fila, const char* texto) {
  lcd.setCursor(0, fila);
  int n = 0;
  while (texto[n] != '\0' && n < 16) { lcd.print(texto[n]); n++; }
  while (n < 16) { lcd.print(' '); n++; }
}
