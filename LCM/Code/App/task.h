#ifndef __TASK_H
#define __TASK_H

#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "ws2812.h"
#include "flag_bit.h"
#include "vesc_uasrt.h"

typedef enum
{
	NORMAL,
	RAINBOW
} BOOT_ANIMATION;

typedef enum
{
	P42A,
	DG40
} CELL_TYPE;

typedef enum
{
	LCM,
	VESC,
	OFF
} BUZZER_TYPE;

#define	  CHARGING_VOLTAGE	  		40   		// Charging voltage detection. Greater than this value is considered that the charger is plugged in
#define   BATTERY_STRING      		20    		// Number of battery cells
#define   SHUTDOWN_TIME		  		30   		// Shutdown time (minutes)
#define   VESC_RPM            		250  		// RPM
#define   VESC_BOOT_TIME      		6000 		// VESC boot time (ms)
#define   DUTY_CYCLE          		0.7  		// When duty cycle greater than this value (%), beep beep beep (sic)
#define   VOLTAGE_RECEIPT     		0.02 		// Voltage receipt unit V
#define   ADC_THRESHOLD_LOWER       2.5         // Threshold value for footpad activation detection
#define   ADC_THRESHOLD_UPPER       2.9         // Threshold value for footpad activation detection
/*******************************************************************************/
#define   VESC_RPM_WIDTH      		100 		// When the rotation speed is ± this value, do not switch the travel direction
#define   LIGHTBAR_BRIGHTNESS_HIGH 	140			// High brightness value (0-255) -- Stock (204)
#define   LIGHTBAR_BRIGHTNESS_MED	70			// Medium brightness value (0-255) -- Stock (128)
#define   LIGHTBAR_BRIGHTNESS_LOW 	10			// Low brightness value (0-255) -- Stock (30)
#define   MAIN_BRIGHTNESS_REST		9000		// Main brightness at rest -- Stock 10% (9000)
#define   MAIN_BRIGHTNESS_LOW		7000		// Low brightness value (0-9999) -- Stock (7000)
#define   MAIN_BRIGHTNESS_MED		4000		// Medium brightness value (0-9999) -- Stock (4000)
#define   MAIN_BRIGHTNESS_HIGH		0			// High brightness value (0-9999) -- Stock (0)
#define   CHARGE_CURRENT			0.3         // Charging current unit A
#define   DETECTION_SWITCH_TIME     500         // Detection switch time unit ms
#define   CHARGER_DETECTION_DELAY	1000        // Charger detection delay unit ms
#define   BOOT_ANIMATION		    RAINBOW     // Boot animation (NORMAL, RAINBOW)
#define   CELL_TYPE                 DG40        // Cell configuration to use for voltage display (P42A, DG40)
#define	  BUZZER_TYPE				LCM			// Change control of buzzer (LCM, VESC, OFF) - TODO implement the different options in code
#define	  ENABLE_POWER_WHILE_CHARGE	true		// Enable power while charging
#define   FADE_TIME					1000		// Time of fade transition (ms)
#define   FADE_REFRESH				2			// (ms); FADE_TIME/FADE_REFRESH = fade steps 

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
void Change_Light_Profile(bool persist);
void Change_Boot_Animation(uint8_t animation);
void Change_Cell_Type(uint8_t type);
void Change_Buzzer_Type(uint8_t type);
#endif



