
# Proyecto 2 – Protocolos de Comunicación Serial y Timers

**Universidad del Valle de Guatemala**  
**BE3029 – Electrónica Digital 2**

**Integrantes:**
- Stephany Mejía – 24037
- Camila Aristizábal – 24367

## Descripción

El proyecto consiste en implementar un sistema de comunicación entre una computadora, una NUCLEO-F446RE y un ESP32 mediante los protocolos UART, SPI e I2C.

La Nucleo funciona como controlador principal, mientras que el ESP32 se encarga de controlar tres LEDs, leer un potenciómetro y mostrar información en una pantalla LCD 16×2.

Además, se trabaja con un temporizador para generar una señal cuadrada. El circuito se integra en una maqueta de un tractor con remolque, simulando el control y monitoreo de maquinaria agrícola.

## Objetivos

1. Implementar los protocolos UART, SPI e I2C para establecer comunicación entre la computadora, la Nucleo y el ESP32.
2. Controlar LEDs, adquirir datos de un potenciómetro mediante ADC y mostrar la información en una pantalla LCD.
3. Configurar temporizadores para generar una señal cuadrada sin detener las demás funciones del sistema.

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

Se utiliza USART2 a 115200 baudios para establecer comunicación entre la computadora y la Nucleo.

Al iniciar el programa, se muestra el siguiente menú:

    1. Controlar dispositivo SPI
    2. Obtener medicion de sensor I2C

La Nucleo recibe los comandos mediante UART. Se utiliza `\r` para identificar el final de cada instrucción y `sscanf()` para separar y validar los valores ingresados.

### 2. Comunicación SPI

Se configura SPI1 en modo maestro, Full-Duplex, con datos de 8 bits y modo SPI 0.

La Nucleo envía al ESP32 el número del LED y el tiempo que debe permanecer encendido.

Por ejemplo, el comando `1,500` indica que el LED 1 debe encenderse durante 500 ms.

La información se transmite mediante tres bytes: número del LED, parte alta y parte baja del tiempo. El ESP32 reconstruye el valor recibido y utiliza `millis()` para controlar el apagado sin bloquear el programa.

### 3. Comunicación I2C y ADC

Se configura I2C1 a 100 kHz, con direccionamiento de 7 bits.

La Nucleo funciona como maestro y solicita al ESP32, cuya dirección es `0x08`, la medición del potenciómetro.

El ESP32 utiliza su ADC de 12 bits para obtener valores entre 0 y 4095. Posteriormente, la medición se convierte a un rango de 0 a 255 para transmitirla mediante un byte por I2C.

La Nucleo recibe el dato y lo muestra en la terminal serial.

### 4. Pantalla LCD

Se utiliza una pantalla LCD 16×2 conectada al ESP32 en modo de 4 bits.

La pantalla permite visualizar el voltaje del potenciómetro, el valor digital del ADC y la inicial del último LED activado.

### 5. Temporizadores

Se trabaja con TIM3 de la Nucleo para generar una señal cuadrada.

La configuración de prescaler 8399 y período 4999, con un reloj de 84 MHz, permite obtener un evento cada 500 ms.

Para el comando `1,500`, la señal debe permanecer 500 ms en nivel alto y 500 ms en nivel bajo, obteniendo un período total de 1000 ms.

El objetivo es que el tiempo pueda actualizarse según el comando ingresado, sin detener las demás comunicaciones del sistema.

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

Ambos microcontroladores comparten GND. Las líneas I2C utilizan resistencias pull-up hacia 3.3 V.

## Librerías utilizadas

### NUCLEO-F446RE – STM32 HAL

Se utilizan las funciones de la librería HAL para controlar los diferentes periféricos:

- `HAL_UART_Transmit()` y `HAL_UART_Receive_IT()`: comunicación UART.
- `HAL_SPI_Transmit()`: envío de datos mediante SPI.
- `HAL_I2C_Master_Receive()`: recepción de datos mediante I2C.
- `HAL_GPIO_WritePin()`: control de salidas digitales.

### ESP32 – PlatformIO

- `Arduino.h`: funciones generales del ESP32.
- `stdint.h`: manejo de tipos de datos.
- `LiquidCrystal.h`: control de la pantalla LCD.
- `ESP32SPISlave.h`: comunicación SPI como esclavo.
- `Wire.h`: comunicación I2C.

También se utilizan `analogRead()` para adquirir la lectura del potenciómetro y `millis()` para controlar los tiempos de los LEDs sin detener el programa.

## Resultados y conclusiones

El proyecto permite integrar los protocolos UART, SPI e I2C para establecer comunicación entre dos microcontroladores y una computadora.

Mediante SPI se realiza el control de los LEDs, mientras que I2C permite solicitar la medición del potenciómetro. Además, el ADC convierte la señal analógica en un valor digital y la pantalla LCD permite visualizar la información.

También se trabaja con la configuración de temporizadores para generar señales periódicas sin interrumpir las demás funciones.

De esta manera, se demuestra cómo diferentes protocolos y periféricos pueden integrarse dentro de un mismo sistema.

## Evidencias

**Montaje del circuito y maqueta:**  
Agregar fotografías.

**Diagrama de flujo:**  
Agregar imagen.

**Video de funcionamiento:**  
Agregar enlace de YouTube.

---
