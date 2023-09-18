#include "key.h"

/*
 *	KEY1_State = 0  Not Pressed
 *	KEY1_State = 1  Click
 *	KEY1_State = 2  Double Click
 *  KEY1_State = 3  Press
 *  KEY1_State = 4  Tripple Click
 */
uint8_t KEY1_State = 0;

/**************************************************
 * @brie   :KEY_Init()
 * @note   :KEY����
 * @param  :��
 * @retval :��
 **************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**************************************************
 * @brie   :KEY1_Scan()
 * @note   :KEY1ɨ��
 * @param  :��
 * @retval :��
 **************************************************/
void KEY1_Scan(void)
{
	static uint8_t key1_st = 0;
	static uint16_t key1_cnt = 0;
	static uint16_t key1_cnt_2 = 0;
	static uint16_t key1_num = 0;

	switch (key1_st)
	{
	case 0:
		if (KEY1 == 0)
		{
			key1_cnt++;
			if (key1_cnt == 20) // key is pressed for longer than 20ms count as a state change
			{
				key1_cnt = 0;
				key1_st = 1;
			}
		}
		else
		{
			key1_cnt = 0;
			key1_cnt_2 = 0;
		}

		break;

	case 1:
		key1_cnt++;
		key1_cnt_2++;
		if (KEY1 == 1) // The button has been lifted, atleast a click
		{
			key1_num++;
			key1_st = 2;
			key1_cnt = 0;
		}
		else if (key1_cnt > 1500) // Long Press
		{
			key1_cnt = 0;
			key1_st = 3;

			KEY1_State = 3;
		}

		break;

	case 2:
		key1_cnt_2++;

		if (key1_cnt_2 < 500) // timeout for sequence of button clicks
		{
			if (KEY1 == 0)
			{
				key1_cnt++;
				if (key1_cnt == 2) // Eliminate Jitter (less sensitive than first press)
				{
					key1_cnt = 0;
					key1_st = 1;
				}
			}
			else
			{
				key1_cnt = 0;
			}
		}
		else
		{
			key1_cnt_2 = 0;
			key1_st = 4;
		}

		break;

	case 3:

		if (KEY1 == 1) // After long press reset when released
		{
			key1_st = 0;
		}

		break;

	case 4:

		switch (key1_num)
		{
		case 1: // Click
			KEY1_State = 1;
			break;

		case 2: // double click
			KEY1_State = 2;
			break;

		case 3: // triple click
			KEY1_State = 4;
			break;
			// 3... 4... add your own

		default: // exception handling

			break;
		}

		key1_num = 0;
		key1_st = 0;
		break;
	}
}
