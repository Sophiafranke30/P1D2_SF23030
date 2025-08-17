// Prueba de sensor 
#include <Arduino.h>

#define ADC_en_mV 3300.0
#define ADC_bits  4096.0
#define PIN_SENSOR 34

void setup(){
  Serial.begin(115200);
}

void loop(){
  int adcValue = analogRead(PIN_SENSOR);
  float mV = (adcValue/ADC_bits)*ADC_en_mV;
  float tempC = (mV/10);

  Serial.print("Bits Leidos: ");
  Serial.print(adcValue,1);
  Serial.print(" | ");
  Serial.print("Temperatura (°C): ");
  Serial.println(tempC,1);

  delay(1500);
}