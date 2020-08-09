#include "hcsr.h"
#include "misc.h"
#include "stm32f10x.h"
#include "delay.h"
//#include "bsp_usart.h"
//?????

uint16_t  msHcCount = 0;




//void delay_init()
//{
//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //ѡ���ⲿʱ��Ԫ
//    fac_us=SystemCoreClock/8000000; // 72/8 ��ʱ1΢�� 9��ʱ������
//    fac_ms=(u16)fac_us*1000;   // ��ʱ1������9000��Cysticʱ������
//		printf("delay\n");
//}


///**
// * nus : ??????
// **/
//void delay_us(u32 nus)
//{
//    u32 temp;
//    //nus*fac_us???????SysTick->LOAD(24?)-1
//    SysTick->LOAD=nus*fac_us;    // ?????:n(us)*??1us?????SysTick????
//    SysTick->VAL=0x00;                       // VAL????0
//    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; // ??SysTick???
//    do
//    {
//        temp=SysTick->CTRL;
//    }while((temp&0x01)&&!(temp&(1<<16)));    // ????????(?16)
//    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; // ????
//    SysTick->VAL =0X00;                      // ??VAL
//}

///**
// * nms : ??????
// **/
//void delay_ms(u16 nms)
//{
//    u32 temp;
//    SysTick->LOAD=(u32)nms*fac_ms;
//    SysTick->VAL =0x00;
//    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
//    do
//    {
//        temp=SysTick->CTRL;
//    }while((temp&0x01)&&!(temp&(1<<16)));
//    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
//    SysTick->VAL =0X00;
//}



//��ʱ��4����
void hcsr05_NVIC()
{
		NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;       
		NVIC_Init(&NVIC_InitStructure);
}


//IO�ڼ���ʱ����ʼ��
void Hcsr05Init()
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(HCSR05_CLK, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin =HCSR05_TRIG;      
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(HCSR05_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR05_PORT,HCSR05_TRIG);
     
    GPIO_InitStructure.GPIO_Pin =   HCSR05_ECHO;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR05_PORT, &GPIO_InitStructure);  
    GPIO_ResetBits(HCSR05_PORT,HCSR05_ECHO);    
     
          
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   
     
    TIM_DeInit(TIM4);  //��λ
    TIM_TimeBaseStructure.TIM_Period = (1000-1); 
    TIM_TimeBaseStructure.TIM_Prescaler =(72-1);   //1MS����һ���ж�
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);          
        
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);  
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);    
    hcsr05_NVIC();
    TIM_Cmd(TIM4,DISABLE);    
		//printf("hcsr05\n");
}




//�򿪶�ʱ��
static void OpenTimerForHc()  
{
   TIM_SetCounter(TIM4,0);
   msHcCount = 0;
   TIM_Cmd(TIM4, ENABLE); 
}


//�رն�ʱ��
static void CloseTimerForHc()    
{
   TIM_Cmd(TIM4, DISABLE); 
}


//��ʱ���жϺ���
void TIM4_IRQHandler(void)  
{
   if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  
   {
       TIM_ClearITPendingBit(TIM4, TIM_IT_Update  ); 
       msHcCount++;
   }
}
 


//��ȡ��ʱ������ֵ
u32 GetEchoTimer(void)
{
   u32 t = 0;
   t = msHcCount*1000;
   t += TIM_GetCounter(TIM4);
//   TIM4->CNT = 0;  
	TIM_SetCounter(TIM4,0);
   delay_ms(50);
   return t;
}
 
//ͨ����ʱ��������ֵ�������
//float distance;
float Hcsr05GetLength(void )
{
   u32 t = 0;
   int i = 0;
   float lengthTemp = 0;
   float sum = 0;
//   while(i!=5)
//   {

			GPIO_SetBits(HCSR05_PORT,HCSR05_TRIG); //����TRIG�� ���ͳ���10us�ĸߵ�ƽ
		  delay_us(20);
			GPIO_ResetBits(HCSR05_PORT,HCSR05_TRIG); //����TRIG
//      OpenTimerForHc();         //�򿪶�ʱ��
      i = i + 1;
      while(GPIO_ReadInputDataBit(HCSR05_PORT,HCSR05_ECHO) == 0);  //�ȴ�ECHO�Ÿߵ�ƽ ����whileѭ��
			OpenTimerForHc();         //�򿪶�ʱ��
			while(GPIO_ReadInputDataBit(HCSR05_PORT,HCSR05_ECHO) == 1){} //�ȴ�ECHO�Ÿߵ�ƽ ����whileѭ��
      CloseTimerForHc();         //�رն�ʱ��
      t = GetEchoTimer();        //��ȡ��ʱ��������ֵ
      lengthTemp = ((float)t/58.0);//cm
//		 lengthTemp=((float)t*0.34)/2;//cm
//			distance=0.17*t;			//�������
      sum = lengthTemp + sum ;
        
//    }
//    lengthTemp = sum/5.0;
    return lengthTemp;
}

