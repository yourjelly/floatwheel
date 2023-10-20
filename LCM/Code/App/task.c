#include "task.h"
#include "eeprom.h"
#include <math.h>

///Colors
//These are warnings displayed on the lightbar -> use full brightness
uint8_t purple[3] = {0x80, 0x00, 0x80};		//Tiltback - high, low, duty
uint8_t red[3] = {0xff, 0x00, 0x00};		//Error
uint8_t orange[3] = {0xff, 0x5a, 0x00};		//Temp
uint8_t yellow[3] = {0xff, 0xff, 0x00};		//Current

void Change_Cell_Type(uint8_t type) {
	if (type == DG40 || type == P42A) {
		EEPROM_WriteByte(CHANGE_CELL_TYPE, type);
	}
}

void Change_Boot_Animation(uint8_t animation) {
	if (animation == RAINBOW || animation == NORMAL) {
		EEPROM_WriteByte(CHANGE_BOOT_ANIMATION, animation);
	}
}

void Change_Buzzer_Type(uint8_t type) {
	if (type == LCM || type == VESC || type == OFF) {
		EEPROM_WriteByte(CHANGE_BUZZER_TYPE, type);
	}
}
/*
void Change_Cell_Type(uint8_t type, bool get) {
	if ((type == DG40 || type == P42A) & get) {
		EEPROM_WriteByte(CHANGE_CELL_TYPE, type);
	} else {
		EEPROM_ReadByte(CHANGE_CELL_TYPE, type);
	}
}

void Change_Boot_Animation(uint8_t animation,bool get) {
	if ((animation == RAINBOW || animation == NORMAL) & get) {
		EEPROM_WriteByte(CHANGE_BOOT_ANIMATION, animation);
	} else {
		EEPROM_ReadByte(CHANGE_BOOT_ANIMATION, animation);
	}
}*/

/**************************************************
 * @brie   :KEY1_Task()
 * @note   :KEY1����
 **************************************************/
void KEY1_Task(void)
{
	if(KEY1_State == 0)
	{
		return;
	}

	switch(KEY1_State)
	{
		case 1: // Click
			if(Power_Flag != 2)
			{
				Power_Flag = 1;
			}
			break;

		case 2: // Double Click
			if (Power_Flag == 2 || Power_Flag == 3)
			{
				Change_Light_Profile(true);
			}
			break;

		case 3: // Press
			Power_Flag = 4;
			// Flashlight_Flag = 0;
			// Lightbar_Mode_Flag = 0;
			break;

		case 4: // Tripple Press
			if (Power_Flag == 2 || Power_Flag == 3)
			{
				if(Buzzer_Flag == 2)
				{
					Buzzer_Flag = 1;
				}
				else
				{
					Buzzer_Flag = 2;
				}
			}
			break;
			
	}
	KEY1_State = 0;
}

/**************************************************
 * @note	Displays current power level on lightbar
 **************************************************/
void Power_Display(void)
{
	uint8_t i;
	uint8_t num = 10 - (Power_Display_Flag - 1);

	for (i=0;i<10;i++) {
		if (Power_Display_Flag == 0) {
			// Something is wrong - set all LEDs to red
			WS2812_Set_Colour(i,WS2812_Measure,0,0);
		} else {
			if (i < num) {
				if (num <= 2) {
					WS2812_Set_Colour(i,WS2812_Measure,0,0);
				} else {
					WS2812_Set_Colour(i,WS2812_Measure,WS2812_Measure,WS2812_Measure);
				}
			} else {
				WS2812_Set_Colour(i,0,0,0);
			}
		}
	}

	WS2812_Refresh();//ˢ����ʾ
}

/**************************************************
 * @note	Displays current footpad sensor activation on the lightbar
 **************************************************/
void Sensor_Activation_Diplay(void)
{
	uint8_t i;

	switch(Footpad_Activation_Flag)
	{
		case 1:// adc1>ADC_THRESHOLD_LOWER  adc2<ADC_THRESHOLD_LOWER
			for(i=0;i<5;i++)
		{
				WS2812_Set_Colour(i,0,0,WS2812_Measure);
		}
				for(i=5;i<10;i++)
		{
				WS2812_Set_Colour(i,0,0,0);
		}
		break;

		case 2:// adc1<ADC_THRESHOLD_LOWER  adc2>ADC_THRESHOLD_LOWER
			for(i=0;i<5;i++)
		{
				WS2812_Set_Colour(i,0,0,0);
		}
				for(i=5;i<10;i++)
		{
				WS2812_Set_Colour(i,0,0,WS2812_Measure);
		}
		break;

		case 3:// adc1>ADC_THRESHOLD_LOWER  adc2>ADC_THRESHOLD_LOWER
			for(i=0;i<10;i++)
		{
				WS2812_Set_Colour(i,0,0,WS2812_Measure);
		}
		break;

		case 4:// adc1<ADC_THRESHOLD_LOWER  adc2<ADC_THRESHOLD_LOWER
			for(i=0;i<10;i++)
		{
				WS2812_Set_Colour(i,0,0,0);
		}
		break;

	default:
			for(i=0;i<10;i++)
		{
			WS2812_Set_Colour(i,0,0,0);
		}
		break;
	}
	WS2812_Refresh();//ˢ����ʾ
}


/**************************************************
 * @note	Displays the boot animation on the lightbar
 **************************************************/
void WS2812_Boot(void) {
	uint8_t i;
	uint8_t num = floor(Power_Time / 500) + 1;
	uint8_t rgbMap[10][3] = {{255,0,0}, {255,127,0}, {255,255,0}, {127,255,0}, {0,255,0}, {0,255,127}, {0,255,255}, {0,127,255}, {0,0,255}, {127,0,255}};

	if (num > 10) {
		num = 10;
	}

	for (i=0;i<num;i++) {
		//switch (Config_Boot_Animation) {  Not sure what would happen if no bytes are stored (set to 0xFF?)
		//Then this switch can be implemented
		switch (BOOT_ANIMATION) {
		case NORMAL:
			WS2812_Set_Colour(i,0,255,255);
			break;
		case RAINBOW:
			WS2812_Set_Colour(i,rgbMap[i][0],rgbMap[i][1],rgbMap[i][2]);
			break;
		}
	}

	for (i = num; i < 10; i++) {
		WS2812_Set_Colour(i,0,0,0);
	}

	WS2812_Refresh();//ˢ����ʾ
}

/**************************************************
 * @brie   :WS2812_Cal_Bri()
 * @note   :��������
 * @param  :���� 1�α�ʾ200ms
 * @retval :����
 **************************************************/
uint8_t WS2812_Cal_Bri(uint8_t cnt)
{
	static uint8_t brightness = 1;

	// Update brightness
	if(cnt < 50)
	{
		brightness++;
	}
	else
	{
		brightness--;
	}

	// Clamp brightness
	if(brightness < 1)
	{
		brightness = 1;
	}
	else if(brightness > 50)
	{
		brightness = 50;
	}

	//	if(cnt == 50)
	//	{
	//	brightness = brightness;
	//	}
	//
	//	if(cnt == 99)
	//	{
	//	brightness = brightness;
	//	}
	//	switch(cnt)
	//	{
	//		case 0:
	//			brightness = 0x03;
	//		break;
	//
	//		case 1:
	//			brightness = 0x04;
	//		break;
	//
	//		case 2:
	//			brightness = 0x05;
	//		break;
	//
	//		case 3:
	//			brightness = 0x06;
	//		break;
	//
	//		case 4:
	//			brightness = 0x07;
	//		break;
	//
	//		case 5:
	//			brightness = 0x08;
	//		break;
	//
	//		case 6:
	//			brightness = 0x07;
	//		break;
	//
	//		case 7:
	//			brightness = 0x06;
	//		break;
	//
	//		case 8:
	//			brightness = 0x05;
	//		break;
	//
	//		case 9:
	//			brightness = 0x04;
	//		break;
	//
	//		case 10:
	//			brightness = 0x03;
	//		break;
	//	}

	return brightness;
}

/**************************************************
 * @note	Shows the current battery % when the board is charging
 **************************************************/
void WS2812_Charge(void)
{
	uint8_t i;
	uint8_t num = 10 - (Power_Display_Flag - 1);
	static uint8_t cnt = 0;
	uint8_t brightness = 0;

	brightness = WS2812_Cal_Bri(cnt);
	for (i=0;i<10; i++) {
		if (i <= num) {
			if (num >= 10) {
				// Full charged
				WS2812_Set_Colour(i,0,brightness,0);
			} else if (num <= 2) {
				// Low battery
				WS2812_Set_Colour(i,brightness,0,0);
			} else {
				// Normal charging
				WS2812_Set_Colour(i,brightness,brightness,brightness);
			}
		} else {
			WS2812_Set_Colour(i,0,0,0);
		}
	}

	cnt++;

	if(cnt == 100)
	{
		cnt = 0;
	}

	WS2812_Refresh();//ˢ����ʾ
}

/**************************************************
 * @brie   :WS2812_Task()
 * @note   :WS2812����
 * @param  :��
 * @retval :��
 **************************************************/
void WS2812_Task(void)
{
	//	static uint8_t Footpad_Activation_Flag_last = 0; //��һ�ε�״̬
	//	static uint8_t power_display_flag_last = 0; //��һ�ε�״̬
	uint8_t i;

	if(WS2812_Counter < 20) //20msˢ��һ��
	{
		return;
	}
	WS2812_Counter = 0;

	// Inititalize lightbar; set all lights off
	if(Power_Flag == 0 || (Power_Flag == 3 && Charge_Flag == 0))
	{
		for(i=0;i<10;i++)
		{
			WS2812_Set_Colour(i,0,0,0);
		}
		WS2812_Refresh();

		Lightbar_Mode_Flag = 0;
		Footpad_Activation_Flag = 0;
		Power_Display_Flag = 0;

		return;
	}

	// VESC Booting play boot animation
	if(Power_Flag == 1)
	{
		WS2812_Boot();
		return;
	}

	// Charging full; set all 10 lights to white
	if(Charge_Flag == 3)
	{
		for(i=0;i<10;i++)
		{
			WS2812_Set_Colour(i,255,255,255);
		}
		return;
	}

	// Display charge state while charging
	if(Charge_Flag == 2)
	{
		WS2812_Charge();
		return;
	}

	if(Lightbar_Mode_Flag == 1)  //��ʾ����
	{
		//		if(Power_Display_Flag == power_display_flag_last) //��һ�κ���һ��һ��ֱ���˳�
		//		{
		//			return;
		//		}
		//		else
		//		{
		//			power_display_flag_last = Power_Display_Flag;
		//			Power_Display();// ������ʾ
		//		}
		Power_Display();// ������ʾ
	}
	else //����ʾ����
	{
		//		if(Footpad_Activation_Flag_last == Footpad_Activation_Flag) //��һ�κ���һ��һ��ֱ���˳�
		//		{
		//			return;
		//		}
		//		else
		//		{
		//			Footpad_Activation_Flag_last = Footpad_Activation_Flag;
		//			WS2812();//����ʾ����WS2812
		//		}
		Sensor_Activation_Diplay();//����ʾ����WS2812
	}

}

/**************************************************
 * @note   Cycles through the available lighting profiles
 * @param  persist (bool): whether to save the current profile to EEPROM
 **************************************************/
void Change_Light_Profile(bool persist)
{
	// Change light profile
	Light_Profile++;
	if (Light_Profile == 4)
	{
		Light_Profile = 1;
	}
	if (persist)
	{
		EEPROM_WriteByte(0, Light_Profile);
	}
	Set_Light_Brightness();
}

/// Only call the when the Light_Profile is changed (either via button or bluetooth) or after first eeprom read
/// Allows runtime bluetooth changes on WS2812_Measure and Main_Brightness
/*
* @note Sets light brightness to the current Light_Profile
*/
void Set_Light_Brightness()
{
	switch (Light_Profile)
	{
	// Inversely set the brightness of the lightbar to the brightness of the main lights
	case 1: // Low
		WS2812_Measure = LIGHTBAR_BRIGHTNESS_HIGH;
		Main_Brightness = MAIN_BRIGHTNESS_LOW;
		break;

	case 2: // Med
		WS2812_Measure = LIGHTBAR_BRIGHTNESS_MED;
		Main_Brightness = MAIN_BRIGHTNESS_MED;
		break;

	case 3: // High
		WS2812_Measure = LIGHTBAR_BRIGHTNESS_LOW;
		Main_Brightness = MAIN_BRIGHTNESS_HIGH;
		break;

	default:
		WS2812_Measure = LIGHTBAR_BRIGHTNESS_HIGH;
		Main_Brightness = MAIN_BRIGHTNESS_LOW;
		break;
	}
}

/**
 * @note transitions from current brightness to end brightness
 * @param target ending brightness
 * @param time transition time in ms
 */
void Light_Transition(uint16_t target, uint16_t time)
{
	static uint16_t brightness = 9999;
	uint16_t diff = 0;
	int mod = 1;
	if (Brightness_Flag == 1) // if target has changed restart timer
	{
		Brightness_Flag = 2;
		Flashlight_Time = 0;
	}
	if (brightness > target)
	{
		diff = brightness - target;
		mod = -1;
	}
	else {
		diff = target - brightness;
	}
	if (Flashlight_Time >= time)
	{
		TIM_SetCompare2(TIM1, target);
		Brightness_Flag = 3;
		return;
	}
	if (Flashlight_Time % FADE_REFRESH == 0)
	{
		brightness = brightness + (round((Flashlight_Time * diff) / time) * mod);
		TIM_SetCompare2(TIM1, brightness);
		time -= FADE_REFRESH;
	}
	if (Brightness_Flag == 2)
	{
		Light_Transition(target, time);
	}
}

/**************************************************
 * @brie   : Flashlight_Bright()
 * @note   : Flashlight brightness control
 * @param direction 1=Forward 2=Reverse  
 * @param bright 1=transition to REST 2=transition from 10% to 100%
 **************************************************/
void Flashlight_Bright(uint8_t direction,uint8_t bright)
{
	static uint8_t direction_last = 0;
	if (Brightness_Flag == 2)
	{ 
		Brightness_Flag = 3;
	}
	if (direction == 1) // Direction forward
	{
		LED_F_OFF;
		LED_B_ON;
	}
	else // Direction backward
	{
		LED_B_OFF;
		LED_F_ON;
	}

	if (bright == 1)
	{
		if (Main_Brightness > MAIN_BRIGHTNESS_REST) {  // If your light profile is less than rest use that
			Light_Transition(Main_Brightness, FADE_TIME);
		} 
		else {
			Light_Transition(MAIN_BRIGHTNESS_REST, FADE_TIME);
		}
	}
	else
	{
		Light_Transition(Main_Brightness, FADE_TIME);
	}
}


/**************************************************
 * @note	Controls headlight/taillight brightness multiplier and direction
 **************************************************/
void Flashlight_Task(void)
{
	static uint8_t flashlight_flag_last = 0;

	if(Power_Flag != 2) //When just booting or vesc is off turn lights off
	{
		LED_B_OFF;
		LED_F_OFF;
		Flashlight_Flag = 0;
		TIM_SetCompare2(TIM1,9999);
		return;
	}

	if(flashlight_flag_last == Flashlight_Flag) //Direction isnt changed
	{
		return; //Dont do anything -> state didnt change
	}
	else if(flashlight_flag_last != Flashlight_Flag)
	{
		flashlight_flag_last = Flashlight_Flag;
		Brightness_Flag = 3; // Breaks out of current transition to prepare for the next

	}
	switch(Flashlight_Flag)
	{
		case 1://VESC booting -> 0% -> 10% in 2 sec
			Flashlight_Bright(1,1);
		break;

		case 2:
			Flashlight_Bright(1,2);
		break;

		case 3://VESCǰ���ƺ���׵�(��ת)
			Flashlight_Bright(2,2);
		break;

		case 4:
			Flashlight_Bright(1,1);
		break;

		default:
		break;
	}
}

void Flashlight_Detection(void)
{
	static uint8_t Light_Profile_last = 0;

	if(Light_Profile_last == Light_Profile && Flashlight_Detection_Time >= 3000)
	{
		if (ADC1_Val < ADC_THRESHOLD_LOWER && ADC2_Val < ADC_THRESHOLD_LOWER)
		{
			Flashlight_Flag = 4;
		}
		return;
	}
	else
	{
		if(Light_Profile_last != Light_Profile)
		{
			if(ADC1_Val < ADC_THRESHOLD_LOWER && ADC2_Val < ADC_THRESHOLD_LOWER)
			{
				Set_Light_Brightness();
				TIM_SetCompare2(TIM1,Main_Brightness);
				Flashlight_Detection_Time = 0;
			}
			else
			{
				Set_Light_Brightness();
				TIM_SetCompare2(TIM1,Main_Brightness);

				Flashlight_Detection_Time = 3100;
			}
			Light_Profile_last = Light_Profile;
		}
	}
}

/**************************************************
 * @note	Sets appropriate flags for current power state
 **************************************************/
void Power_Task(void)
{
	static uint8_t power_flag_last = 0; // ��һ�ε�״̬
	static uint8_t power_step = 0;

	if (power_flag_last == Power_Flag && Power_Flag != 1)
	{
		return;
	}
	power_flag_last = Power_Flag;

	switch (Power_Flag)
	{
	case 1: // VESC����
		PWR_ON;
		Flashlight_Flag = 1;
		switch (power_step)
		{
		case 0:
			Power_Time = 0;
			power_step = 1;
			break;

		case 1:
			if (Power_Time > VESC_BOOT_TIME)
			{
				Power_Flag = 2;	   // �������
				Light_Profile = 1; // ������Ĭ����1��
				Buzzer_Flag = 2;   // ����Ĭ�Ϸ�������
				power_step = 0;
				Config_Cell_Type = CELL_TYPE;			// Set to the define
				Config_Boot_Animation = BOOT_ANIMATION; // Set to the define
				Config_Buzzer = BUZZER_TYPE;
				// Read saved value from EEPROM
				uint8_t data = Light_Profile;
				EEPROM_ReadByte(0, &data);

				if (data > 0 && data < 4)
				{
					Light_Profile = data;
				}
				data = Config_Cell_Type;
				EEPROM_ReadByte(CHANGE_CELL_TYPE, &data);
				if (data == DG40 || data == P42A)
				{
					Config_Cell_Type = data;
				}
				data = Config_Boot_Animation;
				EEPROM_ReadByte(CHANGE_BOOT_ANIMATION, &data);
				if (data == NORMAL || data == RAINBOW)
				{
					Config_Boot_Animation = data;
				}
				data = Config_Buzzer;
				EEPROM_ReadByte(CHANGE_BUZZER_TYPE, &data);
				if (data == LCM || data == VESC || data == OFF)
				{
					Config_Buzzer = data;
				}
				Set_Light_Brightness();
			}
			break;
		}

		break;

	case 2: // Booted UP
		break;

	case 3: // Powered by charger
		if (ENABLE_POWER_WHILE_CHARGE != true)
		{
			PWR_OFF;
		}
		break;

	case 4: // Signal to power board off
		PWR_OFF;
		break;

	default:

		break;
	}
}

/**************************************************
 * @note	Sets appropriate flags for current charging state
 **************************************************/
void Charge_Task(void)
{
	static uint8_t charge_step = 0;

	if (Charge_Flag == 0)
	{
		return;
	}

	switch (charge_step)
	{
	case 0:
		Charge_Time = 0;
		charge_step = 1;
		break;

	case 1:
		if (Charge_Time > 1000) // DELAY 1S
		{
			charge_step = 2;
		}
		break;

	case 2:
		CHARGE_ON; // TURN ON THE CHARGER
		Charge_Flag = 2;
		charge_step = 3;
		break;

	case 3:
		Charge_Time = 0;
		charge_step = 4;
		break;

	case 4:
		if (Charge_Time > DETECTION_SWITCH_TIME)
		{
			V_I = 1;
			Charge_Time = 0;
			LED1_ON; // COLLECT CHARGING VOLTAGE
			charge_step = 5;
		}
		break;

	case 5:
		if (Charge_Time > DETECTION_SWITCH_TIME)
		{
			V_I = 0;
			Charge_Time = 0;
			LED1_OFF; // COLLECT CHARGING FLOW
			charge_step = 4;
		}
		break;

	default:

		break;
	}
}

/**************************************************
 * @brie   :Buzzer_Task()
 * @note   :����������
 * @param  :��
 * @retval :��
 **************************************************/
void Buzzer_Task(void)
{
	static uint8_t buzzer_step = 0;
	static uint8_t Light_Profile_last = 0; //��һ�εĵ�λ
	static uint8_t ring_frequency = 0;
	static uint16_t sound_frequency = 0;
	if (Config_Buzzer == OFF) {
		BUZZER_OFF; //Make sure the buzzer is really off
		return;
	}
	if (Config_Buzzer == VESC) {
		return; //Do nothing with the buzzer, let the vesc handle everything -> when changed to vesc make sure to initialy set buzzer off
	}
	if(Power_Flag != 2 || Buzzer_Flag == 1) //VESC�ϵ��������ر�
	{
		BUZZER_OFF;
		buzzer_step = 0;
		return;
	}

	if(Buzzer_Frequency == 0 && Light_Profile_last == Light_Profile) //���������Ƶ��Ϊ0����һ�εĵ�λ������εĵ�λ
	{
		BUZZER_OFF;
		buzzer_step = 0;
		return;
	}

	if (Buzzer_Frequency != 0) // this triggers only on dutycycle
	{
		switch(buzzer_step)
		{
		case 0:
			sound_frequency = (uint16_t)(-2.78*Buzzer_Frequency+666);
			Buzzer_Time = 0;
			buzzer_step = 1;
			break;

		case 1:
			Buzzer_Ring(sound_frequency>>2);
			buzzer_step = 2;
			break;

		case 2:
			if(Buzzer_Time > sound_frequency)
			{
				buzzer_step = 0;
			}
			break;

		default:

			break;
		}
	}
	else
	{
		switch(buzzer_step)
		{
		case 0:
			Buzzer_Time = 0;
			buzzer_step = 1;
			break;

		case 1:
			Buzzer_Ring(200);
			buzzer_step = 2;
			break;

		case 2:
			if(Buzzer_Time > 400)
			{
				ring_frequency++;
				buzzer_step = 0;
				if(ring_frequency >= Light_Profile) //fix for infinite beep bug
				{
					ring_frequency = 0;
					Light_Profile_last = Light_Profile;
				}

			}
			break;

		default:

			break;
		}
	}
}

/**************************************************
 * @note	Sends commands to VESC controller to get data
 **************************************************/
void Usart_Task(void)
{
	static uint8_t usart_step = 0;
	uint8_t result;

	if(Power_Flag != 2)
	{
		data.inpVoltage = 0;
		data.rpm = 0;
		data.avgInputCurrent = 0;
		data.dutyCycleNow = 0;
		usart_step = 0;
		return;
	}

	switch(usart_step)
	{
		case 0:
			Get_Vesc_Pack_Data(COMM_GET_VALUES);
			usart_step = 1;
			break;

		case 1:
			if(VESC_RX_Flag == 1)
			{
				VESC_RX_Flag = 0;
				result = Protocol_Parse(VESC_RX_Buff);

				if(result == 0) //�����ɹ�
				{
						//LED1_Filp_Time(500);
						Usart_Flag = 1;
//						Battery_Voltage = data.inpVoltage; //��ص�ѹ
//						VESC_Rpm = data.rpm;            //ת��
//						AvgInputCurrent = data.avgInputCurrent;  //ĸ�ߵ���
//						DutyCycleNow = data.dutyCycleNow;   //ռ�ձ�
				}
				else	//����ʧ��
				{
					//LED1_Filp_Time(100);
					Usart_Flag = 2;
				}
				Usart_Time = 0;
				usart_step = 2;
			}
			else
			{
				usart_step = 3;
				Usart_Time = 0;
			}
		break;

		case 2:
			if(Usart_Time >= 100)
			{
				usart_step = 0;
			}
		break;

		case 3:
			if(VESC_RX_Flag == 1)
			{
				usart_step = 1;
			}
			else if(Usart_Time >= 100)
			{
				usart_step = 0;
			}
		break;

		default:

		break;

	}

}
/**************************************************
 * @note	Sets appropriate flags for current ADC/footpad sensor state
 **************************************************/
void ADC_Task(void)
{
	static uint8_t adc_step = 0;
	//	static uint8_t i = 0;
	//	static uint16_t adc_charge_sum[10];
	static uint16_t adc_charge_sum_ave = 0;
	static uint16_t adc1_val_sum_ave = 0;
	static uint16_t adc2_val_sum_ave = 0;
	//	float old_charge_current = 0;

	//	if(Power_Flag != 2)
	//	{
	//		Charge_Voltage = 0;
	//		ADC1_Val = 0;
	//		ADC2_Val = 0;
	//		adc_step = 0;
	//		return;
	//	}

	switch(adc_step)
	{
	case 0:
		ADC_Time = 0;
		adc_step = 1;
		break;

	case 1:
		if(ADC_Time>100)
		{
			ADC_Time = 0;

			adc_charge_sum_ave= Read_ADC_Value(3);
			adc1_val_sum_ave  = Read_ADC_Value(1);
			adc2_val_sum_ave  = Read_ADC_Value(2);

			ADC1_Val = (float)(adc1_val_sum_ave*0.0012890625);
			ADC2_Val = (float)(adc2_val_sum_ave*0.0012890625);

			//				if(Charge_Flag == 3)
			//				{
			//					if(V_I == 1)
			//					{
			//						V_I = 0;
			//						Charge_Time = 0;
			//						Sampling_Completion = 0;
			//						LED1_OFF; //�ɼ������
			//						Charge_Voltage = (float)(adc_charge_sum_ave*0.0257080078125);
			//
			//					}
			//					else
			//					{
			//						if(Charge_Time>100)
			//						{
			//							adc_charge_sum[i] = adc_charge_sum_ave;
			//							i++;
			//
			//							if(i == 10)
			//							{
			//								LED1_ON; //�ɼ����ѹ
			//								Charge_Time = 0;
			//								Sampling_Completion = 1;
			//								V_I = 1;
			//								i = 0;
			//							}
			//						}
			//					}
			//				}
			//				else
			//				{
			//					Charge_Voltage = (float)(adc_charge_sum_ave*0.0257080078125);
			//				}

			if(V_I == 0)
			{
				if(Charge_Time>100)
				{
					Charge_Current = (float)(-0.008056640625*adc_charge_sum_ave+16.5);
					//Charge_Current = Charge_Current*k + old_charge_current*(1-k);
					//old_charge_current = Charge_Current;
				}
			}
			else
			{
				if(Charge_Time>100)
				{
					Charge_Voltage = (float)(adc_charge_sum_ave*0.0257080078125);
				}
			}
		}

		//			if(i == 8)
		//			{
		//				adc_charge_sum_ave >>= 3;
		//				// y=kx+b 0=k*2048+b  10=k*(0.65/3.3*4096)+b
		//				if(V_I == 0)
		//				{
		//					Charge_Current = (float)(-0.00806*adc_charge_sum_ave+16.5);
		//				}
		//				else
		//				{
		//					Charge_Voltage = (float)(adc_charge_sum_ave*0.0257080078125);
		//				}
		//
		//				adc_charge_sum_ave = 0;
		//				i=0;
		//			}

		break;

	default:

		break;
	}
}

/**************************************************
 * @note Apply the corresponding battery level based on current voltage
 **************************************************/
void Apply_BatteryPowerFlag(float battery_voltage)
{
	float battVoltages[10] = {4.054, 4.01, 3.908, 3.827, 3.74, 3.651, 3.571, 3.485, 3.38, 3.0}; //P42A
	float p42aBattVoltages[10] = {4.054, 4.01, 3.908, 3.827, 3.74, 3.651, 3.571, 3.485, 3.38, 3.0};
	float dg40BattVoltages[10] = {4.07, 4.025, 3.91, 3.834, 3.746, 3.607, 3.49, 3.351, 3.168, 2.81};
	static uint8_t cell_type_last = 0; //CELL_TYPE P42A equates out to 0

	if (CELL_TYPE != cell_type_last) // Only run once or on change
	{
		switch (CELL_TYPE)
		{
		case P42A:
			memcpy(battVoltages, p42aBattVoltages, sizeof(p42aBattVoltages));
			cell_type_last = 0;
			break;
		case DG40:
			memcpy(battVoltages, dg40BattVoltages, sizeof(dg40BattVoltages));
			cell_type_last = 1;
			break;
		default: // Just in case
			memcpy(battVoltages, p42aBattVoltages, sizeof(p42aBattVoltages));
			cell_type_last = 0;
			break;
		}
	}

	for (int i=0;i<10;i++) {
		if (battery_voltage > battVoltages[i]) {
			Power_Display_Flag = i + 1;
			break;
		}
		// Between zero and min voltage
		if (i == 9) {
			Power_Display_Flag = 10;
		}
	}
}

/**************************************************
 * @note The main task for deyermiming how to display the lights
 **************************************************/
void Conditional_Judgment(void)
{
	float battery_voltage = 0;
	static float battery_voltage_last = 0;

	switch(Power_Flag)
	{
	case 1: // Power on check for charging
		if(Charge_Voltage > CHARGING_VOLTAGE)
		{
			Power_Flag = 3;
			Charge_Flag = 1;
		}
		break;

	case 2: // Boot completed
		if(Usart_Flag == 1)
		{
			Usart_Flag = 2;

			if(data.dutyCycleNow < 0)
			{
				data.dutyCycleNow = -data.dutyCycleNow;
			}
			/*duty cycle > DUTY_CYCLE buzzer beeps*/
			if(data.dutyCycleNow >= DUTY_CYCLE)
			{
				Buzzer_Frequency = ((((uint8_t)(data.dutyCycleNow*100))*4)-220);
			}
			else
			{
				Buzzer_Frequency = 0;
			}
				
			if(ADC1_Val>ADC_THRESHOLD_UPPER || ADC2_Val > ADC_THRESHOLD_UPPER)
			{
				if(data.rpm > VESC_RPM_WIDTH)
				{
					Flashlight_Flag = 2; //Forward
				}
				if(data.rpm < -VESC_RPM_WIDTH)
				{
					Flashlight_Flag = 3; //Backward
				}
			}
			else
			{
				Flashlight_Flag = 4;
			}

			battery_voltage = (data.inpVoltage + 1) / BATTERY_STRING; // Divides pack voltage by cells -- +1 is the correction factor

			if((battery_voltage > (battery_voltage_last+VOLTAGE_RECEIPT)) || (battery_voltage < (battery_voltage_last - VOLTAGE_RECEIPT))) {
				Apply_BatteryPowerFlag(battery_voltage);
				battery_voltage_last = battery_voltage;
			}

			if (data.rpm < 0)
			{
				data.rpm = -data.rpm;
			}

			if(data.avgInputCurrent < 0)
			{
				data.avgInputCurrent = -data.avgInputCurrent;
			}

			if(data.rpm<VESC_RPM)
			{
				if(ADC1_Val < ADC_THRESHOLD_UPPER && ADC2_Val < ADC_THRESHOLD_UPPER)
				{
					Lightbar_Mode_Flag = 1;  //��ʾ����
				}
				else if(ADC1_Val > ADC_THRESHOLD_UPPER && ADC2_Val > ADC_THRESHOLD_UPPER)
				{
					Lightbar_Mode_Flag = 2;  //����ʾ����
					Footpad_Activation_Flag = 3;  //10���ƶ�������
				}
				else if(ADC1_Val >ADC_THRESHOLD_UPPER)
				{
					Lightbar_Mode_Flag = 2;//����ʾ����
					Footpad_Activation_Flag = 1;  //���5������     �Ҳ�5���Ʋ�����
				}
				else
				{
					Lightbar_Mode_Flag = 2;//����ʾ����
					Footpad_Activation_Flag = 2;  //���5���Ʋ����� �Ҳ�5������
				}
			}
			else
			{
				if(data.avgInputCurrent < 0.8 && data.rpm < 6000)
				{
					Lightbar_Mode_Flag = 1; //��ʾ����
				}
				else
				{
					Lightbar_Mode_Flag = 2; //����ʾ����
					Footpad_Activation_Flag = 4; //��10����
				}
			}

			if((Charge_Voltage > CHARGING_VOLTAGE) && (data.avgInputCurrent<0.8))
			{
				if(Charger_Detection_1ms > CHARGER_DETECTION_DELAY)
				{
					if (!ENABLE_POWER_WHILE_CHARGE) {
						Power_Flag = 3;
						Flashlight_Flag = 0;
						Lightbar_Mode_Flag = 0;
					}
					Charge_Flag = 1;
				}

			}
			else
			{
				Charger_Detection_1ms = 0;
			}
			/*
				BOARD TIMEOUT SHUTDOWN COUNTER
				Reset if either footpad is activated or if motor rpm excedes 1000
			*/
			if(ADC1_Val > ADC_THRESHOLD_UPPER || ADC2_Val > ADC_THRESHOLD_UPPER || data.rpm > 1000)
			{
				Shutdown_Time_S = 0;
				Shutdown_Time_M = 0;
			}

			if(Shutdown_Time_S>60000)
			{
				Shutdown_Time_S = 0;

				Shutdown_Time_M++;
				if(Shutdown_Time_M >= SHUTDOWN_TIME)
				{
					Power_Flag = 4;
				}
			}
		}
		break;

	case 3: // VESC shutdown and charger supplying power to board
		if(V_I == 0 && Charge_Time > 150)
		{
			if(Charge_Current < CHARGE_CURRENT && Charge_Current > 0) //if the charger current is between 0 & 0.3A
			//if(Charge_Current > CHARGE_CURRENT_L && Charge_Current < CHARGE_CURRENT_H)
			{
				Shutdown_Cnt++;
				if(Shutdown_Cnt>10)
				{
				//	Charge_Flag = 3;
					Shutdown_Cnt = 0;
					CHARGE_OFF;  //�رճ����
				}
			}
			else
			{
				Shutdown_Cnt = 0;
			}
		}
			else if(Charge_Time > 150)
		{
			battery_voltage = (Charge_Voltage + 1) / BATTERY_STRING; // Divides pack voltage by cells -- +1 is a correction factor - Tony
			if(Charge_Flag == 2)
			{
				if((battery_voltage > (battery_voltage_last+VOLTAGE_RECEIPT)) || (battery_voltage < (battery_voltage_last - VOLTAGE_RECEIPT))) // if average cell voltage has changed by more than .02v
				{
					Apply_BatteryPowerFlag(battery_voltage);
					battery_voltage_last = battery_voltage;
				}
			}
		}

		break;

	case 4:
		break;

	default:
			if(Charge_Voltage > CHARGING_VOLTAGE)
		{
			Power_Flag = 3;
			Charge_Flag = 1;
		}
		break;
			
	}
}
