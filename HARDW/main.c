#include "main.h"
uint8_t Card_Type1[2];
uint8_t Card_ID[4];
uint8_t Card_KEY[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //{0x11,0x11,0x11,0x11,0x11,0x11};   //����
uint8_t Card_Data[16];
uint8_t User_Card[10][4] = {0x29, 0x43, 0x1c, 0xbd,//��ǰ���� IC��ID1,2,3
							0xb9, 0x6b, 0xff, 0xc1,
							0xfa, 0x10, 0x41, 0xea};
uint8_t status;
typedef struct USER
{
	u8 name[10];
	u8 age[2];
	u8 sex[2];
	u8 num[14];
	u8 Id[8];
	u8 addr[30];
} USE;

int main(void)
{
	int key, 	//����״̬
		flag,	//ע�Ῠ�жϱ�־��flag=0;�����ڿ��У�flag=11����ǰ�û����Ϊ1��root)��Ϊ0;
		i,		//ѭ��
		j, 		//ѭ��
		root, 	//root��ˢ������
		rootkey, //rootģʽ�ж�
		n, 		//��ǰ�û����
		a, 		//NUll
		doorstatu, //��ǰ��״̬
		mode, 	//��ǰģʽ������ģʽ��(�����п���ģʽ)
		len, 	//��ǰ�����������ֵ
		t, 		//���ڽ��ճ��ȼ���
		action, //��ǰ����ִ�еĶ���
		p, 		//��ǰ����������û����
		Len;	//��ǰ��������ĳ����������ֵ
	float length;//�������õ��ľ���
	char id[17];//ID��ʾ�Ĵ���������Sprintf
	char aa[10];//����������ʾ�Ĵ���������Sprintf
	USE User[2];//�û��ṹ�������ʼ������ʱ����
	u8 UserCard[4];//�û�ID��ȡ�Ĵ���
	u8 command[9];//�����ݴ�Ĵ��������ڴ��ڷ�������
	//u8 time[16]="2020051100580001";//�޸�ʱ��ʱʹ�õ��ַ���
	flag = 1;	   //ע�Ῠ�жϼĴ���
	root = 0;	   //��������ˢ�Ĵ���
	rootkey = 0;   //root mode��־λ
	doorstatu = 0; //��ʼ��״̬Ϊ�ر�
	mode = 0;	   //��ʼģʽΪ��⿨ģʽ
	action = 0;	   //���ڳ�ʼģʽΪ����ģʽ
	Len = R_SaveNumC();//��flash�ж�ȡ�����ֵ
	Card_Type1[0] = 0x04;//������1
	Card_Type1[1] = 0x00;//������2

	delay_init();									//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);								//���ڳ�ʼ��Ϊ115200
	printf("\r\n****SERIAL PORT TEST****\r\n");		//���ڲ���

	DS1302_Init();									//ʵʱʱ��ģ���ʼ��
	SystemInit(); //System init.					//ϵͳʱ�ӳ�ʼ��
	RC522_IO_Init();								//RFID������IO��ʼ��	
	BEEP_Init();									//��������ʼ��
	LED_Init();										//LED I/O��ʼ��
	DIANJI_Init();									//���������ʼ��
	KEY_Init();										//������ʼ��
	Hcsr05Init();									//������ģ���ʼ��
	Lcd_Init();										//��ʾ����ʼ����ST7735 0.96)
	W25QXX_Init(); 									//W25QXX��ʼ��
	//W25QXX_Erase_Chip();							//��Ƭ����
	//Change_SaveNumB(1,0);							//�����־
	LCD_BL_SET;										//lcdI/O����
	LCD_CS_CLR;										//����Ƭѡ
	showimageALL(gImage_2);							//��ʾ����ͼƬ
	delay_ms(1000);
	Lcd_Clear(WHITE);								//����Ϊ��ɫ							    	
	//DS1302_ChangeTime(time);						//�޸�ϵͳʱ��

	PcdReset();		 								//��λRC522
	PcdAntennaOff(); 								//�ر�����
	delay_ms(100);
	PcdAntennaOn(); 								//��������
	//doorstatu=Door_control(0);
	printf("\r\n****HJT SERIAL PORT TEST****\r\n");

	Usermessage_Add(0, "ROOT", "NU", "NU", "NULL", User_Card[0], "NULL");//��ROOT����������Ϣд��Flash
	sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
	Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);//�����ǰ�û�������־������ӡ����ʾ��  

	while (1)
	{
		key = KEY_Scan(0);			//�����Ƿ���
		if (mode == 0)				//�ж�ģʽ
		{
			if (rootkey == 0)		//�ж��Ƿ�Ϊrootģʽ
			Gui_DrawFont_GBK16(144, 00, BLACK, WBLUE, "  ");//root״̬��ӡ��ʾ����rootģʽ״̬��Ϊ��
			length = Hcsr05GetLength(); //�õ�����
			if (length <= Len)//������״̬�ж��Ǳ������ǻ����Զ��ر�
			{
				if (doorstatu == 0)
				{
					ledout(0);
					beepout(0);
				}
				if (doorstatu == 1)
				{
					ledout(11);
					BEEP_once();
					doorstatu = Door_control(0);//����
					root = 0;
					rootkey = 0;
				}
			}
			if (length > 700)//��ֹ������ģ�����
				length = -1;

			DS1302_GetTime();//��ȡ��ǰʱ��

			sprintf(aa, "%3.0f", length);//�����ǰ����
			Gui_DrawFont_GBK16(132, 64, BLUE, WHITE, (u8 *)aa); //��Ļ��ӡ����

			key = KEY_Scan(0);
			delay_ms(10);
			if (MI_OK == PcdRequest(0x26, Card_Type1)) //Ѱ������������ɹ�����MI_OK  ��ӡһ�ο���
			{
				//printf("ID:%02X%02X%02X%02X",Card_ID[0],Card_ID[1],Card_ID[2],Card_ID[3]);

				status = PcdAnticoll(Card_ID); //����ײ ����ɹ�����MI_OK
				if (status != MI_OK)
				{
					//printf("Anticoll Error\r\n");
				}
				else
				{
					a = rootkey;
					sprintf(id, "ID:%02X%02X%02X%02X  ", Card_ID[0], Card_ID[1], Card_ID[2], Card_ID[3]); //��ȡ����
					rootkey = a;
					Gui_DrawFont_GBK16(16, 16, BLUE, WHITE, (u8 *)id);
					for (i = 0; i < R_SaveNumA(); i++) //����п������Ա�
					{
						GetUserID(i, UserCard);
						for (j = 0; j < 4; j++)
						{
							if (Card_ID[j] == UserCard[j])
								flag = (i + 1) * 10 + 1; //����flag���Զ�ȡ���������
							else
								flag = 0;
							break;
						}
						if (flag != 0) //flag����0�ж��Ƿ���������Ȼ������
						{
							if (i == 0) //���IC���Ƿ�Ϊ����
								root = root + 1;
							if (i != 0)
								root = 0;
							break;
						}
					}

					status = PcdSelect(Card_ID); //ѡ�� ����ɹ�����MI_OK
					if (status != MI_OK)
					{
						printf("Select Card Error\r\n");
					}
					else
					{
						BEEP_once(); //��������һ��
									 //printf("Select Card OK\r\n");
					}

					if (root >= 2) //��������ˢ�������Σ��������Աģʽ
					{
						rootkey = 1;//����rootģʽ
						Gui_DrawFont_GBK16(144, 00, WHITE, BLACK, "�");//һ���Լ����Ĺ���Աģʽͼ��ȡģ
					}

					status = PcdHalt(); //��Ƭ��������״̬��ֹ�ظ�����
					if (status != MI_OK)
					{
						printf("PcdHalt Error\r\n");
					}
					else
					{
						if (rootkey == 1) //����Ѿ�������rootģʽ
						{
							if (flag != 0 && flag != 11) //������ڿ��У��Ҳ�������
							{
								n = (flag - 1) / 10 - 1; //�õ�������ţ�Ҳ����������б�
								DelUser(n);				 //ɾ�����û���Ϣ
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								printf("n=%d  flag=%d %d\n", n, flag, (flag - 1) % 10 - 1);
								rootkey = 0;
								Gui_DrawFont_GBK16(0, 32, BLACK, WHITE, "DEL �� ");
							}
							else if (flag == 0) //�����ڿ���
							{
								AddUser(Card_ID); //���һ��ֻ�п��ŵ��û���Ϣ
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								rootkey = 0;
								Gui_DrawFont_GBK16(0, 32, BLACK, WHITE, "ADD �� ");
							}
						}
						else
						{
							if (flag != 0) //��⿨���Ƿ���ȷ
							{
								Gui_DrawFont_GBK16(0, 16, BLUE, WHITE, "��");
								Log_save(flag);
								sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
								Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
								ledout(1);
								beepout(1);
								if (doorstatu == 0)
								{
									ledout(10);
									doorstatu = Door_control(1);
								}
							}

							else //
							{
								Gui_DrawFont_GBK16(0, 16, RED, WHITE, "��");
								if (doorstatu == 1)
								{
									ledout(11);
									doorstatu = Door_control(1);
								}
							}
						}
					}
				}
			}
		}
		if (mode)//��������ģʽ
		{
			if (USART_RX_STA & 0x8000 && action == 0)
			{
				len = USART_RX_STA & 0x3f; //�õ��˴ν��յ������ݳ���
				if (len > 9 | len < 9)
				{
					printf("��������ȷ������...\n");
					USART_RX_STA = 0;
				}
				else
				{
					printf("\r\n Your message is:\r\n\r\n");
					for (t = 0; t < 9; t++)
					{
						USART_SendData(USART1, USART_RX_BUF[t]);
						//�򴮿� 1 ��������
						command[t] = USART_RX_BUF[t];
						while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
							; //�ȴ�//���ͽ���
					}
					printf("\r\n\r\n"); //���뻻��

					if (strcmp((const char *)command, "HELP11111") == 0)
					{
						printf("CHANGEMES:�޸��û���Ϣ\n"); //����
						printf("CHANGELEN:�޸ļ����ֵ\n");
						printf("RETURNUSE:�����û���Ϣ\n");
						printf("CLEARUSE1:����û���Ϣ\n");
						printf("RETURNLOG:������־\n");
						printf("CLEARLOG1:�����־\n");
						printf("CHANGETIM:�޸�ʱ��\n");
						printf("OPENDOOR1:����\n");
						printf("CLOSEDOOR:����\n");
						printf("ESC111111:�˳�����ģʽ\n");
						printf("END...\n����������:\n");
					}
					else if (strcmp((const char *)command, "CHANGEMES") == 0)
					{
						action = 1; //�޸��û���Ϣ
						UserMessage_display();
						printf("��������Ҫ�޸ĵ��û����:\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "CHANGELEN") == 0)
					{
						action = 7; //�޸ļ����ֵ
						printf("��ǰ�����ֵΪ: %d\n", R_SaveNumC());
						printf("���������޸ĺ�ļ����ֵ:\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "CLEARLOG1") == 0)
					{
						Change_SaveNumB(1, 0);
						printf("�������־...\n����������:\n"); //�����־
						sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, WHITE, BLACK, (u8 *)aa);
					}
					else if (strcmp((const char *)command, "CLEARUSE1") == 0)
					{
						Change_SaveNumA(1, 1);
						printf("������û���Ϣ...\n����������:\n"); //�����־
						sprintf(aa, "USER:%d LOG:%d\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, WHITE, BLACK, (u8 *)aa);
					}
					else if (strcmp((const char *)command, "CHANGETIM") == 0)
					{
						action = 8; //�޸�ʱ��
						printf("������ʱ��(ʵ��:2020010112000001):\n");
						USART_RX_STA = 0;
					}
					else if (strcmp((const char *)command, "OPENDOOR1") == 0)
					{
						if (doorstatu == 0)
							doorstatu = Door_control(1);
						USART_RX_STA = 0;
						printf("ִ����ϣ�\n����������:\n");
					} //����
					else if (strcmp((const char *)command, "CLOSEDOOR") == 0)
					{
						if (doorstatu == 1)
							doorstatu = Door_control(0);
						USART_RX_STA = 0;
						printf("ִ����ϣ�\n����������:\n");
					} //����
					else if (strcmp((const char *)command, "ESC111111") == 0)
					{
						mode = 0;
						printf("���˳���������ģʽ:\n");
						rootkey = 0;
						Lcd_Clear(WHITE);
						sprintf(aa, "USER:%d LOG:%d\r\n", R_SaveNumA(), R_SaveNumB());
						Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
						USART_RX_STA = 0;
					} //�˳����ڵ���ģʽ
					else if (strcmp((const char *)command, "RETURNLOG") == 0)
					{
						Log_display();
						USART_RX_STA = 0;
						printf("����������:\n");
					} //������־
					else if (strcmp((const char *)command, "RETURNUSE") == 0)
					{
						UserMessage_display();
						USART_RX_STA = 0;
						printf("����������:\n");
					} //�����û���Ϣ
					else if (strcmp((const char *)command, "SHOWMYJPG") == 0)
					{
						showimageALL(gImage_2);
						printf("����������:\n");
					} //�����û���Ϣ

					else
					{
						printf("�Ҳ�������...\n");
						USART_RX_STA = 0;
					}
					USART_RX_STA = 0;
				}
			}

			else if (USART_RX_STA & 0x8000 && action == 1)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û����Ϊ:");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				p = atoi((char *)USART_RX_BUF);
				printf("\n");
				if (p >= R_SaveNumA())
				{
					printf("�����������\n���������������:\n");
					action = 1;
					USART_RX_STA = 0;
				}
				else
				{
					action = 2;
					USART_RX_STA = 0;
					printf("�������û�����:\n");
				}
			}
			else if (USART_RX_STA & 0x8000 && action == 2)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û�����Ϊ:\n");
				for (t = 0; t < len; t++)
				{
					User[0].name[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("�������û��Ա�:\n");
				action = 3;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 3)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û��Ա�Ϊ:\n");
				for (t = 0; t < len; t++)
				{
					User[0].sex[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("�������û�����:\n");
				action = 4;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 4)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û�����Ϊ:\n");
				for (t = 0; t < len; t++)
				{
					User[0].age[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("�������û��绰:\n");
				action = 5;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 5)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û��绰Ϊ:\n");
				for (t = 0; t < len; t++)
				{
					User[0].num[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				action = 3;
				USART_RX_STA = 0;
				printf("�������û�סַ:\n");
				action = 6;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 6)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ��������û�סַΪ:\n");
				for (t = 0; t < len; t++)
				{
					User[0].addr[t] = USART_RX_BUF[t];
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				printf("\n");
				GetUserID(p, User_Card[4]);
				Usermessage_Add(p, User[0].name, User[0].age, User[0].sex, User[0].num, User_Card[4], User[0].addr);
				printf("-----------��Ϣ¼�����--------\n����������:\n");
				action = 0;
				USART_RX_STA = 0;
			}
			else if (USART_RX_STA & 0x8000 && action == 7)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n ������ļ����ֵΪ:\n");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				p = atoi((char *)USART_RX_BUF);
				printf("\n");
				if (p >= 750)
				{
					printf("�����ֵ����\n����������������ֵ(MAX=750):\n");
					action = 7;
					USART_RX_STA = 0;
				}
				else
				{
					Change_SaveNumC(1, p);
					Len = R_SaveNumC();
					action = 0;
					USART_RX_STA = 0;
					printf("-----------��ֵ�޸����--------\n����������:\n");
				}
			}
			else if (USART_RX_STA & 0x8000 && action == 8)
			{
				len = USART_RX_STA & 0x3f;
				printf("\r\n �������ʱ��Ϊ:\n");
				for (t = 0; t < len; t++)
				{
					USART_SendData(USART1, USART_RX_BUF[t]);
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
						;
				}
				DS1302_ChangeTime(USART_RX_BUF);
				printf("\n");
				action = 0;
				USART_RX_STA = 0;
				printf("-----------ʱ���޸����--------\n����������:\n");
			}
			else
			{
				ledout(1);
				ledout(10);
			}
		}

		if (key)
		{
			root = 0;
			BEEP_once();

			if (rootkey & mode == 0) //���������rootģʽ
			{
				Lcd_Clear(WHITE);
				showimage1(gImage_3, 40, 0, 120, 45);
				mode = 1; //���봮�ڵ���ģʽ

				Gui_DrawFont_GBK16(0, 48, BLACK, WHITE, "*�ѽ��봮������ģʽ*");
				Gui_DrawFont_GBK16(0, 64, BLACK, WHITE, "*���ڴ�����λ������*");
				printf("�ѽ��봮�ڵ���ģʽ������������......\n");
			}
			else if (mode == 0)
			{
				ledout(1);
				ledout(10);
				Log_display();
				Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, "LOG Send OK! ");
			}
			else if (mode == 1)
			{
				mode = 0;
				rootkey = 0;
				Lcd_Clear(WHITE);
				sprintf(aa, "USER:%d LOG:%d\r\n", R_SaveNumA(), R_SaveNumB());
				Gui_DrawFont_GBK16(0, 64, BLUE, WHITE, (u8 *)aa);
			}
		}
	}
}
