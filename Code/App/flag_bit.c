#include "flag_bit.h"

//// BOARD STATE

/*
	Power_Flag = 0; Default state.
	Power_Flag = 1; VESC booting up.
	Power_Flag = 2; VESC boot-up completed.
	Power_Flag = 3; Board on charger.
	Power_Flag = 4; Power off Signal
*/
uint8_t Power_Flag = 0;
/*
	Charge_Flag = 0; Default state.
	Charge_Flag = 1; Ready for charging.
	Charge_Flag = 2; Charging in progress.
	Charge_Flag = 3; Battery fully charged.
*/
uint8_t Charge_Flag = 0;

//// LIGHTS STATE

/*
	Flashlight_Flag = 0; Default state.
	Flashlight_Flag = 1; VESC booting up.
	Flashlight_Flag = 2; Front light white, rear light red (forward).
	Flashlight_Flag = 3; Front light red, rear light white (reverse).
	Flashlight_Flag = 4; Brightness set to 10%.
*/
uint8_t Flashlight_Flag = 0;
/*
	Light_Profile = 0; Default state.
	Light_Profile = 1; Headlight bightness low, Lightpad brightness high, beep once
	Light_Profile = 2; Headlight bightness mid, Lightpad brightness mid, beep twice
	Light_Profile = 3; Headlight bightness high, Lightpad brightness low, beep thrice
*/
uint8_t Light_Profile = 0;
/*
	Brightness_Adjustment_Adjustment_Flag = 0; Default state.
	Brightness_Adjustment_Adjustment_Flag = 1; Starting brightness adjustment.
	Brightness_Adjustment_Adjustment_Flag = 2; Brightness adjustment completed.
*/
uint8_t Brightness_Adjustment_Flag = 0;
/*
	Lightbar_Battery_Flag = 0; Powered on.
	Lightbar_Battery_Flag = 1; Display battery level.
	Lightbar_Battery_Flag = 2; Do not display battery level.
*/
uint8_t Lightbar_Battery_Flag = 0;
/*
	Sensor_Activation_Display_Flag = 0; Default state.
	Sensor_Activation_Display_Flag = 1; Left 5 LEDs blue, Right 5 LEDs off (adc1 > ADC_THRESHOLD_LOWER > adc2)
	Sensor_Activation_Display_Flag = 2; Left 5 LEDs off, Right 5 LEDs blue (adc1 < ADC_THRESHOLD_LOWER < adc2)
	Sensor_Activation_Display_Flag = 3; All 10 LEDs blue (adc1 && adc2 > ADC_THRESHOLD_LOWER)
	Sensor_Activation_Display_Flag = 4; All 10 LEDs off (adc1 && adc2 < ADC_THRESHOLD_LOWER)
*/
uint8_t Sensor_Activation_Display_Flag = 0;

uint8_t Lightbar_Brightness = 0; // Lightbar runtime brightness setting (0-255)

uint16_t Main_Brightness = 0; // Main lights runtime brightness setting (0-9999) -> 9999 = 0% brightness and 0 = 100% brightness

/*
	Power_Display_Flag = 0; Default state.
	Power_Display_Flag = 1; Full charge: 10 white LEDs.
	Power_Display_Flag = 2; 90%: 9 white LEDs.
	Power_Display_Flag = 3; 80%: 8 white LEDs.
	Power_Display_Flag = 4; 70%: 7 white LEDs.
	Power_Display_Flag = 5; 60%: 7 white LEDs.
	Power_Display_Flag = 6; 50%: 5 white LEDs.
	Power_Display_Flag = 7; 40%: 4 white LEDs.
	Power_Display_Flag = 8; 30%: 3 white LEDs.
	Power_Display_Flag = 9; 20%: 2 red LEDs.
	Power_Display_Flag = 10; 10%: 1 red LED.
*/
uint8_t Power_Display_Flag = 0;

/*
	Animation_Running = 0: False
	Animation_Running = 1: Boot
	Animation_Running = 2: Shutdown
	Animation_Running = 3: Disabled
	Animation_Running = 4: Idle
*/
uint8_t Animation_Running = 0;

//// FOOTPAD STATE

uint16_t ADC_Time = 0; // ADC sampling time.

float ADC1_Val = 0; // ADC1

float ADC2_Val = 0; // ADC2

//// BUZZER STATE

/*
	Buzzer_Flag = 0; Default state.
	Buzzer_Flag = 1; Buzzer disabled.
	Buzzer_Flag = 2; Buzzer enabled.
*/
uint8_t Buzzer_Flag = 0;

uint16_t Buzzer_Time = 0; // Buzzer sounding time.

uint8_t Buzzer_Frequency = 0; // Buzzer frequency in BPM, beats per minute, ranging from 60 to 180, corresponding to a duty cycle of 70 to 100)

//// CHARGE STATE

/*
	V_I = 0: Sampling charging current.
	V_I = 1: Sampling charging voltage.
*/
uint8_t V_I = 1;

float Charge_Voltage = 0; // Charging Port Voltage

float Charge_Current = 0; // Charging Port Current


//// COUNTING FLAGS

uint16_t Power_Time = 0; // Power on time.
uint16_t Charge_Time = 0; // Charging time.
uint16_t Usart_Time = 0; // USART communication time.

uint16_t Flashlight_Time = 0; // Flashlight time.
uint16_t Flashlight_Detection_Time = 0; // When the button is double-clicked without pressing it, the headlight stays on for 3 seconds.

uint8_t Shutdown_Time_M = 0; // Shutdown time in minutes.
uint16_t Shutdown_Time_S = 0; // Shutdown time in seconds.
uint8_t Shutdown_Cnt = 0; // Number of times the current reaches the shutdown value.
uint16_t Charger_Detection_Timer = 0; // Charger detection timer.

//// CONFIG
/// Configured cell type (gets read from EEPROM)
uint8_t Config_Cell_Type = 0;

/// Configured boot animation (gets read from EEPROM)
uint8_t Config_Boot_Animation = 0;

/// Configured buzzer (gets read from EEPROM)
uint8_t Config_Buzzer = 0;