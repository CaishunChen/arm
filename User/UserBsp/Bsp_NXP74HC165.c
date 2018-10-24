/**
  ******************************************************************************
  * @file    Bsp_NXP74HC165.c 
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
//-------------------------------------------------------------------------------
#include "Bsp_NXP74HC165.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//
uint32_t BspNXP74HC165_Value=0xFFFFFFFF;
static uint8_t BspNXP74HC165_DebugTest_Enable=0;
/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void BspNXP74HC165_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    // GPIO初始化---时钟
    BSP_NXP74HC165_RCC_ENABLE;
    // GPIO初始化---输出
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC165_SH_PIN;
    GPIO_Init(BSP_NXP74HC165_SH_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC165_CLK_PIN;
    GPIO_Init(BSP_NXP74HC165_CLK_PORT, &GPIO_InitStructure);
    // GPIO初始化---输入
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    //GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC165_QH_PIN;
    GPIO_Init(BSP_NXP74HC165_QH_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_output(BSP_NXP74HC165_SH_PIN);
    nrf_gpio_cfg_output(BSP_NXP74HC165_CLK_PIN);
    nrf_gpio_cfg_input(BSP_NXP74HC165_QH_PIN, NRF_GPIO_PIN_PULLUP);
#endif
}
/*
******************************************************************************
* 函数功能: 读取键值
******************************************************************************
*/
uint32_t BspNXP74HC165_read(void)
{
    uint8_t i;
    uint32_t sw_dat  = 0xFFFFFFFF;
    BSP_NXP74HC165_SH_SET;
    NXP74HC165_NOP;
    NXP74HC165_NOP;
    BSP_NXP74HC165_SH_CLR;
    NXP74HC165_NOP;
    NXP74HC165_NOP;
    BSP_NXP74HC165_SH_SET;
    NXP74HC165_NOP;
    NXP74HC165_NOP;
    if(BSP_NXP74HC165_QH_R)
    {
        sw_dat |= 0x01;
    }
    else
    {
        sw_dat &= (~0x01);
    }
    for(i=0; i<(BSP_NXP74HC165_NUM*8-1); i++)
    {
        BSP_NXP74HC165_CLK_CLR;
        NXP74HC165_NOP;
        NXP74HC165_NOP;
        BSP_NXP74HC165_CLK_SET;
        NXP74HC165_NOP;
        NXP74HC165_NOP;
        sw_dat <<= 1;
        if(BSP_NXP74HC165_QH_R)
        {
            sw_dat |= 0x01;
        }
        else
        {
            sw_dat &= (~0x01);
        }
    }
    BSP_NXP74HC165_CLK_CLR;
    NXP74HC165_NOP;
    NXP74HC165_NOP;
    BspNXP74HC165_Value = sw_dat;
    return(sw_dat);
}
/*
******************************************************************************
* 函数功能: 100ms函数
******************************************************************************
*/
void BspNXP74HC165_100ms(void)
{
    uint32_t i32;
    uint8_t *pbuf;
    static uint8_t s_count=0;
    s_count++;
    if(s_count<10)
    {
        return;
    }
    s_count=0;
    if(BspNXP74HC165_DebugTest_Enable==1)
    {
        i32=BspNXP74HC165_read();
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char *)pbuf,"switch:0x%08lx\r\n",i32);
        DebugOutStr((int8_t*)pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
/*
******************************************************************************
* 函数功能: debug函数
******************************************************************************
*/
void BspNXP74HC165_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspNXP74HC165_DebugTest_Enable=1;
    }
    else
    {
        BspNXP74HC165_DebugTest_Enable=0;
    }
}

