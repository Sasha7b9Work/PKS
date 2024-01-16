/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
//#include "stdio.h"
#include "Config.h"
//#include "stm32f1xx_hal_rtc.h"
#include "Timers.h"
#include "Serial.h"
#include "Ports.h"
#include "eeprom.h"
#include "Phase.h"
#include "W25Q64.h"
#include "ADE7758.h"
#include "Net.h"

void        Error_Handler(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);

static void sys_timers(void);
void        sys_led_blink(void);

void        sys_wdt_upd(void);
void        sys_wdt_reset(void);
uint16_t    sys_wdt_max(void);
uint32_t    sys_wdt_cnt(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
