#include "ds1302.h"
#include "delay.h"
#include "usart.h"
#include "GUI.h"
#include "stdlib.h"

/*
DS1302�ӿ�:
	GPIOA_5 ->DS1302_RST
	GPIOA_6 ->DS1302_DAT
	GPIOA_7 ->DS1302_CLK
*/
struct TIMEData TimeData;
//DS1302��ַ����
#define DS1302_SEC_ADDR           0x80		//�����ݵ�ַ
#define DS1302_MIN_ADDR           0x82		//�����ݵ�ַ
#define DS1302_HOUR_ADDR          0x84		//ʱ���ݵ�ַ
#define DS1302_DAY_ADDR           0x86		//�����ݵ�ַ
#define DS1302_MONTH_ADDR         0x88		//�����ݵ�ַ
#define DS1302_WEEK_ADDR          0x8a		//�������ݵ�ַ
#define DS1302_YEAR_ADDR          0x8c		//�����ݵ�ַ
#define DS1302_CONTROL_ADDR       0x8e		//�������ݵ�ַ
#define DS1302_CHARGER_ADDR       0x90 		//��繦�ܵ�ַ			 
#define DS1302_CLKBURST_ADDR      0xbe
 
//��ʼʱ�䶨��
u8 time_buf[8] = {0x20,0x20,0x05,0x07,0x15,0x09,0x30,0x04};//��ʼʱ��2019��4��12��15��07��00�� ������
u8 readtime[15];//��ǰʱ��
u8 sec_buf=0;  //�뻺��
u8 sec_flag=0; //���־λ
u8 k[1];
char TIME[17];
//DS1302��ʼ������
void DS1302_Init() 
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;
 	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);	//��GPIO��ʱ�ӣ��ȴ򿪸��ò����޸ĸ��ù���
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//Ҫ�ȿ�ʱ�ӣ�����ӳ�䣻����ʾ�ر�jtag��ʹ��swd��  

	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_3;				     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 		
		



//	/*1.GPIOCʱ��*/
//	RCC->APB2ENR |= 1<<2;
//	/*2. ����GPIOA_5/6/7ģʽ*/
//	GPIOA->CRL &= 0X000FFFFF;
//	GPIOA->CRL |= 0X33300000;
//	GPIOA->ODR |=1<<6;
//	//printf("DS1302_Init OK!\n");
//		RCC->APB2ENR |= 1<<3;
//	/*2. ����GPIOA_5/6/7ģʽ*/
//	GPIOB->CRL &= 0X000FFFFF;
//	GPIOB->CRL |= 0X33300000;
//	GPIOB->ODR |=1<<6;
	
 
}
//��DS1302д��һ�ֽ�����
void DS1302_WriteByte(u8 addr,u8 data) 
{
	u8 i;
	DS1302_RST=0; //��ֹ���ݴ��� ��������������Ҫ
	DS1302_CLK=0; //ȷ��д����ǰSCLKΪ�͵�ƽ
	DS1302_RST=1;	//����DS1302����	
	DS1302_OutPut_Mode();
	addr=addr&0xFE;  //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for(i=0;i<8;i++) //д��Ŀ���ַ��addr
	{
		if (addr&0x01) DS1302_OUT=1;
		else DS1302_OUT=0;
		DS1302_CLK=1; //ʱ��������д������
		DS1302_CLK=0;
		addr=addr>>1;
	}	
	for (i=0;i<8;i++) //д�����ݣ�data
	{
		if(data&0x01) DS1302_OUT=1;
		else DS1302_OUT=0;
		DS1302_CLK=1;    //ʱ��������д������
		DS1302_CLK=0;
		data = data >> 1;
	}
	DS1302_CLK=1;    // ��ʱ�ӵ�ƽ���ڸߵ�ƽ״̬ ��������֪״̬
	DS1302_RST=0;	//ֹͣDS1302����
}
 
//��DS1302����һ�ֽ�����
u8 DS1302_ReadByte(u8 addr) 
{
	u8 i,temp;	
	DS1302_RST=0; //��������Ҫ
	DS1302_CLK=0; //�Ƚ�SCLK�õ͵�ƽ,ȷ��д����ǰSCLK������
	DS1302_RST=1; //����DS1302����
	DS1302_OutPut_Mode();
	//д��Ŀ���ַ��addr
	addr=addr|0x01; //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for(i=0;i<8;i++) 
	{
		if (addr&0x01) DS1302_OUT=1;
		else DS1302_OUT=0;
		DS1302_CLK=1; //д����
		DS1302_CLK=0;
		addr = addr >> 1;
	}	
	//��DS1302�������ݣ�temp
	DS1302_InPut_Mode();
	for(i=0;i<8;i++)
	{
		temp=temp>>1;
		if (DS1302_IN) temp|=0x80;
		else temp&=0x7F;
		DS1302_CLK=1;
		DS1302_CLK=0;
	}	
	DS1302_CLK=1;  //��ʱ�ӵ�ƽ������֪״̬
	DS1302_RST=0;	//ֹͣDS1302����
	return temp;
}
//��DS1302д��ʱ������,����ʱ��У׼�޸�
void DS1302_WriteTime() 
{
	DS1302_WriteByte(DS1302_CONTROL_ADDR,0x00);       //�ر�д���� 
	DS1302_WriteByte(DS1302_SEC_ADDR,0x80);           //��ͣʱ�� 
	//DS1302_WriteByte(DS1302_CHARGER_ADDR,0xa9);     //������ 
	DS1302_WriteByte(DS1302_YEAR_ADDR,time_buf[1]);   //�� 
	DS1302_WriteByte(DS1302_MONTH_ADDR,time_buf[2]);  //�� 
	DS1302_WriteByte(DS1302_DAY_ADDR,time_buf[3]);    //�� 
	DS1302_WriteByte(DS1302_HOUR_ADDR,time_buf[4]);   //ʱ 
	DS1302_WriteByte(DS1302_MIN_ADDR,time_buf[5]);    //��
	DS1302_WriteByte(DS1302_SEC_ADDR,time_buf[6]);    //��
	DS1302_WriteByte(DS1302_WEEK_ADDR,time_buf[7]);	  //�� 
	DS1302_WriteByte(DS1302_CHARGER_ADDR,0xA5);//�򿪳�繦�� ѡ��2K�����緽ʽ
	DS1302_WriteByte(DS1302_CONTROL_ADDR,0x80);//��д����     
}
//��DS1302����ʱ������
void DS1302_ReadTime(void)  
{
	time_buf[1]=DS1302_ReadByte(DS1302_YEAR_ADDR);          //�� 
	time_buf[2]=DS1302_ReadByte(DS1302_MONTH_ADDR);         //�� 
	time_buf[3]=DS1302_ReadByte(DS1302_DAY_ADDR);           //�� 
	time_buf[4]=DS1302_ReadByte(DS1302_HOUR_ADDR);          //ʱ 
	time_buf[5]=DS1302_ReadByte(DS1302_MIN_ADDR);           //�� 
	time_buf[6]=(DS1302_ReadByte(DS1302_SEC_ADDR))&0x7f;    //�룬������ĵ�7λ�����ⳬ��59
	time_buf[7]=DS1302_ReadByte(DS1302_WEEK_ADDR);          //�� 	
}
 
//������
void DS1302_GetTime()
{ 
    DS1302_ReadTime(); //��ȡʱ��
    TimeData.year=(time_buf[0]>>4)*1000+(time_buf[0]&0x0F)*100+(time_buf[1]>>4)*10+(time_buf[1]&0x0F); //�������
    TimeData.month=(time_buf[2]>>4)*10+(time_buf[2]&0x0F);  //�����·�
    TimeData.day=(time_buf[3]>>4)*10+(time_buf[3]&0x0F);    //��������
    TimeData.hour=(time_buf[4]>>4)*10+(time_buf[4]&0x0F);   //����Сʱ
    TimeData.minute=(time_buf[5]>>4)*10+(time_buf[5]&0x0F); //�������
    TimeData.second=(time_buf[6]>>4)*10+(time_buf[6]&0x0F); //��������
    TimeData.week=(time_buf[7]&0x0F);                       //��������
  //printf("TIME:%d-%d-%d %d:%d:%d WEEK:%d \n",TimeData.year,TimeData.month,TimeData.day,TimeData.hour,TimeData.minute,TimeData.second,TimeData.week);
	  sprintf(TIME,"%02d-%02d %02d:%02d:%02d ��%d",TimeData.month,TimeData.day,TimeData.hour,TimeData.minute,TimeData.second,TimeData.week);
		Gui_DrawFont_GBK16(00,00,BLACK,WBLUE,(u8 *)TIME);//��Ļ��ӡ����	
}
int GetTimeyear()
{
	return TimeData.year;
}
int GetTimemonth()
{
	return TimeData.month;
}
int GetTimeday()
{
	return TimeData.day;
}
int GetTimehour()
{
	return TimeData.hour;
}
int GetTimeminute()
{
	return TimeData.minute;
}
int GetTimesecond()
{
	return TimeData.second;
}
int GetTimeweek()
{
	return TimeData.week;
}
void DS1302_ChangeTime(u8 * a) 
{
	fun(a);
	DS1302_WriteByte(DS1302_CONTROL_ADDR,0x00);       //�ر�д���� 
	DS1302_WriteByte(DS1302_SEC_ADDR,0x80);           //��ͣʱ�� 
	//DS1302_WriteByte(DS1302_CHARGER_ADDR,0xa9);     //������ 
	DS1302_WriteByte(DS1302_YEAR_ADDR,a[1]);   //�� 
	DS1302_WriteByte(DS1302_MONTH_ADDR,a[2]);  //�� 
	DS1302_WriteByte(DS1302_DAY_ADDR,a[3]);    //�� 
	DS1302_WriteByte(DS1302_HOUR_ADDR,a[4]);   //ʱ 
	DS1302_WriteByte(DS1302_MIN_ADDR,a[5]);    //��
	DS1302_WriteByte(DS1302_SEC_ADDR,a[6]);    //��
	DS1302_WriteByte(DS1302_WEEK_ADDR,a[7]);	  //�� 
	DS1302_WriteByte(DS1302_CHARGER_ADDR,0xA5);//�򿪳�繦�� ѡ��2K�����緽ʽ
	DS1302_WriteByte(DS1302_CONTROL_ADDR,0x80);//��д����     
}
 void fun(u8 *s)//һ���ַ���ת16���ƺ���
 {u8 i,n,sum;sum=0;
  for(i=2;i<16;i++)
  {k[0]=s[i];
   n=atoi((char*)k);
   if(i%2==0)sum=n;
	 else s[(i-1)/2]=sum*16+n;}}