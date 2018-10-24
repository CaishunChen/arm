/**
  ******************************************************************************
  * @file    Bsp_Dht11.c
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
/* Includes ------------------------------------------------------------------*/
#include "Bsp_Dht11.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSP_DHTXX_S_INFO BspDhtXX_s_Info;
static uint8_t BspDhtXX_DebugTest_Enable=0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   初始化函数
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspDhtXX_Init(void)
{
    // GPIO初始化
#if     (defined(STM32F1))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_GPIO_IIC_RCC_ENABLE;
    if(first==0)return OK;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SDA_PIN;
    GPIO_Init(BSP_GPIO_IIC_SDA_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SCL_PIN;
    GPIO_Init(BSP_GPIO_IIC_SCL_PORT, &GPIO_InitStructure);
#elif   (defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_GPIO_IIC_RCC_ENABLE;
    if(first==0)return OK;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SDA_PIN;
    GPIO_Init(BSP_GPIO_IIC_SDA_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_GPIO_IIC_SCL_PIN;
    GPIO_Init(BSP_GPIO_IIC_SCL_PORT, &GPIO_InitStructure);

#elif   (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_output(BSP_DHTXX_PIN);
#endif
    BSP_TWI_DQ_H;
    // 变量初始化
    memset((char*)&BspDhtXX_s_Info,0,sizeof(BspDhtXX_s_Info));
    // 应用初始化
}
void BspDhtXX_100ms(void)
{}
void BspDhtXX_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspDhtXX_DebugTest_Enable=1;
    }
    else
    {
        BspDhtXX_DebugTest_Enable=0;
    }
}

/******************* (C) COPYRIGHT 2011 XSLXHN *****END OF FILE****/

