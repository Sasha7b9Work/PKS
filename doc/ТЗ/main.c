/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KM_ON					1U
#define KM_OFF					0
#define KM_TRANSITION_DELAY			130U
#define KM_PAUSE				5000U		//be aware to set pause less than watchdog interval

/* Definition of ADCx conversions data table size */
#define ADC_CONVERTED_DATA_BUFFER_SIZE  	((uint32_t)4096U)
#define ADC_CONVERTION_QTY			256U
#define ADC_MOVING_AVERAGE_WINDOW		8U

#define ADC_180V_INIT	2044U
#define ADC_240V_INIT	2730U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN PV */
uint8_t systick_f = 0;
static uint32_t	led = 0;
__IO   uint16_t   aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE]; /* ADC group regular conversion data (array of data) */
uint64_t sigma;
uint32_t conversion_cnt;
uint32_t adc_voltage = 0;
uint16_t adc_maf_buffer_1[ADC_CONVERTED_DATA_BUFFER_SIZE/4];
//uint16_t adc_maf_buffer_2[ADC_CONVERTED_DATA_BUFFER_SIZE/4];	//not use ADC channel 2
uint32_t adc_maf_sum_1 = 0;
//uint32_t adc_maf_sum_2 = 0;									//not use ADC channel 2
uint16_t adc_maf_prev_buffer_1[ADC_MOVING_AVERAGE_WINDOW];
//uint16_t adc_maf_prev_buffer_2[ADC_MOVING_AVERAGE_WINDOW];	//not use ADC channel 2
uint16_t adc_max[ADC_CONVERTION_QTY], adc_min[ADC_CONVERTION_QTY];
uint16_t adc_160v, adc_180v = ADC_180V_INIT, adc_190v, adc_200v, adc_210v, adc_220v, adc_230v, adc_240v = ADC_240V_INIT, adc_250v, adc_260v, adc_270v;

/* Variable to report status of DMA transfer of ADC group regular conversions */
/*  0: DMA transfer is not completed                                          */
/*  1: DMA transfer is half completed                                         */
/*  2: DMA transfer is completed                                              */
__IO   uint8_t ubDmaTransferStatus = 0; /* Variable set into DMA interruption callback */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */
static void LEDs(uint32_t _led);
static uint16_t sqrxi32(uint32_t y);
static void Array_MovingAverage_w_prev(uint16_t* inputSeries, uint16_t inputSize, uint16_t* output, uint32_t* sum, uint16_t* prev_buffer);
static void compute_voltage_and_led(uint16_t adc_pp);
static void set_stage(uint32_t _voltage);
static void km_action(uint8_t _n, uint8_t _state);
static void km_error(uint32_t _n);
static void check_voltage_calibrate(uint32_t _adc_voltage);
static void adc_recalculate_settings(void);
static void flash_erase_and_store(uint32_t _data1, uint32_t _data2);
static uint32_t flash_retrieve_data(uint32_t _address);

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
  MX_DMA_Init();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */

  //restore voltage calibration
  if((flash_retrieve_data(FLASH_BANK_SIZE - FLASH_PAGE_SIZE) & 0xFFFF) < 4096U)
    adc_180v = flash_retrieve_data(FLASH_BANK_SIZE - FLASH_PAGE_SIZE);
  if((flash_retrieve_data(FLASH_BANK_SIZE - FLASH_PAGE_SIZE + 8U) & 0xFFFF) < 4096U)
    adc_240v = flash_retrieve_data(FLASH_BANK_SIZE - FLASH_PAGE_SIZE + 8U);
  adc_recalculate_settings();

  //Start-up illumination
  for(uint32_t _i=1U; _i<=0x100U; _i<<=1)
  {
	  LEDs(_i);
	  HAL_Delay(200U);
  };
  LEDs(0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
//	  if(tick_cnt_old != HAL_GetTick())
//	  {
		//1ms interval
//		tick_cnt_old = HAL_GetTick();
//	  };

    //acquisition of ADC array of data during 5s
    //One sample takes 10us@2MHz (7.5 sample + 12.5 adc cycles, total 20 cycles).
    //It needs 500000 samples per 10us to save 5 seconds.
    //One DMA cycle consists of 4096 samples (40.96ms) of both channels. DMA half complete after 2048 samples (20.48ms).
    /* Run the ADC calibration */
    if(HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
    {
      /* Calibration Error */
      Error_Handler();
    };

    /*## Start ADC conversions ###############################################*/
    /* Start ADC group regular conversion with DMA */
    if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)aADCxConvertedData, ADC_CONVERTED_DATA_BUFFER_SIZE) != HAL_OK)
    {
      /* ADC conversion start error */
      Error_Handler();
    };

    sigma = 0;
    conversion_cnt = 0;
    while(conversion_cnt < ADC_CONVERTION_QTY)
    {
      uint16_t _j;

      if(ubDmaTransferStatus == 1)
      {
        //DMA half completed
	ubDmaTransferStatus = 0;

	/* расчет среднеквадратического не используется
        for(uint32_t _i = 0; _i < ADC_CONVERTED_DATA_BUFFER_SIZE/2; _i += 2)
	{
          uint32_t _j;
	  if(aADCxConvertedData[_i] > aADCxConvertedData[_i+1])
	    _j = aADCxConvertedData[_i] - aADCxConvertedData[_i+1];
          else
            _j = aADCxConvertedData[_i+1] - aADCxConvertedData[_i];
          sigma += (uint64_t)((uint64_t)_j * (uint64_t)_j);
	};
	*/

        _j = 0;
	for(uint32_t _i = 0; _i < ADC_CONVERTED_DATA_BUFFER_SIZE/2; _i += 2)
        {
          adc_maf_buffer_1[_j] = aADCxConvertedData[_i];
          //adc_maf_buffer_2[_j] = aADCxConvertedData[_i + 1];		//not use results of second channel of ADC
          _j++;
        };

	Array_MovingAverage_w_prev(adc_maf_buffer_1, ADC_CONVERTED_DATA_BUFFER_SIZE/4, adc_maf_buffer_1, &adc_maf_sum_1, adc_maf_prev_buffer_1);

	//find max and min (starts from not first cell to ensure no wrong value during MAF startup)
	uint16_t _max = 0, _min = (uint16_t)-1U;
	for(uint32_t _i = ADC_MOVING_AVERAGE_WINDOW; _i < ADC_CONVERTED_DATA_BUFFER_SIZE/4; _i++)
	{
	  if(adc_maf_buffer_1[_i] > _max)
	    _max = adc_maf_buffer_1[_i];
	  if(adc_maf_buffer_1[_i] < _min)
	    _min = adc_maf_buffer_1[_i];
	};

	//store min and max values
	adc_max[conversion_cnt] = _max;
	adc_min[conversion_cnt] = _min;

	conversion_cnt++;
	HAL_IWDG_Refresh(&hiwdg);
      };

      if(ubDmaTransferStatus == 2)
      {
        //DMA full completed
	ubDmaTransferStatus = 0;

/* test area
__disable_irq();
for(uint32_t _i=ADC_CONVERTED_DATA_BUFFER_SIZE/2; _i<ADC_CONVERTED_DATA_BUFFER_SIZE; _i+=2)
  adc_temp_buffer_2[(_i - ADC_CONVERTED_DATA_BUFFER_SIZE/2)/2] = aADCxConvertedData[_i];
adc_first_last[conversion_cnt*2] = adc_temp_buffer_2[0];
adc_first_last[conversion_cnt*2+1] = adc_temp_buffer_2[ADC_CONVERTED_DATA_BUFFER_SIZE/4-1];
__enable_irq();
*/

	/* расчет среднеквадратического не используется
	for(uint32_t _i = ADC_CONVERTED_DATA_BUFFER_SIZE/2; _i < ADC_CONVERTED_DATA_BUFFER_SIZE; _i += 2)
        {
          uint32_t _j;
          if(aADCxConvertedData[_i] > aADCxConvertedData[_i+1])
            _j = aADCxConvertedData[_i] - aADCxConvertedData[_i+1];
          else
            _j = aADCxConvertedData[_i+1] - aADCxConvertedData[_i];
          sigma += (uint64_t)((uint64_t)_j * (uint64_t)_j);
        };
        */

        _j = 0;
        for(uint32_t _i = ADC_CONVERTED_DATA_BUFFER_SIZE/2; _i < ADC_CONVERTED_DATA_BUFFER_SIZE; _i += 2)
        {
          adc_maf_buffer_1[_j] = aADCxConvertedData[_i];
          //adc_maf_buffer_2[_j] = aADCxConvertedData[_i + 1];		//not use results of second channel of ADC
          _j++;
        };
		Array_MovingAverage_w_prev(adc_maf_buffer_1, ADC_CONVERTED_DATA_BUFFER_SIZE/4, adc_maf_buffer_1, &adc_maf_sum_1, adc_maf_prev_buffer_1);

		//find max and min (starts from not first cell to ensure no wrong value during MAF startup)
		uint16_t _max = 0, _min = (uint16_t)-1U;
		for(uint32_t _i = ADC_MOVING_AVERAGE_WINDOW; _i < ADC_CONVERTED_DATA_BUFFER_SIZE/4; _i++)
		{
		  if(adc_maf_buffer_1[_i] > _max)
			_max = adc_maf_buffer_1[_i];
		  if(adc_maf_buffer_1[_i] < _min)
			_min = adc_maf_buffer_1[_i];
		};

		//store min and max values
		adc_max[conversion_cnt] = _max;
		adc_min[conversion_cnt] = _min;

        conversion_cnt++;
        HAL_IWDG_Refresh(&hiwdg);
      };

      //light up LEDs after each 2nd conversion
      if((conversion_cnt & 0x0001U) == 0x0001U)
      {
        compute_voltage_and_led(adc_max[conversion_cnt-1] - adc_min[conversion_cnt-1]);
      };
    };

    //
    HAL_ADC_Stop_DMA(&hadc1);

    /*
    //compute average voltage
    adc_voltage = sqrxi32((sigma) >> 12U);
    compute_voltage_and_led(adc_voltage);
    */

    uint32_t _sum = 0;
    for(uint32_t _i = 1; _i < ADC_CONVERTION_QTY; _i++)		//не использовать нулевой элемент из-за ошибки, вносимой фильтром скользящего среднего при старте измерений
      _sum += adc_max[_i] - adc_min[_i];
    adc_voltage = _sum / (ADC_CONVERTION_QTY - 1);

    if(HAL_GPIO_ReadPin(DISABLE_KM_GPIO_Port, DISABLE_KM_Pin) == GPIO_PIN_SET)
      set_stage(adc_voltage);

    //short led flash to show user the end of measuring cycle
    LEDs(0);
    check_voltage_calibrate(adc_voltage);
    HAL_Delay(100U);

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_7CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_7CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NC1_Pin|NC2_Pin|NC3_Pin|NC4_Pin
                          |NC8_Pin|NC9_Pin|NC10_Pin|NC11_Pin
                          |LED9_Pin|LED8_Pin|KM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ATST1_Pin|ANC1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED7_Pin|LED6_Pin|LED5_Pin|LED4_Pin
                          |LED3_Pin|LED2_Pin|LED1_Pin|KM3_Pin
                          |KM1_Pin|KM2_Pin|KM4_Pin|KM6_Pin
                          |KM8_Pin|KM5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, NC12_Pin|TST3_Pin|TST1_Pin|TST2_Pin
                          |NC13_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NC1_Pin NC2_Pin NC3_Pin NC4_Pin
                           NC8_Pin NC9_Pin NC10_Pin NC11_Pin
                           LED9_Pin LED8_Pin KM7_Pin */
  GPIO_InitStruct.Pin = NC1_Pin|NC2_Pin|NC3_Pin|NC4_Pin
                          |NC8_Pin|NC9_Pin|NC10_Pin|NC11_Pin
                          |LED9_Pin|LED8_Pin|KM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : NC5_Pin TSTKM5_Pin TSTKM6_Pin TSTKM13_Pin
                           TSTKM14_Pin */
  GPIO_InitStruct.Pin = NC5_Pin|TSTKM5_Pin|TSTKM6_Pin|TSTKM13_Pin
                          |TSTKM14_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PF0 PF1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : ATST1_Pin ANC1_Pin */
  GPIO_InitStruct.Pin = ATST1_Pin|ANC1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DISABLE_KM_Pin LEVEL2_Pin LEVEL3_Pin LEVEL4_Pin
                           TSTKM3_Pin TSTKM4_Pin TSTKM9_Pin TSTKM10_Pin
                           TSTKM11_Pin TSTKM12_Pin */
  GPIO_InitStruct.Pin = DISABLE_KM_Pin|LEVEL2_Pin|LEVEL3_Pin|LEVEL4_Pin
                          |TSTKM3_Pin|TSTKM4_Pin|TSTKM9_Pin|TSTKM10_Pin
                          |TSTKM11_Pin|TSTKM12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED7_Pin LED6_Pin LED5_Pin LED4_Pin
                           LED3_Pin LED2_Pin LED1_Pin KM3_Pin
                           KM1_Pin KM2_Pin KM4_Pin KM6_Pin
                           KM8_Pin KM5_Pin */
  GPIO_InitStruct.Pin = LED7_Pin|LED6_Pin|LED5_Pin|LED4_Pin
                          |LED3_Pin|LED2_Pin|LED1_Pin|KM3_Pin
                          |KM1_Pin|KM2_Pin|KM4_Pin|KM6_Pin
                          |KM8_Pin|KM5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TSTKM1_Pin TSTKM2_Pin */
  GPIO_InitStruct.Pin = TSTKM1_Pin|TSTKM2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TSTKM7_Pin TSTKM8_Pin TSTKM15_Pin TSTKM16_Pin */
  GPIO_InitStruct.Pin = TSTKM7_Pin|TSTKM8_Pin|TSTKM15_Pin|TSTKM16_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : NC12_Pin TST3_Pin TST1_Pin TST2_Pin
                           NC13_Pin */
  GPIO_InitStruct.Pin = NC12_Pin|TST3_Pin|TST1_Pin|TST2_Pin
                          |NC13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void LEDs(uint32_t _led)
{
	if(_led & 0x0001U)
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

	if(_led & 0x0002U)
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

	if(_led & 0x0004U)
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);

	if(_led & 0x0008U)
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);

	if(_led & 0x0010U)
		HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);

	if(_led & 0x0020U)
		HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET);

	if(_led & 0x0040U)
		HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET);

	if(_led & 0x0080U)
		HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_RESET);

	if(_led & 0x0100U)
		HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_RESET);
};

uint16_t sqrxi32(uint32_t y)
{
	if(y == 1)
	  return 1;
	uint32_t xh = y > 0x10000ul ? 0x10000ul : y;
	uint32_t xl = 0;
	uint32_t xc;
	for(int k=0; k<16; k++)
	{
		xc = (xh + xl) >> 1ul;
		if(xc * xc - xc >= y)
		{
			xh = xc;
		}
		else
		{
			xl = xc;
		};
	};
	return (xh + xl) >> 1ul;
}

void Array_MovingAverage_w_prev(uint16_t* inputSeries, uint16_t inputSize, uint16_t* output, uint32_t* sum, uint16_t* prev_buffer)
{
  for(uint16_t i = 0; i < inputSize; i++)
  {
    //Add newest sample
    *sum += inputSeries[i];

    //Subtract oldest sample
    *sum -= prev_buffer[ADC_MOVING_AVERAGE_WINDOW - 1];

    //Shift previous buffer
    for(uint16_t j = ADC_MOVING_AVERAGE_WINDOW - 1; j > 0; j--)
    	prev_buffer[j] = prev_buffer[j - 1];
    prev_buffer[0] = inputSeries[i];

    output[i] = *sum / ADC_MOVING_AVERAGE_WINDOW;
  };
}

//compute voltage according to input adc value peak-to-peak
void compute_voltage_and_led(uint16_t adc_pp)
{
  uint32_t _led = 0;

  if(adc_pp < adc_190v)
	_led = 0x0001U;
  else
	if(adc_pp < adc_200v)
	  _led = 0x0002U;
	else
	  if(adc_pp < adc_210v)
		_led = 0x0004U;
	  else
		if(adc_pp < adc_220v)
		  _led = 0x0008U;
		else
		  if(adc_pp < adc_240v)
			_led = 0x0010U;
		  else
			if(adc_pp < adc_250v)
			  _led = 0x0020U;
			else
			  if(adc_pp < adc_260v)
				_led = 0x0040U;
			  else
				if(adc_pp < adc_270v)
				  _led = 0x0080U;
				else
				  _led = 0x0100U;

  LEDs(_led);
}

//set stage according to measured voltage
void set_stage(uint32_t _adc_voltage)
{
  static int32_t _current_stage_num = 0, _old_stage_num;
  //static uint32_t _voltage_old = 0;

  _old_stage_num = _current_stage_num;

  //determine new stage according to voltage and prev stage
  if(_adc_voltage <= adc_160v)
    _current_stage_num = 0;
  else
    if(_adc_voltage <= adc_190v)
      _current_stage_num = 4;
    else
	if(_adc_voltage <= adc_200v)
	{
	  if(_old_stage_num >= 4)
	    _current_stage_num = 4;
	  else
	    _current_stage_num = 3;
	}
	else
	  if(_adc_voltage <= adc_210v)
	  {
	    if(_old_stage_num >= 3)
	      _current_stage_num = 3;
	    else
	      _current_stage_num = 2;
	  }
	  else
	    if(_adc_voltage <= adc_220v)
	    {
              if(_old_stage_num >= 2)
		_current_stage_num = 2;
              else
		_current_stage_num = 1;
	    }
	    else
	      if(_adc_voltage <= adc_230v)
              {
		if(_old_stage_num >= 1)
		  _current_stage_num = 1;
		else
		  _current_stage_num = 0;
              }
	      else
		if(_adc_voltage <= adc_240v)
		{
                  if(_old_stage_num >= 0)
                    _current_stage_num = 0;
                  else
                    _current_stage_num = -1;
		}
		else
		  if(_adc_voltage <= adc_250v)
		  {
		    if(_old_stage_num >= -1)
	              _current_stage_num = -1;
		    else
		      _current_stage_num = -2;
		  }
		  else
		    if(_adc_voltage <= adc_260v)
		    {
		      if(_old_stage_num >= -2)
			_current_stage_num = -2;
		      else
			_current_stage_num = -3;
		    }
		    else
		      if(_adc_voltage <= adc_270v)
		      {
			if(_old_stage_num >= -3)
			  _current_stage_num = -3;
			else
			  _current_stage_num = -4;
		      }
		      else
			_current_stage_num = -4;

  //switch to another stage
  if(_old_stage_num != _current_stage_num)
  {
    //first, switch to bypass
    if(_old_stage_num != 0)
    {
      km_action(2, KM_ON);
      km_action(3, KM_ON);
      km_action(1, KM_OFF);
      km_action(2, KM_OFF);
      HAL_Delay(KM_PAUSE);
      HAL_IWDG_Refresh(&hiwdg);
      km_action(3, KM_OFF);
    };

    //set current stage
    if((_current_stage_num == 0) || (_current_stage_num == 1))
    {
      km_action(4, KM_OFF);
      km_action(5, KM_OFF);
      km_action(6, KM_OFF);
      km_action(7, KM_OFF);
      km_action(8, KM_OFF);
    };

    if(_current_stage_num == 2)
    {
      km_action(4, KM_OFF);
      km_action(5, KM_OFF);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_OFF);
    };

    if(_current_stage_num == 3)
    {
      km_action(4, KM_OFF);
      km_action(5, KM_ON);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_OFF);
    };

    if(_current_stage_num == 4)
    {
      km_action(4, KM_ON);
      km_action(5, KM_ON);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_OFF);
    };

    if(_current_stage_num == -1)
    {
      km_action(4, KM_OFF);
      km_action(5, KM_OFF);
      km_action(6, KM_OFF);
      km_action(7, KM_OFF);
      km_action(8, KM_ON);
    };

    if(_current_stage_num == -2)
    {
      km_action(4, KM_OFF);
      km_action(5, KM_OFF);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_ON);
    };

    if(_current_stage_num == -3)
    {
      km_action(4, KM_OFF);
      km_action(5, KM_ON);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_ON);
    };

    if(_current_stage_num == -4)
    {
      km_action(4, KM_ON);
      km_action(5, KM_ON);
      km_action(6, KM_ON);
      km_action(7, KM_ON);
      km_action(8, KM_ON);
    };

    //final sequence if not bypass
    if(_current_stage_num != 0)
    {
      km_action(2, KM_ON);
      km_action(3, KM_ON);
      km_action(1, KM_ON);
      km_action(2, KM_OFF);
      HAL_Delay(KM_PAUSE);
      HAL_IWDG_Refresh(&hiwdg);
      km_action(3, KM_OFF);
    };
  };
}

void km_action(uint8_t _n, uint8_t _state)
{
  uint32_t _delay = 50U;			//2.5 periods of grid is a timeout to acquisite pulses from AC detectors
  uint32_t _off_cnt = 0, _on_cnt = 0;

  systick_f = 0;

  //KM1
  if(_n == 1)
  {
	HAL_GPIO_WritePin(KM1_GPIO_Port, KM1_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM1_GPIO_Port, TSTKM1_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM9_GPIO_Port, TSTKM9_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM2 on
  if(_n == 2)
  {
	HAL_GPIO_WritePin(KM2_GPIO_Port, KM2_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM2_GPIO_Port, TSTKM2_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM10_GPIO_Port, TSTKM10_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM3 on
  if(_n == 3)
  {
	HAL_GPIO_WritePin(KM3_GPIO_Port, KM3_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM3_GPIO_Port, TSTKM3_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM11_GPIO_Port, TSTKM11_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM4 on
  if(_n == 4)
  {
	HAL_GPIO_WritePin(KM4_GPIO_Port, KM4_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM4_GPIO_Port, TSTKM4_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM12_GPIO_Port, TSTKM12_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM5 on
  if(_n == 5)
  {
	HAL_GPIO_WritePin(KM5_GPIO_Port, KM5_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM5_GPIO_Port, TSTKM5_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM13_GPIO_Port, TSTKM13_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM6 on
  if(_n == 6)
  {
	HAL_GPIO_WritePin(KM6_GPIO_Port, KM6_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM6_GPIO_Port, TSTKM6_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM14_GPIO_Port, TSTKM14_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM7 on
  if(_n == 7)
  {
	HAL_GPIO_WritePin(KM7_GPIO_Port, KM7_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM7_GPIO_Port, TSTKM7_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM15_GPIO_Port, TSTKM15_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

  //KM8 on
  if(_n == 8)
  {
	HAL_GPIO_WritePin(KM8_GPIO_Port, KM8_Pin, _state ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_Delay(KM_TRANSITION_DELAY);

	while(_delay)
	{
	  if(HAL_GPIO_ReadPin(TSTKM8_GPIO_Port, TSTKM8_Pin) == GPIO_PIN_RESET)
		_on_cnt++;
	  if(HAL_GPIO_ReadPin(TSTKM16_GPIO_Port, TSTKM16_Pin) == GPIO_PIN_RESET)
		_off_cnt++;

	  if(systick_f)
	  {
		systick_f = 0;
		_delay--;
	  };
	};
  };

#ifndef CONTACTORS_NO_FEEDBACK
  //check contactorts except KM8
  if(_n != 8U)
  {
    if(_state && (_off_cnt || !_on_cnt))
      km_error(_n);

    if(!_state && (!_off_cnt || _on_cnt))
      km_error(_n);
  };
#endif
}

//flash led according to number of damaged contactor
void km_error(uint32_t _n)
{
  uint32_t _led = 0;

  HAL_GPIO_WritePin(KM1_GPIO_Port, KM1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM2_GPIO_Port, KM2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM3_GPIO_Port, KM3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM4_GPIO_Port, KM4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM5_GPIO_Port, KM5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM6_GPIO_Port, KM6_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM7_GPIO_Port, KM7_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM8_GPIO_Port, KM8_Pin, GPIO_PIN_RESET);

  while(1)
  {
	HAL_IWDG_Refresh(&hiwdg);
	switch(_n)
	{
	  case 1: _led = 0x0001U; break;
	  case 2: _led = 0x0002U; break;
	  case 3: _led = 0x0004U; break;
	  case 4: _led = 0x0008U; break;
	  case 5: _led = 0x0010U; break;
	  case 6: _led = 0x0020U; break;
	  case 7: _led = 0x0040U; break;
	  case 8: _led = 0x0080U; break;
	};
	LEDs(_led);
	HAL_Delay(167U);
	LEDs(0);
	HAL_Delay(167U);
  };

  //HAL_NVIC_SystemReset();
}

void check_voltage_calibrate(uint32_t _adc_voltage)
{
  if((HAL_GPIO_ReadPin(LEVEL4_GPIO_Port, LEVEL4_Pin) == GPIO_PIN_RESET) || (HAL_GPIO_ReadPin(LEVEL3_GPIO_Port, LEVEL3_Pin) == GPIO_PIN_RESET))
  {
    HAL_Delay(500U);
    HAL_IWDG_Refresh(&hiwdg);
    if(HAL_GPIO_ReadPin(LEVEL4_GPIO_Port, LEVEL4_Pin) == GPIO_PIN_RESET)
    {
      //calibrate 180V point
      if((_adc_voltage < 3000U) && (_adc_voltage > 100U))
      {
        adc_180v = _adc_voltage;
        adc_recalculate_settings();
        flash_erase_and_store(adc_180v, adc_240v);
        HAL_IWDG_Refresh(&hiwdg);
        for(uint32_t _i = 0; _i < 3; _i++)
        {
          LEDs(0x01);
          HAL_Delay(330U);
          LEDs(0);
          HAL_Delay(170U);
        };
      };
    };

    if(HAL_GPIO_ReadPin(LEVEL3_GPIO_Port, LEVEL3_Pin) == GPIO_PIN_RESET)
    {
      //calibrate 240V point
      if((_adc_voltage < 4000U) && (_adc_voltage > 200U))
      {
        adc_240v = _adc_voltage;
        adc_recalculate_settings();
        flash_erase_and_store(adc_180v, adc_240v);
        HAL_IWDG_Refresh(&hiwdg);
        for(uint32_t _i = 0; _i < 3; _i++)
        {
          LEDs(0x20);
          HAL_Delay(330U);
          LEDs(0);
          HAL_Delay(170U);
        };
      };
    };
  };
}

void adc_recalculate_settings(void)
{
  //source points are 180V and 240V
  uint16_t _adc_10v = (adc_240v - adc_180v) / 6U;
  uint16_t _adc_20v = (adc_240v - adc_180v) / 3U;
  uint16_t _adc_30v = (adc_240v - adc_180v) / 2U;

  adc_160v = adc_180v - _adc_20v;
  adc_190v = adc_180v + _adc_10v;
  adc_200v = adc_180v + _adc_20v;
  adc_210v = adc_180v + _adc_30v;
  adc_220v = adc_240v - _adc_20v;
  adc_230v = adc_240v - _adc_10v;
  adc_250v = adc_240v + _adc_10v;
  adc_260v = adc_240v + _adc_20v;
  adc_270v = adc_240v + _adc_30v;
}

void flash_erase_and_store(uint32_t _data1, uint32_t _data2)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PAGEError = 0;

  //unlock flash
  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = FLASH_BANK_1;
  EraseInitStruct.Page        = FLASH_PAGE_NB - 1;
  EraseInitStruct.NbPages     = 1U;

  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    Error_Handler();
  };

  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_BANK_SIZE - FLASH_PAGE_SIZE, (uint64_t)_data1);
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_BANK_SIZE - FLASH_PAGE_SIZE + 8U, (uint64_t)_data2);

  HAL_FLASH_Lock();
}

uint32_t flash_retrieve_data(uint32_t _address)
{
	return *(uint32_t*)_address;
}
/******************************************************************************/
/*   USER IRQ HANDLER TREATMENT                                               */
/******************************************************************************/

/**
  * @brief  Conversion complete callback in non blocking mode
  * @param  hadc: ADC handle
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  /* Update status variable of DMA transfer */
  ubDmaTransferStatus = 2U;
}

/**
  * @brief  Conversion DMA half-transfer callback in non blocking mode
  * @note   This example shows a simple way to report end of conversion
  *         and get conversion result. You can add your own implementation.
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  /* Set flag depending on DMA transfer status */
  ubDmaTransferStatus = 1U;
}

/**
  * @brief  ADC error callback in non blocking mode
  *        (ADC conversion with interruption or transfer by DMA)
  * @param  hadc: ADC handle
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
  /* In case of ADC error, call main error handler */
  Error_Handler();
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
  HAL_GPIO_WritePin(KM1_GPIO_Port, KM1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM2_GPIO_Port, KM2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM3_GPIO_Port, KM3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM4_GPIO_Port, KM4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM5_GPIO_Port, KM5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM6_GPIO_Port, KM6_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM7_GPIO_Port, KM7_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(KM8_GPIO_Port, KM8_Pin, GPIO_PIN_RESET);

  while(1)
  {
    LEDs(0x01FFU);
    HAL_Delay(250U);
    LEDs(0);
    HAL_Delay(250U);
    //exit by watchdog and reset (about 12-15s)
  };
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
