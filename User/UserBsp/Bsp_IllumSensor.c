/**
  ******************************************************************************
  * @file    Bsp_IllumSensor.c 
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
#include "Bsp_IllumSensor.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//-------------------------------------------------------------Private define
//寄存器
#define ILLUM_REG_R_STBA      0x0B     //(0x55)    self-test A
#define ILLUM_REG_R_STBB      0x0C     //(0x55)    self-test B
#define ILLUM_REG_R_INFO      0x0D     //(0x5445)  "More Info"
#define ILLUM_REG_R_WIA       0x0F     //(0x49)    "Who I am"
#define ILLUM_REG_R_DATAX     0x10     //(0x0000)  UV-B Sensor output value
#define ILLUM_REG_R_DATAZ     0x14     //(0x0000)  UV-A Sensor output value
#define ILLUM_REG_R_INS1      0x16     //(0x00)    Interrupt source
#define ILLUM_REG_R_STAT1     0x18     //(0x00)    Status
#define ILLUM_REG_R_INL       0x1A     //(0xF0)    Interrupt latch release
#define ILLUM_REG_RW_CTRL1    0x1B     //(0x22)    Control register 1
#define ILLUM_REG_RW_CTRL2    0x1C     //(0x76)    Control register 2
#define ILLUM_REG_RW_CTRL3    0x1D     //(0x00)    Control register 3
#define ILLUM_REG_RW_CTRL4    0x1E     //(0x80)    Control register 4
#define ILLUM_REG_RW_INC1     0x1F     //(0xE8)    Interrupt control
#define ILLUM_REG_RW_OFFX     0x20     //(0x0000)  UV-B sensor offset drift value
#define ILLUM_REG_RW_OFFZ     0x24     //(0x0000)  UV-A sensor offset drift value
#define ILLUM_REG_RW_ITHR     0x26     //(0x0FFF)  Interrupt threshold value
#define ILLUM_REG_RW_PRST     0x30     //(0x00)    Preset time
//
#define ILLUM_MAX_VAL         2550
#define ILLUM_DIV_VAL         10
#define ILLUM_DUMMY_DATA      1000
#define ILLUM_CHECK_MAX       40000
#define ILLUM_CHECK_MIN       0
#define ILLUM_POR_COUNT       1000
#define ILLUM_ADR_R           0x1A
#define ILLUM_ADR_W           0x1B
//
#define ILLUM_IIC_ERROR_COUNT 2000
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t Illum_DebugTest_Enable=0;
//器件状态 0-无效，1-有效
static uint8_t Illum_DeviceState=0;
//------------------------------- 用户函数声明 ----------------
static uint8_t IllumSensor_SelfCheck(void);
//------------------------------- 照度驱动 --------------------------------------
//-------------------------------------------------------------------------------
// 函数功能: 照度初始化
//-------------------------------------------------------------------------------
uint8_t BspIllumSensor_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;
    //时钟初始化
    BSP_ILLUM_IICX_RCC_ENABLE;
    BSP_ILLUM_GPIO_RCC_ENABLE;
    //GPIO初始化
    BSP_ILLUM_IIC_SDA_REMAP;
    BSP_ILLUM_IIC_SCL_REMAP;
#if   (defined(STM32F1))    
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_OD;
#elif (defined(STM32F4))  
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin =  BSP_ILLUM_IIC_SCL_PIN ;
    GPIO_Init(BSP_ILLUM_IIC_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin =  BSP_ILLUM_IIC_SDA_PIN;
    GPIO_Init(BSP_ILLUM_IIC_SDA_PORT, &GPIO_InitStructure);
    //IIC初始化
    I2C_DeInit(BSP_ILLUM_IICX);
    I2C_Cmd(BSP_ILLUM_IICX, DISABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    //I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_Cmd(BSP_ILLUM_IICX, ENABLE);
    I2C_Init(BSP_ILLUM_IICX, &I2C_InitStructure);
    //I2C_AcknowledgeConfig(I2C1, ENABLE);
    //-----器件自检
    if(OK==IllumSensor_SelfCheck())
    {
        Illum_DeviceState=1;
        return OK;
    }
    else
    {
        return ERR;
    }
    //-----
}
//-------------------------------------------------------------------------------
// 函数功能: 照度寄存器读n个字节
//-------------------------------------------------------------------------------
static uint8_t ILLUM_IIC_Read_nByte(uint8_t addr,uint8_t *buf,uint16_t num)
{
    uint16_t err_count=0;
    //
    if(num==0)
    {
        return ERR;
    }
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_ILLUM_IICX, I2C_FLAG_BUSY))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_ILLUM_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_ILLUM_IICX, ILLUM_ADR_R, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //寄存器地址
    I2C_SendData(BSP_ILLUM_IICX, addr);
    //检查VE8
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_ILLUM_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //接收地址
    I2C_Send7bitAddress(BSP_ILLUM_IICX, ILLUM_ADR_W, I2C_Direction_Receiver);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
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
            I2C_AcknowledgeConfig(BSP_ILLUM_IICX, DISABLE);   //最后一位后要关闭应答的
            //I2C_GenerateSTOP(I2C1, ENABLE);           //发送停止位     //It's a bug.
        }

        err_count = 0;
        while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_BYTE_RECEIVED)) //检测VE7        //always dead here.
        {
            if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
            {
                goto goto_SensorIICReadNByte;
            }
        }
        *buf = I2C_ReceiveData(BSP_ILLUM_IICX);
        buf++;
        num--;
    }
    I2C_AcknowledgeConfig(BSP_ILLUM_IICX,ENABLE);
    I2C_GenerateSTOP(BSP_ILLUM_IICX, ENABLE);
    return OK;
goto_SensorIICReadNByte:
    I2C_ClearFlag(BSP_ILLUM_IICX, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_ILLUM_IICX, ENABLE);
    return ERR;
}
//-------------------------------------------------------------------------------
// 函数功能: 照度寄存器写n个字节
//-------------------------------------------------------------------------------
static uint8_t ILLUM_IIC_Write_nByte(uint16_t addr,uint8_t *buf,uint16_t num)
{
    uint16_t err_count=0;
    //
    if(num==0)
    {
        return ERR;
    }
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_ILLUM_IICX, I2C_FLAG_BUSY))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_GenerateSTART(BSP_ILLUM_IICX, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_ILLUM_IICX, ILLUM_ADR_R, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_SendData(BSP_ILLUM_IICX, addr);
    err_count = 0;
    while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    while(num--)
    {
        I2C_SendData(BSP_ILLUM_IICX, *buf);
        buf++;
        err_count = 0;
        while(!I2C_CheckEvent(BSP_ILLUM_IICX, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(err_count++ >= ILLUM_IIC_ERROR_COUNT)
            {
                goto goto_SensorIICWriteNByte;
            }
        };
    }
    I2C_GenerateSTOP(BSP_ILLUM_IICX, ENABLE);
    return OK;
goto_SensorIICWriteNByte:
    I2C_ClearFlag(BSP_ILLUM_IICX, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_ILLUM_IICX, ENABLE);
    return ERR;
}
//-------------------------------------------------------------------------------
// 函数功能: 照度自检
//-------------------------------------------------------------------------------
static uint8_t IllumSensor_SelfCheck(void)
{
    uint8_t res;
    uint8_t buf[2];
    buf[0]=0xE2;  //    1 1 1 000 1 0
    res=ILLUM_IIC_Write_nByte(ILLUM_REG_RW_CTRL1,buf,1);
    buf[0]=0x56;  //    0 1 0 1 0110
    res=ILLUM_IIC_Write_nByte(ILLUM_REG_RW_CTRL2,buf,1);
    //
    buf[0]=0;
    res=ILLUM_IIC_Read_nByte(ILLUM_REG_R_STBA,buf,1);
    if(buf[0]!=0x55 || res!=OK)
    {
        return ERR;
    }
    //
    buf[0]=0x20;
    res=ILLUM_IIC_Write_nByte(ILLUM_REG_RW_CTRL3,buf,1);
    //
    buf[0]=0;
    res=ILLUM_IIC_Read_nByte(ILLUM_REG_R_STBA,buf,1);
    if(buf[0]!=0xAA || res!=OK)
    {
        return ERR;
    }
    //
    buf[0]=0;
    res=ILLUM_IIC_Read_nByte(ILLUM_REG_R_STBA,buf,1);
    if(buf[0]!=0x55 || res!=OK)
    {
        return ERR;
    }
    //
    return OK;
}

//-------------------------------------------------------------------------------
// 函数功能: 照度读值
//-------------------------------------------------------------------------------
uint8_t BspIllumSensor_Read(uint32_t *pIllum)
{
    uint8_t buf[2];
    uint16_t i16,j16;
    uint8_t res;
    //
    if(Illum_DeviceState==0)
    {
        return ERR;
    }
    //启动转换
    buf[0]=0x40;
    res=ILLUM_IIC_Write_nByte(ILLUM_REG_RW_CTRL3,buf,1);
    //延时
    MODULE_OS_DELAY_MS(10);
    //
    buf[0]=0;
    res=ILLUM_IIC_Read_nByte(ILLUM_REG_R_STAT1,buf,1);
    buf[0] &=0x40;
    if(buf[0]!=0x40)
    {
        return ERR;
    }
    //检测
    res=ILLUM_IIC_Read_nByte(ILLUM_REG_R_DATAX,buf,2);
    if(res!=OK)
    {
        return res;
    }
    //计算
    i16=buf[1];
    i16<<=8;
    i16+=buf[0];

    j16 = (float)i16 * (40.0f / 1.0f);
    //
    if(j16 > ILLUM_CHECK_MAX)
    {
        *pIllum = 0;
        res = ERR;
        return res;
    }
    *pIllum = j16;
    return OK;
}
//-------------------------------------------------------------------------------
// 函数功能: 照度测试
//-------------------------------------------------------------------------------
void BspIllumSensor_DebugTest_100ms(void)
{
    uint8_t res;
    uint8_t *pbuf;
    uint32_t illum_value;
    static uint8_t s_count=0;
    if(Illum_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        res=BspIllumSensor_Read(&illum_value);
        if(res==OK)
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: Illum-%ld(lx)\r\n",illum_value);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: Illum-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
void BspIllumSensor_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Illum_DebugTest_Enable=1;
    }
    else
    {
        Illum_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
