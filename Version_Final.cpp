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

//Pines de los segmentos A-G y DP del display se encuentran en disp7.h
#define DISP_1      23
#define DISP_2      18
#define DISP_3      5

//------------------ Variables ------------------
// ADAFRUIT IO - Configuración
#define IO_LOOP_DELAY 2000
unsigned long lastUpdate = 0;

AdafruitIO_Feed *canalTemperatura = io.feed("temperatura");
AdafruitIO_Feed *canalPB1 = io.feed("pb1");
bool pb1recibido = false; // Botón recibido por Adafruit IO

// Variables de temperatura
float temperatura = 0.0;
float temperaturaFiltrada = 0.0;
const float alpha = 0.75;
bool primerLectura = true;

// Variables del botón con debounce
bool botonAnterior = HIGH;
bool botonPresionado = false;
unsigned long tiempoBoton = 0;
const unsigned long debounceDelay = 50;

// Condiguración del servo
#define SERVO_CHANNEL 0
#define SERVO_FREQ    50
#define SERVO_RES     12

// Temperatura fijada y flag
float temperaturaFija = 0.0;
bool mostrarTemperaturaFija = false;

// Multiplexado
unsigned long tiempoDisplay = 0;
const unsigned long intervaloDisplay = 5; // ms
static uint8_t displayActivo = 0;

//------------------ Prototipado de las funciones ------------------
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
    //Setup Adafruit IO
    while(!Serial);
     Serial.print("Connecting to Adafruit IO");
    io.connect();
    canalPB1->onMessage(handleMessage); // Configurar callback para el feed del botón
    while(io.status() < AIO_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.println(io.statusText());
    canalPB1->get(); // Obtener el estado inicial del botón desde Adafruit IO
   
    // Configuración de pines de entrada y salida
    pinMode(PUSHB_1, INPUT_PULLUP);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_Y, OUTPUT);
    pinMode(LED_R, OUTPUT);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_R, LOW);

    // Configuración de pines para displays
    pinMode(DISP_1, OUTPUT);
    pinMode(DISP_2, OUTPUT);
    pinMode(DISP_3, OUTPUT);
    apagarTodosDisplays();

    // Configurar librería de display
    configDisplay7();
    // Configurar servo
    configurarServo();

    //Mensaje de inicio para asegurar que todo está correcto
    Serial.println("Sistema iniciado correctamente");
}

//------------------ Loop ------------------
void loop() {
    leerBotonConDebounce(); // Leer el botón con debounce
    io.run(); // Mantener la conexión con Adafruit IO
    
    // Leer temperatura periódicamente para actualizar la variable
    temperatura = leerTemperatura();

    // Multiplexado de displays. Se realiza cada intervalo definido y asegura que se muestre la temperatura fijada.
    if (mostrarTemperaturaFija && millis() - tiempoDisplay >= intervaloDisplay) {
        showTemperatureDisplays();
        tiempoDisplay = millis();
    }

    // Loop principal para manejar el botón y actualizar la temperatura fijada.
    if (botonPresionado || pb1recibido) {
        temperaturaFija = temperatura; 
        mostrarTemperaturaFija = true; //Cuando se presiona el botón, se activa la visualización de la temperatura fijada. 

        botonPresionado = false; //Se cambia el estado del botón y del flag de recepción.
        pb1recibido = false;

        Serial.print("\nBotón presionado - Temperatura: "); // Mostrar temperatura fijada en el monitor serie 
        Serial.println(temperaturaFija, 1);

        canalTemperatura->save(temperaturaFija); // Enviar la temperatura fijada a Adafruit IO 
        Serial.print("sending -> ");
        Serial.println(temperatura);

        // Resetear LEDs por seguridad
        digitalWrite(LED_R, LOW);       
        digitalWrite(LED_Y, LOW);
        digitalWrite(LED_G, LOW);

        // Encender LED y mover servo según temperatura obtenida.
        if (temperaturaFija < 22.0) {
            digitalWrite(LED_G, HIGH);
            moverServo(45); 
            Serial.println("Verde - Ángulo: 45°");
        }
        else if (temperaturaFija <= 25.0) {
            digitalWrite(LED_Y, HIGH);
            moverServo(90); 
            Serial.println("Amarillo - Ángulo: 90°");
        }
        else {
            digitalWrite(LED_R, HIGH);
            moverServo(135); 
            Serial.println("Rojo - Ángulo: 135°");
        }
    }
}

//------------------ Funciones Adicionales------------------

float leerTemperatura() { // Lee el sensor LM35, convierte mV a °C y aplica filtro pasa bajos
    int lecturaMV = analogReadMilliVolts(SENSOR_T);
    float tempC = lecturaMV / 10.0; // LM35: 10 mV/°C
    if (primerLectura) {
        temperaturaFiltrada = tempC;
        primerLectura = false;
    } else {
        temperaturaFiltrada = alpha * temperaturaFiltrada + (1 - alpha) * tempC;
    }
    return temperaturaFiltrada;
}

void configurarServo() { // Configura canal PWM 50Hz para servo y lo posiciona en 0°
    ledcSetup(SERVO_CHANNEL, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(SERVO_1, SERVO_CHANNEL);
    moverServo(0); // Posición inicial
}

void moverServo(int angulo) { // Mover servo a un ángulo entre 0° y 180° dependiendo de la temperatura
    int dutyCycle = map(angulo, 0, 180, 204, 409); // 12 bits
    ledcWrite(SERVO_CHANNEL, dutyCycle);
}

void showTemperatureDisplays() {  // Muestra temperatura en 3 displays 7-seg usando multiplexado temporal
    // Separar la temperatura fijada
    int entero = (int)temperaturaFija;           
    int decimal = (int)((temperaturaFija - entero) * 10 + 0.5); 
    int decenas = entero / 10;
    int unidades = entero % 10;

    // Evitar valores fuera de rango
    if (decenas > 9) decenas = 9;
    if (unidades > 9) unidades = 9;
    if (decimal > 9) decimal = 9;

    apagarTodosDisplays();

switch(displayActivo) {
    case 0: // Decenas
        displayNum(decenas);
        displaydot(0);
        digitalWrite(DISP_1, HIGH); // antes estaba LOW
        break;
    case 1: // Unidades con punto decimal
        displayNum(unidades);
        displaydot(1);
        digitalWrite(DISP_2, HIGH);
        break;
    case 2: // Decimal
        displayNum(decimal);
        displaydot(0);
        digitalWrite(DISP_3, HIGH);
        break;
}
    displayActivo = (displayActivo + 1) % 3;
}


void apagarTodosDisplays() { // Desactiva todos los displays para evitar ghosting en multiplexado  
    digitalWrite(DISP_1, LOW);
    digitalWrite(DISP_2, LOW);
    digitalWrite(DISP_3, LOW);
}

void leerBotonConDebounce() { // Detecta pulsación válida del botón con anti-rebote de 50ms
    bool lecturaActual = digitalRead(PUSHB_1);
    if (lecturaActual == LOW && botonAnterior == HIGH && millis() - tiempoBoton > debounceDelay) {
        botonPresionado = true;
        tiempoBoton = millis();
    }
    botonAnterior = lecturaActual;
}

void handleMessage(AdafruitIO_Data *data) { // Procesa comandos IoT del canal "pb1" para activación remota
    Serial.print("received <- ");
    Serial.println(data->value());

    if (data->toInt() == 1) {
        pb1recibido=true;
    }
}
