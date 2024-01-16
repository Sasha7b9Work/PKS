#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

//Версия ПО
#define VERSION                     "7.1"

//#define DEBUG

#define MODEM_PRESENT

#define TIME_MIN                    1505041000

#define FLASH_MAX_SIZE              243712 //131072+112640

#define  zval_8	                    0xFF
#define  zval_16	                  0xFFFF
#define  zval_32	                  0xFFFFFFFF

//#define MICROS_LEN	1000
//#define MODEM_SPEED                38400 

//Tasks
#define TASKS_CMDS_MAX              12
#define TASKS_PARAM_LEN             2
#define TASKS_NAME_LEN              10
#define TASKS_RUN_MAX               600

#define RS485_RX                    0
#define RS485_TX                    1
#define RS_CMD_MAX                  16         //максимальное количество команд в пачке
#define RS_DATA_LEN                 512        //буфер приема ответа от команд

/*
#define RS485_SPEED                 57600
#define RS485_MODE                  "8N1"
#define RS485_MODE_BITS             UART_WordLength8b
#define RS485_MODE_PAR              UART_Parity_No
#define RS485_MODE_STOP             UART_StopBits1
#define RS485_TX_DELAY              3          //задержка в ms после отправки последнего байта
*/

#define SERIAL_TIMEOUT             30                              //ms - время паузы, когда считается, что команда передана
#define SERIAL_DATA_LEN            1024

#define RS485_DIR_PORT						 GPIOB
#define RS485_DIR_PIN							 GPIO_PIN_7

#define MESS_SIZE                   256
#define CMDDATA_SIZE                256
#define CRC_INIT                    0xFFFF

//Таймеры
#define TIMERS_MAX                   8

#define TIMER_CONNECT                0
#define TIMER_WDT                    1
#define TIMER_GP_MS                  2
#define TIMER_GP_SEC                 3
#define TIMER_LED                    4
#define TIMER_NET                    5
#define TIMER_AUTO                   6
#define TIMER_ARCH                   7

#define NS_WATER                     0
#define NS_RTC                       1
#define NS_RFID                      2
#define NS_HEAT_1                    3
#define NS_HEAT_2                    4
#define NS_TEMP                      5
#define NS_CNT_ZERO                  6
#define NS_BAT                       7

//GPIO
#define GP_DT_SAVE

//Максимальное кол-во портов
#define GP_MAX                       4

//Виртуальные порты
#define GP_PORT_1                    1
#define GP_PORT_2                    2
#define GP_PORT_3                    3
#define GP_PORT_4                    4

//Аппаратные порты
#define PIN_GP01										 GP_PORT_1
#define PIN_GP01_PORT								 GPIOB
#define PIN_GP01_PIN								 GPIO_PIN_4

#define PIN_GP02										 GP_PORT_2
#define PIN_GP02_PORT								 GPIOB
#define PIN_GP02_PIN								 GPIO_PIN_3

#define PIN_GP03										 GP_PORT_3
#define PIN_GP03_PORT								 GPIOC
#define PIN_GP03_PIN								 GPIO_PIN_11

#define PIN_GP04										 GP_PORT_4
#define PIN_GP04_PORT								 GPIOC
#define PIN_GP04_PIN								 GPIO_PIN_12


//MODES
#define PIN_GP01_MODE                GP_MODE_INU
#define PIN_GP02_MODE                GP_MODE_INU
#define PIN_GP03_MODE                GP_MODE_INU
#define PIN_GP04_MODE                GP_MODE_INU

//CONF--------------------------------------------------------------------------------------------------
#define PIN_GP01_CONF                0 //1 << GP_CONFBIT_JRN
#define PIN_GP02_CONF                0
#define PIN_GP03_CONF                0
#define PIN_GP04_CONF                0

//ADC--------------------------------------------------GigaDevice---------AVR--------------------------------
//Кол-во каналов АЦП
#define ADC_CH_NUM									1
/*

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
*/

#define PIN_P1_PORT				          GPIOB
#define PIN_P1_PIN					        GPIO_PIN_0

#define PIN_P2_PORT				          GPIOB
#define PIN_P2_PIN					        GPIO_PIN_1

#define PIN_IN1_PORT				        GPIOC
#define PIN_IN1_PIN					        GPIO_PIN_6

#define PIN_IN2_PORT				        GPIOB
#define PIN_IN2_PIN					        GPIO_PIN_13

#define PIN_DS_PORT				          GPIOA
#define PIN_DS_PIN					        GPIO_PIN_12

#define PIN_ST_PORT				          GPIOC
#define PIN_ST_PIN					        GPIO_PIN_7

#define PIN_SH_PORT				          GPIOC
#define PIN_SH_PIN					        GPIO_PIN_8

#define PIN_OE_PORT				          GPIOA
#define PIN_OE_PIN					        GPIO_PIN_15

#define PIN_DS2_PORT				        GPIOC
#define PIN_DS2_PIN					        GPIO_PIN_10

#define PIN_ST2_PORT				        GPIOC
#define PIN_ST2_PIN					        GPIO_PIN_4

#define PIN_SH2_PORT				        GPIOC
#define PIN_SH2_PIN					        GPIO_PIN_5

#define PIN_NET_POWER_PORT				  GPIOC
#define PIN_NET_POWER_PIN					  GPIO_PIN_3

#define PIN_NET_CS_PORT    				  GPIOA
#define PIN_NET_CS_PIN					    GPIO_PIN_1

#define PIN_FLASH_CS_PORT    				GPIOA
#define PIN_FLASH_CS_PIN					  GPIO_PIN_0

#define PIN_FLASH_HLD_PORT    			GPIOA
#define PIN_FLASH_HLD_PIN					  GPIO_PIN_4

//LED
#define PIN_LED_PORT				        GPIOB
#define PIN_LED_PIN					        GPIO_PIN_2

//MODEM
#define PIN_MODEM_POWER_PORT				GPIOA
#define PIN_MODEM_POWER_PIN					GPIO_PIN_8

#define PIN_MODEM_STATUS_PORT				GPIOB
#define PIN_MODEM_STATUS_PIN				GPIO_PIN_14

#define PIN_MODEM_RESET_PORT				GPIOA
#define PIN_MODEM_RESET_PIN					GPIO_PIN_11

#define PIN_MODEM_CONNECTED_PORT		GPIOB
#define PIN_MODEM_CONNECTED_PIN			GPIO_PIN_12

#define PIN_WDT_PORT								GPIOB
#define PIN_WDT_PIN									GPIO_PIN_15

#define SIM_DETECT_ONE
#define PIN_SIM_DETECT_PORT				  GPIOC
#define PIN_SIM_DETECT_PIN					GPIO_PIN_9

#define GP_REG_BITS                 16

#define EROM_BANK                   0x01
#define EROM_ADDR                   0x1F800

#define MEM_LOG_BANK                0x01
#define MEM_LOG_ADDR                0x1B800 //112640
#define MEM_LOG_NUM                 1024
#define MEM_LOG_SIZE                16

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

#define FLASH_BANK_N1               0x00
#define FLASH_BANK_N2               0x01
#define FLASH_BANK_SIZE             131072

#define FLASH_PASS               		"letsdoit"
#define FLASH_CODE               		18
#define FLASH_PASSWD                0xAABBCCDD
#define SLICE_SIZE                  128

//--------------------------------------------------------------------------------------------
#define EE_WDT_MAX                  0   //2
#define EE_BOARD                    2   //12
#define EE_RS485_ADDR               12  //14

#define EE_SET_STEPS                14  //15
#define EE_SET_VOLT_MIN             15  //17
#define EE_SET_VOLT_MAX             17  //19
#define EE_SET_BOOST                19  //20
#define EE_SET_DES_TIME             20  //21
#define EE_SET_POINTS               21  //22
#define EE_SET_KT                   22  //23

#define EE_DEBUG_MESS               23  //24
#define EE_DEBUG_ERROR              24  //25
#define EE_DEBUG_PORTS              25  //26
#define EE_AUTO_VOLT                26  //27

#define EE_SET_RANGE_MIN            27  //29
#define EE_SET_RANGE_MAX            29  //31

#define EE_KT_VA                    31  //35
#define EE_KT_VB                    35  //39
#define EE_KT_VC                    39  //43
#define EE_KT_IA                    43  //47
#define EE_KT_IB                    47  //51
#define EE_KT_IC                    51  //55

#define EE_ADDR_RESTART             55  //59
#define EE_LOG_CNT                  59  //61
#define EE_ELOG_REC                 61  //63

#define EE_FLASH_SIZE               65  //69
#define EE_FLASH_HASH               69  //71
#define EE_FLASH_RES                71  //72
#define EE_FLASH_TIME               72  //76
#define EE_FLASH_PWD                76  //80
#define EE_FLASH_CNT                80  //82
#define EE_FLASH_FLAG               82  //83

#define EE_CONT_LOGIC               83  //89

#define EE_ADDR_PORTS               100 //100 + 32*4 (128) = 228

#define EE_BIND_OUTA                300
#define EE_BIND_OUTB                310
#define EE_BIND_OUTC                320

#define EE_BIND_INA                 330
#define EE_BIND_INB                 340
#define EE_BIND_INC                 350 //360

#define EE_IP_1_name                400
#define EE_IP_1_set                 425
#define EE_IP_1_port                426

#define EE_IP_2_name                430
#define EE_IP_2_set                 455
#define EE_IP_2_port                456

#define EE_gprs_set                 459
#define EE_gprs_apn                 460 //590
#define EE_gprs_usr                 490 //510
#define EE_gprs_pwd                 510 //530

#define EE_STEPS                    560  //next 4*10=600

#define PHONES_ADDR_START           600
#define PHONES_REC_SIZE             5
#define PHONES_REC_MAX              10 //600 + 5*10 (50) = 650

#define EE_STATE_PHASE_A            650  //32*10
#define EE_STATE_PHASE_B            682
#define EE_STATE_PHASE_C            714  //746

#define EE_SHIFT_PHASE_A            746
#define EE_SHIFT_PHASE_B            748
#define EE_SHIFT_PHASE_C            750

#define EE_ARCH_IDX_ADDR            1248 //8*91, 1976 next
//--------------------------------------------------------------------------------------------

//Modem
#define MODEM_SERVER_NAME           "sokol-tm.ru"
#define MODEM_SERVER_PORT           1620

#define MODEM_SEND_BUFF             1024
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

#define TTY_RX_FIFO_SIZE		        128 //256 приводит к вылету
#define TTY_TX_FIFO_SIZE		        128

#define SET_STEPS                   5
#define SET_VOLT_MIN                220
#define SET_VOLT_MAX                240
#define SET_BOOST                   0
#define SET_DES_TIME                5
#define SET_POINTS                  5
#define SET_KT                      1
#define SET_DEBUG_MESS              0
#define SET_DEBUG_ERROR             1
#define SET_DEBUG_PORTS             0
#define SET_AUTO_VOLT               0


#define TASKS_WAIT_BUSY            5   //мс период ожидания освобождения сканирования
#define TASKS_WAIT_INIT            5   //мс ожидание перед измерением, далее будут еще попытки
#define TASKS_WAIT_SCAN            2   //мс ожидание перед сканированием портов
#define TASKS_WAIT_RETRY           10  //мс ожидание перед повтором
#define TASKS_TIMEOUT_1            200 //мс
#define TASKS_TIMEOUT_2            200 //мс
#define TASKS_TIMEOUT_3            200 //мс
#define TASKS_MAX_SCAN_TIME        200000 //мкс

#define NET_SHOW_DEF               'n'
#define NET_BUF_LEN                60
#define NET_MIN_VOLT               150
#define NET_MAX_VOLT               260
#define NET_PERIODS                5
#define NET_RESET_PERIOD           15*60   //раз в 15 минут

#define CONTACTOR_MAX              10      //Максимальное количество контакторов на фазе
#define CONTACTOR_FAIL_TRY         5
#define CONTACTOR_REPAIR_RETRY     60*60   //раз в 60 минут
#define CONTACTOR_WORK_TIME        50      //50ms

#define PHASE_MAX                  3
#define PORTS_MAX                  32

#define ARCH_IDX_REC_MAX            91   //1-91
#define ARCH_IDX_REC_LEN            8
#define ARCH_REC_MAX                131072
#define ARCH_REC_LEN                64
#define ARCH_ADR_MAX                ARCH_REC_MAX*ARCH_REC_LEN

#endif /* CONFIG_H_ */
