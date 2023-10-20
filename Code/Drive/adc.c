#include "adc.h"


/**************************************************
 * @brief  :ADC1_Init()
 * @note   :ADC��ʼ��
 * @param  :��
 * @retval :��
 **************************************************/
void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOD, ENABLE );
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC ,ENABLE);
	//ADC2->PD3 SENS_Charge->PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource3,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_7);
	//ADC1->PC4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource4,GPIO_AF_7);
	
	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1,&ADC_InitStructure);

//	ADC_ChannelConfig(ADC1, ADC_Channel_4 , ADC_SampleTime_239_5Cycles);

	ADC_Cmd(ADC1, ENABLE);

//	ADC_GetCalibrationFactor(ADC1); //У׼
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADCAL));
	
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
//	ADC_StartOfConversion(ADC1);	
}

/**************************************************
 * @brief  :Read_ADC_Value()
 * @note   :��ADCͨ��������
 * @param  :ch ͨ��
 * @retval :��
 **************************************************/
uint16_t Read_ADC_Value(uint8_t ch)
{
    uint16_t adc_value = 0;
	
	ADC_StopOfConversion(ADC1);
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
	
	switch(ch)
	{
		case 1:
			ADC_ChannelConfig(ADC1, ADC_Channel_2 , ADC_SampleTime_239_5Cycles);
		break;
		
		case 2:
			ADC_ChannelConfig(ADC1, ADC_Channel_3 , ADC_SampleTime_239_5Cycles);
		break;
		
		case 3:
			ADC_ChannelConfig(ADC1, ADC_Channel_4 , ADC_SampleTime_239_5Cycles);
		break;
	}

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); //ADC������־
	ADC_StartOfConversion(ADC1);//Ϊѡ���� ADC ͨ������ת����
	
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //ת��������־
	adc_value =ADC_GetConversionValue(ADC1);

	return adc_value;
	
}


