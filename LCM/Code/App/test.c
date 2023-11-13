#include "test.h"


/*���´��붼�ǲ�����ز��ֹ��ܵ�*/
uint8_t  Test_Step = 0;
uint16_t Test_Counter = 0;

/**************************************************
 * @brief  :Test_Charge()
 * @note   :���Գ���
 * @param  :��
 * @retval :��
 **************************************************/
void Test_Charge(void)
{
	static uint8_t test_charge = 0;
	uint16_t adc_val = 0;
	float charge_voltage = 0;
	
	switch(test_charge)
	{
		case 0:
			adc_val = Read_ADC_Value(3);
			charge_voltage = (float)(adc_val*0.0257080078125);
			if(charge_voltage > 1.5)  //���������
			{
				Test_Counter = 0;
				test_charge = 1;
			}
		break;
		
		case 1:
			if(Test_Counter >= 1000) //��ʱ1S
			{
				adc_val = Read_ADC_Value(3);
				charge_voltage = (float)(adc_val*0.0257080078125);
				if(charge_voltage > 1.5) //�����ȷʵ����
				{
					CHARGE_ON;   //�򿪳���
					test_charge = 2;
				}
				else
				{
					CHARGE_OFF;  //�رճ���
					test_charge = 0;
				}
			}
		break;
			
		case 2:	
			adc_val = Read_ADC_Value(3);
			charge_voltage = (float)(adc_val*0.0257080078125);
			if(charge_voltage < 1.0)  //������γ�
			{
				CHARGE_OFF;  //�رճ���
				test_charge = 0;
			}
		break;
	}
}

/**************************************************
 * @brief  :Test_LED()
 * @note   :����LED
 * @param  :��
 * @retval :��
 **************************************************/
void Test_LED(void)
{
	static uint8_t  test_led = 0;
	static uint8_t  led_b_f = 0;
	static uint16_t led_pwm = 0;
	static uint8_t  pwm_st =0;
	static uint8_t  pwm_cnt =0;
	
	switch(test_led)
	{
		case 0:
			if(led_b_f == 0)
			{
				led_b_f = 1;
				LED_F_OFF;
				LED_B_ON;
			}
			else
			{
				led_b_f = 0;
				LED_B_OFF;
				LED_F_ON;
			}
			test_led = 1;
		break;
			
		case 1:
			if(Test_Counter > 10)
			{
				Test_Counter = 0;
				switch(pwm_st)
				{
					case 0:
						led_pwm += 500;
						if(led_pwm >= 5000)
						{
							led_pwm -= 1;
							pwm_st = 1;
						}
					break;
					
					case 1:
						led_pwm += 1;
						pwm_st = 2;
					case 2:
						led_pwm -= 500;
						if(led_pwm == 0)
						{
							pwm_st = 0;
						}
					break;	
				}
					
				TIM_SetCompare2(TIM1,9000);
				pwm_cnt++;
				
				if(pwm_cnt >= 20)
				{
					pwm_cnt = 0;
					led_pwm = 0;
					pwm_st = 0;
					test_led = 0;
					TIM_SetCompare2(TIM1,0);
				}
			}
		break;	
	}
}

/**************************************************
 * @brief  :Test_USART()
 * @note   :���Դ���
 * @param  :��
 * @retval :��
 **************************************************/
void Test_USART(void)
{
	static uint8_t  test_usart = 0;
	uint8_t result;
	
	switch(test_usart)
	{
		case 0:
			Get_Vesc_Pack_Data(COMM_GET_VALUES);
			test_usart = 1;
		break;
		
		case 1:
			if(VESC_RX_Flag == 1)
			{
				VESC_RX_Flag = 0;
				result = Protocol_Parse(VESC_RX_Buff);
				
				if(result == 0) //�����ɹ�
				{
					//Buzzer_Ring(200);		
				}
				else	//����ʧ��
				{
					//Buzzer_Ring(1000);
				}
				Test_Counter = 0;
				test_usart = 2;
			}
			else
			{
				test_usart = 3;
				Test_Counter = 0;
			}
		break;
			
		case 2:
			if(Test_Counter >= 500)
			{
				test_usart = 0;
			}				
		break;
			
		case 3:
			
			if(VESC_RX_Flag == 1)
			{
				test_usart = 1;
			}
			else if(Test_Counter >= 500)
			{
				test_usart = 0;
			}
		break;
				
	}
}

float adc1_voltage = 0;
float adc2_voltage = 0;
float charge_voltage = 0;
/**************************************************
 * @brief  :Test_ADC()
 * @note   :����ADC
 * @param  :��
 * @retval :��
 **************************************************/
void Test_ADC(void)
{
	static uint8_t  test_adc = 0;
	uint16_t adc_val = 0;
	
	switch(test_adc)
	{
		case 0:
			adc_val = Read_ADC_Value(1);
		  adc1_voltage = adc_val*0.0012890625;

			adc_val = Read_ADC_Value(2);
		  adc2_voltage = adc_val*0.0012890625;

			adc_val = Read_ADC_Value(3);
			charge_voltage = (float)(adc_val*0.0257080078125);
		
			Test_Counter = 0;
			test_adc = 1;
		break;	

		case 1:
			if(Test_Counter>1000)
			{
				test_adc = 0;
			}
		break;
		
		case 2:
			adc_val = Read_ADC_Value(2);
		    adc2_voltage = adc_val*0.0008056640625;
		
			Test_Counter = 0;
			test_adc = 3;
		break;
		
		case 3:
			if(Test_Counter>100)
			{
				test_adc = 4;
			}
		break;
			
		case 4:
			adc_val = Read_ADC_Value(3);
			charge_voltage = (float)(adc_val*0.0257080078125);
		
			Test_Counter = 0;
			test_adc = 5;
		break;

		case 5:
			if(Test_Counter>100)
			{
				test_adc = 0;
			}
		break;			
	}
}

/**************************************************
 * @brief  :Test_Task()
 * @note   :��������
 * @param  :��
 * @retval :��
 **************************************************/
void Test_Task(void)
{
	switch(Test_Step)
	{
		case 0:
			
		break;
		
		case 1: //���Գ���
			Test_Charge();
		break;
				
		case 2: //����LED
			Test_LED();
		break;
						
		case 3: //���Դ���
			Test_USART();
		break;
		
		case 4: //����ADC1 ADC2
			Test_ADC();
		break;
		
	}
}

