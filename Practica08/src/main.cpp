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

float setpoint = 40.0;
float kp       = 5.0;
float actual   = 0.0;
float error    = 0.0;
float u        = 0.0;
int   angulo   = ANGULO_MIN;
bool  sensorOK = false;
bool  lecturaIntentada = false;

const float SP_MAX = 99.0;
const float KP_MAX = 100.0;

unsigned long tPeticion = 0;
bool esperandoLectura = false;
const unsigned long PERIODO_LECTURA_MS   = 1000;
const unsigned long ESPERA_CONVERSION_MS = 800;

enum Pantalla { NORMAL, CAPTURANDO, MENSAJE };
Pantalla pantalla = NORMAL;
bool capturandoSP = true;
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
void iniciarCaptura(bool esSP);
void confirmarCaptura();
void cancelarCaptura();
void mostrarMensaje(const char* l1, const char* l2);
void guardarConfiguracion();
void leerSensor();
void calcularControl();
void actualizarLCD();
void imprimirLinea(int fila, const char* texto);

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

  prefs.begin("controlP", false);
  setpoint = prefs.getFloat("sp", 40.0);
  kp       = prefs.getFloat("kp", 5.0);
  prefs.end();

  Serial.println("=== Control P: foco con dimmer, Peltier siempre ON ===");
  Serial.println("A=SP  B=Kp  #=cambiar pantalla  C=borrar  D=cancelar  *=punto");

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
      u = 0;
      angulo = ANGULO_MIN;
      dimmer.write(angulo);
      Serial.println("Error: DS18B20 no responde");
      return;
    }

    sensorOK = true;
    actual = t;
    calcularControl();
  }
}

void calcularControl() {
  error = setpoint - actual;
  u = kp * error;
  if (u > 100.0) u = 100.0;
  if (u < 0.0)   u = 0.0;

  float grados = ANGULO_MIN + (u / 100.0) * (ANGULO_MAX - ANGULO_MIN);
  angulo = (int)(grados + 0.5);
  dimmer.write(angulo);

  Serial.print("SP:");     Serial.print(setpoint, 1);
  Serial.print(" T:");     Serial.print(actual, 1);
  Serial.print(" E:");     Serial.print(error, 1);
  Serial.print(" Kp:");    Serial.print(kp, 1);
  Serial.print(" U:");     Serial.print(u, 0);
  Serial.print("% Servo:"); Serial.println(angulo);
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

  if (tecla == 'A') { iniciarCaptura(true);  return; }
  if (tecla == 'B') { iniciarCaptura(false); return; }
  if (tecla == 'D') { cancelarCaptura();     return; }

  if (pantalla != CAPTURANDO) {
    if (tecla == '#') {
      paginaLCD = !paginaLCD;
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
    }
  } else if (tecla == 'C') {
    if (n > 0) entrada[n - 1] = '\0';
  } else if (tecla == '#') {
    confirmarCaptura();
  }
}

void iniciarCaptura(bool esSP) {
  capturandoSP = esSP;
  entrada[0] = '\0';
  pantalla = CAPTURANDO;
  ultimaTecla = millis();
}

void cancelarCaptura() {
  if (pantalla == CAPTURANDO) mostrarMensaje("Captura", "cancelada");
  else pantalla = NORMAL;
}

void confirmarCaptura() {
  if (strlen(entrada) == 0) {
    mostrarMensaje("Escribe un", "numero primero");
    return;
  }

  float valor = atof(entrada);
  float limite = capturandoSP ? SP_MAX : KP_MAX;

  if (valor < 0 || valor > limite) {
    char l2[17];
    snprintf(l2, sizeof(l2), "Usa 0 a %.0f", limite);
    mostrarMensaje("Fuera de rango", l2);
    return;
  }

  if (capturandoSP) setpoint = valor;
  else              kp = valor;
  guardarConfiguracion();
  if (sensorOK) calcularControl();

  char l1[17];
  snprintf(l1, sizeof(l1), "%s = %.1f", capturandoSP ? "SP" : "Kp", valor);
  mostrarMensaje(l1, "Guardado");
}

void mostrarMensaje(const char* l1, const char* l2) {
  strncpy(mensaje1, l1, 16); mensaje1[16] = '\0';
  strncpy(mensaje2, l2, 16); mensaje2[16] = '\0';
  pantalla = MENSAJE;
  momentoMensaje = millis();
}

void guardarConfiguracion() {
  prefs.begin("controlP", false);
  prefs.putFloat("sp", setpoint);
  prefs.putFloat("kp", kp);
  prefs.end();
}

void actualizarLCD() {
  char linea[32];

  if (pantalla == CAPTURANDO) {
    snprintf(linea, sizeof(linea), "Nuevo %s: %s_", capturandoSP ? "SP" : "Kp", entrada);
    imprimirLinea(0, linea);
    imprimirLinea(1, "#:OK C:Bor D:Sal");
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
  } else {
    snprintf(linea, sizeof(linea), "Kp:%5.1f %%/C", kp);
    imprimirLinea(0, linea);
    snprintf(linea, sizeof(linea), "SRV:%3d PELT:ON", angulo);
    imprimirLinea(1, linea);
  }
}

void imprimirLinea(int fila, const char* texto) {
  lcd.setCursor(0, fila);
  int n = 0;
  while (texto[n] != '\0' && n < 16) { lcd.print(texto[n]); n++; }
  while (n < 16) { lcd.print(' '); n++; }
}
