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
#include "Config.h"
#include "Utility.h"
#include "eeprom.h"

static void Error_Handler(void);
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C2_Init(void);
static void check_blink(void);

//Обработчик прерывания SuperVisor Call
void SVC_Handler(void);

//Загрузка адреса в регистр MSP
__attribute__((naked, noreturn)) void BootJumpASM( uint32_t SP, uint32_t RH );

//Функция - jump на адрес программы
__attribute__((noreturn)) static void BootJump( uint32_t *Address );

//#define ENABLE_RTC

//Вызов прерывания SuperVisor Call (обработчик - SVC_Handler)
#define EnablePrivilegedMode() __asm("SVC #0")

//																		0x800   // 0x1000
#define EEPROM_PAGE_SIZE             (2*1024)//(4*1024)
#define MAIN_EEPAGE                  8

//Page №8 start
//*first page is №0
#define APP_START_ADDR	0x08004000

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
