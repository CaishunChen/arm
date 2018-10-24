/**
  ******************************************************************************
  * @file    Bsp_Bmp180.c 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */ 
//------------------------------- Includes --------------------
#include "Bsp_Bmp180.h"
#include "Bsp_Twi.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//------------------------------- 数据结构 --------------------
typedef struct BSP180_S_CALIBRATION
{
    int16_t  AC1;
    int16_t  AC2;
    int16_t  AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t  B1;
    int16_t  B2;
    int16_t  MB;
    int16_t  MC;
    int16_t  MD;
} BSP180_S_CALIBRATION;
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t BspBmp180_DebugTest_Enable=0;
//
static BSP180_S_CALIBRATION Bsp180_sCalibration;
//------------------------------- 用户函数声明 ----------------
//------------------------------- 用户函数 --------------------
/*
******************************************************************************
* 函数功能: 读数据
******************************************************************************
*/
void BspBmp180_Read_Calibration(void)
{
    uint8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    BspTwi_Read_nByte(BSP_BMP180_ADDR,0xaa,pbuf,22);
    Bsp180_sCalibration.AC1=Count_2ByteToWord(pbuf[0],pbuf[1]);
    Bsp180_sCalibration.AC2=Count_2ByteToWord(pbuf[2],pbuf[3]);
    Bsp180_sCalibration.AC3=Count_2ByteToWord(pbuf[4],pbuf[5]);
    Bsp180_sCalibration.AC4=Count_2ByteToWord(pbuf[6],pbuf[7]);
    Bsp180_sCalibration.AC5=Count_2ByteToWord(pbuf[8],pbuf[9]);
    Bsp180_sCalibration.AC6=Count_2ByteToWord(pbuf[10],pbuf[11]);
    Bsp180_sCalibration.B1=Count_2ByteToWord(pbuf[12],pbuf[13]);
    Bsp180_sCalibration.B2=Count_2ByteToWord(pbuf[14],pbuf[15]);
    Bsp180_sCalibration.MB=Count_2ByteToWord(pbuf[16],pbuf[17]);
    Bsp180_sCalibration.MC=Count_2ByteToWord(pbuf[18],pbuf[19]);
    Bsp180_sCalibration.MD=Count_2ByteToWord(pbuf[20],pbuf[21]);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
//温度单位: 度*10 气压单位: pa 高度单位:米
uint8_t BspBmp180_Read(int16_t *pTemp,uint32_t *pAirPressure,int16_t *pAltitude)
{
    long X1,X2,X3,B3,B5,B6,B7,T,P;
    unsigned long B4;
    uint16_t i16;
    int32_t i32;
    uint8_t buf[3];
    static uint8_t s_first=1;
    if(s_first==1)
    {
        //-----提取芯片ID(固定为55)
        BspTwi_Read_nByte(BSP_BMP180_ADDR,0xD0,&s_first,1);
        //-----器件版本
        BspTwi_Read_nByte(BSP_BMP180_ADDR,0xD1,&s_first,1);
        //-----提取校准值
        BspBmp180_Read_Calibration();
        //
        s_first=0;
    }
    //读取温度(向F4写2E,向F6读2Byte)
    buf[0]=0xf4;
    buf[1]=0x2E;
    BspTwi_Write_nByte(BSP_BMP180_ADDR,buf,2);
    MODULE_OS_DELAY_MS(10);
    BspTwi_Read_nByte(BSP_BMP180_ADDR,0xf6,buf,2);
    i16=Count_2ByteToWord(buf[0],buf[1]);
    //读取气压(向F4写34,向F6读2Byte)
    buf[0]=0xf4;
    buf[1]=0x34+(BSP_BMP180_OSS<<6);
    BspTwi_Write_nByte(BSP_BMP180_ADDR,buf,2);
    MODULE_OS_DELAY_MS(2+(3<<BSP_BMP180_OSS));
    BspTwi_Read_nByte(BSP_BMP180_ADDR,0xf6,buf,3);
    i32=Count_4ByteToLong(0,buf[0],buf[1],buf[2]);
    i32=i32>>(8-BSP_BMP180_OSS);
    //j16=Count_2ByteToWord(buf[0],buf[1]);
    //Bmp180_IIC_Read_nByte(0xf8,buf,2);
    //k16=Count_2ByteToWord(buf[0],buf[1]);
    //j16=(((int32_t)j16<<8+k16))>>(8-OSS);
    //温度换算
    X1 = ((i16-Bsp180_sCalibration.AC6)*Bsp180_sCalibration.AC5)>>15;
    X2 = ((long)Bsp180_sCalibration.MC<<11)/(X1+Bsp180_sCalibration.MD);
    B5 = X1+X2;
    T = (B5+8)>>4;
    if(pTemp!=NULL)
    {
        *pTemp = T/1.0;
    }
    //气压换算
    B6 = B5-4000;
    X1 = (Bsp180_sCalibration.B2*(B6*B6>>12))>>11;
    X2 = (Bsp180_sCalibration.AC2*B6)>>11;
    X3 = X1+X2;
    B3 = (((((long)Bsp180_sCalibration.AC1)*4+X3)<<BSP_BMP180_OSS)+2)/4;
    X1 = (Bsp180_sCalibration.AC3*B6)>>13;
    X2 = (Bsp180_sCalibration.B1*((B6*B6)>>12))>>16;
    X3 = ((X1+X2)+2)>>2;
    B4 = (Bsp180_sCalibration.AC4*(unsigned long)(X3+32768))>>15;
    B7 = (unsigned long)(i32-B3)*(50000>>BSP_BMP180_OSS);
    if (B7 < 0x80000000)
    {
        P = (B7*2)/B4;
    }
    else
    {
        P = (B7/B4)*2;
    }
    X1 = (P/256.0)*(P/256.0);
    X1 = (X1*3038)>>16;
    X2 = (-7357*P)>>16;
    P = P+((X1+X2+3791)>>4);
    if(pAirPressure!=NULL)
    {
        *pAirPressure = P;
    }
    //海拔换算
    if(pAltitude!=NULL)
    {
        *pAltitude = 44330.0*(1.0-pow((P/101325.0),(1.0/5.255)));
    }
    //赋值
    return OK;
}
/*
******************************************************************************
* 函数功能: 100ms定时处理
******************************************************************************
*/
void BspBmp180_DebugTest_100ms(void)
{
    uint8_t res;
    uint8_t *pbuf;
    int16_t temp,altitude;
    uint32_t air_pressure;
    static uint8_t s_count=0;
    static uint8_t s_debug_first=1;
    
    if(BspBmp180_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(s_debug_first==1)
        {
            s_debug_first=0;
            //-----提取芯片ID(固定为55)
            BspTwi_Read_nByte(BSP_BMP180_ADDR,0xD0,&res,1);
            sprintf((char*)pbuf,"BMP180-FixID(0x55):0x%02x\r\n",res);
            DebugOutStr((int8_t*)pbuf);
            //-----器件版本
            BspTwi_Read_nByte(BSP_BMP180_ADDR,0xD1,&res,1);
            sprintf((char*)pbuf,"BMP180-Version:%03d\r\n",res);
            DebugOutStr((int8_t*)pbuf);
            //-----提取校准值
            BspBmp180_Read_Calibration();
            sprintf((char*)pbuf,"BMP180 Calibration :\r\n");
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC1:%d\r\n",Bsp180_sCalibration.AC1);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC2:%d\r\n",Bsp180_sCalibration.AC2);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC3:%d\r\n",Bsp180_sCalibration.AC3);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC4:%d\r\n",Bsp180_sCalibration.AC4);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC5:%d\r\n",Bsp180_sCalibration.AC5);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"  AC6:%d\r\n",Bsp180_sCalibration.AC6);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"   B1:%d\r\n",Bsp180_sCalibration.B1);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"   B2:%d\r\n",Bsp180_sCalibration.B2);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"   MB:%d\r\n",Bsp180_sCalibration.MB);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"   MC:%d\r\n",Bsp180_sCalibration.MC);
            DebugOutStr((int8_t*)pbuf);
            sprintf((char*)pbuf,"   MD:%d\r\n",Bsp180_sCalibration.MD);
            DebugOutStr((int8_t*)pbuf);
        }
        res=BspBmp180_Read(&temp,&air_pressure,&altitude);
        if(res==OK)
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: T-%d.%d AP-%d(pa) A-%d(m)\r\n",temp/10,temp%10,air_pressure,altitude);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: TempAirPressure-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    else
    {
        s_debug_first=1;
    }
}
/*
******************************************************************************
* 函数功能: 调试开关
******************************************************************************
*/
void BspBmp180_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspBmp180_DebugTest_Enable=1;
    }
    else
    {
        BspBmp180_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


