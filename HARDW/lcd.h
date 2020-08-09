#ifndef _LCD_H
#define _LCD_H
#define uchar unsigned char
#define uint unsigned int

//�����ܽŶ���
#define GPIO_TFT GPIOA
#define TFT_RCC_APB RCC_APB2Periph_GPIOA
#define SDA   GPIO_Pin_1
#define SCL   GPIO_Pin_2
#define CS    GPIO_Pin_3    //Ƭѡ��,�͵�ƽ��Ч
#define RESET GPIO_Pin_4 //��λ�ˣ��͵�ƽ��λ
#define RS    GPIO_Pin_5    //0:����  1������
#define HHH 105          //0:����  1������
#define WWW 160          //0:����  1������
#define SDA_H GPIO_SetBits(GPIO_TFT, SDA)
#define SDA_L GPIO_ResetBits(GPIO_TFT, SDA)
#define SCL_H GPIO_SetBits(GPIO_TFT, SCL)
#define SCL_L GPIO_ResetBits(GPIO_TFT, SCL)
#define CS_H GPIO_SetBits(GPIO_TFT, CS)
#define CS_L GPIO_ResetBits(GPIO_TFT, CS)
#define RESET_H GPIO_SetBits(GPIO_TFT, RESET)
#define RESET_L GPIO_ResetBits(GPIO_TFT, RESET)
#define RS_H GPIO_SetBits(GPIO_TFT, RS)
#define RS_L GPIO_ResetBits(GPIO_TFT, RS)

//���峣����ɫ
#define RED   0xf800
#define GREEN 0x07e0
#define BLUE  0x001f
#define WHITE 0xffff
#define BLACK 0x0000
#define YELLOW 0xFFE0
#define GRAY0 0xEF7D
#define GRAY1 0x8410
#define GRAY2 0x4208

void LCD_Init(void);
void LCD_GPIO_Conf(void);
void Font_Test(void);
void Display_Desc(void);
void showimage1(const unsigned char *);
void Display_ASCII8X16(uint, uint, uchar *);
void DisableJTAG(void);
#endif
