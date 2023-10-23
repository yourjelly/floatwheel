/**
  ******************************************************************************
  * @file       : main.c
  * @author     :  FCZ
  * @version    : V1.1.6
  * @date       :  March 28, 2022
  * @brief      :  Main function - LCM light control
  * @mcu        :  HK32F030MF4P6
  ******************************************************************************/
/*----------------------------------------------------------------------------
  Update log:
  2022-03-28 V1.0.0: Initial version
  2022-05-16 Added boot display progress bar and charging display voltage
  2022-05-17 Change charging display to breathing light effect
  2022-06-09 The shutdown time is changed from 5 minutes to 15 minutes
             When the duty cycle changes from more than 70% to more than 80%, the buzzer will sound "beep" and "beep".
			 The automatic shutdown condition is added from the original one of not stepping on the foot pedal and the timing shutdown will only start when the rotation speed is lower than 1000.
  2022-07-19 The battery is changed from 12 strings to 20 strings
             Change the shutdown time to 30 minutes
			 Charger voltage detection plus average filtering, voltage judgment plus receipt of voltage range
  2023-01-16 The WS2812 driver is changed from hardware SPI simulation to IO port plus delay simulation
  ----------------------------------------------------------------------------*/
#include "hk32f030m.h"
#include "led.h"
#include "time.h"
#include "spi.h"
#include "ws2812.h"
#include "power.h"
#include "key.h"
#include "usart.h"
#include "adc.h"
#include "buzzer.h"
#include "led_pwm.h"
#include "test.h"
#include "task.h"
#include "io_ws2812.h"


/**************************************************
 * @brief  :main()
 * @note   :������
 * @param  :��
 * @retval :��
 **************************************************/
int main(void)
{	
	//RCC_GetClocksFreq(&RCC_Clock);
	LED_Init();
	Buzzer_Init();
	ADC1_Init();
	WS2812_Init();
	Power_Init();
	KEY_Init();
	USART1_Init(115200);
	LED_PWM_Init();
	Time6_Init();
	if(KEY1 ==  0)
	{
		KEY1_State = 1;
	}
	while(1)
	{
		KEY1_Task();
		WS2812_Task();
		Power_Task();
		Charge_Task();
		Flashlight_Task();
		Flashlight_Detection();
		Buzzer_Task();
		Usart_Task();
		ADC_Task();
		Conditional_Judgment();
	}
}

