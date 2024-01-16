/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */


#include "main.h"

#define ENABLE_RTC


/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
RTC_HandleTypeDef hrtc;
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1; //modem
UART_HandleTypeDef huart2; //serial

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi1;

volatile uint32_t counter_millis = 0;
volatile uint32_t counter_micros = 0;
volatile uint32_t counter_wdt = 0;

char              cmdData[CMDDATA_SIZE];                  //буфер вывода данных команды
char              mess[MESS_SIZE];                        //буфер текстовых данных
char              out[256];

uint8_t           set_val = 18;
uint8_t           IS_RESET = 0;

uint8_t           is_first_start       = 1; //первый запуск

uint16_t          NS_BITS              = 0; //Флаги NS
uint16_t          wdt_max              = 0;

uint8_t           b_serial = 0;
uint8_t           b_modem = 0;
uint8_t           b_modem_tx = 0;

tp_timer_type timers[TIMERS_MAX];

//SETTINGS--------------------------------------------------------------------------------
uint8_t            auto_volt_phase = 0;
uint8_t            auto_points = 0;
uint8_t            auto_time = 0;
uint8_t            auto_boost = 0;
uint8_t            auto_kt = 0;
uint8_t            auto_steps_count = 0;
uint16_t           auto_volt_min = 0;
uint16_t           auto_volt_max = 0;

uint16_t           range_volt_min = 0;
uint16_t           range_volt_max = 0;

uint8_t            debug_mess = 0;
uint8_t            debug_error = 0;
uint8_t            debug_ports = 0;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	char str[64];
	
  HAL_Init();

  SystemClock_Config();
	
	// Конфигурирование AFIO, чтобы работали ножки PB3, PB4, PA15
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
	
	MX_RTC_Init();
  MX_GPIO_Init();
  MX_TIM1_Init();
	MX_TIM3_Init();
	MX_I2C2_Init();
	MX_DMA_Init();
  MX_ADC1_Init();
	//MX_SPI1_Init();
	
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim1);
	
	MX_USART2_UART_Init(); //serial
	serial_init();
	
	MX_USART1_UART_Init(); //modem
	modem_serial_init();
	
	//Особенность работы, для включения прерываний
	HAL_UART_Receive_IT(&huart2, &b_serial, SINGLE_BYTE);
	HAL_UART_Receive_IT(&huart1, &b_modem, SINGLE_BYTE);
	
	sys_timers();
	
	Ports_init(0);
	ADE7758_init();
	W25Q64_init();
	
	gp_init();
	
	Task_init();
	
	serial_addr_read();
	
	dateFormat(now(), str);
	
	//Restart message
	sprintf(mess, "Restart, ver=%s, %s", VERSION, str);
	serial_write_addr(mess);
	
	Regulator_init(0);
	
	//Инициализация фаз
	Phase_init();
	
	//Восстанавливаем состояние
	/*
	if (wdt_reset && auto_volt_phase) {
		
		for (uint8_t n = 1; n <= PHASE_MAX; n++) {
			phase_read_state(n);
		}
	}
	*/
	
	//Инициализация сетевых параметров
	net_init();
	
	//Авторегулирование
	auto_points      = points_get();
	auto_time        = des_time_get();
	auto_boost       = boost_get();
	auto_kt          = kt_get();
	auto_steps_count = steps_count_get();
	auto_volt_min    = volt_min_get();
	auto_volt_max    = volt_max_get();
	
	range_volt_min   = range_min_get();
	range_volt_max   = range_max_get();
	
	//Debug
	debug_mess       = debug_mess_get();
	debug_error      = debug_error_get();
	debug_ports      = debug_ports_get();
	
	//Авторегулирование
	auto_volt_phase  = auto_volt_get();
	
	//Связки
	Ports_bind_out_read();
	Ports_bind_in_read();
	
	//Логика
	Ports_logic_read();
	
	//Измеритель
	ADE7758_init();
	
	sprintf(mess, "ADE7758 ver = %02X", ADE7758_getVersion());
	serial_write_addr(mess);
	
	//RTC WDT
	sys_wdt_upd();
	if (wdt_max) {
		sprintf(mess, "WDT reset in %u minutes", wdt_max);
		serial_write_addr(mess);
	}
	
	//Отложенный старт
	Task_start("DStart");
	
	
  while (1)
  {
		serial_process();
		
		#ifdef MODEM_PRESENT
			modem_serial_process();
		#endif
		
		//Обработка таймеров
		Timers_process();
		
		//Обработка заданий
		Task_process();
		
		#ifdef MODEM_PRESENT
			modem_task_check();
		#endif
		
		if (IS_RESET) {
			NVIC_SystemReset();
		}
		
		//Проверка на программный WDT
		wdt_reset_check(wdt_max);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	
	#ifdef ENABLE_RTC
		RCC_OscInitStruct.OscillatorType |= RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	#endif
	
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
	
	#ifdef ENABLE_RTC
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}
	#endif
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PIN_WDT_PORT, PIN_WDT_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(PIN_LED_PORT, PIN_LED_PIN, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = PIN_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PIN_LED_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = PIN_WDT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PIN_WDT_PORT, &GPIO_InitStruct);

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
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
	
	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE );

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

void sys_timers(void)
{
	Timers_init();
	
	timers[TIMER_CONNECT] = (tp_timer_type){
		.enabled = 1,
		.period  = 10,
		.onTickHandler = modem_connect_check
	};
	
	timers[TIMER_WDT] = (tp_timer_type){
		.enabled = 1,
		.period  = 100,
		.alarmType     = dtMillis,
		.onTickHandler = sys_wdt_reset
	};
	
	timers[TIMER_GP_MS] = (tp_timer_type){
		.enabled = 1,
		.period  = 1,
		.alarmType     = dtMillis,
		.onTickHandler = gp_input_process_ms
	};
	
	timers[TIMER_GP_SEC] = (tp_timer_type){
		.enabled = 1,
		.period  = 1,
		.onTickHandler = gp_input_process_sec
	};
	
	timers[TIMER_LED] = (tp_timer_type){
		.enabled = 1,
		.period  = 500,
		.alarmType     = dtMillis,
		.onTickHandler = sys_led_blink
	};
	
	//Получение данных с измерителя
	timers[TIMER_NET] = (tp_timer_type){
		.enabled       = 0, //delayed start
		.period        = 1, //sec
		.onTickHandler = task_net_start
	};

	//Авторегулирование
	timers[TIMER_AUTO] = (tp_timer_type){
		.enabled       = 0, //delayed start
		.period        = 1, //sec
		.onTickHandler = task_auto_start
	};
	
	//Архиватор
	timers[TIMER_ARCH] = (tp_timer_type){
		.enabled       = 1,
		.period        = 1,
		.onTickHandler = task_archiver
	};

	Timers_renew();
}

void sys_led_blink(void)
{
	HAL_GPIO_TogglePin(PIN_LED_PORT, PIN_LED_PIN);
}

void sys_wdt_reset(void)
{
	HAL_GPIO_TogglePin(PIN_WDT_PORT, PIN_WDT_PIN);
}

void sys_wdt_upd(void)
{
	wdt_max = wdt_max_read();
}

uint16_t sys_wdt_max(void)
{
	return wdt_max;
}

uint32_t sys_wdt_cnt(void)
{
	return counter_wdt;
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
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

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
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
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 57600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	
	//Millis
	if (htim->Instance == TIM1) {
		
		counter_millis++;
	
		if (counter_millis % 1000 == 0) {
			
			//Секунды по WDT
			counter_wdt ++;
			
			Timers_tick_sec();
		} else {
			Timers_tick_ms();
		}
		
		Task_timers();
	}
	//Micros
	else if (htim->Instance == TIM3) {
		counter_micros += 1000;
	}
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{
	
	//https://blog.radiotech.kz/stm32/i2c-chast-2-perevod-iz-knigi-mastering-stm32/

  GPIO_InitTypeDef GPIO_InitStruct;
	
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 200000; //работает до 250, но на 250 romcrc не выдает то что надо
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	
	//PB10 SCL2 (RFID, LCD)
	//PB11 SDA2 (RFID, LCD)
	
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 5;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}


/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
	

}
