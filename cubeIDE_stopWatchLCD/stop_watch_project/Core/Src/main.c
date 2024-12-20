/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"

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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

WWDG_HandleTypeDef hwwdg;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for read_UART */
osThreadId_t read_UARTHandle;
const osThreadAttr_t read_UART_attributes = {
  .name = "read_UART",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for write_UART */
osThreadId_t write_UARTHandle;
const osThreadAttr_t write_UART_attributes = {
  .name = "write_UART",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for count_stopwatch */
osThreadId_t count_stopwatchHandle;
const osThreadAttr_t count_stopwatch_attributes = {
  .name = "count_stopwatch",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for WatchDogChecks */
osThreadId_t WatchDogChecksHandle;
const osThreadAttr_t WatchDogChecks_attributes = {
  .name = "WatchDogChecks",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* USER CODE BEGIN PV */



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM2_Init(void);
static void MX_WWDG_Init(void);
void StartDefaultTask(void *argument);
void StartTaskReadUART(void *argument);
void StartTaskWriteUART(void *argument);
void StartTaskCountStopwatch(void *argument);
void StartTaskStartTaskWatchDogChecks(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUFFER_SIZE 12
uint16_t UART1_BufferSize = BUFFER_SIZE;
uint8_t UART1_rxBuffer[BUFFER_SIZE] = {0};
uint8_t UART1_txBuffer[BUFFER_SIZE] = {0};
uint8_t UART1_debugBuffer[BUFFER_SIZE] = {0};

uint8_t rxData; // Variable to store received data
uint8_t rxIndex = 0; // Index for buffer


// Global time variables
volatile uint32_t milliseconds = 0;
volatile uint32_t seconds = 0;
volatile uint32_t minutes = 0;
volatile uint32_t hours = 0;
volatile uint32_t prev_milliseconds = 0;

#define START 1
#define STOP 0
#define RESET 1
#define NOT_RESET 0

volatile uint32_t reset_flag = NOT_RESET;  // 0 = not reset, 1 = reset
volatile uint32_t start_stop_flag = STOP;  // 0 = stop, 1 = start

// Define task handles and watchdog status flags
#define NUM_TASKS 1  // Number of tasks to monitor

volatile uint32_t taskAliveFlags[NUM_TASKS] = {0};

// Task IDs (to identify them in the watchdog)
typedef enum {
    TASK_count_stopwatch_ID = 0
} TaskID_t;
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM16_Init();
  MX_TIM2_Init();
  MX_WWDG_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim16);
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
//  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
//
//  /* creation of read_UART */
//  read_UARTHandle = osThreadNew(StartTaskReadUART, NULL, &read_UART_attributes);
//
//  /* creation of write_UART */
//  write_UARTHandle = osThreadNew(StartTaskWriteUART, NULL, &write_UART_attributes);

  /* creation of count_stopwatch */
  count_stopwatchHandle = osThreadNew(StartTaskCountStopwatch, (void*)TASK_count_stopwatch_ID, &count_stopwatch_attributes);

  /* creation of WatchDogChecks */
  WatchDogChecksHandle = osThreadNew(StartTaskStartTaskWatchDogChecks, NULL, &WatchDogChecks_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 3999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 80-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65536-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */

  /* USER CODE END WWDG_Init 0 */

  /* USER CODE BEGIN WWDG_Init 1 */

  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = 95;
  hwwdg.Init.Counter = 127;
  hwwdg.Init.EWIMode = WWDG_EWI_ENABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  HAL_UART_Receive_DMA(&huart1, UART1_rxBuffer, UART1_BufferSize); //You need to toggle a breakpoint on this line!
//  HAL_UART_Transmit_DMA(&huart1, UART1_rxBuffer, UART1_BufferSize);
//}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if (htim == &htim2 && reset_flag == NOT_RESET)
	{
		milliseconds++;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t last_tick = 0;
	uint32_t current_tick = HAL_GetTick();
	uint32_t debouncing_delay = 100;  // in milliseconds

	if (GPIO_Pin == GPIO_PIN_3 && (current_tick - last_tick) > debouncing_delay)
	{
		// reset stopwatch
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET) {  // Confirm button is still pressed
        	reset_flag = RESET;  // Set reset_flag when pin goes high (rising edge)
        	start_stop_flag = STOP;
		}
        else
        {
        	reset_flag = NOT_RESET;  // Clear reset_flag when pin goes low (falling edge)
        }
	}
	else if (GPIO_Pin == GPIO_PIN_4 && (current_tick - last_tick) > debouncing_delay)
	{
		// start or stop stopwatch
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET) {  // Confirm button is still pressed
        	start_stop_flag = !start_stop_flag;
        }
	}

//	if ((GPIO_Pin == GPIO_PIN_4 || GPIO_Pin == GPIO_PIN_3) && (current_tick - last_tick) > debouncing_delay)
//	{
//		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
//	}

}

// Helper function to format and send time over UART
void SendTimeUART(uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t milliseconds) {
    char uart_buf[50];
    uint16_t uart_buf_len = sprintf(uart_buf, "%02d:%02d:%02d:%03d h:m:s:ms\r\n", hours, minutes, seconds, milliseconds);
    HAL_UART_Transmit(&huart2, (uint8_t *)uart_buf, uart_buf_len, 100);
}

// Helper function to increment time components
void UpdateTime(void) {
    if (milliseconds >= 999) {
        seconds++;
        milliseconds = 0;
//        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); // Toggle LED every second
    }
    if (seconds > 59) {
        minutes++;
        seconds = 0;
    }
    if (minutes > 59) {
        hours++;
        minutes = 0;
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
//	  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
//	  osDelay(500);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskReadUART */
/**
* @brief Function implementing the read_UART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskReadUART */
void StartTaskReadUART(void *argument)
{
  /* USER CODE BEGIN StartTaskReadUART */
//	HAL_UART_Receive_DMA(&huart1, UART1_rxBuffer, UART1_BufferSize);
// Check if data is available to read
//	if (HAL_UART_Receive(&huart1, &rxData, 1, HAL_MAX_DELAY) == HAL_OK)
//	{
//		// Store received data in buffer
//		UART1_rxBuffer[rxIndex] = rxData;
//		rxIndex++;
//
//		// Check for end of data or buffer overflow
//		if (rxData == '\n' || rxIndex >= UART1_BufferSize)
//		{
//			// Process received data
//			// Example: Print received data
//			HAL_UART_Transmit(&huart1, UART1_rxBuffer, rxIndex, HAL_MAX_DELAY);
//
//			// Reset buffer and index for next reception
//			rxIndex = 0;
//		}
//	}
//	uint32_t UART1_timeout = 100;
//	/* Infinite loop */
	for(;;)
	{
//	  HAL_UART_Receive(&huart1, UART1_rxBuffer, UART1_BufferSize, UART1_timeout);
//	  osDelay(1000);
//	  HAL_UART_Transmit(&huart1, UART1_rxBuffer, UART1_BufferSize, UART1_timeout);
	}
  /* USER CODE END StartTaskReadUART */
}

/* USER CODE BEGIN Header_StartTaskWriteUART */
/**
* @brief Function implementing the write_UART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskWriteUART */
void StartTaskWriteUART(void *argument)
{
  /* USER CODE BEGIN StartTaskWriteUART */
//	uint32_t UART1_timeout = 100;

//	int A = 65;  // A in ASCII code
//
//	for (int i = 0; i < UART1_BufferSize; i++)
//	{
//		UART1_txBuffer[i] = A + i;
//	}
//
//	for (int i = 0; i < UART1_BufferSize; i++)
//	{
//		UART1_debugBuffer[i] = 90 - i;
//	}


//	HAL_UART_Transmit(&huart1, UART1_txBuffer, UART1_BufferSize, UART1_timeout);

  /* Infinite loop */
  for(;;)
  {
//	  HAL_UART_Transmit(&huart1, UART1_txBuffer, UART1_BufferSize, UART1_timeout);
//	  HAL_UART_Receive(&huart1, UART1_rxBuffer, UART1_BufferSize, UART1_timeout);
//	  HAL_UART_Transmit(&huart2, UART1_rxBuffer, UART1_BufferSize, UART1_timeout);
//	  osDelay(1000);
//	  HAL_UART_Transmit(&huart1, UART1_rxBuffer, UART1_BufferSize, UART1_timeout);
  }
  /* USER CODE END StartTaskWriteUART */
}

/* USER CODE BEGIN Header_StartTaskCountStopwatch */
/**
* @brief Function implementing the count_stopwatch thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskCountStopwatch */
void StartTaskCountStopwatch(void *argument)
{
  /* USER CODE BEGIN StartTaskCountStopwatch */
    /* Infinite loop */
    TaskID_t taskId = (TaskID_t)argument;
    uint32_t reset_flag_prev = reset_flag;
    uint32_t start_stop_flag_prev = start_stop_flag;
    for (;;) {
    	taskAliveFlags[taskId] = 1;  // Signal task is alive
        // If stopwatch is running and not reset
        if ((start_stop_flag == START) && (reset_flag == NOT_RESET)) {
            if (prev_milliseconds != milliseconds) {
                SendTimeUART(hours, minutes, seconds, milliseconds);
                prev_milliseconds = milliseconds;
            }
            UpdateTime();
            osDelay(1); // Delay 1 ms to avoid tight loop
        }

        // If stopwatch is stopped but not reset
        else if ((start_stop_flag == STOP) && (start_stop_flag_prev != start_stop_flag) && (reset_flag == NOT_RESET)) {
            SendTimeUART(hours, minutes, seconds, milliseconds);
            osDelay(100); // Update display once every second when stopped
        }

        // If reset is triggered
        else if (reset_flag == RESET && reset_flag != reset_flag_prev) {
            milliseconds = 0;
            prev_milliseconds = 0;
            seconds = 0;
            minutes = 0;
            hours = 0;
            SendTimeUART(hours, minutes, seconds, milliseconds);
			osDelay(100); // Update display once every second when reset
        }
    }
  /* USER CODE END StartTaskCountStopwatch */
}

/* USER CODE BEGIN Header_StartTaskStartTaskWatchDogChecks */
/**
* @brief Function implementing the WatchDogChecks thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskStartTaskWatchDogChecks */
void StartTaskStartTaskWatchDogChecks(void *argument)
{
  /* USER CODE BEGIN StartTaskStartTaskWatchDogChecks */
  /* Infinite loop */
  for(;;)
  {
	osDelay(900);

	uint32_t allTasksAlive = 1;
    for (int i = 0; i < NUM_TASKS; i++) {
        if (taskAliveFlags[i] == 0) {
            allTasksAlive = 0;  // Task did not signal within time
            break;
        }
    }

    if (allTasksAlive) {
    	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_WWDG_Refresh(&hwwdg);
    } //else {
        // Handle watchdog timeout: log error, reset system, etc.
        // resetSystem(); or print error message
    //}

    // Clear task flags for next check
    for (int i = 0; i < NUM_TASKS; i++) {
        taskAliveFlags[i] = 0;
    }

  }
  /* USER CODE END StartTaskStartTaskWatchDogChecks */
}

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

#ifdef  USE_FULL_ASSERT
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
