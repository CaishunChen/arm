/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes --------------------
#include "Bsp_Key.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
//-------------------------------------------------------------Private define
//------------------------------- 用户变量 --------------------
//输出方式一
uint8_t BspKey_ValueBuf[BSP_KEY_E_MAX]= {0};
//输出方式二
uint8_t BspKey_Value=0;
uint8_t BspKey_NewSign=0;
uint16_t BspKey_KeepTimer_ms=0;
//输出方式三
uint8_t BspKey_RealValueBuf[BSP_KEY_E_MAX]= {0};
//输出方式四(按键计时,按下后计数器大于等于1则累加,应用程序负责清零,松开按键负责置1)
uint16_t BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_MAX]= {0};
//输出信息
uint16_t BspKey_Count=0;
//
static uint8_t Key_DebugTest_Enable=0;
//------------------------------- 用户函数声明 ----------------
#if   (defined(BSP_KEY_ENABLE_NRF_DRIVER))
/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void bsp_event_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            BspKey_NewSign =  1;
            BspKey_Value   =  BSP_KEY_E_KEY1;
            BspKey_KeepTimer_ms=200;
            break;
        case BSP_EVENT_KEY_1:
#if (BSP_KEY_E_MAX>=BSP_KEY_E_KEY2)
            BspKey_NewSign =  1;
            BspKey_Value   =  BSP_KEY_E_KEY2;
            BspKey_KeepTimer_ms=200;
#endif
            break;
        case BSP_EVENT_KEY_2:
#if (BSP_KEY_E_MAX>=BSP_KEY_E_KEY3)
            BspKey_NewSign =  1;
            BspKey_Value   =  BSP_KEY_E_KEY3;
            BspKey_KeepTimer_ms=200;
#endif
            break;
        case BSP_EVENT_KEY_3:
#if (BSP_KEY_E_MAX>=BSP_KEY_E_KEY4)
            BspKey_NewSign =  1;
            BspKey_Value   =  BSP_KEY_E_KEY4;
            BspKey_KeepTimer_ms=200;
#endif
            break;
        case BSP_EVENT_KEY_4:
#if (BSP_KEY_E_MAX>=BSP_KEY_E_KEY5)
			BspKey_NewSign =  1;
			BspKey_Value   =  BSP_KEY_E_KEY5;
			BspKey_KeepTimer_ms=200;
#endif
            break;
        case BSP_EVENT_KEY_5:
            break;
        case BSP_EVENT_KEY_6:
            break;
        case BSP_EVENT_KEY_7:
            break;
        case BSP_EVENT_DEFAULT:
            break;
        case BSP_EVENT_CLEAR_BONDING_DATA:
            break;
        case BSP_EVENT_CLEAR_ALERT:
            break;
        case BSP_EVENT_DISCONNECT:
            break;
        case BSP_EVENT_ADVERTISING_START:
            break;
        case BSP_EVENT_ADVERTISING_STOP:
            break;
        case BSP_EVENT_WHITELIST_OFF:
            break;
        case BSP_EVENT_BOND:
            break;
        case BSP_EVENT_RESET:
            break;
        case BSP_EVENT_SLEEP:
            //sleep_mode_enter();
            break;
        case BSP_EVENT_WAKEUP:
            break;
        case BSP_EVENT_SYSOFF:
            break;
        case BSP_EVENT_DFU:
            break;
        default:
            break;
    }
}
#endif
//-------------------------------------------------------------------------------
// 函数功能: GPIO口配置
//-------------------------------------------------------------------------------
void BspKey_Init(void)
{
#if   (defined(BSP_KEY_ENABLE_NRF_DRIVER))
    uint32_t err_code = bsp_init(BSP_INIT_BUTTONS,
                        APP_TIMER_TICKS(100, APP_TIMER_PRESCALER),
                        bsp_event_handler);
    APP_ERROR_CHECK(err_code);
#else
#if   (defined(STM32F1))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_KEY_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
#elif (defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_KEY_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    //GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
    //
#ifdef   BSP_KEY1_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY1_PIN;
    GPIO_Init(BSP_KEY1_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY1_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY2_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY2_PIN;
    GPIO_Init(BSP_KEY2_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY2_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY3_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY3_PIN;
    GPIO_Init(BSP_KEY3_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY3_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY4_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY4_PIN;
    GPIO_Init(BSP_KEY4_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY4_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY5_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY5_PIN;
    GPIO_Init(BSP_KEY5_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY5_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY6_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY6_PIN;
    GPIO_Init(BSP_KEY6_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY6_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY7_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY7_PIN;
    GPIO_Init(BSP_KEY7_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY7_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY8_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY8_PIN;
    GPIO_Init(BSP_KEY8_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY8_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY9_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY9_PIN;
    GPIO_Init(BSP_KEY9_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY9_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY10_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY10_PIN;
    GPIO_Init(BSP_KEY10_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY10_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY11_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY11_PIN;
    GPIO_Init(BSP_KEY11_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY11_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY12_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY12_PIN;
    GPIO_Init(BSP_KEY12_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY12_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY13_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY13_PIN;
    GPIO_Init(BSP_KEY13_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY13_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY14_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY14_PIN;
    GPIO_Init(BSP_KEY14_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY14_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY15_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY15_PIN;
    GPIO_Init(BSP_KEY15_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY15_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY16_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY16_PIN;
    GPIO_Init(BSP_KEY16_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY16_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY17_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY17_PIN;
    GPIO_Init(BSP_KEY17_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY17_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY18_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY18_PIN;
    GPIO_Init(BSP_KEY18_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY18_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY19_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY19_PIN;
    GPIO_Init(BSP_KEY19_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY19_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#ifdef   BSP_KEY20_PORT
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitStructure.GPIO_Pin     =  BSP_KEY20_PIN;
    GPIO_Init(BSP_KEY20_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_input(BSP_KEY20_PORT, NRF_GPIO_PIN_PULLUP);
#endif
#endif
#if   (defined(NRF51)||defined(NRF52))
    //Bsp_Key_TimerInit();
#endif
#ifdef   BSP_KEY_AD_ENABLE
    ADC_Configuration();
#endif
    BspKey_Main_10ms();
#endif
}

//-------------------------------------------------------------------------------
// 函数功能: 测试
//-------------------------------------------------------------------------------
void Key_Main_DebugTest_100ms(void)
{
    char *pbuf;
    static uint8_t s_count=0;
    uint32_t i32=0;
    s_count++;
    if(s_count<5)
    {
        return;
    }
    s_count=0;
    if(Key_DebugTest_Enable==0)
    {
        return;
    }
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //打印输出
    for(pbuf[0]=0; pbuf[0]<BSP_KEY_E_MAX; pbuf[0]++)
    {
        i32<<=1;
        i32+=BspKey_RealValueBuf[pbuf[0]];
    }
    sprintf(pbuf,"DebugOut: Count-%d,Key=%08lx\r\n",BspKey_Count,i32);
    DebugOutStr((int8_t*)pbuf);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
//-------------------------------------------------------------------------------
// 函数功能: 接口函数---开关量输入主处理函数,调用于100ms定时程序
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void BspKey_Main_1ms(void)
{
    static uint8_t si=0;
    si++;
    if(si!=10)
    {
        return;
    }
    si=0;
    BspKey_Main_10ms();
}
void BspKey_Main_10ms(void)
{
    uint8_t newkeyval;
    static uint8_t KeyFlag=0;
    static uint8_t KeyVal=0;
    static uint8_t KeyCon=0;
    static uint8_t NewKeyBZ=0;
    static uint8_t Key_Value=0;
    //if(Key_10ms_timer < 10) return;
    //Key_10ms_timer = 0;
    //测试
    //AD方式
#ifdef   BSP_KEY_AD_ENABLE
    {
        static uint8_t s_adkey_step=0;
        static uint8_t si=0;
        uint16_t i16;
        AD_ReadValue(ON,5,&i16);
        switch(s_adkey_step)
        {
            case 0:
#ifdef   BSP_KEY_AD_THR_1
                if(((BSP_KEY_AD_THR_1-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_1+BSP_KEY_AD_THR_RANGE)))
                {
                    si=1;
                }
#endif
#ifdef   BSP_KEY_AD_THR_2
                if(((BSP_KEY_AD_THR_2-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_2+BSP_KEY_AD_THR_RANGE)))
                {
                    si=2;
                }
#endif
#ifdef   BSP_KEY_AD_THR_3
                if(((BSP_KEY_AD_THR_3-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_3+BSP_KEY_AD_THR_RANGE)))
                {
                    si=3;
                }
#endif
#ifdef   BSP_KEY_AD_THR_4
                if(((BSP_KEY_AD_THR_4-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_4+BSP_KEY_AD_THR_RANGE)))
                {
                    si=4;
                }
#endif
#ifdef   BSP_KEY_AD_THR_5
                if(((BSP_KEY_AD_THR_5-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_5+BSP_KEY_AD_THR_RANGE)))
                {
                    si=5;
                }
#endif
#ifdef   BSP_KEY_AD_THR_2A4
                if(((BSP_KEY_AD_THR_2A4-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_2A4+BSP_KEY_AD_THR_RANGE)))
                {
                    si=24;
                }
#endif
                if(si!=0)
                {
                    s_adkey_step++;
                }
                break;
            case 1:
            case 2:
#ifdef   BSP_KEY_AD_THR_1
                if(si==1)
                {
                    if(((BSP_KEY_AD_THR_1-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_1+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
#ifdef   BSP_KEY_AD_THR_2
                if(si==2)
                {
                    if(((BSP_KEY_AD_THR_2-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_2+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
#ifdef   BSP_KEY_AD_THR_3
                if(si==3)
                {
                    if(((BSP_KEY_AD_THR_3-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_3+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
#ifdef   BSP_KEY_AD_THR_4
                if(si==4)
                {
                    if(((BSP_KEY_AD_THR_4-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_4+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
#ifdef   BSP_KEY_AD_THR_5
                if(si==5)
                {
                    if(((BSP_KEY_AD_THR_5-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_5+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
#ifdef   BSP_KEY_AD_THR_2A4
                if(si==24)
                {
                    if(((BSP_KEY_AD_THR_2A4-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_2A4+BSP_KEY_AD_THR_RANGE)))
                    {
                        s_adkey_step++;
                    }
                    else
                    {
                        si=0;
                        s_adkey_step=0;
                    }
                }
#endif
                break;
            // 初始化计时器
            case 3:
                BspKey_KeepTimer_ms=s_adkey_step*10;
                s_adkey_step++;
                break;
            // 等待按键放开
            case 4:
                //BspKey_KeepTimer_ms+=10;
#ifdef   BSP_KEY_AD_THR_2A4
                if(BspKey_KeepTimer_ms<100)
                {
                    if(((BSP_KEY_AD_THR_2A4-BSP_KEY_AD_THR_RANGE)<=i16)&&(i16<=(BSP_KEY_AD_THR_2A4+BSP_KEY_AD_THR_RANGE)))
                    {
                        si  =   24;
                        BspKey_KeepTimer_ms+=10;
                    }
                }
#endif
                if(i16>=(BSP_KEY_AD_THR_NULL-BSP_KEY_AD_THR_RANGE))
                {
                    s_adkey_step++;
                }
                break;
            case 5:
                BspKey_Value    = si;
                BspKey_NewSign  =  1;
                s_adkey_step = 0;
                break;
            default:
                break;
        }
    }
#endif
    //输出方式三
#ifdef   BSP_KEY1_PORT
    if      (BSP_KEY1_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY1-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY1-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY1-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY1-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY1-1]=0;
    }
#endif
#ifdef   BSP_KEY2_PORT
    if      (BSP_KEY2_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY2-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY2-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY2-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY2-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY2-1]=0;
    }
#endif
#ifdef   BSP_KEY3_PORT
    if      (BSP_KEY3_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY3-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY3-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY3-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY3-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY3-1]=0;
    }
#endif
#ifdef   BSP_KEY4_PORT
    if      (BSP_KEY4_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY4-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY4-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY4-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY4-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY4-1]=0;
    }
#endif
#ifdef   BSP_KEY5_PORT
    if      (BSP_KEY5_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY5-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY5-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY5-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY5-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY5-1]=0;
    }
#endif
#ifdef   BSP_KEY6_PORT
    if      (BSP_KEY6_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY6-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY6-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY6-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY6-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY6-1]=0;
    }
#endif
#ifdef   BSP_KEY7_PORT
    if      (BSP_KEY7_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY7-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY7-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY7-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY7-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY7-1]=0;
    }
#endif
#ifdef   BSP_KEY8_PORT
    if      (BSP_KEY8_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY8-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY8-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY8-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY8-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY8-1]=0;
    }
#endif
#ifdef   BSP_KEY9_PORT
    if      (BSP_KEY9_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY9-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY9-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY9-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY9-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY9-1]=0;
    }
#endif
#ifdef   BSP_KEY10_PORT
    if      (BSP_KEY10_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY10-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY10-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY10-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY10-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY10-1]=0;
    }
#endif
#ifdef   BSP_KEY11_PORT
    if      (BSP_KEY11_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY11-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY11-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY11-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY11-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY11-1]=0;
    }
#endif
#ifdef   BSP_KEY12_PORT
    if      (BSP_KEY12_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY12-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY12-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY12-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY12-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY12-1]=0;
    }
#endif
#ifdef   BSP_KEY13_PORT
    if      (BSP_KEY13_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY13-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY13-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY13-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY13-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY13-1]=0;
    }
#endif
#ifdef   BSP_KEY14_PORT
    if      (BSP_KEY14_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY14-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY14-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY14-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY14-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY14-1]=0;
    }
#endif
#ifdef   BSP_KEY15_PORT
    if      (BSP_KEY15_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY15-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY15-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY15-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY15-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY15-1]=0;
    }
#endif
#ifdef   BSP_KEY16_PORT
    if      (BSP_KEY16_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY16-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY16-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY16-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY16-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY16-1]=0;
    }
#endif
#ifdef   BSP_KEY17_PORT
    if      (BSP_KEY17_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY17-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY17-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY17-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY17-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY17-1]=0;
    }
#endif
#ifdef   BSP_KEY18_PORT
    if      (BSP_KEY18_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY18-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY18-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY18-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY18-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY18-1]=0;
    }
#endif
#ifdef   BSP_KEY19_PORT
    if      (BSP_KEY19_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY19-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY19-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY19-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY19-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY19-1]=0;
    }
#endif
#ifdef   BSP_KEY20_PORT
    if      (BSP_KEY20_PIN_R ==  0)
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY20-1]=1;
        if(BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY20-1]!=0xFFFF)
        {
            BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY20-1]++;
        }
    }
    else
    {
        BspKey_RealValueBuf[BSP_KEY_E_KEY20-1]=0;
        BspKey_PressKeepTimerBuf_10ms[BSP_KEY_E_KEY20-1]=0;
    }
#endif
    //输出方式一二
#ifdef   BSP_KEY1_PORT
    if      (BSP_KEY1_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY1;
#endif
#ifdef   BSP_KEY2_PORT
    else if (BSP_KEY2_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY2;
#endif
#ifdef   BSP_KEY3_PORT
    else if (BSP_KEY3_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY3;
#endif
#ifdef   BSP_KEY4_PORT
    else if (BSP_KEY4_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY4;
#endif
#ifdef   BSP_KEY5_PORT
    else if (BSP_KEY5_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY5;
#endif
#ifdef   BSP_KEY6_PORT
    else if (BSP_KEY6_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY6;
#endif
#ifdef   BSP_KEY7_PORT
    else if (BSP_KEY7_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY7;
#endif
#ifdef   BSP_KEY8_PORT
    else if (BSP_KEY8_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY8;
#endif
#ifdef   BSP_KEY9_PORT
    else if (BSP_KEY9_PIN_R ==  0)  newkeyval = BSP_KEY_E_KEY9;
#endif
#ifdef   BSP_KEY10_PORT
    else if (BSP_KEY10_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY10;
#endif
#ifdef   BSP_KEY11_PORT
    else if (BSP_KEY11_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY11;
#endif
#ifdef   BSP_KEY12_PORT
    else if (BSP_KEY12_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY12;
#endif
#ifdef   BSP_KEY13_PORT
    else if (BSP_KEY13_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY13;
#endif
#ifdef   BSP_KEY14_PORT
    else if (BSP_KEY14_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY14;
#endif
#ifdef   BSP_KEY15_PORT
    else if (BSP_KEY15_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY15;
#endif
#ifdef   BSP_KEY16_PORT
    else if (BSP_KEY16_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY16;
#endif
#ifdef   BSP_KEY17_PORT
    else if (BSP_KEY17_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY17;
#endif
#ifdef   BSP_KEY18_PORT
    else if (BSP_KEY18_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY18;
#endif
#ifdef   BSP_KEY19_PORT
    else if (BSP_KEY19_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY19;
#endif
#ifdef   BSP_KEY20_PORT
    else if (BSP_KEY20_PIN_R ==  0) newkeyval = BSP_KEY_E_KEY20;
#endif
    else                            newkeyval = BSP_KEY_E_NC;

    switch (KeyFlag)
    {
        case 0:
            if (newkeyval != BSP_KEY_E_NC)
            {
                KeyVal = newkeyval;
                KeyCon = 0;
                KeyFlag = 1;
            }
            break;
        case 1:
            if (++KeyCon > 2)
            {
                if(KeyVal == newkeyval)
                {
                    KeyFlag   = 2;
                    NewKeyBZ  = TRUE;
                    Key_Value = KeyVal;
                    //Lcd_light_timer=0;//启动LCD背光
                }
                else KeyVal = KeyFlag = 0;
            }
            else
            {
                if(KeyVal != newkeyval)
                {
                    KeyVal = KeyFlag = 0;
                }
            }
            break;
        case 2://等待新见处理完毕
            //if(!NewKeyBZ)
        {
            BspKey_KeepTimer_ms=0;
            KeyFlag = 3;
        }
        break;
        case 3://等待见放开
            if(newkeyval == BSP_KEY_E_NC)
            {
                KeyFlag = 4;
                KeyCon = 0;
            }
            else if(newkeyval == KeyVal)
            {
                BspKey_KeepTimer_ms+=10;
            }
            else
            {
                KeyVal = KeyFlag = 0;
            }
            break;
        case 4://放键去抖
            if(++KeyCon > 5)
            {
                if(newkeyval != 0)  KeyCon = 0;
                else                KeyFlag = KeyCon = KeyVal = 0;
            }
            break;
        default :
            KeyFlag = KeyCon = KeyVal = 0;
            break;
    }
    if(NewKeyBZ==1&&KeyFlag==0)
    {
        NewKeyBZ=0;
        BspKey_ValueBuf[Key_Value-1]=1;
        //
        BspKey_Value    = Key_Value;
        BspKey_NewSign  =  1;
        if(BspKey_Count!=0xFFFF)
        {
            BspKey_Count++;
        }
        else
        {
            BspKey_Count=0;
        }
    }
}
//
static uint16_t BspKey_CountBak=0;
uint8_t BspKey_IfPressKey(void)
{

    if(BspKey_CountBak!=BspKey_Count)
    {
        BspKey_CountBak=BspKey_Count;
        return ON;
    }
    return OFF;
}
void BspKey_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Key_DebugTest_Enable=1;
    }
    else
    {
        Key_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
