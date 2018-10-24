//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: MPLAB X IDE V1.95                                //
//                    �� �� ��: XC16  V1.20                                      //
//                    оƬ�ͺ�: dsPIC33EP256GM710                                //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: EEPROM.c                                         //
//                    ��    ��: ����ѧ                                           //
//                    ʱ    ��: 2013-10-21    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
//
// �ļ��������������ģ��I2C����,��д FM24W256   ������д��������
//
//
// ע�����FM24W256Ϊ���紮��EEPROM���ڲ��洢����ҳ����ַ����
//
//
// �޸ļ�¼��
//
//
// �޸�ʱ��:
//
//
//-------------------------------------------------------------------------------


#include <includes.h>
#ifndef STM32
#include "user.h"
#endif


//-------------------------------------------------------------------------------

#ifdef STM32
//----------------STM32
#define I2C_SDA   GPIO_SetBits(GPIOD, GPIO_Pin_15)//(GPIOG->IDR & GPIO_Pin_1)
//----------------
#else
//----------------PIC
#define I2C_SDA   PORTGbits.RG3                   //I2C��������
#define I2C_SCL   PORTGbits.RG2
#define TRIS_SDA  TRISGbits.TRISG3                //I2C���� I/O
#define TRIS_SCL  TRISGbits.TRISG2
//----------------
#endif

INT8U I2C_RD_Buffer[MAX_EEPROM_BUFFER] = {0};  //I2C���߶�����
INT8U I2C_WR_Buffer[MAX_EEPROM_BUFFER] = {0};  //I2C����д����


//-------------------------------------------------------------------------------
// ��������: void Delay10TCY(void)
// ��������: I2C������ʱ����
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void Delay10TCY(void)
{
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
}

//-------------------------------------------------------------------------------
// ��������: I2C_Init(void)
// ��������: I2C��ʼ������
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void I2C_Init(void)
{
    TRIS_SCL = 0;       //���
    TRIS_SDA = 0;
    I2C_SCL = 1;        //����
    I2C_SDA = 1;
    EEPROM_WP2 = 1;     //����
    Delay10TCY();
    RTC_Write_1Byte(0x0D,0x83);   //���ʱ����Ч 1HZ
}

//-------------------------------------------------------------------------------
// ��������: void I2C_Start(void)
// ��������: ���߿�ʼλ
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void I2C_Start(void)
{
    TRIS_SDA = 0;
    TRIS_SCL = 0;
    Delay10TCY();
    I2C_SCL = 1;
    Delay10TCY();
    I2C_SDA = 1;
    Delay10TCY();
    I2C_SDA = 0;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
}

//-------------------------------------------------------------------------------
// ��������: void I2C_Stop(void)
// ��������: ���߽���λ
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void I2C_Stop(void)
{
    TRIS_SDA = 0;
    Delay10TCY();
    I2C_SDA = 0;
    Delay10TCY();
    I2C_SCL = 1;
    Delay10TCY();
    I2C_SDA = 1;
    Delay10TCY();
}

//-------------------------------------------------------------------------------
// ��������: INT8U  SLAVE_ACK(void)
// ��������: ������Ӧ����
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U Slave_ACK(void)
{
    INT8U ACK;
    TRIS_SDA = 1;
    TRIS_SCL = 0;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
    I2C_SCL = 1;
    Delay10TCY();
    ACK = I2C_SDA;    //�� ACK
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
    return (ACK);
}

//-------------------------------------------------------------------------------
// ��������: void Master_ACK(void)
// ��������: ������Ӧ����
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void Master_ACK(void)
{
    TRIS_SDA = 0;
    TRIS_SCL = 0;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
    I2C_SDA = 0;
    Delay10TCY();
    I2C_SCL = 1;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
}

//-------------------------------------------------------------------------------
// ��������: void NO_ACK(void)
// ��������: ��Ӧ��λ
// ���ú���: ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void NO_ACK(void)
{
    INT8U NACK;
    TRIS_SDA = 0;
    Delay10TCY();
    I2C_SDA = 1;
    Delay10TCY();
    I2C_SCL = 1;
    Delay10TCY();
//  while(I2C_SDA);    //������
    NACK = I2C_SDA;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
}

//-------------------------------------------------------------------------------
// ��������: void I2C_write_1Byte(INT8U EEDATA)
// ��������: дһ���ֽ�����
// ���ú���: ��
// ��ڲ���: ��Ҫд������� EEdata
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void I2C_write_1Byte(INT8U EEdata)
{
    INT8U i;
    TRIS_SDA = 0;
    TRIS_SCL = 0;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
    for(i=0; i<8; i++)
    {
        if(EEdata&0x80)   I2C_SDA = 1;
        else              I2C_SDA = 0;
        EEdata = EEdata <<1;
        Delay10TCY();
        I2C_SCL = 1;
        Delay10TCY();
        I2C_SCL = 0;
        Delay10TCY();
    }
}

//-------------------------------------------------------------------------------
// ��������: INT8U EE_Read_1Byte(INT16U Addr)
// ��������: FM24W256 ָ����ַ ��1�ֽ�����
// ���ú���: ��
// ��ڲ���: ��Ҫ�������ݵĵ�ַ Addr(16λ)
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U EE_Read_1Byte(INT16U Addr)
{
    INT8U i;
    INT8U rdata;
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ��־
        I2C_Start();

        I2C_write_1Byte(0xA0);            //����
        Slave_ACK();

        I2C_write_1Byte(addrH);           //�ߵ�ַ
        Slave_ACK();

        I2C_write_1Byte(addrL);           //�͵�ַ
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA1);            //����
        Slave_ACK();
        TRIS_SDA = 1;
        TRIS_SCL = 0;
        Delay10TCY();
        for(i=0; i<8; i++)                //��1�ֽ�����
        {
            rdata <<= 1;
            I2C_SCL = 0;
            Delay10TCY();
            I2C_SCL = 1;
            Delay10TCY();
            if(I2C_SDA)   rdata |= 0x01;
            Delay10TCY();
        }
        NO_ACK();                         //��Ӧ��
        I2C_Stop();                       //ֹͣ
        Status.I2C_RW_Busy = 0;           //���߿���
        return (rdata);
    }
    return 0;
}

//-------------------------------------------------------------------------------
// ��������: void EE_Write_1Byte(INT16U Addr,INT8U EEdata)
// ��������: FM24W256 ָ����ַ д1�ֽ�����
// ���ú���: ��
// ��ڲ���: д�����ݺ����ݵ�ַ  ��ַ(16λ)  ����(8λ)
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void EE_Write_1Byte(INT16U Addr,INT8U EEdata)
{
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);

    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ
        EEPROM_WP2 = 0;                   //��ֹ����

        I2C_Start();                      //����

        I2C_write_1Byte(0xA0);            //����
        Slave_ACK();

        I2C_write_1Byte(addrH);           //�ߵ�ַ
        Slave_ACK();

        I2C_write_1Byte(addrL);           //�͵�ַ
        Slave_ACK();

        I2C_write_1Byte(EEdata);          //����
        Slave_ACK();

        I2C_Stop();                       //ַͣ

        EEPROM_WP2 = 1;                   //��������
        Status.I2C_RW_Busy = 0;           //���߿���
    }
}


//-------------------------------------------------------------------------------
// ��������: void EE_Read_nByte( Addr, *dptr,len )
// ��������: FM24W256 ָ����ַ ��N�ֽ�����
// ���ú���: ��
// ��ڲ���: Addr���ݵ�ַ(16λ) ; len���ݳ���(8λ) ; *dptr������
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void EE_Read_nByte(INT16U Addr,INT8U *dptr,INT16U len)
{
    INT8U i;
    INT8U rdata = 0;

    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ
        I2C_Start();

        I2C_write_1Byte(0xA0);            //����
        Slave_ACK();

        I2C_write_1Byte(addrH);           //�ߵ�ַ
        Slave_ACK();

        I2C_write_1Byte(addrL);           //�͵�ַ
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA1);            //����
        Slave_ACK();

        while(len != 0)                   //������
        {
            TRIS_SDA = 1;
            TRIS_SCL = 0;
            Nop();
            Nop();
            Nop();
            for(i=0; i<8; i++)            //��1byte
            {
                rdata <<= 1;
                I2C_SCL = 0;
                Delay10TCY();
                I2C_SCL = 1;
                Delay10TCY();
                if(I2C_SDA)   rdata |= 0x01;
                Nop();
                Nop();
            }
            *dptr = rdata;                //�洢����
            dptr ++;
            len --;
            if(len == 0)  NO_ACK();       //��Ӧ��
            else          Master_ACK();   //Ӧ��
        }
        I2C_Stop();                       //ֹͣ
        Status.I2C_RW_Busy = 0;           //���߿���
    }
}

//-------------------------------------------------------------------------------
// ��������: void EE_Write_nByte( Addr, *dptr,len )
// ��������: FM24W256 ָ����ַ дN�ֽ�����
// ���ú���: ��
// ��ڲ���: Addr���ݵ�ַ(16λ) ; len���ݳ���(8λ) ; *dptr(������)
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void EE_Write_nByte(INT16U Addr,INT8U *dptr,INT16U len)
{
    INT8U i,j;
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ��־
        EEPROM_WP2 = 0;                   //��ֹ����

        I2C_Start();                      //����

        I2C_write_1Byte(0xA0);            //����
        Slave_ACK();

        I2C_write_1Byte(addrH);           //�ߵ�ַ
        Slave_ACK();

        I2C_write_1Byte(addrL);           //�͵�ַ
        Slave_ACK();

        while(len != 0)                   //������
        {
            TRIS_SDA = 0;
            TRIS_SCL = 0;
            Nop();
            Nop();
            Nop();
            j = *dptr;                   //ָ�봫��
            for(i=0; i<8; i++)
            {
                if(j&0x80)   I2C_SDA = 1;
                else            I2C_SDA = 0;
                j = j <<1;
                Delay10TCY();
                I2C_SCL = 1;
                Delay10TCY();
                I2C_SCL = 0;
                Nop();
                Nop();
                Nop();//Delay10TCY();
            }
            dptr ++;                      //ָ��
            len --;
            if(len == 0)  NO_ACK();       //��Ӧ��
            else          Slave_ACK();    //Ӧ��
        }
        I2C_Stop();                       //ֹͣ
        EEPROM_WP2 = 1;                  //��������
        Status.I2C_RW_Busy = 0;           //���߿���
    }
}

//-------------------------------------------------------------------------------
// ��������: INT8U RTC_Read_1Byte(INT8U Addr)
// ��������: PCF8563 ָ����ַ ��1�ֽ�����
// ���ú���: ��
// ��ڲ���: Addr���ݵ�ַ(8λ)
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U RTC_Read_1Byte(INT8U Addr)
{
    INT8U i;
    INT8U rdata = 0;
    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ��־
        I2C_Start();

        I2C_write_1Byte(0xA2);            //����
        Slave_ACK();

        I2C_write_1Byte(Addr);            //��ַ
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA3);            //����
        Slave_ACK();
        TRIS_SDA = 1;
        TRIS_SCL = 0;
        Delay10TCY();
        for(i=0; i<8; i++)
        {
            rdata <<= 1;
            I2C_SCL = 0;
            Delay10TCY();
            I2C_SCL = 1;
            Delay10TCY();
            if(I2C_SDA)   rdata |= 0x01;
            Delay10TCY();
        }
        NO_ACK();                         //��Ӧ��
        I2C_Stop();                       //ֹͣ
        Status.I2C_RW_Busy = 0;           //���߿���
        return rdata;
    }
    return 0;
}

//-------------------------------------------------------------------------------
// ��������: void RTC_Write_1Byte(INT8U Addr,INT8U EEdata)
// ��������: PCF8563 ָ����ַ д1�ֽ�����
// ���ú���: ��
// ��ڲ���: Addr���ݵ�ַ(8λ)   EEdataд������(8λ)
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void RTC_Write_1Byte(INT8U Addr,INT8U EEdata)
{
    if(Status.I2C_RW_Busy == 0)           //���߿���
    {
        Status.I2C_RW_Busy = 1;           //����æ��־
        I2C_Start();                      //����

        I2C_write_1Byte(0xA2);            //����
        Slave_ACK();

        I2C_write_1Byte(Addr);            //��ַ
        Slave_ACK();

        I2C_write_1Byte(EEdata);          //����
        Slave_ACK();

        I2C_Stop();                       //ֹͣ
        Status.I2C_RW_Busy = 0;           //���߿���
    }
}

//-------------------------------------------------------------------------------
// ��������: INT8U PCF8563_ReadTime(struct tm *tm_t);
// ��������: PCF8563��ȡʱ��
// ���ú���: RTC_Read_1Byte(addr);
// ��ڲ���: tm_t ��׼ʱ��ڹ�
// ���ز���: OK
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U PCF8563_ReadTime(struct tm *tm_t)
{
    unsigned char temp;                       // ����

    temp = RTC_Read_1Byte(0x02)&0x7f;         // ��
    (*tm_t).tm_sec = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x03)&0x7f;         // ��
    (*tm_t).tm_min = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x04)&0x3f;         // ʱ
    (*tm_t).tm_hour = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x05)&0x3f;         // ��
    (*tm_t).tm_mday = (temp >> 4)*10 + (temp&0x0F);

    (*tm_t).tm_wday = RTC_Read_1Byte(0x06)&0x07; // ����

    temp = (RTC_Read_1Byte(0x07)&0x1f) - 1;   // ��
    (*tm_t).tm_mon = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x08);              // ��
    (*tm_t).tm_year = 2000 + (temp >> 4)*10 + (temp&0x0F);

    return OK;
}

//-------------------------------------------------------------------------------
// ��������: INT8U PCF8563_SetTime(struct tm tm_t)
// ��������: PCF8563����ʱ��
// ���ú���: RTC_Write_1Byte(addr,data);
// ��ڲ���: tm_t ��׼ʱ��ڹ�
// ���ز���: OK
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
INT8U PCF8563_SetTime(struct tm tm_t)
{
    unsigned char temp1,temp2;     //����

    temp1 = (tm_t.tm_sec/10)<<4;   //�� ����д
    temp2 = (tm_t.tm_sec%10);
    RTC_Write_1Byte(0x02,temp1|temp2);

    temp1 = (tm_t.tm_min/10)<<4;   //�� ����д
    temp2 = (tm_t.tm_min%10);
    RTC_Write_1Byte(0x03,temp1|temp2);

    temp1 = (tm_t.tm_hour/10)<<4;  //ʱ ����д
    temp2 = (tm_t.tm_hour%10);
    RTC_Write_1Byte(0x04,temp1|temp2);

    temp1 = (tm_t.tm_mday/10)<<4;  //�� ����д
    temp2 = (tm_t.tm_mday%10);
    RTC_Write_1Byte(0x05,temp1|temp2);

    RTC_Write_1Byte(0x06,tm_t.tm_wday); //���� ����д

    temp1 = ((tm_t.tm_mon + 1)/10) << 4;//�� ����д
    temp2 = ((tm_t.tm_mon + 1)%10);
    RTC_Write_1Byte(0x07,temp1|temp2);


    temp2 = tm_t.tm_year%100;      //�� ת��
    temp1 = (temp2/10)<<4;         //�� ����д
    temp2 = (temp2%10);
    RTC_Write_1Byte(0x08,temp1|temp2);

    RTC_Write_1Byte(0x00,0x00);    //����ʱ��

    return OK;
}

//-------------------------------------------------------------------------------


