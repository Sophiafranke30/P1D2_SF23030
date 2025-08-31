#ifndef __DISP7_H__
#define __DISP7_H__

#include <Arduino.h>
#include <stdint.h>

// Definición de Pines
#define SEG_F   12
#define SEG_G   14
#define SEG_B   27
#define SEG_A   26
#define SEG_P   32
#define SEG_E   19
#define SEG_D   21
#define SEG_C   22

// ------------Prototipado de Funciones------------
// Función para configurar Display de 7 Segmentos
void configDisplay7(void);


// Función para desplegar número en display 7 segementos
void displayNum(uint8_t numero); //Dado que se están utilizando uint, agregar stdint

// Función para desplegar el punto () despliega ; 0 no despliega)
void displaydot(uint8_t punto);


#endif // __DISP7_H__
