#ifndef __FLAG_BIT_H
#define __FLAG_BIT_H

#include "hk32f030m.h"

// BOARD STATE

extern uint8_t Power_Flag;
extern uint8_t Charge_Flag;
extern uint8_t Usart_Flag;

// LIGHT STATE

extern uint8_t Flashlight_Flag;
extern uint8_t Light_Profile;
extern uint8_t Brightness_Adjustment_Flag;
extern uint8_t Lightbar_Battery_Flag;
extern uint8_t Sensor_Activation_Display_Flag;
extern uint8_t Lightbar_Brightness;
extern uint8_t Power_Display_Flag;

// FOOTPAD STATE

extern uint16_t ADC_Time;
extern float ADC1_Val;
extern float ADC2_Val;

// BUZZER STATE

extern uint8_t Buzzer_Flag;
extern uint16_t Buzzer_Time;
extern uint8_t Buzzer_Frequency;

// CHARGE STATE

extern uint8_t V_I;
extern float Charge_Voltage;
extern float Charge_Current;

// COUNTING FLAGS

extern uint16_t Power_Time;
extern uint16_t Charge_Time;
extern uint16_t Usart_Time;
extern uint16_t Charger_Detection_1ms;
extern uint16_t Flashlight_Time;
extern uint16_t Flashlight_Detection_Time;
extern uint16_t Shutdown_Time_S;
extern uint8_t Shutdown_Time_M;
extern uint8_t Shutdown_Cnt;
#endif


