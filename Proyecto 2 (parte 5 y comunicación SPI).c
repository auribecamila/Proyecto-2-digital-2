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
#include <stdio.h>
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t comandoUART;

uint8_t bufferUART[20];
uint8_t bufferSPI[20];

uint8_t ledSeleccionado;
uint32_t tiempoLED;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void mostrarMenu(void);
void configurarTimer(uint32_t tiempo_ms);

void mostrarMenuSPI(void);
void recibirComandoUART(void);
void procesarComandoSPI(void);
void enviarComandoSPI(uint8_t led, uint32_t tiempo_ms);

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
  MX_TIM3_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  mostrarMenu();

  //Configuración del timer
  configurarTimer(500);

  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_UART_Receive(&huart2,
                       &comandoUART,
                       1,
                       HAL_MAX_DELAY);

      if (comandoUART == '1')
      {
          mostrarMenuSPI();

          recibirComandoUART();

          procesarComandoSPI();
      }
      else if (comandoUART == '2')
      {
          char mensaje[] =
              "\r\nObtener medicion de sensor I2C\r\n";

          HAL_UART_Transmit(&huart2,
                            (uint8_t *)mensaje,
                            sizeof(mensaje) - 1,
                            HAL_MAX_DELAY);
      }
      else
      {
          char mensaje[] =
              "\r\nOpcion no valida.\r\n";

          HAL_UART_Transmit(&huart2,
                            (uint8_t *)mensaje,
                            sizeof(mensaje) - 1,
                            HAL_MAX_DELAY);
      }

      mostrarMenu();
  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 15999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
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
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_TIMER_GPIO_Port, LED_TIMER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ESP32_CS_GPIO_Port, ESP32_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_TIMER_Pin */
  GPIO_InitStruct.Pin = LED_TIMER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_TIMER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ESP32_CS_Pin */
  GPIO_InitStruct.Pin = ESP32_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ESP32_CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Menú principal
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

//Mostrar la frecuencia del timer
void configurarTimer(uint32_t tiempo_ms)
{
    __HAL_TIM_SET_AUTORELOAD(&htim3, tiempo_ms - 1);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        HAL_GPIO_TogglePin(LED_TIMER_GPIO_Port, LED_TIMER_Pin);
    }
}

// Mostrar submenu SPI

void mostrarMenuSPI(void)
{
    char mensaje[] =
        "\r\n"
        "==============================\r\n"
        "        CONTROL SPI\r\n"
        "==============================\r\n"
        "Ingrese el LED y el tiempo en ms.\r\n"
        "Formato: LED,TIEMPO\r\n"
        "Ejemplo: 1,500\r\n"
        "\r\n"
        "Comando: ";

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)mensaje,
                      sizeof(mensaje) - 1,
                      HAL_MAX_DELAY);
}

// Recibir la frecuencia del UART

void recibirComandoUART(void)
{
    uint8_t caracter;
    uint8_t indice = 0;

    while (1)
    {
        HAL_UART_Receive(&huart2,
                         &caracter,
                         1,
                         HAL_MAX_DELAY);

        if (caracter == '\r' || caracter == '\n')
        {
            bufferUART[indice] = '\0';
            break;
        }

        if (indice < sizeof(bufferUART) - 1)
        {
            bufferUART[indice] = caracter;
            indice++;
        }
    }
}

// Verificar la información enviada
void procesarComandoSPI(void)
{
    int led;
    unsigned int tiempo;

    if (sscanf((char *)bufferUART, "%d,%u", &led, &tiempo) == 2)
    {
        if (led >= 1 && led <= 3 && tiempo > 0)
        {
            ledSeleccionado = led;
            tiempoLED = tiempo;

            enviarComandoSPI(ledSeleccionado, tiempoLED);
        }
        else
        {
            char mensaje[] =
                "\r\nComando invalido.\r\n"
                "El LED debe ser 1, 2 o 3.\r\n"
                "El tiempo debe ser mayor que 0 ms.\r\n";

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)mensaje,
                              sizeof(mensaje) - 1,
                              HAL_MAX_DELAY);
        }
    }
    else
    {
        char mensaje[] =
            "\r\nFormato invalido.\r\n"
            "Use: LED,TIEMPO\r\n"
            "Ejemplo: 1,500\r\n";

        HAL_UART_Transmit(&huart2,
                          (uint8_t *)mensaje,
                          sizeof(mensaje) - 1,
                          HAL_MAX_DELAY);
    }
}

// Enviar el dato al led
void enviarComandoSPI(uint8_t led, uint32_t tiempo_ms)
{
    int longitud;

    longitud = sprintf((char *)bufferSPI,
                       "%d,%lu",
                       led,
                       tiempo_ms);

    HAL_GPIO_WritePin(ESP32_CS_GPIO_Port,
                      ESP32_CS_Pin,
                      GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1,
                     bufferSPI,
                     longitud,
                     HAL_MAX_DELAY);

    HAL_GPIO_WritePin(ESP32_CS_GPIO_Port,
                      ESP32_CS_Pin,
                      GPIO_PIN_SET);
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
