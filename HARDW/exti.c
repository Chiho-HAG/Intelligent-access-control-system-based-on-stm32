#include "exti.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "delay.h"
#include "beep.h"
#include "QDTFT_demo.h"
#include "Lcd_Driver.h"
#include "GUI.h"
//��������ж�  ������ʽ�����ش��� PB4
void EXTI1_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// �����˿ڳ�ʼ�� 
	KEY_Init();

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//GPIOE.2 �ж����Լ��жϳ�ʼ������   �½��ش���   
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource4); 

  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//ģʽ���жϻ����¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//������ʽ
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);  //����ָ���Ĳ�����ʼ���ж��߲��� 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//ʹ�ܰ����ⲿ�ж�ͨ�� 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 2��  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   //�����ȼ� 2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //ʹ���ⲿ�ж�ͨ�� 
  NVIC_Init(&NVIC_InitStructure); 
	
}
                      

void EXTI4_IRQHandler(void)
{int  i;
		
		delay_ms(10);
		//����  
		if(KEy==0)  
			//���� KEY2 
		{  
			 i=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
		   i=!i;
       ledout(i);
		}   
	
		EXTI_ClearITPendingBit(EXTI_Line4); 
		//��� LINE2 �ϵ��жϱ�־λ 
} 





















