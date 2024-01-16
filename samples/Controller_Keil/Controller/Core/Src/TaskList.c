/*
 * TaskList.c
 *
 * Created: 30.01.2018 9:32:52
 *  Author: Admin
 */ 
#include "TaskList.h"

uint32_t t0 = 0;
uint32_t t1 = 0;
uint32_t t2 = 0;

char     net_show = NET_SHOW_DEF;

tp_task_type Tasks[] = {

	#ifdef MODEM_PRESENT
	{
		.name = "MStart",
		.cmds = {
			Modem_start_power,
			Modem_start_sms,
			Modem_start_gprs,
			Modem_start_connect
		}
	},
	{
		.name = "MPower",
		.cmds = {
			M_sub_power,
			M_sub_reset,
			M_sub_at,
			M_sub_echo,
			M_sub_creg,
			M_sub_imei,
			M_sub_csq,
			M_sub_check
		}
	},
	{
		.name = "MSMSS",
		.cmds = {
			M_sub_sms,
			M_sub_sms_scr,
			M_sub_sms_code,
			M_sub_sms_del,
		}
	},
	{
		.name = "MGPRS",
		.cmds = {
			M_call_check,
			M_sub_gprs_exist,
			M_sub_gprs_cont,
			M_sub_gprs_connect,
			M_sub_gprs_IP
			//M_sub_gprs_loc
		}
	},
	{
		.name = "MSRV",
		.cmds = {
			M_call_check,
			M_sub_srv_set_1,
			M_sub_tcp_start,
			M_sub_srv_set_2,
			M_sub_tcp_start,
		}
	},
	{
		.name = "MBlink",
		.cmds = {
			M_blink
		}
	},
	{
		.name = "MSrvS",
		.cmds = {
			M_call_check,
			M_srv_start,
			M_srv_send,
			M_srv_copy,
		}
	},
	{
		.name = "MSMS",
		.cmds = {
			M_sms_start,
			M_sms_send
		}
	},
	{
		.name = "MCALL",
		.cmds = {
			M_call_init,
			M_call_start,
			M_call_end
		}
	},
	{
		.name = "PBlink",
		.cmds = {
			P_blink
		}
	},
	#endif
	//--------------------------------------------------------------------------------------------------------------------------------------------------
	{
		.name = "A0",
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM4_off,
			A_KM5_off,
			A_KM6_off,
			A_KM7_off,
			A_KM8_off,
			A_KM9_off,
			A_finish
		}
	},
	{
		.name = "A1",
		// Выключение КМ4…КМ9
		// Проверка выключения КМ4...КМ9
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM4_off,
			A_KM5_off,
			A_KM6_off,
			A_KM7_off,
			A_KM8_off,
			A_KM9_check,
			A_TransOff,
			A_finish
		}
	},
	{
		.name = "A2",
		// Включение КМ7,КМ8
		// Выключение КМ4…КМ6,КМ9
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM7_on,
			A_KM8_on,
			A_KM4_off,
			A_KM5_off,
			A_KM6_off,
			A_KM9_check,
			A_TransOff,
			A_finish
		}
	},
	{
		.name = "A3",
		// Включение КМ6,КМ7,КМ8
		// Выключение КМ4,КМ5,КМ9
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM6_on,
			A_KM7_on,
			A_KM8_on,
			A_KM4_off,
			A_KM5_off,
			A_KM9_check,
			
			A_TransOff,
			A_finish
		}
	},
	{
		.name = "A4",
		// Включение КМ5,КМ6,КМ7,КМ8
		// Выключение КМ4,КМ9
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM5_on,
			A_KM6_on,
			A_KM7_on,
			A_KM8_on,
			
			A_KM4_off,
			A_KM9_check,
			
			A_TransOff,
			A_finish
		}
	},
	{
		.name = "A5",
		// ВключениеКМ4...КМ8
		// Выключение КМ9
		.cmds = {
			A_begin,
			A_TransOn,
			A_KM4_on,
			A_KM5_on,
			A_KM6_on,
			A_KM7_on,
			A_KM8_on,
			
			A_KM9_check,
			
			A_TransOff,
			A_finish
		}
	},
	{
		.name = "AT_On",
		.cmds = {
			A_KM2_on,
			/*
			A_KM3_on,
			A_KM1_off,
			A_KM2_off,
			*/
			A_KM_Trans_on,
			A_delay_5sec,
			A_KM3_off
		}
	},
	{
		.name = "AT_Off",
		.cmds = {
			A_KM2_on,
			/*
			A_KM3_on,
			A_KM1_on,
			A_KM2_off,
			*/
			A_KM_Trans_off,
			A_delay_5sec,
			A_KM3_off
		}
	},
	{
		.name = "A_FTrans",
		.cmds = {
			A_FTrans
		}
	},
	{
		.name = "B0",
		.cmds = {
			B_begin,
			B_TransOn,
			
			B_KM4_off,
			B_KM5_off,
			B_KM6_off,
			B_KM7_off,
			B_KM8_off,
			B_KM9_off,
			
			B_finish
		}
	},
	{
		.name = "B1",
		// Выключение КМ4…КМ9
		// Проверка выключения КМ4...КМ9
		.cmds = {
			B_begin,
			B_TransOn,
			
			B_KM4_off,
			B_KM5_off,
			B_KM6_off,
			B_KM7_off,
			B_KM8_off,
			B_KM9_check,
			
			B_TransOff,
			B_finish
		}
	},
	{
		.name = "B2",
		// Включение КМ7,КМ8
		// Выключение КМ4…КМ6,КМ9
		.cmds = {
			B_begin,
			B_TransOn,
			B_KM7_on,
			B_KM8_on,
			B_KM4_off,
			B_KM5_off,
			B_KM6_off,
			B_KM9_check,
			B_TransOff,
			B_finish
		}
	},
	{
		.name = "B3",
		// Включение КМ6,КМ7,КМ8
		// Выключение КМ4,КМ5,КМ9
		.cmds = {
			B_begin,
			B_TransOn,
			B_KM6_on,
			B_KM7_on,
			B_KM8_on,
			B_KM4_off,
			B_KM5_off,
			B_KM9_check,
			
			B_TransOff,
			B_finish
		}
	},
	{
		.name = "B4",
		// Включение КМ5,КМ6,КМ7,КМ8
		// Выключение КМ4,КМ9
		.cmds = {
			B_begin,
			B_TransOn,
			B_KM5_on,
			B_KM6_on,
			B_KM7_on,
			B_KM8_on,
			
			B_KM4_off,
			B_KM9_check,
			
			B_TransOff,
			B_finish
		}
	},
	{
		.name = "B5",
		// ВключениеКМ4...КМ8
		// Выключение КМ9
		.cmds = {
			B_begin,
			B_TransOn,
			B_KM4_on,
			B_KM5_on,
			B_KM6_on,
			B_KM7_on,
			B_KM8_on,
			
			B_KM9_check,
			
			B_TransOff,
			B_finish
		}
	},
	{
		.name = "BT_On",
		.cmds = {
			B_KM2_on,
			/*
			B_KM3_on,
			B_KM1_off,
			B_KM2_off,
			*/
			B_KM_Trans_on,
			B_delay_5sec,
			B_KM3_off
		}
	},
	{
		.name = "BT_Off",
		.cmds = {
			B_KM2_on,
			/*
			B_KM3_on,
			B_KM1_on,
			B_KM2_off,
			*/
			B_KM_Trans_off,
			B_delay_5sec,
			B_KM3_off
		}
	},
	{
		.name = "B_FTrans",
		.cmds = {
			B_FTrans
		}
	},
	{
		.name = "C0",
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM4_off,
			C_KM5_off,
			C_KM6_off,
			C_KM7_off,
			C_KM8_off,
			C_KM9_off,
			C_finish
		}
	},
	{
		.name = "C1",
		// Выключение КМ4…КМ9
		// Проверка выключения КМ4...КМ9
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM4_off,
			C_KM5_off,
			C_KM6_off,
			C_KM7_off,
			C_KM8_off,
			C_KM9_check,
			C_TransOff,
			C_finish
		}
	},
	{
		.name = "C2",
		// Включение КМ7,КМ8
		// Выключение КМ4…КМ6,КМ9
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM7_on,
			C_KM8_on,
			C_KM4_off,
			C_KM5_off,
			C_KM6_off,
			C_KM9_check,
			C_TransOff,
			C_finish
		}
	},
	{
		.name = "C3",
		// Включение КМ6,КМ7,КМ8
		// Выключение КМ4,КМ5,КМ9
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM6_on,
			C_KM7_on,
			C_KM8_on,
			C_KM4_off,
			C_KM5_off,
			C_KM9_check,
			
			C_TransOff,
			C_finish
		}
	},
	{
		.name = "C4",
		// Включение КМ5,КМ6,КМ7,КМ8
		// Выключение КМ4,КМ9
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM5_on,
			C_KM6_on,
			C_KM7_on,
			C_KM8_on,
			
			C_KM4_off,
			C_KM9_check,
			
			C_TransOff,
			C_finish
		}
	},
	{
		.name = "C5",
		// ВключениеКМ4...КМ8
		// Выключение КМ9
		.cmds = {
			C_begin,
			C_TransOn,
			C_KM4_on,
			C_KM5_on,
			C_KM6_on,
			C_KM7_on,
			C_KM8_on,
			
			C_KM9_check,
			
			C_TransOff,
			C_finish
		}
	},
	{
		.name = "CT_On",
		.cmds = {
			C_KM2_on,
			/*
			C_KM3_on,
			C_KM1_off,
			C_KM2_off,
			*/
			C_KM_Trans_on,
			C_delay_5sec,
			C_KM3_off
		}
	},
	{
		.name = "CT_Off",
		.cmds = {
			C_KM2_on,
			/*
			C_KM3_on,
			C_KM1_on,
			C_KM2_off,
			*/
			C_KM_Trans_off,
			C_delay_5sec,
			C_KM3_off
		}
	},
	{
		.name = "C_FTrans",
		.cmds = {
			C_FTrans
		}
	},
	{
		.name = "Net",
		.cmds = {
			Net_get,
			Net_save
		}
	},
	{
		.name = "DStart",
		.cmds = {
			Delay_start
		}
	},
	{
		.name = "ARep",
		.cmds = {
			A_rep_start,
			A_rep_end,
		}
	},
	{
		.name = "BRep",
		.cmds = {
			B_rep_start,
			B_rep_end,
		}
	},
	{
		.name = "CRep",
		.cmds = {
			C_rep_start,
			C_rep_end,
		}
	},
	{
		.name = "AdeRms",
		.cmds = {
			AdeRms
		}
	},
	{
		.name = "AdePower",
		.cmds = {
			AdePower
		}
	},
	{
		.name = "NetReset",
		.cmds = {
			NetReset
		}
	}
};

uint8_t  tasks_cnt = sizeof(Tasks)/sizeof(Tasks[0]);

char       server_ip[25];
uint16_t   server_port;

//Actions---------------------------------------------------------------------------------------------------------------------------------------

bool M_AT_send(tp_task_type *task, uint8_t n) {
	
	char cmd[32] = "";
	
	modem_flash(cmd, n);
	modem_write_prepare();
	modem_write_tx(cmd);
	
	//serial_write("M_AT_send");
	//serial_write(cmd);
	
	task->wait = 100; //100ms
	
	return false;
}
bool M_AT_command(tp_task_type *task, uint16_t num, char* response, uint16_t max, uint16_t timeOut, uint8_t retry) {
	
	bool ret = false;
	
	if (task->iter <= timeOut) {
		
		ret = modem_data_num(num, response, max);
		
		if (!ret) {
			//Ожидание ответа на команду
			task->wait = 10; //10ms
		} else {
			ret = true;
			clearString(modem_data);
			modem_serial_task = 0;
		}
		
	} else {
		clearString(modem_data);
		modem_serial_task = 0;
		
		if (retry) {
			//Повтор
			ret = Task_try(task, retry);
		} else {
			ret = true;
		}
	}
	return ret;
}

//Actions---------------------------------------------------------------------------------------------------------------------------------------
bool Modem_start_power(tp_task_type *task) {
	
	bool ret = false;
	
	tp_task_type *sub_power = Task_by_name("MPower");
	tp_task_type *sub_blink = Task_by_name("MBlink");
	
	if (task->iter==0) {
		
		if (modem_sim_detect()) {
			
			modem_status_n(31); //reseting
			MODEM_ALIVE = false;
			
			#ifdef  SIM_DETECT_DUAl
				uint8_t sim = modem_sim_cur();
				sprintf(mess, "SIM=%u", sim+1);
				serial_write(mess);
			#endif
			
			Task_start_sub_tp(sub_power, task);
		} else {
			//Отключаем модем
			modem_power_off();
			
			Task_start("PBlink");
			
			Task_stop(task);
			ret = true;
		}
	}
	else if (task->iter==1) {
		//return of sub_power
		
		if (!Task_is_error(sub_power)) {
			MODEM_ALIVE = true;
			//MODEM_CONNECTED = true;
			modem_status_n(32);//started
			ret = true;
		} else {
			
			serial_write("modem_error");
			
			if (sub_power->index == 1) {
				modem_status_n(33);//power_off
				
				sub_blink->param[0] = 100; //Quant (2 sec)
				sub_blink->param[1] = 20;  //Period, ms
				Task_start_sub_tp(sub_blink, task);
			}
			if (sub_power->index == 2) {
				modem_status_n(34);//no_at
				
				sub_blink->param[0] = 40; //Quant (2 sec)
				sub_blink->param[1] = 50;  //Period, ms
				Task_start_sub_tp(sub_blink, task);
			}
			if (sub_power->index == 3 || sub_power->index == 5 || sub_power->index == 6) {
				
				modem_status_n(35);//no_cmd_reply
				
				sub_blink->param[0] = 30; //Quant (2 sec)
				sub_blink->param[1] = 75;  //Period, ms
				Task_start_sub_tp(sub_blink, task);
			}
			if (sub_power->index == 4) {
				modem_status_n(36);//no_creg
				
				sub_blink->param[0] = 20; //Quant (2 sec)
				sub_blink->param[1] = 100;  //Period, ms
				Task_start_sub_tp(sub_blink, task);
			}
		}
	}
	else {
		//return of sub_blink
		
		//Начинаем сначала
		ret = Task_goto(task, 0); //Modem_start_power
	}
	
	return ret;
}

bool Modem_start_sms(tp_task_type *task) {
	
	bool ret = false;
	
	tp_task_type *sub_sms = Task_by_name("MSMSS");
	//tp_task_type *sub_blink = Task_by_name("MBlink");
	
	if (task->iter == 0) {
		Task_start_sub_tp(sub_sms, task);
		
	} else {
		
		if (!Task_is_error(sub_sms)) {
			
			ret = true;
			
		} else {
			//Повтор
			ret = Task_try(task, 5);
		}
	}
	
	return ret;
}

bool Modem_start_gprs(tp_task_type *task) {
	
	bool ret = false;
	
	tp_task_type *sub_gprs = Task_by_name("MGPRS");
	tp_task_type *sub_blink = Task_by_name("MBlink");
	
	if (task->iter == 0) {
		MODEM_GPRS = false;
		modem_status_n(37); //GPRS
		Task_start_sub_tp(sub_gprs, task);
	}
	else if (task->iter == 1) {
		//return of sub_gprs
		
		if (!Task_is_error(sub_gprs)) {
			
			MODEM_GPRS = true;
			modem_status_n(38); //gprs|ok
			
			//MODEM_CONNECTED = true;
			
			ret = true;
			
		} else {
			modem_status_n(39); //no_gprs
			
			sub_blink->param[0] = 40;  //Quant (1.2 sec)
			sub_blink->param[1] = 30;  //Period, ms
			Task_start_sub_tp(sub_blink, task);
		}
	} else {
		
		//return of sub_blink
		
		//Начинаем сначала
		ret = Task_goto(task, 0); //Modem_start_power
	}
	
	return ret;
}

bool Modem_start_connect(tp_task_type *task) {
	
	bool ret = false;
	
	tp_task_type *sub_connect = Task_by_name("MSRV");
	tp_task_type *sub_blink = Task_by_name("MBlink");
	
	if (!task->iter) {
		
		MODEM_CONNECTED = false;
		//modem_status("srv");
		Task_start_sub_tp(sub_connect, task);
	}
	else if (task->iter==1) {
		//return of sub_connect
		
		if (MODEM_CONNECTED) {
			modem_connected();
			ret = true;
		} else {

			modem_status_n(41);//no_connect
			modem_disconnected();
			
			sub_blink->param[0] = 100;  //Quant (3 sec)
			sub_blink->param[1] = 30;  //Period, ms
			Task_start_sub_tp(sub_blink, task);
		}
	}
	else {
		//return of sub_blink
		
		//Начинаем сначала
		ret = Task_goto(task, 0); //Modem_start_power
	}
	
	return ret;
}

bool M_sub_power(tp_task_type *task) {
	
	bool ret = false;
	
	if (is_first_start) {
		is_first_start = 0;
		
		if (!HAL_GPIO_ReadPin(PIN_MODEM_STATUS_PORT, PIN_MODEM_STATUS_PIN)) {
			//serial_write("First start");
			
			//Модем отключен
			//включаем питание без задержек
			modem_power_on();

			task->wait = 100;
			return true;
		} else {
			//Модем уже включен, значит был soft reset
		}
	}
	
	if (!task->iter) {
		
		HAL_GPIO_WritePin(PIN_MODEM_RESET_PORT, PIN_MODEM_RESET_PIN, GPIO_PIN_RESET);
		
		//отключаем питание на 5 сек
		modem_power_off();
		
		task->wait = 5000;
		ret = false;
	} else {
		HAL_GPIO_WritePin(PIN_MODEM_RESET_PORT, PIN_MODEM_RESET_PIN, GPIO_PIN_SET);
		
		//включаем питание, ждем 1 сек
		modem_power_on();
		
		task->wait = 1000;
		ret = true;
	}
	
	return ret;
}

bool M_sub_reset(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		//Power key подтянут на плюс. Для запуска нужно подать 0 в течение 1 сек
		HAL_GPIO_WritePin(PIN_MODEM_RESET_PORT, PIN_MODEM_RESET_PIN, GPIO_PIN_RESET);
		
		task->wait = 1000;
	}
	else if (task->iter==1) {
		
		HAL_GPIO_WritePin(PIN_MODEM_RESET_PORT, PIN_MODEM_RESET_PIN, GPIO_PIN_SET);
		
		task->wait = 2300;
		
	}
	else if (task->iter>=2) {
		
		if (HAL_GPIO_ReadPin(	PIN_MODEM_STATUS_PORT, PIN_MODEM_STATUS_PIN)) {
			
			modem_status_n(40); //power_on
			
			ret = true;
		} else {
			
			modem_status_n(30); //no_pwr_status
			
			//Повтор
			ret = Task_try(task, 5);
		}
	}
	
	return ret;
}

bool M_at_resp(tp_task_type *task, uint8_t ncmd, uint8_t nresp) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		return M_AT_send(task, ncmd);

	} else if (task->iter <= 200) {//2 сек
		
		char cmd[16];
		modem_flash(cmd, nresp);
		
		ret = modem_data_check(cmd);
		
		if (!ret) {task->wait = 10;}
		
	} else {
		//Повтор
		ret = Task_try(task, 5);
	}
	return ret;
}

bool M_sub_at(tp_task_type *task) {
	
	//AT - OK
	return M_at_resp(task, 45, 48);
}
//Отключаем эхо
bool M_sub_echo(tp_task_type *task) {
	
	//ATE0 - OK
	return M_at_resp(task, 9, 48);
}
//Включаем SMS
bool M_sub_sms(tp_task_type *task) {
	
	//AT+CMGF=1 - OK
	return M_at_resp(task, 47, 48);
}

//Включаем режим отображения сразу
bool M_sub_sms_scr(tp_task_type *task) {
	
	//AT+CNMI=1,2,0,0,0 - OK
	return M_at_resp(task, 19, 48);
}

//Задаем кодировку
bool M_sub_sms_code(tp_task_type *task) {
	
	//AT+CSCS="GSM" - OK
	return M_at_resp(task, 22, 48);
}

//Удаляем все SMS
bool M_sub_sms_del(tp_task_type *task) {
	
	//AT+CSCS="GSM" - OK
	return M_at_resp(task, 13, 48);
}

//Регистрация в сети
bool M_sub_creg(tp_task_type *task) {
	
	char result[64] = "";
	
	if (!task->iter) {
		
		return M_AT_send(task, 0);//AT+CREG?
		
	} else if ( M_AT_command(task, 1, result, sizeof(result), 200, 5) ) {
		
		char cmd[32] = "";
		modem_flash(cmd, 10); //+CREG: 0,1
		
		if ( !strcmp(result, cmd) ) {
			
			MODEM_NET_REG = true;
			
			return true;
			
			} else {
			modem_status(result);
			
			task->wait = 3000;
			
			//Повтор
			return Task_try(task, 8);
		}
	} else {
		return false;
	}
}

//Получаем IMEI
bool M_sub_imei(tp_task_type *task) {

	if (!task->iter) {
		return M_AT_send(task, 1);//AT+GSN
	} else {
		return M_AT_command(task, 1, modem_imei, 32, 200, 5);
	}
}

//Получаем CSQ
bool M_sub_csq(tp_task_type *task) {
	
	if (!task->iter) {
		return M_AT_send(task, 16);//AT+CSQ
		
	} else if ( M_AT_command(task, 1, modem_csq, 16, 200, 5) ) {
		
		shiftString(modem_csq, 6);
		return true;
	} else {
		return false;
	}
}
bool M_sub_check(tp_task_type *task) {
	
	char cmd[32] = "";
	
	modem_flash(cmd, 21); //IMEI: %s, CSQ: %s
	
	sprintf(mess, cmd, modem_imei, modem_csq);
	modem_status(mess);
	
	return true;
}


//Проверка GPRS
bool M_sub_gprs_exist(tp_task_type *task) {
	
	if (MODEM_GPRS) {return true;}
	
	bool ret = false;
	
	if (!task->iter) {
		
		return M_AT_send(task, 61); //AT+CIPSHUT

	} else if (task->iter <= 200) {//2 сек
		
		char cmd[32];
		modem_flash(cmd, 63); //SHUT OK
		
		ret = modem_data_check(cmd);
		
		if (!ret) {task->wait = 10;}
		
	} else {
		//Повтор
		ret = Task_try(task, 5);
	}
	return ret;
}
bool M_sub_gprs_cont(tp_task_type *task) {
	
	if (MODEM_GPRS) {return true;}
	
	bool ret = false;
	
	if (!task->iter) {
		
		modem_write_prepare();
		
		char cmd[32] = "";
		char gprs_apn[30] = "";
		
		modem_flash(cmd, 62); //AT+CSTT="FixedIP.nw","",""
		
		if (eeprom_read_byte(EE_gprs_set) == set_val) {
			eeprom_read_buffer(EE_gprs_apn, (void *)&gprs_apn, sizeof(gprs_apn));
		} else {
			modem_flash(gprs_apn, 50);
		}
		
		char gprs_usr[20] = "";
		
		if (eeprom_read_byte(EE_gprs_set) == set_val) {
			eeprom_read_buffer(EE_gprs_usr, (void *)&gprs_usr, sizeof(gprs_usr));
		} else {
			modem_flash(gprs_usr, 51);//MODEM_APN_USER
		}
		
		char gprs_pwd[20] = "";
		
		if (eeprom_read_byte(EE_gprs_set) == set_val) {
			eeprom_read_buffer(EE_gprs_pwd, (void *)&gprs_pwd, sizeof(gprs_pwd));
		} else {
			modem_flash(gprs_pwd, 52);//MODEM_APN_PASSWD
		}
		
		sprintf(mess, cmd, gprs_apn, gprs_usr, gprs_pwd);
		//serial_write(mess);
		
		modem_write_tx(mess);
		task->wait = 100;

	} else if (task->iter <= 200) {//2 сек
		
		ret = modem_data_check(lbl_OK);
		
		if (!ret) {task->wait = 10;}
		
	} else {
		//Повтор
		ret = Task_try(task, 5);
	}
	return ret;
}
bool M_sub_gprs_connect(tp_task_type *task) {
	
	if (MODEM_GPRS) {return true;}
	
	bool ret = false;
	
	if (!task->iter) {
		
		//return M_AT_send(task, 8);//AT+SAPBR=1,1
		return M_AT_send(task, 59);//AT+CIICR

	} else if (task->iter <= 200) {//2 сек
		
		ret = modem_data_check(lbl_OK);
		
		if (!ret) {task->wait = 10;}
		
	} else {
		//Повтор
		ret = Task_try(task, 5);
	}
	return ret;
}
bool M_sub_gprs_IP(tp_task_type *task) {
	
	if (MODEM_GPRS) {return true;}
	
	if (!task->iter) {
		return M_AT_send(task, 60);//AT+CIFSR
		
	} else if ( M_AT_command(task, 1, modem_ip, 16, 200, 5) ) {
		sprintf(mess, "IP: %s", modem_ip);
		modem_status(mess);
		
		return true;
	} else {
		return false;
	}
}
bool M_sub_srv_set_1(tp_task_type *task) {
	
	//IP 1--------------------------------------------------------
	if (eeprom_read_byte(EE_IP_1_set) == set_val) {
		eeprom_read_buffer(EE_IP_1_name, (void *)&server_ip, sizeof(server_ip));
		server_port = eeprom_read_uint16(EE_IP_1_port);
	} else {
		modem_flash(server_ip, 25);
		server_port = MODEM_SERVER_PORT;
	}
	return true;
}
bool M_sub_srv_set_2(tp_task_type *task) {
	
	if (MODEM_CONNECTED) {return true;}
	
	if (eeprom_read_byte(EE_IP_2_set) == set_val) {
		eeprom_read_buffer(EE_IP_2_name, (void *)&server_ip, sizeof(server_ip));
		server_port = eeprom_read_uint16(EE_IP_2_port);
	} else {
		clearString(server_ip);
		server_port = 0;
	}
	return true;
}
bool M_sub_tcp_start(tp_task_type *task) {
	
	if (!strlen(server_ip)) {return true;}
	if (MODEM_CONNECTED) {return true;}
	
	bool ret = false;
	
	char cmd[50];
	char cmd_2[50];
	
	if (!task->iter) {

		modem_flash(cmd, 11); //AT+CIPSTART=\"TCP\",\"%s\",%d

		sprintf(mess, cmd, server_ip, server_port);
		
		modem_write_prepare();
		modem_write_tx(mess);
		
		modem_flash(cmd, 28); //Connecting to \"%s\":%d
		sprintf(mess, cmd, server_ip, server_port);
		serial_write(mess);
		
		task->wait = 100;

	} else if (task->iter <= 1200) {//120 сек
		
		if (modem_data_ready()) {

			modem_flash(cmd, 12); //CONNECT OK
			modem_flash(cmd_2, 29); //CONNECT FAIL
			
			#ifdef DEBUG
				serial_write(modem_data);
			#endif
			
			if (strstr(modem_data, cmd) != NULL) {
				
				//Коннект к серверу
				MODEM_CONNECTED = true;
				
				ret = true;
				
				clearString(modem_data);
				modem_serial_task = 0;
				
			} else if (strstr(modem_data, cmd_2) != NULL) {
				
				clearString(modem_data);
				modem_serial_task = 0;
				
				//Повтор
				ret = Task_try(task, 2);
				Task_error(task, 0); //не нужно завершения
				
			} else {
				task->wait = 100;
				ret = false;
			}
			
		} else {
			task->wait = 100;
		}
		
	} else {
		
		//Повтор
		ret = Task_try(task, 2);
		Task_error(task, 0); //не нужно завершения
	}
	return ret;
}

bool M_call_check(tp_task_type *task) {
	
	bool ret = false;
	
	if (task->iter <= 600) {
		
		//Таск звонка
		tp_task_type *task_modem_call = Task_by_name("MCALL");
		
		if (Task_is_running(task_modem_call)) {
			
			task->wait = 100; //60sec
			
		} else {
			ret = true;
		}
	} else {
		ret = true;
	}
	
	return ret;
}

bool M_srv_start(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		//serial_write("M_srv_start");
		
		modem_is_sending = true;
		
		return M_AT_send(task, 14);//AT+CIPSEND

	} else if (task->iter <= 50) {//5 сек
		
		if (modem_data_ready()) {
			
			if (modem_data_check(">")) {
				ret = true;
			} else {
				//Повтор
				ret = Task_try(task, 3);
			}
			
		} else {
			task->wait = 100;
		}
		
	} else {
		//Не дождались, повтор
		ret = Task_try(task, 3);
	}
	
	if (Task_is_error(task)) {
		modem_is_sending = false;
	}
	return ret;
}

bool M_srv_send(tp_task_type *task) {
	
	bool ret = false;
	task->wait = 100;
	
	if (!task->iter) {
		
		//serial_write("M_srv_send");
		
		modem_write_prepare();
		
		char cmd[16];
		modem_flash(cmd, 54); //srvdata|
		
		modem_write_str(cmd);
		
		modem_write_b1();
		
		modem_write_char(0x0D);
		modem_write_char(0x1A);

	} else if (task->iter <= 50) {//5 сек
		
		if (modem_data_ready()) {
			
			char cmd[16];
			modem_flash(cmd, 15); //SEND OK
			
			if (modem_data_check(cmd)) {
				ret = true;
				
				//serial_write("SEND OK");
				
				MODEM_LAST_SEND = now();
				MODEM_LAST_CHECK = now();
			
			} else {
				serial_write("SEND ERROR");
				
				//Не дождались, повтор
				ret = Task_try(task, 3);
				
				//ret = true;
			}
		}
		
	} else {
		ret = Task_try(task, 3);
	}
	if (ret) {
		modem_is_sending = false;
	}
	if (Task_is_error(task)) {
		clearString(modem_data);
		modem_serial_task = 0;
		
		#ifdef DEBUG
			serial_write("TASK SEND ERROR");
		#endif
		
		//Передать не получилось, перезагруз модема
		modem_disconnected();
	}
	return ret;
}
bool M_srv_copy(tp_task_type *task) {
	
	modem_buf_cpy();
	
	modem_is_sending = false;
	
	return true;
}

bool M_sms_start(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		char cmd_2[64] = "";
		
		//Обрезаем первые два символа у номера  (+7)
		strcpy(cmd_2, modem_phone);
		shiftString(cmd_2, 2);
		gp_log_add(ELOG_SMS, 0, cmd_2);
		
		sprintf(cmd_2, "AT+CMGS=\"%s\"", modem_phone);
		
		modem_write_prepare();
		modem_write_tx(cmd_2);
		
		task->wait = 100; //100ms
		
		return false;

	} else if (task->iter <= 50) {//5 сек
		
		if (modem_data_ready()) {
			
			if (modem_data_check(">")) {
				ret = true;
			} else {
				//Повтор
				ret = Task_try(task, 3);
			}
			
			} else {
			task->wait = 100;
		}
		
	} else {
		//Не дождались, повтор
		ret = Task_try(task, 3);
	}
	if (Task_is_error(task)) {
		clearString(modem_data);
		modem_serial_task = 0;
	}
	return ret;
}

bool M_sms_send(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		addChar(mess, 0x1A); //Esc
		
		modem_write_prepare();
		modem_write_tx(mess);
		
		task->wait = 100;

	} else if (task->iter <= 100) {//10 сек
		
		task->wait = 100;
		
		if (modem_data_ready()) {
			
			Trim(modem_data);
			if (strlen(modem_data)>0) {
				
				//char cmd[16];
				//modem_flash(cmd, 60); //+CMGS:
				
				if (modem_data_check(lbl_OK)) {
					ret = true;
				} else {
					Task_error(task, 1);
					ret = true;
				}
			} else {
				clearString(modem_data);
				modem_get_time = 0; //пришли пустые строки
			}
		}
		
	} else {
		Task_error(task, 1);
		ret = true;
	}
	if (ret) {
		modem_serial_task = 0;
		
		modem_flash(mess, 46); //sms|
		
		if (Task_is_error(task)) {
			strcat(mess, lbl_fail);
			
			clearString(modem_data);
			modem_serial_task = 0;
		} else {
			strcat(mess, lbl_ok);
		}
		serial_write(mess);
		clearString(mess);
	}
	return ret;
}

bool M_call_init(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		task->wait = 2000; //2sec
		return false;
		
	} else if (task->iter <= 100) {
		
		//Таск отправки данных
		tp_task_type *task_modem_send = Task_by_name("MSrvS");
		
		if (Task_is_running(task_modem_send)) {
			
			task->wait = 100; //10sec
			
		} else {
			ret = true;
		}
	} else {
		ret = true;
	}
	
	return ret;
}

bool M_call_start(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		
		char cmd_1[16] = "";
		modem_flash(cmd_1, 24); //ATD%s
		
		char cmd_2[32] = "";
		char phone[32] = "";
		
		phone_num(task->param[0], phone);
		sprintf(cmd_2, cmd_1, phone);
		
		gp_log_add(ELOG_CALL, task->param[1], phone);
		
		modem_write_prepare();
		modem_write_tx(cmd_2);
		
		modem_flash(cmd_1, 44); //Calling %s
		sprintf(cmd_2, cmd_1, phone);
		serial_write(cmd_2);
		
		task->wait = 100; //100ms
		
		return false;

	} else if (task->iter <= 50) {//5 сек
		
		if (modem_data_ready()) {
			
			if (modem_data_check(lbl_OK)) {
				ret = true;
			} else {
				//Повтор
				ret = Task_try(task, 3);
			}
			
			} else {
			task->wait = 100;
		}
		
	} else {
		//Не дождались, повтор
		ret = Task_try(task, 3);
	}
	if (Task_is_error(task)) {
		clearString(modem_data);
		modem_serial_task = 0;
	}
	return ret;
}

bool M_call_end(tp_task_type *task) {
	
	bool ret = false;
	
	if (task->iter < 25) {
		
		if (!task->iter) {
			modem_write_prepare();
		}
		
		if (modem_data_ready()) {
			
			char cmd_1[16] = "";
			modem_flash(cmd_1, 23); //BUSY
			
			if (modem_data_check(cmd_1)) {
				serial_write(cmd_1);
				ret = true;
			}
		}
		
		task->wait = ret ? 1 : 1000;

	} else if (task->iter==25) {
		
		modem_write_prepare();
		modem_write_tx("ATH0");
		
		task->wait = 100;
	} else {
		if (modem_data_ready()) {
			
			if (modem_data_check(lbl_OK)) {
				
				clearString(modem_data);
				modem_serial_task = 0;
				
				ret = true;
			} else {
				//Повтор
				ret = Task_try(task, 3);
			}
			
			task->wait = 100;
		}
	}
	
	if (ret) {
		clearString(modem_data);
		modem_serial_task = 0;
	}
	
	return ret;
}

bool M_blink(tp_task_type *task) {
	bool ret = false;
	
	if (task->iter <= task->param[0]) {
		task->wait = task->param[1];
		
		HAL_GPIO_TogglePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN);
	} else {
		HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
		ret = true;
	}
	
	return ret;
}

bool P_blink(tp_task_type *task) {
	bool ret = false;
	
	if (!task->iter) {
		task->wait = 200;
		HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
		ret = true;
	}
	
	return ret;
}

//---------------------------------------------------------------------------------------------------------------------------------


bool task_step_begin(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	tp_phase_stat *phase_stat = Phase_stat_num(p);
	
	char str[32] = "";
	
	if (debug_mess) {
		sprintf(mess, "Phase %c Begin", phase->let);
		serial_write(mess);
	}
	
	Regulator_dt_reg();
	
	Phase_stat_init(p-1);
	
	phase->is_running = true;
	phase->dt_start = now();
	phase->dt_end   = 0;
	
	Phase_err_clr(phase);
	
	task->param[0] = 0; //порт
	task->param[1] = 0; //код ошибки
	
	phase_stat->vbef = phase->volt_before = volt_last(p);
	phase_stat->sbef = phase->step_before = phase->step;
	
	phase_save_state(p, task);
	
	return true;
}

bool task_step_finish(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	tp_phase_stat *stat = Phase_stat_num(p);
	
	phase->is_running = false;
	phase->dt_end = stat->dt = now();
	phase->net_count = 0;
	phase->swithes++;
	
	char str[32] = "";
	
	//Отключаем сканирование
	if (Ports_scan_busy(phase->num)) {
		Ports_scan_mode(0);
		Ports_scan_busy(0);
	}
	
	if (!phase->error) {
		if (debug_mess) {
			sprintf(mess, "Phase %c End Success", phase->let);
			serial_write(mess);
		}
		stat->step = phase->step = phase->next_step;
	} else {
		//sprintf(mess, "task_step_finish error p0=%u, p1=%u", task->param[0], task->param[1]);
		//serial_write(mess);
		
		if (debug_mess) {
			sprintf(mess, "Phase %c End Fail", phase->let);
			serial_write(mess);
		}
		
		//Процедура ошибки регулирования
		uint8_t cont   = task->param[0];
		uint8_t errnum = task->param[1];
			
		phase->error    =  1;
		phase->errnum   =  errnum;
		phase->step     =  0;
		phase->dt_error =  now();
		
		Phase_ns_on(phase->num, cont);
	
		phase_save_state(p, task);
		
		Phase_err_set(phase, cont, errnum);
	}
	
	stat->volt = volt_last(p);
	stat->errcont = phase->errcont;
	stat->errnum = phase->errnum;

	phase->next_step = 0;
	
	phase_save_state(p, task);
	
	//sprintf(mess, "%u;%lu;%lu;%lu;%lu;", stat->num, stat->cont[0].on_t1, stat->cont[0].on_t2, stat->cont[0].of_t1, stat->cont[0].of_t2);
	//serial_write(mess);
	
	//Пишем стату
	//Phase_archive_stat(phase->let, stat);
	
	return true;
}

bool task_force_trans(tp_task_type *task, uint8_t p) {
	
	bool ret = false;
	task->wait = 20;
	
	tp_phase_type *phase = Phase_num(p);
	
	/*
	A_KM2_on,
	A_KM3_on,
	A_KM1_off,
	A_KM2_off,
	A_delay_5sec,
	A_KM3_off
	*/
	
	if (debug_mess) {
		sprintf(mess, "Force_trans iter=%u", task->iter); //Force_trans iter=%u
		serial_write(mess);
	}
	
	switch (task->iter) {
		case 0:
			phase->error     = 2; //отключаем ремонт
			
			//Подготовка транзита
			Port_clr(phase->num, 2);
			Port_clr(phase->num, 3);
			Ports_renew();
		break;
		
		case 1:
			//Установка транзита
			Port_set(phase->num, 2);
			Ports_renew();
		break;
		
		case 2:
			//Установка транзита
			Port_set(phase->num, 3);
			Ports_renew();
		break;
		
		case 3:
			//KM1_off
			Port_clr(phase->num, 1);
			Ports_renew();
		break;
		
		case 4:
			//KM2_off
			Port_clr(phase->num, 2);
			Ports_renew();
		break;
		
		case 5:
			//A_delay_5sec
			task->wait = 5000;
		break;
		
		case 6:
			//KM3_off
			Port_clr(phase->num, 3);
			Ports_renew();
		break;
		
		case 7:
			//KM4-9_off
			for (uint8_t n = 4; n <= 9; n++) {
				Port_clr(phase->num, n);
			}
			Ports_renew();
			
			phase->error     = 1; //восстанавливаем ошибку
			
			ret = true;
		break;
		
		default:
			phase->error     = 1;
			ret = true;
		break;
	}
	return ret;
}

bool task_TransOn(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	
	if (!task->iter) {
		
		phase_save_state(p, task);
			
		if (phase->error) {return true;}
	
		//Уже в транзите и переключаемся не в транзит
		if (!phase->step && phase->next_step) {return true;}
	}
	
	bool ret = false;
	
	char tname[16] = "";
	sprintf(tname, "%cT_On", phase->let);
	tp_task_type *sub = Task_by_name(tname);
	
	switch (task->iter) {
		case 0:
			
			if (debug_mess) {
				serial_write(sub->name);
			}
			
			Task_start_sub_tp(sub, task);
		break;
		
		case 1:
			if (phase->error) {
				task->param[0] = sub->param[0];
				task->param[1] = sub->param[1];
			}
			ret = true;
		break;
		
		default:
			ret = true;
		break;
	}
	return ret;
}

bool task_TransOff(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	
	if (!task->iter) {
		phase_save_state(p, task);
		
		if (phase->error) {return true;}
	}

	bool ret = false;
	
	char tname[16] = "";
	
	sprintf(tname, "%cT_Off", phase->let);

	tp_task_type *sub = Task_by_name(tname);

	switch (task->iter) {
		case 0:
			if (debug_mess) {
				serial_write(sub->name);
			}
			
			Task_start_sub_tp(sub, task);
		break;
	
		case 1:
		
			if (phase->error) {
				task->param[0] = sub->param[0];
				task->param[1] = sub->param[1];
			}
			ret = true;
		break;
		
		default:
			ret = true;
		break;
	}
	return ret;
}

bool task_repair_begin(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	
	uint8_t port = 0;
	bool    ret = false;
	
	if (!task->iter) {
		task->param[0] = port;
			
		//Отключаем все контакторы
		for (uint8_t n = 1; n <= 9; n++) {
			Port_clr(phase->num, n);
		}
		Ports_renew();
			
		//Определяем, какой	порт ошибочный
		port = Phase_ns_port(phase);
		
		//Сохраняем параметры для будущих итераций
		task->param[0] = port;
		task->param[1] = 0; //результат
		
		if (!port) {return true;}
		
		Phase_repair_on(phase); //идет исправление ошибки
			
		if (port==1 || port==9) {
			//Первый и девятый порт нельзя ремонтировать, сбрасываем ошибку
			task->param[1] = 1; //результат ок
			return true;
		}
		
		if (debug_mess) {
			sprintf(mess, "REP%c port=%u start", phase->let, port);
			serial_write(mess);
		}
		
		Port_clr(phase->num, port);
		Ports_renew();
		
		task->wait = 1;
		
		return false;
	} else {
		port = task->param[0];
	}
	//Дергаем 5 раз, останавливаемся во включенном состоянии
	if (task->iter==1 || task->iter==3 || task->iter==5 || task->iter==7 || task->iter==9) {
		Port_set(phase->num, port);
		Ports_renew();
		
		task->wait = 500;
		return false;
	}
	if (task->iter==2 || task->iter==4 || task->iter==6 || task->iter==8) {
		Port_clr(phase->num, port);
		Ports_renew();
		
		task->wait = 500;
		return false;
	}
	//Проверяем включение
	if (task->iter==10) {
		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
		//Установка входного порта
		Port_phase_in(phase->num, port);
		
		//Запускаем сканирование
		Ports_scan_mode(2);
		
		task->wait = TASKS_WAIT_INIT;
		return false;
	}
	
	if (task->iter==11) {

		if (Ports_scan_P2()) {
			//Все ок, продолжаем
			
			task->wait = 500;
		} else {
			//Проверку не прошли
			ret = true;
		}
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();

		Port_clr(phase->num, port);
		Ports_renew();
		
		return ret;
	}
	
	if (task->iter==12) {
		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
		//Установка входного порта
		Port_phase_in(phase->num, port);
		
		//Запускаем сканирование
		Ports_scan_mode(1);
	
		task->wait = TASKS_WAIT_INIT;
		return false;
	}
	
	if (task->iter==13) {
		
		if (Ports_scan_P1()) {
			//Ошибка исправлена
			task->param[1] = 1;
		}
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		return true;
	}
	return false;
}
bool task_repair_finish(tp_task_type *task, uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	
	//Нет порта
	if (!task->param[0]) {return true;}
	
	if (debug_mess) {
		sprintf(mess, "REP%c port=%u result %u", phase->let, task->param[0], task->param[1]);
		serial_write(mess);
	}
	
	if (task->param[1]) {
		//Ошибка исправлена
		Phase_err_clr(phase);
	} else {
		//Оставляем ошибку
		phase->error = 1;
	}
	phase_save_state(phase->num, task);
	
	return true;
}


bool Contactor_on(tp_phase_type *phase, tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	bool ret = false;
	uint16_t step_1 = TASKS_TIMEOUT_1;
	uint16_t step_2 = TASKS_TIMEOUT_1 + TASKS_TIMEOUT_2;
	uint16_t step_3 = TASKS_TIMEOUT_1 + TASKS_TIMEOUT_2 + TASKS_TIMEOUT_3;
	
	//uint8_t  p = phase->num;
	char str[32] = "";
	
	tp_phase_stat *phase_stat = Phase_stat_num(phase->num);
	
	//Начало
	if (!task->iter) {
		
		//Пропуск контактора
		if (port==4 && auto_steps_count <= 4) {
			//Нет пятой ступени, пропускаем 4 контактор
			return true;
		}
		if (port==5 && auto_steps_count <= 3) {
			//Нет четвертой ступени, пропускаем 5 контактор
			return true;
		}
		if (port==6 && auto_steps_count <= 2) {
			//Нет третьей ступени, пропускаем 6 контактор
			return true;
		}
		
		//Если уже включен
		if ( Port_get(phase->num, port) ) {
			return true;
		}
		
		//Чтение логики
		uint8_t logic = Ports_logic_get(phase->num, port);

		if (logic) {
			//Просто включаем
			Phase_port_on(phase, port);
	
			task->wait = CONTACTOR_WORK_TIME;
	
			return true;
		}
		
		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
		
		phase_save_state(phase->num, task);
		
		if (debug_mess) {
		
			sprintf(mess, "KM%c%u on", phase->let, port);
			serial_write(mess);
		}
		
		//Запускаем сканирование
		Ports_scan_mode(2);
		
		//Установка входного порта
		Port_phase_in(phase->num, port);
		
		task->wait = TASKS_WAIT_INIT;
		return false;
	}
	if (task->iter==1) {
		t0=t1=t2=0;
		
		if (Ports_scan_P2()) {
			
			//Уже включен
			Ports_scan_mode(0);
			Ports_scan_busy(0);
			Ports_in_clr();
			
			if (debug_mess) {
				sprintf(mess, "KM%c%u on skip", phase->let, port);
				serial_write(mess);
			}
			return true;
		} else {
			//Если не включен и не выключен то видимо залип
			//На всякий случай принудительно выключаем
			Port_clr(phase->num, port);
			Ports_renew();
			
			//Включаем сканирование
			Ports_scan_mode(1);
			task->wait = TASKS_WAIT_SCAN;
			return false;
		}
	}
	
	if (task->iter < step_1) {
		
		task->wait = 1;
		
		//ждем нормального положения контакта
		if ( Ports_scan_P1() ) {
			
			//Contactor ON
			counter_micros_start();
			Port_set(phase->num, port);
			Ports_renew();
			t0 = counter_micros_stop();
			
			counter_micros_start();
			
			task->iter = step_1;
		}
		ret = false; //next iter
		
	} else if (task->iter == step_1) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Нет сигнала с нормально замкнутого контакта
		if (debug_error) {
			sprintf(mess, "KM%c%u on error %u", phase->let, port, 1);
			serial_write(mess);
		}
		
		//Повтор, сбрасываем порт
		Port_clr(phase->num, port);
		Ports_renew();
			
		task->wait = TASKS_WAIT_RETRY;
		ret = Task_try(task, CONTACTOR_FAIL_TRY); //Делаем еще несколько попыток
			
		if (Task_is_error(task)) {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 11;
			
			Task_error(task, 0);
		}
		
	} else if (task->iter < step_2) {
		
		if (!Ports_scan_P1()) {//Контакт разомкнулся, свободный полет
			
			t1 = counter_micros_stop();
			
			phase_stat->cont[port-1].on_t1 = t1;
			
			task->iter = step_2; //step_2+1
			
			counter_micros_start();
			
			Ports_scan_mode(2); //Ловим второй контакт
			
			task->wait = TASKS_WAIT_SCAN; //wait
		} else {
			task->wait = 1; //wait
		}
		
		ret = false; //next iter
		
	} else if (task->iter == step_2) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Контакт 1 не разомкнулся
		if (debug_error) {
			sprintf(mess, "KM%c%u on error %u", phase->let, port, 2);
			serial_write(mess);
		}
		
		if (try_cnt) {
			//Повтор, сбрасываем порт
			Port_clr(phase->num, port);
			Ports_renew();

			task->wait = TASKS_WAIT_RETRY;
			ret = Task_try(task, try_cnt); //возвращаемся в начало
			
			if (Task_is_error(task)) {
				phase->error   = 1;
				task->param[0] = port;
				task->param[1] = 12;
				Task_error(task, 0);
			}
		} else {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 12;
			ret = true;
		}
		
	} else if (task->iter < step_3) {
		
		if (Ports_scan_P2()) {
			//Успешное включение
			
			t2 = counter_micros_stop();
			phase_stat->cont[port-1].on_t2 = t2;
			
			Ports_scan_mode(0);
			Ports_scan_busy(0);
			Ports_in_clr();
			
			if (debug_mess) {
				sprintf(mess, "t1=%u, t2=%u", t1, t2);
				serial_write(mess);
			}
			
			ret = true;
		} else {
			task->wait = 1; //wait
			ret = false; //next iter
		}
		
	} else if (task->iter >= step_3) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Контакт 2 не замкнулся, состояние свободного полета
		if (debug_error) {
			sprintf(mess, "KM%c%u on error %u", phase->let, port, 3);
			serial_write(mess);
		}
		
		if (try_cnt) {
			//Повтор
			Port_clr(phase->num, port);
			Ports_renew();
			
			task->wait = TASKS_WAIT_RETRY;
			ret = Task_try(task, try_cnt);
			
			if (Task_is_error(task)) {
				phase->error   = 1;
				task->param[0] = port;
				task->param[1] = 13;
				Task_error(task, 0);
			}
		} else {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 13;
			ret = true;
		}
	}
	
	return ret;
}
bool Contactor_off(tp_phase_type *phase, tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	bool ret = false;
	uint16_t step_1 = TASKS_TIMEOUT_1;
	uint16_t step_2 = TASKS_TIMEOUT_1 + TASKS_TIMEOUT_2;
	uint16_t step_3 = TASKS_TIMEOUT_1 + TASKS_TIMEOUT_2 + TASKS_TIMEOUT_3;
	
	char str[32] = "";
	
	tp_phase_stat *phase_stat = Phase_stat_num(phase->num);
	
	//Начало
	if (!task->iter) {
		
		//Пропуск контактора
		if (port==4 && auto_steps_count <= 4) {
			//Нет пятой ступени, пропускаем 4 контактор
			return true;
		}
		if (port==5 && auto_steps_count <= 3) {
			//Нет четвертой ступени, пропускаем 5 контактор
			return true;
		}
		if (port==6 && auto_steps_count <= 2) {
			//Нет третьей ступени, пропускаем 6 контактор
			return true;
		}
		
		//Если уже выключен
		if ( !Port_get(phase->num, port) ) {
			return true;
		}
		
		//Чтение логики
		uint8_t logic = Ports_logic_get(phase->num, port);

		if (logic) {
			//Просто выключаем
			Phase_port_off(phase, port);
	
			task->wait = CONTACTOR_WORK_TIME;
	
			return true;
		}
		
		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
		
		phase_save_state(phase->num, task);
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u off", phase->let, port);
			serial_write(mess);
		}
		
		//Запускаем сканирование
		Ports_scan_mode(1);
		
		//Установка входного порта
		Port_phase_in(phase->num, port);
		
		task->wait = TASKS_WAIT_INIT;
		return false;
	}
	if (task->iter==1) {
		t0=t1=t2=0;
		
		if (Ports_scan_P1()) {
			//Уже выключен
			
			Ports_scan_mode(0);
			Ports_scan_busy(0);
			Ports_in_clr();
			
			if (debug_mess) {
				sprintf(mess, "KM%c%u off skip", phase->let, port);
				serial_write(mess);
			}
			return true;
		} else {
			//Если не выключен и не включен то видимо залип
			//На всякий случай принудительно включаем
			Port_set(phase->num, port);
			Ports_renew();
			
			//Включаем сканирование
			Ports_scan_mode(2);
			task->wait = TASKS_WAIT_SCAN;
			
			return false;
		}
	}
	
	if (task->iter < step_1) {
		
		if (Ports_scan_P2()) {//ждем нормального положения контакта, должен быть замкнут
			
			//Contactor OFF
			counter_micros_start();
			Port_clr(phase->num, port);
			Ports_renew();
			t0 = counter_micros_stop();
			
			counter_micros_start();
			
			task->iter = step_1;
		}
		
		task->wait = 1;
		ret = false; //next iter
		
	} else if (task->iter == step_1) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Нет сигнала с нормально разомкнутого контакта
		if (debug_error) {
			sprintf(mess, "KM%c%u off error %u", phase->let, port, 1);
			serial_write(mess);
		}
		
		//Повтор, устанавливаем порт
		Port_set(phase->num, port);
		Ports_renew();
			
		task->wait = TASKS_WAIT_RETRY;
		ret = Task_try(task, CONTACTOR_FAIL_TRY);
			
		if (Task_is_error(task)) {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 21;
				
			Task_error(task, 0);
		}
		
	} else if (task->iter < step_2) {
		
		if (!Ports_scan_P2()) {//Контакт разомкнулся, свободный полет
			
			t1 = counter_micros_stop();
			phase_stat->cont[port-1].of_t1 = t1;
			
			task->iter = step_2;
			
			counter_micros_start();
			
			Ports_scan_mode(1); //Ловим второй контакт
			
			task->wait = TASKS_WAIT_SCAN; //wait
			
		} else {
			task->wait = 1; //wait
		}
		
		ret = false; //next iter
		
	} else if (task->iter == step_2) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Контакт 2 не разомкнулся, видимо сварился
		if (debug_error) {
			sprintf(mess, "KM%c%u off error %u", phase->let, port, 2);
			serial_write(mess);
		}
		
		if (try_cnt) {
			//Повтор
			Port_set(phase->num, port);
			Ports_renew();
			
			task->wait = TASKS_WAIT_RETRY;
			ret = Task_try(task, try_cnt);
			
			if (Task_is_error(task)) {
				phase->error   = 1;
				task->param[0] = port;
				task->param[1] = 22;
				
				Task_error(task, 0);
			}
		} else {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 22;
			ret = true;
		}
		
	} else if (task->iter < step_3) {

		if (Ports_scan_P1()) {
			//Успешное выключение
			
			t2 = counter_micros_stop();
			phase_stat->cont[port-1].of_t2 = t2;
			
			Ports_scan_mode(0);
			Ports_scan_busy(0);
			Ports_in_clr();
			
			if (debug_mess) {
				sprintf(mess, "t1=%u, t2=%u", t1, t2);
				serial_write(mess);
			}
			
			ret = true;
		} else {
			task->wait = 1; //wait
			ret = false; //next iter
		}
		
	} else if (task->iter == step_3) {
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		//Контакт 1 не замкнулся, состояние свободного полета
		if (debug_error) {
			sprintf(mess, "KM%c%u off error %u", phase->let, port, 3);
			serial_write(mess);
		}
		
		if (try_cnt) {
			//Повтор
			Port_set(phase->num, port);
			Ports_renew();
			
			task->wait = TASKS_WAIT_RETRY;
			ret = Task_try(task, try_cnt);
			
			if (Task_is_error(task)) {
				phase->error   = 1;
				task->param[0] = port;
				task->param[1] = 23;

				Task_error(task, 0);
			}
		} else {
			phase->error   = 1;
			task->param[0] = port;
			task->param[1] = 23;
			ret = true;
		}
	} else {
		//Успешное выключение
		
		t2 = counter_micros_stop();
		phase_stat->cont[port-1].of_t2 = t2;
		
		Ports_scan_mode(0);
		Ports_scan_busy(0);
		Ports_in_clr();
		
		if (debug_mess) {
			sprintf(mess, "t1=%u, t2=%u", t1, t2);
			serial_write(mess);
		}
		
		ret = true;
	}
	
	return ret;
}

bool Contactor_on2(tp_phase_type *phase, tp_task_type *task, uint16_t port) {
	
	if (!task->iter) {
		if (debug_mess) {
			sprintf(mess, "KM%c%u on", phase->let, port);
			serial_write(mess);
		}
		
		phase_save_state(phase->num, task);
	}
	
	Port_set(phase->num, port);
	Ports_renew();
	
	//Эмуляция ошибки
	/*
	if (port==4) {
		phase->error   = 1;
		task->param[0] = port;
		task->param[1] = 11;
	}
	*/
	
	task->wait = 1000;
	return true;
}
bool Contactor_off2(tp_phase_type *phase, tp_task_type *task, uint16_t port) {
	
	if (!task->iter) {
		if (debug_mess) {
			sprintf(mess, "KM%c%u off", phase->let, port);
			serial_write(mess);
		}
		
		phase_save_state(phase->num, task);
	}
	Port_clr(phase->num, port);
	Ports_renew();
	
	task->wait = 1000;
	return true;
}


//Actions---------------------------------------------------------------------------------------------------------------------------------------
bool Net_get(tp_task_type *task) {
	
	bool ret = false;
	
	if (!task->iter) {
		if (now() - Regulator_dt_net_reset_get() <=5) {
			//serial_write("Wait ADE");
			Task_error(task, 1);
			return true;
		}
	}
	
	//char sub_rms[16] = "AdeRms";
	
	tp_task_type *sub_power;
	if (task->iter >=5) {
		sub_power = Task_by_name("AdePower");
	}
	
	switch (task->iter) {
		case 0:
			ADE7758_rms_clear();
			
			//Task_start_sub(sub_rms, task);
			ADE7758_rms_get();
		break;
		
		case 1:
			//Task_start_sub(sub_rms, task);
			ADE7758_rms_get();
		break;
		
		case 2:
			//Task_start_sub(sub_rms, task);
			ADE7758_rms_get();
		break;
		
		case 3:
			//Task_start_sub(sub_rms, task);
			ADE7758_rms_get();
		break;
		
		case 4:
			//Task_start_sub(sub_rms, task);
			ADE7758_rms_get();
		break;
		
		case 5:
			Task_start_sub_tp(sub_power, task);
			sub_power->param[0] = 1;
		break;
		
		case 6:
			Task_start_sub_tp(sub_power, task);
			sub_power->param[0] = 2;
		break;
		
		case 7:
			Task_start_sub_tp(sub_power, task);
			sub_power->param[0] = 3;
		break;
		
		case 8:
			ret = true;
		break;
		
		default:
			ret = true;
		break;
	}
	return ret;
}
bool Net_save(tp_task_type *task) {
	
	buffers_save();
	
	char str[16] = "%u|%u|%u";
	char fstr[15] = "";
	
	//tp_phase_type *phase;
	
	if (net_show != 'n') {
		if (net_show == 'v') {
		
			sprintf(mess,str,volt_last(1),volt_last(2),volt_last(3));
			serial_write(mess);
		}
		else if (net_show == 'a') {
		
			sprintf(mess,str,volt_avg_reg(0, auto_points),volt_avg_reg(1, auto_points),volt_avg_reg(2, auto_points));
			serial_write(mess);
		}
		else if (net_show == 'r') {
			sprintf(
			mess,
			str,
			(uint16_t)((float)volt_last(1) / steps_get(Phase_num(1)->step)),
			(uint16_t)((float)volt_last(2) / steps_get(Phase_num(2)->step)),
			(uint16_t)((float)volt_last(3) / steps_get(Phase_num(3)->step))
			);
			serial_write(mess);
		}
		else if (net_show == 'i') {
		
			
			sprintf(mess, "%0.3f|%0.3f|%0.3f", curr_last(1) * auto_kt, curr_last(2) * auto_kt, curr_last(3) * auto_kt);
			serial_write(mess);
		}
		else if (net_show == 'c') {
		
			sprintf(mess, "%0.3f|%0.3f|%0.3f", cos_last(1), cos_last(2), cos_last(3));
			serial_write(mess);
		}
	}
	
	return true;
}

bool Delay_start(tp_task_type *task) {
	if (!task->iter) {
		
		
		task->wait = 5000;
		return false;
		
	} else {
		Timers_start(TIMER_NET);
		Timers_start(TIMER_AUTO);
		
		//Запись времени рестарта
		eeprom_write_uint32(EE_ADDR_RESTART, now());
		
		//Проверка автоматической прошивки
		uint8_t flag = eeprom_read_byte(EE_FLASH_FLAG);
		if (flag == FLASH_CODE) {
	
			//Дата
			uint32_t dt = now();
			eeprom_write_uint32(EE_FLASH_TIME, dt);
	
			//Сброс флага
			eeprom_write_byte(EE_FLASH_FLAG, 0xFF);
	
			//Увеличиваем счетчик прошивок
			uint16_t flash_cnt = eeprom_read_uint16(EE_FLASH_CNT);
	
			if (flash_cnt==zval_16) {
				flash_cnt = 0;
			}
			flash_cnt++;
	
			eeprom_write_uint16(EE_FLASH_CNT, flash_cnt);
		}
		
		return true;
	}
}

bool KM_Trans_on(tp_task_type *task, uint8_t p) {
	
	uint32_t t1 = 0;
	uint32_t t2 = 0;
	uint8_t  err = 0;
	uint8_t  port = 0;
	
	char str[64] = "";
	
	//Фаза
	tp_phase_type *phase = Phase_num(p);
	tp_phase_stat *phase_stat = Phase_stat_num(phase->num);
	
	if (phase->error) {
		return true;
	}
	
	//Начало
	if (!task->iter) {
		
		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
	}
	
	phase_save_state(phase->num, task);
	
	//KM3_on
	if (!phase->error) {
		port = 3;
		err = Phase_cont_on(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].on_t1 = t1;
		phase_stat->cont[port-1].on_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u on, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				sprintf(mess, "KM%c%u on, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}
	
	//KM1_off
	if (!phase->error) {
		port = 1;
		err = Phase_cont_off(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].of_t1 = t1;
		phase_stat->cont[port-1].of_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u off, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				StrFlash(str, 43); //KM%c%u off, err=%u, t1=%lu, t2=%lu
				sprintf(mess, str, phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}

	//KM2_off
	if (!phase->error) {
		port = 2;
		err = Phase_cont_off(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].of_t1 = t1;
		phase_stat->cont[port-1].of_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u off, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				StrFlash(str, 43); //KM%c%u off, err=%u, t1=%lu, t2=%lu
				sprintf(mess, str, phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}
	
	//Произошла ошибка
	if (phase->error) {
		//Отключаем
		Phase_port_off(phase, 2);
		Phase_port_off(phase, 3);
		Phase_port_off(phase, 1);
	}
	
	Ports_scan_mode(0);
	Ports_scan_busy(0);
	Ports_in_clr();
	
	return true;
}

bool KM_Trans_off(tp_task_type *task, uint8_t p) {
	
	uint32_t t1 = 0;
	uint32_t t2 = 0;
	uint8_t  err = 0;
	uint8_t  port = 0;
	char     str[64] = "";
	
	//Фаза
	tp_phase_type *phase = Phase_num(p);
	tp_phase_stat *phase_stat = Phase_stat_num(phase->num);
	
	if (phase->error) {
		return true;
	}
	
	//Начало
	if (!task->iter) {

		if (!Ports_scan_busy(phase->num)) {
			//Ждем, когда освободится сканирование
			Task_iter_manual(task, 1);
			task->wait = TASKS_WAIT_BUSY;
			return false;
		}
	}
	
	phase_save_state(phase->num, task);
	
	//KM3_on
	if (!phase->error) {
		port = 3;
		err = Phase_cont_on(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].on_t1 = t1;
		phase_stat->cont[port-1].on_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u on, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				StrFlash(str, 42); //KM%c%u on, err=%u, t1=%lu, t2=%lu
				sprintf(mess, str, phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}
	
	//KM1_on
	if (!phase->error) {
		port = 1;
		err = Phase_cont_on(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].on_t1 = t1;
		phase_stat->cont[port-1].on_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u on, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				StrFlash(str, 42); //KM%c%u on, err=%u, t1=%lu, t2=%lu
				sprintf(mess, str, phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}

	//KM2_off
	if (!phase->error) {
		port = 2;
		err = Phase_cont_off(phase, port, &t1, &t2);
		
		phase_stat->cont[port-1].of_t1 = t1;
		phase_stat->cont[port-1].of_t2 = t2;
		
		if (debug_mess) {
			sprintf(mess, "KM%c%u off, err=%u, t1=%u, t2=%u", phase->let, port, err, t1, t2);
			serial_write(mess);
		}
		if (err) {
			/*
			if (debug_error) {
				StrFlash(str, 43); //KM%c%u off, err=%u, t1=%lu, t2=%lu
				sprintf(mess, str, phase->let, port, err, t1, t2);
				serial_write(mess);
			}
			*/
			phase->error = 1;
			task->param[0] = port;
			task->param[1] = err;
		}
	}
	
	//Произошла ошибка
	if (phase->error) {
		//Отключаем
		Phase_port_off(phase, 2);
		Phase_port_off(phase, 3);
		Phase_port_off(phase, 1);
	}
	
	Ports_scan_mode(0);
	Ports_scan_busy(0);
	Ports_in_clr();
	
	return true;
}

//Phase A-----------------------------------------------------------------------------------------------------------------------
bool A_KM_on(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	
	uint8_t  p = 1;    //фаза А
	
	tp_phase_type *phase = Phase_num(p);
	
	/*
	if (!task->iter) {
		sprintf(mess, "A_KM_on start, port=%u, error=%u, name=%s, idx=%u, cnt=%u", port, Task_is_error(task), task->name, task->index, task->cmds_cnt);
		serial_write(mess);
		
		if (Task_is_sub(task)) {
			Task_stat_one(task, mess);
			serial_write(mess);
		}
	}
	*/
	
	if (phase->error) {
		serial_write("Phase error");
		return true;
	}
	

	if (debug_ports) {
		return Contactor_on2(phase, task, port);
	} else {
		return Contactor_on(phase, task, port, try_cnt);
	}
}
bool A_KM_off(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	uint8_t  p = 1;    //фаза А
	
	tp_phase_type *phase = Phase_num(p);
	
	/*
	if (!task->iter) {
		sprintf(mess, "A_KM_off start, port=%u, error=%u, name=%s, idx=%u, cnt=%u", port, Task_is_error(task), task->name, task->index, task->cmds_cnt);
		serial_write(mess);
	}
	*/
	
	if (phase->error) {
		return true;
	}
	
	if (debug_ports) {
		return Contactor_off2(phase, task, port);
	} else {
		return Contactor_off(phase, task, port, try_cnt);
	}
}
bool A_KM_Trans_on(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	
	return KM_Trans_on(task, p);
}
bool A_KM_Trans_off(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	
	return KM_Trans_off(task, p);
}
bool A_KM0_on(tp_task_type *task) {
	
	return A_KM_on(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool A_KM0_off(tp_task_type *task) {
	
	return A_KM_off(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool A_KM1_on(tp_task_type *task) {
	
	return A_KM_on(task, 1, CONTACTOR_FAIL_TRY);
}
bool A_KM1_off(tp_task_type *task) {
	
	return A_KM_off(task, 1, CONTACTOR_FAIL_TRY);
}
bool A_KM2_on(tp_task_type *task) {
	
	//Допускаются повторные на включение
	return A_KM_on(task, 2, CONTACTOR_FAIL_TRY);
}
bool A_KM2_off(tp_task_type *task) {
	
	//Не допускается повторных срабатываний
	return A_KM_off(task, 2, 0);
}
bool A_KM3_on(tp_task_type *task) {

	//Допускаются повторные на включение
	return A_KM_on(task, 3, CONTACTOR_FAIL_TRY);
}
bool A_KM3_off(tp_task_type *task) {
	
	//Допускаются повторные на включение
	return A_KM_off(task, 3, CONTACTOR_FAIL_TRY);
}
bool A_KM4_on(tp_task_type *task) {
	
	return A_KM_on(task, 4, CONTACTOR_FAIL_TRY);
}
bool A_KM4_off(tp_task_type *task) {
	
	return A_KM_off(task, 4, CONTACTOR_FAIL_TRY);
}
bool A_KM5_on(tp_task_type *task) {
	
	return A_KM_on(task, 5, CONTACTOR_FAIL_TRY);
}
bool A_KM5_off(tp_task_type *task) {
	
	return A_KM_off(task, 5, CONTACTOR_FAIL_TRY);
}
bool A_KM6_on(tp_task_type *task) {
	
	return A_KM_on(task, 6, CONTACTOR_FAIL_TRY);
}
bool A_KM6_off(tp_task_type *task) {
	
	return A_KM_off(task, 6, CONTACTOR_FAIL_TRY);
}
bool A_KM7_on(tp_task_type *task) {
	
	return A_KM_on(task, 7, CONTACTOR_FAIL_TRY);
}
bool A_KM7_off(tp_task_type *task) {
	
	return A_KM_off(task, 7, CONTACTOR_FAIL_TRY);
}
bool A_KM8_on(tp_task_type *task) {
	
	return A_KM_on(task, 8, CONTACTOR_FAIL_TRY);
}
bool A_KM8_off(tp_task_type *task) {
	
	return A_KM_off(task, 8, CONTACTOR_FAIL_TRY);
}
bool A_KM9_on(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_on2(phase, task, port);
}
bool A_KM9_off(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_off2(phase, task, port);
}

bool A_KM9_check(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
		
	if (!auto_boost) {return true;}
		
	if (phase->next_step < 0) {
		return A_KM9_on(task);
	} else {
		return A_KM9_off(task);
	}
}

bool A_delay_5sec(tp_task_type *task) {
	
	uint8_t  p = 1;    //фаза А
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
	
	if (debug_mess && !task->iter) {
		sprintf(mess, "%c_Wait 5 sec", phase->let);
		serial_write(mess);
	}
	task->wait = 5000;
	return true;
}


bool A_TransOn(tp_task_type *task) {
	
	return task_TransOn(task, 1);
}
bool A_TransOff(tp_task_type *task) {
	
	return task_TransOff(task, 1);
}
bool A_begin(tp_task_type *task) {
	
	return task_step_begin(task, 1);
}
bool A_finish(tp_task_type *task) {
	
	return task_step_finish(task, 1);
}
bool A_FTrans(tp_task_type *task) {
	
	return task_force_trans(task, 1);
}


bool A_rep_start(tp_task_type *task) {
	
	return task_repair_begin(task, 1);
}
bool A_rep_end(tp_task_type *task) {
	
	return task_repair_finish(task, 1);
}
bool B_rep_start(tp_task_type *task) {
	
	return task_repair_begin(task, 2);
}
bool B_rep_end(tp_task_type *task) {
	
	return task_repair_finish(task, 2);
}
bool C_rep_start(tp_task_type *task) {
	
	return task_repair_begin(task, 3);
}
bool C_rep_end(tp_task_type *task) {
	
	return task_repair_finish(task, 3);
}

//Phase B-----------------------------------------------------------------------------------------------------------------------
bool B_KM_on(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	uint8_t  p = 2;    //фаза B
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	if (debug_ports) {
		return Contactor_on2(phase, task, port);
	} else {
		return Contactor_on(phase, task, port, try_cnt);
	}
}
bool B_KM_off(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	uint8_t  p = 2;    //фаза B
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	if (debug_ports) {
		return Contactor_off2(phase, task, port);
	} else {
		return Contactor_off(phase, task, port, try_cnt);
	}
}
bool B_KM_Trans_on(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	
	return KM_Trans_on(task, p);
}
bool B_KM_Trans_off(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	
	return KM_Trans_off(task, p);
}
bool B_KM0_on(tp_task_type *task) {
	
	return B_KM_on(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool B_KM0_off(tp_task_type *task) {
	
	return B_KM_off(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool B_KM1_on(tp_task_type *task) {
	
	return B_KM_on(task, 1, CONTACTOR_FAIL_TRY);
}
bool B_KM1_off(tp_task_type *task) {
	
	return B_KM_off(task, 1, CONTACTOR_FAIL_TRY);
}
bool B_KM2_on(tp_task_type *task) {
	
	//Допускаются повторные на включение
	return B_KM_on(task, 2, CONTACTOR_FAIL_TRY);
}
bool B_KM2_off(tp_task_type *task) {
	
	//Не допускается повторных срабатываний
	return B_KM_off(task, 2, 0);
}
bool B_KM3_on(tp_task_type *task) {

	//Допускаются повторные на включение
	return B_KM_on(task, 3, CONTACTOR_FAIL_TRY);
}
bool B_KM3_off(tp_task_type *task) {
	
	//Допускаются повторные на включение
	return B_KM_off(task, 3, CONTACTOR_FAIL_TRY);
}
bool B_KM4_on(tp_task_type *task) {
	
	return B_KM_on(task, 4, CONTACTOR_FAIL_TRY);
}
bool B_KM4_off(tp_task_type *task) {
	
	return B_KM_off(task, 4, CONTACTOR_FAIL_TRY);
}
bool B_KM5_on(tp_task_type *task) {
	
	return B_KM_on(task, 5, CONTACTOR_FAIL_TRY);
}
bool B_KM5_off(tp_task_type *task) {
	
	return B_KM_off(task, 5, CONTACTOR_FAIL_TRY);
}
bool B_KM6_on(tp_task_type *task) {
	
	return B_KM_on(task, 6, CONTACTOR_FAIL_TRY);
}
bool B_KM6_off(tp_task_type *task) {
	
	return B_KM_off(task, 6, CONTACTOR_FAIL_TRY);
}
bool B_KM7_on(tp_task_type *task) {
	
	return B_KM_on(task, 7, CONTACTOR_FAIL_TRY);
}
bool B_KM7_off(tp_task_type *task) {
	
	return B_KM_off(task, 7, CONTACTOR_FAIL_TRY);
}
bool B_KM8_on(tp_task_type *task) {
	
	return B_KM_on(task, 8, CONTACTOR_FAIL_TRY);
}
bool B_KM8_off(tp_task_type *task) {
	
	return B_KM_off(task, 8, CONTACTOR_FAIL_TRY);
}
bool B_KM9_on(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_on2(phase, task, port);
}
bool B_KM9_off(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_off2(phase, task, port);
}

bool B_KM9_check(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
	
	if (!auto_boost) {return true;}
	
	if (phase->next_step < 0) {
		return B_KM9_on(task);
	} else {
		return B_KM9_off(task);
	}
}

bool B_delay_5sec(tp_task_type *task) {
	
	uint8_t  p = 2;    //фаза B
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
	
	if (debug_mess && !task->iter) {
		sprintf(mess, "%c_Wait 5 sec", phase->let);
		serial_write(mess);
	}
	task->wait = 5000;
	return true;
}


bool B_TransOn(tp_task_type *task) {
	
	return task_TransOn(task, 2);
}
bool B_TransOff(tp_task_type *task) {
	
	return task_TransOff(task, 2);
}
bool B_begin(tp_task_type *task) {
	
	return task_step_begin(task, 2);
}
bool B_finish(tp_task_type *task) {
	
	return task_step_finish(task, 2);
}
bool B_FTrans(tp_task_type *task) {
	
	return task_force_trans(task, 2);
}

//Phase C-----------------------------------------------------------------------------------------------------------------------
bool C_KM_on(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	uint8_t  p = 3;    //фаза C
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	if (debug_ports) {
		return Contactor_on2(phase, task, port);
	} else {
		return Contactor_on(phase, task, port, try_cnt);
	}
}
bool C_KM_off(tp_task_type *task, uint16_t port, uint16_t try_cnt) {
	
	uint8_t  p = 3;    //фаза C
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	if (debug_ports) {
		return Contactor_off2(phase, task, port);
	} else {
		return Contactor_off(phase, task, port, try_cnt);
	}
}
bool C_KM_Trans_on(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	
	return KM_Trans_on(task, p);
}
bool C_KM_Trans_off(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	
	return KM_Trans_off(task, p);
}
bool C_KM0_on(tp_task_type *task) {
	
	return C_KM_on(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool C_KM0_off(tp_task_type *task) {
	
	return C_KM_off(task, task->param[0], CONTACTOR_FAIL_TRY);
}
bool C_KM1_on(tp_task_type *task) {
	
	return C_KM_on(task, 1, CONTACTOR_FAIL_TRY);
}
bool C_KM1_off(tp_task_type *task) {
	
	return C_KM_off(task, 1, CONTACTOR_FAIL_TRY);
}
bool C_KM2_on(tp_task_type *task) {
	
	//Допускаются повторные на включение
	return C_KM_on(task, 2, CONTACTOR_FAIL_TRY);
}
bool C_KM2_off(tp_task_type *task) {
	
	//Не допускается повторных срабатываний
	return C_KM_off(task, 2, 0);
}
bool C_KM3_on(tp_task_type *task) {

	//Допускаются повторные на включение
	return C_KM_on(task, 3, CONTACTOR_FAIL_TRY);
}
bool C_KM3_off(tp_task_type *task) {
	
	return C_KM_off(task, 3, CONTACTOR_FAIL_TRY);
}
bool C_KM4_on(tp_task_type *task) {
	
	return C_KM_on(task, 4, CONTACTOR_FAIL_TRY);
}
bool C_KM4_off(tp_task_type *task) {
	
	return C_KM_off(task, 4, CONTACTOR_FAIL_TRY);
}
bool C_KM5_on(tp_task_type *task) {
	
	return C_KM_on(task, 5, CONTACTOR_FAIL_TRY);
}
bool C_KM5_off(tp_task_type *task) {
	
	return C_KM_off(task, 5, CONTACTOR_FAIL_TRY);
}
bool C_KM6_on(tp_task_type *task) {
	
	return C_KM_on(task, 6, CONTACTOR_FAIL_TRY);
}
bool C_KM6_off(tp_task_type *task) {
	
	return C_KM_off(task, 6, CONTACTOR_FAIL_TRY);
}
bool C_KM7_on(tp_task_type *task) {
	
	return C_KM_on(task, 7, CONTACTOR_FAIL_TRY);
}
bool C_KM7_off(tp_task_type *task) {
	
	return C_KM_off(task, 7, CONTACTOR_FAIL_TRY);
}
bool C_KM8_on(tp_task_type *task) {
	
	return C_KM_on(task, 8, CONTACTOR_FAIL_TRY);
}
bool C_KM8_off(tp_task_type *task) {
	
	return C_KM_off(task, 8, CONTACTOR_FAIL_TRY);
}
bool C_KM9_on(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_on2(phase, task, port);
}
bool C_KM9_off(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	uint16_t port = 9;
	
	tp_phase_type *phase = Phase_num(p);
	
	if (phase->error) {return true;}
	
	//Нет обратной связи
	return Contactor_off2(phase, task, port);
}

bool C_KM9_check(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
	
	if (!auto_boost) {return true;}
	
	if (phase->next_step < 0) {
		return C_KM9_on(task);
	} else {
		return C_KM9_off(task);
	}
}

bool C_delay_5sec(tp_task_type *task) {
	
	uint8_t  p = 3;    //фаза C
	tp_phase_type *phase = Phase_num(p);
	
	phase_save_state(phase->num, task);
	
	if (phase->error) {return true;}
	
	if (debug_mess && !task->iter) {
		sprintf(mess, "%c_Wait 5 sec", phase->let);
		serial_write(mess);
	}
	task->wait = 5000;
	return true;
}


bool C_TransOn(tp_task_type *task) {
	
	return task_TransOn(task, 3);
}
bool C_TransOff(tp_task_type *task) {
	
	return task_TransOff(task, 3);
}
bool C_begin(tp_task_type *task) {
	
	return task_step_begin(task, 3);
}
bool C_finish(tp_task_type *task) {
	
	return task_step_finish(task, 3);
}
bool C_FTrans(tp_task_type *task) {
	
	return task_force_trans(task, 3);
}

bool AdeRms(tp_task_type *task) {
	bool ret = false;
	
	task->wait = 1;
	
	if (!task->iter) {
		task->param[0] = 0;
		ADE7758_resetStatus(); // Clear all interrupts
		return ret;
	}
	
	uint32_t st = ADE7758_getStatus();
	uint8_t   p = 0;
	
	if (st & ZXA) {
		p = 1;
		if (!(task->param[0] >> p) & 1) {
			ADE7758_rms_add(p);
			task->param[0] |= 1 << p;
		}
	}
	if (st & ZXB) {
		p = 2;
		if (!(task->param[0] >> p) & 1) {
			ADE7758_rms_add(p);
			task->param[0] |= 1 << p;
		}
	}
	if (st & ZXC) {
		p = 3;
		if (!(task->param[0] >> p) & 1) {
			ADE7758_rms_add(p);
			task->param[0] |= 1 << p;
		}
	}
	
	if (task->param[0] == 0x0E) { //ZXA & ZXB & ZXC
		ret = true;
	
	} else if (task->iter > 20) { //max 20ms
		
		//Читаем RMS вне зависимости был ли переход или нет
		for (uint8_t n = 1; n <= 3; n++) {
			if (!(task->param[0] >> n) & 1) {
				ADE7758_rms_add(n);
			}
		}
		
		ret = true;
	}
	
	return ret;
	
}

bool AdePower(tp_task_type *task) {
	
	bool ret = false;
	
	task->wait = 1;
	
	//20*15 = 300ms;
	
	if (!task->iter) {
		
		ADE7758_startPotLine(task->param[0], 20);
		
	} else {
		if (ADE7758_getStatus() & LENERGY) {
			
			ADE7758_setCos(task->param[0]);
			
			ret = true;
			
		} else if (task->iter >= 300) {
			ret = true;
		}
	}
	
	return ret;
}

bool NetReset(tp_task_type *task) {
	bool ret = false;
	
	task->wait = 1;
	
	if (!task->iter) {
		Regulator_dt_net_reset();
		HAL_GPIO_WritePin(PIN_NET_POWER_PORT, PIN_NET_POWER_PIN, 0);
		
		task->wait = 500;
		return ret;
	}
	if (task->iter==1) {
		HAL_GPIO_WritePin(PIN_NET_POWER_PORT, PIN_NET_POWER_PIN, 1);
	}
	if (task->iter==2) {
		ADE7758_init();
		ret = true;
	}
	
	return ret;
}


bool phase_read_state(uint8_t p) {
	
	tp_phase_type *phase = Phase_num(p);
	
	uint16_t addr_state  = 0;
	uint16_t addr_shift  = 0;
	
	//Смещение адреса
	uint16_t shift    = eeprom_read_uint16(addr_shift);
	if (shift == zval_16) {
		shift = 0;
	}
	
	//Адреса
	if (phase->num == 1) {
		
		addr_state  = EE_STATE_PHASE_A;
		addr_shift  = EE_SHIFT_PHASE_A;
		
	} else if (phase->num == 2) {
		
		addr_state  = EE_STATE_PHASE_B;
		addr_shift  = EE_SHIFT_PHASE_B;
		
	} else {
		
		addr_state  = EE_STATE_PHASE_C;
		addr_shift  = EE_SHIFT_PHASE_C;
	}
	
	//Читаем состояние
	tp_phase_type_save state;
	eeprom_read_buffer(addr_state + shift, (void *)&state, sizeof(state));
	
	//sprintf(mess, "Addr=%u; shift=%u; size=%u; counter=%u", addr_state, shift, sizeof(state), state.counter);
	//serial_write(mess);
	
	/*
	sprintf(mess, "Run=%u; %lu; %lu", state.running, state.dt_start, state.dt_end);
	serial_write(mess);
	
	sprintf(mess, "Error=%02X; %lu", state.errnum, state.dt_error);
	serial_write(mess);
	
	sprintf(mess, "Step=%d; before=%d; next=%d; volt=%u", state.step, state.step_before, state.step_next, state.volt_before);
	serial_write(mess);
	
	sprintf(mess, "Task n=%u; idx=%u; Sub n=%u; idx=%u", state.task_num, state.task_index, state.sub_num, state.sub_index);
	serial_write(mess);
	*/
	
	phase->is_running  = state.running;
	phase->errnum      = state.errnum;
	phase->errcont     = state.errcont;
	phase->dt_start    = state.dt_start;
	phase->dt_end      = state.dt_end;
	phase->dt_error    = state.dt_error;
	phase->dt_repair   = state.dt_repair;
	phase->step        = state.step;
	phase->step_before = state.step_before;
	phase->next_step   = state.step_next;
	phase->volt_before = state.volt_before;
	
	phase->error       = phase->errnum ? 1 : 0;
	
	if (phase->error) {
		//Сброс контакторов, на всякий случай (если вылет случился во время процедуры ошибки)
		for (uint8_t n = 1; n <= 9; n++) {
			Port_clr(phase->num, n);
		}
		Ports_renew();
		
		return true;
	}
	
	if (!phase->is_running) {
		//нет регулирования
		return false;
	}
		
	tp_task_type *task = Task_by_num(state.task_num);
	if (task == NULL) {
		//serial_write("Task not found");
		return false;
	}
	
	Task_start_tp(task);
	task->index = state.task_index;
	
	if (state.sub_num!=0xFF) {
		//подпрограмма
		tp_task_type *sub = Task_by_num(state.sub_num);
		
		if (sub == NULL) {
			//serial_write("Sub not found");
			return false;
		}
		
		Task_start_sub_tp(sub, task);
		sub->index = state.sub_index;
		
		task->iter = 1;
	}
	return true;
}

void phase_save_state(uint8_t p, tp_task_type *task) {
	
	uint16_t addr_state  = 0;
	uint16_t addr_shift  = 0;
	uint16_t addr_max    = 0;
	
	//uint16_t counter  = 0;
	
	tp_phase_type *phase = Phase_num(p);
	
	//Адреса
	if (phase->num == 1) {
		
		addr_state  = EE_STATE_PHASE_A;
		addr_shift  = EE_SHIFT_PHASE_A;
		addr_max    = EE_STATE_PHASE_B;
	
	} else if (phase->num == 2) {
		
		addr_state  = EE_STATE_PHASE_B;
		addr_shift  = EE_SHIFT_PHASE_B;
		addr_max    = EE_STATE_PHASE_C;
	
	} else {
	
		addr_state  = EE_STATE_PHASE_C;
		addr_shift  = EE_SHIFT_PHASE_C;
		addr_max    = EE_STATE_PHASE_C+256;
	}
	
	//Смещение адреса
	uint16_t shift  = eeprom_read_uint16(addr_shift);
	if (shift == zval_16) {shift = 0;}
		
	//Читаем состояние
	tp_phase_type_save state;
	eeprom_read_buffer(addr_state + shift, (void *)&state, sizeof(state));
	
	//Счетчик
	if (state.counter == zval_16) {
		state.counter = 0; //Записи еще не было
	} else {
		state.counter ++;
	}
	
	//Заполняем состояние
	state.running       = phase->is_running;
	state.errnum        = phase->errnum;
	state.errcont       = phase->errcont;
	state.dt_start      = phase->dt_start;
	state.dt_end        = phase->dt_end;
	state.dt_error      = phase->dt_error;
	state.dt_repair     = phase->dt_repair;
	state.step          = phase->step;
	state.step_before   = phase->step_before;
	state.step_next     = phase->next_step;
	state.volt_before   = phase->volt_before;
	state.task_num      = task->n;
	state.task_index    = task->index;
	state.sub_num       = 0xFF;
	state.sub_index     = 0;
	
	if (Task_is_sub(task->n)) { //подпрограмма
		
		tp_task_type *task_parent = Task_get_parent(task->n);
		if (task_parent!=NULL) {
			state.task_num    = task_parent->n;
			state.task_index  = task_parent->index;
			
			state.sub_num     = task->n;
			state.sub_index   = task->index;
		}
	}
	
	//Смещение, если счетчик переполнен
	/*
	if (state.counter >= 65500) {
		
		state.counter = 0;
		uint16_t add_shift  = sizeof(state);
		
		if (addr_state + shift + add_shift <= addr_max) {
			
			shift += add_shift;
			if (shift>=320) { //не более 10 раз
				shift = 0;
			}
			eeprom_write_uint16(addr_shift, shift);
		}
	}
	*/
	
	//Пишем состояние
	eeprom_write_buffer(addr_state + shift, (void *)&state, sizeof(state));
}