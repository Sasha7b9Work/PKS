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


/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
I2C_HandleTypeDef hi2c2;

volatile uint32_t counter_millis   = 0;
const    uint8_t  double_word_size = 4;
static   uint32_t last_blink       = 0;


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();

  SystemClock_Config();
	
  MX_GPIO_Init();
  MX_TIM1_Init();
	MX_I2C2_Init();
	
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_SR_UIF);
	HAL_TIM_Base_Start_IT(&htim1);
	
	//Читаем параметры
	uint32_t flash_size = eeprom_read_uint32(EE_FLASH_SIZE);
	uint16_t flash_hash = eeprom_read_uint16(EE_FLASH_HASH);
	uint32_t flash_pwd  = eeprom_read_uint32(EE_FLASH_PWD);
	
	uint8_t  flash_res = 0;
	
	if (flash_size>0 && flash_size<=FLASH_MAX_SIZE && flash_pwd==FLASH_PASSWD) {
		
		uint32_t addr_m24m01;
		uint32_t addr_flash;
		uint8_t  data_read[SLICE_SIZE];
		uint16_t crc;
		uint8_t  bank;
		uint32_t len;
		uint16_t crc_flash;
		uint32_t size;
		
		crc  = CRC_INIT;
		addr_flash = APP_START_ADDR;
		size = flash_size;
		
		//Вычисляем CRC кода FLASH памяти
		do {
			
			len = ((size >= SLICE_SIZE) ? SLICE_SIZE : size);
			
			uint32_t flash_double_word;
			
			//Кол-во полных двойных слов
			uint8_t len_dwords = len / double_word_size;
			
			//Длина неполного двойного слова
			uint8_t not_full_dword_size = len % double_word_size;
			
			//Добавляем неполное двойное слово
			len_dwords += not_full_dword_size ? 1 : 0;
			
			//Read max 128 bytes (32 double words)
			for (uint8_t i = 0; i < len_dwords; i++) {
				
				//Read double word
				flash_double_word = *(__IO uint32_t *)(addr_flash + (i*double_word_size));
				
				//Если есть неполное двойное слово и мы дошли до него
				if (not_full_dword_size && (i == len_dwords - 1)) {
					
					//Маска - 0xff, 0xff00, 0xff0000, 0xff000000
					uint32_t mask = 0xff;
					
					//По всему неполного двойному слову
					for (uint8_t k = 0; k < not_full_dword_size; k++) {
							
						//Берем байт
						data_read[double_word_size*i + k] = (flash_double_word & mask) >> (k * 8);
							
						//Сдвигаем маску
						mask <<= 8;
					}
					
				} else {
					
					//Init byte array with word values
					data_read[double_word_size*i] 	= (uint8_t)flash_double_word;
					data_read[double_word_size*i+1] = (uint8_t)((flash_double_word & 0xFF00) >> 8);
					data_read[double_word_size*i+2] = (uint8_t)((flash_double_word & 0xFF0000) >> 16);
					data_read[double_word_size*i+3] = (uint8_t)((flash_double_word & 0xFF000000) >> 24);
				}
			}
			
			crc = crc16(data_read, len, crc);
			
			size -= len;
			addr_flash += len;
			
			check_blink();
			
		} while (size);
		
		
		if (crc != flash_hash) {
			
			//CRC кода FLASH памяти не соответствует заданному
			
			crc  = CRC_INIT;
			addr_m24m01 = 0;
			size = flash_size;
			bank = FLASH_BANK_N1;
			
			//Вычисляем CRC прошивки в микросхеме памяти
			do {
	
				len = size >=SLICE_SIZE ? SLICE_SIZE : size;
				
				if (addr_m24m01 >= FLASH_BANK_SIZE) {
					addr_m24m01 -= FLASH_BANK_SIZE;
					bank = FLASH_BANK_N2;
				}
				
				if ( M24M01_read(bank, addr_m24m01, &data_read[0], len) ) {
	
					crc = crc16(data_read, len, crc);
	
				} else {
					break;
				}
	
				size -= len;
				addr_m24m01 += len;
				check_blink();
	
			} while (size);
			
			
			//CRC прошивки в микросхеме памяти соответствует заданной CRC
			if (crc == flash_hash) {
				
				addr_flash = APP_START_ADDR;
				size = flash_size;
				uint8_t cnt = 0;
				
				//How many pages need to program
				uint8_t pages_need = flash_size / EEPROM_PAGE_SIZE;
				pages_need +=  flash_size % EEPROM_PAGE_SIZE ? 1 : 0;
				
				uint32_t page_addr = addr_flash;
				
				static FLASH_EraseInitTypeDef EraseInitStruct;
				EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
				EraseInitStruct.PageAddress = page_addr;
				EraseInitStruct.NbPages = pages_need;
				
				uint32_t page_error = 0;
				
				HAL_FLASH_Unlock();
				HAL_FLASHEx_Erase(&EraseInitStruct, &page_error);
				HAL_FLASH_Lock();
				
				addr_m24m01 = 0;
				bank = FLASH_BANK_N1;
				
				do {
	
					len = size >=SLICE_SIZE ? SLICE_SIZE : size;
					
					if (addr_m24m01 >= FLASH_BANK_SIZE) {
						bank = FLASH_BANK_N2;
						addr_m24m01 -= FLASH_BANK_SIZE;
					}
	
					if ( M24M01_read(bank, addr_m24m01, &data_read, len) ) {
		
						crc = crc16(data_read, len, CRC_INIT);
						
						//Variable - word for flash to program
						uint32_t flash_double_word;
						
						//Кол-во полных двойных слов
						uint8_t len_dwords = len / double_word_size;
						
						//Длина неполного двойного слова
						uint8_t not_full_word_len = len % double_word_size;
						
						//Добавляем неполное двойное слово
						len_dwords += not_full_word_len ? 1 : 0;
						
						HAL_FLASH_Unlock();
						
						//Program
						for (uint8_t i = 0; i < len_dwords; i++) {
							
							//Если есть неполное двойное слово и мы дошли до него
							if (not_full_word_len && (i == len_dwords - 1)) {
								
								//Обнуляем двойное слово
								flash_double_word = 0;
								
								//Проходим по всему двойному слову
								for (uint8_t k = 0; k < double_word_size; k++) {
										
									//Если записали еще не все неполное двойное слово
									if (not_full_word_len) {
										flash_double_word |= data_read[double_word_size * i + k] << (k*8);
									} else { //Иначе записываем 0xff
										flash_double_word |= 0xff << (k*8);
									}
										
									//Переходим к следующему байту двойного слова
									not_full_word_len--;
								}
							} else { //Иначе записываем полное двойное слово
								//Init word value with byte array
								flash_double_word = data_read[double_word_size*i];
								flash_double_word |= (data_read[double_word_size*i+1] << 8);
								flash_double_word |= (data_read[double_word_size*i+2] << 16);
								flash_double_word |= (data_read[double_word_size*i+3] << 24);
							}
							
							//Program word to flash
							HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr_flash + (i*4), flash_double_word);
						}
						
						HAL_FLASH_Lock();
						
						not_full_word_len = len % double_word_size;
						
						//Read and check
						for (uint8_t i = 0; i < len_dwords; i++) {
							
							flash_double_word = *(__IO uint32_t *)(addr_flash + (i*double_word_size));
							
							//Если есть неполное двойное слово и мы дошли до него
							if (not_full_word_len && (i == len_dwords - 1)) {
									
								uint32_t mask = 0xff;
								
								//Проходим по всему двойному слову
								for (uint8_t k = 0; k < not_full_word_len; k++) {
										
									data_read[double_word_size*i + k] = (flash_double_word & mask) >> (k * 8);
									
									mask <<= 8;
								}
							} else {
								
								//Init byte array with word values
								data_read[double_word_size*i] 	= (uint8_t)flash_double_word;
								data_read[double_word_size*i+1] = (uint8_t)((flash_double_word & 0xFF00) >> 8);
								data_read[double_word_size*i+2] = (uint8_t)((flash_double_word & 0xFF0000) >> 16);
								data_read[double_word_size*i+3] = (uint8_t)((flash_double_word & 0xFF000000) >> 24);
							}
						}
						
						crc_flash = crc16(data_read, len, CRC_INIT);
						
						if (crc_flash != crc) {
							flash_res = 2;
							break;
						}
					} else {
						//Page read fail
						flash_res = 1;
						break;
					}
					
					size -= len;
					addr_flash += len;
					addr_m24m01 += len;
					cnt  ++;
					check_blink();
	
				} while (size);
				
				//Результат
				eeprom_write_byte(EE_FLASH_RES, flash_res);
				
				uint8_t code = FLASH_CODE;
				eeprom_write_byte(EE_FLASH_FLAG, code);
				
				uint16_t rate = flash_res ? 150 : 50;
				
				//Blinking fast
				for (uint8_t n = 0; n < 20; n++) {

						HAL_GPIO_TogglePin(PIN_WDT_PORT, PIN_WDT_PIN);
						HAL_GPIO_TogglePin(PIN_LED_PORT, PIN_LED_PIN);

						delay_millis(rate);
				}
				
			}
		}
	}
	
	//Переходим на исполнение программы
	BootJump((uint32_t*)APP_START_ADDR);
	
  while (1)
  {
		check_blink();
  }
}

static void check_blink(void)
{
	if (millis() - last_blink >= 100) {
		last_blink = millis();
		HAL_GPIO_TogglePin(PIN_WDT_PORT, PIN_WDT_PIN);
		HAL_GPIO_TogglePin(PIN_LED_PORT, PIN_LED_PIN);
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	HAL_GPIO_WritePin(PIN_WDT_PORT, PIN_WDT_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(PIN_LED_PORT, PIN_LED_PIN, GPIO_PIN_RESET);
	
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	GPIO_InitStruct.Pin = PIN_WDT_PIN;
  HAL_GPIO_Init(PIN_WDT_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = PIN_LED_PIN;
  HAL_GPIO_Init(PIN_LED_PORT, &GPIO_InitStruct);

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

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	
	//Millis
	if (htim->Instance == TIM1) {
		counter_millis++;
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

  /* USER CODE BEGIN I2C2_Init 0 */
	//LCD + RFID

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
	
	/*
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	*/
	
	//PB10 SCL2 (RFID, LCD)
	//PB11 SDA2 (RFID, LCD)
	
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

}




__attribute__((noreturn)) static void BootJump( uint32_t *Address ) {
	
	//Переход в привелигированный режим
	if (CONTROL_nPRIV_Msk & __get_CONTROL()) {
		EnablePrivilegedMode();
	}
	
	//Запрещаем все прерывания
	for (uint8_t i = 0; i < 8; i++) {
		
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
	
	//Деинициализируем всё
	HAL_TIM_Base_Stop_IT(&htim1);
  HAL_TIM_Base_DeInit(&htim1);
	HAL_I2C_DeInit(&hi2c2);
	
	HAL_GPIO_DeInit(PIN_WDT_PORT, PIN_WDT_PIN);
	HAL_GPIO_DeInit(PIN_LED_PORT, PIN_LED_PIN);
	
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
	HAL_DeInit();
	
	//Сбрасываем флаги прерываний, ожидающих обработки
	for (uint8_t i = 0; i < 8; i++) {
		
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}
	
	//Отключаем системный таймер
	SysTick->CTRL = 0 ;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
	
	//Отключаем обработчики fault
	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk |  
                  SCB_SHCSR_BUSFAULTENA_Msk | 
                  SCB_SHCSR_MEMFAULTENA_Msk ) ;
	
	//Активируем регистр MSP
	if( CONTROL_SPSEL_Msk & __get_CONTROL( ) )
	{  
		__set_MSP( __get_PSP( ) ) ;
		__set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
	}
	
	//Переносим таблицу векторов прерываний
	SCB->VTOR = ( uint32_t )Address;
	
	//"Прыгаем" на адрес
	BootJumpASM(Address[0], Address[1]);
}

//"Прыгаем" на адрес, с которого начинается пользовательская программа
__attribute__((naked, noreturn)) void BootJumpASM( uint32_t SP, uint32_t RH ) {
  
	__asm("MSR      MSP,r0");
  __asm("BX       r1");
}

//Переход в привилегированный режим (privileged mode)
void SVC_Handler_Main(unsigned int *svc_args) {
	
	unsigned int svc_number;
	
	
   // Stack contains:
   // r0, r1, r2, r3, r12, r14, the return address and xPSR
   // First argument (r0) is svc_args[0]
   
	
	//Берем аргумент инструкции SVC - в данном случае ноль (__asm("SVC #0"))
	svc_number = ((char*)svc_args[6])[-2] ;
	
	switch( svc_number )
  {
    case 0: 
      __set_CONTROL( __get_CONTROL() & ~CONTROL_nPRIV_Msk ) ;
      break;
    default:   
      break;
  }
}