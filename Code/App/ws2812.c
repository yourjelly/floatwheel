#include "ws2812.h"

uint16_t WS2812_Counter = 0;
uint8_t WS2812_Buff[WS2812_N][24];

/**************************************************
 * @brief  :WS2812_Init()
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
 * @brie	WS2812_Set_Colour()
 * @note	WS2812 set color
 * @param	num 	light to assign color to
 * @param	colour 	0-14 from ws2812.h enum list
 * @param	brightness 0-100 scale for brightness
 * @retval	None
 **************************************************/
void WS2812_Set_Colour(uint8_t num, uint8_t colour, uint8_t brightness)
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
	
	uint8_t i = 0;
	uint8_t red = colourRGB[colour][0];
	uint8_t green = colourRGB[colour][1];
	uint8_t blue = colourRGB[colour][2];
	
	if (colour == Off || brightness == 0) // shortcut if light should be off
	{
		for(i=0; i<24; i++)
		{
			WS2812_Buff[num][i] = WS2812_0;
		}
		return;
	}
	else if (brightness != 255) {
		red = round(red * brightness / 255);
		green = round(green * brightness / 255);
		blue = round(blue * brightness / 255);
	}
	
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
 * @note   :Set all to off
 * @param  None
 * @retval None
 **************************************************/
void WS2812_All_Off(void)
{
	for(uint8_t num=0; num<10; num++)
	{
		for(uint8_t i=0; i<24; i++)
		{
			WS2812_Buff[num][i] = WS2812_0;
		}
	}
}

/**************************************************
 * @brief  :WS2812_Left()
 * @note   :WS2812 Left
 * @param  None
 * @retval None
 **************************************************/
void WS2812_Left(void)
{
	uint8_t buff[24];
	uint8_t i = 0,j = 0;
	
	for(i=0; i<24; i++)
	{
		buff[i] = WS2812_Buff[WS2812_N-1][i];
	}
	
	for(i=WS2812_N-1; i>0; i--)
	{
		for(j=0; j<24; j++)
		{
			WS2812_Buff[i][j] = WS2812_Buff[i-1][j];
		}
	}
	
	for(i=0; i<24; i++)
	{
		WS2812_Buff[0][i] = buff[i];
	}
}

void delay(uint16_t i)
{
	while(i--);
}

///**************************************************
// * @brief  :WS2812_Refresh()
// * @note   :WS2812 Refresh
// * @param  None
// * @retval None
// **************************************************/
void WS2812_Refresh(void)
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
