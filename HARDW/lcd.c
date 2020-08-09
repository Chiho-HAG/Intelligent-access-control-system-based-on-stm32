#include "stm32f10x.h"
#include "lcd.h"
#include "string.h"
#include "font.h"
#include "delay.h"
/*

�ڲ��������϶������ű�ʶ�����Ժ�����֪�������ܽŵ���;��TFT����������ѹΪ3.3V����Ȼ���ܽ�5VҲ����ʹ�ã�����Һ�����ã�����1������û��ǽ�3.3V��

 2����ΪGND��ֱ�ӽӵؾͺ��ˡ�

 3����CSΪ��ģ���SPIƬѡ�ź��������ţ���STM32оƬIO����Ӽ��ɣ�������ӵ���A13��

 4����REST(RST)Ϊ��ģ�鸴λ�������ţ���STM32оƬIO����Ӽ��ɣ�������ӵ���A14��

 5A0(RS)����Ϊ��ģ������ /���� �ź�����ѡ�����ţ���STM32оƬIO����Ӽ��ɣ�������ӵ���A15��

 6����SDA��SDI��Ϊ��ģ���SPI�����������ţ���STM32оƬIO����Ӽ��ɣ�������ӵ���A11��

 7����SCL(SCK)Ϊ��ģ���SPIʱ���ź��������ţ���STM32оƬIO����Ӽ��ɣ�������ӵ���A12��
 8����LEDΪ��ģ���LED���⣬ע�⣬LCDû�б����һƬ�ڣ�ɶҲ����������������ӵ���VCC3.3��
  
*/

uchar Zk_ASCII8X16[] =
	{
		0x00, 0x00, 0x00, 0x10, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, /*"1",0*/
		0x00, 0x00, 0x00, 0x7E, 0x40, 0x40, 0x40, 0x58, 0x64, 0x02, 0x02, 0x42, 0x44, 0x38, 0x00, 0x00, /*"5",1*/

		0x00, 0x00, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, /*"I",0*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*" ",1*/
		0x00, 0x00, 0x00, 0x70, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x7C, 0x00, 0x00, /*"l",2*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, /*"o",3*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0x42, 0x24, 0x24, 0x28, 0x10, 0x10, 0x00, 0x00, /*"v",4*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x40, 0x42, 0x3C, 0x00, 0x00, /*"e",5*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*" ",6*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0x42, 0x24, 0x24, 0x28, 0x18, 0x10, 0x10, 0xE0, /*"y",7*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, /*"o",8*/
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x42, 0x42, 0x42, 0x42, 0x46, 0x3B, 0x00, 0x00	/*"u",9*/
};
//�����ܽų�ʼ��
void LCD_GPIO_Conf()
{
	GPIO_InitTypeDef GPIO_InitStruct;			 //����ṹ��
	RCC_APB2PeriphClockCmd(TFT_RCC_APB, ENABLE); //��ʹ��GPIOA�������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = SDA | SCL | CS | RESET | RS;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_TFT, &GPIO_InitStruct);
}

//#define USE_LANDSCAPE//�������л�

void SPI_WriteData(uchar Data)
{
	unsigned char i = 0;
	for (i = 8; i > 0; i--)
	{
		if (Data & 0x80)
			SDA_H;
		else
			SDA_L;
		SCL_L;
		SCL_H;
		Data <<= 1;
	}
}
//
void Lcd_WriteIndex(uchar Data)
{

	CS_L;
	RS_L;
	SPI_WriteData(Data);
	CS_H;
}
//
void Lcd_WriteData(uchar Data)
{
	CS_L;
	RS_H;
	SPI_WriteData(Data);
	CS_H;
}
//???16???
void LCD_WriteData_16Bit(unsigned int Data)
{
	CS_L;
	RS_H;
	SPI_WriteData(Data >> 8); //
	SPI_WriteData(Data);	  //
	CS_H;
}

void Reset()
{
	RESET_L;
	delay_ms(100);
	RESET_H;
	delay_ms(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//  for S6D02A1
void LCD_Init()
{
	Reset(); //Reset before LCD Init.

	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	Lcd_WriteIndex(0x11); //Sleep exit
	delay_ms(120);

	//ST7735R Frame Rate
	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB3);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB4); //Column inversion
	Lcd_WriteData(0x07);

	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0xA2);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x84);
	Lcd_WriteIndex(0xC1);
	Lcd_WriteData(0xC5);

	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0x2A);
	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0xEE);

	Lcd_WriteIndex(0xC5); //VCOM
	Lcd_WriteData(0x0E);

	Lcd_WriteIndex(0x36); //MX, MY, RGB mode
#ifdef USE_LANDSCAPE
	Lcd_WriteData(0xA8); //??C8 ??08 A8
#else
	Lcd_WriteData(0xC8); //??C8 ??08 A8
#endif
	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1a);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x18);
	Lcd_WriteData(0x2f);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x20);
	Lcd_WriteData(0x22);
	Lcd_WriteData(0x1f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x23);
	Lcd_WriteData(0x37);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0xe1);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x17);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x2c);
	Lcd_WriteData(0x29);
	Lcd_WriteData(0x2e);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x39);
	Lcd_WriteData(0x3f);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00 + 2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x80 + 2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00 + 3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x80 + 3);

	Lcd_WriteIndex(0xF0); //Enable test command
	Lcd_WriteData(0x01);
	Lcd_WriteIndex(0xF6); //Disable ram power save mode
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0x3A); //65k mode
	Lcd_WriteData(0x05);

	Lcd_WriteIndex(0x29); //Display on
}

/*************************************************
???:LCD_Set_Region
??:??lcd,
:xy?
???:?
*************************************************/
void Lcd_SetRegion(unsigned int x_start, unsigned int y_start, unsigned int x_end, unsigned int y_end)
{
#ifdef USE_LANDSCAPE //??
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start + 3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end + 3);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start + 2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end + 2);

#else //
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start + 2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end + 2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start + 3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end + 3);
#endif
	Lcd_WriteIndex(0x2c);
}

void PutPixel(uint x_start, uint y_start, uint color)
{
	Lcd_SetRegion(x_start, y_start, x_start + 1, y_start + 1);
	LCD_WriteData_16Bit(color);
}

void dsp_single_colour(int color)
{
	uchar i, j;
	Lcd_SetRegion(0, 0, HHH - 1, WWW - 1);
	for (i = 0; i < HHH; i++)
		for (j = 0; j < WWW; j++)
			LCD_WriteData_16Bit(color);
}

//
void Display_ASCII8X16(uint x0, uint y0, uchar *s)
{
	int i, j, k, x, y, xx;

	unsigned char qm;

	long int ulOffset;

	char ywbuf[32], temp[2];

	for (i = 0; i < strlen((char *)s); i++)
	{
		if (((unsigned char)(*(s + i))) >= 161)
		{
			temp[0] = *(s + i);
			temp[1] = '\0';
			return;
		}

		else
		{
			qm = *(s + i);

			ulOffset = (long int)(qm)*16;

			for (j = 0; j < 16; j++)
			{
				ywbuf[j] = Zk_ASCII8X16[ulOffset + j];
			}

			for (y = 0; y < 16; y++)
			{
				for (x = 0; x < 8; x++)
				{
					k = x % 8;

					if (ywbuf[y] & (0x80 >> k))
					{
						xx = x0 + x + i * 8;
						PutPixel(xx, y + y0, RED);
					}
				}
			}
		}
	}
}

void Display_Desc()
{
	Display_ASCII8X16(10, 10, "Welcome");
	Display_ASCII8X16(10, 26, "QDtech");
	Display_ASCII8X16(10, 42, "Dots:128*128");
	Display_ASCII8X16(10, 58, "IC: ST7735");
	Display_ASCII8X16(10, 74, "VA:6 o'clock");
	Display_ASCII8X16(10, 90, "2013.10.26");
}

//??,??Gui_DrawFont_GBK16???

void Fast_DrawFont_GBK16(uint x, uint y, uint fc, uint bc, uchar *s)
{
	unsigned char i, j;
	unsigned short k;
	uint HZnum;
	HZnum = sizeof(hz16) / sizeof(typFNT_GBK16);
	while (*s)
	{
		if ((*s) >= 128)
		{
			for (k = 0; k < HZnum; k++)
			{
				if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1)))
				{
					Lcd_SetRegion(x, y, x + 16 - 1, y + 16 - 1);
					for (i = 0; i < 16 * 2; i++)
					{
						for (j = 0; j < 8; j++)
						{
							if (hz16[k].Msk[i] & (0x80 >> j))
								LCD_WriteData_16Bit(fc);
							else
							{
								if (fc != bc)
									LCD_WriteData_16Bit(bc);
							}
						}
					}
				}
			}
			s += 2;
			x += 16;
		}

		else
			s += 1;
	}
}
//?
void Gui_DrawFont_GBK16(uint x, uint y, uint fc, uint bc, uchar *s)
{
	unsigned char i, j;
	unsigned short k;

	while (*s)
	{
		if ((*s) >= 128)
		{
			for (k = 0; k < hz16_num; k++)
			{
				if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s + 1)))
				{
					for (i = 0; i < 16; i++)
					{
						for (j = 0; j < 8; j++)
						{
							if (hz16[k].Msk[i * 2] & (0x80 >> j))
								PutPixel(x + j, y + i, fc);
							else
							{
								if (fc != bc)
									PutPixel(x + j, y + i, bc);
							}
						}
						for (j = 0; j < 8; j++)
						{
							if (hz16[k].Msk[i * 2 + 1] & (0x80 >> j))
								PutPixel(x + j + 8, y + i, fc);
							else
							{
								if (fc != bc)
									PutPixel(x + j + 8, y + i, bc);
							}
						}
					}
				}
			}
			s += 2;
			x += 16;
		}
		else
			s += 1;
	}
}

void Font_Test(void)
{ //
	dsp_single_colour(BLACK);
	Gui_DrawFont_GBK16(40, 10, BLUE, YELLOW, "������");
	Fast_DrawFont_GBK16(32, 30, RED, WHITE, "�Ž�ϵͳ");
	Fast_DrawFont_GBK16(48, 50, BLUE, WHITE, "����");
	Fast_DrawFont_GBK16(32, 70, WHITE, BLUE, "��Ϊרע");
	Fast_DrawFont_GBK16(32, 90, WHITE, BLUE, "����רҵ");
	//delay_ms(1800);
	//dsp_single_colour(BLUE);
}

void showimage1(const unsigned char *p) //40*40 QQ??
{
	int i;
	unsigned char picH, picL;
	dsp_single_colour(BLACK); //??

	Lcd_SetRegion(0, 0, HHH - 1, WWW - 1); //
	for (i = 0; i < HHH * WWW; i++)
	{
		picL = *(p + i * 2); //??
		picH = *(p + i * 2 + 1);
		LCD_WriteData_16Bit(picH << 8 | picL);
	}
}
void DisableJTAG(void)
{

	//����AFIO ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_Disable SWJ ��ȫ���ã�JTAG+SW-DP��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	// �ı�ָ���ܽŵ�ӳ�� GPIO_Remap_SWJ_JTAGDisable ��JTAG-DP ���� + SW-DP ʹ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
}
