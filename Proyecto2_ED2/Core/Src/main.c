/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>   // Permite utilizar sscanf y snprintf
#include <string.h>  // Permite trabajar con cadenas de texto
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

// Guarda la opcion seleccionada por el usuario
uint8_t opcionUART;

// Guarda los caracteres recibidos del comando SPI
// Arreglo para guardar hasta 19 caracteres del comando SPI y el ultimo
char comandoSPI[20];

// Guarda el numero del LED seleccionado
int ledSPI;

// Guarda el tiempo de encendido en milisegundos
int tiempoSPI;

// Variable para guardar el valor recibido del ESP32
uint8_t valorPotI2C = 0;

// Direccion del ESP32 en el bus I2C
#define ESP32_I2C_ADDR (0x08 << 1)

// Tiempo que debe permanecer la señal en cada estado
volatile uint32_t tiempoTimer = 500;

// Cuenta los milisegundos transcurridos
volatile uint32_t contadorTimer = 0;

// Indica si la señal cuadrada esta activa
volatile uint8_t timerActivo = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

//DECLARACION DE FUNCIONES

// Funcion para mostrar el menu principal
void mostrarMenu(void);

// Funcion para mostrar el submenu de control SPI
void mostrarMenuSPI(void);

// Funcion para recibir un comando completo por UART
void recibirComandoSPI(void);

// Funcion para enviar el LED y el tiempo al ESP32 por SPI
void enviarComandoSPI(void);

// Funcion para solicitar el valor del potenciometro por I2C
void recibirDatoI2C(void);

// Actualizar el tiempo de la señal cuadrada
void actualizarTimer(uint16_t tiempo);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // Mostrar menu al iniciar
  mostrarMenu();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // Recibir un caracter por UART
      HAL_UART_Receive(&huart2,
                       &opcionUART,
                       1,
                       HAL_MAX_DELAY);

      // Ignorar los saltos de linea enviados por el monitor serial
      // para evitar que se interpreten como opciones incorrectas.
      if (opcionUART == '\r' || opcionUART == '\n')
      {
          continue;
      }

      //********************************************/
      // OPCION 1: CONTROL SPI
      //********************************************/

      if (opcionUART == '1')
      {
          // Confirmar la opcion seleccionada
          char mensaje[] = "\r\nOpcion 1 seleccionada: SPI\r\n";

          HAL_UART_Transmit(&huart2,
                            (uint8_t *)mensaje,
                            sizeof(mensaje) - 1,
                            HAL_MAX_DELAY);

          // Mostrar las instrucciones del control SPI
          mostrarMenuSPI();

          // Esperar el comando LED,TIEMPO enviado por el usuario
          recibirComandoSPI();
      }

      else if (opcionUART == '2')
      {
          char mensaje[] = "\r\nOpcion 2 seleccionada: I2C\r\n";

          HAL_UART_Transmit(&huart2,
                            (uint8_t *)mensaje,
                            sizeof(mensaje) - 1,
                            HAL_MAX_DELAY);
          // Solicitar la lectura del potenciometro al ESP32
              recibirDatoI2C();
      }
      else
      {
          char mensaje[] = "\r\nOpcion no valida.\r\n";

          HAL_UART_Transmit(&huart2,
                            (uint8_t *)mensaje,
                            sizeof(mensaje) - 1,
                            HAL_MAX_DELAY);
      }

      // Separar la respuesta del siguiente menu
      char espacio[] = "\r\n\r\n\r\n";

      HAL_UART_Transmit(&huart2,
                        (uint8_t *)espacio,
                        sizeof(espacio) - 1,
                        HAL_MAX_DELAY);

      // Volver a mostrar el menu principal
      mostrarMenu();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8399;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TIMER_LED_GPIO_Port, TIMER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TIMER_LED_Pin */
  GPIO_InitStruct.Pin = TIMER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TIMER_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


// Mostrar menu principal por UART
void mostrarMenu(void)
{
    char mensaje[] =
        "\r\n"
        "==============================\r\n"
        "       MENU PRINCIPAL\r\n"
        "==============================\r\n"
        "1. Controlar dispositivo SPI\r\n"
        "2. Obtener medicion de sensor I2C\r\n"
        "\r\n"
        "Seleccione una opcion: ";

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)mensaje,
                      sizeof(mensaje) - 1,
                      HAL_MAX_DELAY);
}

// Mostrar submenu SPI
void mostrarMenuSPI(void)
{
    // Mensaje que se mostrara en el monitor serial
    char mensaje[] =
        "\r\n"
        "==============================\r\n"
        "         CONTROL SPI\r\n"
        "==============================\r\n"
        "Ingrese el LED y tiempo en ms.\r\n"
        "Formato: LED,TIEMPO\r\n"
        "Ejemplo: 1,500\r\n"
        "\r\n"
        "Comando: ";

    // Enviar el submenu a la computadora por UART
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)mensaje,
                      sizeof(mensaje) - 1,
                      HAL_MAX_DELAY);
}
    //********************************************/
    // RECEPCION DEL COMANDO SPI ---------------
    //********************************************/

    // Recibe el comando LED,TIEMPO por UART
    // Separa el numero del LED y su tiempo.

    void recibirComandoSPI(void)
    {
        uint8_t dato;
        uint8_t indice = 0;

        // Recibir caracteres hasta que el usuario presione ENTER
        while (1)
        {
            HAL_UART_Receive(&huart2, &dato, 1, HAL_MAX_DELAY);

            // Ignorar ENTER si todavia no se ha recibido ningun caracter
            if ((dato == '\r' || dato == '\n') && indice == 0)
            {
                continue;
            }

            // Terminar la recepcion cuando se presione ENTER
            if (dato == '\r' || dato == '\n')
            {
                break;
            }

            // Guardar los caracteres sin superar el espacio disponible
            if (indice < sizeof(comandoSPI) - 1)
            {
                comandoSPI[indice] = dato;
                indice++;
            }
        }

        // Agregar el caracter que indica el final del texto
        comandoSPI[indice] = '\0';

        // Separar el LED y el tiempo recibidos
        char extra;
        int datos = sscanf(comandoSPI, "%d,%d %c",
                           &ledSPI, &tiempoSPI, &extra);

        // Verificar que el comando tenga el formato correcto
        if (datos == 2 &&
            ledSPI >= 1 && ledSPI <= 3 &&
            tiempoSPI > 0 && tiempoSPI <= 65535)
        {
            char respuesta[100];

            // Mostrar los valores que recibio la Nucleo
            snprintf(respuesta, sizeof(respuesta),
                     "\r\nLED seleccionado: %d\r\n"
                     "Tiempo: %d ms\r\n",
                     ledSPI, tiempoSPI);

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)respuesta,
                              strlen(respuesta),
                              HAL_MAX_DELAY);


            // Enviar el LED y el tiempo al ESP32 mediante SPI - SOLO SI ES VALIDO
            enviarComandoSPI();

            // Actualizar la señal cuadrada usando el tiempo recibido
            actualizarTimer((uint16_t)tiempoSPI);
        }
        else
        {
            // Avisar cuando el comando no sea valido
            char error[] = "\r\nComando no valido. Ejemplo: 1,500\r\n";

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)error,
                              sizeof(error) - 1,
                              HAL_MAX_DELAY);

        }
    }

        //********************************************/
        // ENVIO DE DATOS POR SPI
        //********************************************/

        // Funcion para enviar el numero del LED y el tiempo
        //  Nucleo hacia el ESP32
        void enviarComandoSPI(void)
        {
            // 3 bytes para guardar los datos a transmitir
            uint8_t datosSPI[3];

            // Primer byte: numero del LED seleccionado
            datosSPI[0] = (uint8_t)ledSPI;

            // Segundo byte: parte alta del tiempo
            datosSPI[1] = (uint8_t)(tiempoSPI >> 8);

            // Tercer byte: parte baja del tiempo
            datosSPI[2] = (uint8_t)(tiempoSPI & 0xFF);

            // Colocar CS en LOW para seleccionar al ESP32
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

            // Enviar los 3 bytes por SPI1
            HAL_StatusTypeDef estado = HAL_SPI_Transmit(
                &hspi1,
                datosSPI,
                3,
                1000
            );

            // Colocar CS en HIGH para finalizar la comunicacion
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

            // Comprobar si la transmision se completo correctamente
            if (estado == HAL_OK)
            {
                char mensaje[] = "\r\nTransmision SPI completada.\r\n";

                HAL_UART_Transmit(&huart2,
                                  (uint8_t *)mensaje,
                                  sizeof(mensaje) - 1,
                                  HAL_MAX_DELAY);
            }
            else
            {
                // Informar si ocurrio un error durante la transmision
                char mensaje[] = "\r\nError en la transmision SPI.\r\n";

                HAL_UART_Transmit(&huart2,
                                  (uint8_t *)mensaje,
                                  sizeof(mensaje) - 1,
                                  HAL_MAX_DELAY);
            }
        }
            //******************************************/
            // RECIBIR DATO POR I2C
            //******************************************/

            void recibirDatoI2C(void)
            {
                // Solicitar un byte al ESP32 por I2C
                HAL_StatusTypeDef estado = HAL_I2C_Master_Receive( //hace que la Nucleo solicite un byte al ESP32
                    &hi2c1,
                    ESP32_I2C_ADDR,
                    &valorPotI2C, //indica dónde guardar el dato recibido.
                    1, //1 byte
                    100 //tiempo máximo de espera, en milisegundos.
                );

                // Verificar si la comunicacion fue correcta
                if (estado == HAL_OK)
                {
                    // Mostrar el valor recibido por UART
                    char mensaje[50];

                    sprintf(mensaje, "\r\nPotenciometro: %u\r\n", valorPotI2C);

                    HAL_UART_Transmit(&huart2,
                                      (uint8_t *)mensaje,
                                      strlen(mensaje),
                                      HAL_MAX_DELAY);
                }
                else
                {
                    // Mostrar mensaje si falla la comunicacion
                    char mensaje[] = "\r\nError de comunicacion I2C\r\n";

                    HAL_UART_Transmit(&huart2,
                                      (uint8_t *)mensaje,
                                      strlen(mensaje),
                                      HAL_MAX_DELAY);
                }

            }


                // Actualizar el tiempo de la señal cuadrada
                void actualizarTimer(uint16_t tiempo)
                {
                    // Detener TIM3 antes de cambiar el tiempo
                    HAL_TIM_Base_Stop_IT(&htim3);

                    // Guardar el tiempo recibido de Watchport
                    tiempoTimer = tiempo;

                    // Reiniciar el contador
                    contadorTimer = 0;

                    // Comenzar la señal en HIGH
                    HAL_GPIO_WritePin(TIMER_LED_GPIO_Port,
                                      TIMER_LED_Pin,
                                      GPIO_PIN_SET);

                    // Reiniciar el contador interno de TIM3
                    __HAL_TIM_SET_COUNTER(&htim3, 0);

                    // Limpiar interrupciones pendientes
                    __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);

                    // Activar la señal
                    timerActivo = 1;

                    // Iniciar TIM3 con interrupciones
                    HAL_TIM_Base_Start_IT(&htim3);
                }



                // Interrupcion que ocurre cada 1 ms
                void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
                {
                    // Verificar que sea TIM3
                    if (htim->Instance == TIM3)
                    {
                        if (timerActivo == 1)
                        {
                            // Aumentar el contador de milisegundos
                            contadorTimer++;

                            // Verificar si ya paso el tiempo indicado
                            if (contadorTimer >= tiempoTimer)
                            {
                                // Reiniciar el contador
                                contadorTimer = 0;

                                // Cambiar el estado de PA8
                                HAL_GPIO_TogglePin(TIMER_LED_GPIO_Port,
                                                   TIMER_LED_Pin);
                            }
                        }
                    }
                }




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
