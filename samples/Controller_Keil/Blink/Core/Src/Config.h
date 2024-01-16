#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"


//#define DEBUG

//#define MODEM_PRESENT

#define FLASH_MAX_SIZE              129024 //131072

#define FLASH_CODE                  18
#define FLASH_PASSWD                0xAABBCCDD

#define MEM_BANK                    1


#define  zval_8	                   0xFF
#define  zval_16	                 0xFFFF
#define  zval_32	                 0xFFFFFFFF

//	Тактовая частота ядра
#define PLL_MUL  10 //4//8/10
#define CPU_FREQ HSE_Value * PLL_MUL	// 4MHz * 8 = 32MHz

//Версия ПО
#define VERSION                     "12.0"

//I2C
#define M24M01_ADDR					0x50
#define LCD_I2C_ADDR				0x27
#define PN532_ADDR          0x48 >> 1

//Кол-во устройств на шине I2C
#define I2C_DEVICES_QNT			3

#define I2C_M24M01_INDEX		0
#define I2C_LCD_INDEX				1
#define I2C_PN532_INDEX			2

//Миллисекундный таймер
#define TIMER2 		MDR_TIMER2 
#define PERIOD  	1
#define TIME_MIN                    1505041000
#define TIMER1 		MDR_TIMER1 

//SERIAL

#define MODEM_SERIAL               MDR_UART1
#define MODEM_SERIAL_IRQ           UART1_IRQn
#define MODEM_SERIAL_CLOCK         RST_CLK_PCLK_UART1

#define MODEM_SERIAL_CLOCK_TX      RST_CLK_PCLK_PORTA
#define MODEM_SERIAL_CLOCK_RX      RST_CLK_PCLK_PORTA

#define MODEM_SPEED                38400 



#define RS485_RX                    0
#define RS485_TX                    1
#define RS_CMD_MAX                  16         //максимальное количество команд в пачке
#define RS_DATA_LEN                 512        //буфер приема ответа от команд

#define RS485_SPEED                 57600
#define RS485_MODE                  "8N1"
#define RS485_MODE_BITS             UART_WordLength8b
#define RS485_MODE_PAR              UART_Parity_No
#define RS485_MODE_STOP             UART_StopBits1
#define RS485_TX_DELAY              3          //задержка в ms после отправки последнего байта

#define SERIAL_TIMEOUT             30                              //ms - время паузы, когда считается, что команда передана
#define SERIAL_DATA_LEN            1024 

#define UART_SERIAL_X              USART2
#define UART_SERIAL_IRQ            UART2_IRQn
#define UART_SERIAL_CLOCK          RST_CLK_PCLK_UART2
	
#define UART_SERIAL_CLOCK_TX       RST_CLK_PCLK_PORTF
#define UART_SERIAL_CLOCK_RX       RST_CLK_PCLK_PORTF
#define RS485_DIR_CLOCK						 RST_CLK_PCLK_PORTF

#define UART_SERIAL_PORT_TX        GPIOA
#define UART_SERIAL_PORT_PinTX     PORT_PIN_3
#define UART_SERIAL_PORT_FuncTX    PORT_FUNC_OVERRID

#define UART_SERIAL_PORT_RX        GPIOA	
#define UART_SERIAL_PORT_PinRX     PORT_PIN_2
#define UART_SERIAL_PORT_FuncRX    PORT_FUNC_OVERRID

#define RS485_DIR_PORT						 GPIOA
#define RS485_DIR_PIN							 GPIO_PIN_1

#define MESS_SIZE                   256
#define CMDDATA_SIZE                256
#define CRC_INIT                    0xFFFF

#define EE_RS485_ADDR               25 //27

//Таймеры
#define TIMERS_MAX                  10

#define TIMER_CONNECT               0
#define TIMER_WDT                   1
#define TIMER_GP_MS                 2
#define TIMER_GP_SEC                3
#define TIMER_TEMP                  4
#define TIMER_POWER                 5
#define TIMER_BAT                   6
#define TIMER_RFID                  7
#define TIMER_LCD                   8
#define TIMER_BUF                   9

//#define TIMER_RTC                   5

#define NS_WATER                    0
#define NS_RTC                      1
#define NS_RFID                     2
#define NS_HEAT_1                   3
#define NS_HEAT_2                   4
#define NS_TEMP                     5
#define NS_CNT_ZERO                 6
#define NS_BAT                      7
//Добавлено**************************

#define NS_I2C											8		//NS по шине I2C (шина занята)
#define NS_M24M01										8		//NS по доступу к памяти через I2C
#define NS_LCD											8		//NS по доступу к дисплею через I2C
#define NS_PN532										8		//NS по доступу ко rfid'у через I2C

//GPIO
#define GP_DT_SAVE

//Максимальное кол-во портов
#define GP_MAX                      19//18

//Виртуальные порты
#define GP_MCU                      1
#define GP_CHARGE                   2
#define GP_BAT_SW                   3
#define GP_POWER_IN                 4
#define GP_BAT_IN                   5
#define GP_KEY_1                    6
#define GP_KEY_2                    7
#define GP_KEY_3                    8
#define GP_PRESS_IN                 9
#define GP_DOOR                     10
#define GP_RFID_KEY                 11
#define GP_HEAT                     12
#define GP_HEAT_CURR1               13
#define GP_HEAT_CURR2               14
#define GP_HEAT_EXT                 15
#define GP_SIM1_ACT                 16
#define GP_SIM2_ACT                 17
#define GP_HEAT_2                   18
#define GP_COUNTER									19

//Аппаратные порты
#define PIN_GP01										GP_MCU
#define PIN_GP01_PORT								GPIOC		//MCU_POWER
#define PIN_GP01_PIN								GPIO_PIN_8

#define PIN_GP02										GP_CHARGE
#define PIN_GP02_PORT								GPIOC		//KEY_CHARGE
#define PIN_GP02_PIN								GPIO_PIN_10

#define PIN_GP03										GP_BAT_SW
#define PIN_GP03_PORT								GPIOC		//BAT_SW
#define PIN_GP03_PIN								GPIO_PIN_11

#define PIN_GP04										GP_POWER_IN
#define PIN_GP04_PORT								GPIOB		//Power voltage
#define PIN_GP04_PIN								GPIO_PIN_1

#define PIN_GP05										GP_BAT_IN
#define PIN_GP05_PORT								GPIOB		//BAT voltage
#define PIN_GP05_PIN								GPIO_PIN_0

#define PIN_GP06										GP_KEY_1
#define PIN_GP06_PORT								GPIOB		//KEY_1
#define PIN_GP06_PIN								GPIO_PIN_14

#define PIN_GP07										GP_KEY_2
#define PIN_GP07_PORT								GPIOB		//KEY_2
#define PIN_GP07_PIN								GPIO_PIN_13

#define PIN_GP08										GP_KEY_3
#define PIN_GP08_PORT								GPIOA		//KEY_3
#define PIN_GP08_PIN								GPIO_PIN_6

#define PIN_GP09										GP_PRESS_IN
#define PIN_GP09_PORT								GPIOA		//Pressure
#define PIN_GP09_PIN								GPIO_PIN_7

#define PIN_GP10										GP_DOOR
#define PIN_GP10_PORT								GPIOB		//Door Open IN
#define PIN_GP10_PIN								GPIO_PIN_2

#define PIN_GP11										GP_RFID_KEY
#define PIN_GP11_PORT								GPIOC		//RFID_KEY
#define PIN_GP11_PIN								GPIO_PIN_0

#define PIN_GP12										GP_HEAT
#define PIN_GP12_PORT								GPIOC		//KEY_HEATER 1
#define PIN_GP12_PIN								GPIO_PIN_9

#define PIN_GP13										GP_HEAT_CURR1
#define PIN_GP13_PORT								GPIOC		//Heat current 1
#define PIN_GP13_PIN								GPIO_PIN_5

#define PIN_GP14										GP_HEAT_CURR2
#define PIN_GP14_PORT								GPIOC		//Heat current 2
#define PIN_GP14_PIN								GPIO_PIN_4

#define PIN_GP15										GP_HEAT_EXT
#define PIN_GP15_PORT								GPIOA		//KEY_HEAT_EXT
#define PIN_GP15_PIN								GPIO_PIN_4

#define PIN_GP16										GP_SIM1_ACT
#define PIN_GP16_PORT								GPIOA		//SIM1_ACT
#define PIN_GP16_PIN								GPIO_PIN_12

#define PIN_GP17										GP_SIM2_ACT
#define PIN_GP17_PORT								GPIOA		//SIM2_ACT
#define PIN_GP17_PIN								GPIO_PIN_11

#define PIN_GP18										GP_HEAT_2
#define PIN_GP18_PORT								GPIOA		//KEY_HEATER 2
#define PIN_GP18_PIN								GPIO_PIN_8


//Счетчик
#define PIN_GP19										GP_COUNTER
#define PIN_GP19_PORT								GPIOA
#define PIN_GP19_PIN								GPIO_PIN_15

//MODES
#define PIN_GP01_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP02_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP03_MODE                GP_MODE_OUT
#define PIN_GP04_MODE                GP_MODE_ADC
#define PIN_GP05_MODE                GP_MODE_ADC
#define PIN_GP06_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP07_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP08_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP09_MODE                GP_MODE_ADC
#define PIN_GP10_MODE                GP_MODE_INU
#define PIN_GP11_MODE                GP_MODE_OUT
#define PIN_GP12_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP13_MODE                GP_MODE_ADC
#define PIN_GP14_MODE                GP_MODE_ADC
#define PIN_GP15_MODE                GP_MODE_OUT
#define PIN_GP16_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP17_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP18_MODE                GP_MODE_OUT//GP_MODE_REG
#define PIN_GP19_MODE								 GP_MODE_INU

//CONF--------------------------------------------------------------------------------------------------
#define PIN_GP01_CONF                1 << GP_CONFBIT_INIT_TRUE // | 1 << GP_CONFBIT_INV_OUT
#define PIN_GP02_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP03_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP04_CONF                1 << GP_CONFBIT_JRN// | GP_BAT_SW << GP_CONFBIT_MINBIND //переключение на батарею при отключении БП
#define PIN_GP05_CONF                0 //1 << GP_CONFBIT_JRN - информацию о заряде мы узнаем по GP_CHARGE
#define PIN_GP06_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP07_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP08_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP09_CONF                1 << GP_CONFBIT_JRN
#define PIN_GP10_CONF                1 << GP_CONFBIT_LOG | 1 << GP_CONFBIT_TPSCAN | 1 << GP_CONFBIT_JRN
#define PIN_GP11_CONF                1 << GP_CONFBIT_INIT_TRUE
#define PIN_GP12_CONF                1 << GP_CONFBIT_INV_OUT | 1 << GP_CONFBIT_INIT_TRUE | 1 << GP_CONFBIT_JRN
#define PIN_GP13_CONF                0 //1 << GP_CONFBIT_JRN
#define PIN_GP14_CONF                0 //1 << GP_CONFBIT_JRN
#define PIN_GP15_CONF                GP_HEAT << GP_CONFBIT_BIND
#define PIN_GP16_CONF                0
#define PIN_GP17_CONF                0
#define PIN_GP18_CONF                1 << GP_CONFBIT_INV_OUT | 1 << GP_CONFBIT_INIT_TRUE | 1 << GP_CONFBIT_JRN // | GP_HEAT << GP_CONFBIT_BIND
#define PIN_GP19_CONF								 1 << GP_CONFBIT_COUNTER

//ADC--------------------------------------------------GigaDevice---------AVR--------------------------------
//Кол-во каналов АЦП
#define ADC_CH_NUM									5

#define PIN_GP04_ADC                //ADC_CH_ADC1				//B1
#define PIN_GP04_ADC_RANK						ADC_REGULAR_RANK_3
#define PIN_GP05_ADC                //ADC_CH_ADC0				//B0
#define PIN_GP05_ADC_RANK						ADC_REGULAR_RANK_2
#define PIN_GP09_ADC                //ADC_CH_ADC4		    //A7
#define PIN_GP09_ADC_RANK						ADC_REGULAR_RANK_1
#define PIN_GP13_ADC                //ADC_CH_ADC2		    //C5
#define PIN_GP13_ADC_RANK						ADC_REGULAR_RANK_5
#define PIN_GP14_ADC                //ADC_CH_ADC3		    //C4
#define PIN_GP14_ADC_RANK						ADC_REGULAR_RANK_4


#define HEAT_SPLIT                  //раздельное управление нагревом

//MODEM
#define PIN_MODEM_POWER_PORT				GPIOB
#define PIN_MODEM_POWER_PIN					GPIO_PIN_9

#define PIN_MODEM_RX_PORT						GPIOA
#define PIN_MODEM_RX_PIN						GPIO_PIN_9

#define PIN_MODEM_TX_PORT						GPIOA
#define PIN_MODEM_TX_PIN						GPIO_PIN_10

#define PIN_MODEM_STATUS_PORT				GPIOB
#define PIN_MODEM_STATUS_PIN				GPIO_PIN_5

#define PIN_MODEM_RESET_PORT				GPIOB
#define PIN_MODEM_RESET_PIN					GPIO_PIN_8

#define PIN_MODEM_CONNECTED_PORT		GPIOB
#define PIN_MODEM_CONNECTED_PIN			GPIO_PIN_4

#define PIN_WDT_PORT								GPIOB
#define PIN_WDT_PIN									GPIO_PIN_3

#define PIN_BELL_PORT								GPIOC
#define PIN_BELL_PIN								GPIO_PIN_13

#define PIN_TERMO_IN_PORT						GPIOA
#define PIN_TERMO_IN_PIN						GPIO_PIN_5

#define PIN_TERMO_OUT_PORT					GPIOA
#define PIN_TERMO_OUT_PIN						GPIO_PIN_0

#define SIM_DETECT_DUAl

#define PIN_SIM1_DETECT_PORT				GPIOC
#define PIN_SIM1_DETECT_PIN					GPIO_PIN_12

#define PIN_SIM2_DETECT_PORT				GPIOD
#define PIN_SIM2_DETECT_PIN					GPIO_PIN_2


/*
//Blink LED
#define PIN_GP01                    0												//PIN_BLINK
#define PIN_GP01_MODE               GP_MODE_OUT
#define PIN_GP01_CONF               1 << GP_CONFBIT_INIT_TRUE
#define PIN_GP01_PORT             	MDR_PORTE
#define PIN_GP01_PIN								PORT_Pin_0

//Test INPUT
#define PIN_GP02                    1
#define PIN_GP02_MODE               GP_MODE_INU
#define PIN_GP02_CONF               1 << GP_CONFBIT_TPSCAN	//Scan every sec (For counter should be ms!)
#define PIN_GP02_PORT             	MDR_PORTA//MDR_PORTC
#define PIN_GP02_PIN								PORT_Pin_5//PORT_Pin_2
//#define PIN_BUTTON									MDR_PORTC, PORT_Pin_2

//Test ADC
#define PIN_GP03										2
#define PIN_GP03_MODE               GP_MODE_ADC
#define PIN_GP03_CONF               1 << GP_CONFBIT_TPSCAN
#define PIN_GP03_PORT             	MDR_PORTD
#define PIN_GP03_PIN								PORT_Pin_3
//#define PIN_ADC6										MDR_PORTD, PORT_Pin_3

//OneWire IN
#define PIN_GP04										3
#define PIN_GP04_MODE               GP_MODE_INU
#define PIN_GP04_CONF               0
#define PIN_GP04_PORT             	MDR_PORTC//MDR_PORTA
#define PIN_GP04_PIN								PORT_Pin_2//PORT_Pin_5

//OneWire OUT
//#define PIN_GP05                    4												
#define PIN_GP05_MODE               GP_MODE_OUT
#define PIN_GP05_CONF               1 << GP_CONFBIT_OPEN_DRAIN
#define PIN_GP05_PORT             	MDR_PORTA
#define PIN_GP05_PIN								PORT_Pin_3

#define PIN_GP03_ADC								ADC_CH_ADC3
*/

#define VCC_PRESENT                 5.00
#define VCC_DELTA                   0.6
#define TIME_24h                    3600*24
#define VCC_OFF_SELF                10.00
#define VCC_TIME_CHARGE             5*60

#define GP_REG_BITS                 16
//#define ADC_CORR                    180  //Correction ADC

#define EROM_BANK                   0x01
#define EROM_SRC_PASSWD             0xAACC5533
#define EROM_ADDR                   0x1F800

#define EE_WDT_MAX                  10 //12

#define EE_gprs_set                 159
#define EE_gprs_apn                 160 //190
#define EE_gprs_usr                 190 //210
#define EE_gprs_pwd                 210 //230

#define EE_PWR_ON                   60 //next 64
#define EE_PWR_OFF                  64 //next 68
#define EE_CHR_ON                   68 //next 72
#define EE_CHR_OFF                  72 //next 76
#define EE_MCU_OFF                  76 //next 80
#define EE_CHR_ON_VOLT              242 //246
#define EE_CHR_OFF_VOLT             246 //250

#define EE_BOARD                    230 //240
#define EE_T1_NUM                   240 //241
#define EE_T2_NUM                   241 //242

#define EE_ROM_SRC                  284 //288
#define EE_ADDR_PORTS               288 //288 + 32*16 (512) = 800

#define MEM_LOG_BANK                3
#define MEM_LOG_ADDR                0x00000000
#define MEM_LOG_NUM                 8192
#define MEM_LOG_SIZE                16

#define EE_ADDR_RESTART             16 //20

#define EE_LOG_CNT                  55 //57

#define ELOG_GP_LOW                 0
#define ELOG_GP_HIGH                1
#define ELOG_GP_ADC_LOW             2
#define ELOG_GP_ADC_NORM            3
#define ELOG_GP_ADC_HIGH            4
#define ELOG_CALL                   5
#define ELOG_SMS                    6
#define ELOG_RESTART                7
#define ELOG_NS_ON                  8
#define ELOG_NS_OFF                 9

#define FLASH_BANK                  0x01
#define SLICE_SIZE                  128

#define EE_FLASH_FLAG               84 //85
#define EE_FLASH_SIZE               85 //89
#define EE_FLASH_HASH               89 //91
#define EE_FLASH_RES                91 //92
#define EE_FLASH_TIME               92 //96
#define EE_FLASH_PWD                96 //100
#define EE_FLASH_CNT                282 //284
#define FLASH_PASS               		"letsdoit"
#define FLASH_CODE               		18
#define FLASH_PASSWD                0xAABBCCDD

#define ONEWIRE_DUALPIN

#define MICROS_LEN	1000

//TWI
#define  TWI_SPEED_HIGH              1000000 //1000000
#define  TWI_SPEED_LOW               125000

//RFID
#define RFID_CHK_TIMEOUT            50        
#define RFID_UID_TIMEOUT            200       
#define RFID_ACT_RETRIES            1         
#define RFID_REMOVE_WAIT            2000

//Tasks
#define TASKS_CMDS_MAX              12
#define TASKS_PARAM_LEN             2
#define TASKS_NAME_LEN              10
#define TASKS_RUN_MAX               600

//Water
#define  WATER_SRV_WAIT              30 //20
#define  WATER_LIT_OUTSIDE           5
#define  WATER_IMP_MIN_MS            10//400

#define EE_WTR_IMPCOST              27 //31
#define EE_WTR_DELAY_ON             31 //33
#define EE_WTR_DELAY_OFF            33 //35
#define EE_WTR_MAX_TIME             35 //37
#define EE_WTR_MAX_LIT              37 //39
#define EE_WTR_MODE                 39 //41
#define EE_BAT_TEST                 41 //42
//#define EE_RFID_KEY                 42 //43
#define EE_TEMP_MIN                 43 //45
#define EE_TEMP_MAX                 45 //47
#define EE_TEMP_SET                 47 //48
#define EE_TEST_MODE                48 //49
#define EE_WTR_SRV                  50 //52
#define EE_WTR_KEY_MODE             52 //53
#define EE_WTR_IMP_MIN              53 //55
#define EE_LOG_CNT                  55 //57

#define EE_SESSION_LAST             80 //next 84

#define MEM_KEYS_ADDR               0x00000000
#define MEM_KEYS_NUM                1024
#define MEM_KEYS_SIZE               32

#define MEM_SPEND_ADDR              0x00010000     //65536
#define MEM_SPEND_NUM               4096
#define MEM_SPEND_SIZE              16

//Modem
#define MODEM_SERVER_IP             "93.95.99.220"
#define MODEM_SERVER_PORT           87
#define MODEM_SEND_BUFF             1024

#define MODEM_SERVER_NAME           "avk.dstc.ru"
#define MODEM_SERVER_PORT           87
#define MODEM_AUTO_CONNECT_VAL      true                //автоподключение к серверу
#define MODEM_START_INTERVAL        5

//Параметры GPRS
#define MODEM_APN                   "internet"
#define MODEM_APN_USER              "gdata"
#define MODEM_APN_PASSWD            "gdata"

#define MODEM_DATA_LEN              256        //буфер приема ответов с модема
#define MODEM_CMD_MS                20         //Минимальное время вывода ответа от модема
#define MODEM_SRV_BUF_LEN           128        //буфер отправки данных (их два)

#define MODEM_SEND_BUFF             1024
#define MODEM_GET_TIMEOUT           50
#define MODEM_SEND_INTERVAL         10

#define TTY_RX_FIFO_SIZE		    128 //256 приводит к вылету
#define TTY_TX_FIFO_SIZE		    128

#define EE_IP_1_name                100
#define EE_IP_1_set                 125
#define EE_IP_1_port                126

#define EE_IP_2_name                130
#define EE_IP_2_set                 155
#define EE_IP_2_port                156

#define EE_ELOG_REC                 250 //252
#define EE_SRV_REC                  252 //254

#define PHONES_ADDR_START           1360
#define PHONES_REC_SIZE             5
#define PHONES_REC_MAX              10 //1360 + 5*10 (50) = 1310

//Температура включения нагревателя
#define  TEMP_HEAT_ON                1

//Температура выключения нагревателя
#define  TEMP_HEAT_OFF               4

#endif /* CONFIG_H_ */
