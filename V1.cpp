// Sophia Franke | 23030
// PROYECTO 1 : SENSOR DE TEMPERATURA
// DIGITAL II

#include <Arduino.h>
#include <disp7.h>
#include <stdint.h>
#include "config.h"
//------------------ Pines ------------------
#define SERVO_1     13
#define SENSOR_T    34
#define PUSHB_1     25  // Botón cambiado
#define LED_G       15
#define LED_Y       2
#define LED_R       4

#define DISP_1      23
#define DISP_2      18
#define DISP_3      5


// Segmentos definidos en disp7.h: SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_P

//------------------ Variables ------------------
// ADAFRUIT IO - Configuración
#define IO_LOOP_DELAY 2000
unsigned long lastUpdate = 0;
AdafruitIO_Feed *canalTemperatura = io.feed("temperatura");
AdafruitIO_Feed *canalPB1 = io.feed("pb1");
 
float temperatura = 0.0;
float temperaturaFiltrada = 0.0;
const float alpha = 0.75;
bool primerLectura = true;

// Botón
bool botonAnterior = HIGH;
bool botonPresionado = false;
unsigned long tiempoBoton = 0;
const unsigned long debounceDelay = 50;

//botón recibido por Adafruit IO
bool pb1recibido = false;

// Servo PWM
#define SERVO_CHANNEL 0
#define SERVO_FREQ    50
#define SERVO_RES     12

// Displays
unsigned long tiempoDisplay = 0;
uint8_t displayActual = 0;
const unsigned long intervaloDisplay = 500; // ms

// Lectura temperatura
unsigned long tiempoLectura = 0;
const unsigned long intervaloLectura = 100; // ms

//------------------ Prototipos ------------------
void configurarServo();
float leerTemperatura();
void moverServo(int angulo);
void showTemperatureDisplays(float temp);
void leerBotonConDebounce();
void handleMessage(AdafruitIO_Data *data);


//------------------ Setup ------------------
void setup() {
//ADAFRUIT IO
    Serial.begin(115200);
    while(!Serial);
    Serial.print("Connecting to Adafruit IO");
    io.connect();
    canalPB1->onMessage(handleMessage);
    while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    Serial.println();
    Serial.println(io.statusText());
  //counter->get();
  canalPB1->get();


  // Botón y LEDs
  pinMode(PUSHB_1, INPUT_PULLUP);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);

  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);

  //Displays apagados inicialmente
  digitalWrite(DISP_1, HIGH);
  digitalWrite(DISP_2, HIGH);
  digitalWrite(DISP_3, HIGH);

  // Configurar displays de 7 segmentos
  configDisplay7();

  // Configurar servo
  configurarServo();

  Serial.println("Sistema iniciado correctamente");
}

//------------------ Loop ------------------
void loop() {
    leerBotonConDebounce();
    io.run();

    // Leer temperatura periódicamente
    if (millis() - tiempoLectura > intervaloLectura) {
        temperatura = leerTemperatura();
        tiempoLectura = millis();
    }

    // Actualizar LEDs, servo y displays solo si presionan el botón
    if (botonPresionado || pb1recibido) {
        Serial.print("\n");
        Serial.print("Botón presionado - Temperatura: ");
        Serial.print(temperatura, 1);
        Serial.println(" °C");
        Serial.print("Ángulo servo: ");
        Serial.println(map((int)(temperatura * 10), 150, 350, 0, 180));
        Serial.print("LED encendido: ");
        if (temperatura < 22.0) {
            Serial.println("Verde");
        } 
        else if (temperatura <= 25.0) {
            Serial.println("Amarillo");
        } 
        else {
            Serial.println("Rojo");
        }

        // Resetear flags
        botonPresionado = false;
        pb1recibido = false;

        // LEDs y servo
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_Y, LOW);
        digitalWrite(LED_G, LOW);

        if (temperatura < 22.0) {
            digitalWrite(LED_G, HIGH);
            moverServo(map((int)(temperatura * 10), 150, 219, 45, 45));
        } 
        else if (temperatura <= 25.0) {
            digitalWrite(LED_Y, HIGH);
            moverServo(map((int)(temperatura * 10), 220, 250, 135, 135));
        } 
        else {
            digitalWrite(LED_R, HIGH);
            moverServo(map((int)(temperatura * 10), 251, 350, 180, 180));
        }

        // MOSTRAR DISPLAYS (sin multiplexar)
        showTemperatureDisplays(temperatura);

        // Enviar datos a Adafruit IO
        Serial.print("sending -> ");
        Serial.println(temperatura);
        canalTemperatura->save(temperatura);
    }
   
}

//------------------ Funciones ------------------

float leerTemperatura() {
  int lecturaMV = analogReadMilliVolts(SENSOR_T);
  float tempC = lecturaMV / 10.0;
  // LM35: 10 mV/°C

  if (primerLectura) {
    temperaturaFiltrada = tempC;
    primerLectura = false;
  } else {
    temperaturaFiltrada = alpha * temperaturaFiltrada + (1 - alpha) * tempC;
  }

  return temperaturaFiltrada;
}

void configurarServo() {
  ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
  ledcAttachPin(SERVO_1, SERVO_CHANNEL);
  moverServo(0);
}

void moverServo(int angulo) {
  int dutyCycle = map(angulo, 0, 180, 204, 409); // 12 bits
  ledcWrite(SERVO_CHANNEL, dutyCycle);
}

void showTemperatureDisplays(float temp) {
    // Separar número en decenas, unidades y decimal para enseñar cada uno dentro de los displays
    int entero = (int)temp;                  
    int decimal = (int)((temp - entero) * 10 + 0.5); 
    int decenas = entero / 10;
    int unidades = entero % 10;

    // Encender todos los displays
    digitalWrite(DISP_1, LOW);
    digitalWrite(DISP_2, LOW);
    digitalWrite(DISP_3, LOW);

    // Mostrar decenas en display 1
    displayNum(decenas);
    displaydot(LOW);
    delay(10); // Pequeña pausa para estabilizar

    // Mostrar unidades en display 2 (con punto decimal)
    displayNum(unidades);
    displaydot(HIGH);
    delay(10);

    // Mostrar decimal en display 3
    displayNum(decimal);
    displaydot(LOW);
    delay(10);
}

void leerBotonConDebounce() {
  bool lecturaActual = digitalRead(PUSHB_1);
  if (lecturaActual == LOW && botonAnterior == HIGH && millis() - tiempoBoton > debounceDelay) {
    botonPresionado = true;
    tiempoBoton = millis();
  }
  botonAnterior = lecturaActual;
}

void handleMessage(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  Serial.println(data->value());
    if (data->toInt() == 1) {
      pb1recibido = true;
    }
}
