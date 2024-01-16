/*
 * TaskList.h
 *
 * Created: 30.01.2018 9:33:27
 *  Author: Admin
 */ 


#ifndef TASKLIST_H_
#define TASKLIST_H_

#include <string.h>

#include "Tasks.h"
#include "main.h"
#include "modem.h"
#include "stm32f1xx_hal_i2c.h"
#include "Phase.h"

extern char     modem_imei[];
extern char     modem_csq[];
extern char     modem_lat[];
extern char     modem_lon[];
extern char     modem_phone[];
extern char     modem_ip[];

extern bool     MODEM_GPRS;
extern bool     MODEM_ALIVE;
extern bool     MODEM_CONNECTED;

extern char       server_ip[];
extern uint16_t   server_port;

extern char lbl_fail[];
extern char lbl_ok[];

bool Modem_start_power(tp_task_type *task);
bool Modem_start_gprs(tp_task_type *task);
bool Modem_start_connect(tp_task_type *task);
bool Modem_start_sms(tp_task_type *task);
bool M_blink(tp_task_type *task);

bool M_sub_power(tp_task_type *task);
bool M_sub_reset(tp_task_type *task);
bool M_sub_at(tp_task_type *task);
bool M_sub_echo(tp_task_type *task);
bool M_sub_sms(tp_task_type *task);
bool M_sub_creg(tp_task_type *task);
bool M_sub_imei(tp_task_type *task);
bool M_sub_csq(tp_task_type *task);
bool M_sub_check(tp_task_type *task);

bool M_sub_gprs_exist(tp_task_type *task);
bool M_sub_gprs_cont(tp_task_type *task);
bool M_sub_gprs_connect(tp_task_type *task);
bool M_sub_gprs_IP(tp_task_type *task);

bool M_sub_srv_set_1(tp_task_type *task);
bool M_sub_srv_set_2(tp_task_type *task);
bool M_sub_tcp_start(tp_task_type *task);

bool M_srv_start(tp_task_type *task);
bool M_srv_send(tp_task_type *task);
bool M_srv_copy(tp_task_type *task);

bool M_sms_start(tp_task_type *task);
bool M_sms_send(tp_task_type *task);

bool M_sub_sms(tp_task_type *task);
bool M_sub_sms_scr(tp_task_type *task);
bool M_sub_sms_code(tp_task_type *task);
bool M_sub_sms_del(tp_task_type *task);

bool M_call_check(tp_task_type *task);
bool M_call_init(tp_task_type *task);
bool M_call_start(tp_task_type *task);
bool M_call_end(tp_task_type *task);

bool M_AT_send(tp_task_type *task, uint8_t n);
bool M_AT_command(tp_task_type *task, uint16_t num, char* response, uint16_t max, uint16_t timeOut, uint8_t retry);
bool M_at_resp(tp_task_type *task, uint8_t ncmd, uint8_t nresp);

bool P_blink(tp_task_type *task);

//Tasks-----------------------------------------------------------------------------------------------------------------------
bool Net_get  (tp_task_type *task);
bool Net_save (tp_task_type *task);
bool Net_reset (tp_task_type *task);
bool Delay_start (tp_task_type *task);

void task_auto_volt(tp_phase_type *phase, uint16_t volt);
bool task_step_begin(tp_task_type *task, uint8_t p);
bool task_step_finish(tp_task_type *task, uint8_t p);
bool task_force_trans(tp_task_type *task, uint8_t p);
bool task_TransOn(tp_task_type *task, uint8_t p);
bool task_TransOff(tp_task_type *task, uint8_t p);
bool task_repair_begin(tp_task_type *task, uint8_t p);
bool task_repair_finish(tp_task_type *task, uint8_t p);


//Phase A-----------------------------------------------------------------------------------------------------------------------
bool A_KM0_on (tp_task_type *task);
bool A_KM0_off (tp_task_type *task);

bool A_KM1_on (tp_task_type *task);
bool A_KM1_off (tp_task_type *task);

bool A_KM2_on (tp_task_type *task);
bool A_KM2_off (tp_task_type *task);

bool A_KM3_on (tp_task_type *task);
bool A_KM3_off (tp_task_type *task);

bool A_KM4_on (tp_task_type *task);
bool A_KM4_off (tp_task_type *task);

bool A_KM5_on (tp_task_type *task);
bool A_KM5_off (tp_task_type *task);

bool A_KM6_on (tp_task_type *task);
bool A_KM6_off (tp_task_type *task);

bool A_KM7_on (tp_task_type *task);
bool A_KM7_off (tp_task_type *task);

bool A_KM8_on (tp_task_type *task);
bool A_KM8_off (tp_task_type *task);

bool A_KM9_check (tp_task_type *task);
bool A_KM9_on (tp_task_type *task);
bool A_KM9_off (tp_task_type *task);

bool A_delay_5sec (tp_task_type *task);

bool A_TransOn (tp_task_type *task);
bool A_TransOff (tp_task_type *task);

bool A_begin  (tp_task_type *task);
bool A_finish (tp_task_type *task);

bool A_rep_start  (tp_task_type *task);
bool A_rep_end (tp_task_type *task);
bool A_FTrans(tp_task_type *task);

bool A_KM_Trans_on(tp_task_type *task);
bool A_KM_Trans_off(tp_task_type *task);
//-----------------------------------------------------------------------------------------------------------------------

//Phase B----------------------------------------------------------------------------------------------------------------
bool B_KM0_on (tp_task_type *task);
bool B_KM0_off (tp_task_type *task);

bool B_KM1_on (tp_task_type *task);
bool B_KM1_off (tp_task_type *task);

bool B_KM2_on (tp_task_type *task);
bool B_KM2_off (tp_task_type *task);

bool B_KM3_on (tp_task_type *task);
bool B_KM3_off (tp_task_type *task);

bool B_KM4_on (tp_task_type *task);
bool B_KM4_off (tp_task_type *task);

bool B_KM5_on (tp_task_type *task);
bool B_KM5_off (tp_task_type *task);

bool B_KM6_on (tp_task_type *task);
bool B_KM6_off (tp_task_type *task);

bool B_KM7_on (tp_task_type *task);
bool B_KM7_off (tp_task_type *task);

bool B_KM8_on (tp_task_type *task);
bool B_KM8_off (tp_task_type *task);

bool B_KM9_check (tp_task_type *task);
bool B_KM9_on (tp_task_type *task);
bool B_KM9_off (tp_task_type *task);

bool B_delay_5sec (tp_task_type *task);

bool B_TransOn (tp_task_type *task);
bool B_TransOff (tp_task_type *task);

bool B_begin  (tp_task_type *task);
bool B_finish (tp_task_type *task);

bool B_rep_start  (tp_task_type *task);
bool B_rep_end (tp_task_type *task);
bool B_FTrans(tp_task_type *task);

bool B_KM_Trans_on(tp_task_type *task);
bool B_KM_Trans_off(tp_task_type *task);
//-----------------------------------------------------------------------------------------------------------------------

//Phase C-----------------------------------------------------------------------------------------------------------------------
bool C_KM0_on (tp_task_type *task);
bool C_KM0_off (tp_task_type *task);

bool C_KM1_on (tp_task_type *task);
bool C_KM1_off (tp_task_type *task);

bool C_KM2_on (tp_task_type *task);
bool C_KM2_off (tp_task_type *task);

bool C_KM3_on (tp_task_type *task);
bool C_KM3_off (tp_task_type *task);

bool C_KM4_on (tp_task_type *task);
bool C_KM4_off (tp_task_type *task);

bool C_KM5_on (tp_task_type *task);
bool C_KM5_off (tp_task_type *task);

bool C_KM6_on (tp_task_type *task);
bool C_KM6_off (tp_task_type *task);

bool C_KM7_on (tp_task_type *task);
bool C_KM7_off (tp_task_type *task);

bool C_KM8_on (tp_task_type *task);
bool C_KM8_off (tp_task_type *task);

bool C_KM9_check (tp_task_type *task);
bool C_KM9_on (tp_task_type *task);
bool C_KM9_off (tp_task_type *task);

bool C_delay_5sec (tp_task_type *task);

bool C_TransOn (tp_task_type *task);
bool C_TransOff (tp_task_type *task);

bool C_begin  (tp_task_type *task);
bool C_finish (tp_task_type *task);

bool C_rep_start  (tp_task_type *task);
bool C_rep_end (tp_task_type *task);
bool C_FTrans(tp_task_type *task);

bool C_KM_Trans_on(tp_task_type *task);
bool C_KM_Trans_off(tp_task_type *task);
//-----------------------------------------------------------------------------------------------------------------------

bool AdeRms(tp_task_type *task);
bool AdePower(tp_task_type *task);

bool NetReset(tp_task_type *task);

void phase_save_state(uint8_t p, tp_task_type *task);

#endif /* TASKLIST_H_ */
