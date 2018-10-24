/**
  ******************************************************************************
  * @file    Bsp_NXP74HC595.c 
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
#include "Bsp_NXP74HC595.h"
//
uint32_t BspNXP74HC595_LedBitmap=0;
static uint8_t BspNXP74HC595_DebugTest_Enable=0;
/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void BspNXP74HC595_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    // GPIO初始化---时钟
    BSP_NXP74HC595_RCC_ENABLE;
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
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC595_SER_PIN;
    GPIO_Init(BSP_NXP74HC595_SER_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC595_SCLK_PIN;
    GPIO_Init(BSP_NXP74HC595_SCLK_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_NXP74HC595_RCLK_PIN;
    GPIO_Init(BSP_NXP74HC595_RCLK_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_output(BSP_NXP74HC595_SER_PIN);
    nrf_gpio_cfg_output(BSP_NXP74HC595_SCLK_PIN);
    nrf_gpio_cfg_output(BSP_NXP74HC595_RCLK_PIN);
#endif
}
//-------------------------------------------------------------------------------
// 函数名称: void NXP74HC595Display(INT8U Byte)
// 函数功能: LED显示函数
// 调用函数: 无
// 入口参数: Byte 每1位对应1个LED控制, 置1 点亮LED; 清零 关闭LED
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
static void BspNXP74HC595_Display(uint8_t Byte)
{
    uint8_t i = 0;
    BSP_NXP74HC595_SCLK_CLR;
    BSP_NXP74HC595_RCLK_CLR;
    for(i=0; i<8; i++)
    {
        if(Byte&0x80)  BSP_NXP74HC595_SER_SET;
        else           BSP_NXP74HC595_SER_CLR;
        BSP_NXP74HC595_SCLK_SET;
        BSP_NXP74HC595_SCLK_CLR;
        Byte = Byte << 1;
    }
    BSP_NXP74HC595_RCLK_SET;
    BSP_NXP74HC595_RCLK_CLR;
}
//-------------------------------------------------------------------------------
// 函数名称: void NXP74HC595_APP(INT8U lednum,INT8U ledstate)
// 函数功能: 输出函数---HT1632应用函数
// 调用函数: 无
// 入口参数:
//             lednum:  1-8
//             ledstate:ON /OFF
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void BspNXP74HC595_APP(uint8_t lednum,uint8_t ledstate)
{
    //LED
    if(lednum>0&&lednum<=16)
    {
        if(ledstate==ON)
        {
            BspNXP74HC595_LedBitmap|=1<<(lednum-1);
        }
        else
        {
            BspNXP74HC595_LedBitmap&=~(1<<(lednum-1));
        }
    }
}
//-------------------------------------------------------------------------------
// 函数名称: void NXP74HC595_100ms(void)
// 函数功能: 输出函数---HT1632应用函数
// 调用函数: 无
// 入口参数:   refresh - ON 马上刷新显示/OFF延时刷新显示
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------

void BspNXP74HC595_100ms(void)
{
    uint8_t *pbuf;
    static uint8_t s_count=0;
    s_count++;
    if(s_count<10)
    {
        return;
    }
    s_count=0;
    //
    BspNXP74HC595_Display(BspNXP74HC595_LedBitmap);
    if(BspNXP74HC165_DebugTest_Enable==1)
    {
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        sprintf((char *)pbuf,"Nxp74HC595 LED!\r\n");
        DebugOutStr((int8_t*)pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
//-------------------------------------------------------------------------------
// 函数名称: void NXP74HC595_Test_100ms(void)
// 函数功能: HT1632测试
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void BspNXP74HC595_DebugTestOnOff(uint8_t OnOff)
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
//---------------------END-------------------------------------------------------

