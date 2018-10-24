/**
  ******************************************************************************
  * @file    Bsp_Twi.c 
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
#include "Bsp_Twi.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
#if (defined(NRF51)||defined(NRF52))
#include "app_util_platform.h"
#include "nrf_drv_twi.h"
#include "app_twi.h"
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(BSP_TWI_X);
#endif

static uint8_t BspTwi_DebugTest_Enable=0;
#ifndef  BSP_TWI_MUTEX_DISABLE
MODULE_OS_SEM(BspTwi_Sem);
#endif

#ifdef	BSP_TWI_MODE_SOFTWARE
static void BspTwi_Delay(void);
static void BspTwi_Start(void);
static void BspTwi_Stop(void);
static void BspTwi_SendByte(uint8_t _ucByte);
static uint8_t BspTwi_ReadByte(void);
static uint8_t BspTwi_WaitAck(void);
static void BspTwi_Ack(void);
static void BspTwi_NAck(void);
/**
 * @brief   I2C总线位延迟，最快400KHz
 * @note    I2C总线位延迟，最快400KHz
 * @param   None
 * @return  None
 */
static void BspTwi_Delay(void)
{
    uint8_t i;

    /*　
        CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
        循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
        循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
        循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

        上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

        实际应用选择400KHz左右的速率即可
    */
    for (i = 0; i < 30; i++);
}
/**
 * @brief   CPU发起I2C总线启动信号
 * @note    CPU发起I2C总线启动信号
 * @param   None
 * @return  None
 */
static void BspTwi_Start(void)
{
    BSP_TWI_SDA_H;
    BSP_TWI_SCL_H;
    BspTwi_Delay();
    BSP_TWI_SDA_L;
    BspTwi_Delay();
    BSP_TWI_SCL_L;
    BspTwi_Delay();
}
/**
 * @brief   CPU发起I2C总线停止信号
 * @note    CPU发起I2C总线停止信号
 * @param   None
 * @return  None
 */

static void BspTwi_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    BSP_TWI_SDA_L;
    BSP_TWI_SCL_H;
    BspTwi_Delay();
    BSP_TWI_SDA_H;
    BspTwi_Delay();
}
/**
 * @brief   CPU向I2C总线设备发送8bit数据
 * @note    CPU向I2C总线设备发送8bit数据
 * @param   None
 * @return  None
 */

static void BspTwi_SendByte(uint8_t _ucByte)
{
    uint8_t i;
    // 先发送字节的高位bit7
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            BSP_TWI_SDA_H;
        }
        else
        {
            BSP_TWI_SDA_L;
        }
        BspTwi_Delay();
        BSP_TWI_SCL_H;
        BspTwi_Delay();
        BSP_TWI_SCL_L;
        if (i == 7)
        {
            BSP_TWI_SDA_H;
        }
        _ucByte <<= 1;  /* 左移一个bit */
        BspTwi_Delay();
    }
}
/**
 * @brief   CPU从I2C总线设备读取8bit数据
 * @note    CPU从I2C总线设备读取8bit数据
 * @param   None
 * @return  读到的数据
 */

static uint8_t BspTwi_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    // 读到第1个bit为数据的bit7
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        BSP_TWI_SCL_H;
        BspTwi_Delay();
        if (BSP_TWI_SDA_R)
        {
            value++;
        }
        BSP_TWI_SCL_L;
        BspTwi_Delay();
    }
    return value;
}
/**
 * @brief   读取器件的ACK应答信号
 * @note    CPU产生一个时钟，并读取器件的ACK应答信号
 * @param   None
 * @return  返回0表示正确应答，1表示无器件响应
 */

static uint8_t BspTwi_WaitAck(void)
{
    uint8_t re;

    BSP_TWI_SDA_H;
    BspTwi_Delay();
    BSP_TWI_SCL_H;
    BspTwi_Delay();
    if (BSP_TWI_SDA_R)
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    BSP_TWI_SCL_L;
    BspTwi_Delay();
    return re;
}
/**
 * @brief   CPU产生一个ACK信号
 * @note    CPU产生一个ACK信号
 * @param   None
 * @return  None
 */

static void BspTwi_Ack(void)
{
    BSP_TWI_SDA_L;
    BspTwi_Delay();
    BSP_TWI_SCL_H;
    BspTwi_Delay();
    BSP_TWI_SCL_L;
    BspTwi_Delay();
    BSP_TWI_SDA_H;
}
/**
 * @brief   CPU产生1个NACK信号
 * @note    CPU产生1个NACK信号
 * @param   None
 * @return  None
 */

static void BspTwi_NAck(void)
{
    BSP_TWI_SDA_H;
    BspTwi_Delay();
    BSP_TWI_SCL_H;
    BspTwi_Delay();
    BSP_TWI_SCL_L;
    BspTwi_Delay();
}

#endif

/**
 * @brief   TWI初始化函数
 * @note    初始化时钟，GPIO，TWI
 * @param   None
 * @return  None
 */

uint8_t BspTwi_Init(void)
{
#if   (defined(BSP_TWI_MODE_HARDWARE))
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;
    //时钟初始化
    BSP_TWI_GPIO_RCC_ENABLE;
    BSP_TWI_IICX_RCC_ENABLE;
    //GPIO初始化
    BSP_TWI_SDA_REMAP;
    BSP_TWI_SCL_REMAP;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin =  BSP_TWI_SCL_PIN ;
    GPIO_Init(BSP_TWI_SCL_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin =  BSP_TWI_SDA_PIN;
    GPIO_Init(BSP_TWI_SDA_PORT, &GPIO_InitStructure);
    //IIC初始化
    I2C_DeInit(BSP_TWI_X);
    I2C_Cmd(BSP_TWI_X, DISABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    //I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;
    I2C_Cmd(BSP_TWI_X, ENABLE);
    I2C_Init(BSP_TWI_X, &I2C_InitStructure);
    //I2C_AcknowledgeConfig(I2C1, ENABLE);
#elif (defined(NRF51)||defined(NRF52))
    ret_code_t err_code;
    const nrf_drv_twi_config_t twi_config =
    {
        .scl                = BSP_TWI_SCL_PIN,
        .sda                = BSP_TWI_SDA_PIN,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOW,
        /*.clear_bus_init     = false*/
			  .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
		if(err_code==NRF_SUCCESS)
		{
			nrf_drv_twi_enable(&m_twi);
		}
#endif
    return OK;
#elif (defined(BSP_TWI_MODE_SOFTWARE))
	
#endif
	//创建互斥信号量
#ifndef  BSP_TWI_MUTEX_DISABLE
#ifdef MODULE_OS_MUTEX_CREATE
    MODULE_OS_MUTEX_CREATE(BspTwi_Sem,NULL,NULL);
#else
    MODULE_OS_SEM_CREATE(BspTwi_Sem,"Sem_BspTwi",1);
#endif
#endif
}
/*
******************************************************************************
* 函数功能: 读n个字节
******************************************************************************
*/
uint8_t BspTwi_Read_nByte(uint8_t device_addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
#if   (defined(BSP_TWI_MODE_HARDWARE))
    uint16_t err_count=0;
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_ERR err;
    MODULE_OS_SEM_PEND(BspTwi_Sem,0,TRUE,err);    
    err=err;
#endif    
    err_count=err_count;
#if     (defined(STM32F1)||defined(STM32F4))
    //
    if(num==0)
    {
#ifndef  BSP_TWI_MUTEX_DISABLE
        MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
        return ERR;
    }
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_TWI_X, I2C_FLAG_BUSY))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_TWI_X, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_TWI_X, device_addr+1, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //寄存器地址
    I2C_SendData(BSP_TWI_X, addr);
    //检查VE8
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //产生IIC1传输START条件
    I2C_GenerateSTART(BSP_TWI_X, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICReadNByte;
        }
    };
    //接收地址
    I2C_Send7bitAddress(BSP_TWI_X, device_addr, I2C_Direction_Receiver);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
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
            I2C_AcknowledgeConfig(BSP_TWI_X, DISABLE);   //最后一位后要关闭应答的
            //I2C_GenerateSTOP(I2C1, ENABLE);           //发送停止位     //It's a bug.
        }

        err_count = 0;
        while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_BYTE_RECEIVED)) //检测VE7        //always dead here.
        {
            if(err_count++ >= BSP_TWI_ERROR_COUNT)
            {
                goto goto_SensorIICReadNByte;
            }
        }
        *buf = I2C_ReceiveData(BSP_TWI_X);
        buf++;
        num--;
    }
    I2C_AcknowledgeConfig(BSP_TWI_X,ENABLE);
    I2C_GenerateSTOP(BSP_TWI_X, ENABLE);
#ifndef  BSP_TWI_MUTEX_DISABLE    
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    return OK;
goto_SensorIICReadNByte:
    I2C_ClearFlag(BSP_TWI_X, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_TWI_X, ENABLE);
#ifndef  BSP_TWI_MUTEX_DISABLE    
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    return ERR;
#else
    nrf_drv_twi_tx(&m_twi, device_addr>>1, &addr,1, 1);
    nrf_drv_twi_rx(&m_twi, device_addr>>1, buf, num);
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    return OK;
#endif
#elif (defined(BSP_TWI_MODE_SOFTWARE))
#endif
}

/*
******************************************************************************
* 函数功能: 写n个字节
******************************************************************************
*/
uint8_t BspTwi_Write_nByte(uint8_t device_addr,uint8_t *pbuf,uint16_t num)
{
#if   (defined(BSP_TWI_MODE_HARDWARE))
    uint32_t result=0;
    uint16_t err_count=0;
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_ERR err;
    MODULE_OS_SEM_PEND(BspTwi_Sem,0,TRUE,err);
    err=err;
#endif
    err_count=err_count;
    result=result;
#if     (defined(STM32F1)||defined(STM32F4))
    //
    if(num==0)
    {
#ifndef  BSP_TWI_MUTEX_DISABLE
        MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
        return ERR;
    }
    //检测总线忙标志位
    err_count = 0;
    while(I2C_GetFlagStatus(BSP_TWI_X, I2C_FLAG_BUSY))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_GenerateSTART(BSP_TWI_X, ENABLE);
    //检查EV5
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    //发送器件地址
    I2C_Send7bitAddress(BSP_TWI_X, device_addr+1, I2C_Direction_Transmitter);
    //检查VE6
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    I2C_SendData(BSP_TWI_X, *pbuf);
    err_count = 0;
    while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if(err_count++ >= BSP_TWI_ERROR_COUNT)
        {
            goto goto_SensorIICWriteNByte;
        }
    };
    pbuf++;
    while(--num)
    {
        I2C_SendData(BSP_TWI_X, *pbuf);
        pbuf++;
        err_count = 0;
        while(!I2C_CheckEvent(BSP_TWI_X, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if(err_count++ >= BSP_TWI_ERROR_COUNT)
            {
                goto goto_SensorIICWriteNByte;
            }
        };
    }
    I2C_GenerateSTOP(BSP_TWI_X, ENABLE);
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    return OK;
goto_SensorIICWriteNByte:
    I2C_ClearFlag(BSP_TWI_X, I2C_FLAG_AF);
    I2C_GenerateSTOP(BSP_TWI_X, ENABLE);
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    return ERR;
#else
    err_count = BSP_TWI_ERROR_COUNT;
    do
    {
        result = nrf_drv_twi_tx(&m_twi, device_addr>>1, pbuf,num ,0);
        err_count--;
    }
    while ((NRF_SUCCESS != result) && (0 < err_count));
#ifndef  BSP_TWI_MUTEX_DISABLE
    MODULE_OS_SEM_POST(BspTwi_Sem);
#endif
    if(err_count==0)
    {
      return ERR;
    }
    return OK;
#endif
#elif (defined(BSP_TWI_MODE_SOFTWARE))
#endif
}
uint8_t BspTwi_Write1_nByte(uint8_t device_addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num<=20)
    {
        uint8_t buf1[21];
        buf1[0]=addr;
        memcpy(&buf1[1],buf,num);
        return BspTwi_Write_nByte(device_addr,buf1,num+1);
    }
    else
    {
        uint8_t *pbuf;
        uint8_t res;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pbuf[0]=addr;
        memcpy(&pbuf[1],buf,num);
        res = BspTwi_Write_nByte(device_addr,pbuf,num+1);
        Count_DelayUs(1000);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        return res;
    }
}
uint8_t BspTwi_SearchDevice(uint8_t *p_num,uint8_t *p_buf)
{
    uint8_t *pbuf;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    if(BspTwi_DebugTest_Enable==1)
    {
        DebugOutStr("TWI Device Search Start:\r\n");
    }
    pbuf[202]=0;
    for(pbuf[200]=0; pbuf[200]<254; pbuf[200]+=2)
    {
        pbuf[201]=0xFE;
        BspTwi_Read_nByte(pbuf[200],0x00,&pbuf[201],1);
        if(pbuf[201]!=0xFE)
        {
            if(p_buf!=NULL)
            {
                p_buf[pbuf[202]]=pbuf[201];
            }
            pbuf[202]++;
            if(BspTwi_DebugTest_Enable==1)
            {
                switch(pbuf[200])
                {
                    case 0x3C:
                        sprintf((char*)pbuf,"TWI Device %02d: %02x(AP3216)\r\n",pbuf[202],pbuf[200]);
                        DebugOutStr((int8_t*)pbuf);
                        break;
                    case 0xD0:
                    case 0xD2:
                        sprintf((char*)pbuf,"TWI Device %02d: %02x(MPU6000/6050)\r\n",pbuf[202],pbuf[200]);
                        DebugOutStr((int8_t*)pbuf);
                        break;
                    case 0xEE:
                        sprintf((char*)pbuf,"TWI Device %02d: %02x(BMP180)\r\n",pbuf[202],pbuf[200]);
                        DebugOutStr((int8_t*)pbuf);
                        break;
                    default:
                        sprintf((char*)pbuf,"TWI Device %02d: %02x\r\n",pbuf[202],pbuf[200]);
                        DebugOutStr((int8_t*)pbuf);
                        break;
                }
            }
        }
        if(pbuf[202]>=10)
        {
            break;
        }
    }
    if(p_num!=NULL)
    {
        *p_num=pbuf[202];
    }
    if(BspTwi_DebugTest_Enable==1)
    {
        DebugOutStr("TWI Device Search End!\r\n");
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    return OK;
}
void BspTwi_DebugTestOnOff(uint8_t OnOff)
{
    OnOff=OnOff;
    BspTwi_DebugTest_Enable=1;
    BspTwi_SearchDevice(NULL,NULL);
    BspTwi_DebugTest_Enable=0;
}

