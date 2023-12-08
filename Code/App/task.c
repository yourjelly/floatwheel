#include "task.h"

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

/**************************************************
 * @brief  :KEY1_Task()
 * @note   :KEY1�1�7�1�7�1�7�1�7
 **************************************************/
void KEY1_Task(void)
{
	switch(KEY1_State)
	{
		case 0:  // No Press return
			return;
		break;

		case 1:  	// Click ~~ Boot
			if(Power_Flag != 2) // If VESC not booted
			{
				Power_Flag = 1;  // Boot VESC
			}	
		break;
		
		case 2:		// Double Click	~~ Increment Light Profile
			if(Power_Flag == 2 || Power_Flag == 3) // If VESC booted
			{
				Change_Light_Profile(true);				
			}
		break;
		
		case 3:		// Long Press ~~ Power Off
			Power_Flag = 4;
			// Flashlight_Flag = 0;
			// Lightbar_Battery_Flag = 2;
		break;
		
		case 4:		// Triple Click ~~ Toggle Buzzer
			if(Power_Flag == 2 || Power_Flag == 3) // If VESC booted
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
 * @brief  :Power_Display()
 * @note   :Displays current power level on lightbar
 **************************************************/
void Power_Display(void)
{
	uint8_t i;
	uint8_t num = 10 - (Power_Display_Flag - 1);

	for (i=0;i<10;i++) {
		if (Power_Display_Flag == 0) {
			// Something is wrong - set all LEDs to red at full brightness
			WS2812_Set_Colour(i, Red, 255);
		} else {
			if (i < num) {
				if (num <= 2) {
					WS2812_Set_Colour(i, Red, Lightbar_Brightness);
				} else {
					WS2812_Set_Colour(i, White, Lightbar_Brightness);
				}
			} else {
				WS2812_Set_Colour(i, Off, 0);
			}
		}
	}

	WS2812_Refresh();
}

/**************************************************
 * @brief  :Sensor_Activation_Display()
 * @note   :Displays current footpad sensor activation on the lightbar
 **************************************************/
void Sensor_Activation_Display(void)
{
	uint8_t i;

	switch(Sensor_Activation_Display_Flag)
	{
		case 1: // adc1>ADC_THRESHOLD_LOWER  adc2<ADC_THRESHOLD_LOWER
			for(i=0;i<5;i++)
			{
				WS2812_Set_Colour(i, Blue, Lightbar_Brightness);
			}
				for(i=5;i<10;i++)
			{
				WS2812_Set_Colour(i, Off, 0);
			}
		break;

		case 2: // adc1<ADC_THRESHOLD_LOWER  adc2>ADC_THRESHOLD_LOWER
			for(i=0;i<5;i++)
			{
				WS2812_Set_Colour(i, Off, 0);
			}
				for(i=5;i<10;i++)
			{
				WS2812_Set_Colour(i, Blue, Lightbar_Brightness);
			}
		break;
		
		case 3: // adc1>ADC_THRESHOLD_LOWER  adc2>ADC_THRESHOLD_LOWER
			for(i=0;i<10;i++)
			{
				WS2812_Set_Colour(i, Blue, Lightbar_Brightness);
			}
		break;

		case 4: // adc1<ADC_THRESHOLD_LOWER  adc2<ADC_THRESHOLD_LOWER
			WS2812_All_Off();
		break;
		
		default:
			WS2812_All_Off();
		break;
	}
	WS2812_Refresh();
}

/**************************************************
 * @brief  :Boot_Animation()
 * @note   :Displays the boot animation on the lightbar
 **************************************************/
void Boot_Animation(void)
{
	uint8_t i;
	uint8_t num = floor(Power_Time / 500) + 1;
	uint8_t rgbMap[10] = {Red, RedOrange, Orange, Yellow, YellowGreen, Green, Cyan, Blue, Violet, Magenta};

	if (num > 10) {
		num = 10;
	}

	for (i=0;i<num;i++) {
		//switch (Config_Boot_Animation) {  Not sure what would happen if no bytes are stored (set to 0xFF?)
		//Then this switch can be implemented
		switch (BOOT_ANIMATION) {
		case NORMAL:
			WS2812_Set_Colour(i,Cyan, Lightbar_Brightness);
			break;
		case RAINBOW:
			WS2812_Set_Colour(i,rgbMap[i], Lightbar_Brightness);
			break;
		}
	}

	for (i = num; i < 10; i++) {
		WS2812_Set_Colour(i, Off, 0);
	}

	WS2812_Refresh();
}

/**************************************************
 * @brief  :WS2812_Cal_Bri()
 * @note   :Brightness pulse effect
 * @param  cnt: 1 count is 200ms
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
		
	return brightness;
}

/**************************************************
 * @brief  :WS2812_Charge()
 * @note   :Shows the current battery % when the board is charging
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
			if (num >= 10) { // Full charged - Set all to green
				WS2812_Set_Colour(i, Green, brightness);
			} else if (num <= 2) { // Low battery - Set first two to red
				WS2812_Set_Colour(i, Red, brightness);
			} else { // Normal charging - All to bright
				WS2812_Set_Colour(i, White, brightness);
			}
		} else {
			WS2812_Set_Colour(i, Off, 0);
		}
	}
	
	cnt++;
	
	if(cnt == 100)
	{
		cnt = 0;
	}
	
	WS2812_Refresh();
}	

/**************************************************
 * @brief  :WS2812_Task()
 * @note   :Main lightbar control task
 **************************************************/
void WS2812_Task(void)
{
	uint8_t i;

	if(WS2812_Counter < 20) // Refresh every 20ms
	{
		return;
	}
	WS2812_Counter = 0;
	
	if(Power_Flag == 0 || (Power_Flag == 3 && Charge_Flag == 0))
	{
			WS2812_All_Off();
			WS2812_Refresh();
			
			Lightbar_Battery_Flag = 0;
			Sensor_Activation_Display_Flag = 0;
			Power_Display_Flag = 0;
			
			return;
	}
	
	if(Power_Flag == 1)
	{
		Boot_Animation();
		return;
	}
	
	if(Charge_Flag == 3) // Battery Fully charged - All LED white
	{
		for(i=0;i<10;i++)
		{
			WS2812_Set_Colour(i, White, 255);
		}
		return;
	}
	
	if(Charge_Flag == 2) // Charging call charge function
	{
		WS2812_Charge();
		return;
	}
	
	switch(Light_Profile)
	{
		case 1: // Low Headlights; High lightbar
			Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_HIGH;
		break;
		
		case 2:	// Medium headlights; Medium lightbar
			Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_MED;
		break;
		
		case 3: // High headlights; Low lightbar
			Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_LOW;
		break;
		
		default:
			
		break;
	}
	
	if(Lightbar_Battery_Flag == 1)  // Display Battery level
	{
		Power_Display();
	}
	else // Display Sensor Activation
	{
		Sensor_Activation_Display();
	}
	
}

/**************************************************
 * @note Apply the corresponding battery level based on current voltage
 * @param battery_volatage float of the voltage of the battery cells
 **************************************************/
void Apply_BatteryPowerFlag(float battery_voltage)
{
	float battVoltages[2][10] = {{4.054,	4.01,	3.908,	3.827,	3.74,	3.651,	3.571,	3.485,	3.38,	3.0}, //P42A discharge
								 {4.200,	4.19,	4.18,	4.15,	4.108,	4.002,	3.894,	3.79,	3.703,	3.279}}; //P42A charge
	float cellcurvdischarge[2][10] = {{4.054,	4.01,	3.908,	3.827,	3.74,	3.651,	3.571,	3.485,	3.38,	3.0},   //P42A
								      {4.07,	4.025,	3.91,	3.834,	3.746,	3.607,	3.49,	3.351,	3.168,	2.81}}; //DG40
	float cellcurvcharge[2][10] = {{4.2,	4.19,	4.18,	4.15,	4.108,	4.002,	3.894,	3.79,	3.703,	3.279},   //P42A
								   {4.18,	4.15,	4.1,	4.048,	3.966,	3.878,	3.783,	3.681,	3.603,	3.51}}; //DG40
	static uint8_t cell_type_last = 0; //CELL_TYPE P42A equates out to 0

	if (CELL_TYPE != cell_type_last) // If !P42a run once at boot or on change
	{
		cell_type_last = CELL_TYPE;
		for (int i=0;i<10;i++)
		{
			battVoltages[0][i] = cellcurvdischarge[cell_type_last][i];
			battVoltages[1][i] = cellcurvcharge[cell_type_last][i];
		}
	}

	if (Power_Flag == 3) //if charging
	{
		for (int i=0;i<10;i++) {
			if (battery_voltage > battVoltages[1][i]) {
				Power_Display_Flag = i + 1;
				break;
			}
			// Between zero and min voltage
			if (i == 9) {
				Power_Display_Flag = 10;
			}
		}
	}
	else
	{
		for (int i=0;i<10;i++) {
			if (battery_voltage > battVoltages[0][i]) {
				Power_Display_Flag = i + 1;
				break;
			}
			// Between zero and min voltage
			if (i == 9) {
				Power_Display_Flag = 10;
			}
		}
	}
}

/**************************************************
 * @brief  :Power_Task()
 * @note   :Sets appropriate flags for current power state
 **************************************************/
void Power_Task(void)
{
	static uint8_t power_flag_last = 0; // Previous state
	static uint8_t power_step = 0;
	
	if(power_flag_last == Power_Flag && Power_Flag != 1)
	{
		return;
	}
	power_flag_last = Power_Flag;
	
	switch(Power_Flag)
	{
		case 1:// VESC Booting
			PWR_ON;
			Flashlight_Flag = 1;
			switch(power_step)
			{
				case 0: // Wating to boot or boot finished
					Power_Time = 0;
					power_step = 1;
				break;
				
				case 1: // Boot in progress
					if(Power_Time > VESC_BOOT_TIME) // Wait timer for VESC to boot
					{
						Power_Flag = 2;		// VESC Boot Complete
						Light_Profile = 1;	// Set light profile to Low
						Buzzer_Flag = 2;	// Set buzzer to on
						power_step = 0;		// Reset boot progress
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
		
		case 3: // Powered by charger
			if (POWER_VESC_ON_CHARGER != true)
			{
				Power_Flag = 4;
			}
		break;
		
		case 4: // Signal to power board off
			Flashlight_Flag = 0;
			Lightbar_Battery_Flag = 2;
			PWR_OFF;
		break;

		default:
			
		break;
	}
}

/**************************************************
 * @brief  :Charge_Task()
 * @note   :Sets appropriate flags for current charging state
 **************************************************/
void Charge_Task(void)
{
	static uint8_t charge_step = 0; 
		
	if(Charge_Flag == 0)
	{
		return;
	}else if(Charge_Flag == 3)
	{
		CHARGE_OFF;  // Stop charging
		return;
	}
	
	switch(charge_step)
	{
		case 0:
			Charge_Time = 0;
			charge_step = 1;
		break;
		
		case 1:
			if(Charge_Time > 1000)  // Delay 1 second
			{
				CHARGE_ON;  // Enable charging
				Charge_Flag = 2;
				Charge_Time = 0;
				charge_step = 2; // Charge sampling
			}
		break;
			
		case 2:	// Current Sampling
			if(Charge_Time > 500) // If sampling for longer than 500ms switch type
			{
				Charge_Time = 0;
				if(V_I == 0) // If Current sampling switch to Volatge
				{
					V_I = 1;
					LED1_ON;
				}else // Else Voltage sampling switch to Current
				{
					V_I = 0;
					LED1_OFF;
				}
			}
		break;
		
		default:
			
		break;
		
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
	if (Flashlight_Flag == 4)
	{
		Flashlight_Flag = 2;
	}
	Set_Light_Brightness();
}

/// Only call the when the Light_Profile is changed (either via button or bluetooth) or after first eeprom read
/// Allows runtime bluetooth changes on Lightbar_Brightness and Main_Brightness
/**
* @note Sets light brightness to the current Light_Profile
**/
void Set_Light_Brightness()
{
	switch (Light_Profile)
	{
	// Inversely set the brightness of the lightbar to the brightness of the main lights
	case 1: // Low
		Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_HIGH;
		Main_Brightness = MAIN_BRIGHTNESS_LOW;
		break;

	case 2: // Med
		Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_MED;
		Main_Brightness = MAIN_BRIGHTNESS_MED;
		break;

	case 3: // High
		Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_LOW;
		Main_Brightness = MAIN_BRIGHTNESS_HIGH;
		break;

	default:
		Lightbar_Brightness = LIGHTBAR_BRIGHTNESS_HIGH;
		Main_Brightness = MAIN_BRIGHTNESS_LOW;
		break;
	}
}

/**
 * @note transitions from current brightness to end brightness
 * @param target ending brightness
 * @param time transition time in ms
 **/
void Light_Transition(uint16_t target, uint16_t time)
{
	static uint16_t brightness = 9999;
	uint16_t diff = 0;
	int mod = 1;
	if (Brightness_Adjustment_Flag == 1) // if target has changed restart timer
	{
		Brightness_Adjustment_Flag = 2;
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
		Brightness_Adjustment_Flag = 3;
		return;
	}
	if (Flashlight_Time % FADE_REFRESH == 0)
	{
		brightness = brightness + (roundf((Flashlight_Time * diff) / time) * mod);
		TIM_SetCompare2(TIM1, brightness);
		time -= FADE_REFRESH;
	}
	if (Brightness_Adjustment_Flag == 2)
	{
		Light_Transition(target, time);
	}
}

/**************************************************
 * @brief  :Flashlight_Bright()
 * @note   :Flashlight brightness control
 * @param direction 1=Forward 2=Reverse  
 * @param bright 1=transition to REST 2=transition from 10% to 100%
 **************************************************/
void Flashlight_Bright(uint8_t direction, uint8_t bright)
{
	// static uint8_t direction_last = 0;
	if (Brightness_Adjustment_Flag == 2)
	{ 
		Brightness_Adjustment_Flag = 3;
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
		if (Main_Brightness > MAIN_BRIGHTNESS_REST) {  // If your light profile is less than rest use that **higher number is dimmer**
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
 * @brief  :Flashlight_Task()
 * @note   :Controls headlight/tail light brightness multiplier and direction
 **************************************************/
void Flashlight_Task(void)
{
	static uint8_t flashlight_flag_last = 0;

	if(Power_Flag != 2) //When just booting or vesc is off turn lights off
	{
		LED_B_OFF;
		LED_F_OFF;
		Flashlight_Flag = 0;
		// TIM_SetCompare2(TIM1,9999);
		return;
	}

	if(flashlight_flag_last == Flashlight_Flag) //Direction isnt changed
	{
		return; //Dont do anything -> state didnt change
	}
	else if(flashlight_flag_last != Flashlight_Flag)
	{
		flashlight_flag_last = Flashlight_Flag;
		Brightness_Adjustment_Flag = 3; // Breaks out of current transition to prepare for the next

	}
	switch(Flashlight_Flag)
	{
		case 0: // Lights out
			Light_Transition(9999, FADE_TIME);
		break;
		
		case 1: // VESC booting -> 0% -> 10% in 2 sec
			Flashlight_Bright(1,1);
		break;

		case 2: // Riding forward
			Flashlight_Bright(1,2);
		break;

		case 3: // Riding reverse
			Flashlight_Bright(2,2);
		break;

		case 4: // Dim at rest
			Flashlight_Bright(1,1);
		break;

		default:
		break;
	}
}

/**************************************************
 * @brief  : Flashlight_Detection()
 * @note   : Controls lightS activity on profile change and allows to dim
 **************************************************/
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
			Flashlight_Detection_Time = 0;
			Set_Light_Brightness();
			Light_Transition(Main_Brightness,FADE_TIME);
			Light_Profile_last = Light_Profile;
		}
	}
}

/**************************************************
 * @brief  :Buzzer_Task()
 * @note   :Controls the buzzer
 **************************************************/
void Buzzer_Task(void)
{
	static uint8_t buzzer_step = 0;
	static uint8_t Light_Profile_last = 0;
	static uint8_t ring_frequency = 0;
	static uint16_t sound_frequency = 0;

	if (Config_Buzzer == VESC) {
		return; //Do nothing with the buzzer, let the vesc handle everything -> when changed to vesc make sure to initialy set buzzer off
	}

	if(Power_Flag != 2 || Buzzer_Flag == 1 || Config_Buzzer == OFF) // If buzzer is set to off turn off
	{
		BUZZER_OFF;
		buzzer_step = 0;
		return;
	}
	
	if(Buzzer_Frequency == 0 && Light_Profile_last == Light_Profile) // If the frequency is zero and the light profile hasnt changed
	{
		BUZZER_OFF;
		buzzer_step = 0;
		return;
	}
	
	if(Buzzer_Frequency != 0) // this triggers only on dutycycle
	{
		switch(buzzer_step)
		{
			case 0:
				sound_frequency = (uint16_t)(-2.78F*Buzzer_Frequency+666);
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
					if(ring_frequency >= Light_Profile)
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
 * @brief  :Usart_Task()
 * @note   :Sends commands to VESC controller to get data
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
				
				if(result == 0) //�1�7�1�7�1�7�1�7�1�7�0�6�1�7
				{
						//LED1_Filp_Time(500);				
						Usart_Flag = 1;
						//Battery_Voltage = data.inpVoltage; //�1�7�1�7�1�3�1�7�1�7?
						//AvgInputCurrent = data.avgInputCurrent;  //�0�0�1�7�1�1�1�7�1�7�1�7
						//DutyCycleNow = data.dutyCycleNow;   //�0�8�1�7�0�7�1�7
				}
				else	//�1�7�1�7�1�7�1�7�0�2�1�7�1�7
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
 * @brief  :ADC_Task()
 * @note   :Sets appropriate flags for current ADC/footpad sensor state
 **************************************************/
void ADC_Task(void)
{
	static uint8_t adc_step = 0;
	//static uint8_t i = 0;
	//static uint16_t adc_charge_sum[10];
	static uint16_t adc_charge_sum_ave = 0;
	static uint16_t adc1_val_sum_ave = 0;
	static uint16_t adc2_val_sum_ave = 0;
	//float old_charge_current = 0;
	
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
				
				ADC1_Val = (float)(adc1_val_sum_ave*0.0012890625F);
				ADC2_Val = (float)(adc2_val_sum_ave*0.0012890625F);
				
				if(V_I == 0)
				{
					if(Charge_Time>100)
					{
						Charge_Current = (float)(-0.008056640625F*adc_charge_sum_ave+16.5F);
						//Charge_Current = Charge_Current*k + old_charge_current*(1-k);
						//old_charge_current = Charge_Current;
					}
				}
				else
				{
					if(Charge_Time>100)
					{
						Charge_Voltage = (float)(adc_charge_sum_ave*0.0257080078125F);
					}
				}
			}
			
		break;
			
	  default:
			
		break;
	}
}

/**************************************************
 * @brief  :Conditional_Judgment()
 * @note   :The main task for determining how to display the lights
 **************************************************/
void Conditional_Judgment(void)
{
	float battery_voltage = 0;
	static float battery_voltage_last = 0;
	float abs_rpm = 0;
			
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
				
				if(data.rpm>0){
					abs_rpm = data.rpm;
				}else{
					abs_rpm = -data.rpm;
				}

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
						Flashlight_Flag = 2; // Forward
					}
					if(data.rpm < -VESC_RPM_WIDTH)
					{
						Flashlight_Flag = 3; // Backward
					}
				}
				else
				{
					Flashlight_Flag = 4;
				}
				
				battery_voltage = (data.inpVoltage+1)/BATTERY_STRING; // Divides pack voltage by cells -- +1 is the correction factor
				
				if((battery_voltage > (battery_voltage_last+VOLTAGE_RECEIPT)) || (battery_voltage < (battery_voltage_last - VOLTAGE_RECEIPT))) {
					Apply_BatteryPowerFlag(battery_voltage);
					
					battery_voltage_last = battery_voltage;
				}
				
				if(data.avgInputCurrent < 0)
				{
					data.avgInputCurrent = -data.avgInputCurrent;
				}

				if(data.rpm < SENSOR_ACTV_DISPLAY_RPM) //Below this RPM show footpad activation on Lightbar
				{
					if(ADC1_Val < ADC_THRESHOLD_UPPER && ADC2_Val < ADC_THRESHOLD_UPPER)
					{
						Lightbar_Battery_Flag = 1; // Display Battery Level
					}
					else if(ADC1_Val > ADC_THRESHOLD_UPPER && ADC2_Val > ADC_THRESHOLD_UPPER)
					{
						Lightbar_Battery_Flag = 2; // Do not Display Battery Level
						Sensor_Activation_Display_Flag = 3;  // Both Footpad Sensors Activated
					}
					else if(ADC1_Val >ADC_THRESHOLD_UPPER)
					{
						Lightbar_Battery_Flag = 2; // Do not Display Battery Level
						Sensor_Activation_Display_Flag = 1;  // Left Footpad Sensor Activated
					}
					else
					{
						Lightbar_Battery_Flag = 2; // Do not Display Battery Level
						Sensor_Activation_Display_Flag = 2;  // Right Footpad Sensor Activated
					}
				}
				else 
				{
					if(data.avgInputCurrent < 0.8F && abs_rpm < LIGHTBAR_SPEEDGATE_ON) // Conditions for showing battery level
					{
						Lightbar_Battery_Flag = 1;
					}
					else if(abs_rpm > LIGHTBAR_SPEEDGATE_OFF) // Too fast no distracting leds
					{
						Lightbar_Battery_Flag = 2;
						Sensor_Activation_Display_Flag = 4; // sus TODO
					}
				}
				
				if((Charge_Voltage > CHARGING_VOLTAGE) && (data.avgInputCurrent<0.8F))
				{
					if(Charger_Detection_Timer > 1000) // If charger detection timer reaches 1000ms (1s), initiate charging
					{
						Power_Flag = 3;
						Charge_Flag = 1;
						Flashlight_Flag = 0;
						Lightbar_Battery_Flag = 0;
					}
					
				}
				else
				{
					Charger_Detection_Timer = 0;
				}
				/*
					BOARD TIMEOUT SHUTDOWN COUNTER
					Reset if either footpad is activated or if motor rpm excedes 1000
				*/
				if(ADC1_Val > ADC_THRESHOLD_UPPER || ADC2_Val > ADC_THRESHOLD_UPPER || abs_rpm > 1000)
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
						Power_Flag = 3;
					}
				}
			}
		break;
		
		case 3: // Charging shutdown and charger supplying power to board
			if(V_I == 0 && Charge_Time > 150)
			{
				if(Charge_Current > 0 && Charge_Current < CHARGE_SHUTOFF_CURRENT) // if the charger current is between 0 & 0.3A
				{
					Shutdown_Cnt++;
					if(Shutdown_Cnt>10) // 10 count to stop charging (ms?)
					{
						Charge_Flag = 3;
						Shutdown_Cnt = 0;
					}
				}
				else
				{
					Shutdown_Cnt = 0;
				}
			}
			else if(Charge_Time > 150)
			{
				battery_voltage = (Charge_Voltage+1)/BATTERY_STRING; // Divides pack voltage by cells -- +1 is a correction factor - Tony
				if(Charge_Flag == 2)
				{
					if((battery_voltage > (battery_voltage_last+VOLTAGE_RECEIPT)) || (battery_voltage < (battery_voltage_last - VOLTAGE_RECEIPT))) {
						Apply_BatteryPowerFlag(battery_voltage);
						battery_voltage_last = battery_voltage;
					}
				}
			}

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
