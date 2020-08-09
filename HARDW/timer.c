#include "timer.h"
#include "led.h"
#include "Lcd_Driver.h"
#include "p.h"
//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//����ʱ��ѡ��Ϊ APB1 �� 2 ������ APB1 Ϊ 36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ�� 3!
void TIM3_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef
		TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//��ʱ�� TIM3 ʹ��
																//��ʱ�� TIM3 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr;						//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//�ڳ�ʼ�� TIM3

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //����������ж�
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;			  //TIM3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 0 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  //�����ȼ� 3 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);							  //�ܳ�ʼ�� NVIC �Ĵ���
	TIM_Cmd(TIM3, ENABLE);									  //��ʹ�� TIM3
}
//��ʱ�� 3 �жϷ�������
void TIM3_IRQHandler(void) //TIM3 �ж�
{
	int i;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //��� TIM3 �����жϷ������
	{
		i = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
		i = !i;
		ledout(i + 10);
		//	showimageALL(gImage_2);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //��� TIM3 �����жϱ�־
	}
}

void TIM1_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //��ʹ�ܶ�ʱ�� 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //��ʹ�� GPIO �� AFIO ���ù���ʱ��

	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE); //�ڣ�Partial��������ӳ�� TIM3_CH2

	//���ø�����Ϊ�����������,��� TIM1 CH4 �� PWM ���岨�� GPIOA.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //�ٳ�ʼ�� GPIO //��ʼ�� TIM1

	TIM_TimeBaseStructure.TIM_Period = arr;						//�������Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);				//�۳�ʼ�� TIMx    //��ʼ�� TIM3 Channel2 PWM ģʽ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //ѡ�� PWM ģʽ 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //������Ը�
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);					  //�ܳ�ʼ������ TIM3 OC4

	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_Cmd(TIM1, ENABLE);							  //��ʹ�� TIM1 }
}

void TIM3_PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //��ʹ�ܶ�ʱ�� 1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //��ʹ�� GPIO �� AFIO ���ù���ʱ��

	//GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //�ڣ�Partial��������ӳ�� TIM3_CH2

	//���ø�����Ϊ�����������,��� TIM1 CH4 �� PWM ���岨�� GPIOA.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		//TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //�ٳ�ʼ�� GPIO //��ʼ�� TIM1

	TIM_TimeBaseStructure.TIM_Period = arr;						//�������Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//�۳�ʼ�� TIMx    //��ʼ�� TIM3 Channel2 PWM ģʽ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //ѡ�� PWM ģʽ 2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //������Ը�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);					  //�ܳ�ʼ������ TIM3 OC4

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
	TIM_Cmd(TIM3, ENABLE);							  //��ʹ�� TIM1 }
}

void TIM4_PWM_Init(u16 arr, u16 psc)
{
	//**�ṹ������**//
	GPIO_InitTypeDef GPIO_InitStructure;		   //����GPIO
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; //������ʱ��
	TIM_OCInitTypeDef TIM_OCInitStructure;		   //����PWMͨ��

	//**ʱ��ʹ��**//
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //ʹ�ܶ�ʱ��TIM4ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PB�˿�ʱ��

	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		  //LED0-->PB.6 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //�����趨������ʼ��GPIOB.6
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
	//****//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		  //LED0-->PB.7 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			  //�����趨������ʼ��GPIOB.7
	GPIO_SetBits(GPIOB, GPIO_Pin_7);

	//��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;						//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM3 Channel1 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);					  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	//��ʼ��TIM3 Channel2 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);					  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4_CH1Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4_CH2Ԥװ�ؼĴ���

	TIM_Cmd(TIM4, ENABLE); //ʹ��TIM3
}
