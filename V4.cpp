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
#define PUSHB_1     25  
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
//botón recibido por Adafruit IO
bool pb1recibido = false;

//Filtrado de temperatura
float temperatura = 0.0;
float temperaturaFiltrada = 0.0;
const float alpha = 0.75;
bool primerLectura = true;

// Botón
bool botonAnterior = HIGH;
bool botonPresionado = false;
unsigned long tiempoBoton = 0;
const unsigned long debounceDelay = 50;

// Servo PWM
#define SERVO_CHANNEL 0
#define SERVO_FREQ    50
#define SERVO_RES     12

// Displays multiplexeados
unsigned long tiempoDisplay = 0;
uint8_t displayActual = 0;
const unsigned long intervaloDisplay = 500; // ms

// Lectura temperatura para mostrar en displays de manera fija y flags.
float temperaturafija = 0.0;
bool mostrarTemperaturaFija = false;

//------------------ Prototipado de Funciones ------------------
void configurarServo();
float leerTemperatura();
void moverServo(int angulo);
void showTemperatureDisplays();
void leerBotonConDebounce();
void apagarTodosDisplays();
void handleMessage(AdafruitIO_Data *data);

//------------------ Setup ------------------
void setup() {
    
    Serial.begin(115200);
  //Setup de AdaFruit IO
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
    canalPB1->get();

  // Setup de Botones, LEDs y displays
  pinMode(PUSHB_1, INPUT_PULLUP);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);

  digitalWrite(LED_G, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_R, LOW);

  //Displays apagados para seguridad
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
    temperatura = leerTemperatura();
 
    //Multiplexeado de Displays
    if (mostrarTemperaturaFija && millis() - tiempoDisplay >= intervaloDisplay) {
        showTemperatureDisplays();
        tiempoDisplay = millis();
    }

    // Acciones al presionar el botón físico o el de Adafruit IO
    if (botonPresionado || pb1recibido) {
        // Resetear flags
        botonPresionado = false;
        pb1recibido = false;

        temperaturaFija = temperatura;
        mostrarTemperaturaFija = true;
     
        Serial.print("\nBotón presionado - Temperatura: ");
        Serial.print(temperatura, 1);
        Serial.println(" °C");
        canalTemperatura->save(temperatura);

        // LEDs y servo
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_Y, LOW);
        digitalWrite(LED_G, LOW)

        if (temperaturaFija < 22.0) {
            digitalWrite(LED_G, HIGH);
            moverServo(45);
            Serial.print("Verde - Ángulo: "); Serial.println(angulo);
        }
        else if (temperaturaFija <= 25.0) {
            digitalWrite(LED_Y, HIGH);
            moverServo(90);
            Serial.print("Amarillo - Ángulo: "); Serial.println(angulo);
        } 
        else {
            digitalWrite(LED_R, HIGH);
            moverServo(135);
            Serial.print("Rojo - Ángulo: "); Serial.println(angulo);
        }
    }
        if (millis() - lastUpdate > IO_LOOP_DELAY) {
        Serial.print("sending -> ");
        Serial.println(temperatura);
        canalTemperatura->save(temperatura);
        lastUpdate = millis();
    }
      
}
   
//------------------ Funciones ------------------

float leerTemperatura() {
  int lecturaMV = analogReadMilliVolts(SENSOR_T);
  float tempC = lecturaMV / 10.0;
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
  int dutyCycle = map(angulo, 0, 180, 102, 512); //Resolución de 12 bits para no sobrecargar el ADC; 1ms ~ 0° | 2ms ~ 180°
  ledcWrite(SERVO_CHANNEL, dutyCycle);
}

void showTemperatureDisplays() {
    // Separar número en decenas, unidades y decimal para enseñar cada uno dentro de los displays
    int entero = (int)temperaturaFija;                  
    int decimal = (int)((temperaturaFija - entero) * 10 + 0.5); 
    int decenas = entero / 10;
    int unidades = entero % 10;

    apagarTodosDisplays();

    switch(displayActivo){
       case 0: //decenas
           displayNum(decenas);
           displaydot(0);
           digitalWrite(DISP_1, HIGH); 
           break;
       case 1: //unidades con el punto encendido
           displayNum(unidades);
           displaydot(1);
           digitalWrite(DISP_2, HIGH); 
           break;
       case 2: //decimales
           displayNum(decimal);
           displaydot(0);
           digitalWrite(DISP_3, HIGH); 
           break;
    }
    displayActivo = (displayActivo + 1) % 3;
}

void apagarTodosDisplays() {
    digitalWrite(DISP_1, LOW);
    digitalWrite(DISP_2, LOW);
    digitalWrite(DISP_3, LOW);
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
