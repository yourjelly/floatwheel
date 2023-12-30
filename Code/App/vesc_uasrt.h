#ifndef __VESC_USART_H
#define __VESC_USART_H

#include <string.h>
#include "hk32f030m.h"
#include "crc.h"
#include "usart.h"
#include "datatypes.h"
// #include "eeprom.h"
// #include "task.h"


typedef struct {
	bool floatPackageSupported;
	// float tempFET;
	// float tempMotor;
	// float avgMotorCurrent;
	float avgInputCurrent;
	int dutyCycleNow;
	long rpm;
	float inpVoltage;
	// float ampHours;
	// float ampHoursCharged;
	// long tachometer;
	// long tachometerAbs;
	char state;
	char fault;
	bool isForward;
	bool isHandtest;
}dataPackage;

typedef struct {
	uint8_t headlightBrightness;
	uint8_t statusbarBrightness;
	uint8_t statusbarMode;
	uint8_t dutyBeep;
	uint8_t boardOff;
	bool isSet;
}lcmConfig_t;

extern uint8_t VESC_RX_Buff[256];
extern uint8_t VESC_RX_Flag;
extern dataPackage data;
extern lcmConfig_t lcmConfig;
extern uint8_t errCode;

void Get_Vesc_Pack_Data(COMM_PACKET_ID id);  //Get the normal values from the vesc
// void Get_Vesc_Adc_Data();					 //Get footsensor config from vesc (store in eeprom?)
// void Get_Eeprom_Data(LCM_COMMANDS command);  //Get eeprom data and send back over bluetooth - TODO implement + implement in vesc pkg
uint8_t Protocol_Parse(uint8_t * message);	

#endif


