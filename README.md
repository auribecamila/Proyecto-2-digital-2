# Proyecto 2 – Protocolos de Comunicación Serial y Timers

**Universidad del Valle de Guatemala**  
**BE3029 – Electrónica Digital 2**

**Integrantes:**
- Stephany Mejía – 24037
- Camila Aristizábal – 24367

## Descripción

En este proyecto implementamos la comunicación entre una computadora, una NUCLEO-F446RE y un ESP32 mediante los protocolos UART, SPI e I2C.

La Nucleo funciona como maestro y controlador principal, mientras que el ESP32 se encarga del control de tres LEDs, la lectura de un potenciómetro y la visualización de datos en una pantalla LCD 16×2.

También trabajamos con un temporizador para generar una señal cuadrada. El circuito se integró en una maqueta de un tractor con remolque.

## Objetivos

1. Implementar UART, SPI e I2C para comunicar la computadora, la Nucleo y el ESP32.
2. Controlar LEDs, leer un potenciómetro mediante ADC y mostrar los datos en una LCD.
3. Utilizar temporizadores para generar una señal cuadrada sin detener las demás funciones.

## Materiales

- NUCLEO-F446RE
- ESP32 DevKit V1
- Pantalla LCD 16×2
- Potenciómetros
- 4 LEDs y resistencias
- Protoboard y jumpers
- Computadora y cables USB

## Funcionamiento

### 1. UART

Utilizamos USART2 a 115200 baudios para comunicar la computadora con la Nucleo.

Al iniciar el programa aparece un menú:

    1. Controlar dispositivo SPI
    2. Obtener medicion de sensor I2C

La Nucleo recibe los comandos mediante UART. Utilizamos `\r` para identificar el final de cada instrucción y `sscanf()` para separar y validar los valores.

### 2. SPI

Configuramos SPI1 en modo maestro, Full-Duplex, con datos de 8 bits y modo SPI 0.

La Nucleo envía al ESP32 el número del LED y el tiempo que debe permanecer encendido.

Por ejemplo, `1,500` indica que el LED 1 debe encenderse durante 500 ms.

El comando se envía en tres bytes: número del LED, parte alta y parte baja del tiempo. En el ESP32 se reconstruye el tiempo y se utiliza `millis()` para controlar el apagado sin bloquear el programa.

### 3. I2C y ADC

Configuramos I2C1 a 100 kHz, con direccionamiento de 7 bits.

La Nucleo funciona como maestro y solicita al ESP32, cuya dirección es `0x08`, la medición del potenciómetro.

El ESP32 utiliza su ADC de 12 bits para obtener un valor entre 0 y 4095. Luego convierte la medición a un rango de 0 a 255 para enviarla mediante I2C.

### 4. Pantalla LCD

Utilizamos una LCD 16×2 conectada al ESP32 en modo de 4 bits.

La pantalla muestra el voltaje del potenciómetro, la lectura digital del ADC y la inicial del último LED activado.

### 5. Timers

Trabajamos con TIM3 de la Nucleo para generar una señal cuadrada.

La configuración de prescaler 8399 y período 4999, con un reloj de 84 MHz, permite obtener un evento cada 500 ms.

Para el comando `1,500`, la señal debe permanecer 500 ms en alto y 500 ms en bajo. El objetivo es actualizar ese tiempo según el comando ingresado y mantener funcionando las demás comunicaciones.

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

Todas las conexiones entre microcontroladores comparten GND. Las líneas I2C utilizan resistencias pull-up hacia 3.3 V.

## Librerías utilizadas

### NUCLEO-F446RE – STM32 HAL

- `HAL_UART_Transmit()` y `HAL_UART_Receive_IT()` para UART.
- `HAL_SPI_Transmit()` para SPI.
- `HAL_I2C_Master_Receive()` para I2C.
- `HAL_GPIO_WritePin()` para controlar salidas.

### ESP32 – PlatformIO

- `Arduino.h`: funciones generales.
- `stdint.h`: tipos de datos.
- `LiquidCrystal.h`: pantalla LCD.
- `ESP32SPISlave.h`: comunicación SPI.
- `Wire.h`: comunicación I2C.

También utilizamos `analogRead()` para leer el potenciómetro y `millis()` para manejar los tiempos de los LEDs.

## Resultados y conclusiones

Con este proyecto integramos UART, SPI e I2C para comunicar dos microcontroladores y una computadora. Logramos trabajar con el control de LEDs, la adquisición de datos analógicos y la visualización de información en una pantalla LCD.

Además, estudiamos la configuración de temporizadores para generar señales periódicas sin detener las demás funciones del sistema.
