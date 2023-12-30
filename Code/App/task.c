#include "task.h"

/**************************************************
+ * Reset LCM Config
+ **************************************************/
static void lcmConfigReset(void)
{
	lcmConfig.isSet = false;
	lcmConfig.headlightBrightness = 0;
	lcmConfig.statusbarBrightness = 30;
	lcmConfig.statusbarMode = 0;
	lcmConfig.dutyBeep = 90;
	lcmConfig.boardOff = 0;
	errCode = 0;
}

// brightnesses for Light Profile 1, 2, 3:
int headlight_brightnesses[] = { LIGHTBAR_BRIGHTNESS_LOW, LIGHTBAR_BRIGHTNESS_MED, LIGHTBAR_BRIGHTNESS_HIGH };
int status_brightnesses[] = { MAIN_BRIGHTNESS_HIGH, MAIN_BRIGHTNESS_MED, MAIN_BRIGHTNESS_LOW };

/**************************************************
 * @brief  :KEY1_Task()
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
			if (Power_Flag < 3) {
				lcmConfigReset();
			}
			Power_Flag = 4;
			Power_Time = 0;
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
 * @note:	Displays current power level on lightbar
 * @param:	brightness (0-255)
 **************************************************/
static void Power_Display(uint8_t brightness)
{
	uint8_t numleds = 11 - Power_Display_Flag;
	// 20% and below: red
	// 40% and below: yellow
	// > 40% white
	uint8_t ledcolour = Red; 
	if (numleds > 2) {
		ledcolour = Yellow;
	}
	if (numleds > 4) {
		ledcolour = White;
	}
	if (Power_Display_Flag > 0) {
		Lightbar_Set_Colour_Range(0, numleds, ledcolour, brightness);
	}
	else { // Error lights
		Lightbar_Set_Colour_Range(5, 6, Red, brightness);
	}

	Lightbar_Refresh();
}

/**************************************************
 * @note   :Displays current footpad sensor activation on the lightbar
 **************************************************/
static void Sensor_Activation_Display(void)
{
	uint8_t i;

	switch(Sensor_Activation_Display_Flag)
	{
		case 1:	// Left Foot Sensors
			if (data.state == DISABLED) {
				Disabled_Animation();
				return;
			}
			Lightbar_Set_Colour_Range(1, 5, indicator_color, Lightbar_Brightness);
			WS2812_Refresh();
		break;
		
		case 2:	// Right Foot Sensor
			if (data.state == DISABLED) {
				Disabled_Animation();
				return;
			}
			Lightbar_Set_Colour_Range(6, 10, indicator_color, Lightbar_Brightness);
			WS2812_Refresh();
		break;
		
		case 3:	// Both Foot Sensors
			if (data.state == DISABLED) {
				Disabled_Animation();
				Buzzer_Frequency = 100;
				return;
			}
			Lightbar_Set_Colour_Range(1, 10, indicator_color, Lightbar_Brightness);
			WS2812_Refresh();
		break;
			
		default: // Riding
			Lightbar_VESC();
		break;
	}
}

/**************************************************
 * @note   :Brightness pulse effect
 * @param  cnt: 1 count is 200ms
 **************************************************/
uint8_t Lightbar_Pulse(uint8_t cnt)
{
	static uint8_t brightness = 1;

	// Update brightness
	if(cnt < 50) {
		brightness++;
	}
	else {
		brightness--;
	}
	
	// Clamp brightness
	if(brightness < 1) {
		brightness = 1;
	}	
	else if(brightness > 50) {
		brightness = 50;
	}
		
	return brightness;
}

/**************************************************
 * @note   :Shows the current battery % when the board is charging
 **************************************************/
void Lightbar_Charge(void)
{
	static uint8_t cnt = 0;
	if (Power_Display_Flag == 1) {
		Lightbar_Set_Colour_Range(1, 10, Green, Lightbar_Pulse(cnt));

		Lightbar_Refresh();
	}
	else {
		Power_Display(Lightbar_Pulse(cnt));
	}

	cnt++;

	if(cnt == 100)
	{
		cnt = 0;
	}
}	

/**************************************************
 * @note   :Displays the boot animation on the lightbar
 **************************************************/
void Boot_Animation(void)
{
	Animation_Running = 1;
	
	uint8_t i;
	uint8_t num = floor(Power_Time / 100) + 1;
	uint8_t rgbMap[3][10] = {{Cyan, Cyan, Cyan, Cyan, Cyan, Cyan, Cyan, Cyan, Cyan, Cyan}, // STOCK
							{Red, RedOrange, Orange, Yellow, YellowGreen, Green, Cyan, Blue, Violet, Magenta}, // RAINBOW
							{Red, White, Blue, Red, White, Blue, Red, White, Blue, Red}}; // RWB

	if (num > 10) {
		num -= 10;
	}

	for (i=0;i<num;i++) {
		Lightbar_Set_Colour(i, rgbMap[BOOT_ANIMATION][i], LIGHTBAR_BRIGHTNESS_MED);
	}
	
	Lightbar_Set_Colour(num, rgbMap[BOOT_ANIMATION][num], LIGHTBAR_BRIGHTNESS_HIGH);

	for (i = num + 1; i < 10; i++) {
		Lightbar_Set_Colour(i, rgbMap[BOOT_ANIMATION][i], LIGHTBAR_BRIGHTNESS_LOW);
	}

	Lightbar_Refresh();

	if (Power_Time < VESC_BOOT_TIME){
		Animation_Running = 0;
	}
}

/**************************************************
 * @note   :Displays the shutdown animation on the lightbar
 **************************************************/
void Shutdown_Animation(void)
{
	Animation_Running = 2;

	uint8_t brightness = 100;
	uint8_t num = 10 - floor(Power_Time / 100);

	if (num < 1) {
		num = 1;
	}

	Lightbar_Set_Colour_Range(num, num, Red, brightness / (11 - num));

	Lightbar_Refresh();

	if (Power_Time < 1000){
		Animation_Running = 0;
	}
}

/**************************************************
 * @note   :Displays disabled animation on the lightbar
 **************************************************/
static void Disabled_Animation(void)
{
	Animation_Running = 3;

	uint8_t brightness = Lightbar_Brightness;
	if (brightness < 20)
		brightness = 20;

	uint8_t tick = Power_Time % 100;
	
	if (tick < 70) {
		Lightbar_All_Off();
	}
	else {
		Lightbar_Set_Colour_Range(5, 6, Red, brightness); // 2 red LEDs in the center
	}

	Lightbar_Refresh();

	if (Power_Time < 1000){
		Animation_Running = 0;
	}
}

/**************************************************
 * @note:	Displays an idle animation on the lightbar
 **************************************************/
static void Idle_Animation(void)
{
	Animation_Running = 4;

	static int cnt = 0;
	cnt++;
	if(cnt == 8 * 512)
	{
		cnt = 0;
	}
	if ((cnt % 80) == 0) {
		int r, g, b;
		int div = cnt >> 3; // fast div by 8
		int idx = div % 10;
		int clr = div % 13;
		int brightness = div % 100 + 100
		Lightbar_Set_Colour_Range(idx, idx, clr, brightness);
		Lightbar_Refresh();
	}

	if (Shutdown_Time_M < IDLE_TIME){
		Animation_Running = 0;
	}
}

/**************************************************
 * @note:	Display handtest indicators on the lightbar
 **************************************************/
static void Lightbar_Handtest(void)
{
	static uint8_t pulsate = 0;
	uint8_t brightness = Lightbar_Brightness;
	if (brightness < 20)
		brightness = 20;
	pulsate++;
	if (pulsate > 50)
		pulsate = 0;

	// 4 LEDs in the center
	Lightbar_Set_RGB(4, 7, brightness, pulsate, 0);

	//SUS will these turn off? Replaced with sensor flag switch
	// if(ADC1_Val > ADC_THRESHOLD_LOWER) // Left ADC check - light first led green 
	// {
	// 	Lightbar_Set_Colour(0, Green, brightness);
	// }

	// if(ADC2_Val > ADC_THRESHOLD_LOWER) // Right ADC check - light last led green
	// {
	// 	Lightbar_Set_Colour(9, Green, brightness);
	// }

	switch(Sensor_Activation_Display_Flag)
	{
		case 1:	// Left Foot Sensor
			Lightbar_Set_Colour(0, Green, brightness);
		break;
		
		case 2:	// Right Foot Sensor
			Lightbar_Set_Colour(9, Green, brightness);
		break;

		case 3:	// Both Foot Sensors
			Lightbar_Set_Colour(0, Green, brightness);
			Lightbar_Set_Colour(9, Green, brightness);
		break;

		default:
			Lightbar_Set_Colour(0, Off, brightness);
			Lightbar_Set_Colour(9, Off, brightness);
		break;
	}

	Lightbar_Refresh();
}

/**************************************************
 * @note   :Main lightbar control task
 **************************************************/
void Lightbar_Task(void)
{
	uint8_t i;
	
	if(WS2812_Counter < 20) // Refresh every 20ms
	{
		return;
	}
	WS2812_Counter = 0;
	
	Set_Light_Brightness()

	switch(Animation_Running)
	{
		case 1:
			Boot_Animation();
			return;
		break;

		case 2:
			Shutdown_Animation();
			return;
		break;

		case 3:
			Disabled_Animation();
			return;
		break;

		case 4:
			Idle_Animation();
			return;
		break;
		
		default:
		break;
	}

	if(Power_Flag == 0 || (Power_Flag == 3 && Charge_Flag == 0)) // Board is off lights should be off
	{
			Lightbar_All_Off();
			Lightbar_Refresh();
			
			Lightbar_Battery_Flag = 0;
			Sensor_Activation_Display_Flag = 0;
			Power_Display_Flag = 0;
			
			return;
	}
	
	if(Charge_Flag == 3) // Battery Fully charged - All LED white
	{
		Lightbar_Set_RGB_Range(1, 10, 50, 150, 50); // white with a strong green tint
		Lightbar_Refresh();
	}
	
	if(Charge_Flag == 2) // Charging call charge function
	{
		Lightbar_Charge();
		return;
	}

	if(Power_Flag != 2)
	{
		return;
	}
	
	if (data.isHandtest) {
		Lightbar_Handtest();
		return;
	}
	
	Sensor_Activation_Display();
}

/**************************************************
 * @note   : Display VESC status & Incorportaes Foot Sensors
 **************************************************/
static void Lightbar_VESC(void)
{
	uint8_t i;
	uint8_t indicator_color = Blue;
	if (data.floatPackageSupported) {
		// make footpad indicators purple if float package commands are received successfully!
		indicator_color = Violet;
	}
	
	if(Lightbar_Battery_Flag == 1)  // Display Battery level
		{
			Power_Display(Lightbar_Brightness);
			return;
		}

	switch(Sensor_Activation_Display_Flag)
	{
		case 1:	// Left Foot Sensors
			if (data.state == DISABLED) {
				Disabled_Animation();
				return;
			}
			Lightbar_Set_Colour_Range(1, 5, indicator_color, Lightbar_Brightness);
		break;
		
		case 2:	// Right Foot Sensor
			if (data.state == DISABLED) {
				Disabled_Animation();
				return;
			}
			Lightbar_Set_Colour_Range(6, 10, indicator_color, Lightbar_Brightness);
		break;
		
		case 3:	// Both Foot Sensors
			if (data.state == DISABLED) {
				Disabled_Animation();
				Buzzer_Frequency = 100;
				return;
			}
			Lightbar_Set_Colour_Range(1, 10, indicator_color, Lightbar_Brightness);
		break;
			
		case 4: // Riding
			if (data.state != RUNNING_WHEELSLIP) {
				uint8_t brightness = lcmConfig.isSet ? lcmConfig.statusbarBrightness : Lightbar_Brightness;

				if (Power_Display_Flag > 8) {
					// Voltage below 30%?
					// Display red leds at full brightness above anything else
					Power_Display(255);
				}
				else if (data.dutyCycleNow > 90) {
					Lightbar_Set_Colour_Range(1, 10, Red, brightness);
				}
				else if (data.dutyCycleNow > 85) {
					Lightbar_Set_Colour_Range(1, 9, Red, brightness);
				}
				else if (data.dutyCycleNow > 80) {
					Lightbar_Set_Colour_Range(1, 8, Orange, brightness);
				}
				else if (data.dutyCycleNow > 70) {
					Lightbar_Set_Colour_Range(1, 7, Yellow, brightness/2);
				}
				else if (data.dutyCycleNow > 60) {
					Lightbar_Set_Colour_Range(1, 6, Green, brightness/3);
				}
				else if (data.dutyCycleNow > 50) {
					Lightbar_Set_Colour_Range(1, 5, Green, brightness/4);
				}
				else if (Power_Display_Flag > 6) {
					// Voltage below 40%?
					// Display yellow leds at full brightness
					Power_Display(255);
				}
				else {
					Lightbar_Set_Colour_Range(1, 10, Off, brightness);
				}
			}
			else {
				Lightbar_Set_Colour_Range(1, 10, Off, brightness);
			}
		break;

		case 5:
			// Flywheel Mode: just a rando pattern fpr now
			uint8_t pos = (Power_Time/100) % 10;
			uint8_t colour = (Power_Time + pos) % 13;
			uint8_t bright = (Power_Time + colour) % 255;
			Lightbar_Set_Colour(pos, color, bright);
		break;

		default:
			if (errCode == 0)
				errCode = 1;
			//Lightbar_Set_Colour_Range(9, 10, Red, 50);
		break;
	}
	Lightbar_Refresh();
}

/**************************************************
 * @note Apply the corresponding battery level based on current voltage
 * @param battery_volatage float of the voltage of the battery cells
 **************************************************/
void CheckPowerLevel(float battery_voltage)
{
	float battVoltages[2][10] =		 {{4.054,	4.01,	3.908,	3.827,	3.740,	3.651,	3.571,	3.485,	3.380,	3.000},		//P42A discharge
									  {4.200,	4.19,	4.180,	4.150,	4.108,	4.002,	3.894,	3.790,	3.703,	3.279}};	//P42A charge
	float cellcurvdischarge[2][10] = {{4.054,	4.010,	3.908,	3.827,	3.740,	3.651,	3.571,	3.485,	3.380,	3.00},		//P42A
									  {4.070,	4.025,	3.910,	3.834,	3.746,	3.607,	3.490,	3.351,	3.168,	2.81}};		//DG40
	float cellcurvcharge[2][10] =	 {{4.20,	4.19,	4.18,	4.150,	4.108,	4.002,	3.894,	3.790,	3.703,	3.279},		//P42A
									  {4.18,	4.15,	4.10,	4.048,	3.966,	3.878,	3.783,	3.681,	3.603,	3.510}};	//DG40
	static uint8_t cell_type_last = 0; //CELL_TYPE P42A equates out to 0
	
	bool is_charging = false;

	if (Power_Flag == 3) // If charging
	{
		is_charging = true;
	}

	if (CELL_TYPE != cell_type_last) // If !P42a run once at boot or on change
	{
		cell_type_last = CELL_TYPE;
		for (int i=0;i<10;i++)
		{
			battVoltages[0][i] = cellcurvdischarge[CELL_TYPE][i];
			battVoltages[1][i] = cellcurvcharge[CELL_TYPE][i];
		}
	}

	for (int i=0;i<10;i++) {
		if (battery_voltage > battVoltages[is_charging][i]) {
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
					Boot_Animation();
				break;
				
				case 1: // Boot in progress
					if(Power_Time > VESC_BOOT_TIME) // Wait timer for VESC to boot
					{
						Power_Flag = 2;		// VESC Boot Complete
						Light_Profile = 1;	// Set light profile to Low
						Buzzer_Flag = 2;	// Set buzzer to on
						power_step = 0;		// Reset boot progress
						Lightbar_Battery_Flag = 1;
						Set_Light_Brightness();
					}
				break;
			}
			
		break;	
		
		case 3: // Powered by charger
			Flashlight_Flag = 0;
			Lightbar_Battery_Flag = 0;
			if (POWER_VESC_ON_CHARGER == false)
			{
				Power_Time = 0;
				Disabled_Animation();
				PWR_OFF;
			}
		break;
		
		case 4: // Signal to power board off
			Power_Time = 0;
			Shutdown_Animation();
			PWR_OFF;
			Power_Flag = 0;
		break;

		default:
			
		break;
	}
}

/**************************************************
 * @note   :Sets appropriate flags for current charging state
 **************************************************/
void Charge_Task(void)
{
	static uint8_t charge_step = 0; 
		
	if(Charge_Flag > 0 && Charge_Time > 150)
	{
		if(V_I == 0)
		{
			if(Charge_Current > 0 && Charge_Current < CHARGE_SHUTOFF_CURRENT) // if the charger current is between 0 & 0.3A
			{
				Charge_Flag = 3;
				Shutdown_Cnt++;
				if(Shutdown_Cnt>10) // 10 count to stop charging (ms?)
				{
					Charge_Time = 0;
					V_I = 1;
					LED1_ON; // Use ADC3 to measure charge voltage
					CHARGE_OFF;
				}
			}
			else
			{
				Shutdown_Cnt = 0;
			}
		}
		else
		{
			if(Charge_Flag == 2)
			{
				CheckPowerLevel((Charge_Voltage+1)/BATTERY_STRING);
			}
			if((Charge_Flag == 3) && (Shutdown_Cnt > 10))
			{
				if (Charge_Voltage < CHARGING_VOLTAGE)
				{
					// wait for charger to get unplugged to reset back to normal state
					Charge_Flag = 0;
					charge_step = 0;
					Charge_Voltage = 0;
					Charger_Detection_1ms = 0;
				}
			}
		}
	}
	else // Charge_Flag == 0
	{
		charge_step = 0;
		if(Charge_Voltage > CHARGING_VOLTAGE)// && (Charge_Current > 0.1))
		{
			if(Charger_Detection_1ms > CHARGER_DETECTION_DELAY)
			{
				if (Charge_Flag != 2)
					Charge_Flag = 1;
				Lightbar_Battery_Flag = 0;
			}
		}
		else {
			Charger_Detection_1ms = 0;
			return;
		}
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
 **************************************************/
void Change_Light_Profile(void)
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
/**************************************************
* @note Sets light brightness to the current Light_Profile
**************************************************/
void Set_Light_Brightness(void)
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

/**************************************************
 * @note transitions from current brightness to end brightness
 * @param target ending brightness
 * @param time transition time in ms
 **************************************************/
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
		LED_F_ON;
		LED_B_OFF;
	}
	else // Direction backward
	{
		LED_B_ON;
		LED_F_OFF;
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
 * @note   : Controls lights activity on profile change and allows to dim
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
 * @note   :Controls the buzzer
 **************************************************/
void Buzzer_Task(void)
{
	static uint8_t buzzer_step = 0;
	static uint8_t Light_Profile_last = 0;
	static uint8_t ring_frequency = 0;
	static uint16_t sound_frequency = 0;

	if(Power_Flag != 2 || Buzzer_Flag == 1) // If buzzer is set to off turn off
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
 * @note   :Sends commands to VESC controller to get data
 **************************************************/
void Usart_Task(void)
{
	static uint8_t usart_step = 0;
	static uint8_t alternate = 0;
	uint8_t result;

	if(Power_Flag != 2)
	{
		// legacy/motor data
		data.rpm = 0;
		data.dutyCycleNow = 0;
		data.avgInputCurrent = 0;
		data.inpVoltage = 0;

		// float package data
		data.floatPackageSupported = false;
		data.state = 255;
		data.fault = 0;
		data.isForward = true;

		lcmConfigReset();

		usart_step = 0;
		
		return;
	}
	
	switch(usart_step)
	{
		case 0:
			// Try the custom app command for the first 2 seconds then fall back to generic GET_VALUES
			if ((data.floatPackageSupported == false) && (Power_Time > VESC_BOOT_TIME * 2))
				Get_Vesc_Pack_Data(COMM_GET_VALUES);
			else {
				if (alternate) {
					Get_Vesc_Pack_Data(COMM_CUSTOM_DEBUG);
				}
				else {
					Get_Vesc_Pack_Data(COMM_CUSTOM_APP_DATA);
				}
			}

			alternate = 1 - alternate;

			usart_step = 1;
		break;
		
		case 1:
			if(VESC_RX_Flag == 1)
			{
				VESC_RX_Flag = 0;
				result = Protocol_Parse(VESC_RX_Buff);
				
				Vesc_Data_Ready = (result == 0);
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
 * @note   :Sets appropriate flags for current ADC/footpad sensor state
 **************************************************/
void ADC_Task(void)
{
	static uint8_t adc_step = 0;
	static uint16_t adc_charge_sum_ave = 0;
	static uint16_t adc1_val_sum_ave = 0;
	static uint16_t adc2_val_sum_ave = 0;
	
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
 * @note   :The main task for determining how to display the lights
 **************************************************/
void VESC_State_Task(void)
{
	if ((Charge_Flag > 0) || (Power_Flag > 2) || !Vesc_Data_Ready)
		return;

	Vesc_Data_Ready = false;

	CheckPowerLevel((data.inpVoltage+1)/BATTERY_STRING);

	if(data.dutyCycleNow < 0)
	{
		data.dutyCycleNow = -data.dutyCycleNow;
	}
	// Duty Cycle beep
	if ((lcmConfig.dutyBeep > 0) && (data.dutyCycleNow >= lcmConfig.dutyBeep))
	{
		Buzzer_Frequency = ((((uint8_t)(data.dutyCycleNow))*4)-220);
	}
	else
	{
		Buzzer_Frequency = 0;
	}

	// Don't buzz in wheel slip or flywheel mode
	if (data.state == RUNNING_UPSIDEDOWN) {
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

	if(data.rpm<0)
	{
		data.rpm = -data.rpm;
	}

	if(data.state == RUNNING_FLYWHEEL) {
		Lightbar_Battery_Flag = 2;
		Sensor_Activation_Display_Flag = 5;
		Buzzer_Frequency = 0;
	}
	else if(data.rpm < SENSOR_ACTV_DISPLAY_RPM) // Below this RPM show footpad activation on Lightbar
	{
		if (data.state == DISABLED) {
			if ((ADC1_Val > ADC_THRESHOLD_LOWER) || (ADC2_Val > ADC_THRESHOLD_LOWER)) {
				// Don't touch my board when it's disabled :)
				Buzzer_Frequency = 100;
			}
		}
		else {
			Buzzer_Frequency = 0;

			if(ADC1_Val < ADC_THRESHOLD_UPPER && ADC2_Val < ADC_THRESHOLD_UPPER)
			{
				Lightbar_Battery_Flag = 1; // Display Battery Level
			}
			else {
				Lightbar_Battery_Flag = 2; // Do not Display Battery Level
				if(ADC1_Val > ADC_THRESHOLD_UPPER && ADC2_Val > ADC_THRESHOLD_UPPER)
				{
					Sensor_Activation_Display_Flag = 3;  // Both Footpad Sensors Activated
				}
				else if(ADC1_Val >ADC_THRESHOLD_UPPER)
				{
					Sensor_Activation_Display_Flag = 1;  // Left Footpad Sensor Activated
				}
				else
				{
					Sensor_Activation_Display_Flag = 2;  // Right Footpad Sensor Activated
				}
			}
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
		// Add check for low voltage to force voltage display on WS2812!
		Lightbar_Battery_Flag = 2;
		Sensor_Activation_Display_Flag = 4;	// Normal Riding!
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
	
	if(Shutdown_Time_S>60000) // if we reach 60s increment minite 
	{
		Shutdown_Time_S = 0;
		
		Shutdown_Time_M++;

		if(Shutdown_Time_M >= IDLE_TIME && Animation_Running == 0)
		{
			Idle_Animation();
		}

		if(Shutdown_Time_M >= SHUTDOWN_TIME)
		{
			Power_Flag = 4;
			Power_Time = 0;
		}
	}

	if(((Shutdown_Time_M > 0) || (Shutdown_Time_S >= 10000)) && (lcmConfig.boardOff > 0))
	{
		// After 10 seconds of idle we allow the board to be shut down via app
		Power_Flag = 4;
		Power_Time = 0;
	}
}
