#include "disp7.h"

// ------------Funciones------------
// Función para configurar Display de 7 Segmentos
void configDisplay7(void){
    //Configuración como salidas
    pinMode(SEG_A, OUTPUT);
    pinMode(SEG_B, OUTPUT);
    pinMode(SEG_C, OUTPUT);
    pinMode(SEG_D, OUTPUT);
    pinMode(SEG_E, OUTPUT);
    pinMode(SEG_F, OUTPUT);
    pinMode(SEG_G, OUTPUT);
    pinMode(SEG_P, OUTPUT);

    // Configuración para ánodo común dejando todos los segmentos apagados inicialmente
    digitalWrite(SEG_A,HIGH);
    digitalWrite(SEG_B,HIGH);
    digitalWrite(SEG_C,HIGH);
    digitalWrite(SEG_D,HIGH);
    digitalWrite(SEG_E,HIGH);
    digitalWrite(SEG_F,HIGH);
    digitalWrite(SEG_G,HIGH);
    digitalWrite(SEG_P,HIGH);
} 

// Función para desplegar el número en el display de 7 segmentos.
void displayNum(uint8_t numero){
    // Apagar todos los segmentos primero
    digitalWrite(SEG_A, HIGH);
    digitalWrite(SEG_B, HIGH);
    digitalWrite(SEG_C, HIGH);
    digitalWrite(SEG_D, HIGH);
    digitalWrite(SEG_E, HIGH);
    digitalWrite(SEG_F, HIGH);
    digitalWrite(SEG_G, HIGH);

    switch (numero) {
        case 0:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_E, LOW);
            digitalWrite(SEG_F, LOW);
            break;
        case 1:
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            break;
        case 2:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_E, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 3:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 4:
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_F, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 5:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_F, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 6:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_E, LOW);
            digitalWrite(SEG_F, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 7:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            break;
        case 8:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_E, LOW);
            digitalWrite(SEG_F, LOW);
            digitalWrite(SEG_G, LOW);
            break;
        case 9:
            digitalWrite(SEG_A, LOW);
            digitalWrite(SEG_B, LOW);
            digitalWrite(SEG_C, LOW);
            digitalWrite(SEG_D, LOW);
            digitalWrite(SEG_F, LOW);
            digitalWrite(SEG_G, LOW);
            break;
    }
}

// Función para desplegar el punto
void displaydot(uint8_t punto){
    if (punto == 1){
        digitalWrite(SEG_P, LOW);
    }
    else{
        digitalWrite(SEG_P, HIGH);
    }
}
