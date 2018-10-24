/**
  ******************************************************************************
  * @file    Bsp_Ap3216c.c 
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
#include "Bsp_Ap3216c.h"
#include "Bsp_Twi.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//------------------------------- 数据结构 --------------------
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t BspAp3216c_DebugTest_Enable=0;

#ifdef BSP_AP3216C_AUTO
//连续测量
uint16_t BspAp3216c_Als=0,BspAp3216c_Ps=0,BspAp3216c_Ir=0;
#endif
//------------------------------- 用户函数声明 ----------------
uint8_t BspAp3216c_Init(void)
{
	//uint8_t buf[2];
	BspTwi_Init();
#ifdef   BSP_AP3216C_INT_PIN
    nrf_gpio_cfg_input(BSP_AP3216C_INT_PIN, NRF_GPIO_PIN_PULLUP);
#endif
	/*
	buf[0]	=	BSP_AP3216C_REG_SYS_CONF;
	buf[1]	=	4;
	BspTwi_Write_nByte(BSP_AP3216C_ADDR,buf,2);
	*/
    return OK;
}
uint8_t BspAp3216c_Read(uint16_t *p_ALS,uint16_t *p_PS,uint16_t *p_IR)
{
    uint8_t buf[2];
    if(p_ALS!=NULL&&p_PS==NULL)
    {
        buf[0]=BSP_AP3216C_REG_SYS_CONF;
        buf[1]=0x05;
        BspTwi_Write_nByte(BSP_AP3216C_ADDR,buf,2);
        MODULE_OS_DELAY_MS(300);
        BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_ALS_DATA_L,buf,2);
        *p_ALS=Count_2ByteToWord(buf[1],buf[0]);
    }
    else if(p_ALS==NULL&&p_PS!=NULL)
    {
        buf[0]=BSP_AP3216C_REG_SYS_CONF;
        buf[1]=0x06;
        BspTwi_Write_nByte(BSP_AP3216C_ADDR,buf,2);
        MODULE_OS_DELAY_MS(150);
        BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_PS_DATA_L,buf,2);
        *p_PS=Count_2ByteToWord(buf[1],buf[0]);
    }
    else
    {
        buf[0]=BSP_AP3216C_REG_SYS_CONF;
        buf[1]=0x07;
        BspTwi_Write_nByte(BSP_AP3216C_ADDR,buf,2);
        MODULE_OS_DELAY_MS(400);
        BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_ALS_DATA_L,buf,2);
        *p_ALS=Count_2ByteToWord(buf[1],buf[0]);
        BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_PS_DATA_L,buf,2);
        *p_PS=Count_2ByteToWord(buf[1],buf[0]);
    }
    if(p_IR!=NULL)
    {
        BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_IR_DATA_L,buf,2);
        *p_IR=Count_2ByteToWord(buf[1],buf[0]);
    }
    return OK;
}
#ifdef BSP_AP3216C_AUTO
static void BspAp3216c_Read_Auto(void)
{
	static uint8_t step=0xFF;
    static uint8_t buf[2]={0};
#if		(HARDWARE_VER==30 && HARDWARE_SUB_VER==3)
	return;
#elif	(HARDWARE_VER==35 && HARDWARE_SUB_VER==3)
	return;
#endif
	switch(step)
	{
		case 0:	
        case 1:
		case 2:
		case 3:        	
		//case 4:
		//case 5:
		//case 6:
		//case 7:
			step++;
			break;
		case 4:	
			BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_ALS_DATA_L,buf,2);
        	BspAp3216c_Als=Count_2ByteToWord(buf[1],buf[0]);
        	BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_PS_DATA_L,buf,2);
        	BspAp3216c_Ps=Count_2ByteToWord(buf[1],buf[0]);
        	BspTwi_Read_nByte(BSP_AP3216C_ADDR,BSP_AP3216C_REG_SYS_IR_DATA_L,buf,2);
        	BspAp3216c_Ir=Count_2ByteToWord(buf[1],buf[0]);
			step++;
			break;
		case 0xFF:
			buf[0]=BSP_AP3216C_REG_SYS_CONF;
        	buf[1]=0x03;
        	BspTwi_Write_nByte(BSP_AP3216C_ADDR,buf,2);
			step=0;
			break;
		default:
			step=0;
			break;
	}
}
#endif
void BspAp3216c_DebugTest_100ms(void)
{    
    uint8_t *pbuf;    
    static uint8_t s_count=0;
    static uint8_t s_first=1;
#ifdef	BSP_AP3216C_AUTO
	BspAp3216c_Read_Auto();
#else
	uint8_t res;
	uint16_t i16=0,j16=0,k16=0;
#endif	
    if(BspAp3216c_DebugTest_Enable==1)
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
#ifdef	BSP_AP3216C_AUTO
		sprintf((char*)pbuf,"ALS-%05d PS-%05d IR-%05d\r\n",BspAp3216c_Als,BspAp3216c_Ps,BspAp3216c_Ir);
        DebugOutStr((int8_t*)pbuf);
#else
        res=BspAp3216c_Read(&i16,&j16,&k16);
        if(res==OK)
        {
            sprintf((char*)pbuf,"ALS-%05d PS-%05d IR-%05d\r\n",i16,j16,k16);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: AP3216C-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
#endif		
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }	
    else
    {
        s_first=1;
    }
}
void BspAp3216c_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspAp3216c_DebugTest_Enable=1;
    }
    else
    {
        BspAp3216c_DebugTest_Enable=0;
    }
}
