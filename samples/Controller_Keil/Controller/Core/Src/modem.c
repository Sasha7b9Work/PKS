#include "modem.h"
/*
 * modem.c
 *
 * Created: 18.01.2017 23:49:18
 *  Author: Hyper
 */
 

bool     MODEM_AUTO_CONNECT = MODEM_AUTO_CONNECT_VAL;
bool     MODEM_ALIVE        = false;
bool     MODEM_NET_REG      = false;
bool     MODEM_GPRS         = false;
bool     MODEM_CONNECTED    = false;
bool     modem_is_sending   = false;

uint32_t MODEM_LAST_SEND = 0;
uint32_t MODEM_LAST_CHECK = 0;
uint32_t MODEM_LAST_ALIVE = 0;

uint8_t  modem_sim = 1;

uint32_t  modem_serial_task    = 0;
uint32_t  modem_hello_time     = 0;

char     modem_data[MODEM_DATA_LEN] = ""; //Буфер для ответов на AT команды модема. Вроде как достаточно 256 байт

char     modem_srv_b1[MODEM_SRV_BUF_LEN] = "";
char     modem_srv_b2[MODEM_SRV_BUF_LEN] = "";

uint16_t modem_get_timeout = 50;
uint32_t modem_get_time = 0;

char     modem_imei[20] = "";
char     modem_lat[16] = "";
char     modem_lon[16] = "";
char     modem_csq[16] = "";
char     modem_phone[20] = "";
char     modem_ip[16] = "";


extern UART_HandleTypeDef huart1;

fifo_desc_t   modem_rx_fifo;
uint8_t       modem_rx_buffer[TTY_RX_FIFO_SIZE];

fifo_desc_t   modem_tx_fifo;
uint8_t       modem_tx_buffer[TTY_TX_FIFO_SIZE];

extern uint8_t b_modem_tx;

//Строки
const char ms0[] = "AT+CREG?";
const char ms1[] = "AT+GSN";
const char ms2[] = "AT+SAPBR=2,1";
const char ms3[] = "+SAPBR: 1,1,";
const char ms4[] = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
const char ms5[] = "AT+SAPBR=3,1,\"APN\",\"%s\"";
const char ms6[] = "AT+SAPBR=3,1,\"USER\",\"%s\"";
const char ms7[] = "AT+SAPBR=3,1,\"PWD\",\"%s\"";
const char ms8[] = "AT+SAPBR=1,1";
const char ms9[] = "ATE0";
const char ms10[] = "+CREG: 0,1";
const char ms11[] = "AT+CIPSTART=\"TCP\",\"%s\",%d";
const char ms12[] = "CONNECT OK";
const char ms13[] = "AT+CMGD=1,4";
const char ms14[] = "AT+CIPSEND";
const char ms15[] = "SEND OK";
const char ms16[] = "AT+CSQ";
const char ms17[] = "AT+CIPGSMLOC=1,1";
const char ms18[] = "AT+CIPSTATUS";
const char ms19[] = "AT+CNMI=1,2,0,0,0";
const char ms20[] = "modem|status|";
const char ms21[] = "IMEI: %s, CSQ: %s";
const char ms22[] = "AT+CSCS=\"GSM\"";
const char ms23[] = "BUSY";
const char ms24[] = "ATD%s;";
const char ms25[] = MODEM_SERVER_NAME;
const char ms26[] = "+CIPGSMLOC: ";
const char ms27[] = "lat:%s|lon:%s";
const char ms28[] = "Connecting to %s:%d";
const char ms29[] = "CONNECT FAIL";
const char ms30[] = "no_pwr_status";
const char ms31[] = "reseting";
const char ms32[] = "started";
const char ms33[] = "power_off";
const char ms34[] = "no_at";
const char ms35[] = "no_cmd_reply";
const char ms36[] = "no_creg";
const char ms37[] = "GPRS";
const char ms38[] = "gprs|ok";
const char ms39[] = "no_gprs";
const char ms40[] = "power_on";
const char ms41[] = "no_connect";
const char ms42[] = "srv|con";
const char ms43[] = "srv|dis";
const char ms44[] = "Calling %s";
const char ms45[] = "AT";
const char ms46[] = "sms|";
const char ms47[] = "AT+CMGF=1";
const char ms48[] = "OK";
const char ms49[] = "AT+CMGS=\"%s\"";
const char ms50[] = MODEM_APN;
const char ms51[] = MODEM_APN_USER;
const char ms52[] = MODEM_APN_PASSWD;
const char ms53[] = "idle|";
const char ms54[] = "srvdata|";
const char ms55[] = "New SMS:";
const char ms56[] = "Data:";
const char ms57[] = "cmd|";
const char ms58[] = "AT+CIPSEND=%u";
const char ms59[] = "AT+CIICR";
const char ms60[] = "AT+CIFSR";
const char ms61[] = "AT+CIPSHUT";
const char ms62[] = "AT+CSTT=\"%s\",\"%s\",\"%s\"";
const char ms63[] = "SHUT OK";
const char ms64[] = "SEND ERROR";

const char* const modem_str_tbl[] = {
	ms0, ms1, ms2, ms3, ms4, ms5, ms6, ms7, ms8, ms9, ms10, ms11, ms12, ms13, ms14, ms15, ms16, ms17, ms18, ms19,
	ms20, ms21, ms22, ms23, ms24, ms25, ms26, ms27, ms28, ms29, ms30, ms31, ms32, ms33, ms34, ms35, ms36, ms37,
	ms38, ms39, ms40, ms41, ms42, ms43, ms44, ms45, ms46, ms47, ms48, ms49, ms50, ms51, ms52, ms53, ms54, ms55,
	ms56, ms57, ms58, ms59, ms60, ms61, ms62, ms63, ms64
};

void modem_flash(char* buff, uint8_t i) {
	//strcpy_P(buff, (char*)pgm_read_word(&(modem_str_tbl[i])));
	strcpy(buff, modem_str_tbl[i]);
}

void modem_task_check() {
	
	#ifdef MODEM_PRESENT
	
	//Таск запуска модема
	tp_task_type *task_modem_start = Task_by_name("MStart");

	//Таск отправки данных
	tp_task_type *task_modem_send = Task_by_name("MSrvS");
	
	//Контроль времени, не более 150 сек
	if (modem_serial_task) {
		if (now() - modem_serial_task > 150) {
			modem_serial_task = 0;
		}
	}
	
	if (!MODEM_CONNECTED && MODEM_AUTO_CONNECT) {
		if (! Task_is_running(task_modem_start) ) {
			if (now() - task_modem_start->dt_end >= MODEM_START_INTERVAL) {
				Task_start_tp(task_modem_start);
			}
		}
	}
	if (MODEM_CONNECTED && modem_hello_time) {
		//Не чаще, чем раз в 10 сек (30 сек очень долго, запрос ключа с сервера сваливается в таймаут)
		if (! Task_is_running(task_modem_send) && modem_srv_write_ready() && now() - task_modem_send->dt_end >= MODEM_SEND_INTERVAL && now() - modem_hello_time > 3) {
			
			char cmd[32];
			
			modem_tcp_status(cmd, sizeof(cmd));
			
			if (!strncmp(cmd, "CONNECT OK", 10)) {
				Task_start_tp(task_modem_send);
			}
		}
	}
	#endif
}

uint8_t modem_tcp_write_start(void) {

	if (!MODEM_CONNECTED) {
		return 0;
	}
	
	uint8_t answer = 0;
	
	//Режим отправки
	
	for (uint8_t n = 1; n <= 3; n++) {
		
		answer = modem_at("AT+CIPSEND", ">", 5000);
		if ( answer ) {
			modem_is_sending = true;
			break;
		}
	}
	return answer;
}

uint8_t modem_tcp_write_end(void) {
	
	char cmd_1[16];
	
	modem_write_char(0x1A);

	modem_flash(cmd_1, 15); //SEND OK
	uint8_t ret = modem_waitResp(cmd_1, lbl_err, 20000);
	
	modem_is_sending = false;
	
	return ret;
}

uint8_t modem_tcp_write(char* string) {
	
	if (!MODEM_CONNECTED) {
		modem_is_sending = false;
		return 0;
	}

	uint8_t answer = 0;
	
	char cmd_1[32];
	char cmd_2[32];
	
	addChar(string, 0x0D);
	
	modem_flash(cmd_2, 58); //AT+CIPSEND=%u
	
	//Режим отправки
	sprintf(cmd_1, cmd_2, strlen(string));
	
	for (uint8_t n = 1; n <= 3; n++) {
		
		answer = modem_at(cmd_1, ">", 5000);
		if ( answer ) {
			modem_is_sending = true;
			break;
		}
	}

	if (answer) {
		
		modem_write(string);
		
		modem_flash(cmd_1, 15); //SEND OK
		
		answer = modem_waitResp(cmd_1, lbl_err, 20000);
	
		modem_is_sending = false;
		
		if (answer) {
			#ifdef DEBUG
				modem_status("SEND OK");
			#endif
			
			MODEM_LAST_SEND = now();
			MODEM_LAST_CHECK = now();
		} else {
			
			#ifdef DEBUG
				modem_status("SEND FAIL");
			#endif
		}
	} else {
		//serial_write("start failed");
	}
	
	return answer;
}

void modem_tcp_status(char *ret, int ret_len) {
	
	char cmd[16];
	
	modem_flash(cmd, 18); //AT+CIPSTATUS
	modem_at_num(cmd, 2, 2000, ret, ret_len);
	
	char dest[16];
	subString (ret, 0, 7, dest);

	if (!strcmp(dest,"STATE: ")) {
		
		shiftString(ret, 7);
	}
	//return ret;
}

void modem_connected(void) {
	
	HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_SET);
	
	MODEM_LAST_CHECK = now();
	
	MODEM_ALIVE = true;
	MODEM_NET_REG = true;
	MODEM_GPRS = true;
	MODEM_CONNECTED = true;

	modem_status_n(42);//srv|con
}

bool modem_is_connected(void) {
	return MODEM_CONNECTED;
}

void modem_disconnected(void) {
	
	MODEM_ALIVE = false;
	MODEM_NET_REG = false;
	MODEM_CONNECTED = false;
	
	modem_hello_time = 0;
	modem_is_sending = false;
	
	HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
	
	modem_status_n(43);//srv|dis
}

uint8_t modem_at(char* ATcommand, char* expected_answer, uint16_t timeout) {

	uint32_t previous = 0;
	uint32_t delay = 0;
	uint8_t  answer = 0;
	uint8_t  inChar = 0;
	uint32_t wdt_delay = 0;

	// Clean the input buffer
	modem_serial_clean();
	
	clearString(modem_data);

	modem_write(ATcommand);
	
	#ifdef DEBUG
		serial_write(ATcommand);
	#endif

	previous = wdt_delay = millis();

	// this loop waits for the answer
	do {
		if (!fifo_is_empty(&modem_rx_fifo)) {
			while (!fifo_is_empty(&modem_rx_fifo)) {
				//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					inChar = fifo_pull_uint8_nocheck(&modem_rx_fifo);
				//}
				
				if (strlen(modem_data) < MODEM_DATA_LEN-1) {
					addChar(modem_data, inChar);
				}

				if (strstr(modem_data, expected_answer) != NULL) {
					answer = 1;
				}
			}
		}
		delay = millis() - previous;
		
		if (millis() - wdt_delay > 100) {
			wdt_delay = millis();
	
			//Reset WDT
			sys_wdt_reset();
		}
	}
	// Waits for the asnwer with time out
	while ( (!answer && delay < timeout) || delay < MODEM_CMD_MS);
	
	#ifdef DEBUG
		serial_write(modem_data);
	#endif
	
	modem_serial_clean();
	clearString(modem_data);
	
	return answer;
}

uint8_t modem_at_try(char* ATcommand, char* expected_answer, uint16_t timeout, uint8_t trys) {
	
	uint8_t  answer = 0;
	
	for (uint8_t n = 1; n <= trys; n++) {

		answer = modem_at(ATcommand, expected_answer, timeout);
		
		if (answer) {
			return 1;
		}
	}
	return answer;
}

void modem_at_num(char* ATcommand, uint16_t num, uint16_t timeout, char* response, uint16_t max) {

	uint32_t previous = 0;
	uint32_t delay = 0;
	uint8_t  answer = 0;
	uint8_t  n = 0;
	uint8_t  inChar = 0;
	uint32_t wdt_delay = 0;

	// Clean the input buffer
	modem_serial_clean();
	clearString(response);

	modem_write(ATcommand);
	
	#ifdef DEBUG
		serial_write(ATcommand);
	#endif

	previous = wdt_delay = millis();

	// this loop waits for the answer
	do {
		if (!fifo_is_empty(&modem_rx_fifo)) {
			while (!fifo_is_empty(&modem_rx_fifo)) {
				//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					inChar = fifo_pull_uint8_nocheck(&modem_rx_fifo);
				//}
				
				if (inChar == 0x0D) {
					Trim(response);
					if (strlen(response) > 0) {
						n++;
						if (n == num) {
							answer = 1;
						} else if (answer == 0) {
							clearString(response);
						}
					}
				} else if (answer == 0 && strlen(response) < max-2) {
					addChar(response, inChar);
				}
			}
		}
		if (millis() - wdt_delay > 100) {
			wdt_delay = millis();
			
			//Reset WDT
			sys_wdt_reset();
		}
		delay = millis() - previous;
	}
	// Waits for the asnwer with time out
	while ((!answer && delay < timeout) || delay < MODEM_CMD_MS);
	
	#ifdef DEBUG
		serial_write(response);
	#endif
	
	modem_serial_clean();
}

uint8_t modem_waitResp(char* expected_answer_1, char* expected_answer_2, uint16_t timeout) {

	uint32_t previous = 0;
	uint32_t delay = 0;
	uint8_t  answer = 0;
	uint8_t  inChar = 0;
	uint32_t wdt_delay = 0;

	// Clean the input buffer
	//modem_serial_clean();


	previous = wdt_delay = millis();

	// this loop waits for the answer
	do {
		if (!fifo_is_empty(&modem_rx_fifo)) {
			while (!fifo_is_empty(&modem_rx_fifo)) {
				//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					inChar = fifo_pull_uint8_nocheck(&modem_rx_fifo);
				//}
				
				if (strlen(modem_data) < MODEM_DATA_LEN-1) {
					addChar(modem_data, inChar);
				}

				if (strstr(modem_data, expected_answer_1) != NULL) {
					answer = 1;
				}
				
				if (strstr(modem_data, expected_answer_2) != NULL) {
					answer = 2;
				}
			}
		}
		delay = millis() - previous;
		
		if (millis() - wdt_delay > 100) {
			wdt_delay = millis();
			
			//Reset WDT
			sys_wdt_reset();
		}
	}
	// Waits for the asnwer with time out
	while ( (!answer && delay < timeout) || delay < MODEM_CMD_MS);
	
	#ifdef DEBUG
		serial_write(modem_data);
	#endif
	
	modem_serial_clean();
	clearString(modem_data);
	
	return answer;
}

bool modem_data_check(char* answer) {
	
	bool     ret = false;

	if (modem_data_ready()) {
		
		if (strstr(modem_data, answer) != NULL) {
			ret = true;
		
			clearString(modem_data);
			modem_serial_task = 0;
		}
	}
	return ret;
}

bool modem_data_ready(void) {
	
	uint32_t time = millis();
	//uint8_t  is_end = 0;
	bool     ret = false;

	if (modem_get_time) { //Есть какие-то данные
		if (time - modem_get_time > modem_get_timeout || time < modem_get_time) {
			ret = true;
		}
	}
	return ret;
}

bool modem_data_num(uint16_t num, char* response, uint16_t max) {
	
	bool     ret = false;
	uint8_t  i = 0;
	uint8_t  answer = 0;
	uint16_t  len = 0;

	if (!modem_data_ready()) {
		return false;
	}
	
	clearString(response);
		
	len = strlen(modem_data);
	if (len > 0) {
		for (uint16_t n = 0; n < len; n++) {
				
			if (modem_data[n] == 0x0D || n==len-1) {
				Trim(response);
				if (strlen(response) > 0) {//не было пустой строки
					i++;
					if (i == num) {
						//Строка найдена, выходим
						answer = 1;
						break;
					} else if (answer == 0) {
						clearString(response);
					}
				}
			} else if (answer == 0 && strlen(response) < max-2) {
				addChar(response, modem_data[n]);
			}
				
		}
	}
	if (answer) {
		clearString(modem_data);
		modem_serial_task = 0;
		ret = true;
	}
	
	return ret;
}

void modem_status(char* s) {
	char msg[64] = "";
	modem_flash(msg, 20); //modem|status|
	
	strcat(msg, s);
	serial_write_addr(msg);
}

void modem_status_n(uint8_t n) {
	char str[64] = "";
	modem_flash(str, n);
	
	modem_status(str);
}

void modem_write(char* mess) {
	
	modem_write_str(mess);
	modem_write_char('\r');
	
	//while (UART_GetFlagStatus(MODEM_SERIAL, UART_FLAG_TXFE ) != SET);
	//while (huart1.gState == HAL_UART_STATE_READY);
	
	//delay_millis(RS485_TX_DELAY);
}

void modem_write_prepare(void) {
	
	// Clean the input buffer
	modem_serial_clean();
	clearString(modem_data);
	
	modem_serial_task = now();
	
	modem_get_time = 0;
}
void modem_write_tx(char* str) {
	
	//Очищаем
	
	while (!fifo_is_empty(&modem_tx_fifo)) {
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			fifo_pull_uint8_nocheck(&modem_tx_fifo);
		//}
	}
	
	//Заполняем
	for (uint8_t n = 0; n < strlen(str); n++) {
		if (fifo_is_full(&modem_tx_fifo)) {break;}
		
		fifo_push_uint8_nocheck(&modem_tx_fifo, str[n]);
	}
	
	//Конец строки
	if (!fifo_is_full(&modem_tx_fifo)) {
		fifo_push_uint8_nocheck(&modem_tx_fifo, '\r');
	}
	
	//Отправляем первый байт
	//usart_put(MODEM_SERIAL, fifo_pull_uint8_nocheck(&modem_tx_fifo));
	//UART_SendData(MODEM_SERIAL, fifo_pull_uint8_nocheck(&modem_tx_fifo));
	b_modem_tx = fifo_pull_uint8_nocheck(&modem_tx_fifo);
	HAL_UART_Transmit_IT(&huart1, &b_modem_tx, SINGLE_BYTE);
	
	//Врубаем прерывание
	//usart_set_dre_interrupt_level(MODEM_SERIAL, TTY_INTERRUPT_LEVEL);
}
	
void modem_write_char(uint8_t c) {
	while (HAL_UART_Transmit(&huart1, &c, SINGLE_BYTE, HAL_MAX_DELAY) != HAL_OK)
	{
	}
}
void modem_write_esc(void) {
	modem_write_char(0x0D);
	modem_write_char(0x1A);
}
void modem_write_str(char* str) {
	
	uint16_t len = strlen(str);
	
	if (len) {
		
		while (HAL_UART_Transmit(&huart1, (uint8_t*)str, len, HAL_MAX_DELAY) != HAL_OK)
		{
		}
	}
}

void modem_serial_init(void) {
	
	fifo_init(&modem_rx_fifo, modem_rx_buffer, TTY_RX_FIFO_SIZE);
	fifo_init(&modem_tx_fifo, modem_tx_buffer, TTY_TX_FIFO_SIZE);
	
	/*
	RST_CLK_PCLKcmd (MODEM_SERIAL_CLOCK | MODEM_SERIAL_CLOCK_TX | MODEM_SERIAL_CLOCK_RX, ENABLE);
	
	PORT_InitTypeDef GPIOInitStruct;
	
	PORT_StructInit (&GPIOInitStruct);
  GPIOInitStruct.PORT_SPEED = PORT_SPEED_MAXFAST;
  GPIOInitStruct.PORT_MODE  = PORT_MODE_DIGITAL;

  // Eiioeao?aoey e eieoeaeecaoey eeiee aey i?eaia aaiiuo 
	GPIOInitStruct.PORT_FUNC  	= PORT_FUNC_OVERRID;
  GPIOInitStruct.PORT_OE    	= PORT_OE_IN;
  GPIOInitStruct.PORT_Pin   	= PIN_MODEM_RX_PIN;
	GPIOInitStruct.PORT_PULL_UP = PORT_PULL_UP_ON;
  PORT_Init (PIN_MODEM_RX_PORT, &GPIOInitStruct);

  // Eiioeao?aoey e eieoeaeecaoey eeiee aey ia?aaa?e aaiiuo 
  GPIOInitStruct.PORT_FUNC  = PORT_FUNC_OVERRID;	
  GPIOInitStruct.PORT_OE    = PORT_OE_OUT;
  GPIOInitStruct.PORT_Pin   = PIN_MODEM_TX_PIN;
  PORT_Init (PIN_MODEM_TX_PORT, &GPIOInitStruct);
	
	UART_InitTypeDef UARTInitStruct;
	
	UARTInitStruct.UART_BaudRate            = MODEM_SPEED;                  // 
  UARTInitStruct.UART_WordLength          = RS485_MODE_BITS;             // 
  UARTInitStruct.UART_StopBits            = RS485_MODE_STOP;                // 
  UARTInitStruct.UART_Parity              = RS485_MODE_PAR;                // 
  UARTInitStruct.UART_FIFOMode            = UART_FIFO_OFF;                 // 
  UARTInitStruct.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE   // 
                                          | UART_HardwareFlowControl_TXE;
	
	UART_Init (MODEM_SERIAL, &UARTInitStruct);
	
	UART_BRGInit (MODEM_SERIAL, UART_HCLKdiv1);
	
	UART_ITConfig (MODEM_SERIAL, UART_IT_RX | UART_IT_TX, ENABLE);
	
	UART_Cmd (MODEM_SERIAL, ENABLE);
	
	NVIC_SetPriority (MODEM_SERIAL_IRQ, 1);
	NVIC_EnableIRQ (MODEM_SERIAL_IRQ);*/


	//usart_init_rs232(MODEM_SERIAL, &USART_MODEM_OPTIONS);
	//usart_set_rx_interrupt_level(MODEM_SERIAL, TTY_INTERRUPT_LEVEL);
	//usart_set_dre_interrupt_level(MODEM_SERIAL, USART_INT_LVL_OFF);
}

void modem_serial_clean(void) {
	
	if (!fifo_is_empty(&modem_rx_fifo)) {
		while (!fifo_is_empty(&modem_rx_fifo)) {
			//ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				fifo_pull_uint8_nocheck(&modem_rx_fifo);
			//}
		}
	}
}

void modem_power_on(void) {
	
	  HAL_GPIO_WritePin(PIN_MODEM_POWER_PORT, PIN_MODEM_POWER_PIN, GPIO_PIN_SET);

}

void modem_power_off(void) {
	
		HAL_GPIO_WritePin(PIN_MODEM_POWER_PORT, PIN_MODEM_POWER_PIN, GPIO_PIN_RESET);
}

void modem_connect_check(void) {
	
  char cmd_1[32];
  char cmd_2[32];
  uint16_t delay = 0;
	
  if (!MODEM_ALIVE) {
	  #ifdef DEBUG
		modem_status("not_alive");
	  #endif
	  
	  HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
		return;
  }
  
  //Включен ли модем
	if (!HAL_GPIO_ReadPin(PIN_MODEM_STATUS_PORT, PIN_MODEM_STATUS_PIN)) {
		#ifdef DEBUG
		modem_status("zero_status");
	  #endif
	  
	  modem_disconnected();
	  return;
  }
  
  if (cmd_src_read() || modem_serial_task || modem_is_sending) {
	  return;
  }
  
  //Статус соединения
  if (MODEM_LAST_CHECK) {
	  delay = now() - MODEM_LAST_CHECK;
	  if ( delay >= 60 - 2) {
		  
		//Получен ли ответ от сервера
		if (!modem_hello_time) {
			#ifdef DEBUG
				modem_status("no_hello");
			#endif
			
			modem_disconnected();
			return;
		}
		  
		  MODEM_LAST_CHECK = now();
		  
		  modem_tcp_status(cmd_2, sizeof(cmd_2));
		  
		  modem_flash(cmd_1, 12); //CONNECT OK
		  
		  if (!strncmp(cmd_2, cmd_1, 10)) {
			  #ifdef DEBUG
				modem_status(cmd_2);
			  #endif
			  
			  //OK
			  
			  /*
			  modem_flash(cmd_1, 60); //AT+CIFSR
			  
			  if (modem_at(cmd_1, lbl_err, 5000)) {
				  #ifdef DEBUG
				  modem_status("GPRS fail");
				  #endif
				  
				  //Нет GPRS, отключаем модем
				  modem_disconnected();
			  }
			  */
		  } else {
			  #ifdef DEBUG
				modem_status(cmd_1);
			  #endif
			  modem_disconnected();
		  }
	  } else {
		  //sprintf(mess,"Wait 60 sec - %u spend",delay);
		  //serial_write(mess);
	  }
  }

  //Если все ок, шлем IDLE на сервер
  if (MODEM_CONNECTED && MODEM_LAST_SEND) {
	  
	  delay = now() - MODEM_LAST_SEND;
	  
	  if (delay >= 120 - 2) {
		  
		  modem_flash(mess, 53); //idle|
		  
		  modem_flash(cmd_1, 16); //AT+CSQ
		  modem_at_num(cmd_1, 1, 2000, cmd_2, sizeof(cmd_2));
		  shiftString(cmd_2, 6);
		  
		  strcat(mess, cmd_2);
		  
			/*
		  strcat(mess, sep);
		  
		  //dtostrf( Temperature_body(), 3, 2, cmd_2 );
			sprintf(cmd_2, "%0.2f", (double)Temperature_body());
		  strcat(mess, cmd_2);
			*/
		  
		  #ifdef DEBUG
		    modem_status(mess);
		  #endif
		  
		  if (!modem_tcp_write( mess )) {
			  #ifdef DEBUG
				modem_status("Write fail");
			  #endif
			  
			  //Не удалось записать, сброс модема
			  modem_disconnected();
		  } else {
			  #ifdef DEBUG
				modem_status("Write ok");
			  #endif
		  }
	  } else {
		  #ifdef DEBUG
			sprintf(mess,"Wait 120 sec - %u spend",delay);
			serial_write(mess);
		  #endif
	  }
  }
  
}

void modem_serial_process(void) {
	
	uint8_t  inChar = 0;
	uint8_t  pvChar = 0;
	uint16_t len = 0;
	uint8_t  is_ok = 0;
	
	
	//Пришли данные с Модема
	
	if (!fifo_is_empty(&modem_rx_fifo)) {
		while (!fifo_is_empty(&modem_rx_fifo)) {

			inChar = fifo_pull_uint8_nocheck(&modem_rx_fifo);
			
			modem_get_time = millis();
			
			if (!modem_serial_task) {
				uint8_t src = cmd_src_read();
				if (!src) {
					src = 2; //modem
					cmd_src_write(src);
					serial_data_clear(); //Очищаем буфер
				}
			
				if (src==2) {

					is_ok = 0;
					len = serial_data_len();
					if (len < SERIAL_DATA_LEN-2) {
						if (len > 0) {
							if (inChar == 0x0D || inChar == 0x0A) {
								pvChar = serial_data[len-1];
								if (pvChar != 0x0D && pvChar != 0x0A) {
									is_ok = 1;
								}
							} else {
								is_ok = 1;
							}
						} else {
							if (inChar != 0x0D && inChar != 0x0A) {
								is_ok = 1;
							}
						}
					}
					if (is_ok) {
						serial_data_add(inChar);
					}
				}
			} else {//внутренние команды модему
				
				is_ok = 0;
				len = strlen(modem_data);
				if (len < MODEM_DATA_LEN-2) {
					if (len>0) {
						if (inChar == 0x0D || inChar == 0x0A) {
							pvChar = modem_data[len-1];
							if (pvChar != 0x0D && pvChar != 0x0A) {
								is_ok = 1;
							}
						} else {
							is_ok = 1;
						}
					} else {
						if (inChar != 0x0D && inChar != 0x0A) {
							is_ok = 1;
						}
					}
				}
				if (is_ok) {
					addChar(modem_data, inChar);
				}
			}
		}
	}
}


uint8_t  modem_cmd_process(void) {
	
	if (modem_data_ready()) { //Данные готовы
		
		if (!serial_data_trim()) {
			cmd_src_write(0);
			return 0;
		}
		
		if (serial_is_command()) {
			
			#ifdef DEBUG
				serial_write("modem cmd:");
				serial_write(serial_data);
			#endif
			
			cmd_src_write(2); //modem_reply
			
			return 1;
			
		} else if (!strncmp(serial_data, "CLOSED", 6)) {
			
			cmd_src_write(0);
			serial_data_clear();
					
			//Соединение с сервером разорвано
			modem_disconnected();
			
		} else if (!strncmp(serial_data, "+CMT: ", 6)) {
			
			//Пришло SMS
			
			shiftString(serial_data, 6);
			
			if (strlen(serial_data)) {
				
				char info_str[64];
				//char str[32];
				lines_num(1, serial_data, info_str, sizeof(info_str));
				
				paramStringC(info_str, 2, '"', modem_phone);
				
				sprintf(mess, "New SMS: %s", modem_phone);
				serial_write_addr(mess);
				
				lines_num(2, serial_data, mess, sizeof(mess));
				
				//modem_flash(str, 57); //cmd|
				
				if ( !strncmp(mess, "cmd|", 4) ) {
					
					shiftString(mess, 4);
					if (strlen(mess)) {
						
						cmd_src_write(3); //sms_reply
						
						//Копируем данные
						serial_data_clear();
						strcat(serial_data, mess);
						clearString(mess);
						
						return 1;
					}
				} else {
					  //modem_flash(str, 56); //Data:
					  serial_write("Data: ");
					  serial_write(mess);
									
					  cmd_src_write(0);
					  serial_data_clear();
				}
			}
					
		} else {
			
			cmd_src_write(0);
					
			uint16_t len = serial_data_trim();
			if (len) {
					
				serial_write_start();
				serial_write_str("M: ");
				serial_write_str(serial_data);
				serial_write_char('\r');
				serial_write_end();
			}
			serial_data_clear();
		}
	}
	
	return 0;
}

void modem_blink(uint16_t ms, uint8_t sec) {
	uint16_t num = sec * 1000 / ms;
	for (uint16_t n = 0; n < num; n++) {
		
		HAL_GPIO_TogglePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN);
		delay_millis(ms);
	}
	
	HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
}

bool modem_buff_add(char* buff, char* string) {
	
	uint16_t len = strlen(buff);

	//добавляем во второй буфер
	if (len > 0) {
		if (len >= MODEM_SRV_BUF_LEN-1) {
			//нет места
			return false;
		}
		addChar(buff, ';');
		len++;
	}
	if (len + strlen(string) < MODEM_SRV_BUF_LEN-1) {
		strcat(buff, string);
		return true;
	} else {
		//нет места
		return false;
	}
}

bool modem_srv_write(char* string) {
	
	if (modem_is_sending) {//идет отправка
		
		return modem_buff_add(modem_srv_b2, string);
		
	} else {
		if (!modem_buff_add(modem_srv_b1, string)) {
			return modem_buff_add(modem_srv_b2, string);
		} else {
			return true;
		}
	}
}
void modem_write_b1(void) {
	modem_write(modem_srv_b1);
}
void modem_write_b2(void) {
	modem_write(modem_srv_b2);
}
void modem_buf_cpy(void) {
	clearString(modem_srv_b1);
	memcpy (modem_srv_b1, modem_srv_b2, MODEM_SRV_BUF_LEN);
	clearString(modem_srv_b2);
}
bool modem_srv_write_ready(void) {
	if (!modem_is_sending && strlen(modem_srv_b1)) {
		return true;
	} else {
		return false;
	}
}

void modem_dt_shift(int32_t dt) {
	if (modem_hello_time) {
		modem_hello_time = (uint32_t)(modem_hello_time + dt);
	}
}

uint32_t modem_serial_time(void) {
	return modem_serial_task;
}

void modem_param(char* input, uint8_t num) {
	switch (num) {
		case 1:
			strcpy(input, modem_imei);
		break;
		
		case 2:
			strcpy(input, modem_csq);
		break;
		
		case 3:
			strcpy(input, modem_lat);
		break;
		
		case 4:
			strcpy(input, modem_lon);
		break;
	}
}
//SIM-------------------------------------------------------------------------------------------------------------------------------------
bool modem_sim_detect(void) {
	//Контакт разомкнут, когда симка есть. Замкнут на землю, когда нет.
	//gp_get_state для pull-up выдает 1, когда 0 на входе
	
	#ifdef  SIM_DETECT_ONE
		GPIO_PinState state = HAL_GPIO_ReadPin(PIN_SIM_DETECT_PORT, PIN_SIM_DETECT_PIN);
		return state==GPIO_PIN_SET ? true : false;

	#endif
	
	#ifdef  SIM_DETECT_DUAl
		bool is_present = modem_sim_change();
	
		if (is_present) {
			return true;
		}
	
		//Change SIM
		is_present = modem_sim_change();
	
		if (is_present) {
			return true;
		} else {
			gp_set_state(GP_SIM1_ACT, 0);
			gp_set_state(GP_SIM2_ACT, 0);
			return false;
		}
	#endif
	
	return true;
}

uint8_t modem_sim_cur(void) {
	return modem_sim;
}

bool modem_sim_change(void) {
	
	#ifdef  SIM_DETECT_DUAl
	
	gp_set_state(GP_SIM1_ACT, 0);
	gp_set_state(GP_SIM2_ACT, 0);
	
	//Change SIM
	modem_sim = modem_sim ? 0 : 1;
	
	//0 - if sim not inserted
	
	gp_set_state( modem_sim ? GP_SIM2_ACT : GP_SIM1_ACT, 1 );
	delay_millis(5);
	
	//return ioport_get_pin_level(modem_sim ? PIN_SIM2_DETECT_PIN : PIN_SIM1_DETECT_PIN);
	if (modem_sim) {
		//return PORT_ReadInputDataBit(PIN_SIM2_DETECT_PORT, PIN_SIM2_DETECT_PIN);
		return HAL_GPIO_ReadPin(PIN_SIM2_DETECT_PORT, PIN_SIM2_DETECT_PIN);
	} else {
		//return PORT_ReadInputDataBit(PIN_SIM1_DETECT_PORT, PIN_SIM1_DETECT_PIN);
		return HAL_GPIO_ReadPin(PIN_SIM1_DETECT_PORT, PIN_SIM1_DETECT_PIN);
	}
	
	#endif
	
	#ifndef  SIM_DETECT_DUAl
		return true;
	#endif
}
