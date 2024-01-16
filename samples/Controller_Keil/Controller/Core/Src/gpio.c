#include "gpio.h"

tp_gpio_type PORTS[GP_MAX];
tp_gpio_def  PORT_DEFS[GP_MAX];
GPIO_InitTypeDef GPIOInitStruct[GP_MAX];

uint8_t ports_imp_delay[] = {1, 2, 5, 10, 25, 50, 100, 200};
uint8_t ports_call_delay[] = {5, 15, 30, 60};

uint16_t gp_regValue =  0;
uint16_t gp_log_cnt  = 0;
uint8_t gp_log_cnt_isSet = 0;

extern char mess[];
extern ADC_HandleTypeDef hadc1;
volatile bool adc_complete = false;

void gp_init(void) {
	
	//BLINK INIT
	//gpin_init(PIN_BLINK, PORT_OE_OUT);
	
	//REG Init
	/*
	ioport_set_pin_level( GP_REG_DS, false );
	ioport_set_pin_dir( GP_REG_DS, IOPORT_DIR_OUTPUT );
	
	ioport_set_pin_level( GP_REG_SH, false );
	ioport_set_pin_dir( GP_REG_SH, IOPORT_DIR_OUTPUT );
	
	ioport_set_pin_level( GP_REG_ST, false );
	ioport_set_pin_dir( GP_REG_ST, IOPORT_DIR_OUTPUT );
	*/
	
	//gp_reg_renew();
	
	#ifdef PIN_GP01
	PORTS [1-1].pin = PIN_GP01;
	
	PORT_DEFS[1-1] = (tp_gpio_def){
		.mod		= PIN_GP01_MODE,
		.config = PIN_GP01_CONF,
		.port		= PIN_GP01_PORT,
		.pin		= PIN_GP01_PIN
	};
	#endif
	
	#ifdef PIN_GP02
	PORTS[2-1].pin = PIN_GP02;
	
	PORT_DEFS[2-1] = (tp_gpio_def) {
		.mod 		= PIN_GP02_MODE,
		.config = PIN_GP02_CONF,
		.port		= PIN_GP02_PORT,
		.pin		= PIN_GP02_PIN
	};	
	#endif
	
	#ifdef PIN_GP03
	PORTS[3-1].pin = PIN_GP03;
	
	PORT_DEFS[3-1] = (tp_gpio_def) {
		.mod 		= PIN_GP03_MODE,
		.config = PIN_GP03_CONF,
		.port		= PIN_GP03_PORT,
		.pin		= PIN_GP03_PIN
	};	
	#endif
	
	#ifdef PIN_GP04
	PORTS[4-1].pin = PIN_GP04;
	
	PORT_DEFS[4-1] = (tp_gpio_def) {
		.mod 		= PIN_GP04_MODE,
		.config = PIN_GP04_CONF,
		.port		= PIN_GP04_PORT,
		.pin		= PIN_GP04_PIN
	};	
	#endif
	
	#ifdef PIN_GP05
	PORTS[5-1].pin = PIN_GP05;
	
	PORT_DEFS[5-1] = (tp_gpio_def) {
		.mod 		= PIN_GP05_MODE,
		.config = PIN_GP05_CONF,
		.port		= PIN_GP05_PORT,
		.pin		= PIN_GP05_PIN
	};	
	#endif
	
	#ifdef PIN_GP06
	PORTS[6-1].pin = PIN_GP06;
	
	PORT_DEFS[6-1] = (tp_gpio_def) {
		.mod 		= PIN_GP06_MODE,
		.config = PIN_GP06_CONF,
		.port		= PIN_GP06_PORT,
		.pin		= PIN_GP06_PIN
	};	
	#endif
	
	
	#ifdef PIN_GP07
	PORTS[7-1].pin = PIN_GP07;
	
	PORT_DEFS[7-1] = (tp_gpio_def) {
		.mod 		= PIN_GP07_MODE,
		.config = PIN_GP07_CONF,
		.port		= PIN_GP07_PORT,
		.pin		= PIN_GP07_PIN
	};	
	#endif
	
	#ifdef PIN_GP08
	PORTS[8-1].pin = PIN_GP08;
	
	PORT_DEFS[8-1] = (tp_gpio_def) {
		.mod 		= PIN_GP08_MODE,
		.config = PIN_GP08_CONF,
		.port		= PIN_GP08_PORT,
		.pin		= PIN_GP08_PIN
	};	
	#endif
	
	#ifdef PIN_GP09
	PORTS[9-1].pin = PIN_GP09;
	
	PORT_DEFS[9-1] = (tp_gpio_def) {
		.mod 		= PIN_GP09_MODE,
		.config = PIN_GP09_CONF,
		.port		= PIN_GP09_PORT,
		.pin		= PIN_GP09_PIN
	};	
	#endif
	
	#ifdef PIN_GP10
	PORTS[10-1].pin = PIN_GP10;
	
	PORT_DEFS[10-1] = (tp_gpio_def) {
		.mod 		= PIN_GP10_MODE,
		.config = PIN_GP10_CONF,
		.port		= PIN_GP10_PORT,
		.pin		= PIN_GP10_PIN
	};	
	#endif
	
	#ifdef PIN_GP11
	PORTS[11-1].pin = PIN_GP11;
	
	PORT_DEFS[11-1] = (tp_gpio_def) {
		.mod 		= PIN_GP11_MODE,
		.config = PIN_GP11_CONF,
		.port		= PIN_GP11_PORT,
		.pin		= PIN_GP11_PIN
	};	
	#endif
	
	#ifdef PIN_GP12
	PORTS[12-1].pin = PIN_GP12;
	
	PORT_DEFS[12-1] = (tp_gpio_def) {
		.mod 		= PIN_GP12_MODE,
		.config = PIN_GP12_CONF,
		.port		= PIN_GP12_PORT,
		.pin		= PIN_GP12_PIN
	};	
	#endif
	
	#ifdef PIN_GP13
	PORTS[13-1].pin = PIN_GP13;
	
	PORT_DEFS[13-1] = (tp_gpio_def) {
		.mod 		= PIN_GP13_MODE,
		.config = PIN_GP13_CONF,
		.port		= PIN_GP13_PORT,
		.pin		= PIN_GP13_PIN
	};	
	#endif
	
	#ifdef PIN_GP14
	PORTS[14-1].pin = PIN_GP14;
	
	PORT_DEFS[14-1] = (tp_gpio_def) {
		.mod 		= PIN_GP14_MODE,
		.config = PIN_GP14_CONF,
		.port		= PIN_GP14_PORT,
		.pin		= PIN_GP14_PIN
	};	
	#endif
	
	#ifdef PIN_GP15
	PORTS[15-1].pin = PIN_GP15;
	
	PORT_DEFS[15-1] = (tp_gpio_def) {
		.mod 		= PIN_GP15_MODE,
		.config = PIN_GP15_CONF,
		.port		= PIN_GP15_PORT,
		.pin		= PIN_GP15_PIN
	};	
	#endif
	
	#ifdef PIN_GP16
	PORTS[16-1].pin = PIN_GP16;
	
	PORT_DEFS[16-1] = (tp_gpio_def) {
		.mod 		= PIN_GP16_MODE,
		.config = PIN_GP16_CONF,
		.port		= PIN_GP16_PORT,
		.pin		= PIN_GP16_PIN
	};	
	#endif
	
	#ifdef PIN_GP17
	PORTS[17-1].pin = PIN_GP17;
	
	PORT_DEFS[17-1] = (tp_gpio_def) {
		.mod 		= PIN_GP17_MODE,
		.config = PIN_GP17_CONF,
		.port		= PIN_GP17_PORT,
		.pin		= PIN_GP17_PIN
	};	
	#endif
	
	#ifdef PIN_GP18
	PORTS[18-1].pin = PIN_GP18;
	
	PORT_DEFS [18-1] = (tp_gpio_def) {
		.mod 		= PIN_GP18_MODE,
		.config = PIN_GP18_CONF,
		.port		= PIN_GP18_PORT,
		.pin		= PIN_GP18_PIN
	};	
	#endif
	
	#ifdef PIN_GP19
	PORTS[19-1].pin = PIN_GP19;
	
	PORT_DEFS [19-1] = (tp_gpio_def) {
		.mod 		= PIN_GP19_MODE,
		.config = PIN_GP19_CONF,
		.port		= PIN_GP19_PORT,
		.pin		= PIN_GP19_PIN
	};	
	#endif
	
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		gpin_init(num);
	}
	
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		gp_conf_init(num);
	}
	
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		gp_bind_renew(num);
	}
}

void gp_reg_bit(uint8_t bit, uint8_t val) {
	if (val) {
		gp_regValue |= 1 << bit;
	} else {
		gp_regValue &= ~(1 << bit);
	}
}

void gp_reg_renew(void) {
	
	uint8_t max = GP_REG_BITS;
	
	/*
	//Move data
	for (uint8_t n = 0; n < max; n++) {
		
		//??????? ????? ??????
		ioport_set_pin_level( GP_REG_DS, (gp_regValue >> (max-1-n)) & 1 );
		
		//????? ?? 1
		ioport_set_pin_level( GP_REG_SH, true );
		delay_micros(3);
		ioport_set_pin_level( GP_REG_SH, false );
	}
	
	//??????
	ioport_set_pin_level( GP_REG_ST, true );
	delay_micros(6);
	ioport_set_pin_level( GP_REG_ST, false );
	*/
}

void gp_dtCall_set(uint8_t num, uint16_t sec) {
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->call_delay = sec;
}

void gp_call_retries_set(uint8_t num, uint8_t n) {
	tp_gpio_type   *Port  = gp_port(num);
	Port->call_retries = n;
}

void gp_call_reset(uint8_t num) {
		
	gp_dtCall_set(num, 0);
	gp_call_retries_set(num, 0 );
}

void gp_conf_init(uint8_t num) {
	
	tp_gpio_type *Port = gp_port(num);
	uint8_t state = 0;
	
	Port->mode = gp_mode_read(num);
	Port->config = gp_conf_read(num);
	
	uint8_t mode = Port->mode;
	
	//Phone
	gp_call_reset(num);
	
	//ADC
	if (mode==GP_MODE_ADC) {
		
		Port->val = 0;
		
		Port->zvalue = gp_zvalue_read(num);
		
		Port->kt     = gp_kt_read(num);
		
		uint32_t min = gp_min_read(num);

		if (min) {
			Port->counter = min;
			Port->val |= 1 << 0;
		} else {
			Port->val &= ~(1 << 0);
		}
		
		uint32_t max = gp_max_read(num);
		
		if (max) {
			Port->max = max;
			Port->val |= 1 << 1;
		} else {
			Port->val &= ~(1 << 1);
		}
		
		Port->val &= ~(1 << 2);
		Port->val &= ~(1 << 3);
	}
	
	//INU
	else if (mode==GP_MODE_INU) {
		ioport_set_pin_mode( gp_port(num)->pin, GPIO_MODE_INPUT, GPIO_PULLUP);
		
		Port->val = gp_get(num) ? 0:1;
		
	}
	//IND
	else if (mode==GP_MODE_IND) {
		ioport_set_pin_mode( gp_port(num)->pin, GPIO_MODE_INPUT, GPIO_PULLDOWN);
		
		Port->val = gp_get(num) ? 1:0;
	}
	//IN
	else if (mode==GP_MODE_IN) {
		ioport_set_pin_mode( gp_port(num)->pin, GPIO_MODE_INPUT, GPIO_NOPULL);
		
		Port->val = gp_get(num) ? 1:0;
	}
	//PWM
	else if (mode==GP_MODE_PWM) {
		gp_set(num, state);
		ioport_set_pin_mode(gp_port(num)->pin, GPIO_MODE_OUTPUT_PP, 0);
	}
	//REG
	#ifdef GP_USE_REG
	else if (mode==GP_MODE_REG) {
		if (gp_conf_bit(Port->config, GP_CONFBIT_SAVE)) {
			//??????????? ?????????
			//>state = eeprom_read_byte(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_STATE);
			
			memcpy(&state, &buffer[0] + GP_STORE_STATE, sizeof(state));
		
		} else if (gp_conf_bit(Port->config, GP_CONFBIT_INIT_TRUE)) {
			
			state = 1;
		}
		
		gp_set(num, state);
	}
	#endif
	
	//OUT
	else {
		if (gp_conf_bit(Port->config, GP_CONFBIT_SAVE)) {
			state = eeprom_read_byte(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_STATE);
		
		} else if (gp_conf_bit(Port->config, GP_CONFBIT_INIT_TRUE)) {
			
			state = 1;
		}
		
		ioport_set_pin_mode(gp_port(num)->pin, GPIO_MODE_OUTPUT_PP, 0);
		
		gp_set(num, state);
	}
	
	//All In
	if (mode>=2 && mode<=4) {
		
		Port->delay = 0;
		
		if (gp_conf_bit(num, GP_CONFBIT_COUNTER)) {
			
			uint32_t counter = eeprom_read_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_CNT);
			
			Port->max     = gp_max_read(num);
		}
	}
}

uint32_t gp_max_read(uint8_t num) {
	
	uint32_t value = eeprom_read_uint32(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_MAX);
	
	if (value == zval_32) {
		value = 0;
	}

	return value;
}

void gp_set_state(uint8_t num, uint8_t state) {
	
	if (num <= 0) {num = 1;}
	if (num > GP_MAX) {num = GP_MAX;}
		
	uint8_t cur_state = gp_get_state(num);
	
	if (cur_state != state) {
		
		gp_set(num, state);
		
		//Binded ports
		gp_bind_value(num, state);
		
		uint16_t conf = gp_conf(num);
		
		if (gp_conf_bit(conf, GP_CONFBIT_SAVE)) { //save state
			uint8_t save = state ? 1 : 0;
			eeprom_write_byte(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_STATE, save);
		}
		
		if (gp_conf_bit(conf, GP_CONFBIT_JRN)) { //journal
			gp_log_add(state ? ELOG_GP_HIGH : ELOG_GP_LOW, num, "");
		}
		
		if (gp_conf_bit(conf, GP_CONFBIT_LOG)) { //log state
			sprintf(mess, "%u", state);
			gp_log_state(num, mess);
		}
	}
}

uint8_t gp_mode_read(uint8_t num) {

	uint8_t mod = eeprom_read_byte(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_MODE);

	if (mod==zval_8) {
		mod = PORT_DEFS[num-1].mod;
	}

	return mod;
}

void gp_bind_value(uint8_t p, uint8_t state) {
	
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		
		if (gp_bind(num)==p && (gp_mode(num)==GP_MODE_OUT || gp_mode(num)==GP_MODE_REG)) {
			
			//Set state
			if (gp_bind_inverse(num)) {
				gp_set(num, !state);
			} else {
				gp_set(num, state);
			}
		}
	}
}

uint16_t gp_conf_read(uint8_t num) {
	
	uint16_t  conf = eeprom_read_uint16(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_CONF);
	
	if (conf==zval_16) {
		conf = PORT_DEFS[num-1].config;
	}
	
	return conf;
}

void ioport_set_pin_mode(uint8_t pin, uint32_t mode, uint8_t pull) {
	
	if (mode == GPIO_MODE_OUTPUT_PP) {
		
		GPIOInitStruct[pin-1].Speed = GPIO_SPEED_LOW;
	
	} else if (mode == GPIO_MODE_INPUT) {
		
		GPIOInitStruct[pin-1].Pull = pull;
	}
	
	GPIOInitStruct[pin-1].Mode = mode;
	GPIOInitStruct[pin-1].Pin = PORT_DEFS[pin-1].pin;
	
	GPIO_TypeDef* GPIOx = PORT_DEFS[pin-1].port;
   
  //PORT_Init(GPIOx, &GPIOInitStruct[pin-1]);
	HAL_GPIO_Init(GPIOx, &GPIOInitStruct[pin-1]);
}

tp_gpio_type* gp_port(uint8_t num) {
	
	if (num <= 0) {num = 1;}
	if (num > GP_MAX) {num = GP_MAX;}
		
	return &PORTS [num-1];
}

uint8_t gp_mode(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->mode & 0x07;
}

bool gp_conf_bit(uint16_t conf, uint8_t x) {
	
	return (conf >> x) & 1;
}

uint16_t gp_conf(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->config;
}

uint8_t gp_bind(uint8_t num) {
	
	return gp_conf(num) & 0x0F; //4 lsb
}

void gp_bind_renew(uint8_t num) {
	
	uint8_t bind = gp_bind(num);
	uint8_t mode = gp_mode(num);
	
	if ((mode==GP_MODE_OUT || mode==GP_MODE_REG) && bind) {
		
		uint8_t state = gp_get_state(bind);
		
		if (gp_bind_inverse(num)) {
			gp_set(num, !state);
		} else {
			gp_set(num, state);
		}
	}
	
	//ADC
	if (mode==GP_MODE_ADC) {
		tp_gpio_type   *Port  = gp_port(num);
	
		bool set_min = (Port->val >> 0) & 1;
		bool set_max = (Port->val >> 1) & 1;
	
		if (set_min || set_max) {
			float fval = gp_adc_val(num);
			float min = gp_min_getF(num);
			float max = gp_max_getF(num);
		
			//Check min
			if (fval < min && set_min) {
			
				//flag_min set
				//Port->val |= 1 << 2;
				gp_bind_renew_adc(num, 1, 1, 0);
				
				//flag_max clear
				//Port->val &= ~(1 << 3);
				gp_bind_renew_adc(num, 0, 0, 0);
			
			}
			if (fval > max && set_max) {
			
				//flag_max set
				//Port->val |= 1 << 3;
				gp_bind_renew_adc(num, 0, 1, 0);
				
				//flag_min clear
				//Port->val &= ~(1 << 2);
				gp_bind_renew_adc(num, 1, 0, 0);
			
			}
			if (fval >= min && set_min) {
				//Normal
				//flag_min clear
				//Port->val &= ~(1 << 2);
				gp_bind_renew_adc(num, 1, 0, 0);
			}
			if (fval <= max && set_max) {
				//Normal
				//flag_max clear
				//Port->val &= ~(1 << 3);
				gp_bind_renew_adc(num, 0, 0, 0);
			}
		}
	}
	
}

void gp_bind_renew_adc(uint8_t num, bool is_min, bool state, bool is_log) {
	
	uint8_t num_bind;
	
	tp_gpio_type   *Port  = gp_port(num);
	
	if (is_log) {
		gp_dt_save(num);
	
		if (gp_conf_bit(Port->config, GP_CONFBIT_LOG)) {
			//Отправляем
		
			char str[16];
		
			float fval = gp_adc_val(num);
			//dtostrf(fval, 1, 3, str);
			sprintf(str, "%0.3f", (double)fval);
		
			gp_log_state(num, str);
		}
	}
	
	if (is_min) {
		num_bind = gp_bind_min(num);
		if (num_bind) {
			//>sprintf(str, "Min %u bind %u, state = %u", num, num_bind, state);
			//>serial_write(str);|
			if (state) {
				gp_set_state(num_bind, !gp_bind_inverse_min(num));
			} else {
				gp_set_state(num_bind, gp_bind_inverse_min(num));
			}
		}
	} else {
		num_bind = gp_bind(num);
		if (num_bind) {
			//>sprintf(str, "Max %u bind %u, state = %u", num, num_bind, state);
			//>serial_write(str);
			
			if (state) {
				gp_set_state(num_bind, !gp_bind_inverse(num));
			} else {
				gp_set_state(num_bind, gp_bind_inverse(num));
			}
		}
	}
}

void gp_log_state(uint8_t num, char* strval) {
	
	char dt[32];
	char out[64];
	
	dateFormatS(gp_get_upd(num), dt);
	
	sprintf(out, "gp-%u-%s=%s", num, dt, strval);
	
	//modem_srv_write(out);
}

void gp_input_process_ms(void) {
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		uint8_t  mod   = gp_mode(num);
		uint16_t conf  = gp_conf(num);
		
		//частота сканирования 0 - 1 ms
		if (mod>=2 && mod<=4 && !gp_conf_bit(conf, GP_CONFBIT_TPSCAN)) {
			gp_input_check(num);
		}
	}
}

void gp_input_check(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	uint8_t  mod   = gp_mode(num);
	
	uint8_t new_value = 0;
	
	if (mod==GP_MODE_INU) {//pull up, active 0
		
		new_value = gp_get(num) ? 0:1;
	
	} else if (mod==GP_MODE_IND) {//pull down, active 1
		
		new_value = gp_get(num) ? 1:0;
	}
	
	if (new_value != Port->val) {
		
		Port->delay++;
		
		if (Port->delay >= gp_imp_delay(num)) {
			
			//Фиксируем изменение порта
			Port->val = new_value;
			Port->delay = 0;
			
			//Если счетчик
			if (gp_conf_bit(Port->config, GP_CONFBIT_COUNTER )) {
				
				if (new_value) {//счет ведем при изменении на активный уровень
					
					Port->counter++;
					
					//Вызываем callback
					Port->onCallBack(num, new_value);
					
					if (Port->max) {

						if (Port->counter == Port->max) {
							
							//Окончание счета
							gp_dt2_save(num);
							
							if (gp_conf_bit(Port->config, GP_CONFBIT_LOG)) {
								//Отправляем
								sprintf(mess, "%u", Port->counter);
								gp_log_state(num, mess);
							}
							//Новый счет
							gp_cnt_set(num, 0);
						}
					}
				}
			} else {
				
				//Не счетчик
				
				//Связанные порты
				gp_bind_value(num, Port->val);
				
				gp_dt_save(num);
				
				if (gp_conf_bit(Port->config, GP_CONFBIT_JRN)) { //journal
					gp_log_add(Port->val ? ELOG_GP_HIGH : ELOG_GP_LOW, num, "");
				}
				
				//Изменение состояния
				if (gp_conf_bit(Port->config, GP_CONFBIT_LOG)) { //log state
					sprintf(mess, "%u", Port->val);
					gp_log_state(num, mess);
				}
				
				//Звонок
				gp_call_start(num, Port->val);
			}
		}
	} else {
		Port->delay = 0;
	}
}

void gp_log_fill(void) {
	
	tp_gp_log_type log_rec;
	
	log_rec.dt  = 1505041000;
	log_rec.tp  = 0;
	log_rec.num = 1;
	strcpy(log_rec.info, "test");
	
	uint32_t wdt = millis();
	uint32_t m = 0;
	
	for (uint16_t rec = 0; rec < MEM_LOG_NUM; rec++) {
	
		M24M01_write (MEM_LOG_BANK, MEM_LOG_ADDR + (uint32_t)rec * MEM_LOG_SIZE, &log_rec, MEM_LOG_SIZE);
		
		if (rec < 5000) {
			log_rec.dt += 15;
		} else {
			log_rec.dt -= 12;
		}
		
		m = millis();
		
		if (m - wdt > 100)  {
			sys_wdt_reset();
			wdt = m;
		}
	}
	
	gp_log_reidx();
}

uint16_t gp_log_reidx(void) {

	gp_log_cnt = gp_log_last();

	return gp_log_cnt;
}

uint16_t gp_log_last(void)
{
	uint32_t addr = MEM_LOG_ADDR;
	uint32_t dt;
	
	uint32_t wdt = millis();
	uint32_t m;
	uint16_t rec;
	
	//Читаем последний сохраненный индекс
	//Указывает на заглушку
	uint16_t idx = gp_log_idx_read();
	
	//sprintf(mess, "Read idx=%u", idx);
	//serial_write(mess);
	
	bool is_found = false;
	
	for (rec = 0; rec < MEM_LOG_NUM; rec++) {
		if (rec >= idx) {
			if ( M24M01_read (MEM_LOG_BANK, addr, &dt, sizeof(dt)) ) {
			
				//sprintf(mess, "rec=%u; addr=%06lX; dt=%08lX", rec, addr, dt);
				//serial_write(mess);
			
				if (dt==zval_32) {
					//Конец записей
					is_found = true;
					break;
				}
			
				m = millis();

				if (m - wdt > 100)  {
					sys_wdt_reset();
					wdt = m;
				}
			} else {
				break;
			}
		}
		addr += MEM_LOG_SIZE;
	}
	
	if (!is_found) {
		//Не нашли маркер конца записей
		//Ищем раздел по дате
		
		uint16_t max_rec = 0;
		uint32_t max_dt = 0;
		
		for (rec = 0; rec < MEM_LOG_NUM; rec++) {
			if ( M24M01_read (MEM_LOG_BANK, addr, &dt, sizeof(dt)) ) {
	
				if (dt > max_dt) {
					max_dt = dt;
					max_rec = rec;
				}
	
				m = millis();

				if (m - wdt > 100)  {
					sys_wdt_reset();
					wdt = m;
				}
			} else {
				break;
			}
		}
		
		rec = max_rec + 1;
		if (rec == MEM_LOG_NUM) {
			rec = 0;
		}
	}
	
	//sprintf(mess, "New idx=%u", rec);
	//serial_write(mess);
	
	//Сохраняем
	gp_log_idx_write(rec);
	
	return rec;
}

bool gp_log_add(uint8_t tp, uint8_t num, char* info) {
	
	tp_gp_log_type log_rec;
	
	log_rec.dt  = now();
	log_rec.tp  = tp;
	log_rec.num = num;
	strncpy(log_rec.info, info, sizeof(log_rec.info) ); //-1
	//>log_rec.info[ sizeof(log_rec.info)-1 ] = 0x00;
	
	uint8_t     lst[MEM_LOG_SIZE];
	for (uint8_t n = 0; n < MEM_LOG_SIZE; n++) {
		lst[n] = zval_8;
	}
	
	uint16_t rec = gp_log_idx();
	
	if ( M24M01_write (MEM_LOG_BANK, MEM_LOG_ADDR + (uint32_t)rec * MEM_LOG_SIZE, &log_rec, MEM_LOG_SIZE) ) {
		
		gp_log_cnt++;
		if (gp_log_cnt==MEM_LOG_NUM) {
			//Пишем сначала
			gp_log_cnt = 0;
			
			uint16_t cnt = gp_log_cnt_get();
			cnt++;
			gp_log_cnt_set(cnt);
		}
		
		//Пишем в следующую запись маркер конца
		M24M01_write (MEM_LOG_BANK, MEM_LOG_ADDR + (uint32_t)gp_log_cnt * MEM_LOG_SIZE, &lst, MEM_LOG_SIZE);
		
		return true;
	} else {
		return false;
	}
}

void gp_dt2_save(uint8_t num) {
	#ifdef GP_DT_SAVE
		eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_DTPRV, now());
	#endif
}

void gp_dt_save(uint8_t num) {
	#ifdef GP_DT_SAVE
		tp_gpio_type   *Port  = gp_port(num);

		uint32_t        date;
	
		//Сохраняем предыдущую дату
		date = Port->dtupd;
		if (date) {
			eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_DTPRV, date);
		}
	
		//Сохраняем текущую дату
		date = now();
		if (date) {
			Port->dtupd = date;
			eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_DTUPD, date);
		}
	#endif
}

float    gp_adc_val(uint8_t num) {
	
	int16_t rvalue = gp_adc_rvalue(num);
	
	tp_gpio_type   *Port  = gp_port(num);
	
	//Вычисляем значение
	rvalue -= Port->zvalue; //вычитаем нулевое значение
	if (rvalue < 0) {
		rvalue = 0;
	}
	
	return (float)rvalue / Port->kt;
}

bool gp_get(uint8_t num) {
	return ioport_get_pin_level( gp_port(num)->pin );
}

void ioport_set_pin_level(uint8_t num, bool level) {
	
	GPIO_TypeDef* GPIOx = PORT_DEFS[num-1].port;
	uint16_t PORT_Pin = PORT_DEFS[num-1].pin;
	
	//sprintf(mess, "gpio pin=%u lvl=%u", num, level);
	//serial_write_addr(mess);
	
	if (!level) {
		//PORT_ResetBits(GPIOx, PORT_Pin);
		HAL_GPIO_WritePin(GPIOx, PORT_Pin, GPIO_PIN_RESET);
	} else {
		//PORT_SetBits(GPIOx, PORT_Pin);
		HAL_GPIO_WritePin(GPIOx, PORT_Pin, GPIO_PIN_SET);
	}
}

uint8_t ioport_get_pin_level(uint8_t num) {
	
	GPIO_TypeDef* GPIOx = PORT_DEFS[num-1].port;
	uint16_t PORT_Pin = PORT_DEFS[num-1].pin;
	
	//return PORT_ReadInputDataBit(GPIOx, PORT_Pin);
	return HAL_GPIO_ReadPin(GPIOx, PORT_Pin);
}

uint16_t gp_log_idx(void) {
	if (!gp_log_cnt_isSet) {
		gp_log_cnt = gp_log_last();
		gp_log_cnt_isSet = 1;
	}
	return gp_log_cnt;
}

uint16_t gp_log_cnt_get(void) {
	
	uint16_t  tmp = eeprom_read_uint16(EE_LOG_CNT);
	if (tmp==zval_16) {
		tmp = 0;
	}
	return tmp;
}

uint32_t gp_min_read(uint8_t num) {
	
	uint32_t value = eeprom_read_uint32(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_MIN);
	
	if (value==zval_32) {
		value = 0;
	}

	return value;
}

void gp_dtCall_reset(uint8_t num) {
	
	uint8_t byte = gp_call_read(num);
	gp_dtCall_set(num, gp_call_delay(byte) * 60);
}

void gp_input_process_sec(void) {
	
	for (uint8_t num = 1; num <= GP_MAX; num++) {
		
		uint8_t  mod   = gp_mode(num);
		uint16_t conf  = gp_conf(num);
		
		//частота сканирования 1 - 1 sec
		if (mod>=2 && mod<=4) {
			if (gp_conf_bit(conf, GP_CONFBIT_TPSCAN)) {
				gp_input_check(num);
			}
		} else if (mod==GP_MODE_ADC) {
		
			//Всегда 1 сек
			gp_input_adc(num);
		}
		
		//Обработка звонка
		/*
		if (MODEM_NET_REG && gp_call_mode_get(num) ) {
			
			tp_task_type *task_call = Task_by_name("MCALL");
			if (! Task_is_running(task_call) ) {
					
				uint8_t byte = gp_call_read(num); //Параметры звонка на телефон
				uint8_t pos = gp_call_rec(byte);  //Номер записи звонка
				if (pos) {
					char phone[16] = "";

					phone_num(pos, phone);
					if (strlen(phone)) {
				
						uint16_t retries = gp_call_retries_get(num);
							
						if (retries) {
								
							uint16_t sec = gp_dtCall_get(num);
							if (sec) {
								//char str[64] = "";
								//sprintf(str, "retries call: num: %u; retries: %u; sec: %u", num, retries, sec);
								//serial_write(str);
							
								sec--;
								gp_dtCall_set(num, sec);
								if (!sec) {
									retries--;
									gp_call_retries_set(num, (uint8_t)retries);
										
									if (retries) {
										gp_dtCall_reset(num);
									}
										
									task_call->param[0] = pos;
									task_call->param[1] = num;
									Task_start_tp(task_call);
								}
							}
						}
					}
				}
			}
		}
		*/
	}
}

void gp_input_adc(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	bool set_min = (Port->val >> 0) & 1;
	bool set_max = (Port->val >> 1) & 1;
	bool flag_min = (Port->val >> 2) & 1;
	bool flag_max = (Port->val >> 3) & 1;
	char  str[10];
	
	if (set_min || set_max) {
		
		float fval = gp_adc_val(num);
		float min = gp_min_getF(num);
		float max = gp_max_getF(num);
		
		//Check min
		if (fval < min && set_min && !flag_min) {
			
			//flag_min set
			Port->val |= 1 << 2;
				
			if (gp_conf_bit(Port->config, GP_CONFBIT_JRN)) { //journal
				//dtostrf(fval, 1, 2, str);
				sprintf(str, "%0.2f", (double)fval);
				gp_log_add(ELOG_GP_ADC_LOW, num, str);
			}
				
			gp_bind_renew_adc(num, 1, 1, 1);
				
			gp_call_start(num, 0);
		}
		if (fval > max && set_max && !flag_max) {
			
			//flag_max set
			Port->val |= 1 << 3;
				
			if (gp_conf_bit(Port->config, GP_CONFBIT_JRN)) { //journal
				//dtostrf(fval, 1, 2, str);
				sprintf(str, "%0.2f", (double)fval);
				gp_log_add(ELOG_GP_ADC_HIGH, num, str);
			}
				
			gp_bind_renew_adc(num, 0, 1, 1);
				
			gp_call_start(num, 1);
		}
		
		//Normal
		bool is_normal = false;
		
		//Наверное возвращение в норму - не нужно звонить
		if (flag_min && fval >= min) {
			//flag_min clear
			Port->val &= ~(1 << 2);
				
			gp_bind_renew_adc(num, 1, 0, 1);
				
			gp_call_reset(num);
			is_normal = true;
		}
		if (flag_max && fval <= max) {
			//flag_max clear
			Port->val &= ~(1 << 3);
				
			gp_bind_renew_adc(num, 0, 0, 1);
				
			gp_call_reset(num);
			is_normal = true;
		}
		
		if (is_normal && gp_conf_bit(Port->config, GP_CONFBIT_JRN)) { //journal
			//dtostrf(fval, 1, 2, str);
			sprintf(str, "%0.2f", (double)fval);
			gp_log_add(ELOG_GP_ADC_NORM, num, str);
		}
	}
}

void gp_log_clear(void) {
	//Обнуление счетчиков
	gp_log_cnt_set(0);
	gp_log_cnt = 0;
	gp_log_cnt_isSet = 0;
	
	gp_log_idx_write(0);
	
	//Очистка памяти	
	uint32_t addr = 0;
	uint8_t len = 128;
	uint8_t data_send[len];

	for (uint8_t n=0; n < len; n++) {
		data_send[n] = zval_8;
	}
	uint32_t wdt = millis();
	uint32_t m = 0;
	
	//128кб
	for (uint16_t p=0; p < 8*128; p++) {
	
		M24M01_write(MEM_LOG_BANK, addr, &data_send, len);
	
		addr += len;
	
		m = millis();
	
		if (m - wdt > 100)  {
			sys_wdt_reset();
			wdt = m;
		}
	}
}


void gp_log_cnt_set(uint16_t cnt) {
	eeprom_write_uint16(EE_LOG_CNT, cnt);
}

bool  gp_log_rec(uint16_t rec, tp_gp_log_type *log_rec) {
	
	if (rec > MEM_LOG_NUM || !rec) {
		return false;
	}
	
	uint32_t addr = MEM_LOG_ADDR + (uint32_t)(rec-1)*MEM_LOG_SIZE;
	
	if ( M24M01_read (MEM_LOG_BANK, addr, log_rec, MEM_LOG_SIZE) ) {
		return log_rec->dt == zval_32 ? false : true;
	} else {
		return false;
	}
	
}


void gp_set(uint8_t num, uint8_t state) {
	tp_gpio_type   *Port  = gp_port(num);

	uint8_t mode = gp_mode(num);
	
	bool inv = gp_conf_bit(Port->config, GP_CONFBIT_INV_OUT);
	
	#ifdef GP_USE_REG
	if (mode==GP_MODE_REG) {//REG
		gp_reg_bit( gp_reg_val(num),  inv ? !state : state );
		gp_reg_renew();
	}
	#endif
	
	if (mode==GP_MODE_OUT) {//OUT
		
		//sprintf(mess, "num:%u, state:%u, inv:%u", num, state, inv);
		//serial_write(mess);
		ioport_set_pin_level( Port->pin, inv ? !state : state );
	}
	
	if (state != Port->val) {
		gp_dt_save(num);
	
		//Calling
		gp_call_start(num, state);
	
		Port->val = state;
	}
}

uint8_t gp_reg_val(uint8_t num) {
	
	return (gp_conf(num) >> GP_CONFBIT_REG) & 0x0F; //4 ??????? ???
}

void gp_call_start(uint8_t num, uint8_t state) {
	
	uint8_t call_mode = gp_call_mode_get(num);
	
	/*>
	char str[64] = "";
	sprintf(str, "call_start=%u; state=%u; mode=%u", num, state, mode);
	serial_write(str);
	<*/
	
	if (call_mode) {
		if ( (call_mode==1 && state) || (call_mode==2 && !state) || call_mode>=3) {
			//>serial_write("Init");
			gp_call_init(num);
		} else {
			//>serial_write("Reset");
			gp_call_reset(num);
		}
	} else {
		gp_call_reset(num);
	}
}

void gp_call_init(uint8_t num) {
	
	if (gp_call_mode_get(num)) {
		
		/*>
		char str[64] = "";
		sprintf(str, "call_init: %u", num);
		serial_write(str);
		<*/
		
		uint8_t byte = gp_call_read(num);
		gp_dtCall_set(num, 5); //звонок без задержки
		gp_call_retries_set(num, gp_call_number(byte) );
	}
}

uint8_t gp_bind_inverse(uint8_t num) {
	
	uint16_t conf = gp_conf(num);
	
	return gp_conf_bit(conf, GP_CONFBIT_BIND_INV);
}

uint8_t gp_get_state(uint8_t num) {
	
	if (num <= 0) {num = 1;}
	if (num > GP_MAX) {num = GP_MAX;}
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->val;
}

void gp_bind_set(uint8_t num, uint8_t bind, uint8_t inverse) {
	
	//>tp_gpio_type   *Port  = gp_port(num);
	
	bind &= 0x0F;
	
	uint16_t conf = gp_conf(num);
	
	conf &= 0xFFE0; //?1110 0000?
	conf |= bind;
	
	//Инверсия
	if (inverse) {
		conf |= 1 << GP_CONFBIT_BIND_INV;
	}
	
	//Сохраняем
	gp_conf_write( num, conf);
	
	//Обновляем состояние
	gp_bind_renew(num);
}

void gp_conf_write(uint8_t num, uint16_t conf) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	//Конфигурация
	if (conf != gp_conf(num)) {
		Port->config = conf;
		
		eeprom_write_uint16(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_CONF, conf);
		
		//Сохраняем состояние выходного порта
		if (gp_conf_bit(conf, GP_CONFBIT_SAVE) && !gp_mode(num)) {
			uint8_t save = gp_get_state(num) ? 1 : 0;
			eeprom_write_byte(EE_ADDR_PORTS+(num-1)*GP_STORE + GP_STORE_STATE, save);
		}
		//>gp_conf_init(num);
		
	}
}

uint32_t gp_get_upd(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->dtupd;
}

uint32_t gp_get_prv(uint8_t num) {
	
	uint32_t date = eeprom_read_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_DTPRV);
	if (date == zval_32) {
		date = 0;
	}
	return date;
}

uint16_t gp_dtCall_get(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->call_delay;
}

uint16_t gp_call_retries_get(uint8_t num) {
	tp_gpio_type   *Port  = gp_port(num);
	return Port->call_retries;
}

void gp_kt_save(uint8_t num, float fvalue) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	//Конфигурация
	if (fvalue != Port->kt) {
		Port->kt = fvalue;
		eeprom_write_float(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_KT, fvalue);
	}
}

float gp_kt_read(uint8_t num) {
	
	uint32_t ivalue = eeprom_read_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_KT);
	
	if (ivalue==zval_32) {
		return 1;
	}
	
	float fvalue = eeprom_read_float(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_KT);

	return fvalue;
}

int16_t gp_zvalue_read(uint8_t num) {
	
	int16_t zvalue;
	
	eeprom_read_buffer(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_ZVAL, (void *)&zvalue, sizeof(zvalue));
	
	if (zvalue==-1) {
		zvalue = 0;
	}
	
	return zvalue;
}

void gp_zvalue_write(uint8_t num, int16_t zvalue) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	//Конфигурация
	if (zvalue != Port->zvalue) {
		Port->zvalue = zvalue;
		eeprom_write_buffer(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_ZVAL, (void *)&zvalue, sizeof(zvalue));
	}
}

void gp_min_saveF(uint8_t num, float value) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->counter = *(uint32_t *)&value;
	if (Port->counter) {
		//Значение установлено
		Port->val |= 1 << 0;
	} else {
		Port->val &= ~(1 << 0);
	}
  //Сброс флага срабатывания
	Port->val &= ~(1 << 2);
	
	eeprom_write_float(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_MIN, value);
}

void gp_bind_min_set(uint8_t num, uint8_t bind, uint8_t inverse) {
	
	//>tp_gpio_type   *Port  = gp_port(num);
	
	bind &= 0x0F;
	
	uint16_t conf = gp_conf(num);
	
	conf &= 0x83FF; //10000011
	conf |= bind << GP_CONFBIT_MINBIND;
	
	//Инверсия
	if (inverse) {
		conf |= 1 << GP_CONFBIT_MININV;
	}
	
	//Сохраняем
	gp_conf_write( num, conf);
	
	//Обновляем состояние
	gp_bind_renew(num);
}

float gp_min_getF(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return *(float *)&Port->counter;
}

uint8_t gp_bind_min(uint8_t num) {
	
	return (gp_conf(num) >> GP_CONFBIT_MINBIND) & 0x0F; //4 ??????? ???
}

uint8_t gp_bind_inverse_min(uint8_t num) {
	
	uint16_t conf = gp_conf(num);
	
	return gp_conf_bit(conf, GP_CONFBIT_MININV);
}

void gp_max_saveF(uint8_t num, float value) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->max = *(uint32_t *)&value;
	
	if (Port->max) {
		//Значение установлено
		Port->val |= 1 << 1;
	} else {
		Port->val &= ~(1 << 1);
	}
	//Сброс флага срабатывания
	Port->val &= ~(1 << 3);
	
	eeprom_write_float(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_MAX, value);
}

float gp_max_getF(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return *(float *)&Port->max;
}

void gp_conf_write_bit(uint8_t num, uint8_t bit, uint16_t val) {
	
	uint16_t conf = gp_conf(num);
	
	if (val) {
		conf |= 1 << bit;
	} else {
		conf &= ~(1 << bit);
	}
	
	gp_conf_write(num, conf);
}

void gp_conf_write_delay(uint8_t num, uint16_t val) {
	
	uint16_t conf = gp_conf(num);
	uint16_t delay = 0;
	uint16_t dconf = 0;
	
	//Сброс
	conf &= 0xF8FF;
	//>conf &= ~(1 << 8);
	//>conf &= ~(1 << 9);
	//>conf &= ~(1 << 10);
	
	//>sprintf(mess, "conf before=%04X", conf);
	//>serial_write(mess);
	
	for (dconf = 0; dconf <= 7; dconf++) {
		delay = ports_imp_delay[dconf];
		
		if (val == delay) {
			break;
		}
	}
	
	conf |= dconf << 8;
	
	//>sprintf(mess, "conf after=%04X; dconf = %u", conf, dconf);
	//>serial_write(mess);
	
	gp_conf_write(num, conf);
}

void gp_max_save(uint8_t num, uint32_t value) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->max = value;
	
	eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_MAX, value);
}

uint8_t gp_imp_delay(uint8_t num) {
	
	//бит 8-10 (3 бита) - мин. длит. импульса
	
	uint16_t dconf = (gp_conf(num) >> 8) & 0x07;
	
	return ports_imp_delay[dconf];
}

uint32_t gp_max_get(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->max;
}

uint8_t gp_delay(uint8_t num) {
	tp_gpio_type   *Port  = gp_port(num);
	return Port->delay;
}

void gp_call_mode_set(uint8_t num, uint8_t mode) {
	tp_gpio_type   *Port  = gp_port(num);
	
	mode = (mode & 0x03) << 3;
	
	mode |= Port->mode & 0xE7; //?11100111?
	
	if (mode!=Port->mode) {
		Port->mode = mode;
		eeprom_write_byte(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_MODE, mode);
	}
}

void gp_call_save(uint8_t num, uint8_t rec, uint8_t calls, uint8_t interval) {
	
	//>tp_gpio_type   *Port  = gp_port(num);
	
	
	uint8_t byte = rec & 0x0F;
	
	if (calls) {calls--;}
	
	byte |= (calls & 0x03) << 4;
	
	uint8_t n;
	bool is_found = false;
	for (n = 0; n < 4; n++) {
		
		if (interval ==  ports_call_delay[n]) {
			is_found = true;
			break;
		}
	}
	interval = is_found ? n : 0;
	
	byte |= (interval & 0x03) << 6;
	
	eeprom_write_byte(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_PHONE, byte);
}

uint8_t gp_call_read(uint8_t num) {
	
	uint8_t byte = eeprom_read_byte(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_PHONE);
	
	if (byte == zval_8) {byte = 0;}
	
	return byte;
}

uint8_t gp_call_rec(uint8_t byte) {
	
	return byte & 0x0F;
}

uint8_t gp_call_number(uint8_t byte) {
	
	return ((byte >> 4) & 0x03) + 1;
}

uint8_t gp_call_delay(uint8_t byte) {
	
	return ports_call_delay[ (byte >> 6) & 0x03 ];
}

uint8_t gp_call_mode_get(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return (Port->mode >> 3) & 0x03;
}

uint32_t gp_cnt_get(uint8_t num) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	return Port->counter;
}

void gp_cnt_set(uint8_t num, uint32_t counter) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->counter = counter;
	
	//Сохраняем дату начала счета
	uint32_t        date = now();
	
	Port->dtupd = date;
	eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_DTUPD, date);
	
	//Сохраняем начало счета
	eeprom_write_uint32(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_CNT, counter);
}

uint16_t gp_log_idx_read(void) {
	
	uint16_t  idx = eeprom_read_uint16(EE_ELOG_REC);
	
	if (idx==zval_16) {
		idx = 0;
	}
	
	return idx;
}

void gp_log_idx_write(uint16_t idx) {
	
	if (idx != gp_log_idx_read()) {
		eeprom_write_uint16(EE_ELOG_REC, idx);
	}
}

int16_t gp_adc_rvalue(uint8_t num) {
	
	
	uint8_t        mod = gp_mode(num);

	if (mod != 1) {
		return 0;
	}
	
	
	//enum adcch_positive_input pin;
	//pin = ADCCH_POS_PIN0;  //A0 - default
	
	
	uint8_t rank = ADC_REGULAR_RANK_1;
	
	switch (num) {
		case 1:
			#ifdef PIN_GP01_ADC
				pin = PIN_GP01_ADC;
			#endif
		break;
		
		case 2:
			#ifdef PIN_GP02_ADC
				pin = PIN_GP02_ADC;
			#endif
		break;
		
		case 3:
			#ifdef PIN_GP03_ADC
				pin = PIN_GP03_ADC;
			#endif
		break;
		
		case 4:
			#ifdef PIN_GP04_ADC
				//pin = PIN_GP04_ADC;
				rank = PIN_GP04_ADC_RANK;
			#endif
		break;
		
		case 5:
			#ifdef PIN_GP05_ADC
				//pin = PIN_GP05_ADC;
				rank = PIN_GP05_ADC_RANK;
			#endif
		break;
		
		case 6:
			#ifdef PIN_GP06_ADC
				pin = PIN_GP06_ADC;
			#endif
		break;
		
		case 7:
			#ifdef PIN_GP07_ADC
				pin = PIN_GP07_ADC;
			#endif
		break;
		
		case 8:
			#ifdef PIN_GP08_ADC
				pin = PIN_GP08_ADC;
			#endif
		break;
		
		case 9:
			#ifdef PIN_GP09_ADC
				//pin = PIN_GP09_ADC;
				rank = PIN_GP09_ADC_RANK;
			#endif
		break;
			
		case 10:
			#ifdef PIN_GP10_ADC
				pin = PIN_GP10_ADC;
			#endif
		break;
			
		case 11:
			#ifdef PIN_GP11_ADC
				pin = PIN_GP11_ADC;
			#endif
		break;
			
		case 12:
			#ifdef PIN_GP12_ADC
				pin = PIN_GP12_ADC;
			#endif
		break;
			
		case 13:
			#ifdef PIN_GP13_ADC
				//pin = PIN_GP13_ADC;
				rank = PIN_GP13_ADC_RANK;
			#endif
		break;
		
		case 14:
			#ifdef PIN_GP14_ADC
				//pin = PIN_GP14_ADC;
				rank = PIN_GP14_ADC_RANK;
			#endif
		break;
		
		case 15:
			#ifdef PIN_GP15_ADC
				pin = PIN_GP15_ADC;
			#endif
		break;
		
		case 16:
			#ifdef PIN_GP16_ADC
				pin = PIN_GP16_ADC;
			#endif
		break;
		
		case 17:
			#ifdef PIN_GP17_ADC
				pin = PIN_GP17_ADC;
			#endif
		break;
		
		case 18:
			#ifdef PIN_GP18_ADC
				pin = PIN_GP18_ADC;
			#endif
		break;
		
		case 19:
			#ifdef PIN_GP19_ADC
				pin = PIN_GP19_ADC;
			#endif
		break;
		
		case 20:
			#ifdef PIN_GP20_ADC
				pin = PIN_GP20_ADC;
			#endif
		break; 
	}
	
	//init_adc();
	
	//ADC1_SetChannel(pin);
	
	
	
	uint16_t n;
	//uint32_t result = 0;
	uint8_t  max = 20;
	
	int16_t  res_ar[max];
	int16_t  swap;
	int32_t  avg = 0;
	uint16_t adc_data[ADC_CH_NUM];
		
	for (n = 0; n < max; n++) {
		
		//ADC1_Start();
		HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_data, ADC_CH_NUM);
		
		//adc_start_conversion(&ADCA, ADC_CH1);
		
		//while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
		//adc_wait_for_interrupt_flag(&ADCA, ADC_CH1);
		
		while (!adc_complete);
		
		res_ar[n] = adc_data[rank-1];//ADC1_GetResult() & 0x0000ffff;
		//res_ar[n] = adc_get_result(&ADCA, ADC_CH1);
		
		adc_complete = false;
	}
	
	//??????????
	bool is_swap;
	do {
		is_swap = false;
		for (n = 0; n < max-1; n++) {
			if (res_ar[n+1] > res_ar[n]) {
				is_swap = true;
					
				swap = res_ar[n];
				res_ar[n] = res_ar[n+1];
				res_ar[n+1] = swap;
			}
		}
	} while (is_swap);
		
	uint8_t c = 0;
	for (n = 5; n < 15; n++) {
		avg += res_ar[n];
		c++;
	}
		
	avg /= c;

	return avg;
}

void gp_mode_save(uint8_t num, uint8_t mod) {
	
	tp_gpio_type   *Port  = gp_port(num);
	
	uint8_t new_mode = (Port->mode & 0xF8) | (mod & 0x07);
	
	//Режим
	if (new_mode != Port->mode) {
		Port->mode = new_mode;
		eeprom_write_byte(EE_ADDR_PORTS + (num-1)*GP_STORE + GP_STORE_MODE, mod);
		
		gp_conf_init(num);
	}
}

void gp_value(uint8_t num, uint8_t format, char* ret, bool dt_short) {
	
	
	uint8_t    mod = gp_mode(num);
	char       str[32];
	char       cmd[32];
	//>uint16_t   value;
	uint16_t   conf = gp_conf(num);
	
	char dt[32] = "";
	uint32_t t = gp_get_upd(num);
	
	if (dt_short) {
		dateFormatS(t, dt);
	} else {
		dateFormat(t, dt);
	}
	
	tp_gpio_type   *Port  = gp_port(num);
	
	if (mod==GP_MODE_ADC) {
		//ADC
		float fval = gp_adc_val(num);
		/*
		if (num==13 || num==14) {
			int16_t rvalue = gp_adc_rvalue(num);
			
			fval = (float)rvalue / Port->kt;
			
			sprintf(mess, "rval = %d; kt=%0.2f fval=%0.2f", rvalue, Port->kt, fval);
			serial_write(mess);
		}
		*/
		sprintf(str, "%0.2f", fval);//(double)
	} else {
		//Если счетчик
		if (gp_conf_bit(conf, GP_CONFBIT_COUNTER )) {
			sprintf(str, "%u", Port->counter);
		} else {
			sprintf(str, "%u", gp_get_state(num));
		}
	}
	
	switch (format) {
		case 0:
			sprintf(ret, "%s", str);
		break;
		case 1:
			sprintf(ret, "%u=%s", num, str);
		break;
	
		case 2:
			sprintf(ret, "%s|%s", str, dt);
		break;
	
		default:
			sprintf(ret, "%u-%u-%04X-%s=%s;", num, mod, conf, dt, str);
		break;
	}
}

void gp_set_callback(uint8_t num, CallBack_t CallBack) {
	tp_gpio_type   *Port  = gp_port(num);
	
	Port->onCallBack = CallBack;
}

void gpin_init(uint8_t num)
{
		//PORT_StructInit(&GPIOInitStruct [num-1]);
	
		//GPIO_InitTypeDef GPIOInit = {0};
	
		//GPIOInitStruct[num-1] = GPIOInit;
		
		GPIO_TypeDef* GPIOx = PORT_DEFS[num-1].port;
		uint16_t PORT_Pin 	= PORT_DEFS[num-1].pin;
		uint32_t PORT_MODE 	= GPIO_MODE_INPUT;
		uint32_t PORT_PULL	= GPIO_NOPULL;
		uint32_t PORT_SPEED	= GPIO_SPEED_FREQ_LOW;
		
		//Enable Clocking
		gp_rst_clk(GPIOx);
	
		HAL_GPIO_WritePin(GPIOx, PORT_Pin, GPIO_PIN_RESET);
		
		uint8_t mode = PORT_DEFS[num-1].mod;//PORTS[num-1].mode;
		
		switch (mode) {
			case GP_MODE_OUT:
				PORT_MODE = GPIO_MODE_OUTPUT_PP;
			break;
			case GP_MODE_ADC:
				PORT_MODE = GPIO_MODE_ANALOG;
			break;
			case GP_MODE_INU:
				PORT_PULL = GPIO_PULLUP;
			break;
			case GP_MODE_IND:
				PORT_PULL = GPIO_PULLDOWN;
			break;
		}
		
		uint16_t conf = PORT_DEFS[num-1].config;
		
		//If open drain bit is set
		if ((conf & (1 << GP_CONFBIT_OPEN_DRAIN)) && (mode == GP_MODE_OUT)) {
			//Set open drain bit
			PORT_MODE = GPIO_MODE_OUTPUT_OD;
		}
		
		if (mode == GP_MODE_ADC) {
			/*
			GPIOInitStruct[num-1] = (GPIO_InitTypeDef) {
				.Pin 		= PORT_Pin,
				.Mode 	= GPIO_MODE_ANALOG
			};
			HAL_GPIO_Init(GPIOx, &GPIOInitStruct[num-1]);*/
		} else {
			GPIOInitStruct[num-1] = (GPIO_InitTypeDef) {
				.Pin 		= PORT_Pin,
				.Pull 	= PORT_PULL,
				.Mode 	= PORT_MODE,
				.Speed 	= PORT_SPEED
			};
			HAL_GPIO_Init(GPIOx, &GPIOInitStruct[num-1]);
		}
   
    //PORT_Init(GPIOx, &GPIOInitStruct[num-1]);
}

void gp_rst_clk(GPIO_TypeDef* GPIOx) {
	
		if ((GPIOx == GPIOA) && !(__HAL_RCC_GPIOA_IS_CLK_ENABLED())) {
			__HAL_RCC_GPIOA_CLK_ENABLE();
		}
		else if ((GPIOx == GPIOB) && __HAL_RCC_GPIOB_IS_CLK_DISABLED()) {
			__HAL_RCC_GPIOB_CLK_ENABLE();
		}
		else if ((GPIOx == GPIOC) && __HAL_RCC_GPIOC_IS_CLK_DISABLED()) {
			__HAL_RCC_GPIOC_CLK_ENABLE();
		}
		else if ((GPIOx == GPIOD) && __HAL_RCC_GPIOD_IS_CLK_DISABLED()) {
			__HAL_RCC_GPIOD_CLK_ENABLE();
		}/*
		else if ((MDR_PORTx == MDR_PORTE) && !(__HAL_RCC_GPIOD_IS_CLK_ENABLED())) {
			RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
		}
		else if ((MDR_PORTx == MDR_PORTF) && !(__HAL_RCC_GPIOE_IS_CLK_ENABLED())) {
			RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
		}*/
}

void gp_port_init(GPIO_TypeDef* GPIOx, uint16_t PORT_Pin, uint32_t PORT_MODE, uint8_t pullupdown) {
	
	gp_rst_clk(GPIOx);
	
	HAL_GPIO_WritePin(GPIOx, PORT_Pin, GPIO_PIN_RESET);
	
	GPIO_InitTypeDef GPIOInitStruct = {0};
		
	GPIOInitStruct = (GPIO_InitTypeDef) {
		.Pin = PORT_Pin,
		.Mode = PORT_MODE,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_HIGH
	};
	
	if (pullupdown == PULL_UP) {
		GPIOInitStruct.Pull = GPIO_PULLUP;
	
	} else if (pullupdown == PULL_DOWN) {
		GPIOInitStruct.Pull = GPIO_PULLDOWN;
	
	} else {
		GPIOInitStruct.Pull = GPIO_NOPULL;
	}
  
	HAL_GPIO_Init(GPIOx, &GPIOInitStruct);
	
	//PORT_ResetBits(GPIOx, PORT_Pin);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance == ADC1)
  {
    /*for (uint8_t i = 0; i < ADC_CH_NUM; i++)
    {
      //adcVoltage[i] = adcData[i] * 3.3 / 4095;
			
			//printf("ADC CH%u = %.2f V\r\n", i, adcVoltage[i]);
			//printf("ADC CH%u = %04X\r\n", i, adcData[i]);
    }*/
		adc_complete = true;
  }
}
