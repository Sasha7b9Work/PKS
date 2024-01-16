#include "Commands.h"

char sep[] = "|";
char lbl_on[] = "on";
char lbl_off[] = "off";
char lbl_ok[] = "ok";
char lbl_OK[] = "OK";
char lbl_start[] = "start";
char lbl_stop[] = "stop";
char lbl_fail[] = "fail";
char lbl_none[] = "none";
char lbl_set[] = "set";
char lbl_get[] = "get";
char lbl_unknown[] = "unknown";
char lbl_at[] = "AT";
char lbl_err[] = "ERROR";
char lbl_wrong[] = "wrong params";
char lbl_clear[] = "clear";
char lbl_stat[] = "stat";
//char lbl_bind[] = "bind";

//COMMANDS---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void command_process(char* data)
{
	
	char cmd[16];
	bool is_clear = true;
	clearString(out);
	
	paramStringMax(data,1,cmd,sizeof(cmd));
	
	if ( !strcmp(cmd, "ver") ) {
		command_ver(cmd);
	}

	else if ( !strcmp(cmd, "hello") ) {
		command_hello(cmd, data);
	}

	else if ( !strcmp(cmd, "reset") ) {
		command_reset(cmd, data);
	}

	else if ( !strcmp(cmd, "task") ) {
		command_task(cmd, data);
	}
	
	else if ( !strcmp(cmd, "info") ) {
		command_info(cmd);
	}
	
	else if ( !strcmp(cmd, "time") ) {
		command_time(cmd, data);
	}
	
	else if ( !strcmp(cmd, "gp") ) {
		command_gp(cmd, data);
	}
	
	else if ( !strcmp(cmd, "mem") ) {
		command_mem(cmd, data);
	}
	
	else if ( !strcmp(cmd, "elog") ) {
		command_elog(cmd, data);
	}
	
	else if ( !strcmp(cmd, "rom") ) {
		command_rom(cmd, data);
	}
	
	else if ( !strcmp(cmd, "romcrc") ) {
		command_romcrc(cmd, data);
	}
	
	else if ( !strcmp(cmd, "flash") ) {
		command_flash(cmd, data);
	}
	
	else if ( !strcmp(cmd, "server") ) {
		command_server(cmd, data);
	}

	else if ( !strcmp(cmd, "addr") ) {
		command_addr(cmd, data);
	}
	
	else if ( !strcmp(cmd, "com") ) {
		command_com(cmd, data);
	}

	else if ( !strcmp(cmd, "rs") ) {
		command_rs(cmd, data);
		is_clear = false;
	}

	else if ( !strcmp(cmd, "ns") ) {
		command_ns(cmd, data);
	}
	
	else if ( !strcmp(cmd, "timer") ) {
		command_timer(cmd, data);
	}
	
	else if ( !strcmp(cmd, lbl_set) ) {
		command_set(cmd, data);
	}
	
	else if ( !strcmp(cmd, "wdt") ) {
		command_wdt(cmd, data);
	}
	
	else if ( !strcmp(cmd, "modem") ) {
		command_modem(cmd, data);
	}
	
	else if ( !strcmp(cmd, "phone") ) {
		command_phone(cmd, data);
	}
	
	else if ( !strcmp(cmd, "gprs") ) {
		command_gprs(cmd, data);
	}
	
	else if ( !strcmp(cmd, "sms") ) {
		command_sms(cmd, data);
	}
	
	else if ( !strcmp(cmd, "test") ) {
		command_test(cmd, data);
	}

//--------------------------------------------------------------------------------------	
	
	else if ( !strcmp(cmd, "bind") ) {
		command_bind(cmd, data);
	}
	else if ( !strcmp(cmd, "blink") ) {
		command_blink(cmd, data);
	}
	else if ( !strcmp(cmd, "port") ) {
		command_port(cmd, data);
	}
	
	else if ( !strcmp(cmd, "cont") ) {
		command_cont(cmd, data);
	}

	else if ( !strcmp(cmd, "contlog") ) {
		command_contlog(cmd, data);
	}
	
	else if ( !strcmp(cmd, "in") ) {
		command_in(cmd, data);
	}
	
	else if ( !strcmp(cmd, "scan") ) {
		command_scan(cmd, data);
	}
	
	else if ( !strcmp(cmd, "arc") ) {
		command_arc(cmd, data);
	}
	
	else if ( !strcmp(cmd, "phase") ) {
		command_phase(cmd, data);
	}
	
	else if ( !strcmp(cmd, "cal") ) {
		command_cal(cmd, data);
	}
	
	else if ( !strcmp(cmd, "net") ) {
		command_net(cmd, data);
	}
	
	else if ( !strcmp(cmd, "run") ) {
		command_run(cmd, data);
	}
	
	else if ( !strcmp(cmd, "step") ) {
		command_step(cmd, data);
	}

	else if ( !strcmp(cmd, "debug") ) {
		command_debug(cmd, data);
	}
	
	else {
		command_unknown(cmd);
	}
	if (is_clear) {
		clearString(data);
	}
}

void command_unknown(char* cmd)
{
	sprintf(cmdData, "%s|%s", cmd, lbl_unknown);
}

void command_ver(char* cmd) {
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, VERSION);
	
	strcat(cmdData, sep);
	
	if (eeprom_read_byte(EE_BOARD) == zval_8) {
		strcat(cmdData, lbl_none);
	} else {
		char       str_board[10];
		eeprom_read_buffer(EE_BOARD, (void *)&str_board, sizeof(str_board));
		strcat(cmdData, str_board);
	}
}

void command_info(char* cmd) {
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	
	char str[64] = "";
	char fval[16] = "";
	
	tp_regulator_type *regulator = Regulator_get();
		
	sprintf(mess, "%s|%s|%u|%u|%u|%u|%u|%u", VERSION, "0", regulator->wd, regulator->dt_restart, regulator->dt_reg, regulator->dt_net, regulator->dt_net_reset, regulator->dt_archive);
	strcat(cmdData, mess);
}

void command_reset(char* cmd, char* data) {
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	if (!strlen(param)) { //param==""
		IS_RESET = 1;
	} else {
		
		Timers_stop(TIMER_WDT);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, lbl_ok);
}

//Ответ на приветствие сервера
void command_hello(char* cmd, char* data) {
	
	char str[64];
	modem_hello_time = now();

	strcat(cmdData, "connect");
	
	strcat(cmdData, sep);
	modem_param(str, 1); //imei
	strcat(cmdData, str);
				
	//Сигнал
	strcat(cmdData, sep);
	modem_param(str, 2); //csq
	strcat(cmdData, str);
				
	//Время
	strcat(cmdData, sep);
	dateFormatS(now(), str);
	strcat(cmdData, str);
				
	//Широта
	strcat(cmdData, sep);
	modem_param(str, 3); //lat
	strcat(cmdData, str);
				
	//Долгота
	strcat(cmdData, sep);
	modem_param(str, 4); //lon
	strcat(cmdData, str);

	//Температура
	strcat(cmdData, sep);
	strcat(cmdData, "0");

	//Версия
	strcat(cmdData, sep);
	strcat(cmdData, VERSION);

	//Формат COM порта
	/*
	uint32_t spd;
	char mode[8] = "";
	if (eeprom_read_byte(EE_COM_SET) == set_val) {
		eeprom_read_buffer(EE_COM_MODE, (void *)&mode, 3);
		spd = eeprom_read_uint32(EE_COM_SPEED);
	} else {
		strcpy(mode, RS485_MODE);
		spd = RS485_SPEED;
	}
	StrFlash(cmd, 7); //%lu-%s-%u
	sprintf(mess, cmd, spd, mode, serial_timeout_get());
	strcat(cmdData, mess);
	strcat(cmdData, sep);
	*/
	strcat(cmdData, sep);
	strcat(cmdData, "57600-8N1-500");

	//Состояние портов
	strcat(cmdData, sep);
	for (uint8_t num = 1; num <= GP_MAX; num++) {
	
		gp_value(num, 3, mess, true);
		strcat(cmdData, mess);
	}
}

void command_task(char* cmd, char* data) {
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[16];
	paramStringMax(data,3,sub,sizeof(sub));
	
	if (!strlen(param)) {
		sprintf(out, "%u", Task_num());
	} else {
		
		strcat(out, param);
		strcat(out, sep);
		
		tp_task_type *task = Task_by_name(param);
		if (task == NULL) {
			strcat(out, lbl_wrong);
		} else {
			if (!strlen(sub)) {
				Task_stat_one(task, mess);
				strcat(out, mess);
			} else {
				
				strcat(out, sub);
				strcat(out, sep);
				
				if ( !strcmp(sub, "start") ) {
					Task_start_tp(task);
					strcat(out, lbl_ok);
				
				} else if ( !strcmp(sub, "stop") ) {
					Task_stop(task);
					strcat(out, lbl_ok);
				
				} else {
					strcat(out, lbl_wrong);
				}
			}
		}
		
	}
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_time(char* cmd, char* data)
{
	uint32_t time = 0;
	uint16_t ival;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	char str[32];
	
	if (!strlen(param)) { //param==""
		
		time = now();
		
		dateFormat(time, out);

	} else {
		strcat(out, param);
		strcat(out, sep);
		
		if ( !strcmp(param, lbl_set) ) {

			if (strlen(sub)>=19) {
				
				struct calendar_date cdate;
				
				int y,m,d,h,i,s;
				
				//2022-07-13 18:03:00
				if ( sscanf(sub, "%d-%d-%d %d:%d:%d", &y, &m, &d, &h, &i, &s) == 6 ) {
					
					cdate.year = y;
					cdate.month = m-1;
					cdate.date = d-1;
					cdate.hour = h;
					cdate.minute = i;
					cdate.second = s;
				
					time = calendar_date_to_timestamp( &cdate );
				}
			} else {
				sscanf(sub, "%u", &time);
			}
		
			if (time > TIME_MIN) {
			
				//Разница
				int32_t dt = time - now();
				
				/*
				if (modem_hello_time) {
					modem_hello_time = (uint32_t)(modem_hello_time + dt);
				}
				*/
			
				//Обновляем таймеры
				Timers_shift(dt);
			
				//Обновляем таски
				Task_dt_update(time);
				
				//Обновляем RTC
				
				//__HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);
				SET_BIT(hrtc.Instance->CRL, RTC_CRL_CNF);
			
				WRITE_REG(hrtc.Instance->CNTH, (time >> 16U));
	
				WRITE_REG(hrtc.Instance->CNTL, (time & RTC_CNTL_RTC_CNT));
			
				//__HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);
				CLEAR_BIT(hrtc.Instance->CRL, RTC_CRL_CNF);
			
				strcat(out, lbl_ok);
			
			} else {
				strcat(out, lbl_fail);
			}
		
		} else if ( !strcmp(param, "restart") ) {
			
			time = eeprom_read_uint32(EE_ADDR_RESTART);
	
			if (time != zval_32) {
				dateFormat(time, mess);
				strcat(out, mess);
			} else {
				strcat(out, lbl_none);
			}
			

		} else {
			strcat(out, lbl_unknown);
		}
	}
	
	sprintf(cmdData, "%s|%s", cmd, out);
}

void command_mem(char* cmd, char* data) {
	
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	uint8_t data_initial[8];
	uint8_t data_received[8];
	
	uint32_t t1 = 0;
	uint32_t t2 = 0;
	
	//Банк 1
	if ( !strcmp(param, "t") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		uint8_t data_send[8] = {0x00, 0x25, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
		
		uint32_t addr;
		
		addr = 0;
		//char str_ok[32];
		//char str_fail[32];
		uint8_t is_ok = 0;
		
		for (uint8_t bank = 0; bank <= 3 ; bank++) {
			
			if ( M24M01_read(bank, addr, &data_initial, sizeof(data_initial)) ) {
				
				is_ok = 1;
		
				counter_micros_start();
				
				if ( M24M01_write(bank, addr, &data_send, sizeof(data_send)) ) {
					t1 = counter_micros_stop();
					
					sprintf(mess, "B%u write = %u", bank, t1);
					serial_write(mess);
					
					//Необходима задержка для осуществления записи перед чтением
					delay_millis(10);
					
					counter_micros_start();
					
					if ( M24M01_read(bank, addr, &data_received, sizeof(data_received)) ) {
						
						t2 = counter_micros_stop();
						
						sprintf(mess, "B%u read OK = %u", bank, t2);
						serial_write(mess);
						
						for (uint8_t n = 0; n < sizeof(data_received); n++) {
							sprintf(mess, "Byte=%u, value=%02X", n, data_received[n]);
							serial_write(mess);
						}
						
						M24M01_write(bank, addr, &data_initial, sizeof(data_initial));
						
					} else {
						counter_micros_stop();
						
						sprintf(mess, "B%u read fail", bank);
						serial_write(mess);
					}
					
				} else {
					counter_micros_stop();
					
					sprintf(mess, "B%u write fail", bank);
					serial_write(mess);
				}
				

			} else {
				is_ok = 0;
				
				sprintf(mess, "B%u read fail", bank);
				serial_write(mess);
			}
			sprintf(mess, "b%u=%u|", bank, is_ok);
			strcat(out, mess);
		}
		
		strcat(out, lbl_ok);
	
	} else if ( !strcmp(param, "r") || !strcmp(param, "crc") ) {
		
		char param1[16];
		paramStringMax(data,3,param1,sizeof(param1));
		
		char param2[16];
		paramStringMax(data,4,param2,sizeof(param2));
		
		char param3[16];
		paramStringMax(data,5,param3,sizeof(param3));
		
		//????
		uint32_t bank = 0;
		sscanf(param1, "%u", &bank);
		if (bank > 3) {bank = 3;}
		
		//?????
		uint32_t addr = 0;
		sscanf(param2, "%06X", &addr);
		
		//?????
		uint32_t len = 0;
		sscanf(param3, "%02X", &len);
		if (len > 128) {len = 128;}
		
		uint8_t data_read[128];
		uint16_t crc = 0;
		
		char str[32];
		
		if ( M24M01_read(bank, addr, &data_read, len) ) {
			crc = crc16(data_read, len, CRC_INIT);
			
			clearString(mess);
			
			if ( !strcmp(param, "r") ) {
				if (len > 64) {len = 64;}
				HexString(data_read, len, mess);
			} else {
				strcat(mess, lbl_ok);
			}
		}
		
		sprintf(out, "%s|%u|%06X|%02X|%04X|%s", param, bank, addr, len, crc, mess);
		
	} else if ( !strcmp(param, lbl_clear) ) {
		
		strcat(out, param);
		strcat(out, sep);
		 
		 char param1[16];
		 paramStringMax(data,3,param1,sizeof(param1));
		 
		 if (strlen(param1)) {
		 
			 //????
			 uint32_t bank = 0;
			 sscanf(param1, "%u", &bank);
			 if (bank > 3) {bank = 3;}
		
			uint32_t addr = 0;
			uint8_t len = 128;
			uint8_t data_send[len];
		
			for (uint8_t n=0; n < len; n++) {
				data_send[n] = zval_8;
			}
			uint32_t wdt = millis();
			uint32_t m = 0;
			
			for (uint16_t p=0; p < 8*128; p++) {
				
				//sprintf(mess, "%u|%06X", p, addr);
				//serial_write(mess);
				
				M24M01_write(bank, addr, &data_send, len);
				
				addr += len;
				
				m = millis();
				
				if (m - wdt > 100)  {
					sys_wdt_reset();
					wdt = m;
				}
			}
			
			sprintf(mess, "%u", bank);
			strcat(out, mess);
			strcat(out, sep);
			strcat(out, lbl_ok);
		} else {
			strcat(out, lbl_wrong);
		}
	
	} else {
		strcat(out, lbl_unknown);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_gp(char* cmd, char* data) {
	
	uint32_t       num;
	//uint16_t       value;
	uint8_t        state = 0;
	uint8_t        mod = 0;
	uint32_t       conf = 0;
	
	char gpnum[64];
	paramStringMax(data,2,gpnum,sizeof(gpnum));
	
	char param[16];
	paramStringMax(data,3,param,sizeof(param));
	
	char param_4[16];
	paramStringMax(data,4,param_4,sizeof(param_4));
	
	char param_5[16];
	paramStringMax(data,5,param_5,sizeof(param_5));
	
	char str[32];
	
	clearString(cmdData);
	
	if (!strlen(gpnum)) {
		for (uint8_t n = 1; n <= GP_MAX; n++) {
			
			gp_value(n, 0, mess, true);
			strcat(out, mess);
			if (n < GP_MAX) {
				strcat(out, ";");
			}
		}
	} else {
		
		char *ret;
		ret = strstr(gpnum, ",");
		if (ret!=NULL) {
			
			char pnum[16];
			
			for (uint8_t n = 1; n <= GP_MAX; n++) {
				
				//Ищем номер порта в параметре
				
				paramStringC(gpnum, n, ',', pnum);
				
				if (!strlen(pnum)) {
					break;
				} else {
					sscanf(pnum, "%u", &num);
					
					gp_value(num, 1, mess, true);
					strcat(out, mess);
					strcat(out, ";");
				}
			}
			
		} else {
			
			sscanf(gpnum, "%u", &num);
			//num = atoi(gpnum);
			if (num>=1 && num<=GP_MAX) {
				
				strcat(out, gpnum);
				strcat(out, sep);
				
				//Состояние порта
				if (!strlen(param)) {
					
					gp_value(num, 2, mess, false);
					strcat(out, mess);
					
				} else if ( !strcmp(param, "bind") ) { //связка

					strcat(out, param);
					strcat(out, sep);
					
					uint8_t   inverse = 0;
					
					if (strlen(param_4) > 0) {
						//???????
						uint32_t  bind = 0;
						sscanf(param_4, "%u", &bind);
						
						strcat(out, param_4);
						strcat(out, sep);
						
						if ( !strcmp(param_5, "1") ) {
							inverse = 1;
						}
						
						if (bind!=num) {
							gp_bind_set(num, bind, inverse);
							strcat(out, lbl_ok);
						} else {
							strcat(out, lbl_fail);
						}
						
					} else {
						//просмотр
						sprintf(mess, "%u|%u", gp_bind(num), gp_bind_inverse(num));
						strcat(out, mess);
					}
			#ifdef GP_DT_SAVE
				} else if ( !strcmp(param, "dt") ) { //даты

					strcat(out, param);
					strcat(out, sep);
					
					uint32_t date = gp_get_upd(num);
					
					dateFormat(date, mess);
					strcat(out, mess);
					
					strcat(out, sep);
					
					date = gp_get_prv(num);
					
					dateFormat(date, mess);
					strcat(out, mess);
			#endif
					
				} else if ( !strcmp(param, "conf") ) { //конфигурация
					
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
						//задание
						sscanf(param_4, "%04X", &conf);
						
						strcat(out, param_4);
						strcat(out, sep);

						gp_conf_write(num, conf);
						
						gp_bind_renew(num);
						
						strcat(out, lbl_ok);
						
					} else {
						//просмотр
						sprintf(mess, "%04X", gp_conf(num));
						strcat(out, mess);
					}
					
				} else if ( !strcmp(param, "call") ) { //статистика звонка
					
					strcat(out, param);
					strcat(out, sep);
					
					uint16_t sec = gp_dtCall_get(num);
					uint16_t retries = gp_call_retries_get(num);
					
					sprintf(mess, "%u|%u", retries, sec);
					strcat(out, mess);
					
					
				} else if ( !strcmp(param, lbl_set) ) { //установка
					
					//set|out,adc,inu,ind,in,pwm
					
					if ( !strcmp(param_4, "ina") || !strcmp(param_4, "adc") ) {
						//ADC
						mod = 1;
					} else if ( !strcmp(param_4, "inu") ) {
						//Digital in pullup
						mod = 2;
					} else if ( !strcmp(param_4, "ind") ) {
						//Digital in pulldown
						mod = 3;
					} else if ( !strcmp(param_4, "in") ) {
						//Digital in
						mod = 4;
					} else if ( !strcmp(param_4, "pwm") ) {
						mod = 5;
					} else if ( !strcmp(param_4, "reg") ) {
						mod = 6;
					} else {
						mod = 0; //out
					}

					if (strlen(param_5) > 0) {
						//Задана конфигурация
						sscanf(param_5, "%04X", &conf);
						
						gp_conf_write((uint8_t)num, (uint16_t)conf);
					}
					
					gp_mode_save((uint8_t)num, mod);
					
					gp_bind_renew((uint8_t)num);
					
					strcat(out, param);
					strcat(out, sep);
					strcat(out, lbl_ok);
					
				} else if ( !strcmp(param, "kt") ) { //коэффициент
					
					strcat(out, param);
					strcat(out, sep);
					
					float fval = 0;
					
					if (strlen(param_4) > 0) {
						
						//задание
						fval = atof(param_4);
						
						gp_kt_save(num, fval);
					}
					
					//просмотр
					fval = gp_kt_read(num);
					//dtostrf(fval, 1, 6, mess);
					sprintf(mess, "%0.6f", (double)fval);
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
				} else if ( !strcmp(param, "ktval") ) { //вычисление к-та по значению
				
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
					
						float fval = atof(param_4);

						int16_t rval = gp_adc_rvalue(num);
						int16_t zval = gp_zvalue_read(num);
						rval -= zval;
						if (rval < 0) {
							rval = 0;
						}

						gp_kt_save(num, (float)rval / fval);
						
						fval = gp_kt_read(num);
						//dtostrf(fval, 1, 6, mess);
						sprintf(mess, "%0.6f", (double)fval);
						strcat(out, mess);
						
						strcat(out, sep);
						strcat(out, lbl_ok);
					} else {
						strcat(out, lbl_wrong);
					}
					
				}   else if ( !strcmp(param, "zval") ) { //нулевое значение
					
					strcat(out, param);
					strcat(out, sep);
					
					int32_t   zvalue = 0;
					
					if (strlen(param_4) > 0) {
						//задание
						sscanf(param_4, "%d", &zvalue);
						
						strcat(out, param_4);
						strcat(out, sep);
						
						gp_zvalue_write(num, zvalue);
						strcat(out, lbl_ok);
						
					} else {
						//просмотр
						sprintf(mess, "%d", gp_zvalue_read(num));
						strcat(out, mess);
					}
					
				} else if ( !strcmp(param, "rval") ) { //сырое значение ADC
				
					strcat(out, param);
					strcat(out, sep);
					
					if (gp_mode(num)==1) {
						sprintf(mess, "%d", gp_adc_rvalue(num));
						strcat(out, mess);
					} else {
						strcat(out, lbl_fail);
					}
					
				} else if ( !strcmp(param, "min") ) {
					
					strcat(out, param);
					strcat(out, sep);
					
					float    fval = 0;
					uint8_t  inverse = 0;
					uint32_t bind = 0;
					
					if (strlen(param_4) > 0) {
						
						//задание
						fval = atof(param_4);
						
						gp_min_saveF(num, fval);
						
						if (strlen(param_5) > 0) {
							
							sscanf(param_5, "%u", &bind);
							
							char param_6[16];
							paramStringMax(data,6,param_6,sizeof(param_6));
							
							if ( !strcmp(param_6, "1") ) {
								inverse = 1;
								strcat(out, param_6);
								strcat(out, sep);
							}
							
							if (bind!=num && bind <= GP_MAX) {
								gp_bind_min_set(num, bind, inverse);
							}
						}
					}
					
					//просмотр
					fval = gp_min_getF(num);
					//dtostrf(fval, 1, 3, mess);
					sprintf(mess, "%0.3f", (double)fval);
					strcat(out, mess);
					
					strcat(out, sep);
					
					sprintf(mess, "%u|%u", gp_bind_min(num), gp_bind_inverse_min(num));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
				}   else if ( !strcmp(param, "max") ) {
					
					strcat(out, param);
					strcat(out, sep);
					
					float    fval = 0;
					uint8_t  inverse = 0;
					uint32_t bind = 0;
					
					if (strlen(param_4) > 0) {
						
						//задание
						fval = atof(param_4);
						
						gp_max_saveF(num, fval);
						
						if (strlen(param_5) > 0) {
							
							sscanf(param_5, "%u", &bind);
							
							char param_6[16];
							paramStringMax(data,6,param_6,sizeof(param_6));
							
							if ( !strcmp(param_6, "1") ) {
								inverse = 1;
								strcat(out, param_6);
								strcat(out, sep);
							}
							
							if (bind!=num && bind <= GP_MAX) {
								gp_bind_set(num, bind, inverse);
							}
						}
					}
					
					//просмотр
					fval = gp_max_getF(num);
					//dtostrf(fval, 1, 3, mess);
					sprintf(mess, "%0.3f", (double)fval);
					strcat(out, mess);
					
					strcat(out, sep);
					
					sprintf(mess, "%u|%u", gp_bind(num), gp_bind_inverse(num));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
				} else if ( !strcmp(param, "in") ) { //параметры входа
					
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
						//задание
						
						//param_4 - сканирование 0 - ms / 1 - sec
						//param_5 - инкремент счетчика 0/1
						//param_6 - мин. длит. импульса
						//param_7 - макс значение счетчика
						
						uint32_t  bit;
						
						sscanf(param_4, "%u", &bit);
						//sprintf(mess, "%s;val=%u", param_4, bit);
						//serial_write(mess);
						
						gp_conf_write_bit(num, GP_CONFBIT_TPSCAN, bit);
						
						if (strlen(param_5) > 0) {
							sscanf(param_5, "%u", &bit);
							
							//sprintf(mess, "%s;val=%u", param_5, bit);
							//serial_write(mess);
							
							gp_conf_write_bit(num, GP_CONFBIT_COUNTER, bit);
						}
						
						char prm[16];
						paramStringMax(data,6,prm,sizeof(prm));
						
						if (strlen(prm) > 0) {
							sscanf(prm, "%u", &bit);
							
							gp_conf_write_delay(num, bit);
						}
						
						//Макс
						uint32_t  cnt;
						paramStringMax(data,7,prm,sizeof(prm));
						
						if (strlen(prm) > 0) {
							sscanf(prm, "%u", &cnt);
							
							gp_max_save(num, cnt);
						}
					}
					
					conf = gp_conf(num);
					
					//просмотр
					sprintf(mess, "%u|%u|%u|%u", gp_conf_bit(conf, GP_CONFBIT_TPSCAN), gp_conf_bit(conf, GP_CONFBIT_COUNTER), gp_imp_delay(num), gp_max_get(num));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
				} else if ( !strcmp(param, "real") ) { //данные входа
					
					strcat(out, param);
					strcat(out, sep);
					
					sprintf(mess, "%u|%u", gp_get(num), gp_delay(num));
					strcat(out, mess);
					
				} else if ( !strcmp(param, "phone") ) { //????????? ??????
					
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
						//задание
						
						//param_4 - номер телефона в книге, 0 - звонок не используется
						//param_5 - кол-во звонков 1-4
						//param_6 - интервал звонка 5 мин, 15 мин, 30 мин, 60 мин
						//param_7 - уровень срабатывания (0-3)
						
						uint32_t  rec;
						uint32_t  calls = 0;
						uint32_t  interval = 0;
						
						sscanf(param_4, "%u", &rec);
						
						if (strlen(param_5) > 0) {
							sscanf(param_5, "%u", &calls);
						}
						if (!calls) {calls = 1;}
						
						char param_6[16];
						paramStringMax(data,6,param_6,sizeof(param_6));
						
						if (strlen(param_6) > 0) {
							sscanf(param_6, "%u", &interval);
						}
						if (!interval) {interval = 5;}
						
						char param_7[16];
						paramStringMax(data,7,param_7,sizeof(param_7));
						
						if (strlen(param_7) > 0) {
							uint32_t  mode = 0;
							sscanf(param_7, "%u", &mode);
							gp_call_mode_set(num, mode);
						}
						
						gp_call_save(num, rec, calls, interval);
					}
					
					uint8_t byte = gp_call_read(num);
					
					//просмотр
					sprintf(mess, "%u|%u|%u|%u", gp_call_rec(byte), gp_call_number(byte), gp_call_delay(byte), gp_call_mode_get(num));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
					} else if ( !strcmp(param, "cnt") ) { //счетчик импульсов
					
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
						//задание
						
						uint32_t  cnt = 0;
						sscanf(param_4, "%u", &cnt);
						
						gp_cnt_set(num, cnt);
						
						if (strlen(param_5) > 0) {
							sscanf(param_5, "%u", &cnt);
							
							gp_max_save(num, cnt);
						}
					}
					
					//просмотр
					sprintf(mess, "%u|%u", gp_cnt_get(num), gp_max_get(num));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
				}
				else if ( !strcmp(param, "log") ) { //параметры отправки на сервер
					
					strcat(out, param);
					strcat(out, sep);
					
					if (strlen(param_4) > 0) {
						//задание
						
						uint32_t  val = 0;
						sscanf(param_4, "%u", &val);
						
						gp_conf_write_bit(num, GP_CONFBIT_LOG, val);
					}
					
					//просмотр
					conf = gp_conf(num);
					sprintf(mess, "%u", gp_conf_bit(conf, GP_CONFBIT_LOG));
					strcat(out, mess);
					
					if (strlen(param_4) > 0) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
					
					
				} else if ( !strcmp(param, "sv") ) { //сохранение значения

					strcat(out, param);
					strcat(out, sep);
					
					mod = gp_mode(num);
					
					if (!mod) {
					
						if (strlen(param_4) > 0) {
							//задание
						
							uint32_t  val = 0;
							sscanf(param_4, "%u", &val);
						
							gp_conf_write_bit(num, GP_CONFBIT_SAVE, val);
						}
					
						//просмотр
						conf = gp_conf(num);
						sprintf(mess, "%u", gp_conf_bit(conf, GP_CONFBIT_SAVE));
						strcat(out, mess);
					
						if (strlen(param_4) > 0) {
							strcat(out, sep);
							strcat(out, lbl_ok);
						}
					} else {
						strcat(out, lbl_fail);
					}
					
				} else if (!strcmp(param, lbl_get)) {
					
					mod = gp_mode(num);

					strcat(out, param);
					strcat(out, sep);
					
					if (mod==1) {
						strcat(out, "adc");
					} else if (mod==2) {
						strcat(out, "inu");
					} else if (mod==3) {
						strcat(out, "ind");
					} else if (mod==4) {
						strcat(out, "in");
					} else if (mod==5) {
						strcat(out, "pwm");
					} else if (mod==6) {
						strcat(out, "reg");
					} else {
						strcat(out, "out");
					}
					
					strcat(out, sep);
					
					conf = gp_conf(num);
					sprintf(mess, "%04X", conf);
					strcat(out, mess);
					
				} else {
					//Установка значения порта
					bool is_ok = false;
					
					mod = gp_mode(num);
					
					if (!mod || mod==6) {//только выход и регистр
						
						if (!strcmp(param, lbl_on) || !strcmp(param, "1")) {
							state = 1;
							is_ok = true;
						}
						if (!strcmp(param, lbl_off) || !strcmp(param, "0")) {
							state = 0;
							is_ok = true;
						}
						if (is_ok) {
							gp_set_state(num, state);
						}
					}
					
					strcat(out, param);
					strcat(out, sep);
					if (is_ok) {
						strcat(out, lbl_ok);
					} else {
						strcat(out, lbl_fail);
					}
				}
				
			} else {
				strcat(out, lbl_wrong);
			}
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_rom(char* cmd, char* data) {
	
	char str[64];
	
	char param1[16];
	paramStringMax(data,2,param1,7); //sizeof(param1)
	
	char param2[16];
	paramStringMax(data,3,param2,3); //sizeof(param2)
	
	char param3[16];
	paramStringMax(data,4,param3,5); //sizeof(param3)
	
	uint8_t  err_num = 0;
	uint8_t  byte = 0;
	uint32_t addr = 0;
	uint32_t addr_real = 0;
	uint32_t len = 0;
	uint32_t crc = 0;
	uint16_t crc_data;
	uint8_t  bank = FLASH_BANK_N1;
	
	uint8_t data_mem[128];
	//>uint8_t data_read[128];
	
	//Адрес
	sscanf(param1, "%06X", &addr);
	
	//Длина
	sscanf(param2, "%02X", &len);
	if (len > 128) {len = 128;}
	
	//CRC
	sscanf(param3, "%04X", &crc);
	
	shiftData(data, 19, SERIAL_DATA_LEN);
	
	if (strlen(data) < len*2 || addr+len >= FLASH_MAX_SIZE) {
		err_num = 5;
	}
	
	if (!err_num) {
	
		if (addr >= FLASH_BANK_SIZE) {
			addr_real = addr - FLASH_BANK_SIZE;
			bank = FLASH_BANK_N2;
		} else {
			addr_real = addr;
		}
		
		//1FF80 + 80 = 20000 OK
		//1FF85 + 80 = 20000 FAIL
		if (addr_real + len > FLASH_BANK_SIZE) {
			err_num = 6;
		}
	}
	
	sys_wdt_reset();
	
	//Преобразование в байты
	if (!err_num) {
		uint8_t i = 0;
		for (i = 0; i < len; i++) {
			byte = toHex(data[i*2+1]) + (toHex(data[i*2]) << 4);
			data_mem[i] = byte;
		}
		
		crc_data = crc16(data_mem, len, CRC_INIT);
		
		if (crc!=crc_data) {
			err_num = 1;
		}
	}
	
	if (!err_num) {
		//Переменная глобальная, не работает указатель на &data, а &data[0] работает
		if ( !M24M01_write(bank, addr_real, &data_mem[0], len) ) {
			err_num = 2;
		}
	}
	
	for (uint16_t i = 0; i < len; i++) {
		data_mem[i]=0x00;
	}
	
	delay_millis(5);
	
	if (!err_num) {
		sys_wdt_reset();
		
		if ( !M24M01_read(bank, addr_real, &data_mem[0], len) ) {
			err_num = 3;
		}
	}
	
	if (!err_num) {
		sys_wdt_reset();
		
		crc_data = crc16(data_mem, len, CRC_INIT);
		
		if (crc!=crc_data) {
			err_num = 4;
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	sprintf(str, "%06X|%02X|%04X|%u", addr, len, crc, err_num);
	strcat(cmdData, str);
}

void command_romcrc(char* cmd, char* data) {
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	uint8_t  bank = FLASH_BANK_N1;
	uint32_t addr = 0;
	uint8_t  data_read[SLICE_SIZE];
	uint16_t crc = CRC_INIT;
	uint8_t  len;
	uint16_t page;
	
	//Длина
	uint32_t flash_size = 0;
	uint32_t flash_len = 0;
	sscanf(param, "%u", &flash_size);
	flash_len = flash_size;// = fs;
	//flash_len = flash_size;
	
	uint32_t m = millis();
	
	if (flash_size) {
		
		do {
			if (millis() - m > 100) {
				sys_wdt_reset();
				m = millis();
			}
			
			len = flash_size >=SLICE_SIZE ? SLICE_SIZE : flash_size;
			
			if ( M24M01_read(bank, addr, &data_read[0], len) ) {
				
				crc = crc16(data_read, len, crc);
				
			} else {
				break;
			}
			
			flash_size -= len;
			addr       += len;
			
			if (addr >= FLASH_BANK_SIZE) {
				addr -= FLASH_BANK_SIZE;
				bank = FLASH_BANK_N2;
			}
			page++;
			
		} while (flash_size);
		
		sprintf(out,"%u|%04X", flash_len, crc);
		
	} else {
		strcat(out, lbl_fail);
	}
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_flash(char* cmd, char* data)
{
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char param2[16];
	paramStringMax(data,3,param2,sizeof(param2));
	
	char param3[16];
	paramStringMax(data,4,param3,sizeof(param3));
	
	if (!strlen(param)) {
		
		uint8_t  flash_res  = eeprom_read_byte(EE_FLASH_RES);
		uint32_t flash_size = eeprom_read_uint32(EE_FLASH_SIZE);
		uint16_t flash_hash = eeprom_read_uint16(EE_FLASH_HASH);
		uint32_t flash_time = eeprom_read_uint32(EE_FLASH_TIME);
		uint16_t flash_cnt  = eeprom_read_uint16(EE_FLASH_CNT);
		
		if (flash_cnt==zval_16) {
			flash_cnt = 0;
		}
		
		char dt[32];
		
		dateFormat(flash_time, dt);
		
		if (flash_size != zval_32) {
			sprintf(mess, "%u|%04X|%u|%s|%u", flash_size, flash_hash, flash_res, dt, flash_cnt);
			strcat(out, mess);
		} else {
			strcat(out, lbl_none);
		}
	} else {
		
		strcat(out, param);
		strcat(out, sep);
	
		if ( !strcmp(param, FLASH_PASS) && strlen(param2) > 0 && strlen(param3) > 0 ) {
		
			//Длина
			uint32_t len = 0;
			sscanf(param2, "%u", &len);
		
			if (len) {
				//Хэш прошивки
				uint32_t hash1 = 0;
				sscanf(param3, "%04X", &hash1);
				uint16_t hash = (uint16_t)(hash1 & 0xFFFF);
				
				eeprom_write_uint16(EE_FLASH_HASH, hash);
				eeprom_write_uint32(EE_FLASH_SIZE, len);
				
				//Пароль 4 байта
				uint32_t pwd   = FLASH_PASSWD;
				eeprom_write_uint32(EE_FLASH_PWD, pwd);
		
				strcat(out, lbl_ok);
		
				//IS_RESET = 1;
			} else {
				strcat(out, lbl_wrong);
			}
			
		} else if ( !strcmp(param, lbl_clear)) {
			
			eeprom_write_uint32(EE_FLASH_SIZE, zval_32);
			eeprom_write_uint32(EE_FLASH_PWD, zval_32);
			eeprom_write_uint16(EE_FLASH_HASH, zval_16);
			
			strcat(out, lbl_ok);
		
		} else {
			strcat(out, lbl_wrong);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_elog(char* cmd, char* data)
{
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	if (!strlen(param)) { //param==""
		
		//Количество записей, счетчик переполнения журнала
		sprintf( mess, "%u|%u", gp_log_idx(), gp_log_cnt_get() );
		strcat(out, mess);
		

	} else if ( !strcmp(param, lbl_clear) ) {

		strcat(out, param);
		strcat(out, sep);
	
		gp_log_clear();
	
		strcat(out, lbl_ok);
		
	} else if ( !strcmp(param, "reidx") ) {

		strcat(out, param);
		strcat(out, sep);
	
		sprintf( mess, "%u|%u", gp_log_reidx(), gp_log_cnt_get() );
		strcat(out, mess);
	
	/*
	} else if ( !strcmp(param, "fill") ) {

		strcat(out, param);
		strcat(out, sep);
		
		gp_log_fill();
		
		strcat(out, lbl_ok);
	*/
	
	} else {
			
		uint32_t rec;
		sscanf(param, "%u", &rec);
			
		if (rec) {
				
			sprintf(mess, "%u", rec);
			strcat(out, mess);
			strcat(out, sep);
				
			tp_gp_log_type log_rec;
				
			if (gp_log_rec(rec, &log_rec)) {
					
				dateFormatS(log_rec.dt, mess);
				strcat(out, mess);
				strcat(out, sep);
					
				char  buf[11] = {0};
				strncpy(buf, log_rec.info, 10);
					
				sprintf(mess, "%u|%u", log_rec.tp, log_rec.num);
				strcat(out, mess);
					
				if (strlen(buf)) {
					strcat(out, sep);
					strcat(out, buf);
				}

			} else {
				strcat(out, lbl_fail);
			}
		} else {
			strcat(out, param);
			strcat(out, sep);
			strcat(out, lbl_wrong);
		}
		
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_server(char* cmd, char* data)
{
	char       server_ip[25];
	uint32_t   server_port;
	uint8_t    not_set = 0;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char port[16];
	
	char str[32];
	
	if (!strlen(param)) { //param==""
		
		if (eeprom_read_byte(EE_IP_1_set) == set_val) {
			eeprom_read_buffer(EE_IP_1_name, (void *)&server_ip, sizeof(server_ip));
			server_port = eeprom_read_uint16(EE_IP_1_port);
		} else {
			strcat(server_ip, MODEM_SERVER_NAME);
			server_port = MODEM_SERVER_PORT;
		}
		
		strcat(out, server_ip);
		strcat(out, sep);
		
		sprintf(str, "%u", server_port);
		strcat(out, str);
		
		if (eeprom_read_byte(EE_IP_2_set) == set_val) {
			eeprom_read_buffer(EE_IP_2_name, (void *)&server_ip, sizeof(server_ip));
			server_port = eeprom_read_uint16(EE_IP_2_port);
			
			strcat(out, sep);
			strcat(out, server_ip);
			strcat(out, sep);
			
			sprintf(str, "%u", server_port);
			strcat(out, str);
		}
	
	} else if ( !strcmp(param, lbl_set) ) {
	
		paramStringMax(data,3,server_ip,sizeof(server_ip));
		paramStringMax(data,4,port,sizeof(port));
		
		if (strlen(server_ip)>0 && strlen(port)>0) {
			sscanf(port, "%u", &server_port);
			
			eeprom_write_byte(EE_IP_1_set, set_val);
			eeprom_write_buffer(EE_IP_1_name, (void *)&server_ip, sizeof(server_ip));
			eeprom_write_uint16(EE_IP_1_port, server_port);
		} else {
			eeprom_write_byte(EE_IP_1_set, not_set);
		}
		
		paramStringMax(data,5,server_ip,sizeof(server_ip));
		paramStringMax(data,6,port,sizeof(port));
		
		if (strlen(server_ip)>0 && strlen(port)>0) {
			sscanf(port, "%u", &server_port);
			
			eeprom_write_byte(EE_IP_2_set, set_val);
			eeprom_write_buffer(EE_IP_2_name, (void *)&server_ip, sizeof(server_ip));
			eeprom_write_uint16(EE_IP_2_port, server_port);
		} else {
			eeprom_write_byte(EE_IP_2_set, not_set);
		}
		strcat(out, lbl_set);
		strcat(out, sep);
		strcat(out, lbl_ok);
	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_wdt(char* cmd, char* data) {
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[16];
	
	uint32_t wdt_max;
	
	if (!strlen(param)) { //param==""
		
		uint32_t max = (uint32_t)sys_wdt_max() * 60;
		
		sprintf(mess, "%u|%u", sys_wdt_cnt(), max);
		
		strcat(out, mess);
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		
		if ( !strcmp(param, lbl_set) ) {
		
			paramStringMax(data,3,sub,sizeof(sub));
			sscanf(sub, "%u", &wdt_max);
		
			wdt_max_write(wdt_max);
		
			sys_wdt_reset();
		
			strcat(out, lbl_ok);
		
		} else if ( !strcmp(param, "reset") ) {
	
			sys_wdt_reset();
	
			strcat(out, lbl_ok);
		
		} else {
			strcat(out, lbl_unknown);
		}
	}
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}



void command_addr(char* cmd, char* data)
{
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	if (!strlen(param)) { //param==""
		
		sprintf(out, "%u", RS485_ADDR);

	} else {
		strcat(out, param);
		strcat(out, sep);
		
		if ( !strcmp(param, lbl_set) ) {
		
			strcat(out, sub);
			strcat(out, sep);
			
			uint32_t addr = 0;
			sscanf(sub, "%u", &addr);
			if (addr) {
				RS485_ADDR = addr;
				eeprom_write_uint16(EE_RS485_ADDR, RS485_ADDR);
				
				strcat(out, lbl_ok);
			} else {
				strcat(out, lbl_wrong);
			}

		} else {
			strcat(out, lbl_unknown);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_com(char* cmd, char* data)
{
	/*
	uint32_t spd;
	uint16_t timeout;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[16] = "";
	char mode[8] = "";
	
	clearString(cmdData);
	
	if (!strlen(param)) { //param==""
		
		if (eeprom_read_byte(EE_COM_SET) == set_val) {
			eeprom_read_buffer(EE_COM_MODE, (void *)&mode, 3);
			spd = eeprom_read_uint32(EE_COM_SPEED);
		} else {
			strcpy(mode, RS485_MODE);
			spd = RS485_SPEED;
		}

	} else { //Установка
		
		//Speed
		paramStringMax(data,2,sub,sizeof(sub));
		sscanf(sub, "%u", &spd);
		if (spd > 0) {
			usart_set_baudrate(USART_SERIAL, spd, sysclk_get_per_hz());
		}
		
		//Timeout
		paramStringMax(data,4,sub,sizeof(sub));
		sscanf(sub, "%u", &timeout);
		if (timeout > 0) {
			serial_timeout_set(timeout);
			eeprom_write_uint16(EE_CMD_TIMEOUT, timeout);
		}
		
		//Mode
		paramStringMax(data,3,mode,sizeof(mode));
		
		eeprom_write_byte(EE_COM_SET, set_val);
		eeprom_write_uint32(EE_COM_SPEED, spd);
		eeprom_write_buffer(EE_COM_MODE, (void *)&mode, 3);

		serial_params_read();
		
		usart_format_set(
			USART_SERIAL,
			USART_SERIAL_OPTIONS.charlength,
			USART_SERIAL_OPTIONS.paritytype,
			USART_SERIAL_OPTIONS.stopbits
		);
	}
	timeout = serial_timeout_get();
	sprintf(out,"%u|%s|%u", spd, mode, timeout);
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
	*/
}

void command_rs(char* cmd, char* data)
{
	//char str[16] = "";
	
	//Убираем rs|, остальное данные
	shiftString(data, 3);
	
	serial_data_trim();
	
	if (serial_data_len()) {

		serial_rs485_prepare(data);
	} else {
		strcat(cmdData, cmd);
		strcat(cmdData, sep);
		strcat(cmdData, lbl_wrong);
	}
}

void command_ns(char* cmd, char* data)
{
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	if (!strlen(param)) { //param==""
		
		sprintf(out, "%02X", NS_BITS);
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		
		if ( !strcmp(param, lbl_set) ) {
		
			char param2[16];
			paramStringMax(data,3,param2,sizeof(param2));
		
			uint32_t ns = 0;
			sscanf(param2, "%02X", &ns);
			if (ns) {
				NS_BITS = ns;
				strcat(out, lbl_ok);
			} else {
				strcat(out, lbl_wrong);
			}

		} else if ( !strcmp(param, lbl_clear) ) {
		
			strcat(out, lbl_ok);
			
			NS_BITS = 0;
			
		} else {
			strcat(out, lbl_unknown);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_timer(char* cmd, char* data) {
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[16];
	paramStringMax(data,3,sub,sizeof(sub));
	
	char str[32];
	
	uint32_t n = 0;
	uint32_t t = 0;
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	
	if (strlen(param)>0) {
		
		sscanf(param, "%u", &n);
		strcat(cmdData, param);
		strcat(cmdData, sep);
		
		if (n < TIMERS_MAX) {
			
			if (!strlen(sub)) {
			
				strcat(cmdData, Timers_status(n) ? lbl_on : lbl_off);
				strcat(cmdData, sep);
			
				t = Timers_next(n);
				if (t) {
					dateFormat(t, str);
					sprintf(mess, "%s|%u", str, t - now());
				} else {
					strcpy(mess, lbl_none);
				}
				strcat(cmdData, mess);
			
			} else {
				strcat(cmdData, sub);
				strcat(cmdData, sep);
				
				if ( !strcmp(sub, lbl_on) ) {
					
					strcat(cmdData, lbl_ok);
				
					Timers_start(n);
				
				} else if ( !strcmp(sub, lbl_off) ) {
				
					strcat(cmdData, lbl_ok);
				
					Timers_stop(n);
				
				} else {
					strcat(cmdData, lbl_unknown);
				}
			}
		} else {
			strcat(cmdData, lbl_wrong);
		}
		
	} else {
		strcat(cmdData, param);
		strcat(cmdData, sep);
		strcat(cmdData, lbl_unknown);
	}
}

void command_set(char* cmd, char* data)
{
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	char sub2[32];
	paramStringMax(data,4,sub2,sizeof(sub2));
	
	//float     fval  = 0;
	uint16_t  ival  = 0;
	
	int32_t  step;
	uint32_t min;
	uint32_t max;
	uint8_t  is_set = 0;
	uint8_t  is_err = 0;
	
	if (!strlen(param)) { //param==""
	
		strcat(out, lbl_wrong);
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		
		
		if ( !strcmp(param, "board") ) {
			
			char       str_board[10];
		
			if (strlen(sub)) {
				strcat(out, sub);
				strcat(out, sep);
				
				if (strlen(sub) > 10) {
					strcat(out, lbl_wrong);
				} else {
					strcat(out, lbl_ok);
					strcpy(str_board, sub);
					eeprom_write_buffer(EE_BOARD, (void *)&str_board, sizeof(str_board));
				}
				
			} else {
				if (eeprom_read_byte(EE_BOARD) == zval_8) {
					strcat(out, lbl_none);
				} else {
					eeprom_read_buffer(EE_BOARD, (void *)&str_board, sizeof(str_board));
					strcat(out, str_board);
				}
			}
			
		}
		
		else if ( !strcmp(param, "step") ) {
	
			if (strlen(sub)>0) {
				
				//Установка
				sscanf(sub, "%u", &step);
				
				if (step<=5 && step>0) {
					steps_count_set(step);
					is_set = 1;
				} else {
					is_err = 1;
				}
		
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				step = steps_count_get();
				
				sprintf(mess,"%u", step);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		}
		
		else if ( !strcmp(param, "boost") ) {
			
			if (strlen(sub)>0) {
				
				//Установка
				if ( !strcmp(sub, lbl_on) || !strcmp(sub, "1") || !strcmp(sub, "true") ) {
					boost_set(1);
				} else {
					boost_set(0);
				}
			}
				
			strcat(out, boost_get() ? lbl_on : lbl_off);
				
			if (is_set) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
		}
		
		
		else if ( !strcmp(param, "auto") ) {
			
			if (strlen(sub)>0) {
				
				//Установка
				if ( !strcmp(sub, lbl_on) || !strcmp(sub, "1") || !strcmp(sub, "true") ) {
					auto_volt_set(1);
				} else {
					auto_volt_set(0);
				}
			}
			
			strcat(out, auto_volt_get() ? lbl_on : lbl_off);
			
			if (is_set) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
		}
		
		else if ( !strcmp(param, "volt") ) {
	
			if (strlen(sub)>0 && strlen(sub2)>0) {
		
				//Установка
				sscanf(sub, "%u", &min);
				sscanf(sub2, "%u", &max);
		
				if (max > min && max > 0 && min > 0) {
					volt_minmax_set(min, max);
					is_set = 1;
				} else {
					is_err = 1;
				}
		
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				min = volt_min_get();
				max = volt_max_get();
				
				sprintf(mess,"%u|%u", min, max);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		}
		
		else if ( !strcmp(param, "range") ) {
			
			if (strlen(sub)>0 && strlen(sub2)>0) {
				
				//Установка
				sscanf(sub, "%u", &min);
				sscanf(sub2, "%u", &max);
				
				if (max > min && max > 0 && min > 0) {
					range_minmax_set(min, max);
					is_set = 1;
				} else {
					is_err = 1;
				}
				
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				min = range_min_get();
				max = range_max_get();
				
				sprintf(mess,"%u|%u", min, max);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		}
		
		else if ( !strcmp(param, "time") ) {
			
			if (strlen(sub)>0) {
				
				//Установка
				sscanf(sub, "%u", &step);
				
				if (step<=180 && step>0) {
					des_time_set(step);
					is_set = 1;
				} else {
					is_err = 1;
				}
				
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				step = des_time_get();
				
				sprintf(mess,"%u", step);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		}
		
		else if ( !strcmp(param, "points") ) {
			
			if (strlen(sub)>0) {
				
				//Установка
				sscanf(sub, "%u", &step);
				
				if (step <= NET_BUF_LEN && step > 0) {
					points_set(step);
					is_set = 1;
				} else {
					is_err = 1;
				}
				
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				step = points_get();
				
				sprintf(mess,"%u", step);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		}
		
		else if ( !strcmp(param, "kt") ) {
			
			if (strlen(sub)>0) {
				
				//Установка
				sscanf(sub, "%u", &step);
				
				if (step <= 255 && step > 0) {
					kt_set(step);
					is_set = 1;
				} else {
					is_err = 1;
				}
				
			}
			if (is_err) {
				strcat(out, lbl_wrong);
			} else {
				step = kt_get();
				
				sprintf(mess,"%u", step);
				strcat(out, mess);
				
				if (is_set) {
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}

		} else {
			strcat(out, lbl_unknown);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_modem(char* cmd, char* data)
{
	uint32_t timeOut = 0;
	uint32_t num = 0;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	char param_2[16];
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	
	if (!strlen(param)) { //param==""
		
		char result[64];
		modem_tcp_status(result, sizeof(result));
		
		strcat(cmdData, result);
		
	} else {
		
		strcat(cmdData, param);
		strcat(cmdData, sep);
		
		if ( !strcmp(param, "task") ) {
		
			//Таск запуска модема
			tp_task_type *task_modem_start = Task_by_name("MStart");
		
			//Таск отправки данных
			tp_task_type *task_modem_send = Task_by_name("MSrvS");
		
			sprintf(mess, "%u|%u|%u|%u", Task_is_running(task_modem_start), task_modem_start->index, Task_is_running(task_modem_send), task_modem_send->index);
			strcat(cmdData, mess);
		
		} else if ( !strcmp(param, "status") ) {
		
			char result[64];
			modem_tcp_status(result, sizeof(result));
		
		
			strcat(cmdData, result);
	
		} else if ( !strcmp(param, "auto") ) {
		
			if (!strlen(sub)) {
				strcat(cmdData, MODEM_AUTO_CONNECT ? lbl_on : lbl_off);
			} else {
				if ( !strcmp(sub, lbl_on) ) {
					MODEM_AUTO_CONNECT = true;
				} else {
					MODEM_AUTO_CONNECT = false;
				}
				strcat(cmdData, lbl_ok);
			}
	
		} else if ( !strcmp(param, "at") ) {
		
			//Очищаем входной буфер
			modem_serial_clean();
		
			//<!>modem|at|AT+CSQ|2|2000
		
			//Num
			paramStringMax(data,4,param_2,sizeof(param_2));
		
			if (!strlen(param_2)) {
				strcat(cmdData, lbl_ok);
			
				//Отправляем произвольную AT команду
				modem_write(sub);
			} else {
				//Результат AT команды, номер строки
				sscanf(param_2, "%u", &num);
			
				//TimeOut
				paramStringMax(data,5,param_2,sizeof(param_2));
			
				if (strlen(param_2) > 0) {
					sscanf(param_2, "%u", &timeOut);
				} else {
					timeOut = 2000;
				}
			
				modem_at_num(sub, num, timeOut, mess, MESS_SIZE);
				strcat(cmdData, mess);
			}
		} else {
			strcat(cmdData, lbl_unknown);
		}
	}
}


void command_phone(char* cmd, char* data)
{
	//uint32_t time;
	//uint16_t ival;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[32];
	paramStringMax(data,3,sub,sizeof(sub));
	
	uint8_t  phone[PHONES_REC_SIZE];
	
	char str[32];
	
	if (!strlen(param)) { //param==""
		
		uint8_t count = 0;
		
		for (uint8_t n = 1; n <= PHONES_REC_MAX; n++) {
			eeprom_read_buffer(PHONES_ADDR_START + (n-1)*PHONES_REC_SIZE, (void *)&phone, PHONES_REC_SIZE);
			if (phone[0] != zval_8) {
				count++;
			}
		}
		sprintf(mess, "%u", count);
		strcat(out, mess);
	
	} else {
		strcat(out, param);
		strcat(out, sep);
		
		if (strlen(sub)) {
			strcat(out, sub);
			strcat(out, sep);
		}

		if ( !strcmp(param, "add") ) {
		
			if (strlen(sub)!=10) {
				strcat(out, lbl_wrong);
			} else {
		
				uint8_t pos = 0;
				bool is_found = false;
				//bool is_new = false;
				bool is_equal = false;
			
				uint8_t  phone_new[PHONES_REC_SIZE];
				for (uint8_t i = 0; i < strlen(sub); i+=2) {
					phone_new[i / 2] = (toHex(sub[i]) << 4) + toHex(sub[i+1]);
				}
		
				for (pos = 1; pos <= PHONES_REC_MAX; pos++) {
					eeprom_read_buffer(PHONES_ADDR_START + (pos-1)*PHONES_REC_SIZE, (void *)&phone, PHONES_REC_SIZE);
					if (phone[0] == zval_8) {
						is_found = true;
						break;
					} else {
						//Проверяем на совпадение
						is_equal = true;
						for (uint8_t n = 0; n < PHONES_REC_SIZE; n++) {
							if (phone[n] != phone_new[n]) {
								is_equal = false;
								break;
							}
						}
						if (is_equal) {
							break;
						}
					}
				}
				if (!is_found && !is_equal) {
					strcat(out, lbl_fail);
				} else {
					if (!is_equal) {
						eeprom_write_buffer(PHONES_ADDR_START + (pos-1)*PHONES_REC_SIZE, (void *)&phone_new, PHONES_REC_SIZE);
					}
				
					sprintf(mess, "%u|%s", pos, lbl_ok);
					strcat(out, mess);
				}
			}
			
		} else if ( !strcmp(param, "upd") ) {
			
			uint32_t pos;
			sscanf(sub, "%u", &pos);
			if (pos < 1 || pos > PHONES_REC_MAX) {
				strcat(out, lbl_wrong);
			} else {
				
				char param4[32];
				paramStringMax(data,4,param4,sizeof(param4));
				
				if (strlen(param4)!=10) {
					strcat(out, lbl_wrong);
				} else {
					uint8_t  phone_new[PHONES_REC_SIZE];
					for (uint8_t i = 0; i < strlen(param4); i+=2) {
						phone_new[i / 2] = (toHex(param4[i]) << 4) + toHex(param4[i+1]);
					}
					
					eeprom_write_buffer(PHONES_ADDR_START + (pos-1)*PHONES_REC_SIZE, (void *)&phone_new, PHONES_REC_SIZE);
					
					strcat(out, param4);
					strcat(out, sep);
					strcat(out, lbl_ok);
				}
			}
		
		} else if ( !strcmp(param, "rec") ) {
			
			uint32_t pos;
			sscanf(sub, "%u", &pos);
			if (pos < 1 || pos > PHONES_REC_MAX) {
				strcat(out, lbl_wrong);
			} else {
				phone_num(pos, str);
				if (strlen(str)) {
					strcat(out, str);
				} else {
					strcat(out, lbl_none);
				}
			}
		
		} else if ( !strcmp(param, "del") ) {
		
			uint32_t pos;
			sscanf(sub, "%u", &pos);
			if (pos < 1 || pos > PHONES_REC_MAX) {
				strcat(out, lbl_wrong);
			} else {
				for (uint8_t n = 0; n < PHONES_REC_SIZE; n++) {
					phone[n] = zval_8;
				}
				eeprom_write_buffer(PHONES_ADDR_START + (pos-1)*PHONES_REC_SIZE, (void *)&phone, PHONES_REC_SIZE);
				strcat(out, lbl_ok);
			}
		
		} else if ( !strcmp(param, "call") ) {
	
			uint32_t pos;
			sscanf(sub, "%u", &pos);
			if (pos < 1 || pos > PHONES_REC_MAX) {
				strcat(out, lbl_wrong);
			} else {
				phone_num(pos, str);
				if (strlen(str)) {
					
					tp_task_type *task_call = Task_by_name("MCALL");
					task_call->param[0] = pos;
					task_call->param[1] = 0;
					Task_start_tp(task_call);
					
					strcat(out, str);
					strcat(out, sep);
					strcat(out, lbl_ok);
					
				} else {
					strcat(out, lbl_none);
				}
			}

		} else {
			strcat(out, param);
			strcat(out, sep);
			strcat(out, lbl_unknown);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_gprs(char* cmd, char* data)
{
	char       gprs_apn[30];
	char       gprs_usr[20];
	char       gprs_pwd[20];
	
	uint8_t    not_set = zval_8;
	
	char param[16];
	paramStringMax(data,2,param,sizeof(param));
	
	if (!strlen(param)) { //param==""
		
		if (eeprom_read_byte(EE_gprs_set) == set_val) {
			eeprom_read_buffer(EE_gprs_apn, (void *)&gprs_apn, sizeof(gprs_apn));
			eeprom_read_buffer(EE_gprs_usr, (void *)&gprs_usr, sizeof(gprs_usr));
			eeprom_read_buffer(EE_gprs_pwd, (void *)&gprs_pwd, sizeof(gprs_pwd));
		} else {
			/*
			modem_flash(gprs_apn, 50); //internet
			modem_flash(gprs_usr, 51); //MODEM_APN_USER
			modem_flash(gprs_pwd, 52); //MODEM_APN_PASSWD
			*/
		}
		
		strcat(out, gprs_apn);
		strcat(out, sep);
		strcat(out, gprs_usr);
		strcat(out, sep);
		strcat(out, gprs_pwd);
		
	} else if ( !strcmp(param, lbl_set) ) {
		
		paramStringMax(data, 3, gprs_apn, sizeof(gprs_apn));
		paramStringMax(data, 4, gprs_usr, sizeof(gprs_usr));
		paramStringMax(data, 5, gprs_pwd, sizeof(gprs_pwd));
		
		if (strlen(gprs_apn)>0 && strlen(gprs_usr)>0 && strlen(gprs_pwd)>0) {
			
			eeprom_write_byte(EE_gprs_set, set_val);
			
			eeprom_write_buffer(EE_gprs_apn, (void *)&gprs_apn, sizeof(gprs_apn));
			eeprom_write_buffer(EE_gprs_usr, (void *)&gprs_usr, sizeof(gprs_usr));
			eeprom_write_buffer(EE_gprs_pwd, (void *)&gprs_pwd, sizeof(gprs_pwd));
		} else {
			eeprom_write_byte(EE_gprs_set, not_set);
		}
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_ok);
	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_sms(char* cmd, char* data)
{	
	//Телефон
	paramStringMax(data,2,modem_phone,sizeof(modem_phone));
	
	//Сообщение
	paramStringMax(data,3,mess,sizeof(mess));
	
	tp_task_type *task_sms_send = Task_by_name("MSMS");
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	
	if (strlen(modem_phone) > 0 && strlen(mess) > 0) {
		
		if (!Task_is_running(task_sms_send)) {
			
			Task_start_tp(task_sms_send);
			
			strcat(cmdData, lbl_start);
		} else {
			strcat(cmdData, lbl_fail);
		}
	} else {
		strcat(cmdData, lbl_wrong);
	}
}

void command_test(char* cmd, char* data) {
	
}

//-------------------------------------------------------------------------------------------------------------------------------

void command_port(char* cmd, char* data) {
	
	uint32_t p = 0;
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char val[16] = "";
	paramString(data,4,val);
	
	char out[64] = "";
	char str[16] = "";
	
	if (strlen(sub)) {
		sscanf(sub, "%u", &p);
		
		if (p > CONTACTOR_MAX) {p = CONTACTOR_MAX;}
	}
	
	if ( !strcmp(param, "A") || !strcmp(param, "a")) {
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		
		if (!strlen(val)) {
			if (PortA_get(p)) {
				strcat (out, "true");
			} else {
				strcat (out, "false");
			}
		} else {
		
			if ( !strcmp(val, "1") || !strcmp(val, "true")) {
				PortA_set(p);
				strcat (out, "true|ok");
			} else {
				PortA_clr(p);
				strcat (out, "false|ok");
			}
			Ports_renew();
		}
		
	} else if ( !strcmp(param, "B") || !strcmp(param, "b")) {
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		
		if (!strlen(val)) {
			if (PortB_get(p)) {
				strcat (out, "true");
			} else {
				strcat (out, "false");
			}
		} else {
			
			if ( !strcmp(val, "1") || !strcmp(val, "true")) {
				PortB_set(p);
				strcat (out, "true");
				strcat (out, sep);
				strcat (out, lbl_ok);
			} else {
				PortB_clr(p);
				strcat (out, "false");
				strcat (out, sep);
				strcat (out, lbl_ok);
			}
			Ports_renew();
		}
		
	} else if ( !strcmp(param, "C") || !strcmp(param, "c")) {
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		
		if (!strlen(val)) {
			if (PortC_get(p)) {
				strcat (out, "true");
			} else {
				strcat (out, "false");
			}
		} else {
			
			if ( !strcmp(val, "1") || !strcmp(val, "true")) {
				PortC_set(p);
				strcat (out, "true");
				strcat (out, sep);
				strcat (out, lbl_ok);
			} else {
				PortC_clr(p);
				strcat (out, "false");
				strcat (out, sep);
				strcat (out, lbl_ok);
			}
			Ports_renew();
		}
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}

	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_in(char* cmd, char* data) {
	
	uint32_t p = 0;
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char val[16] = "";
	paramString(data,4,val);
	
	char out[64] = "";
	char str[16] = "";
	
	if (strlen(sub)) {
		sscanf(sub, "%u", &p);
		
		if (p > CONTACTOR_MAX) {p = CONTACTOR_MAX;}
	}
	
	if ( !strcmp(param, "A") || !strcmp(param, "a")) {
			
		if (p) {
			PortA_in(p);
		} else {
			Ports_in_clr();
		}
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		strcat (out, lbl_ok);
		
	} else if ( !strcmp(param, "B") || !strcmp(param, "b")) {
			
		if (p) {
			PortB_in(p);
		} else {
			Ports_in_clr();
		}
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		strcat (out, lbl_ok);
		
	} else if ( !strcmp(param, "C") || !strcmp(param, "c")) {
		
		if (p) {
			PortC_in(p);
		} else {
			Ports_in_clr();
		}
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(str,"%u", p);
		strcat (out, str);
		strcat(out, sep);
		strcat (out, lbl_ok);
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}

	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_scan(char* cmd, char* data) {
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char out[64] = "";
	char str[64] = "";
	
	if ( !strcmp(param, "p1") ) {
	
		Ports_scan_mode(1);
	
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_ok);
		
	} else if ( !strcmp(param, "p2") ) {
	
		Ports_scan_mode(2);
	
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_ok);
		
	} else if ( !strcmp(param, "off") ) {

		Ports_scan_mode(0);
		
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_ok);
		
	} else if ( !strcmp(param, "in") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(mess, "%u|%u", Ports_scan_P1(), Ports_scan_P2());
		strcat(out, mess);
		
	} else if (!strlen(param) || !strcmp(param, "stat") ) {
	
		strcat(out, param);
		strcat(out, sep);
		
		uint16_t P1[3] = {0, 0, 0};
		uint16_t P2[3] = {0, 0, 0};
		
		for (uint8_t phase = 1; phase <= 3; phase++) {
			
			for (uint8_t p = 1; p <= 10; p++) {
				Port_phase_in(phase, p);
				
				//Включаем P1
				Ports_scan_mode(1);
				delay_millis(TASKS_WAIT_SCAN);
				
				if ( Ports_scan_P1() ) {
					P1[phase-1] |= 1 << (p-1);
				}
		
				//Включаем P2
				Ports_scan_mode(2);
				delay_millis(TASKS_WAIT_SCAN);
		
				if ( Ports_scan_P2() ) {
					P2[phase-1] |= 1 << (p-1);
				}
		
				//Отключаем
				Ports_scan_mode(0);
			}
		}
		Ports_in_clr();
		
		sprintf(mess, "PA1=%02X, PA2=%02X, PB1=%02X, PB2=%02X, PC1=%02X, PC2=%02X", P1[0], P2[0], P1[1], P2[1], P1[2], P2[2]);
	
		strcat(out, mess);

	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_blink(char* cmd, char* data)
{
	//uint16_t port;
	
	char param[16] = "";
	paramString(data,2,param);
	
	//char sub[16] = "";
	//paramString(data,3,sub);
	
	char out[64] = "";
	
	GPIO_TypeDef *Port;
	uint16_t Pin;
	bool found = false;
	bool auto_volt_phase = false;
	
	if (auto_volt_phase) {
		strcat(out, lbl_fail);
	} else {
	
		if (strlen(param)>0) {
			
			strcat(out, param);
			strcat(out, sep);

			if ( !strcmp(param, "ST") ) {
				Port = PIN_ST_PORT;
				Pin  = PIN_ST_PIN;
				found = true;
			}
		
			else if ( !strcmp(param, "SH") ) {
				Port = PIN_SH_PORT;
				Pin  = PIN_SH_PIN;
				found = true;
			}
		
			else if ( !strcmp(param, "DS") ) {
				Port = PIN_DS_PORT;
				Pin  = PIN_DS_PIN;
				found = true;
			}
		
			else if ( !strcmp(param, "ST2") ) {
				Port = PIN_ST2_PORT;
				Pin  = PIN_ST2_PIN;
				found = true;
			}
		
			else if ( !strcmp(param, "SH2") ) {
				Port = PIN_SH2_PORT;
				Pin  = PIN_SH2_PIN;
				found = true;
			}
		
			else if ( !strcmp(param, "DS2") ) {
				Port = PIN_DS2_PORT;
				Pin  = PIN_DS2_PIN;
				found = true;
		
			} else {
				
				strcat(out, lbl_wrong);
			}
			
			//sprintf(mess, "Port = %u, pin=%u", Port, Pin);
			//serial_write(mess);
		
			if (found) {
				
				for (uint8_t n = 0; n < 5; n++) {
					
					sprintf(mess,"try = %u", n+1);
					serial_write(mess);
					
					HAL_GPIO_WritePin(Port, Pin, 1);
					
					delay_millis(500);
					sys_wdt_reset();
					
					HAL_GPIO_WritePin(Port, Pin, 0);
					
					delay_millis(500);
					sys_wdt_reset();
				}
				strcat(out, lbl_ok);
			}
		
		} else {
			strcat(out, lbl_wrong);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_arc(char* cmd, char* data) {
	
	char param[16] = "";
	paramStringMax(data,2,param,sizeof(param));
	
	char p1[16] = "";
	paramStringMax(data,3,p1,sizeof(p1));
	
	char p2[16] = "";
	paramStringMax(data,4,p2,sizeof(p2));
	
	char out[256] = "";
	char str[64] = "";
	
	if ( !strcmp(param, "ms") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		uint32_t ver = W25Q64_JEDEC();
		uint8_t  uid[8];
		
		W25Q64_read_UID(uid);
		
		uint32_to_hex(ver, mess);
		strcat(out, mess);
		strcat(out, sep);
		
		bytes_dump_str(&uid, sizeof(uid), mess);
		strcat(out, mess);
	
	} else if ( !strcmp(param, "save") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		struct calendar_date sdate;
		uint32_t time = now();
		calendar_timestamp_to_date(time, &sdate);
		
		tp_arc_rec  arc_rec;
		
		arc_rec.year  = sdate.year;
		arc_rec.month = sdate.month + 1;
		arc_rec.day   = sdate.date  + 1;
		arc_rec.hour  = sdate.hour;
		arc_rec.min   = sdate.minute;
		arc_rec.res   = 0xFF;
		
		//A
		uint8_t n = 0;
		arc_rec.phase[n].Uavg = 220;
		arc_rec.phase[n].Umax = 222;
		arc_rec.phase[n].Umin = 218;
		
		arc_rec.phase[n].Iavg = 22.54;
		arc_rec.phase[n].Cavg = 0.97;
		
		arc_rec.phase[n].Step = 2;
		arc_rec.phase[n].NumSwitches = 5;
		arc_rec.phase[n].Errnum =  0;
		arc_rec.phase[n].Contnum =  0;
		
		//B
		n = 1;
		arc_rec.phase[n].Uavg = 230;
		arc_rec.phase[n].Umax = 232;
		arc_rec.phase[n].Umin = 228;
		
		arc_rec.phase[n].Iavg = 26.32;
		arc_rec.phase[n].Cavg = 0.85;
		
		arc_rec.phase[n].Step = 3;
		arc_rec.phase[n].NumSwitches = 2;
		arc_rec.phase[n].Errnum  =  1;
		arc_rec.phase[n].Contnum =  2;
		
		//C
		n = 2;
		arc_rec.phase[n].Uavg = 210;
		arc_rec.phase[n].Umax = 212;
		arc_rec.phase[n].Umin = 208;
		
		arc_rec.phase[n].Iavg = 16.32;
		arc_rec.phase[n].Cavg = 0.72;
		
		arc_rec.phase[n].Step = -1;
		arc_rec.phase[n].NumSwitches = 3;
		arc_rec.phase[n].Errnum  =  0;
		arc_rec.phase[n].Contnum =  0;
		
		if (arc_save(&arc_rec, true)) {
			strcat(out, lbl_ok);
		} else {
			strcat(out, lbl_fail);
		}
		
	} else if ( !strcmp(param, "erase") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		bool is_ok = false;
		
		if (strlen(p1) > 0) {
			if ( !strcmp(p1, FLASH_PASS)) {
				is_ok = true;
			}
		}
		if (is_ok) {
			sys_wdt_reset();
			
			arc_idx_clear();
			
			sys_wdt_reset();
			
			W25Q64_chip_erase();
			
			strcat(out, lbl_ok);
		} else {
			strcat(out, lbl_wrong);
		}
		/*
		strcat(out, param);
		strcat(out, sep);
		
		uint32_t addr = 0;
		
		if (strlen(p1) > 0) {
			sscanf(p1, "%u", &addr);
		}
		
		sprintf(mess, "%lu", addr);
		strcat(out, mess);
		strcat(out, sep);
		
		W25Q64_sector_erase(addr);
		*/
		
	} else if ( !strcmp(param, "read") ) {

		strcat(out, param);
		strcat(out, sep);
	
		uint32_t addr = 0;
	
		if (strlen(p1) > 0) {
			sscanf(p1, "%u", &addr);
		}
	
		sprintf(mess, "%u", addr);
		strcat(out, mess);
		strcat(out, sep);
		
		/*
		tp_arc_rec  arc_rec;
	
		W25Q64_read_data(addr, &arc_rec, ARCH_REC_LEN);
	
		bytes_dump_str(&arc_rec, ARCH_REC_LEN, mess);
		*/
		uint8_t data[ARCH_REC_LEN];
		
		W25Q64_read_data(addr, data, ARCH_REC_LEN);
	
		bytes_dump_str(data, ARCH_REC_LEN, mess);
		
		strcat(out, mess);
		
	} else if ( !strcmp(param, "idx") ) {

		strcat(out, param);
		strcat(out, sep);
		
		if (strlen(p1) > 0) {
			uint32_t rec = 0;
			sscanf(p1, "%u", &rec);
			
			if (rec > ARCH_IDX_REC_MAX) {rec = ARCH_IDX_REC_MAX;}
			if (rec < 1) {rec = 1;}
				
			tp_arc_idx rec_idx;
				
			arc_idx_get(rec, &rec_idx);
				
			sprintf(mess, "%u", rec);
			strcat(out, mess);
			strcat(out, sep);
			
			if (rec_idx.year == zval_16) {
				
				strcat(out, "empty");
			
			} else {
				char addr_str[16];
				uint32_to_hex(rec_idx.addr, addr_str);
				
				uint16_t cnt = arc_addr_count(&rec_idx);
				
				sprintf(mess, "%04u-%02u-%02u|%s|%u", rec_idx.year, rec_idx.month, rec_idx.day, addr_str, cnt);
				strcat(out, mess);
			}
				
		} else {
			uint16_t cnt = arc_idx_count();
			sprintf(mess, "%u", cnt);
			strcat(out, mess);
		}
		
	} else if ( !strcmp(param, "last") ) {

		strcat(out, param);
		strcat(out, sep);
		
		uint8_t rec = arc_idx_last();
		
		sprintf(mess, "%u", rec);
		strcat(out, mess);
	
		if (rec > 0) {
			
			strcat(out, sep);
		
			tp_arc_idx rec_idx;
		
			arc_idx_get(rec, &rec_idx);
		
			if (rec_idx.year == zval_16) {
				strcat(out, "empty");
			} else {
				char addr_str[16];
				uint32_to_hex(rec_idx.addr, addr_str);
			
				uint16_t cnt = arc_addr_count(&rec_idx);
			
				sprintf(mess, "%04u-%02u-%02u|%s|%u", rec_idx.year, rec_idx.month, rec_idx.day, addr_str, cnt);
				strcat(out, mess);
			}
		}
		
		
	} else if ( !strcmp(param, "dt") ) {

		strcat(out, param);
		strcat(out, sep);
	
		if (strlen(p1) > 0) {
			
			uint32_t year = 0;
			uint32_t month = 0;
			uint32_t day = 0;
			sscanf(p1, "%04u-%02u-%02u", &year, &month, &day);
			
			sprintf(mess, "%04u-%02u-%02u", year, month, day);
			strcat(out, mess);
			strcat(out, sep);
		
			tp_arc_idx rec_idx;
		
			if (arc_idx_find(year, month, day, &rec_idx)) {
				
				uint16_t cnt = arc_addr_count(&rec_idx);
				
				if (strlen(p2) > 0) {
					uint32_t rec = 0;
					sscanf(p2, "%u", &rec);
					
					sprintf(mess, "%u", rec);
					strcat(out, mess);
					strcat(out, sep);
					
					if (rec > 0 && rec <= cnt) {
						if (arc_rec_get(rec_idx.addr, rec, mess)) {
							strcat(out, mess);
						} else {
							strcat(out, lbl_fail);
						}
					} else {
						strcat(out, lbl_wrong);
					}
				} else {
				
					uint32_to_hex(rec_idx.addr, str);
					sprintf(mess, "%s|%u", str, cnt);
					strcat(out, mess);
				}
			} else {
				strcat(out, lbl_fail);
			}
		
		} else {
			strcat(out, lbl_unknown);
		}
	/*
	} else if ( !strcmp(param, "fill") ) {

		strcat(out, param);
		strcat(out, sep);
	
		if (strlen(p1) > 0) {
			
			char dtStr[16];
			StrFlash(dtStr, 51); //%04u-%02u-%02u
			
			uint16_t year = 0;
			uint16_t month = 0;
			uint16_t day = 0;
			sscanf(p1, dtStr, &year, &month, &day);
			
			sprintf(mess, dtStr, year, month, day);
			strcat(out, mess);
			strcat(out, sep);

			tp_arc_rec  arc_rec;
			uint32_t wdt = millis();
			uint32_t m = 0;
			uint16_t cnt = 0;
			
			for (uint8_t hour = 0; hour <= 23; hour++) {
				for (uint8_t minute = 0; minute <= 59; minute++) {
					arc_rec.year  = year;
					arc_rec.month = month;
					arc_rec.day   = day;
					arc_rec.hour  = hour;
					arc_rec.min   = minute;
					arc_rec.res   = 0xFF;

					for (uint8_t n = 0; n < PHASE_MAX; n++) {
	
						arc_rec.phase[n].Uavg = 220;
						arc_rec.phase[n].Umax = 222;
						arc_rec.phase[n].Umin = 218;
	
						arc_rec.phase[n].Iavg = 22.54;
						arc_rec.phase[n].Cavg = 0.97;
	
						arc_rec.phase[n].Step = 2;
						arc_rec.phase[n].NumSwitches = 5;
						arc_rec.phase[n].Errnum =  0;
					}

					if (arc_save(&arc_rec, true)) {
						cnt++;
					}
					
					
					m = millis();
					
					if (m - wdt > 100)  {
						watchdog_reset();
						wdt = m;
					}
				}
			}
			sprintf(mess,"%u",cnt);
			strcat(out, mess);
			strcat(out, sep);
			strcat(out, lbl_ok);
		
		} else {
			strcat(out, lbl_unknown);
		}
	*/	
		
	} else {
		strcat(out, param);
		strcat(out, sep);
		strcat(out, lbl_unknown);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_cal(char* cmd, char* data) {
	
	char out[64]   = "";
	char fstr[16] = "";
	char str[32] = "";
	
	char param[16] = "";
	paramStringMax(data, 2, param, sizeof(param));
	
	char sub[16] = "";
	paramStringMax(data, 3, sub, sizeof(sub));
	
	uint8_t periods = NET_PERIODS;
	
	if (strlen(param)) {
		
		strcat(out, param);
		strcat(out, sep);
		
		if ( !strcmp(param, "v") ) {
		
			ADE7758_rms_clear();
			
			for (uint8_t n = 0; n < periods; n++) {
				ADE7758_rms_get();
			}
			
			uint32_t volt[3];
			float    fvolt[3];
			
			for (uint8_t n = 1; n <= 3; n++) {
				volt[n-1] = ADE7758_rms_val(n);
			}
			for (uint8_t n = 0; n < 3; n++) {
				fvolt[n] = volt[n] / net_kt_volt[n];
			}
			
			for (uint8_t n = 0; n < 3; n++) {
				//dtostrf(fvolt[n], 1, 3, fstr);
				sprintf(fstr, "%0.3f", (double)fvolt[n]);
				strcat(out, fstr);
				
				if (n < 2) {
					strcat(out, ", ");
				}
			}
			
		} else if ( !strcmp(param, "vraw") || !strcmp(param, "vr") ) {
			
			ADE7758_rms_clear();
			
			for (uint8_t n = 0; n < periods; n++) {
				ADE7758_rms_get();
			}

			sprintf(mess, "%u|%u|%u", ADE7758_rms_val(1), ADE7758_rms_val(2), ADE7758_rms_val(3));
			strcat(out, mess);
			
		} else if ( !strcmp(param, "i") ) {
			
			ADE7758_rms_clear();
			
			for (uint8_t n = 0; n < periods; n++) {
				ADE7758_rms_get();
			}
			
			uint32_t curr[3];
			float    fcurr[3];
			
			char fstr[16] = "";

			for (uint8_t n = 4; n <= 6; n++) {
				curr[n-4] = ADE7758_rms_val(n);
			}
			for (uint8_t n = 0; n < 3; n++) {
				fcurr[n] = curr[n] / net_kt_curr[n];
			}
			
			for (uint8_t n = 0; n < 3; n++) {
				//dtostrf(fcurr[n], 1, 3, fstr);
				sprintf(fstr, "%0.3f", (double)fcurr[n]);
				strcat(out, fstr);
				
				if (n < 2) {
					strcat(out, ", ");
				}
			}
			
		} else if ( !strcmp(param, "iraw") || !strcmp(param, "ir") ) {
			
			ADE7758_rms_clear();
			
			for (uint8_t n = 0; n < periods; n++) {
				ADE7758_rms_get();
			}
			
			uint32_t curr[3];
			
			char fstr[16] = "";

			for (uint8_t n = 4; n <= 6; n++) {
				curr[n-4] = ADE7758_rms_val(n);
			}
			
			sprintf(mess, "%u|%u|%u", curr[0], curr[1], curr[2]);
			strcat(out, mess);
		

		} else if ( !strcmp(param, "stat") ) {
			uint32_t st =  ADE7758_getStatus();
			
			sprintf(mess, "%08X", st);
			strcat(out, mess);
			
		} else if ( !strcmp(param, "mask") ) {
			uint32_t msk =  ADE7758_getMaskInterrupts();
			
			sprintf(mess, "%08X", msk);
			strcat(out, mess);
			
		} else if ( !strcmp(param, "w") || !strcmp(param, "pwr") ) {
			
			if (strlen(sub)>0) {
				
				strcat(out, sub);
				strcat(out, sep);
				
				uint8_t p = 0;
			
				if ( !strcmp(sub, "a") || !strcmp(sub, "1") ) {
					
					p = 1;
					
				} else if ( !strcmp(sub, "b") || !strcmp(sub, "2") ) {
					
					p = 2;
					
				} else if ( !strcmp(sub, "c") || !strcmp(sub, "3") ) {
					
					p = 3;
					
				} else {
					strcat(out, lbl_wrong);
				}
				if (p) {
					
					if (ADE7758_setPotLine(p, 20)) {
						
						int16_t wa = ADE7758_getWatt(p);
						int16_t wr = ADE7758_getVar(p);
						int16_t ws = ADE7758_getVa(p);
						
						
						if (ws) {
							//dtostrf((float)wa / (float)ws, 1, 3, fstr);
							sprintf(fstr, "%0.3f", (double)wa / (double)ws);
						} else {
							strcat(fstr, "1");
						}
						
						sprintf(mess, "A=%d, R=%d, S=%d, cosF=%s", wa, wr, ws, fstr);
						strcat(out, mess);
						
					} else {
						strcat(out, lbl_fail);
					}
				}
			} else {
				strcat(out, lbl_wrong);
			}
		
		} else if ( !strcmp(param, "f") ) {
			
			float freq = ADE7758_getFreq();
			
			//dtostrf(freq, 1, 3, fstr);
			sprintf(fstr, "%0.3f", (double)freq);

			strcat(out, fstr);
			
		} else if ( !strcmp(param, "kt") ) {

			if (strlen(sub)>0) {
		
				uint8_t is_set = 0;
				float   fval   = 0;
		
				char sval[16] = "";
				char fstr[16] = "";
				paramString(data, 4, sval);
		
				if ( !strcmp(sub, "va") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_volt_set(1, fval);
						is_set = 1;
					} else {
						fval = net_kt_volt_get(1);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else if ( !strcmp(sub, "vb") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_volt_set(2, fval);
						is_set = 1;
					} else {
						fval = net_kt_volt_get(2);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else if ( !strcmp(sub, "vc") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_volt_set(3, fval);
						is_set = 1;
					} else {
						fval = net_kt_volt_get(3);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else if ( !strcmp(sub, "ia") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_curr_set(1, fval);
						is_set = 1;
					} else {
						fval = net_kt_curr_get(1);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else if ( !strcmp(sub, "ib") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_curr_set(2, fval);
						is_set = 1;
					} else {
						fval = net_kt_curr_get(2);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else if ( !strcmp(sub, "ic") ) {
			
					strcat(out, sub);
					strcat(out, sep);
			
					if (strlen(sval)>0) {
						fval = atof(sval);
				
						net_kt_curr_set(3, fval);
						is_set = 1;
					} else {
						fval = net_kt_curr_get(3);
					}
					//dtostrf(fval, 1, 4, fstr);
					sprintf(fstr, "%0.4f", (double)fval);
					strcat(out, fstr);
					if (is_set) {
						strcat(out, sep);
						strcat(out, lbl_ok);
					}
			
				} else {
					strcat(out, lbl_wrong);
				}
			} else {
				strcat(out, lbl_unknown);
			}
			
		} else if ( !strcmp(param, "ktv") ) {

			if (strlen(sub) > 0) {
			
				strcat(out, sub);
				strcat(out, sep);
			
				uint32_t p = 0;
				
				
				float   fval   = 0;
				float   vkt    = 0;
				char sval[16] = "";
				char fstr[16] = "";
				paramString(data, 4, sval);
				
				fval = atof(sval);
				if (fval > 0) {
					//dtostrf(fval, 1, 3, fstr);
					sprintf(fstr, "%0.3f", (double)fval);
					strcat(out, fstr);
					
					//Вычисляем к-т
					ADE7758_rms_clear();

					for (uint8_t n = 0; n < periods; n++) {
						ADE7758_rms_get();
					}
					
					if ( !strcmp(sub, "0") ) {
						for (p = 1; p <= 3; p++) {

							vkt = ADE7758_rms_val(p) / fval; //1-3

							//Сохраняем
							net_kt_volt_set(p, vkt);
						}
						strcat(out, sep);
						strcat(out, lbl_ok);
					} else {
						sscanf(sub, "%u", &p);
					

						if (p > 0 && p <= 3) {
		
							vkt = ADE7758_rms_val(p) / fval; //4-6
		
							//Сохраняем
							net_kt_volt_set(p, vkt);
		
							//Выводим
							strcat(out, sep);
		
							fval = net_kt_volt_get(p);
							//dtostrf(fval, 1, 4, fstr);
							sprintf(fstr, "%0.4f", (double)fval);
							strcat(out, fstr);
						}
					}
					
				} else {
					strcat(out, lbl_wrong);
				}
				
				
			} else {
				strcat(out, lbl_wrong);
			}
			
		} else if ( !strcmp(param, "kti") ) {

			if (strlen(sub) > 0) {
				
				strcat(out, sub);
				strcat(out, sep);
				
				uint32_t p = 0;
				sscanf(sub, "%u", &p);
				
				if (p > 0 && p <= 3) {
				
					float   fval   = 0;
					float   ikt    = 0;
					char sval[16] = "";
					char fstr[16] = "";
					paramString(data, 4, sval);
					
					fval = atof(sval);
					if (fval > 0) {
						//dtostrf(fval, 1, 3, fstr);
						sprintf(fstr, "%0.3f", (double)fval);
						strcat(out, fstr);
						
						//Вычисляем к-т
						//fval = fval / auto_kt;
						
						ADE7758_rms_clear();
			
						for (uint8_t n = 0; n < NET_PERIODS; n++) {
							ADE7758_rms_get();
						}
			
						ikt = ADE7758_rms_val(p+3) / fval; //4-6
						
						//Сохраняем
						net_kt_curr_set(p, ikt);
						
						//Выводим
						strcat(out, sep);
						
						fval = net_kt_curr_get(p);
						//dtostrf(fval, 1, 4, fstr);
						sprintf(fstr, "%0.4f", (double)fval);
						strcat(out, fstr);
						
					} else {
						strcat(out, lbl_wrong);
					}
					
				} else {
					strcat(out, lbl_wrong);
				}
			} else {
				strcat(out, lbl_wrong);
			}
			
		} else {
			strcat(out, lbl_unknown);
		}
	} else {
		strcat(out, lbl_unknown);
	}

	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_net(char* cmd, char* data)
{
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char out[64] = "";
	char str[32] = "";
	
	float avg = 0;
	char fstr[15] = "";
	
	uint16_t iavg = 0;
	uint32_t points = 0;
	
	tp_phase_type *phase;
	
	if ( !strcmp(param, "auto") ) {
		//Снятие сетевых параметров с измерителя
		
		strcat(out, param);
		strcat(out, sep);
		
		if (strlen(sub)>0) {

			if ( !strcmp(sub, lbl_on) ) {
				Regulator_dt_net();
				Timers_start(TIMER_NET);
			} else {
				Timers_stop(TIMER_NET);
			}
			strcat(out, sub);
			strcat(out, sep);
			strcat(out, lbl_ok);
			
		} else {
			strcat(out, Timers_status(TIMER_NET) ? lbl_on : lbl_off);
		}
		
	} else if ( !strcmp(param, "show") ) {
		
		/*
		strcat(out, param);
		strcat(out, sep);
		
		if (strlen(sub)>0) {

			if ( !strcmp(sub, "v") ) {
				net_show = 'v';
			} else if ( !strcmp(sub, "r") ) {
				net_show = 'r';
			} else if ( !strcmp(sub, "a") ) {
				net_show = 'a';
			} else if ( !strcmp(sub, "i") ) {
				net_show = 'i';
			} else if ( !strcmp(sub, "c") ) {
				 net_show = 'c';
			} else {
				net_show = 'n';
			}
			//strcat(out, sub);
			addChar(out, net_show);
			strcat(out, sep);
			strcat(out, lbl_ok);
			
		} else {
			addChar(out, net_show);
		}
		*/
		
	} else if ( !strcmp(param, "all") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		uint16_t rvolt = 0;
		uint16_t cvolt = 0;
		
		for (uint8_t p = 1; p <= PHASE_MAX; p++) {
			
			//Фаза
			sprintf(mess,"%u", p);
			strcat(out, mess);
			strcat(out, sep);
			
			phase = Phase_num(p);
			
			//Ступень
			sprintf(mess,"%d", phase->step);
			strcat(out, mess);
			strcat(out, sep);
			
			//Напряжение
			cvolt = volt_last(p);
			sprintf(mess,"%u", cvolt);
			strcat(out, mess);
			strcat(out, sep);
			
			//Входное напряжение
			rvolt = (uint16_t)((float)cvolt / steps_get(phase->step));
			sprintf(mess,"%u", rvolt);
			strcat(out, mess);
			strcat(out, sep);
			
			//Ток
			avg = curr_last(p) * auto_kt;
			//dtostrf(avg, 1, 1, fstr);
			sprintf(fstr, "%0.1f", (double)avg);
			strcat(out, fstr);
			strcat(out, sep);
			
			//К-т мощности
			avg = cos_last(p);
			//dtostrf(avg, 1, 2, fstr);
			sprintf(fstr, "%0.2f", (double)avg);
			strcat(out, fstr);
			
			if (p < PHASE_MAX) {
				strcat(out, sep);
			}
		}
		
	} else if ( !strcmp(param, "v") ) {
		
		strcat(out, param);
		strcat(out, sep);

		iavg = volt_last(1);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
	
		iavg = volt_last(2);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
	
		iavg = volt_last(3);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
	
	} else if ( !strcmp(param, "va") ) {
		
		strcat(out, param);
		strcat(out, sep);

		iavg = volt_avg(0);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
		
		iavg = volt_avg(1);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
		
		iavg = volt_avg(2);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		
	} else if ( !strcmp(param, "vr") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		if (strlen(sub)>0) {
			sscanf(sub, "%u", &points);
		}
		if (points<=0) {
			points = auto_points;
		}
		if (points > NET_BUF_LEN) {
			points = NET_BUF_LEN;
		}
		iavg = volt_avg_reg(0, points);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
	
		iavg = volt_avg_reg(1, points);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		strcat(out, sep);
	
		iavg = volt_avg_reg(2, points);
		sprintf(mess,"%u", iavg);
		strcat(out, mess);
		
	} else if ( !strcmp(param, "vin") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		uint16_t rvolt = 0;
		
		rvolt = (uint16_t)((float)volt_last(1) / steps_get(Phase_num(1)->step));
		
		sprintf(mess,"%u", rvolt);
		strcat(out, mess);
		strcat(out, sep);
		
		rvolt = (uint16_t)((float)volt_last(2) / steps_get(Phase_num(2)->step));
		
		sprintf(mess,"%u", rvolt);
		strcat(out, mess);
		
		strcat(out, sep);
		
		rvolt = (uint16_t)((float)volt_last(3) / steps_get(Phase_num(3)->step));
		
		sprintf(mess,"%u", rvolt);
		strcat(out, mess);
	
	} else if ( !strcmp(param, "curr") ) {
		
		strcat(out, param);
		strcat(out, sep);

		avg = curr_avg(1) * auto_kt;
		//dtostrf(avg, 1, 3, fstr);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = curr_avg(2) * auto_kt;
		//dtostrf(avg, 1, 3, fstr);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = curr_avg(3) * auto_kt;
		//dtostrf(avg, 1, 3, fstr);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "i") ) {
		
		strcat(out, param);
		strcat(out, sep);

		avg = curr_last(1) * auto_kt;
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
	
		avg = curr_last(2) * auto_kt;
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
	
		avg = curr_last(3) * auto_kt;
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "cos") ) {
		
		strcat(out, param);
		strcat(out, sep);

		avg = cos_avg(1);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = cos_avg(2);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = cos_avg(3);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "c") ) {
		
		strcat(out, param);
		strcat(out, sep);

		avg = cos_last(1);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = cos_last(2);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = cos_last(3);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "count") ) {
	
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(mess,"%u", Phase_num(1)->net_count);
		strcat(out, mess);
		strcat(out, sep);
		
		sprintf(mess,"%u", Phase_num(2)->net_count);
		strcat(out, mess);
		strcat(out, sep);

		sprintf(mess,"%u", Phase_num(3)->net_count);
		strcat(out, mess);
		
	} else if ( !strcmp(param, "acnt") ) {
	
		strcat(out, param);
		strcat(out, sep);
	
		sprintf(mess,"%u", net_info(1)->count);
		strcat(out, mess);
		strcat(out, sep);
	
		sprintf(mess,"%u", net_info(2)->count);
		strcat(out, mess);
		strcat(out, sep);

		sprintf(mess,"%u", net_info(3)->count);
		strcat(out, mess);
		
	} else if ( !strcmp(param, "max") ) {
	
		strcat(out, param);
		strcat(out, sep);
		
		avg = volt_max(1);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = volt_max(2);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = volt_max(3);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "min") ) {
	
		strcat(out, param);
		strcat(out, sep);
		
		avg = volt_min(1);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = volt_min(2);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		strcat(out, sep);
		
		avg = volt_min(3);
		sprintf(fstr, "%0.3f", (double)avg);
		strcat(out, fstr);
		
	} else if ( !strcmp(param, "reset") ) {
		
		strcat(out, param);
		strcat(out, sep);
		
		Task_start("NetReset");
		
		strcat(out, lbl_ok);
		
	} else if ( !strcmp(param, "ver") ) {
	
		strcat(out, param);
		strcat(out, sep);
		
		sprintf(mess, "%02X", ADE7758_getVersion());
		
		strcat(out, mess);
		
	} else {
		sprintf(out, "%u|%u|%u|%u|%u|%u", auto_steps_count, auto_boost, auto_volt_min, auto_volt_max, auto_time, auto_points);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_phase(char* cmd, char* data)
{		
	char param[16] = "";
	paramString(data,2,param);
	
	char out[128] = "";
	//char str[64] = "";
	
	if (strlen(param)>0) {
		
		tp_phase_type *phase;
		phase = Phase_get(param);

		sprintf(out, "%u|%d|%u|%u|%02X|%u|%u|%u|%u", phase->num, phase->step, phase->is_running, phase->error, phase->ports, phase->errcont, phase->dt_start, phase->dt_end, phase->dt_repair);
		
	} else {
		tp_phase_type *phaseA = Phase_num(1);
		tp_phase_type *phaseB = Phase_num(2);
		tp_phase_type *phaseC = Phase_num(3);
		
		sprintf(out, "%d|%d|%d|%u|%u|%u|%u|%u|%u", phaseA->step, phaseB->step, phaseC->step, phaseA->error, phaseB->error, phaseC->error, volt_last(1), volt_last(2), volt_last(3));
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_cont(char* cmd, char* data)
{
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	//char sub2[16] = "";
	//paramString(data,4,sub2);
	
	char out[128] = "";
	char str[32] = "";
	
	uint8_t err = 0;
	uint8_t pstate = 0;
		
	uint32_t port = 0;
	//uint16_t delay = 0;
	
	uint32_t t1 = 0;
	uint32_t t2 = 0;
	
	uint32_t micros = 0;
	
	uint32_t max_time = TASKS_MAX_SCAN_TIME;
	
	bool is_ok = false;
	bool p1 = false;
	bool p2 = false;
	
	tp_phase_type *phase;
	
	if (strlen(param)>0) {
		
		phase = Phase_get(param);
		
		sprintf(out, "%u", phase->num);
		strcat(out, sep);

		sscanf(sub, "%u", &port);
		if (port > 0 && port <= CONTACTOR_MAX) {
		
			//phase->num
			//Нормально замкнутый контакт отражается в P1, в PA2 - включенные контакторы (нормально разомкнутые контакты)
			
			//Установка входного порта
			Port_phase_in(phase->num, port);
		
			//Проверяем, замкнут ли контактор
			Ports_scan_mode(1);
			delay_millis(TASKS_WAIT_SCAN);
			
			p1 = Ports_scan_P1();

			Ports_scan_mode(2);
			delay_millis(TASKS_WAIT_SCAN);
			
			p2 = Ports_scan_P2();
			
			if (p2) {
				
				//Замкнут, отключаем---------------------------------------------------------------------------------------------------
				Phase_port_off(phase, port);
				
				pstate = 0;
				is_ok = false;
				
				counter_micros_start();
				do {
					//Сканируем
					p2 = Ports_scan_P2();
					
					if (!p2) {//Контакт разомкнулся, свободный полет
						is_ok = true;
					}
					micros = counter_micros_cur();
					
				} while (!is_ok && micros<=max_time);
				
				t1 = counter_micros_stop();
				
				if (is_ok) {
					//Ловим замыкание
					Ports_scan_mode(1);
	
					counter_micros_start();
	
					is_ok = false;
	
					do {
						//Сканируем
						p1 = Ports_scan_P1();
		
						if (p1) {//Контакт замкнулся
							is_ok = true;
						}
						micros = counter_micros_cur();
		
					} while (!is_ok && micros<=max_time);
	
					t2 = counter_micros_stop();
	
					if (!is_ok) {
						err = 2;
					}
				} else {
					err = 1;
				}
				
			} else if (p1) {
				
				//Разомкнут, включаем---------------------------------------------------------------------------------------------------
				Phase_port_on(phase, port);
				
				pstate = 1;
				Ports_scan_mode(1);
				
				counter_micros_start();
				is_ok = false;
				
				do {
					//Сканируем
					p1 = Ports_scan_P1();
								
					if (!p1) {//Контакт разомкнулся, свободный полет
						is_ok = true;
					}
					micros = counter_micros_cur();
				
				} while (!is_ok && micros<=max_time);
				
				t1 = counter_micros_stop();
				
				if (is_ok) {
					//Ловим замыкание
					Ports_scan_mode(2);
					
					counter_micros_start();
					is_ok = false;
				
					do {
						//Сканируем
						p2 = Ports_scan_P2();
					
						if (p2) {//Контакт замкнулся
							is_ok = true;
						}
						micros = counter_micros_cur();
					
					} while (!is_ok && micros<=max_time);
				
					t2 = counter_micros_stop();
					
					if (!is_ok) {
						err = 2;
					}
				} else {
					err = 1;
				}
				
			} else {
				//Нет замыкания по обеим линиям
				err = 3;
			}
			
			Ports_scan_mode(0);
			
			Ports_in_clr();
			
			sprintf(mess, "%u|%u|%u|%u", pstate, err, t1, t2);
			strcat(out, mess);
			
		} else {
			strcat(out, sub);
			strcat(out, sep);
			strcat(out, lbl_wrong);
		}
		
	} else {
		strcat(out, lbl_wrong);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_bind(char* cmd, char* data)
{
	uint8_t  phase = 0;
	uint32_t p = 0;
	uint32_t v = 0;
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char port[16] = "";
	paramString(data,4,port);
	
	char value[16] = "";
	paramString(data,5,value);
	
	char out[64] = "";
	char str[32] = "";
	
	char letters[3] = "abc";
	
	if (strlen(param)>0) {
		
		if (strlen(port) > 0) {
			sscanf(port, "%u", &p);
			if (p < 1) {p=1;}
			if (p > CONTACTOR_MAX) {p = CONTACTOR_MAX;}
			
			if (strlen(value) > 0) {
				sscanf(value, "%u", &v);
				if (v > CONTACTOR_MAX) {v = CONTACTOR_MAX;}
			}
		} else {
			p = 0;
		}
			
		if ( !strcmp(sub, "A") || !strcmp(sub, "a")) {
			phase = 0;
		}
		else if ( !strcmp(sub, "B") || !strcmp(sub, "b")) {
			phase = 1;
		} else {
			phase = 2;
		}

		if ( !strcmp(param, "out") ) {
			strcat(out, param);
			strcat(out, sep);
			
			addChar(out, letters[phase]);
			strcat(out, sep);
			
			if (p) {
				sprintf(str, "%u", p);
				strcat (out, str);
				strcat(out, sep);
				
				//sprintf(str, "phase=%u; port=%u; v=%u", phase, p, v);
				//serial_write(str);
			
				if (v) {
					//Установка
					Ports_bind_out_set(phase, p, v);
					sprintf(str, "%u", v);
					strcat(out, str);
					strcat(out, sep);
					strcat(out, lbl_ok);
				} else {
					//Чтение
					sprintf(str, "%u", Ports_bind_out_get(phase, p));
					strcat(out, str);
				}
			} else {
				//выдаем все связки
				for (uint8_t n = 1; n <= CONTACTOR_MAX; n++) {
					sprintf(str, "%u;", Ports_bind_out_get(phase, n));
					strcat(out, str);
				}
			}
		}
		else if ( !strcmp(param, "in") ) {
			strcat(out, param);
			strcat(out, sep);
			
			addChar(out, letters[phase]);
			strcat(out, sep);
			
			if (p) {
				sprintf(str, "%u", p);
				strcat (out, str);
				strcat(out, sep);
			
				if (v) {
					//Установка
					Ports_bind_in_set(phase, p, v);
					sprintf(str, "%u", v);
					strcat(out, str);
					strcat(out, sep);
					strcat(out, lbl_ok);
				} else {
					//Чтение
					sprintf(str, "%u", Ports_bind_in_get(phase, p));
					strcat(out, str);
				}
			} else {
				//выдаем все связки
				for (uint8_t n = 1; n <= CONTACTOR_MAX; n++) {
					sprintf(str, "%u;", Ports_bind_in_get(phase, n));
					strcat(out, str);
				}
			}
		} else {
			strcat(out, lbl_wrong);
		}
		
	} else {
		strcat(out, lbl_wrong);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_run(char* cmd, char* data)
{
	int32_t  step = 0;
	uint8_t  is_reverse = 0;
	
	tp_task_type *task;	
	char task_name[20] = "";
	
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char out[128] = "";
	char str[64] = "";
	
	tp_phase_type *phase;
	
	uint8_t  steps_count = steps_count_get();
	
	if (strlen(param)>0) {
		
		sscanf(sub, "%d", &step);
		if (step < 0) {
			step = -step;
			is_reverse = 1;
		}
		if (step > steps_count) {
			step = steps_count;
		}
		
		if ( !strcmp(param, "all") ) {
			for (uint8_t n = 0; n < PHASE_MAX; n++) {
				
				phase = Phase_num(n+1);
				
				if (!phase->is_running) {
					sprintf(task_name,"%c%u", phase->let, step);
					
					//serial_write(task_name);

					if (strlen(task_name)>0) {
						
						task = Task_by_name(task_name);
	
						if (task != NULL) {
		
							phase->next_step = is_reverse ? -step : step;

							Task_start_tp(task);
						}
					}
				}
			}
			strcat(out, param);
			strcat(out, sep);
			strcat(out, sub);
			strcat(out, sep);
			strcat(out, lbl_ok);
		} else {
		
			phase = Phase_get(param);
		
			if (phase->is_running) {
				strcat(out, "already running");
			} else {
			
				sprintf(task_name,"%c%u", phase->let, step);

				if (strlen(task_name)>0) {
					task = Task_by_name(task_name);
	
					if (task == NULL) {
						sprintf(out, "task not found %s", task_name);
					} else {
						//Хотим перейти в это состояние
						//serial_write(task_name);
					
						phase->next_step = is_reverse ? -step : step;
		
						Task_start_tp(task);
						
						sprintf(out, "started %s (%u)", task_name, task->n);
					}
				}
			}
		}
		
	} else {
		strcat(out, lbl_wrong);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_step(char* cmd, char* data)
{
	int32_t step;
	float   volt;
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char fstr[16] = "";
	char out[128] = "";
	
	if (strlen(param)>0) {

		sscanf(param, "%d", &step);
		
		strcat(out, param);
		strcat(out, sep);
		
		if (step<=5 && step>=-5) {
			
			if (strlen(sub)>0) {
				//Установка
				
				volt = atof(sub);
				
				steps_set(step, volt);
			}
			volt = steps_get(step);
			
			//dtostrf(volt, 1, 5, fstr);
			sprintf(fstr, "%0.5f", (double)volt);
			strcat(out, fstr);
			
			if (strlen(sub)>0) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
			
		} else {
			strcat(out, lbl_wrong);
		}
		
	} else {
		step = steps_count_get();
		sprintf(mess,"%u", step);
		strcat(out, mess);
		
		for (uint8_t n = 1; n <= 5; n++) {
			strcat(out, sep);
			sprintf(fstr, "%0.5f", (double)steps_get(n));
			//dtostrf(steps_get(n), 1, 5, fstr);
			strcat(out, fstr);
		}
		for (uint8_t n = 1; n <= 5; n++) {
			strcat(out, sep);
			//dtostrf(steps_get(-n), 1, 5, fstr);
			sprintf(fstr, "%0.5f", (double)steps_get(-n));
			strcat(out, fstr);
		}
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}


void command_debug(char* cmd, char* data)
{
	uint8_t  is_set = 0;
	uint8_t  val = 0;
	
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char out[64] = "";
	
	if (strlen(param)>0) {
		
		if ( !strcmp(param, "mess") ) {
			
			strcat(out, param);
			strcat(out, sep);
			
			if (strlen(sub)>0) {
				
				//Установка
				val = !strcmp(sub, lbl_on) ? 1:0;
				debug_mess_set(val);
				is_set = 1;
			}
			
			strcat(out, debug_mess_get() ? lbl_on : lbl_off);
			
			if (is_set) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
		}
		
		else if ( !strcmp(param, "ports") ) {
			
			strcat(out, param);
			strcat(out, sep);
			
			if (strlen(sub)>0) {
				
				//Установка
				val = !strcmp(sub, lbl_on) ? 1:0;
				debug_ports_set(val);
				is_set = 1;
			}
			
			strcat(out, debug_ports_get() ? lbl_on : lbl_off);
			
			if (is_set) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
		}
		
		else if ( !strcmp(param, "error") ) {
			
			strcat(out, param);
			strcat(out, sep);
			
			if (strlen(sub)>0) {
				
				//Установка
				val = !strcmp(sub, lbl_on) ? 1:0;
				debug_error_set(val);
				is_set = 1;
			}
			
			strcat(out, debug_error_get() ? lbl_on : lbl_off);
			
			if (is_set) {
				strcat(out, sep);
				strcat(out, lbl_ok);
			}
		}
		
		else {
			strcat(out, lbl_wrong);
		}
		
	} else {
		strcat(out, lbl_wrong);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_contlog(char* cmd, char* data) {
	
	char param[16] = "";
	paramStringMax(data,2,param,sizeof(param));
	
	char sub[16] = "";
	paramStringMax(data,3,sub,sizeof(sub));
	
	char val[16] = "";
	paramStringMax(data,4,val,sizeof(val));
	
	char out[64] = "";
	char str[16] = "";
	
	tp_phase_type *phase;
	
	if (strlen(param)>0) {
		
		phase = Phase_get(param);
		
		sprintf(out,"%u", phase->num);
		strcat(out, sep);
		
		if (strlen(sub)>0) {
			uint32_t port = 0;
			sscanf(sub, "%u", &port);
			if (port > CONTACTOR_MAX) {port = CONTACTOR_MAX;}
			if (port < 1) {port = 1;}
				
			sprintf(str,"%u", port);
			strcat(out, str);
			strcat(out, sep);
			
			if (strlen(val)>0) {
				//Задание логики
				uint8_t logic = 0;
				if ( !strcmp(val, "1") ) {
					logic = 1;
				}
				Ports_logic_set(phase->num, port,  logic);
				
				sprintf(str, "%u", logic);
				strcat(out, str);
				strcat(out, sep);
				
				strcat(out, lbl_ok);
			} else {
				//Чтение логики
				uint8_t logic = Ports_logic_get(phase->num, port);
				sprintf(str, "%u", logic);
				strcat(out, str);
			}
		} else {
			sprintf(str, "%02X", Ports_logic_port(phase->num));
			strcat(out, str);
		}
		
	} else {
		strcat(out, lbl_wrong);
	}

	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}

void command_error(char* cmd, char* data)
{		
	char param[16] = "";
	paramString(data,2,param);
	
	char sub[16] = "";
	paramString(data,3,sub);
	
	char sub2[16] = "";
	paramString(data,4,sub2);
	
	char out[128] = "";
	char str[32] = "";
	
	tp_phase_type *phase;
	
	if (strlen(param)>0) {
		
		phase = Phase_get(param);
		
		if (strlen(sub)>0) {

			if ( !strcmp(sub, lbl_on) || !strcmp(sub, "1") ) {
				phase->error = 1;
				if (strlen(sub2) > 0) {
					uint32_t err;
					sscanf(sub2, "%u", &err);
					phase->errcont = err;
				}
			} else {
				phase->error   = 0;
				phase->errnum  = 0;
				phase->errcont = 0;
			}
			strcat(out, sub);
			strcat(out, sep);
			strcat(out, lbl_ok);
			
		} else {
			char dt_repair[32] = "";
			char dt_error[32] = "";
			
			if (phase->dt_repair) {
				dateFormat(phase->dt_repair, dt_repair);
			} else {
				strcat(dt_repair, lbl_none);
			}
			
			if (phase->dt_error) {
				dateFormat(phase->dt_error, dt_error);
			} else {
				strcat(dt_error, lbl_none);
			}
			
			sprintf(out, "%u|%u|%u|%u|%s|%s", phase->num, phase->error, phase->errcont, phase->errnum, dt_error, dt_repair);
		}
		
	} else {
		strcat(out, lbl_wrong);
	}
	
	strcat(cmdData, cmd);
	strcat(cmdData, sep);
	strcat(cmdData, out);
}