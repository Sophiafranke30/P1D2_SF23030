// ------------------ Prueba de Servo sincronizado a LEDS------------------
// Sophia Franke | 23030
//Fecha: 18/08/2025

//------------------ Librerías ------------------
#include <Arduino.h>
#include <ESP32Servo.h>

//------------------ Definición de Pines ------------------
#define SENSOR_T 33
#define SERVO_1 25
#define LED_R 4
#define LED_Y 2
#define LED_G 15
#define PushB_1 34

//------------------ Variables Globales ------------------
Servo servoMotor;

// ------------------ Prototipos de Funciones ------------------
float leerTemperatura();
void actualizarLED_Servo (int estado);
int estado_Termo(float t);
// ------------------ Setup ------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(PushB_1, INPUT_PULLUP);

  servoMotor.attach(SERVO_1);
  servoMotor.write(0); //Posicion incial donde se encuetra el servomotor
}

// ------------------ Loop ------------------
void loop() {
  if (digitalRead(PushB_1) ==LOW) {
    float temperatura = leerTemperatura();
    Serial.print("Temperatura: ");
    Serial.print(temperatura,1);
    Serial.println("°C");

    int estado = estado_Termo(temperatura);
    actualizarLED_Servo(estado);
    delay(500);
  }
  else {
    Serial.print("El botón no se ha presionado.");
    delay(3000);
  }

}

// ------------------ Funciones ------------------
float leerTemperatura(){ //Tomando código de la lectura de práctica del sensor
  int adcValue = analogRead(PIN_SENSOR);
  float mV = (adcValue/ADC_bits)*ADC_en_mV;
  float tempC = (mV/10);

  Serial.print("Bits Leidos: ");
  Serial.print(adcValue,1);
  Serial.print(" | ");
  Serial.print("Temperatura (°C): ");
  Serial.println(tempC,1);
}

void estado_Termo(float t){ // Función para calcular el estado y luego imprimir con switch case
  if (t<22.0) { //ED verde y 45°
    return 0;
  }
  else if (t>22.0 && t<25.0) { //LED amarillo y 90°
    return 1;
  }
  else { //LED rojo y 135°
return 2;
  }
}

void actualizarLED_Servo(int estado) {
  int angulo = 0;
  string led = "";
  switch (estado) {
    case 0: //Verde
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_Y, LOW);
      digitalWrite(LED_R, LOW);
      angulo = 45;
      led = "Verde";
      break;
    case 1: //Amarillo
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_Y, HIGH);
      digitalWrite(LED_R, LOW);
      angulo = 90;
      led = "Amarillo";
      break;
    case 2: //rojo
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_Y, LOW);
      digitalWrite(LED_R, HIGH);
      angulo = 135;
      led = "Rojo";
      break;
  }

  servoMotor.write(angulo);

  Serial.print("LED: ");
  Serial.print(led);
  Serial.print("; Angulo: ");
  Serial.print(angulo);
  Serial.println("°")
    }


      





