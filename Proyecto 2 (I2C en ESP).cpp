//******************************************/
// Universidad del Valle de Guatemala
// BE3029 - Electronica Digital 2
// Stephany Mejia
// Proyecto 2
// Parte LCD + Comunicacion SPI
// MCU: ESP32 dev kit 1.0
//******************************************/

//******************************************/
// Librerias
//******************************************/

#include <Arduino.h>
#include <stdint.h>
#include <LiquidCrystal.h>
#include <ESP32SPISlave.h>
#include <Wire.h>

//******************************************/
// Definiciones
//******************************************/

// Pin de señal del potenciometro
#define POT1 34

//******************************************/
// Pines LCD
//******************************************/

#define LCD_RS 13
#define LCD_E  14

#define LCD_D4 25
#define LCD_D5 26
#define LCD_D6 27
#define LCD_D7 32

//******************************************/
// Pines LEDs
//******************************************/

#define LED1 4
#define LED2 15
#define LED3 33

//******************************************/
// SPI
//******************************************/

// Solamente una transaccion SPI pendiente
#define QUEUE_SIZE 1

//******************************************/
// LCD
//******************************************/

LiquidCrystal lcd(LCD_RS, LCD_E,
                  LCD_D4, LCD_D5, LCD_D6, LCD_D7);
//******************************************/
// SPI Slave
//******************************************/

ESP32SPISlave slave;

#define I2C_ADDRESS 0x08

uint16_t valorADC_I2C = 0;

// Buffer que recibe los datos de la Nucleo
//
// Byte 0 = numero de LED
// Byte 1 = parte alta del tiempo
// Byte 2 = parte baja del tiempo

uint8_t datosRecibidos[3] = {0, 0, 0};

// Buffer de transmision
uint8_t datosEnviar[3] = {0, 0, 0};

//******************************************/
// Variables globales
//******************************************/

// Lectura ADC de 0 a 4095
int lecturaADC = 0;

// Valor convertido de 0 a 255
uint8_t valorPot = 0;

// Voltaje del potenciometro
float voltaje = 0.0;

// Ultimo LED activado
char ultimoLED = '-';

// Tiempo para actualizar LCD
unsigned long tiempoLCD = 0;

//******************************************/
// Prototipos
//******************************************/

void mostrarLCD(float voltaje, uint8_t valorPot);

void ejecutarLED(uint8_t numeroLED,
                 uint16_t tiempo);

void solicitarMedicionI2C();
void enviarMedicionI2C();
//******************************************/
// SETUP
//******************************************/

void setup()
{
    //******************************************/
    // UART
    //******************************************/

    Serial.begin(115200);


    //******************************************/
    // ADC
    //******************************************/

    analogReadResolution(12);

    pinMode(POT1, INPUT);
    //******************************************/
    // I2C SLAVE
    //******************************************/

    Wire.begin(I2C_ADDRESS, 21, 22);

    Wire.onRequest(solicitarMedicionI2C);


    //******************************************/
    // LEDs
    //******************************************/

    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);


    //******************************************/
    // LCD
    //******************************************/

    lcd.begin(16, 2);

    lcd.clear();


    //******************************************/
    // SPI SLAVE
    //******************************************/

    // SPI Mode 0
    slave.setDataMode(SPI_MODE0);

    // Una transaccion pendiente
    slave.setQueueSize(QUEUE_SIZE);

    // Inicializar ESP32 como SPI Slave
    //
    // VSPI:
    // CS   = GPIO 5
    // SCK  = GPIO 18
    // MOSI = GPIO 23
    // MISO = GPIO 19

    slave.begin(VSPI);


    //******************************************/
    // Mensaje inicial
    //******************************************/

    Serial.println();
    Serial.println("==============================");
    Serial.println("       ESP32 SPI SLAVE");
    Serial.println("==============================");
    Serial.println("Esperando comando...");
}

//******************************************/
// LOOP
//******************************************/

void loop()
{

    //******************************************/
    // Preparar una transaccion SPI
    //******************************************/

    if (slave.hasTransactionsCompletedAndAllResultsHandled())
    {

        // Limpiar datos anteriores

        datosRecibidos[0] = 0;
        datosRecibidos[1] = 0;
        datosRecibidos[2] = 0;


        // Preparar una transaccion de 3 bytes
        //
        // datosEnviar    = ESP32 -> Nucleo
        // datosRecibidos = Nucleo -> ESP32

        slave.queue(datosEnviar,
                    datosRecibidos,
                    3);


        // Habilitar la transaccion

        slave.trigger();
    }


    //******************************************/
    // Verificar si llegaron los 3 bytes
    //******************************************/

    if (slave.hasTransactionsCompletedAndAllResultsReady(QUEUE_SIZE))
    {

        // Obtener resultado de la transaccion

        slave.numBytesReceivedAll();


        //******************************************/
        // Obtener numero de LED
        //******************************************/

        uint8_t numeroLED =
            datosRecibidos[0];


        //******************************************/
        // Reconstruir tiempo
        //******************************************/

        // Byte 1 = MSB
        // Byte 2 = LSB

        uint16_t tiempo =
            ((uint16_t)datosRecibidos[1] << 8)
            |
            datosRecibidos[2];


        //******************************************/
        // Mostrar datos recibidos
        //******************************************/

        Serial.println();
        Serial.println("Comando recibido por SPI");

        Serial.print("LED: ");
        Serial.println(numeroLED);

        Serial.print("Tiempo: ");
        Serial.print(tiempo);
        Serial.println(" ms");


        //******************************************/
        // Verificar datos
        //******************************************/

        if (numeroLED >= 1 &&
            numeroLED <= 3 &&
            tiempo > 0)
        {

            // Ejecutar accion

            ejecutarLED(numeroLED,
                        tiempo);

        }

        else
        {

            Serial.println("Comando SPI invalido");

        }


        Serial.println("Esperando nuevo comando...");
    }


    //******************************************/
    // Leer potenciometro
    //******************************************/

    lecturaADC = analogRead(POT1);


    // Convertir de 12 bits a 8 bits

    valorPot = map(lecturaADC,
                   0,
                   4095,
                   0,
                   255);


    // Convertir ADC a voltios

    voltaje =
        lecturaADC * 3.3 / 4095.0;


    //******************************************/
    // Actualizar LCD cada 200 ms
    //******************************************/

    if (millis() - tiempoLCD >= 200)
    {

        tiempoLCD = millis();

        mostrarLCD(voltaje,
                   valorPot);

    }
}

//******************************************/
// EJECUTAR LED
//******************************************/

void ejecutarLED(uint8_t numeroLED,
                 uint16_t tiempo)
{

    //******************************************/
    // LED 1
    //******************************************/

    if (numeroLED == 1)
    {

        ultimoLED = '1';

        Serial.println("Encendiendo LED 1");

        digitalWrite(LED1, HIGH);

        delay(tiempo);

        digitalWrite(LED1, LOW);

    }


    //******************************************/
    // LED 2
    //******************************************/

    else if (numeroLED == 2)
    {

        ultimoLED = '2';

        Serial.println("Encendiendo LED 2");

        digitalWrite(LED2, HIGH);

        delay(tiempo);

        digitalWrite(LED2, LOW);

    }


    //******************************************/
    // LED 3
    //******************************************/

    else if (numeroLED == 3)
    {

        ultimoLED = '3';

        Serial.println("Encendiendo LED 3");

        digitalWrite(LED3, HIGH);

        delay(tiempo);

        digitalWrite(LED3, LOW);

    }


    Serial.println("LED apagado");
}

//******************************************/
// MOSTRAR LCD
//******************************************/

void mostrarLCD(float voltaje,
                uint8_t valorPot)
{

    //******************************************/
    // Primera fila
    //******************************************/

    lcd.setCursor(0, 0);

    lcd.print("Pot1:");

    lcd.print(voltaje, 2);

    lcd.print("V");

    lcd.print("   ");


    //******************************************/
    // Segunda fila
    //******************************************/

    lcd.setCursor(0, 1);

    lcd.print("Pot1:");

    lcd.print(valorPot);

    lcd.print("   ");


    //******************************************/
    // Mostrar ultimo LED
    //******************************************/

    lcd.setCursor(11, 1);

    lcd.print("LED:");

    lcd.print(ultimoLED);
}
void enviarMedicionI2C()
{
    uint8_t datos[2];

    datos[0] = (valorADC_I2C >> 8) & 0xFF;
    datos[1] = valorADC_I2C & 0xFF;

    Wire.write(datos, 2);
}

void solicitarMedicionI2C()
{
    valorADC_I2C = analogRead(POT1);

    enviarMedicionI2C();
}