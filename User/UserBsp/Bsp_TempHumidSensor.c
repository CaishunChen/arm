/**
  ******************************************************************************
  * @file    Bsp_TempHumidSensor.c 
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
#include "Bsp_TempHumidSensor.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
#include "Bsp_Twi.h"
#include "Bsp_Dac.h"
//-------------------------------------------------------------Private define
//寄存器
#define TEMPHUMID_REG_R_STR   0x0C     //(0x00)    Self test response
#define TEMPHUMID_REG_R_INF1  0x0D     //(0x11)    More info Version
#define TEMPHUMID_REG_R_INF2  0x0E     //(0x23)    More info ALPS
#define TEMPHUMID_REG_R_WIA   0x0F     //(0x49)    Who I am
#define TEMPHUMID_REG_R_HUMI1 0x10     //(0x00)    HUMI[7:0]
#define TEMPHUMID_REG_R_HUMI2 0x11     //(0x00)    HUMI[15:8]
#define TEMPHUMID_REG_R_TEMP1 0x12     //(0x00)    TEMP[7:0]
#define TEMPHUMID_REG_R_TEMP2 0x13     //(0x00)    TEMP[15:8]
#define TEMPHUMID_REG_R_STAT  0x18     //(0x00)    Status
#define TEMPHUMID_REG_RW_CTL1 0x1B     //(ROM-55)  Control
#define TEMPHUMID_REG_R_RDAT1 0x20     //(ROM-4F)  Reserved data1
#define TEMPHUMID_REG_R_RDAT2 0x21     //(ROM-50)  Reserved data2
#define TEMPHUMID_REG_R_RDAT3 0x22     //(ROM-51)  Reserved data3
#define TEMPHUMID_REG_R_RDAT4 0x23     //(ROM-52)  Reserved data4
#define TEMPHUMID_REG_AC_REST 0x30     //(0x00)    Reset command
#define TEMPHUMID_REG_AC_DET  0x31     //(0x00)    Sensor detect command
#define TEMPHUMID_REG_AC_STST 0x32     //(0x00)    Self test command
//
#define TEMPHUMID_ADR_R       0x30
#define TEMPHUMID_ADR_W       0x31
//
#define TEMPHUMID_IIC_ERROR_COUNT 2000
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t BspTempHumidSensor_DebugTest_Enable=0;
//器件状态 0-无效，1-有效
static uint8_t TempHumidSensor_DeviceState=0;
//------------------------------- 用户函数声明 ----------------
//------------------------------- 照度驱动 ---------------------------------------
//-------------------------------------------------------------------------------
// 函数功能: 温湿度寄存器读n个字节
//-------------------------------------------------------------------------------

static uint8_t TempHumidSensor_IIC_Read_nByte(uint8_t addr,uint8_t *buf,uint16_t num)
{
    uint16_t err_count=0;
    //
    if(num==0)
    {
        return ERR;
    }
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_TEMPHUMIDSENSOR_IICX, I2C_FLAG_BUSY))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_TEMPHUMIDSENSOR_IICX, TEMPHUMID_ADR_R, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //寄存器地址
    I2C_SendData(BSP_TEMPHUMIDSENSOR_IICX, addr);
    //检查VE8
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //接收地址
    I2C_Send7bitAddress(BSP_TEMPHUMIDSENSOR_IICX, TEMPHUMID_ADR_W, I2C_Direction_Receiver);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    while (num)
    {
        //-----后加
        GPIO_SetBits(GPIOB, GPIO_Pin_7);
        //-----
        if(num==1)
        {
            I2C_AcknowledgeConfig(BSP_TEMPHUMIDSENSOR_IICX, DISABLE);   //最后一位后要关闭应答的
            //I2C_GenerateSTOP(I2C1, ENABLE);           //发送停止位     //It's a bug.
        }

        err_count = 0;
        while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_BYTE_RECEIVED)) //检测VE7        //always dead here.
        {
            if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
            {
                goto goto_SensorIICReadNByte;
            }
        }
        *buf = I2C_ReceiveData(BSP_TEMPHUMIDSENSOR_IICX);
        buf++;
        num--;
    }
    I2C_AcknowledgeConfig(BSP_TEMPHUMIDSENSOR_IICX,ENABLE);
    I2C_GenerateSTOP(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    return OK;
goto_SensorIICReadNByte:
    I2C_ClearFlag(BSP_TEMPHUMIDSENSOR_IICX, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    return ERR;
}

//-------------------------------------------------------------------------------
// 函数功能:温湿度寄存器写n个字节
//-------------------------------------------------------------------------------

static uint8_t TempHumidSensor_IIC_Write_nByte(uint8_t addr,uint8_t *buf,uint16_t num)
{
    uint16_t err_count=0;
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_TEMPHUMIDSENSOR_IICX, I2C_FLAG_BUSY))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_GenerateSTART(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_TEMPHUMIDSENSOR_IICX, TEMPHUMID_ADR_R, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_SendData(BSP_TEMPHUMIDSENSOR_IICX, addr);
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    while(num--)
    {
        I2C_SendData(BSP_TEMPHUMIDSENSOR_IICX, *buf);
        buf++;
        err_count = 0;
        while(!I2C_CheckEvent(BSP_TEMPHUMIDSENSOR_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(err_count++ >= TEMPHUMID_IIC_ERROR_COUNT)
            {
                goto goto_SensorIICWriteNByte;
            }
        };
    }
    I2C_GenerateSTOP(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    return OK;
goto_SensorIICWriteNByte:
    I2C_ClearFlag(BSP_TEMPHUMIDSENSOR_IICX, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    return ERR;
}

//-------------------------------------------------------------------------------
// 函数功能: 温湿度自检
//-------------------------------------------------------------------------------
static uint8_t TempHumidSensor_SelfCheck(void)
{
    uint8_t res;
    uint8_t buf[2];
    //
    buf[0]=0;
    res=TempHumidSensor_IIC_Read_nByte(TEMPHUMID_REG_R_STR,buf,1);
    //res=BspTwi_Read_nByte(BSP_TEMPHUMID_ADDR,TEMPHUMID_REG_R_STR,buf,1);
    if(buf[0]!=0x55 || res!=OK)
    {
        return ERR;
    }
    //
    res=TempHumidSensor_IIC_Write_nByte(TEMPHUMID_REG_AC_STST,buf,0);
    //buf[0]=TEMPHUMID_REG_AC_STST;
    //res=BspTwi_Write_nByte(BSP_TEMPHUMID_ADDR,buf,1);
    //
    buf[0]=0;
    res=TempHumidSensor_IIC_Read_nByte(TEMPHUMID_REG_R_STR,buf,1);
    //res=BspTwi_Read_nByte(BSP_TEMPHUMID_ADDR,TEMPHUMID_REG_R_STR,buf,1);
    if(buf[0]!=0xAA || res!=OK)
    {
        return ERR;
    }
    //
    buf[0]=0;
    res=TempHumidSensor_IIC_Read_nByte(TEMPHUMID_REG_R_STR,buf,1);
    //res=BspTwi_Read_nByte(BSP_TEMPHUMID_ADDR,TEMPHUMID_REG_R_STR,buf,1);
    if(buf[0]!=0x55 || res!=OK)
    {
        return ERR;
    }
    //
    return OK;
}
//-------------------------------------------------------------------------------
// 函数功能: 照度初始化
//-------------------------------------------------------------------------------
uint8_t BspTempHumidSensor_Init(void)
{
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;
    //
    BSP_TEMPHUMIDSENSOR_POWER_INIT;
    //断电
    BSP_TEMPHUMIDSENSOR_POWER_OFF;
    MODULE_OS_DELAY_MS(2000);
    //供电
    BSP_TEMPHUMIDSENSOR_POWER_ON;
    MODULE_OS_DELAY_MS(2000);
    
    //时钟初始化
    BSP_TEMPHUMIDSENSOR_GPIO_RCC_ENABLE;
    BSP_TEMPHUMIDSENSOR_IICX_RCC_ENABLE;
    //GPIO初始化
    BSP_TEMPHUMIDSENSOR_IIC_SDA_REMAP;
    BSP_TEMPHUMIDSENSOR_IIC_SCL_REMAP;
#if   (defined(STM32F1))    
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_OD;
#elif (defined(STM32F4))  
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif     
    GPIO_InitStructure.GPIO_Pin =  BSP_TEMPHUMIDSENSOR_IIC_SCL_PIN ;
    GPIO_Init(BSP_TEMPHUMIDSENSOR_IIC_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin =  BSP_TEMPHUMIDSENSOR_IIC_SDA_PIN;
    GPIO_Init(BSP_TEMPHUMIDSENSOR_IIC_SDA_PORT, &GPIO_InitStructure);
    //IIC初始化
    I2C_DeInit(BSP_TEMPHUMIDSENSOR_IICX);
    I2C_Cmd(BSP_TEMPHUMIDSENSOR_IICX, DISABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    //I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_Cmd(BSP_TEMPHUMIDSENSOR_IICX, ENABLE);
    I2C_Init(BSP_TEMPHUMIDSENSOR_IICX, &I2C_InitStructure);
    //I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    //-----器件自检
    if(OK==TempHumidSensor_SelfCheck())
    {
        TempHumidSensor_DeviceState=1;
        return OK;
    }
    else
    {
        TempHumidSensor_DeviceState=1;
        return ERR;
    }
    //-----
}
//-------------------------------------------------------------------------------
// 函数功能: 照度读值
//-------------------------------------------------------------------------------
uint8_t BspTempHumidSensor_Read(int16_t *pTemp,uint16_t *pHumi)
{
    uint8_t buf[4];
    uint16_t i16;
    uint8_t res;
    //
    if(TempHumidSensor_DeviceState==0)
    {
        return ERR;
    }
    //启动转换
    buf[0]=0x72;
    res=TempHumidSensor_IIC_Write_nByte(TEMPHUMID_REG_AC_DET,buf,1);
    //buf[0]=TEMPHUMID_REG_AC_DET;
    //buf[1]=0x72;
    //res=BspTwi_Write_nByte(BSP_TEMPHUMID_ADDR,buf,2);
    //延时
    MODULE_OS_DELAY_MS(10);
    //
    res=TempHumidSensor_IIC_Write_nByte(TEMPHUMID_REG_AC_DET,buf,0);
    //buf[0]=TEMPHUMID_REG_AC_DET;
    //res=BspTwi_Write_nByte(BSP_TEMPHUMID_ADDR,buf,1);
    //检测
    res=TempHumidSensor_IIC_Read_nByte(TEMPHUMID_REG_R_HUMI1,buf,4);
    //res=BspTwi_Read_nByte(BSP_TEMPHUMID_ADDR,TEMPHUMID_REG_R_HUMI1,buf,4);
    if(res!=OK)
    {
        return res;
    }
    //计算湿度
    i16=Count_2ByteToWord(buf[1],buf[0]);
    *pHumi=((i16*15625L)/100000)-140;
    //计算温度
    i16=Count_2ByteToWord(buf[3],buf[2]);
    *pTemp=(i16*2)/10-419;
    //温度校正
    *pTemp += BSP_TEMPHUMIDSENSOR_TEMP_CORRENT;
    //
    return OK;
}
//-------------------------------------------------------------------------------
// 函数功能: 照度测试
//-------------------------------------------------------------------------------
void BspTempHumidSensor_DebugTest_100ms(void)
{
    uint8_t res;
    uint8_t *pbuf;
    int16_t temp_value;
    uint16_t Humi_value;
    static uint8_t s_count=0;
    if(BspTempHumidSensor_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        res=BspTempHumidSensor_Read(&temp_value,&Humi_value);
        if(res==OK)
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: Temp-%d(0.1度) Humi-%d\r\n",temp_value,Humi_value);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: TempHumi-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
void BspTempHumidSensor_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspTempHumidSensor_DebugTest_Enable=1;
    }
    else
    {
        BspTempHumidSensor_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
