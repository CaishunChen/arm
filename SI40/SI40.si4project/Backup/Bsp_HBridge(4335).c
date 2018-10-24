/**
  ******************************************************************************
  * @file    Bsp_HBridge.c
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
#include "Bsp_HBridge.h"
#include "Bsp_Led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if   (defined(STM32F1)||defined(STM32F4))
static GPIO_TypeDef* HBRIDGE_P_GPIOX_BUF[HBRIDGE_MAX_NUM]= {HBRIDGE_1_P_GPIOX,HBRIDGE_2_P_GPIOX,HBRIDGE_3_P_GPIOX,HBRIDGE_4_P_GPIOX};
static GPIO_TypeDef* HBRIDGE_N_GPIOX_BUF[HBRIDGE_MAX_NUM]= {HBRIDGE_1_N_GPIOX,HBRIDGE_2_N_GPIOX,HBRIDGE_3_N_GPIOX,HBRIDGE_4_N_GPIOX};
static const uint16_t HBRIDGE_P_PINX_BUF[HBRIDGE_MAX_NUM]= {HBRIDGE_1_P_PINX,HBRIDGE_2_P_PINX,HBRIDGE_3_P_PINX,HBRIDGE_4_P_PINX};
static const uint16_t HBRIDGE_N_PINX_BUF[HBRIDGE_MAX_NUM]= {HBRIDGE_1_N_PINX,HBRIDGE_2_N_PINX,HBRIDGE_3_N_PINX,HBRIDGE_4_N_PINX};
#elif(defined(NRF52)||defined(NRF51))
static uint8_t  HBRIDGE_P_PINX_BUF[HBRIDGE_MAX_NUM]=
{
#ifdef  HBRIDGE_1_P_PINX
    HBRIDGE_1_P_PINX
#endif
#ifdef  HBRIDGE_2_P_PINX
    ,HBRIDGE_2_P_PINX
#endif
#ifdef  HBRIDGE_3_P_PINX
    ,HBRIDGE_3_P_PINX
#endif
#ifdef  HBRIDGE_4_P_PINX
    ,HBRIDGE_4_P_PINX
#endif
};
static uint8_t  HBRIDGE_N_PINX_BUF[HBRIDGE_MAX_NUM]=
{
#ifdef  HBRIDGE_1_N_PINX
    HBRIDGE_1_N_PINX
#endif
#ifdef  HBRIDGE_2_N_PINX
    ,HBRIDGE_2_N_PINX
#endif
#ifdef  HBRIDGE_3_N_PINX
    ,HBRIDGE_3_N_PINX
#endif
#ifdef  HBRIDGE_4_N_PINX
    ,HBRIDGE_4_N_PINX
#endif

};
#endif

long BspHBridge_1msTimerBuf[HBRIDGE_MAX_NUM];
static uint8_t BspHBridge_DebugTest_Enable=0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void BspHBridge_Test_1ms(void);
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   H桥初始化
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspHBridge_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟初始化
    BSP_HBRIDGE_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
#endif
#ifdef  HBRIDGE_1_P_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_1_P_PINX ;
    GPIO_Init(HBRIDGE_1_P_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_1_N_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_1_N_PINX ;
    GPIO_Init(HBRIDGE_1_N_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_2_P_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_2_P_PINX ;
    GPIO_Init(HBRIDGE_2_P_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_2_N_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_2_N_PINX ;
    GPIO_Init(HBRIDGE_2_N_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_3_P_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_3_P_PINX ;
    GPIO_Init(HBRIDGE_3_P_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_3_N_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_3_N_PINX ;
    GPIO_Init(HBRIDGE_3_N_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_4_P_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_4_P_PINX ;
    GPIO_Init(HBRIDGE_4_P_GPIOX, &GPIO_InitStructure);
#endif
#ifdef  HBRIDGE_4_N_PINX
    GPIO_InitStructure.GPIO_Pin   = HBRIDGE_4_N_PINX ;
    GPIO_Init(HBRIDGE_4_N_GPIOX, &GPIO_InitStructure);
#endif
#elif(defined(NRF52)||defined(NRF51))
#ifdef  HBRIDGE_1_P_PINX
    nrf_gpio_cfg_output(HBRIDGE_1_P_PINX);
#endif
#ifdef  HBRIDGE_1_N_PINX
    nrf_gpio_cfg_output(HBRIDGE_1_N_PINX);
#endif
#ifdef  HBRIDGE_2_P_PINX
    nrf_gpio_cfg_output(HBRIDGE_2_P_PINX);
#endif
#ifdef  HBRIDGE_2_N_PINX
    nrf_gpio_cfg_output(HBRIDGE_2_N_PINX);
#endif
#ifdef  HBRIDGE_3_P_PINX
    nrf_gpio_cfg_output(HBRIDGE_3_P_PINX);
#endif
#ifdef  HBRIDGE_3_N_PINX
    nrf_gpio_cfg_output(HBRIDGE_3_N_PINX);
#endif
#ifdef  HBRIDGE_4_P_PINX
    nrf_gpio_cfg_output(HBRIDGE_4_P_PINX);
#endif
#ifdef  HBRIDGE_4_N_PINX
    nrf_gpio_cfg_output(HBRIDGE_4_N_PINX);
#endif
#endif
}
/**
 * @brief   周期处理函数
 * @note    放在1ms循环中执行
 * @param   None
 * @return  None
 */

void BspHBridge_1ms(void)
{
    uint8_t i;
    for(i=0; i<HBRIDGE_MAX_NUM; i++)
    {
        if(BspHBridge_1msTimerBuf[i]==0)
        {
            //停止
#if   (defined(STM32F1)||defined(STM32F4))
            GPIO_ResetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
            GPIO_ResetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
#elif(defined(NRF52)||defined(NRF51))
            nrf_gpio_pin_write(HBRIDGE_P_PINX_BUF[i],0);
            nrf_gpio_pin_write(HBRIDGE_N_PINX_BUF[i],0);

#endif
        }
        else if(BspHBridge_1msTimerBuf[i]>0)
        {
            BspHBridge_1msTimerBuf[i]--;
            //正转
#if   (defined(STM32F1)||defined(STM32F4))
            GPIO_ResetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
            GPIO_SetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
#elif(defined(NRF52)||defined(NRF51))
            nrf_gpio_pin_write(HBRIDGE_P_PINX_BUF[i],0);
            nrf_gpio_pin_write(HBRIDGE_N_PINX_BUF[i],1);

#endif
        }
        else
        {
            BspHBridge_1msTimerBuf[i]++;
            //反转
#if   (defined(STM32F1)||defined(STM32F4))
            GPIO_SetBits(HBRIDGE_P_GPIOX_BUF[i] , HBRIDGE_P_PINX_BUF[i]);
            GPIO_ResetBits(HBRIDGE_N_GPIOX_BUF[i] , HBRIDGE_N_PINX_BUF[i]);
#elif(defined(NRF52)||defined(NRF51))
            nrf_gpio_pin_write(HBRIDGE_P_PINX_BUF[i],1);
            nrf_gpio_pin_write(HBRIDGE_N_PINX_BUF[i],0);

#endif
        }
    }
    if(BspHBridge_DebugTest_Enable==1)
    {
        static uint16_t stimer=0;
        if(stimer==2500)
        {
            //正转
            BspHBridge_1msTimerBuf[0]=1000;
            BspHBridge_1msTimerBuf[1]=1000;
            BspHBridge_1msTimerBuf[2]=1000;
            BspHBridge_1msTimerBuf[3]=1000;
            //开启前轮灯
            BSP_LED_LED2_ON;
            //关闭后轮灯
            BSP_LED_LED3_OFF;
        }
        else if(stimer==5000)
        {
            //反转
            BspHBridge_1msTimerBuf[0]=-1000;
            BspHBridge_1msTimerBuf[1]=-1000;
            BspHBridge_1msTimerBuf[2]=-1000;
            BspHBridge_1msTimerBuf[3]=-1000;
            //开启后轮灯
            BSP_LED_LED3_ON;
            //关闭前轮灯
            BSP_LED_LED2_OFF;
        }
        else if(stimer>5000)
        {
            stimer=0;
        }
    }
}
//--------------------------------------------------------------测试函数
static void BspHBridge_Test_1ms(void)
{

    stimer++;
#if (defined(PROJECT_TCI_V30))
    if(stimer==2500)
    {
        //正转
        BspHBridge_1msTimerBuf[0]=1000;
        BspHBridge_1msTimerBuf[1]=1000;
        BspHBridge_1msTimerBuf[2]=1000;
        BspHBridge_1msTimerBuf[3]=1000;
        //开启前轮灯
        BSP_LED_LED2_ON;
        //关闭后轮灯
        BSP_LED_LED3_OFF;
    }
    else if(stimer==5000)
    {
        //反转
        BspHBridge_1msTimerBuf[0]=-1000;
        BspHBridge_1msTimerBuf[1]=-1000;
        BspHBridge_1msTimerBuf[2]=-1000;
        BspHBridge_1msTimerBuf[3]=-1000;
        //开启后轮灯
        BSP_LED_LED3_ON;
        //关闭前轮灯
        BSP_LED_LED2_OFF;
    }
    else if(stimer>5000)
    {
        stimer=0;
    }
#elif (defined(TEST_NRF52_V1))
    if(stimer==2500)
    {
        BspHBridge_1msTimerBuf[0]=1000;
    }
    else if(stimer==5000)
    {
        BspHBridge_1msTimerBuf[0]=-1000;
    }
    else if(stimer>5000)
    {
        stimer=0;
    }
#endif
}
/**
 * @brief   Debug接口函数
 * @note    开启Debug相关指令后会循环点亮，关闭Debug后所有LED全部关闭
 * @param   OnOff-判断指令进入与退出
 * @return  None
 */
void BspLed_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspHBridge_DebugTest_Enable=1;
    }
    else
    {
        BspHBridge_DebugTest_Enable=0;
    }
}

//------------------------------------------------------------

