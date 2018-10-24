/**
  ******************************************************************************
  * @file    Bsp_BH1750.c 
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
#include "Bsp_BH1750.h"
#include "Bsp_Twi.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//------------------------------- 数据结构 --------------------
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t BspBH1750_DebugTest_Enable=0;
//------------------------------- 用户函数声明 ----------------
uint8_t BspBH1750_Init(void)
{
    return OK;
}
uint8_t BspBH1750_Read(uint16_t *p_ALS)
{
    uint8_t buf[2];
    buf[0]=0x00;
    BspTwi_Write_nByte(BSP_BH1750_ADDR,buf,1);
    buf[0]=0x01;
    BspTwi_Write_nByte(BSP_BH1750_ADDR,buf,1);
    buf[0]=0x21;
    BspTwi_Write_nByte(BSP_BH1750_ADDR,buf,1);
    MODULE_OS_DELAY_MS(1);
    buf[0]=BSP_BH1750_ADDR;
    BspTwi_Read_nByte(BSP_BH1750_ADDR,buf,buf,2);
    *p_ALS=Count_2ByteToWord(buf[1],buf[0]);
    return OK;
}
void BspBH1750_DebugTest_100ms(void)
{
    uint8_t res;
    uint8_t *pbuf;
    uint16_t i16=0;
    static uint8_t s_count=0;
    static uint8_t s_first=1;
    if(BspBH1750_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(s_first==1)
        {
            s_first=0;
        }
        res=BspBH1750_Read(&i16);
        if(res==OK)
        {
            sprintf((char*)pbuf,"ALS-%05d\r\n",i16);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: BH1750-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    else
    {
        s_first=1;
    }
}
void BspBH1750_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspBH1750_DebugTest_Enable=1;
    }
    else
    {
        BspBH1750_DebugTest_Enable=0;
    }
}

/******************************************************************
bh1750fvi子程序
*******************************************************************/
/*
void bh1750_ASCII()
	{
		uchar xdata buf[5];
		ulong xdata i_32;

		wr_1_8563(0x46,0x00);
		delay_1ms();
		wr_1_8563(0x46,0x01);
		delay_1ms();
		wr_1_8563(0x46,0x21);
		
		delay_1ms();
		rd_n_8563(0x47,2,buf);
		i_32 = buf[0]*256+buf[1];
		i_32 = i_32*10;
		i_32 = i_32/12;

		BH1750_Value_16b=i_32;

		buf[0]=(i_32/10000) 		+ '0';
		buf[1]=(i_32%10000/1000)+ '0';
		buf[2]=(i_32%1000/100) 	+ '0';
		buf[3]=(i_32%100/10) 		+ '0';
		buf[4]=(i_32%10) 				+ '0';
		//Send_Mem("光照度：",8);
		//Send_Mem(buf,5);
		i_32=i_32*5000;
		i_32=i_32/65535;
		PWM_DUTY_mV(i_32);
		//LED_display_API(100,'c',' ',' ',' ');
		if(buf[0]=='0')
			{
				LED_display_API(100,' ',buf[1]|0x80,buf[2],buf[3]);
			}	
		else
			{
				LED_display_API(100,buf[0],buf[1]|0x80,buf[2],buf[3]);
			}
	}
	*/
