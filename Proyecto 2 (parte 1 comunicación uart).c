/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* Variable para guardar el caracter recibido */
uint8_t comandoUART;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */

void mostrarMenu(void);

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

  /* USER CODE BEGIN 2 */

  /* Mostrar el menu principal una sola vez */
  mostrarMenu();

  /* Activar recepcion UART por interrupcion */
  HAL_UART_Receive_IT(&huart2, &comandoUART, 1);

  /* USER CODE END 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
      /* UART trabaja mediante interrupciones */
  }

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

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

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Configure voltage scaling */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /* Configure oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure CPU and bus clocks */
  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK |
      RCC_CLOCKTYPE_SYSCLK |
      RCC_CLOCKTYPE_PCLK1 |
      RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                          FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}


/* USER CODE BEGIN 4 */


/* =========================================================
                    MENU PRINCIPAL
   ========================================================= */

void mostrarMenu(void)
{
    char mensaje[] =
        "\r\n\r\n"
        "==============================\r\n"
        "       MENU PRINCIPAL\r\n"
        "==============================\r\n"
        "\r\n"
        "1. Controlar dispositivo SPI\r\n"
        "2. Obtener medicion de sensor I2C\r\n"
        "\r\n"
        "Seleccione una opcion: ";

    HAL_UART_Transmit(&huart2,
                      (uint8_t *)mensaje,
                      sizeof(mensaje) - 1,
                      HAL_MAX_DELAY);
}


/* =========================================================
              INTERRUPCION DE RECEPCION UART
   ========================================================= */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    /* Verificar que la interrupcion sea de USART2 */
    if (huart->Instance == USART2)
    {

        /* =================================================
                       OPCION 1 - SPI
           ================================================= */

        if (comandoUART == '1')
        {
            char mensaje[] =
                "\r\n\r\n"
                "Opcion 1 seleccionada: SPI\r\n"
                "\r\n";

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)mensaje,
                              sizeof(mensaje) - 1,
                              HAL_MAX_DELAY);

            /* Por ahora regresar al menu */
            mostrarMenu();
        }


        /* =================================================
                       OPCION 2 - I2C
           ================================================= */

        else if (comandoUART == '2')
        {
            char mensaje[] =
                "\r\n\r\n"
                "Opcion 2 seleccionada: I2C\r\n"
                "\r\n";

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)mensaje,
                              sizeof(mensaje) - 1,
                              HAL_MAX_DELAY);

            /* Por ahora regresar al menu */
            mostrarMenu();
        }


        /* =================================================
                         IGNORAR ENTER
           ================================================= */

        else if ((comandoUART == '\r') ||
                 (comandoUART == '\n'))
        {
            /* No hacer nada */
        }


        /* =================================================
                       OPCION NO VALIDA
           ================================================= */

        else
        {
            char mensaje[] =
                "\r\n\r\n"
                "Opcion no valida.\r\n"
                "\r\n";

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)mensaje,
                              sizeof(mensaje) - 1,
                              HAL_MAX_DELAY);

            mostrarMenu();
        }


        /* =================================================
              VOLVER A ACTIVAR RECEPCION UART
           ================================================= */

        HAL_UART_Receive_IT(&huart2,
                            &comandoUART,
                            1);
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

  __disable_irq();

  while (1)
  {
  }

  /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
  * @brief Reports the name of the source file and
  *        the source line number where assert_param
  *        error has occurred.
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  /* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */
