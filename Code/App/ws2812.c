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
 * @param	num 	number of lights
 * @param	red 	0-255
 * @param	green	0-255
 * @param	blue    0-255
 * @retval	None
 **************************************************/
void WS2812_Set_Colour(uint8_t num,uint8_t green,uint8_t red,uint8_t blue)
{
	uint8_t i = 0;
	
	for(i=0; i<24; i++)
	{
		if(i<8)   // red light
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
		else if(i<16)  // green light
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
