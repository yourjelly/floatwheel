#include "spi.h"


/**************************************************
 * @brief  :SPI_Config()
 * @note   :SPI���� 
 * @param  :��
 * @retval :��
 **************************************************/
void SPI_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_2);	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);		
	
	SPI_Cmd(SPI1, ENABLE);
}

/**************************************************
 * @brief  :SPI1_Send_Byte()
 * @note   :SPI1����һ���ֽ�
 * @param  :data ���͵�����
 * @retval :��
 **************************************************/
void SPI1_Send_Byte(uint8_t data)
{
	SPI1->DR = data;
	while(((SPI1->SR) & SPI_I2S_FLAG_TXE) == 0);  //�ȴ��������
}
