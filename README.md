
# Proyecto 2 – Protocolos de Comunicación Serial y Timers

**Universidad del Valle de Guatemala**  
**BE3029 – Electrónica Digital 2**

**Integrantes:**
- Stephany Mejía – 24037
- Camila Aristizábal – 24367

## Descripción

En este proyecto se implementó un sistema de comunicación entre una computadora, una NUCLEO-F446RE y un ESP32 mediante los protocolos UART, SPI e I2C.

La Nucleo se utilizó como controlador principal, mientras que el ESP32 se encargó del control de tres LEDs, la lectura de un potenciómetro y la visualización de datos en una pantalla LCD 16×2.

También se trabajó con un temporizador para generar una señal cuadrada. El circuito se integró en una maqueta de un tractor con remolque, simulando un sistema de control y monitoreo de maquinaria agrícola.

## Objetivos

1. Implementar los protocolos UART, SPI e I2C para establecer comunicación entre la computadora, la Nucleo y el ESP32.
2. Controlar LEDs, adquirir datos de un potenciómetro mediante ADC y mostrar la información en una pantalla LCD.
3. Configurar un temporizador para generar una señal cuadrada sin detener las demás funciones del sistema.

## Materiales

- NUCLEO-F446RE
- ESP32 DevKit V1
- Pantalla LCD 16×2
- Potenciómetros
- 4 LEDs y resistencias
- Protoboard y jumpers
- Computadora y cables USB

## Funcionamiento

### 1. Comunicación UART

Se utilizó USART2 a 115200 baudios para establecer comunicación entre la computadora y la Nucleo.

Al iniciar el programa, se mostraba el siguiente menú:

    1. Controlar dispositivo SPI
    2. Obtener medicion de sensor I2C

La Nucleo recibía los comandos mediante UART. Se utilizó `\r` para identificar el final de cada instrucción y `sscanf()` para separar y validar los valores ingresados.

### 2. Comunicación SPI

Se configuró SPI1 en modo maestro, Full-Duplex, con datos de 8 bits y modo SPI 0.

La Nucleo enviaba al ESP32 el número del LED y el tiempo que debía permanecer encendido.

Por ejemplo, el comando `1,500` indicaba que el LED 1 debía encenderse durante 500 ms.

La información se transmitía mediante tres bytes: número del LED, parte alta y parte baja del tiempo. El ESP32 reconstruía el valor recibido y utilizaba `millis()` para controlar el apagado sin bloquear el programa.

### 3. Comunicación I2C y ADC

Se configuró I2C1 a 100 kHz, con direccionamiento de 7 bits.

La Nucleo funcionaba como maestro y solicitaba al ESP32, cuya dirección era `0x08`, la medición del potenciómetro.

El ESP32 utilizaba su ADC de 12 bits para obtener valores entre 0 y 4095. Posteriormente, la medición se convertía a un rango de 0 a 255 para transmitirla mediante un byte por I2C.

La Nucleo recibía el dato y lo mostraba en la terminal serial.

### 4. Pantalla LCD

Se utilizó una pantalla LCD 16×2 conectada al ESP32 en modo de 4 bits.

La pantalla mostraba el voltaje del potenciómetro, el valor digital del ADC y la inicial del último LED activado.

### 5. Temporizadores

Se trabajó con TIM3 de la Nucleo para generar una señal cuadrada.

Se configuró un prescaler de 8399 y un período de 4999, con un reloj de 84 MHz, lo que permitía obtener un evento cada 500 ms.

Para el comando `1,500`, se estableció como objetivo que la señal permaneciera 500 ms en nivel alto y 500 ms en nivel bajo, obteniendo un período total de 1000 ms.

También se buscó que el tiempo pudiera actualizarse según el comando ingresado, sin detener las demás comunicaciones del sistema.

## Pines utilizados

### SPI – Nucleo y ESP32

| Nucleo | ESP32 | Función |
|--------|-------|---------|
| PA5 | GPIO18 | SCK |
| PA6 | GPIO19 | MISO |
| PA7 | GPIO23 | MOSI |
| PB6 | GPIO5 | CS |

### I2C – Nucleo y ESP32

| Nucleo | ESP32 | Función |
|--------|-------|---------|
| PB8 | GPIO17 | SCL |
| PB9 | GPIO16 | SDA |

### LEDs y potenciómetro

| Componente | ESP32 |
|------------|-------|
| LED rojo | GPIO4 |
| LED verde | GPIO15 |
| LED azul | GPIO33 |
| Potenciómetro | GPIO34 |

### LCD 16×2

| LCD | ESP32 |
|-----|-------|
| RS | GPIO13 |
| E | GPIO14 |
| D4 | GPIO21 |
| D5 | GPIO22 |
| D6 | GPIO27 |
| D7 | GPIO32 |
| RW | GND |

Ambos microcontroladores compartieron GND. Las líneas I2C utilizaron resistencias pull-up hacia 3.3 V.

## Librerías utilizadas

### NUCLEO-F446RE – STM32 HAL

Se utilizaron las siguientes funciones de HAL:

- `HAL_UART_Transmit()` y `HAL_UART_Receive_IT()`: permitieron realizar la comunicación UART.
- `HAL_SPI_Transmit()`: permitió enviar datos mediante SPI.
- `HAL_I2C_Master_Receive()`: permitió recibir datos mediante I2C.
- `HAL_GPIO_WritePin()`: permitió controlar salidas digitales.

### ESP32 – PlatformIO

- `Arduino.h`: proporcionó las funciones generales del ESP32.
- `stdint.h`: permitió trabajar con tipos de datos enteros.
- `LiquidCrystal.h`: se utilizó para controlar la pantalla LCD.
- `ESP32SPISlave.h`: permitió configurar la comunicación SPI como esclavo.
- `Wire.h`: se utilizó para la comunicación I2C.

También se utilizaron `analogRead()` para adquirir la lectura del potenciómetro y `millis()` para controlar los tiempos de los LEDs sin detener el programa.

## Resultados y conclusiones

En este proyecto se integraron los protocolos UART, SPI e I2C para establecer comunicación entre dos microcontroladores y una computadora.

Mediante SPI se realizó el control de los LEDs, mientras que I2C permitió solicitar la medición del potenciómetro. Además, se utilizó el ADC para convertir la señal analógica en un valor digital y una pantalla LCD para visualizar la información.

También se trabajó con la configuración de temporizadores y se comprendió la importancia de generar señales periódicas sin interrumpir las demás funciones del sistema.

El proyecto permitió aplicar los conocimientos adquiridos durante el curso y comprender cómo diferentes protocolos y periféricos podían integrarse dentro de un mismo sistema.

## Evidencias

**Montaje del circuito y maqueta:**  
Agregar fotografías.

**Diagrama de flujo:**  
Agregar imagen.

**Video de funcionamiento:**  
Agregar enlace de YouTube.

---
\
