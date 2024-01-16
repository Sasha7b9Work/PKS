#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"


#define  zval_8	                   0xFF
#define  zval_16	                 0xFFFF
#define  zval_32	                 0xFFFFFFFF

//I2C
#define M24M01_ADDR					        0x50

#define CRC_INIT                    0xFFFF

#define PIN_WDT_PORT								GPIOB
#define PIN_WDT_PIN									GPIO_PIN_15

#define PIN_LED_PORT				        GPIOB
#define PIN_LED_PIN					        GPIO_PIN_2

#define EROM_BANK                   0x01
#define EROM_ADDR                   0x1F800

#define EE_FLASH_SIZE               65  //69
#define EE_FLASH_HASH               69  //71
#define EE_FLASH_RES                71  //72
#define EE_FLASH_TIME               72  //76
#define EE_FLASH_PWD                76  //80
#define EE_FLASH_CNT                80  //82
#define EE_FLASH_FLAG               82  //83

#define FLASH_MAX_SIZE              243712

#define FLASH_CODE                  18
#define FLASH_PASSWD                0xAABBCCDD

#define FLASH_BANK_N1               0x00
#define FLASH_BANK_N2               0x01
#define FLASH_BANK_SIZE             131072
#define SLICE_SIZE                  128



#endif /* CONFIG_H_ */
