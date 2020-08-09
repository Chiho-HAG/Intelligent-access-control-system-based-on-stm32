#include "stm32f10x.h"
#include "spi_driver.h"

static void SPI_RCC_Configuration(SPI_TypeDef *SPIx)
{
	if (SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}
}
/**
  * @brief  ����ָ��SPI������
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @retval None
  */
static void SPI_GPIO_Configuration(SPI_TypeDef *SPIx) //D��İ�����PB13��PB14��PB15
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (SPIx == SPI1)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		//��ʼ��Ƭѡ�������
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}
	else
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		//��ʼ��Ƭѡ�������
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
}
/**
  * @brief  �����ⲿSPI�豸����SPI��ز���
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @retval None
  */
void SPI_Configuration(SPI_TypeDef *SPIx)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_RCC_Configuration(SPIx);

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Hard;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_GPIO_Configuration(SPIx);

	SPI_SSOutputCmd(SPIx, ENABLE);
	SPI_Cmd(SPIx, ENABLE);
}
/**
  * @brief  д1�ֽ����ݵ�SPI����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  TxData д�����ߵ�����
  * @retval ���ݷ���״̬
  *		@arg 0 ���ݷ��ͳɹ�
  * 	@arg -1 ���ݷ���ʧ��
  */
int32_t SPI_WriteByte(SPI_TypeDef *SPIx, uint16_t TxData)
{
	uint8_t retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
		; //�ȴ���������
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR = TxData; //����һ��byte
	retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
		; //�ȴ�������һ��byte
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR;
	return 0; //�����յ�������
}
/**
  * @brief  ��SPI���߶�ȡ1�ֽ�����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  p_RxData ���ݴ����ַ
  * @retval ���ݶ�ȡ״̬
  *		@arg 0 ���ݶ�ȡ�ɹ�
  * 	@arg -1 ���ݶ�ȡʧ��
  */
int32_t SPI_ReadByte(SPI_TypeDef *SPIx, uint16_t *p_RxData)
{
	uint8_t retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
		; //�ȴ���������
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	SPIx->DR = 0xFF; //����һ��byte
	retry = 0;
	while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
		; //�ȴ�������һ��byte
	{
		retry++;
		if (retry > 200)
			return -1;
	}
	*p_RxData = SPIx->DR;
	return 0; //�����յ�������
}
/**
  * @brief  ��SPI����д���ֽ�����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  p_TxData �������ݻ������׵�ַ
  * @param	sendDataNum ���������ֽ���
  * @retval ���ݷ���״̬
  *		@arg 0 ���ݷ��ͳɹ�
  * 	@arg -1 ���ݷ���ʧ��
  */
int32_t SPI_WriteNBytes(SPI_TypeDef *SPIx, uint8_t *p_TxData, uint32_t sendDataNum)
{
	uint8_t retry = 0;
	while (sendDataNum--)
	{
		while ((SPIx->SR & SPI_I2S_FLAG_TXE) == 0)
			; //�ȴ���������
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		SPIx->DR = *p_TxData++; //����һ��byte
		retry = 0;
		while ((SPIx->SR & SPI_I2S_FLAG_RXNE) == 0)
			; //�ȴ�������һ��byte
		{
			SPIx->SR = SPIx->SR;
			retry++;
			if (retry > 20000)
				return -1;
		}
		SPIx->DR;
	}
	return 0;
}
/**
  * @brief  ��SPI���߶�ȡ���ֽ�����
  * @param  SPIx ��Ҫʹ�õ�SPI
  * @param  p_RxData ���ݴ����ַ
  * @param	readDataNum ��ȡ�����ֽ���
  * @retval ���ݶ�ȡ״̬
  *		@arg 0 ���ݶ�ȡ�ɹ�
  * 	@arg -1 ���ݶ�ȡʧ��
  */
int32_t SPI_ReadNBytes(SPI_TypeDef *SPIx, uint8_t *p_RxData, uint32_t readDataNum)
{
	uint8_t retry = 0;
	while (readDataNum--)
	{
		SPIx->DR = 0xFF;
		while (!(SPIx->SR & SPI_I2S_FLAG_TXE))
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		retry = 0;
		while (!(SPIx->SR & SPI_I2S_FLAG_RXNE))
		{
			retry++;
			if (retry > 20000)
				return -1;
		}
		*p_RxData++ = SPIx->DR;
	}
	return 0;
}
void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //PORTBʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //SPI2ʱ��ʹ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //PB13/14/15�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��GPIOB

	GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15); //PB13/14/15����

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	 //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						 //����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					 //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;							 //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;						 //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					 //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							 //CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);									 //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����

	SPI2_ReadWriteByte(0xff); //��������
}
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ
//SPI_BaudRatePrescaler_8   8��Ƶ
//SPI_BaudRatePrescaler_16  16��Ƶ
//SPI_BaudRatePrescaler_256 256��Ƶ

void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= SPI_BaudRatePrescaler; //����SPI2�ٶ�
	SPI_Cmd(SPI2, ENABLE);
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{
	u8 retry = 0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if (retry > 200)
			return 0;
	}
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry = 0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if (retry > 200)
			return 0;
	}
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����
}

/*********************************END OF FILE**********************************/
