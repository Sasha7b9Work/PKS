/*
 * ADE7758.c
 *
 * Created: 02.05.2018 10:49:05
 *  Author: Hyper
 */ 
#include "ADE7758.h"

//float   FREQ_CAL = 16.00;

float   VRMS_CAL = 1;
float   PWR_CAL  = 1;

float   IRMS_A = 1;
float   IRMS_B = 1;
float   IRMS_C = 1;

tp_rms_type rms_values;

void ADE7758_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	//Disable CS
	HAL_GPIO_WritePin(PIN_NET_CS_PORT, PIN_NET_CS_PIN, 1);
	
	GPIO_InitStruct.Pin = PIN_NET_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_NET_CS_PORT, &GPIO_InitStruct);
	
	ADE7758_enableChip();
	ADE7758_config();
	ADE7758_disableChip();
}

void ADE7758_setSPI(void) {
	
	//Initialize the SPI module
	if (hspi1.Init.BaudRatePrescaler != ADE_SPI_PRESCALER) {
	
		/*
		Режим CPOL CPHA
		0     0    0
		1     0    1
		2     1    0
		3     1    1
		*/
		
		/* SPI1 parameter configuration*/
		hspi1.Instance = SPI1;
		hspi1.Init.Mode = SPI_MODE_MASTER;
		hspi1.Init.Direction = SPI_DIRECTION_2LINES;
		hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;   //SPI_POLARITY_LOW соответствует CPOL = 0 и SPI_POLARITY_HIGH соответствует CPOL = 1
		hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;        //SPI_PHASE_1EDGE соответствует  CPHA = 0 и SPI_PHASE_2EDGE   соответствует CPHA = 1
		hspi1.Init.NSS = SPI_NSS_SOFT;
		hspi1.Init.BaudRatePrescaler = ADE_SPI_PRESCALER;
		hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
		hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi1.Init.CRCPolynomial = 10;
		if (HAL_SPI_Init(&hspi1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

void ADE7758_config(void) {
	
	ADE7758_setLcycMode(0);
	ADE7758_gainSetup(INTEGRATOR_OFF, FULLSCALESELECT_0_25V, GAIN_1, GAIN_1);
	ADE7758_setupDivs(2,2,2);
	ADE7758_setMaskInterrupts(1<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11); //ZXA, ZXB, ZCX
}

void ADE7758_closeSPI(void) {
	
	HAL_SPI_DeInit(&hspi1);
}


void ADE7758_delay(void) {
	delay_micros(3);
	//delay_micros(50);
}

void ADE7758_enableChip(void) {
	
	ADE7758_setSPI();
	
	HAL_GPIO_WritePin(PIN_NET_CS_PORT, PIN_NET_CS_PIN, 0);
	
	ADE7758_delay();
}
void ADE7758_disableChip(void) {
	HAL_GPIO_WritePin(PIN_NET_CS_PORT, PIN_NET_CS_PIN, 1);
	
	//ADE7758_closeSPI();
	
	ADE7758_delay();
}

void ADE7758_write8(uint8_t reg, uint8_t data) {

	// 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
	// For Write -> DB7 = 1  / For Read -> DB7 = 0
	reg |= WRITE;
	
	uint8_t tx_data[] = {reg, data};

	HAL_SPI_Transmit(&hspi1, tx_data, sizeof(tx_data), HAL_MAX_DELAY);
}

void ADE7758_write16(uint8_t reg, uint16_t data) {

	// 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
	// For Write -> DB7 = 1  / For Read -> DB7 = 0
	reg |= WRITE;
	
	//data send, MSB first
	uint8_t tx_data[] = {reg, data >> 8, data};

	HAL_SPI_Transmit(&hspi1, tx_data, sizeof(tx_data), HAL_MAX_DELAY);
}

void ADE7758_write24(uint8_t reg, uint32_t data) {

	// 8th bit (DB7) of the register address controls the Read/Write mode (Refer to spec page 55 table 13)
	// For Write -> DB7 = 1  / For Read -> DB7 = 0
	reg |= WRITE;

	//data send, MSB first
	uint8_t tx_data[] = {reg, data >> 16, data >> 8, data};

	HAL_SPI_Transmit(&hspi1, tx_data, sizeof(tx_data), HAL_MAX_DELAY);
}

uint8_t ADE7758_read8(uint8_t reg) {
	
	HAL_StatusTypeDef hal_status;
	
	uint8_t tx_data[] = {reg, 0}; // dummy byte for response
	uint8_t rx_data[2];
	
	uint8_t ret = 0;           
	
	hal_status = HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, HAL_MAX_DELAY);
	
	if (hal_status == HAL_OK)
	{
			ret = rx_data[1];    // response is in the second byte
	}
	
	return ret;
}

uint16_t ADE7758_read16(uint8_t reg) {
	
	uint16_t ret = 0;
	
	HAL_StatusTypeDef hal_status;
	
	uint8_t tx_data[] = {reg, 0, 0};
	uint8_t rx_data[3];
	
	hal_status = HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 3, HAL_MAX_DELAY);
	
	if (hal_status == HAL_OK)
	{
			//MSB first
			ret = (uint16_t)rx_data[1] << 8 | (uint16_t)rx_data[2];
	}
	
	return ret;
}

uint32_t ADE7758_read24(uint8_t reg) {
	
	HAL_StatusTypeDef hal_status;
	
	uint8_t  tx_data[] = {reg, 0, 0, 0};
	uint8_t  rx_data[4];
	uint32_t ret = 0;
	
	hal_status = HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 4, HAL_MAX_DELAY);
	
	if (hal_status == HAL_OK)
	{
			//MSB first
			ret = (uint32_t)rx_data[1] << 16 | (uint32_t)rx_data[2] << 8 | (uint32_t)rx_data[3];
	}
	
	return ret;
}

/*****************************

      public functions

*****************************/

uint8_t ADE7758_getVersion() {
	ADE7758_enableChip();
	
	uint8_t ret = ADE7758_read8(AVERSION);
	
	ADE7758_disableChip();
	
	return ret;
}

void ADE7758_setOpMode(uint8_t m) {
	ADE7758_enableChip();
	
	ADE7758_write8(OPMODE, m);
	
	ADE7758_disableChip();
}

uint8_t ADE7758_getOpMode(void) {
	ADE7758_enableChip();
	
	uint8_t ret = ADE7758_read8(OPMODE);
	
	ADE7758_disableChip();
	
	return ret;
}

/** === setLcycMode / getLcycMode ===

LINE CYCLE ACCUMULATION MODE REGISTER (0x17), DEFAULT 0x78 = 1111000
The functionalities involved the line-cycle accumulation mode in the ADE7758 are defined by writing to the LCYCMODE register.

Bit Location	Bit Mnemonic	Default Value		Description

0				LWATT			0					Setting this bit places the watt-hour accumulation registers (AWATTHR, BWATTHR, and CWATTHR registers) into line-cycle accumulation mode.
1				LVAR			0					Setting this bit places the VAR-hour accumulation registers (AVARHR, BVARHR, and CVARHR registers) into line-cycle accumulation mode.
2				LVA				0					Setting this bit places the VA-hour accumulation registers (AVAHR, BVAHR, and CVAHR registers) into line-cycle accumulation mode.
3 to 5		    ZXSEL			7					These bits select the phases used for counting the number of zero crossings in the line-cycle accumulation mode. Bit 3, Bit 4, and Bit 5 select Phase A, Phase B, and Phase C, respectively. More than one phase can be selected for the zero-crossing detection, and the accumulation time is shortened accordingly.
6				RSTREAD			1					Setting this bit enables the read-with-reset for all the WATTHR, VARHR, and VAHR registers for all threephases, that is, a read to those registers resets the registers to 0 after the content of the registers have been read. This bit should be set to Logic 0 when the LWATT, LVAR, or LVA bits are set to Logic 1.
7				FREQSEL			0					Setting this bit causes the FREQ (0x10) register to display the period, instead of the frequency of the line input.
*/

//inner
void ADE7758_setLcycMode(uint8_t m) {
	
	ADE7758_write8(LCYCMODE, m);
}

//not used
uint8_t ADE7758_getLcycMode(void) {
	
	return ADE7758_read8(LCYCMODE);
}



/** === setMMode / getMMode ===
  MEASUREMENT MODE REGISTER (0x14), DEFAULT 0xFC = 11111100
  The configuration of the PERIOD and peak measurements made by the ADE7758 is defined by writing to the MMODE register.
  Table 19 summarizes the functionality of each bit in the MMODE register.

  Bit Location		Bit Mnemonic		Default Value		Description
  0 to 1				FREQSEL				0					These bits are used to select the source of the measurement of the voltage line frequency.
															FREQSEL1		FREQSEL0		Source
															0				0				Phase A
															0				1				Phase B
															1				0				Phase C
															1				1				Reserved
  2 to 4				PEAKSEL				7					These bits select the phases used for the voltage and current peak registers.
															Setting Bit 2 switches the IPEAK and VPEAK registers to hold the absolute values
															of the largest current and voltage waveform (over a fixed number of half-line cycles)
															from Phase A. The number of half-line cycles is determined by the content of the
															LINECYC register. At the end of the LINECYC number of half-line cycles, the content
															of the registers is replaced with the new peak values. Similarly, setting Bit 3 turns
															on the peak detection for Phase B, and Bit 4 for Phase C. Note that if more than one
															bit is set, the VPEAK and IPEAK registers can hold values from two different phases, that is,
															the voltage and current peak are independently processed (see the Peak Current Detection section).
  5 to 7				PKIRQSEL			7					These bits select the phases used for the peak interrupt detection.
															Setting Bit 5 switches on the monitoring of the absolute current and voltage waveform to Phase A.
															Similarly, setting Bit 6 turns on the waveform detection for Phase B, and Bit 7 for Phase C.
															Note that more than one bit can be set for detection on multiple phases.
															If the absolute values of the voltage or current waveform samples in the selected phases exceeds
															the preset level specified in the VPINTLVL or IPINTLVL registers the corresponding bit(s) in the
															STATUS registers are set (see the Peak Current Detection section).

*/

//Not used
void ADE7758_setMMode(uint8_t m) {
	
  ADE7758_write8(MMODE, m);
}

//Not used
uint8_t ADE7758_getMMode(void) {
	
  return ADE7758_read8(MMODE);
}

//inner
void ADE7758_setLineCyc(uint16_t m) {
	
	ADE7758_write16(LINECYC, m);
}


/**	getStatus / resetStatus
INTERRUPT STATUS REGISTER (0x19)/RESET INTERRUPT STATUS REGISTER (0x1A)
The interrupt status register is used to determine the source of an interrupt event.
When an interrupt event occurs in the ADE7758, the corresponding flag in the interrupt status register is set.
The IRQ pin goes active low if the corresponding bit in the interrupt mask register is set.
When the MCU services the interrupt, it must first carry out a read from the interrupt status register to determine the source of the interrupt.
All the interrupts in the interrupt status register stay at their logic high state after an event occurs.
The state of the interrupt bit in the interrupt status register is reset to its default value once the reset interrupt status register is read.

Bit Location	Interrupt Flag		Default Value		Event Description
0				AEHF				0					Indicates that an interrupt was caused by a change in Bit 14 among any one of the three
WATTHR registers, that is, the WATTHR register is half full.
1				REHF				0					Indicates that an interrupt was caused by a change in Bit 14 among any one of the three
VARHR registers, that is, the VARHR register is half full.
2				VAEHF				0					Indicates that an interrupt was caused by a 0 to 1 transition in Bit 15 among any one of the three
VAHR registers, that is, the VAHR register is half full.
3				SAGA				0					Indicates that an interrupt was caused by a SAG on the line voltage of the Phase A.
4				SAGB				0					Indicates that an interrupt was caused by a SAG on the line voltage of the Phase B.
5				SAGC				0					Indicates that an interrupt was caused by a SAG on the line voltage of the Phase C.
6				ZXTOA				0					Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase A.
7				ZXTOB				0					Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase B.
8				ZXTOC				0					Indicates that an interrupt was caused by a missing zero crossing on the line voltage of the Phase C.
9				ZXA					0					Indicates a detection of a rising edge zero crossing in the voltage channel of Phase A.
10				ZXB					0					Indicates a detection of a rising edge zero crossing in the voltage channel of Phase B.
11				ZXC					0					Indicates a detection of a rising edge zero crossing in the voltage channel of Phase C.
12				LENERGY				0					In line energy accumulation, indicates the end of an integration over an integer number of
half- line cycles (LINECYC). See the Calibration section.
13				RESET				1					Indicates that the 5 V power supply is below 4 V. Enables a software reset of the ADE7758
and sets the registers back to their default values. This bit in the STATUS or RSTATUS register
is logic high for only one clock cycle after a reset event.
14				PKV					0					Indicates that an interrupt was caused when the selected voltage input is above the value in the
VPINTLVL register.
15				PKI					0					Indicates that an interrupt was caused when the selected current input is above the value
in the IPINTLVL register.
16				WFSM				0					Indicates that new data is present in the waveform register.
17				REVPAP				0					Indicates that an interrupt was caused by a sign change in the watt calculation among any
one of the phases specified by the TERMSEL bits in the COMPMODE register.
18				REVPRP				0					Indicates that an interrupt was caused by a sign change in the VAR calculation among any
one of the phases specified by the TERMSEL bits in the COMPMODE register.
19				SEQERR				0					Indicates that an interrupt was caused by a zero crossing from Phase A
followed not by the zero crossing of Phase C but by that of Phase B.
*/

//Inner
uint32_t ADE7758_getStatus(void) {
	return ADE7758_read24(ASTATUS);
}

//Inner
uint32_t ADE7758_resetStatus(void) {
	return ADE7758_read24(RSTATUS);
}

/** === gainSetup ===
GAIN REGISTER (0x23)
The PGA configuration of the ADE7758 is defined by writing to the GAIN register.
Table 18 summarizes the functionality of each bit in the GAIN register.

Bit Location		Bit Mnemonic		Default Value		Description
0 to 1				PGA1				0					Current GAIN
PGA1[1:0]			Description
0	0				x1
0	1				x2
1	0				x4
1	1				RESERVED
2					----				RESERVED			Reserved.
3 to 4				SCALE				0					Current input full-scale select
SCALE[1:0]			Description
0	0				0.5v
0	1				0.25v
1	0				0.125v
1	1				Reserved
5 to 6				PGA2				0					Voltage GAIN
PGA2[1:0]			Description
0	0				x1
0	1				x2
1	0				x4
1	1				RESERVED
7					INTEGRATOR			0					This bit enables the integrator on the current chanel when set.
*/

//Inner
void ADE7758_gainSetup(uint8_t integrator, uint8_t scale, uint8_t PGA2, uint8_t PGA1) {
  uint8_t pgas = (integrator << 7) | (PGA2 << 5) | (scale << 3) | (PGA1);
  ADE7758_write8(GAIN, pgas); //write GAIN register, format is |3 bits PGA2 gain|2 bits full scale|3 bits PGA1 gain
}

//Inner
void ADE7758_setupDivs(uint8_t Watt_div, uint8_t VAR_div, uint8_t VA_div) {
	ADE7758_write8(WDIV, Watt_div);  //Active Energy Register Divider
	ADE7758_write8(VARDIV, VAR_div); //Reactive Energy Register Divider
	ADE7758_write8(VADIV, VA_div);   //Apparent Energy Register Divider
}

//Inner
uint32_t ADE7758_getVRMS(uint8_t phase) {
	
	ADE7758_resetStatus(); // Clear all interrupts
	
	uint8_t  t_of = 0;
	uint32_t lastupdate = millis();
	uint32_t ZX = 0;
	uint8_t  VRMS = 0;
	uint32_t st = 0;
	
	if (phase==PHASE_A) {
		ZX = ZXA;
		VRMS = AVRMS;
	} else if (phase==PHASE_B) {
		ZX = ZXB;
		VRMS = BVRMS;
	} else {
		ZX = ZXC;
		VRMS = CVRMS;
	}
	
	//counter_micros_start();
	
	while (!(ADE7758_getStatus() & ZX))  // wait Zero-Crossing
	{ // wait for the selected interrupt to occur
		if ((millis() - lastupdate) > 20)
		{
			t_of = 1;
			break;
		}
	}
	//uint32_t mcs = counter_micros_stop();
	
	/*
	if (t_of) {
		st =  ADE7758_getStatus();
		sprintf(mess, "t_of status = %08X, mcs = %u", st, mcs);
		serial_write(mess);
		
		//return 0;
		return ADE7758_read24(VRMS);
	} else {
		st =  ADE7758_getStatus();
		sprintf(mess, "ok status = %08X, mcs = %u", st, mcs);
		serial_write(mess);
		
		return ADE7758_read24(VRMS);
	}
	*/
	return ADE7758_read24(VRMS);
}

//Not used
uint32_t ADE7758_vrms(uint8_t phase) {
	
	uint32_t v = 0;
	
	if (ADE7758_getVRMS(phase)) { //Ignore first reading to avoid garbage
		for (uint8_t i = 0; i < AVG_COUNT; ++i) {
			v += ADE7758_getVRMS(phase);
		}
		return v / AVG_COUNT;
	} else {
		return 0;
	}
}

/** === getFreq ===
  Period of the Phase selected in MMode.
  @param none
  @return int with the data (12 bits unsigned).
*/
float ADE7758_getFreq(void) {
	ADE7758_enableChip();
  
	float f = ADE7758_read16(FREQ) / FREQ_CAL;
	
	ADE7758_disableChip();
	
	return f;
}


/** === getMaskInterrupts / setMaskInterrupts
  MASK REGISTER (0x18)
  When an interrupt event occurs in the ADE7758, the IRQ logic output goes active low if the mask bit for this event is Logic 1 in the MASK register.
  The IRQ logic output is reset to its default collector open state when the RSTATUS register is read.
  describes the function of each bit in the interrupt mask register.

  Bit Location	Interrupt Flag		Default Value		Description
  0				AEHF				0					Enables an interrupt when there is a change in Bit 14 of any one of the three WATTHR registers,
														that is, the WATTHR register is half full.
  1				REHF				0					Enables an interrupt when there is a change in Bit 14 of any one of the three VARHR registers,
														that is, the VARHR register is half full.
  2				VAEHF				0					Enables an interrupt when there is a 0 to 1 transition in the MSB of any one of the three VAHR
														registers, that is, the VAHR register is half full.
  3				SAGA				0					Enables an interrupt when there is a SAG on the line voltage of the Phase A.
  4				SAGB				0					Enables an interrupt when there is a SAG on the line voltage of the Phase B.
  5				SAGC				0					Enables an interrupt when there is a SAG on the line voltage of the Phase C.
  6				ZXTOA				0					Enables an interrupt when there is a zero-crossing timeout detection on Phase A.
  7				ZXTOB				0					Enables an interrupt when there is a zero-crossing timeout detection on Phase B.
  8				ZXTOC				0					Enables an interrupt when there is a zero-crossing timeout detection on Phase C.
  9				ZXA					0					Enables an interrupt when there is a zero crossing in the voltage channel of Phase A
														(see the Zero-Crossing Detection section).
  10				ZXB					0					Enables an interrupt when there is a zero crossing in the voltage channel of Phase B
														(see the Zero-Crossing Detection section).
  11				ZXC					0					Enables an interrupt when there is a zero crossing in the voltage channel of Phase C
														(see the Zero-Crossing Detection section).
  12				LENERGY				0					Enables an interrupt when the energy accumulations over LINECYC are finished.
  13				RESERVED			0					Reserved.
  14				PKV					0					Enables an interrupt when the voltage input selected in the MMODE register is above
														the value in the VPINTLVL register.
  15				PKI					0					Enables an interrupt when the current input selected in the MMODE register is above the
														value in the IPINTLVL register.
  16				WFSM				0					Enables an interrupt when data is present in the WAVEMODE register.
  17				REVPAP				0					Enables an interrupt when there is a sign change in the watt calculation among any one of
														the phases specified by the TERMSEL bits in the COMPMODE register.
  18				REVPRP				0					Enables an interrupt when there is a sign change in the VAR calculation among any one of
														the phases specified by the TERMSEL bits in the COMPMODE register.
  19				SEQERR				0					Enables an interrupt when the zero crossing from Phase A is followed not by the zero crossing
														of Phase C but with that of Phase B.
*/

//not used
uint32_t ADE7758_getMaskInterrupts(void) {
	ADE7758_enableChip();
	
	uint32_t val = ADE7758_read24(MASK);
	
	ADE7758_disableChip();
	return val;
}

//Inner
void ADE7758_setMaskInterrupts(uint32_t m) {
	return ADE7758_write24(MASK, m);
}

void ADE7758_rms_clear(void) {
	rms_values.va = 0;
	rms_values.vb = 0;
	rms_values.vc = 0;
	rms_values.ia = 0;
	rms_values.ib = 0;
	rms_values.ic = 0;
	
	rms_values.ca = 1;
	rms_values.cb = 1;
	rms_values.cc = 1;
	
	rms_values.cnta = 0;
	rms_values.cntb = 0;
	rms_values.cntc = 0;
}

void ADE7758_rms_get(void) {
	
	ADE7758_enableChip();
	
	ADE7758_resetStatus(); // Clear all interrupts
	
	//counter_micros_start();
	
	uint8_t  t_of = 0;
	uint8_t  zeroA = 0;
	uint8_t  zeroB = 0;
	uint8_t  zeroC = 0;
	
	uint32_t lastupdate = millis();
	uint32_t st = 0;
	
	/*
	uint32_t rmsA = 0;
	uint32_t rmsB = 0;
	uint32_t rmsC = 0;
	*/

	while ( 1 ) {
		st = ADE7758_getStatus();
		
		if (st & ZXA) {
			if (!zeroA) {
				ADE7758_rms_add(1);
				zeroA = 1;
			}
		}
		if (st & ZXB) {
			if (!zeroB) {
				ADE7758_rms_add(2);
				zeroB = 1;
			}
		}
		if (st & ZXC) {
			if (!zeroC) {
				ADE7758_rms_add(3);
				zeroC = 1;
			}
		}
	
		if ((millis() - lastupdate) > 20) {
			t_of = 1;
			break;
		}
		if (zeroA && zeroB && zeroC) {
			break;
		}
	}
	/*
	uint32_t mcs = counter_micros_stop();
	
	sprintf(mess, "t_of = %u, mcs = %u, ZA = %u%u%u; RMS = %u, %u, %u", t_of, mcs, zeroA, zeroB, zeroC, rmsA, rmsB, rmsC);
	serial_write(mess);
	*/
	ADE7758_disableChip();
}

//inner
void ADE7758_rms_add(uint8_t p) {
	
	switch (p) {
		case 1:
			rms_values.va += ADE7758_read24(AVRMS);
			rms_values.ia += ADE7758_read24(AIRMS);
			rms_values.cnta++;
		break;
		
		case 2:
			rms_values.vb += ADE7758_read24(BVRMS);
			rms_values.ib += ADE7758_read24(BIRMS);
			rms_values.cntb++;
		break;
		
		case 3:
			rms_values.vc += ADE7758_read24(CVRMS);
			rms_values.ic += ADE7758_read24(CIRMS);
			rms_values.cntc++;
		break;
	}
}
uint32_t ADE7758_rms_val(uint8_t p) {
	
	uint32_t ret = 0;
	
	switch (p) {
		case 1:
			ret = rms_values.cnta ? rms_values.va / rms_values.cnta : 0;
		break;
		
		case 2:
			ret = rms_values.cntb ? rms_values.vb / rms_values.cntb : 0;
		break;
		
		case 3:
			ret = rms_values.cntc ? rms_values.vc / rms_values.cntc : 0;
		break;
		
		case 4:
			ret = rms_values.cnta ? rms_values.ia / rms_values.cnta : 0;
		break;
		
		case 5:
			ret = rms_values.cntb ? rms_values.ib / rms_values.cntb : 0;
		break;
		
		case 6:
			ret = rms_values.cntc ? rms_values.ic / rms_values.cntc : 0;
		break;
	}
	
	return ret;
}

//inner
void ADE7758_startPotLine(uint8_t Phase, uint8_t Ciclos) {
	
	uint8_t m = 0;
	
	switch (Phase) {
		case PHASE_A: m = (LWATT | LVAR | LVA | ZXSEL_A);
		break;
		case PHASE_B: m = (LWATT | LVAR | LVA | ZXSEL_B);
		break;
		case PHASE_C: m = (LWATT | LVAR | LVA | ZXSEL_C);
		break;
	}

	ADE7758_setLcycMode(m);
	ADE7758_resetStatus();
	ADE7758_setLineCyc(Ciclos);
}

bool ADE7758_setPotLine(uint8_t Phase, uint8_t Ciclos) {
	
	uint8_t t_of = 0;
	
	ADE7758_enableChip();
	
	ADE7758_startPotLine(Phase, Ciclos);
	
	uint32_t lastupdate = millis();
	
	while (!(ADE7758_getStatus() & LENERGY))  // wait to terminar de acumular
	{ // wait for the selected interrupt to occur
		if ((millis() - lastupdate) > (Ciclos * 15))
		{
			t_of = 1;
			break;
		}
	}
	
	ADE7758_disableChip();
	
	if (t_of) {
		return false;
	} else {
		return true;
	}
}

/** === getWatt(phase) / getVar(phase) / getVa(phase) ===
  Возвращает значения требуемой мощности выбранной фазы.
  Перед созданием значений используйте setPotLine (фаза).
**/

//outer
int16_t ADE7758_getWatt(uint8_t Phase) {
	
	int16_t temp = 0;
	
	ADE7758_enableChip();
	
	switch (Phase) {
		case PHASE_A: temp = ADE7758_read16(AWATTHR);
		break;
		case PHASE_B: temp = ADE7758_read16(BWATTHR);
		break;
		case PHASE_C: temp = ADE7758_read16(CWATTHR);
		break;
	}
	ADE7758_disableChip();
	
	return temp;
}

//outer
int16_t ADE7758_getVar(uint8_t Phase) {
	
	int16_t temp = 0;
	
	ADE7758_enableChip();
	
	switch (Phase) {
		case PHASE_A: temp = ADE7758_read16(AVARHR);
		break;
		
		case PHASE_B: temp = ADE7758_read16(BVARHR);
		break;
		
		case PHASE_C: temp = ADE7758_read16(CVARHR);
		break;
	}
	
	ADE7758_disableChip();
	
	return temp;
}

//outer
int16_t ADE7758_getVa(uint8_t Phase) {
	
	int16_t temp = 0;
	
	ADE7758_enableChip();
	
	switch (Phase) {
		case PHASE_A: temp = ADE7758_read16(AVAHR);
		break;
		
		case PHASE_B: temp = ADE7758_read16(BVAHR);
		break;
		
		case PHASE_C: temp = ADE7758_read16(CVAHR);
		break;
	}
	
	ADE7758_disableChip();
	
	return temp;
}
void ADE7758_setCos(uint8_t Phase) {
	
	int16_t wa = ADE7758_getWatt(Phase);
	int16_t ws = ADE7758_getVa(Phase);

	float cosF = ws >=10 ? (float)wa / (float)ws : 1;
	
	switch (Phase) {
		case PHASE_A:
			rms_values.ca = cosF;
		break;
		
		case PHASE_B:
			rms_values.cb = cosF;
		break;
		
		case PHASE_C:
			rms_values.cc = cosF;
		break;
	}
}
float ADE7758_getCos(uint8_t Phase) {
	
	float ret = 0;
	
	switch (Phase) {
		case PHASE_A:
			ret = rms_values.ca;
		break;
		
		case PHASE_B:
			ret = rms_values.cb;
		break;
		
		case PHASE_C:
			ret = rms_values.cc;
		break;
	}
	return ret;
}
