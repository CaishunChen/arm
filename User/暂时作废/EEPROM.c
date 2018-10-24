//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: MPLAB X IDE V1.95                                //
//                    编 译 器: XC16  V1.20                                      //
//                    芯片型号: dsPIC33EP256GM710                                //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: EEPROM.c                                         //
//                    作    者: 李中学                                           //
//                    时    间: 2013-10-21    版本:  0.1                         //
//-------------------------------------------------------------------------------//
//
// 文件描述：采用软件模拟I2C总线,读写 FM24W256   启用了写保护功能
//
//
// 注意事项：FM24W256为铁电串行EEPROM，内部存储不分页，地址连续
//
//
// 修改记录：
//
//
// 修改时间:
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
#define I2C_SDA   PORTGbits.RG3                   //I2C控制引脚
#define I2C_SCL   PORTGbits.RG2
#define TRIS_SDA  TRISGbits.TRISG3                //I2C控制 I/O
#define TRIS_SCL  TRISGbits.TRISG2
//----------------
#endif

INT8U I2C_RD_Buffer[MAX_EEPROM_BUFFER] = {0};  //I2C总线读缓冲
INT8U I2C_WR_Buffer[MAX_EEPROM_BUFFER] = {0};  //I2C总线写缓冲


//-------------------------------------------------------------------------------
// 函数名称: void Delay10TCY(void)
// 函数功能: I2C总线延时函数
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
// 函数名称: I2C_Init(void)
// 函数功能: I2C初始化程序
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void I2C_Init(void)
{
    TRIS_SCL = 0;       //输出
    TRIS_SDA = 0;
    I2C_SCL = 1;        //空闲
    I2C_SDA = 1;
    EEPROM_WP2 = 1;     //保护
    Delay10TCY();
    RTC_Write_1Byte(0x0D,0x83);   //输出时钟有效 1HZ
}

//-------------------------------------------------------------------------------
// 函数名称: void I2C_Start(void)
// 函数功能: 总线开始位
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
// 函数名称: void I2C_Stop(void)
// 函数功能: 总线结束位
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
// 函数名称: INT8U  SLAVE_ACK(void)
// 函数功能: 从器件应答函数
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
    ACK = I2C_SDA;    //读 ACK
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
    return (ACK);
}

//-------------------------------------------------------------------------------
// 函数名称: void Master_ACK(void)
// 函数功能: 主器件应答函数
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
// 函数名称: void NO_ACK(void)
// 函数功能: 无应答位
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
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
//  while(I2C_SDA);    //会死机
    NACK = I2C_SDA;
    Delay10TCY();
    I2C_SCL = 0;
    Delay10TCY();
}

//-------------------------------------------------------------------------------
// 函数名称: void I2C_write_1Byte(INT8U EEDATA)
// 函数功能: 写一个字节数据
// 调用函数: 无
// 入口参数: 需要写入的数据 EEdata
// 返回参数: 无
// 修改说明：
// 修改时间：
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
// 函数名称: INT8U EE_Read_1Byte(INT16U Addr)
// 函数功能: FM24W256 指定地址 读1字节数据
// 调用函数: 无
// 入口参数: 需要读出数据的地址 Addr(16位)
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U EE_Read_1Byte(INT16U Addr)
{
    INT8U i;
    INT8U rdata;
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙标志
        I2C_Start();

        I2C_write_1Byte(0xA0);            //命令
        Slave_ACK();

        I2C_write_1Byte(addrH);           //高地址
        Slave_ACK();

        I2C_write_1Byte(addrL);           //低地址
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA1);            //命令
        Slave_ACK();
        TRIS_SDA = 1;
        TRIS_SCL = 0;
        Delay10TCY();
        for(i=0; i<8; i++)                //读1字节数据
        {
            rdata <<= 1;
            I2C_SCL = 0;
            Delay10TCY();
            I2C_SCL = 1;
            Delay10TCY();
            if(I2C_SDA)   rdata |= 0x01;
            Delay10TCY();
        }
        NO_ACK();                         //无应答
        I2C_Stop();                       //停止
        Status.I2C_RW_Busy = 0;           //总线空闲
        return (rdata);
    }
    return 0;
}

//-------------------------------------------------------------------------------
// 函数名称: void EE_Write_1Byte(INT16U Addr,INT8U EEdata)
// 函数功能: FM24W256 指定地址 写1字节数据
// 调用函数: 无
// 入口参数: 写入数据和数据地址  地址(16位)  数据(8位)
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void EE_Write_1Byte(INT16U Addr,INT8U EEdata)
{
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);

    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙
        EEPROM_WP2 = 0;                   //禁止保护

        I2C_Start();                      //启动

        I2C_write_1Byte(0xA0);            //命令
        Slave_ACK();

        I2C_write_1Byte(addrH);           //高地址
        Slave_ACK();

        I2C_write_1Byte(addrL);           //低地址
        Slave_ACK();

        I2C_write_1Byte(EEdata);          //数据
        Slave_ACK();

        I2C_Stop();                       //停址

        EEPROM_WP2 = 1;                   //开启保护
        Status.I2C_RW_Busy = 0;           //总线空闲
    }
}


//-------------------------------------------------------------------------------
// 函数名称: void EE_Read_nByte( Addr, *dptr,len )
// 函数功能: FM24W256 指定地址 读N字节数据
// 调用函数: 无
// 入口参数: Addr数据地址(16位) ; len数据长度(8位) ; *dptr缓冲区
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void EE_Read_nByte(INT16U Addr,INT8U *dptr,INT16U len)
{
    INT8U i;
    INT8U rdata = 0;

    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙
        I2C_Start();

        I2C_write_1Byte(0xA0);            //命令
        Slave_ACK();

        I2C_write_1Byte(addrH);           //高地址
        Slave_ACK();

        I2C_write_1Byte(addrL);           //低地址
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA1);            //命令
        Slave_ACK();

        while(len != 0)                   //读数据
        {
            TRIS_SDA = 1;
            TRIS_SCL = 0;
            Nop();
            Nop();
            Nop();
            for(i=0; i<8; i++)            //读1byte
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
            *dptr = rdata;                //存储数据
            dptr ++;
            len --;
            if(len == 0)  NO_ACK();       //无应答
            else          Master_ACK();   //应答
        }
        I2C_Stop();                       //停止
        Status.I2C_RW_Busy = 0;           //总线空闲
    }
}

//-------------------------------------------------------------------------------
// 函数名称: void EE_Write_nByte( Addr, *dptr,len )
// 函数功能: FM24W256 指定地址 写N字节数据
// 调用函数: 无
// 入口参数: Addr数据地址(16位) ; len数据长度(8位) ; *dptr(缓冲区)
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void EE_Write_nByte(INT16U Addr,INT8U *dptr,INT16U len)
{
    INT8U i,j;
    INT8U addrH,addrL;
    addrH = (INT8U)((Addr >>8)&0x7F);
    addrL = (INT8U)(Addr);
    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙标志
        EEPROM_WP2 = 0;                   //禁止保护

        I2C_Start();                      //启动

        I2C_write_1Byte(0xA0);            //命令
        Slave_ACK();

        I2C_write_1Byte(addrH);           //高地址
        Slave_ACK();

        I2C_write_1Byte(addrL);           //低地址
        Slave_ACK();

        while(len != 0)                   //读数据
        {
            TRIS_SDA = 0;
            TRIS_SCL = 0;
            Nop();
            Nop();
            Nop();
            j = *dptr;                   //指针传递
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
            dptr ++;                      //指针
            len --;
            if(len == 0)  NO_ACK();       //无应答
            else          Slave_ACK();    //应答
        }
        I2C_Stop();                       //停止
        EEPROM_WP2 = 1;                  //开启保护
        Status.I2C_RW_Busy = 0;           //总线空闲
    }
}

//-------------------------------------------------------------------------------
// 函数名称: INT8U RTC_Read_1Byte(INT8U Addr)
// 函数功能: PCF8563 指定地址 读1字节数据
// 调用函数: 无
// 入口参数: Addr数据地址(8位)
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U RTC_Read_1Byte(INT8U Addr)
{
    INT8U i;
    INT8U rdata = 0;
    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙标志
        I2C_Start();

        I2C_write_1Byte(0xA2);            //命令
        Slave_ACK();

        I2C_write_1Byte(Addr);            //地址
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(0xA3);            //命令
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
        NO_ACK();                         //无应答
        I2C_Stop();                       //停止
        Status.I2C_RW_Busy = 0;           //总线空闲
        return rdata;
    }
    return 0;
}

//-------------------------------------------------------------------------------
// 函数名称: void RTC_Write_1Byte(INT8U Addr,INT8U EEdata)
// 函数功能: PCF8563 指定地址 写1字节数据
// 调用函数: 无
// 入口参数: Addr数据地址(8位)   EEdata写入数据(8位)
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void RTC_Write_1Byte(INT8U Addr,INT8U EEdata)
{
    if(Status.I2C_RW_Busy == 0)           //总线空闲
    {
        Status.I2C_RW_Busy = 1;           //总线忙标志
        I2C_Start();                      //启动

        I2C_write_1Byte(0xA2);            //命令
        Slave_ACK();

        I2C_write_1Byte(Addr);            //地址
        Slave_ACK();

        I2C_write_1Byte(EEdata);          //数据
        Slave_ACK();

        I2C_Stop();                       //停止
        Status.I2C_RW_Busy = 0;           //总线空闲
    }
}

//-------------------------------------------------------------------------------
// 函数名称: INT8U PCF8563_ReadTime(struct tm *tm_t);
// 函数功能: PCF8563读取时间
// 调用函数: RTC_Read_1Byte(addr);
// 入口参数: tm_t 标准时间节构
// 返回参数: OK
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U PCF8563_ReadTime(struct tm *tm_t)
{
    unsigned char temp;                       // 变量

    temp = RTC_Read_1Byte(0x02)&0x7f;         // 秒
    (*tm_t).tm_sec = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x03)&0x7f;         // 分
    (*tm_t).tm_min = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x04)&0x3f;         // 时
    (*tm_t).tm_hour = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x05)&0x3f;         // 日
    (*tm_t).tm_mday = (temp >> 4)*10 + (temp&0x0F);

    (*tm_t).tm_wday = RTC_Read_1Byte(0x06)&0x07; // 星期

    temp = (RTC_Read_1Byte(0x07)&0x1f) - 1;   // 月
    (*tm_t).tm_mon = (temp >> 4)*10 + (temp&0x0F);

    temp = RTC_Read_1Byte(0x08);              // 年
    (*tm_t).tm_year = 2000 + (temp >> 4)*10 + (temp&0x0F);

    return OK;
}

//-------------------------------------------------------------------------------
// 函数名称: INT8U PCF8563_SetTime(struct tm tm_t)
// 函数功能: PCF8563设置时间
// 调用函数: RTC_Write_1Byte(addr,data);
// 入口参数: tm_t 标准时间节构
// 返回参数: OK
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
INT8U PCF8563_SetTime(struct tm tm_t)
{
    unsigned char temp1,temp2;     //变量

    temp1 = (tm_t.tm_sec/10)<<4;   //秒 操作写
    temp2 = (tm_t.tm_sec%10);
    RTC_Write_1Byte(0x02,temp1|temp2);

    temp1 = (tm_t.tm_min/10)<<4;   //分 操作写
    temp2 = (tm_t.tm_min%10);
    RTC_Write_1Byte(0x03,temp1|temp2);

    temp1 = (tm_t.tm_hour/10)<<4;  //时 操作写
    temp2 = (tm_t.tm_hour%10);
    RTC_Write_1Byte(0x04,temp1|temp2);

    temp1 = (tm_t.tm_mday/10)<<4;  //日 操作写
    temp2 = (tm_t.tm_mday%10);
    RTC_Write_1Byte(0x05,temp1|temp2);

    RTC_Write_1Byte(0x06,tm_t.tm_wday); //星期 操作写

    temp1 = ((tm_t.tm_mon + 1)/10) << 4;//月 操作写
    temp2 = ((tm_t.tm_mon + 1)%10);
    RTC_Write_1Byte(0x07,temp1|temp2);


    temp2 = tm_t.tm_year%100;      //年 转换
    temp1 = (temp2/10)<<4;         //年 操作写
    temp2 = (temp2%10);
    RTC_Write_1Byte(0x08,temp1|temp2);

    RTC_Write_1Byte(0x00,0x00);    //激活时钟

    return OK;
}

//-------------------------------------------------------------------------------


