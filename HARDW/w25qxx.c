#include "w25qxx.h"
#include "spi_driver.h"
#include "delay.h"
#include "usart.h"
#include "stdlib.h"
#include "ds1302.h"

u16 W25QXX_TYPE = W25Q64; //Ĭ����W25Q64

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector

//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //PORTBʱ��ʹ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;		 // PB12 ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

	W25QXX_CS = 1;							//SPI FLASH��ѡ��
	SPI2_Init();							//��ʼ��SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_2); //����Ϊ18Mʱ��,����ģʽ
	W25QXX_TYPE = W25QXX_ReadID();			//��ȡFLASH ID.
}

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25QXX_ReadSR(void)
{
	u8 byte = 0;
	W25QXX_CS = 0;							//ʹ������
	SPI2_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������
	byte = SPI2_ReadWriteByte(0Xff);		//��ȡһ���ֽ�
	W25QXX_CS = 1;							//ȡ��Ƭѡ
	return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(u8 sr)
{
	W25QXX_CS = 0;							 //ʹ������
	SPI2_ReadWriteByte(W25X_WriteStatusReg); //����дȡ״̬�Ĵ�������
	SPI2_ReadWriteByte(sr);					 //д��һ���ֽ�
	W25QXX_CS = 1;							 //ȡ��Ƭѡ
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
	W25QXX_CS = 0;						  //ʹ������
	SPI2_ReadWriteByte(W25X_WriteEnable); //����дʹ��
	W25QXX_CS = 1;						  //ȡ��Ƭѡ
}
//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
	W25QXX_CS = 0;						   //ʹ������
	SPI2_ReadWriteByte(W25X_WriteDisable); //����д��ָֹ��
	W25QXX_CS = 1;						   //ȡ��Ƭѡ
}
//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;
	W25QXX_CS = 0;
	SPI2_ReadWriteByte(0x90); //���Ͷ�ȡID����
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0x00);
	Temp |= SPI2_ReadWriteByte(0xFF) << 8;
	Temp |= SPI2_ReadWriteByte(0xFF);
	W25QXX_CS = 1;
	return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u16 i;
	W25QXX_CS = 0;								//ʹ������
	SPI2_ReadWriteByte(W25X_ReadData);			//���Ͷ�ȡ����
	SPI2_ReadWriteByte((u8)((ReadAddr) >> 16)); //����24bit��ַ
	SPI2_ReadWriteByte((u8)((ReadAddr) >> 8));
	SPI2_ReadWriteByte((u8)ReadAddr);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI2_ReadWriteByte(0XFF); //ѭ������
	}
	W25QXX_CS = 1;
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 i;
	W25QXX_Write_Enable();						 //SET WEL
	W25QXX_CS = 0;								 //ʹ������
	SPI2_ReadWriteByte(W25X_PageProgram);		 //����дҳ����
	SPI2_ReadWriteByte((u8)((WriteAddr) >> 16)); //����24bit��ַ
	SPI2_ReadWriteByte((u8)((WriteAddr) >> 8));
	SPI2_ReadWriteByte((u8)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
		SPI2_ReadWriteByte(pBuffer[i]); //ѭ��д��
	W25QXX_CS = 1;						//ȡ��Ƭѡ
	W25QXX_Wait_Busy();					//�ȴ�д�����
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 pageremain;
	pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite; //������256���ֽ�
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break; //д�������
		else	   //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain; //��ȥ�Ѿ�д���˵��ֽ���
			if (NumByteToWrite > 256)
				pageremain = 256; //һ�ο���д��256���ֽ�
			else
				pageremain = NumByteToWrite; //����256���ֽ���
		}
	};
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; //������ַ
	secoff = WriteAddr % 4096; //�������ڵ�ƫ��
	secremain = 4096 - secoff; //����ʣ��ռ��С
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite; //������4096���ֽ�
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //������������������
		for (i = 0; i < secremain; i++)				  //У������
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break; //��Ҫ����
		}
		if (i < secremain) //��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);	//�����������
			for (i = 0; i < secremain; i++) //����
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //д����������
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.
		if (NumByteToWrite == secremain)
			break; //д�������
		else	   //д��δ����
		{
			secpos++;	//������ַ��1
			secoff = 0; //ƫ��λ��Ϊ0

			pBuffer += secremain;		 //ָ��ƫ��
			WriteAddr += secremain;		 //д��ַƫ��
			NumByteToWrite -= secremain; //�ֽ����ݼ�
			if (NumByteToWrite > 4096)
				secremain = 4096; //��һ����������д����
			else
				secremain = NumByteToWrite; //��һ����������д����
		}
	};
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
	int root;
	root = 96 * 16;
	while (root >= 0)
	{
		W25QXX_Erase_Sector(root);
		root--;
	}
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
	//����falsh�������,������
	//printf("fe:%x\r\n",Dst_Addr);
	Dst_Addr *= 4096;
	W25QXX_Write_Enable(); //SET WEL
	W25QXX_Wait_Busy();
	W25QXX_CS = 0;								//ʹ������
	SPI2_ReadWriteByte(W25X_SectorErase);		//������������ָ��
	SPI2_ReadWriteByte((u8)((Dst_Addr) >> 16)); //����24bit��ַ
	SPI2_ReadWriteByte((u8)((Dst_Addr) >> 8));
	SPI2_ReadWriteByte((u8)Dst_Addr);
	W25QXX_CS = 1;		//ȡ��Ƭѡ
	W25QXX_Wait_Busy(); //�ȴ��������
}
//�ȴ�����
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01)
		; // �ȴ�BUSYλ���
}
//�������ģʽ
void W25QXX_PowerDown(void)
{
	W25QXX_CS = 0;						//ʹ������
	SPI2_ReadWriteByte(W25X_PowerDown); //���͵�������
	W25QXX_CS = 1;						//ȡ��Ƭѡ
	delay_us(3);						//�ȴ�TPD
}
//����
void W25QXX_WAKEUP(void)
{
	W25QXX_CS = 0;							   //ʹ������
	SPI2_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
	W25QXX_CS = 1;							   //ȡ��Ƭѡ
	delay_us(3);							   //�ȴ�TRES1
}
//����һ���û���Ϣ
void Usermessage_Add(int i, u8 *name, u8 age[2], u8 sex[2], u8 num[14], u8 Id[8], u8 addr[30])
{
	W25QXX_Write(name, i * 68 + 0, 10);
	W25QXX_Write(num, i * 68 + 10, 20);
	W25QXX_Write(addr, i * 68 + 30, 30);
	W25QXX_Write(Id, i * 68 + 60, 4);
	W25QXX_Write(age, i * 68 + 64, 2);
	W25QXX_Write(sex, i * 68 + 66, 2);
	//printf("----Write Message OK!---һ\n");
}
//���һ���û���Ϣ
void Usermessage_Print(int i)
{
	u8 datatemp[30];
	u8 Nam[10];
	u8 Sex[2];
	printf("-------------USER-%02d-------------\n", i);
	W25QXX_Read(datatemp, i * 68 + 60, 4);
	printf("ID:%02X%02X%02X%02X\n", datatemp[0], datatemp[1], datatemp[2], datatemp[3]);
	W25QXX_Read(Nam, i * 68 + 0, 10);
	printf("������%s\n", Nam);
	W25QXX_Read(Sex, i * 68 + 66, 2);
	printf("�Ա�%s\n", Sex);
	W25QXX_Read(datatemp, i * 68 + 64, 2);
	printf("���䣺%c%c\n", datatemp[0], datatemp[1]);
	W25QXX_Read(datatemp, i * 68 + 10, 20);
	printf("�绰��%s\n", datatemp);
	W25QXX_Read(datatemp, i * 68 + 30, 30);
	printf("סַ��%s\n", datatemp);
}
//����û�ID
void GetUserID(int i, u8 *pBuffer)
{
	u8 datatemp[30];

	W25QXX_Read(datatemp, i * 68 + 60, 4);
	pBuffer[0] = datatemp[0];
	pBuffer[1] = datatemp[1];
	pBuffer[2] = datatemp[2];
	pBuffer[3] = datatemp[3];
}
//����û�����
void GetUserName(int i, u8 *pBuffer)
{
	u8 datatemp[10];

	W25QXX_Read(datatemp, i * 68 + 0, 10);
	pBuffer[0] = datatemp[0];
	pBuffer[1] = datatemp[1];
	pBuffer[2] = datatemp[2];
	pBuffer[3] = datatemp[3];
	pBuffer[4] = datatemp[4];
	pBuffer[5] = datatemp[5];
	pBuffer[6] = datatemp[6];
	pBuffer[7] = datatemp[7];
	pBuffer[8] = datatemp[8];
	pBuffer[9] = datatemp[9];
}
//ɾ��һ���û���Ϣ
void DelUser(int i)
{
	u8 datatemp[68];
	int n, j;
	n = R_SaveNumA();
	for (j = i + 1; j <= n; j++)
	{
		W25QXX_Read(datatemp, j * 68, 68);
		W25QXX_Write(datatemp, (j - 1) * 68, 68);
	}
	Change_SaveNumA(0, -1);
}
//����һ��ֻ�п��ŵ��û���Ϣ
void AddUser(u8 *pBuffer)
{
	Usermessage_Add(R_SaveNumA(), "NewUser", "NU", "NU", "NULL", pBuffer, "NULL");
	Change_SaveNumA(0, 1);
}
//�ı�洢�������4λ���������
//mode:ģʽ��mode=0ʱ���������+=a
//					 mode=1ʱ���������=a
void Change_SaveNumA(int mode, int a)
{
	int n;
	u8 datatemp[9];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 5, 4);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 5, 4);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 5, 4);
	}
}
//���ش洢�������9λ���������
int R_SaveNumA(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 5, 4);
	n = atoi((char *)datatemp);
	return n;
}
void Change_SaveNumB(int mode, int a)
{
	int n;
	u8 datatemp[5];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 10, 5);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 10, 5);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 10, 5);
	}
}
//���ش洢�������λ���������
int R_SaveNumB(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 10, 5);
	n = atoi((char *)datatemp);
	return n;
}
void Change_SaveNumC(int mode, int a)
{
	int n;
	u8 datatemp[5];
	if (mode == 0)
	{
		W25QXX_Read(datatemp, 96 * 16 * 4096 - 15, 5);
		n = atoi((char *)datatemp);
		n = n + a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 15, 5);
	}
	else
	{
		n = a;
		sprintf((char *)datatemp, "%d", n);
		W25QXX_Write(datatemp, 96 * 16 * 4096 - 15, 5);
	}
}
//���ش洢�������λ���������
int R_SaveNumC(void)
{
	int n;
	u8 datatemp[5];
	W25QXX_Read(datatemp, 96 * 16 * 4096 - 15, 5);
	n = atoi((char *)datatemp);
	return n;
}
//�����ǰ��־
void Log_Print(int i)
{
	int size = 96 * 16 * 4096 - 45;
	u8 Nam[10];
	u8 ID[4];
	u8 year[4];
	u8 month[2];
	u8 day[2];
	u8 hour[2];
	u8 minute[2];
	u8 second[2];
	u8 week[1];
	printf("---------------------LOG-%02d----------------------\n", i);
	W25QXX_Read(Nam, size - 30 * i + 4, 10);
	printf("����:%s ", Nam);
	W25QXX_Read(ID, size - 30 * i, 4);
	printf("    [ID:%02X%02X%02X%02X]\n", ID[0], ID[1], ID[2], ID[3]);
	W25QXX_Read(year, size - 30 * i + 14, 4);
	printf("ʱ��:%c%c%c%c-", year[0], year[1], year[2], year[3]);
	W25QXX_Read(month, size - 30 * i + 18, 2);
	printf("%c%c-", month[0], month[1]);
	W25QXX_Read(day, size - 30 * i + 20, 2);
	printf("%c%c ", day[0], day[1]);
	W25QXX_Read(hour, size - 30 * i + 22, 2);
	printf("%c%c:", hour[0], hour[1]);
	W25QXX_Read(minute, size - 30 * i + 24, 2);
	printf("%c%c:", minute[0], minute[1]);
	W25QXX_Read(second, size - 30 * i + 26, 2);
	printf("%c%c", second[0], second[1]);
	W25QXX_Read(week, size - 30 * i + 28, 1);
	printf(" ��:%c\n", week[0]);
}

void Log_save(int flag)
{
	u8 Log[30];
	u8 temp[10];
	int i, n, r;
	r = R_SaveNumB();
	GetUserID((flag - 1) / 10 - 1, temp);
	for (i = 0; i < 4; i++)
		Log[i] = temp[i];
	GetUserName((flag - 1) / 10 - 1, temp);
	for (i = 0; i < 10; i++)
		Log[i + 4] = temp[i];
	n = GetTimeyear();
	sprintf((char *)temp, "%4d", n);
	for (i = 0; i < 4; i++)
		Log[i + 14] = temp[i];
	n = GetTimemonth();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 18] = temp[i];
	n = GetTimeday();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 20] = temp[i];
	n = GetTimehour();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 22] = temp[i];
	n = GetTimeminute();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 24] = temp[i];
	n = GetTimesecond();
	sprintf((char *)temp, "%02d", n);
	for (i = 0; i < 2; i++)
		Log[i + 26] = temp[i];
	n = GetTimeweek();
	sprintf((char *)temp, "%1d", n);
	for (i = 0; i < 1; i++)
		Log[i + 28] = temp[i];
	//for(i=0;i<29;i++)printf("%02X ",Log[i]);
	W25QXX_Write(Log, 96 * 16 * 4096 - 45 - r * 30, 30);
	printf("log save ok\n");
	Log_Print(R_SaveNumB());
	Change_SaveNumB(0, 1);
}
void Log_display(void)
{
	int i;
	printf("============��־============\n");
	for (i = 0; i < R_SaveNumB(); i++)
		Log_Print(i);
	printf("============��־============\n");
	printf("�û���:%d ��־��Ŀ:%d\n", R_SaveNumA(), R_SaveNumB());
}
void UserMessage_display(void)
{
	int i;
	printf("============�û�============\n");
	for (i = 0; i < R_SaveNumA(); i++)
		Usermessage_Print(i);
	printf("===========�û�============\n");
	printf("�û���:%d \n", R_SaveNumA());
}
