/**
  ******************************************************************************
  * @file    Bsp_Eeprom.c 
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
//------------------------------- Includes -----------------------------------
#include "includes.h"
#include "Bsp_Eeprom.h"
#include "uctsk_Debug.h"
#include "Bsp_Tim.h"
//------------------------------- Typedef ------------------------------------
static uint8_t I2C_RW_Busy=0;
static void I2C_write_1Byte(uint8_t EEdata);
static void NO_ACK(void);
static void Master_ACK(void);
static uint8_t Slave_ACK(void);
static void I2C_Stop(void);
static void I2C_Start(void);
/*******************************************************************************
* 函数功能: EEPROM管脚配置
*******************************************************************************/
void BspEeprom_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    BSP_EEPROM_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_EEPROM_SCL_PIN;
    GPIO_Init(BSP_EEPROM_SCL_PORT, &GPIO_InitStructure);
#ifdef BSP_EEPROM_WP_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_EEPROM_WP_PIN;
    GPIO_Init(BSP_EEPROM_WP_PORT, &GPIO_InitStructure);
#endif
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_EEPROM_SDA_PIN;
    GPIO_Init(BSP_EEPROM_SDA_PORT, &GPIO_InitStructure);
    BSP_EEPROM_SCL_H;
    BSP_EEPROM_SDA_H;
    BSP_EEPROM_WP_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
}

/*******************************************************************************
* 函数功能: 通过指定I2C接口读取多个字节数据
*******************************************************************************/
uint8_t BspEeprom_Read(uint16_t addr,uint8_t *buf,uint16_t num)
{
    uint8_t i;
    uint8_t rdata = 0;

    uint8_t addrH,addrL;
    addrH = (uint8_t)((addr >>8)&0x7F);
    addrL = (uint8_t)(addr);
    if(I2C_RW_Busy == 0)
    {
        I2C_RW_Busy = 1;
        I2C_Start();

        I2C_write_1Byte(BSP_EEPROM_IIC_ADDR);
        Slave_ACK();

        I2C_write_1Byte(addrH);
        Slave_ACK();

        I2C_write_1Byte(addrL);
        Slave_ACK();

        I2C_Start();
        I2C_write_1Byte(BSP_EEPROM_IIC_ADDR|0x01);
        Slave_ACK();

        while(num != 0)
        {
            BSP_EEPROM_NOP;
            BSP_EEPROM_SDA_H;
            for(i=0; i<8; i++)
            {
                rdata <<= 1;
                BSP_EEPROM_SCL_L;
                //Delay10TCY();
                BSP_EEPROM_NOP;
                BSP_EEPROM_NOP;
                BSP_EEPROM_SCL_H;
                //Delay10TCY();
                BSP_EEPROM_NOP;
                BSP_EEPROM_NOP;
                if(BSP_EEPROM_SDA_R)   rdata |= 0x01;
                BSP_EEPROM_NOP;
            }
            *buf = rdata;
            buf ++;
            num --;
            if(num == 0)  NO_ACK();
            else          Master_ACK();
        }
        I2C_Stop();
        I2C_RW_Busy = 0;
    }
    return 0;
}

/*******************************************************************************
* 函数功能: 通过指定I2C接口写入多个字节数据
*******************************************************************************/
uint8_t BspEeprom_Write(uint16_t addr,uint8_t *buf,uint16_t num)
{
    uint8_t i,j;
    uint8_t addrH,addrL;
    addrH = (uint8_t)((addr >>8)&0x7F);
    addrL = (uint8_t)(addr);
    if(I2C_RW_Busy == 0)
    {
        I2C_RW_Busy = 1;
        BSP_EEPROM_WP_L;

        I2C_Start();

        I2C_write_1Byte(BSP_EEPROM_IIC_ADDR);
        Slave_ACK();

        I2C_write_1Byte(addrH);
        Slave_ACK();

        I2C_write_1Byte(addrL);
        Slave_ACK();

        while(num!= 0)
        {
            BSP_EEPROM_NOP;
            j = *buf;
            for(i=0; i<8; i++)
            {
                if(j&0x80)   BSP_EEPROM_SDA_H;
                else         BSP_EEPROM_SDA_L;
                j = j <<1;
                BSP_EEPROM_NOP;
                BSP_EEPROM_SCL_H;
                BSP_EEPROM_NOP;
                BSP_EEPROM_SCL_L;
                BSP_EEPROM_NOP;
            }
            buf ++;
            num --;
            if(num == 0)  NO_ACK();
            else          Slave_ACK();
        }
        I2C_Stop();
        BSP_EEPROM_WP_H ;
        I2C_RW_Busy = 0;
    }
    return 0;
}
//-------------------------------------------------------------------------------
//
void I2C_Start(void)
{
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
}
//
void I2C_Stop(void)
{
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
}
//
uint8_t Slave_ACK(void)
{
    uint8_t ACK;
    BSP_EEPROM_SDA_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    if(BSP_EEPROM_SDA_R)ACK=1;
    else ACK=0;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    return (ACK);
}
//
void Master_ACK(void)
{
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
}
//
void NO_ACK(void)
{
    uint8_t NACK;
    NACK=NACK;
    BSP_EEPROM_SDA_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SDA_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_H;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
//  while(I2C_SDA);    //会死机
    if(BSP_EEPROM_SDA_R)NACK=1;
    else NACK=0;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
}
//
void I2C_write_1Byte(uint8_t EEdata)
{
    uint8_t i;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    BSP_EEPROM_SCL_L;
    //Delay10TCY();
    BSP_EEPROM_NOP;
    BSP_EEPROM_NOP;
    for(i=0; i<8; i++)
    {
        if(EEdata&0x80)   BSP_EEPROM_SDA_H;
        else              BSP_EEPROM_SDA_L;
        EEdata = EEdata <<1;
        //Delay10TCY();
        BSP_EEPROM_NOP;
        BSP_EEPROM_NOP;
        BSP_EEPROM_SCL_H;
        //Delay10TCY();
        BSP_EEPROM_NOP;
        BSP_EEPROM_NOP;
        BSP_EEPROM_SCL_L;
        //Delay10TCY();
        BSP_EEPROM_NOP;
        BSP_EEPROM_NOP;
    }
}
/*******************************************************************************
* 函数功能: 接口函数---测试EEPROM
*******************************************************************************/
void BspEeprom_DebugTestOnOff(uint8_t OnOff)
{
    //-----
    uint8_t *pbuf;
    uint8_t res;
    uint16_t i16,j16,k16;
    uint32_t i32;
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //
    sprintf((char *)pbuf,"----------Memory Test EEPROM CHIP Begin:\r\n");
    DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
    //提示用户输入次数
    sprintf((char *)pbuf,"Please Input Test Count(1-100):\r\n");
    DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
    res = DebugInputNum(&j16,5);
    DebugOut((int8_t*)"\r\n",strlen("\r\n"));
    if(res!=OK || j16==0 || j16>100)
    {
        DebugOut((int8_t*)"Input Error\r\n",strlen("Input Error\r\n"));
        goto Goto_BspEeprom_DebugTestOnOff;
    }
    for(k16=1; k16<=j16; k16++)
    {
        //轮番读写(256B)
        Tim_1ms_Count=0;
        for(i32=0; i32<BSP_EERPOM_END_ADDR; i32+=BSP_EERPOM_MAX_WRITE_SIZE)
        {
            for(i16=0; i16<BSP_EERPOM_MAX_WRITE_SIZE; i16++)
            {
                pbuf[i16]=i16;
            }
            //写256
            BspEeprom_Write(i32,pbuf,BSP_EERPOM_MAX_WRITE_SIZE);
            if(res!=OK)break;
            //清缓存
            memset(pbuf,0,BSP_EERPOM_MAX_WRITE_SIZE);
            //读256
            BspEeprom_Read(i32,pbuf,BSP_EERPOM_MAX_WRITE_SIZE);
            if(res!=OK)break;
            //比对
            for(i16=0; i16<BSP_EERPOM_MAX_WRITE_SIZE; i16++)
            {
                if(i16==pbuf[i16]);
                else break;
            }
            if(i16==BSP_EERPOM_MAX_WRITE_SIZE)
            {
                //比对成功
                if((i32!=0) &&((i32/BSP_EERPOM_MAX_WRITE_SIZE)%(1024L/BSP_EERPOM_MAX_WRITE_SIZE)==0))
                {
                    //sprintf((char *)Member_PrintBuf,"%ld KB OK.\r\n",(i32/1024));
                    //DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                }
            }
            else
            {
                //比对失败
                sprintf((char *)pbuf,"EEPROM CHIP TEST Addr %ld ERR!\r\n",i32);
                DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
                res=ERR;
                break;
            }
        }
        if(res==OK)
        {
            sprintf((char *)pbuf,"%ld KB OK.\r\n",(i32/1024));
            DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
            sprintf((char *)pbuf,"EEPROM CHIP TEST(Count:%d,Timer:%ld(ms)) OK!\r\n",k16,Tim_1ms_Count);
            DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
        }
        else
        {
            break;
        }
    }
Goto_BspEeprom_DebugTestOnOff:    
    //
    sprintf((char *)pbuf,"-----Memory Test EEPROM CHIP End\r\n");
    DebugOut((int8_t*)pbuf,strlen((char *)pbuf));
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    //-----
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
