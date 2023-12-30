#include "ws2812.h"

uint16_t WS2812_Counter = 0;
uint8_t WS2812_Buff[WS2812_N][24];

/**************************************************
 * @note   :WS2812 initialization
 * @param  None
 * @retval None
 **************************************************/
void WS2812_Init(void)
{
	uint16_t i = 0;
	uint8_t* ws2812_buff_add = (uint8_t*)&WS2812_Buff;
	
	//SPI_Config();
	IO_WS2812_Init();
	
	while(i < (WS2812_N*24))
	{
		*ws2812_buff_add = (uint8_t)WS2812_0;
		ws2812_buff_add++;
		i++;
	}
}

/**************************************************
 * @note	Set WS2812 numbered led by RGB values (0-255)
 * @param	num		0-9 from ws2812.h enum list
 * @param	red		0-255 brightness of red led
 * @param	green 	0-255 brightness of green led
 * @param	blue	0-255 brightness of blue led
 * @retval	None
 **************************************************/
void Lightbar_Set_RGB(uint8_t num, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t i = 0;
	
	for(i=0; i<24; i++)
	{
		if(i<8)   // green light
		{
			if(green&0x80)
			{
				WS2812_Buff[num][i] = WS2812_1;
			}
			else
			{
				WS2812_Buff[num][i] = WS2812_0;
			}
			green <<= 1;
		}
		else if(i<16)  // red light
		{
			if(red&0x80)
			{
				WS2812_Buff[num][i] = WS2812_1;
			}
			else
			{
				WS2812_Buff[num][i] = WS2812_0;
			}
			red <<= 1;		
		}
		else if(i<24)  //blue light
		{
			if(blue&0x80)
			{
				WS2812_Buff[num][i] = WS2812_1;
			}
			else
			{
				WS2812_Buff[num][i] = WS2812_0;
			}
			blue <<= 1;			
		}
		
	}
}

/**************************************************
 * @note	WS2812 set only leds within range (1-10) to rgb valunes (0-255)
 * @param	start start led (0-10)
 * @param	end end led (1-10)
 * @param	red	0-255 brightness of red led
 * @param	green 0-255 brightness of green led
 * @param	blue 0-255 brightness of blue led
 * @retval	None
 **************************************************/
void Lightbar_Set_RGB_Range(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t num;

	if(start < 1)
		start = 1;

	if (start > 1) {
		for(num=0; num<start-1; num++) {
			Lightbar_Set_RGB(num,0,0,0);
		}
	}
	for(num=start-1; num<end; num++) {
		Lightbar_Set_RGB(num,green,red,blue);
	}
	for(num=end; num<10; num++) {
		Lightbar_Set_RGB(num,0,0,0);
	}
}

/**************************************************
 * @note	Get RGB array by colour name
 * @param	colour 			0-14 from ws2812.h enum list
 * @retval	array_RGB[3]	{red, green, blue} values (0-255)
 **************************************************/
uint8_t rgb_by_colour(uint8_t colour)
{
	uint8_t colourRGB[15][3] = {
		{255,0,0},    //  0 - Red
		{0,255,0},    //  1 - Green
		{0,0,255},    //  2 - Blue
		{255,128,0},  //  3 - Orange
		{255,255,0},  //  4 - Yellow
		{0,255,255},  //  5 - Cyan
		{127,0,255},  //  6 - Violet
		{255,0,255},  //  7 - Magenta
		{255,0,127},  //  8 - Pink
		{255,255,255},//  9 - White
		{255,69,0},   // 10 - RedOrange
		{127,255,0},  // 11 - YellowGreen
		{0,255,127},  // 12 - GreenCyan
		{0,128,255},  // 13 - CyanBlue
		{0,0,0}       // 14 - Off
	};

	uint8_t array_RGB[3] = {colourRGB[colour][0], colourRGB[colour][1], colourRGB[colour][2]};

	return *array_RGB;
}

/**************************************************
 * @note	Set WS2812 numbered led by color name and brightness (0-255)
 * @param	num			light to assign color to 0-9
 * @param	colour		0-14 from ws2812.h enum list
 * @param	brightness	0-255 scale for brightness
 * @retval	None
 **************************************************/
void Lightbar_Set_Colour(uint8_t num, uint8_t colour, uint8_t brightness)
{
	uint8_t array_RGB[3];
	*array_RGB = rgb_by_colour(colour);
	uint8_t red = array_RGB[0];
	uint8_t green = array_RGB[1];
	uint8_t blue = array_RGB[2];

	uint8_t i;
	
	if (colour == Off || brightness == 0) // shortcut if light should be off
	{
		for(i=0; i<24; i++)
		{
			WS2812_Buff[num][i] = WS2812_0;
		}
		return;
	}
	else if (brightness != 255) {
		red = round(red /255 * brightness);
		green = round(green /255 * brightness);
		blue = round(blue /255 * brightness);
	}
	
	Lightbar_Set_RGB(num, red, green, blue);
	
}

/**************************************************
 * @note	WS2812 set only leds within range (1-10) to color by name and brightness (0-255)
 * @param	start		start led (0-10)
 * @param	end			end led (1-10)
 * @param	colour		0-14 from ws2812.h enum list
 * @param	brightness	0-255 scale for brightness
 * @retval	None
 **************************************************/
void Lightbar_Set_Colour_Range(uint8_t start, uint8_t end, uint8_t colour, uint8_t brightness)
{
	uint8_t num;

	if(start < 1)
		start = 1;

	if (start > 1) {
		for(num=0; num<start-1; num++) {
			Lightbar_Set_Colour(num, Off, brightness);
		}
	}
	for(num=start-1; num<end; num++) {
		Lightbar_Set_Colour(num, colour, brightness);
	}
	for(num=end; num<10; num++) {
		Lightbar_Set_Colour(num, Off, brightness);
	}
}

 /**************************************************
 * @note   :Set all to off
 * @param  None
 * @retval None
 **************************************************/
void Lightbar_All_Off(void)
{
	for(uint8_t num=0; num<10; num++)
	{
		for(uint8_t i=0; i<24; i++)
		{
			WS2812_Buff[num][i] = WS2812_0;
		}
	}
}

// /**************************************************
//  * @brief  :WS2812_Left()
//  * @note   :WS2812 Left
//  * @param  None
//  * @retval None
//  **************************************************/
// void WS2812_Left(void)
// {
// 	uint8_t buff[24];
// 	uint8_t i = 0,j = 0;
	
// 	for(i=0; i<24; i++)
// 	{
// 		buff[i] = WS2812_Buff[WS2812_N-1][i];
// 	}
	
// 	for(i=WS2812_N-1; i>0; i--)
// 	{
// 		for(j=0; j<24; j++)
// 		{
// 			WS2812_Buff[i][j] = WS2812_Buff[i-1][j];
// 		}
// 	}
	
// 	for(i=0; i<24; i++)
// 	{
// 		WS2812_Buff[0][i] = buff[i];
// 	}
// }

void delay(uint16_t i)
{
	while(i--);
}

///**************************************************
// * @note   :Lightbar Refresh
// **************************************************/
void Lightbar_Refresh(void)
{
	uint16_t i = 0;
	uint8_t* ws2812_buff_add = (uint8_t*)&WS2812_Buff;
	
	// WS2812 does not allow interrupts when sending data, otherwise it is abnormal
	
	__set_PRIMASK(1); // Disable the total interrupt
	
	GPIOD->BRR = GPIO_Pin_4;
	delay(250);
	
	for(i=0; i<(WS2812_N*24); i++)
	{
		if(*(ws2812_buff_add) == WS2812_0)
		{
			WS2812_0_Code();
		}
		else
		{
			WS2812_1_Code();
		}
		ws2812_buff_add++;
	}
	
	__set_PRIMASK(0); // Enable the total interrupt
	//GPIOD->BSRR = GPIO_Pin_4;
}
