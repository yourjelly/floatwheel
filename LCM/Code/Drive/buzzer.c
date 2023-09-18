#include "buzzer.h"

uint16_t Buzzer_Counter = 0;

/**************************************************
 * @brie   :Buzzer_Init()
 * @note   :��������ʼ��
 * @param  :��
 * @retval :��
 **************************************************/
void Buzzer_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	// BUZZER
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	BUZZER_OFF;
}

/**************************************************
 * @brie   :Buzzer_Scan()
 * @note   :������ɨ��
 * @param  :��
 * @retval :��
 **************************************************/
void Buzzer_Scan(void)
{
	static uint8_t buzzer_lock = 0;

	if (Buzzer_Counter > 0)
	{
		if (buzzer_lock == 0)
		{
			buzzer_lock = 1;
			BUZZER_ON;
		}
		else
		{
			Buzzer_Counter--;
			if (Buzzer_Counter == 0)
			{
				buzzer_lock = 0;
				BUZZER_OFF;
			}
		}
	}
}

/**************************************************
 * @brie   :Buzzer_Ring()
 * @note   :��������
 * @param  :ring_time ���ʱ��
 * @retval :��
 **************************************************/
void Buzzer_Ring(uint16_t ring_time)
{
	Buzzer_Counter = ring_time;
}
