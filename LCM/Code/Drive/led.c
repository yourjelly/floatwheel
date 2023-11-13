#include "led.h"

uint16_t LED_Counter = 0;
//uint16_t LED_Filp_Time = 200;

/**************************************************
 * @brief  :LED_Init()
 * @note   :LED��ʼ��
 * @param  :��
 * @retval :��
 **************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);	
	//LED_RUN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//LED_B
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	//LED_F
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	LED1_ON;
	LED_B_OFF;
	LED_F_OFF;
}

/**************************************************
 * @brief  :LED1_Filp_Time()
 * @note   :����LED1��˸ʱ��
 * @param  :filp_time ��˸ʱ��
 * @retval :��
 **************************************************/
//void LED1_Filp_Time(uint16_t filp_time)
//{
//	LED_Filp_Time = filp_time;
//}







