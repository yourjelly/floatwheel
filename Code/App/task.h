#ifndef __TASK_H
#define __TASK_H

#include "led.h"
#include "key.h"
#include "ws2812.h"
#include "flag_bit.h"
#include "vesc_uasrt.h"

#define	  CHARGING_VOLTAGE	  		40   		// Charging voltage detection. Greater than this value is considered that the charger is plugged in
#define   BATTERY_STRING      		20    		// Number of battery cells
#define   SHUTDOWN_TIME		  		30   		// Shutdown time (minutes)
#define   VESC_RPM            		250  		// RPM
#define   VESC_BOOT_TIME      		6000 		// VESC boot time (ms)
#define   DUTY_CYCLE          		0.7F  		// When duty cycle greater than this value (%), beep beep beep (sic)
#define   VOLTAGE_RECEIPT     		0.02F 		// Voltage receipt unit V
/*******************************************************************************/
#define   VESC_RPM_WIDTH      		-200 		// When the rotation speed is ± this value, do not switch the travel direction
#define   WS2812_1_BRIGHTNESS 		204			// High brightness value (0-255) -- Stock (204)
#define   WS2812_2_BRIGHTNESS 		128			// Medium brightness value (0-255) -- Stock (128)
#define   WS2812_3_BRIGHTNESS 		30			// Low brightness value (0-255) -- Stock (30)
#define   CHARGE_CURRENT			0.3F        // Charging current unit A
#define   CHARGE_CURRENT_L			0.1F        // Low Charging current unit A
#define   CHARGE_CURRENT_H			0.24F       // High Charging current unit A
#define   DETECTION_SWITCH_TIME     500         // Detection switch time unit ms
#define   CHARGER_DETECTION_DELAY	1000        // Charger detection delay unit ms

void LED_Task(void);
void KEY1_Task(void);
void WS2812_Task(void);
void Power_Task(void);
void Charge_Task(void);
void Flashlight_Task(void);
void Buzzer_Task(void);
void Usart_Task(void);
void ADC_Task(void);
void Conditional_Judgment(void);
void Flashlight_Detection(void);

#endif



