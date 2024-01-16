#include "fifo.h"
#include "Serial.h"

uint16_t                     RS485_ADDR     = 1;                      //Сетевой адрес по дефолту

// SERIAL USART-------------------------------------------------------------------------------

static volatile uint16_t     serial_counter;
static volatile uint8_t      serial_new_byte;

//Буфер приема данных команд из консоли и из модема
//Также применяется для отправки запросов на RS485
char                         serial_data[SERIAL_DATA_LEN];

static uint32_t              serial_get_time;                        //RS485_get_time

//--------------------------------------------------------------------------------------

static uint8_t               RS485_running = 0;                     //признак того, что идет запрос на устройство по RS485
static uint32_t              RS485_cmd_time;                        //время старта отправки данных на устройство по RS485, ms

static uint16_t              RS485_cmd_timeout = 250;

static uint8_t               RS485_cmd_index = 0;
static uint8_t               RS485_cmd_count = 0;

static uint8_t               RS485_data[RS_DATA_LEN];               //буфер приема данных от RS485, данные хранятся в виде байтов 512 / 16 = 32
static volatile uint16_t     RS485_index = 0;
static volatile uint16_t     RS485_count = 0;
static volatile uint32_t     RS485_hsumm = 0;

static tp_rs_command_type    rs_commands[RS_CMD_MAX];           //массив информации о командах

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
extern char cmdData[CMDDATA_SIZE];

extern fifo_desc_t   modem_rx_fifo;
extern fifo_desc_t   modem_tx_fifo;

uint8_t str[3];
uint8_t dataReceived=0; // признак данное получено
uint8_t dataTransmitted=1; // признак данное передано

extern uint8_t b_serial;
extern uint8_t b_modem;
extern uint8_t b_modem_tx;

//--------------------------------------------------------------------------------------

void serial_init(void)
{
	/*
	uint32_t uartBaudRate = RS485_SPEED;

	// Структура для инициализации линий ввода-вывода
  PORT_InitTypeDef GPIOInitStruct;

  // Структура для инициализации модуля UART
  UART_InitTypeDef UARTInitStruct;

  // Разрешение тактирования порта F и модуля UART
  RST_CLK_PCLKcmd (UART_SERIAL_CLOCK | UART_SERIAL_CLOCK_TX | UART_SERIAL_CLOCK_RX | RS485_DIR_CLOCK, ENABLE);

  // Общая конфигурация линий ввода-вывода
  PORT_StructInit (&GPIOInitStruct);
  GPIOInitStruct.PORT_SPEED = PORT_SPEED_MAXFAST;
  GPIOInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;

  // Конфигурация и инициализация линии для приема данных 
	GPIOInitStruct.PORT_FUNC  = UART_SERIAL_PORT_FuncRX;
  GPIOInitStruct.PORT_OE    = PORT_OE_IN;
  GPIOInitStruct.PORT_Pin   = UART_SERIAL_PORT_PinRX;
  PORT_Init (UART_SERIAL_PORT_RX, &GPIOInitStruct);

  // Конфигурация и инициализация линии для передачи данных 
  GPIOInitStruct.PORT_FUNC  = UART_SERIAL_PORT_FuncTX;	
  GPIOInitStruct.PORT_OE    = PORT_OE_OUT;
  GPIOInitStruct.PORT_Pin   = UART_SERIAL_PORT_PinTX;
  PORT_Init (UART_SERIAL_PORT_TX, &GPIOInitStruct);
	
	//Конфигурация пина смены направления передачи UART
	GPIOInitStruct.PORT_FUNC  = PORT_FUNC_PORT;
	GPIOInitStruct.PORT_OE    = PORT_OE_OUT;
	GPIOInitStruct.PORT_Pin   = RS485_DIR_PIN;
	PORT_Init (RS485_DIR_PORT, &GPIOInitStruct);

  UARTInitStruct.UART_BaudRate            = uartBaudRate;                  // 
  UARTInitStruct.UART_WordLength          = RS485_MODE_BITS;             // 
  UARTInitStruct.UART_StopBits            = RS485_MODE_STOP;                // 
  UARTInitStruct.UART_Parity              = RS485_MODE_PAR;                // 
  UARTInitStruct.UART_FIFOMode            = UART_FIFO_OFF;                 // 
  UARTInitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE   // 
                                          | UART_HardwareFlowControl_TXE;

  // Инициализация модуля UART
  UART_Init (UART_SERIAL_X, &UARTInitStruct);

  // Выбор предделителя тактовой частоты модуля UART
  UART_BRGInit (UART_SERIAL_X, UART_HCLKdiv1);

  // Выбор источников прерываний (прием и передача данных)
  UART_ITConfig (UART_SERIAL_X, UART_IT_RX | UART_IT_TX, ENABLE);

  // Разрешение работы модуля UART
  UART_Cmd (UART_SERIAL_X, ENABLE);
	
	//Очищаем буфер
	serial_data_clear();
	
	// Назначение приоритета аппаратного прерывания от UART
  NVIC_SetPriority (UART_SERIAL_IRQ, 1);

  // Разрешение аппаратных прерываний от UART
  NVIC_EnableIRQ (UART_SERIAL_IRQ);*/
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = RS485_DIR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  //GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(RS485_DIR_PORT, &GPIO_InitStruct);
	
	serial_data_clear();
}
void serial_timeout_set(uint16_t timeout) {
	RS485_cmd_timeout = timeout;
}
void serial_addr_read(void) {
	uint16_t ival = eeprom_read_uint16(EE_RS485_ADDR);
	if (ival!=zval_16) {
		RS485_ADDR = ival;
	}
}
uint16_t serial_timeout_get(void) {
	return RS485_cmd_timeout;
}
uint8_t serial_rs485_running(void) {
	return RS485_running;
}

void serial_params_read(void) {
	/*
	uint32_t baudrate;
	USART_CHSIZE_t charlength;
	USART_PMODE_t paritytype;
	bool stopbits; //true: 2 stop bits, false: 1 stop bit
	
	char mode[8] = "";
	eeprom_read_buffer(EE_COM_SPEED, (void *)&baudrate, sizeof(baudrate));
	eeprom_read_buffer(EE_CMD_TIMEOUT, (void *)&RS485_cmd_timeout, 2);
	eeprom_read_buffer(EE_COM_MODE, (void *)&mode, 3);
	
	stopbits = false;
	paritytype = USART_PMODE_DISABLED_gc;
	charlength = USART_CHSIZE_8BIT_gc;
	
	if ( !strcmp(mode, "7N1") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		
	} else if ( !strcmp(mode, "7N2") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		stopbits = true;
		
	} else if ( !strcmp(mode, "8N2") ) {
		
		stopbits = true;
		
	} else if ( !strcmp(mode, "7E1") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		paritytype = USART_PMODE_EVEN_gc;
		
	} else if ( !strcmp(mode, "8E1") ) {
		
		paritytype = USART_PMODE_EVEN_gc;
		
	} else if ( !strcmp(mode, "7E2") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		stopbits = true;
		paritytype = USART_PMODE_EVEN_gc;
		
	} else if ( !strcmp(mode, "8E2") ) {
		
		stopbits = true;
		paritytype = USART_PMODE_EVEN_gc;
		
	} else if ( !strcmp(mode, "7O1") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		paritytype = USART_PMODE_ODD_gc;
		
	} else if ( !strcmp(mode, "8O1") ) {
		
		paritytype = USART_PMODE_ODD_gc;
		
	} else if ( !strcmp(mode, "7O2") ) {
		
		charlength = USART_CHSIZE_7BIT_gc;
		stopbits = true;
		paritytype = USART_PMODE_ODD_gc;
		
	} else if ( !strcmp(mode, "8O1") ) {
		
		stopbits = true;
		paritytype = USART_PMODE_ODD_gc;
		
	} else {
		//8N1
	}
	
	USART_SERIAL_OPTIONS.baudrate = baudrate;
	USART_SERIAL_OPTIONS.charlength = charlength;
	USART_SERIAL_OPTIONS.paritytype = paritytype;
	USART_SERIAL_OPTIONS.stopbits = stopbits;
	*/
}

void serial_write_start(void) {
	
	HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_SET);
}
void serial_write(char* strout) {
	
	serial_write_start();
	
	serial_write_str(strout);
	serial_write_char('\r');
	
	serial_write_end();
}
void serial_write_addr(char* strout) {
	
	char str[32];
	
	serial_write_start();
	
	//Адрес
	sprintf(str, "%u%s", RS485_ADDR, sep);
	serial_write_str(str);
	
	//Данные
	serial_write_str(strout);
	
	//КС
	uint16_t crc = crc16(strout, strlen(strout), CRC_INIT);
	sprintf(str, "|%04X", crc);
	serial_write_str(str);
	
	serial_write_char('\r');
	
	serial_write_end();
}
void serial_write_str(char* data) {
	
	uint16_t len = strlen(data);
	
	
	if (len) {
		
		HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 0xFFFF);
		
		/*
		while (len) {
			
			while (UART_GetFlagStatus(UART_SERIAL_X, UART_FLAG_TXFE ) != SET);
			
			UART_SendData(UART_SERIAL_X, (uint16_t) ( *data ));
			
			len--;
			data++;
		}*/
		
		//Line end
		//while (UART_GetFlagStatus(UART_SERIAL_X, UART_FLAG_TXFE ) != SET);
		//UART_SendData(UART_SERIAL_X, '\r');
	}
}
void serial_write_char(uint8_t c) {
	
	HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 0xFFFF);
	
	/*
	while (UART_GetFlagStatus(UART_SERIAL_X, UART_FLAG_TXFE ) != SET);
	UART_SendData(UART_SERIAL_X, c);*/
}
void serial_write_end(void) {
	
	//while (UART_GetFlagStatus(UART_SERIAL_X, UART_FLAG_TXFE ) != SET);
	//while (huart2.gState == HAL_UART_STATE_READY);
	
	//delay_millis(RS485_TX_DELAY); //нужно для RS485 чтобы корректно уходил последний байт
	
	//HAL_Delay(RS485_TX_DELAY);
		
	//прием
	
	//ioport_set_pin_level( RS485_DIR, RS485_RX );
	//PORT_ResetBits(RS485_DIR_PORT, RS485_DIR_PIN);
	HAL_GPIO_WritePin(RS485_DIR_PORT, RS485_DIR_PIN, GPIO_PIN_RESET);
}

void serial_process(void) {
	
	uint8_t b;
	
	uint8_t src = cmd_src_read();
	uint8_t cmdReady = 0;
	
	//Пришли данные по RS
	if (serial_new_byte) {
		
		//serial_write_addr("byte");
		
		serial_get_time = millis(); //время, когда пришли данные
		
		if ( !RS485_running ) {
			if (!src) {
				//New RS485 console data
				src = 1;
				cmd_src_write(src);
				
				//serial_write_addr("src");
			}
			serial_data[serial_counter] = 0x00;
		}
		
		serial_new_byte = 0;        //сброс флага новых данных
	}
	
	if ( RS485_running ) {//Выполняется команда счетчика
		
		//serial_write_addr("rs");
		serial_rs485_process();
	} else {
	
		if (src) {
			if (src==1) {
				//Команда из консоли
				cmdReady = serial_cmd_ready();
				
				//serial_write_addr("cmd");
			}
			
			#ifdef MODEM_PRESENT
			if (src==2 && !modem_serial_task) {
				//Получены данные с модема
				cmdReady = modem_cmd_process();
			}
			#endif
		
			src = cmd_src_read(); //может измениться
		
			//Команда готова к выполнению
			if (cmdReady) {
			
				command_process(serial_data);
				//strcpy(cmdData, serial_data);
			
				//Получен результат выполнения команды
				if (strlen(cmdData)>0) {
				
					if (src==2) {
						
						modem_tcp_write(cmdData);
					
					} else if (src==3) {
						
						//sms
						tp_task_type *task_sms_send = Task_by_name("MSMS");
						if (!Task_is_running(task_sms_send)) {
							serial_write("Send sms");
							serial_write(cmdData);
							
							Task_start_tp(task_sms_send);
							
							//Результат команды копируем в mess
							strcpy(mess, cmdData);
						}
					
					} else {
						//SRC=1, response to console
						serial_write_addr(cmdData);
					}
				
					serial_data_clear();
					clearString(cmdData);
					cmd_src_write(0);
				}
			}
		}
	}
}
void serial_data_clear(void) {
	serial_counter = 0;
	serial_new_byte = 0;
	serial_data[serial_counter] = 0x00;
	
	serial_get_time = 0;
}
void serial_data_add(uint8_t inChar) {
	if (serial_counter < SERIAL_DATA_LEN-2) {
		serial_data [serial_counter] = inChar;
		serial_data[serial_counter+1] = 0x00;
		serial_counter++;
	}
}
uint16_t serial_data_len(void) {
	return serial_counter;
}
uint16_t serial_data_trim(void) {
	Trim(serial_data);
	serial_counter = strlen(serial_data);
	
	return serial_counter;
}

bool serial_is_command(void) {
	
	char     *ret;
	uint16_t len = 0;
	char     addr[8] = "";
	uint32_t addr_int = 0;
	
	if ( !strncmp(serial_data, "<!>", 3) ) {
	
		shiftData(serial_data, 3, SERIAL_DATA_LEN);
		
		serial_data_trim();
		
		return true;
	
	} else {
	
		ret = strstr(serial_data, "|");
		if (ret!=NULL) {
			len = ret - serial_data; //+1
		
			if (len <=5) {
			
				subString (serial_data, 0, len, addr);
				sscanf(addr, "%u", &addr_int);
			
				//if ( !strcmp(addr, RS485_ADDR)) {
				if ( addr_int==RS485_ADDR) {
				
					shiftData(serial_data, len+1, SERIAL_DATA_LEN);
					
					serial_data_trim();
				
					return true;
				}
			}
		}
	}
	return false;
}

uint8_t serial_cmd_ready(void) {
	
	uint32_t time = millis();
	uint8_t  is_end = 0;
	uint8_t  cmdReady = 0;
	
	
	if (time - serial_get_time > SERIAL_TIMEOUT || time < serial_get_time) {
		is_end = 1;
	}
	
	if (is_end) {
		
		if (serial_is_command()) {
			cmdReady = 1;
			clearString(cmdData);
		} else {
			//Не команда
			serial_data_clear();
			cmd_src_write(0);
		}
	}
	return cmdReady;
}

void serial_rs485_init(uint8_t cmd_index, uint16_t data_index) {
	rs_commands[cmd_index-1].start = data_index;
	rs_commands[cmd_index-1].len = 0;
	rs_commands[cmd_index-1].sum = 0;
}

void serial_rs485_save(uint8_t cmd_index, uint16_t len, uint16_t sum) {
	rs_commands[cmd_index-1].len = len;
	rs_commands[cmd_index-1].sum = sum;
}
void serial_rs485_prepare(char* cmd_data)
{
	uint16_t cmd_start = 0;
	uint16_t cmd_len   = 0;
	
	RS485_cmd_index = 1;

	//Количество команд
	for (RS485_cmd_count = 1; RS485_cmd_count <= RS_CMD_MAX; RS485_cmd_count++) {
		
		paramStringNum(cmd_data, RS485_cmd_count, &cmd_start, &cmd_len);
		
		//sprintf(mess,"index = %u, start = %u, len = %u", RS485_cmd_count, cmd_start, cmd_len);
		//serial_write(mess);
		
		if (!cmd_len) {
			RS485_cmd_count--;
			break;
		}
	}

	//Не больше лимита
	if (RS485_cmd_count > RS_CMD_MAX) {
		RS485_cmd_count = RS_CMD_MAX;
	}

	//Очищаем буфер приема
	memset( RS485_data, 0, RS_DATA_LEN );
	
	//memset( rs_test, 0, RS_TEST_LEN );
	//rs_test_idx = 0;

	//Ставим указатель данных в начало
	RS485_index = 0;

	//Отправляем первую команду
	serial_rs485_send();
}

void serial_rs485_send(void)
{
	uint8_t  sendb     = 0;
	uint16_t cmd_start = 0;
	uint16_t cmd_len   = 0;
	uint16_t i         = 0;
	
	//Инициализируем ответ
	serial_rs485_init(RS485_cmd_index, RS485_index);
	
	//Берем команду
	paramStringNum(serial_data, RS485_cmd_index, &cmd_start, &cmd_len);
	
	//Отправляем команду
	serial_write_start();

	for (i = cmd_start; i < cmd_start+cmd_len; i+=2) {
		sendb = toHex(serial_data[i+1]) + (toHex(serial_data[i]) << 4);
		serial_write_char(sendb);
	}
	serial_write_end();
	
	//Готовимся к приему данных
	RS485_count = 0;
	RS485_hsumm = 0;
	
	//Флаг того, что идет запрос
	RS485_running = 1;

	RS485_cmd_time = millis();
	
	//Очищать буфер нельзя, но нужно сбросить время приема данных
	serial_get_time = 0;
}

void serial_rs485_reply(void) {
	
}
void serial_rs485_process(void) {
	
	uint32_t time = 0;
	uint8_t  is_end = 0;
	
	time = millis();

	if (serial_get_time) { //Есть какие-то данные
		if (time - serial_get_time > SERIAL_TIMEOUT || time < serial_get_time) {
			is_end = 1;
		}
	} else {
		if (time - RS485_cmd_time > RS485_cmd_timeout || time < RS485_cmd_time) {
			is_end = 1;
		}
	}

	if (is_end) { //Отправляем результат по таймауту
	
		//Записываем результат
		serial_rs485_save(RS485_cmd_index, RS485_count, RS485_hsumm);
		
		//sprintf(mess,"\rCmd index = %u, count = %u", RS485_cmd_index, RS485_cmd_count);
		//serial_write(mess);
	
		//Если есть еще команды
		if (RS485_cmd_index < RS485_cmd_count) {
		
			//Следующая команда счетчика
			RS485_cmd_index++;
		
			//Пауза
			//delay_ms(10);
				
			serial_rs485_send();
		} else {
			serial_rs485_reply();
			
			//Окончание
			serial_data_clear();
			clearString(cmdData);
			cmd_src_write(0);
			
			RS485_running = 0;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  if(huart == &huart2) {
		
		serial_new_byte = 1;
		
		if (RS485_running) {
			
			if (RS485_index < RS_DATA_LEN) {
				
				RS485_data[RS485_index] = b_serial;
				
				RS485_index++;
				RS485_count++;
				RS485_hsumm += b_serial;
			}
			
		} else if (serial_counter < SERIAL_DATA_LEN-2) {
			serial_data[serial_counter] = b_serial;
			serial_counter++;
		}
		
		HAL_UART_Receive_IT(&huart2, &b_serial, 1);
		
  } else if (huart == &huart1) {

    if (!fifo_is_full(&modem_rx_fifo))
		{
			fifo_push_uint8_nocheck(&modem_rx_fifo, b_modem);
		}
		
		HAL_UART_Receive_IT(&huart1, &b_modem, SINGLE_BYTE);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{
    if (!fifo_is_empty(&modem_tx_fifo))
		{
			b_modem_tx = fifo_pull_uint8_nocheck(&modem_tx_fifo);
			HAL_UART_Transmit_IT(&huart1, &b_modem_tx, SINGLE_BYTE);
		}
  }
}