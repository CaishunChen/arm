/**
  ******************************************************************************
  * @file    Bsp_BuzzerMusic.c 
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
#define BSP_BUZZER_MUSIC_GLOBAL
#include "Bsp_BuzzerMusic.h"
#include "Bsp_Key.h"

#if (defined(NRF51)||defined(NRF52))
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
static nrf_drv_timer_t timer = NRF_DRV_TIMER_INSTANCE(1);
void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context) {}
#endif

//------------------------------- Define -------------------------------------
//音阶
enum BSP_BUZZER_MUSIC_E_SCALE
{
    //定义低音音阶(数值单位:Hz)
    BSP_BUZZER_MUSIC_E_SCALE_L1 = 262,
    BSP_BUZZER_MUSIC_E_SCALE_L2 = 294,
    BSP_BUZZER_MUSIC_E_SCALE_L3 = 330,
    BSP_BUZZER_MUSIC_E_SCALE_L4 = 349,
    BSP_BUZZER_MUSIC_E_SCALE_L5 = 392,
    BSP_BUZZER_MUSIC_E_SCALE_L6 = 440,
    BSP_BUZZER_MUSIC_E_SCALE_L7 = 494,
    //定义中音音阶
    BSP_BUZZER_MUSIC_E_SCALE_M1 = 523,
    BSP_BUZZER_MUSIC_E_SCALE_M2 = 587,
    BSP_BUZZER_MUSIC_E_SCALE_M3 = 659,
    BSP_BUZZER_MUSIC_E_SCALE_M4 = 698,
    BSP_BUZZER_MUSIC_E_SCALE_M5 = 784,
    BSP_BUZZER_MUSIC_E_SCALE_M6 = 880,
    BSP_BUZZER_MUSIC_E_SCALE_M7 = 988,
    //定义高音音阶
    BSP_BUZZER_MUSIC_E_SCALE_H1 = 1047,
    BSP_BUZZER_MUSIC_E_SCALE_H2 = 1175,
    BSP_BUZZER_MUSIC_E_SCALE_H3 = 1319,
    BSP_BUZZER_MUSIC_E_SCALE_H4 = 1397,
    BSP_BUZZER_MUSIC_E_SCALE_H5 = 1568,
    BSP_BUZZER_MUSIC_E_SCALE_H6 = 1760,
    BSP_BUZZER_MUSIC_E_SCALE_H7 = 1976,
};
//------------------------------- 静态变量 -----------------------------------
static const uint8_t *p_BspBuzzerMusic_Lib=NULL;
static uint8_t BspBuzzerMusic_cmd=0;         // ON-播放 OFF-停止
static uint16_t BspBuzzerMusic_MaxTimeS=0;   // 最大播放时长
static uint32_t BspBuzzerMusiz_CurrentTimeS  =  0; //当前播放时长
//------------------------------- 静态函数 -----------------------------------
static void Bsp_BuzzerMusic_PwmOutWrite(uint32_t freq);
/*******************************************************************************
* 函数功能: 定时器TIM4 PWM模块初始化
* 说    明: 无
*******************************************************************************/
static void Bsp_BuzzerMusic_TimPwm_OnOff(uint8_t OnOff)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef  GPIO_InitStructure;
    if(OnOff==ON)
    {
        //初始化GPIO时钟
        BSP_BUZZER_MUSIC_GPIO_RCC_ENABLE;
        //重映射
        BSP_BUZZER_MUSIC_GPIO_REMAP;
        //GPIO初始化
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
        //GPIO_InitStructure.GPIO_OType  =  GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_UP;
        //GPIO_InitStructure.GPIO_PuPd   =  GPIO_PuPd_NOPULL;
#endif
        GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Pin    = BSP_BUZZER_MUSIC_GPIO_TX_PIN;
        GPIO_Init(BSP_BUZZER_MUSIC_GPIO_TX_PORT,&GPIO_InitStructure);
        //定时器初始化
        BSP_BUZZER_MUSIC_TIM_RCC_ENABLE;
        TIM_ARRPreloadConfig(BSP_BUZZER_MUSIC_TIMX, ENABLE);                          //使能TIM4重载寄存器ARR
        TIM_Cmd(BSP_BUZZER_MUSIC_TIMX, ENABLE);                                       //使能定时器4，1-使能；0-不使能
        TIM_CtrlPWMOutputs(BSP_BUZZER_MUSIC_TIMX, ENABLE);
        Bsp_BuzzerMusic_PwmOutWrite(0);
    }
    else
    {
        //GPIO初始化
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
        //GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
        //GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
#endif
        GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Pin     = BSP_BUZZER_MUSIC_GPIO_TX_PIN;
        GPIO_Init(BSP_BUZZER_MUSIC_GPIO_TX_PORT,&GPIO_InitStructure);
        GPIO_SetBits(BSP_BUZZER_MUSIC_GPIO_TX_PORT , BSP_BUZZER_MUSIC_GPIO_TX_PIN);
        //关闭定时器
        BSP_BUZZER_MUSIC_TIM_RCC_DISABLE;
        TIM_Cmd(BSP_BUZZER_MUSIC_TIMX, DISABLE);
        TIM_CtrlPWMOutputs(BSP_BUZZER_MUSIC_TIMX, DISABLE);
    }
#elif (defined(NRF51)||defined(NRF52))
    static uint8_t first=1;
    ret_code_t err_code;
    if(first==1)
    {
        first=0;
        err_code = nrf_drv_ppi_init();
        APP_ERROR_CHECK(err_code);

        err_code = nrf_drv_gpiote_init();
        APP_ERROR_CHECK(err_code);

        nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
        err_code = nrf_drv_timer_init(&timer, &timer_cfg, timer_dummy_handler);
        APP_ERROR_CHECK(err_code);
#ifdef NRF51
        //Workaround for PAN-73.
        *(uint32_t *)0x40008C0C = 1;
#endif
#ifdef NRF52
        //Workaround for PAN-73.
        *(uint32_t *)0x40008C0C = 1;
#endif			
        {
            uint32_t compare_evt_addr;
            uint32_t gpiote_task_addr;
            nrf_ppi_channel_t ppi_channel;
            ret_code_t err_code;
            nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(true);

            err_code = nrf_drv_gpiote_out_init(BSP_BUZZER_MUSIC_GPIO_TX_PIN, &config);
            APP_ERROR_CHECK(err_code);


            nrf_drv_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 200 * 1000UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

            err_code = nrf_drv_ppi_channel_alloc(&ppi_channel);
            APP_ERROR_CHECK(err_code);

            compare_evt_addr = nrf_drv_timer_event_address_get(&timer, NRF_TIMER_EVENT_COMPARE0);
            gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(BSP_BUZZER_MUSIC_GPIO_TX_PIN);

            err_code = nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);
            APP_ERROR_CHECK(err_code);

            err_code = nrf_drv_ppi_channel_enable(ppi_channel);
            APP_ERROR_CHECK(err_code);

            nrf_drv_gpiote_out_task_enable(BSP_BUZZER_MUSIC_GPIO_TX_PIN);
        }
    }
    if(OnOff==ON)
    {
        nrf_drv_timer_enable(&timer);
    }
    else
    {
        nrf_drv_timer_disable(&timer);
        nrf_drv_gpiote_out_set(BSP_BUZZER_MUSIC_GPIO_TX_PIN);
    }
#endif
}
/*******************************************************************************
* 函数功能: PWM模块频率修改
* 说    明: 无
*******************************************************************************/
static void Bsp_BuzzerMusic_PwmOutWrite(uint32_t freq)
{
#if   (defined(STM32F1)||defined(STM32F4))
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //设置频率
    TIM_TimeBaseStructure.TIM_Period        = ((SystemCoreClock/1000)/freq)-1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 499;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(BSP_BUZZER_MUSIC_TIMX, &TIM_TimeBaseStructure);
    //设置占空比
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = (((SystemCoreClock/1000)/freq)-1)/2;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;
    TIM_OC1Init(BSP_BUZZER_MUSIC_TIMX, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(BSP_BUZZER_MUSIC_TIMX, TIM_OCPreload_Enable);
#elif (defined(NRF51)||defined(NRF52))
    nrf_drv_timer_extended_compare(&timer, (nrf_timer_cc_channel_t)0, 10000000UL/freq, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
#endif
}
/*******************************************************************************
* 函数功能: 音乐演奏
* 说    明: 无
*******************************************************************************/
void Bsp_BuzzerMusic_PlayMusic_10ms(void)
{
    static uint8_t s_music_step  =  0xFF;
    static uint16_t s_num        =  0;
    static uint8_t s_jie_timer   =  0;
    static uint8_t s_jie         =  0;
    uint16_t i=0;
    //音乐处理
    switch(s_music_step)
    {
        case 0:  // 空闲
            if(BspBuzzerMusic_cmd==ON)
            {
                s_music_step=1;
            }
            break;
        case 1:  // 初始化
            if(p_BspBuzzerMusic_Lib!=NULL)
            {
                Bsp_BuzzerMusic_TimPwm_OnOff(ON);
                s_music_step  =  2;
                s_num         =  0;
                BspBuzzerMusiz_CurrentTimeS       =  0;
            }
            else
            {
                s_music_step=  0;
            }
            break;
        case 2:  // 播放中
            if(BspBuzzerMusiz_CurrentTimeS<0xFFFF*100)
            {
                BspBuzzerMusiz_CurrentTimeS++;
            }
            if(s_jie_timer<s_jie)
            {
                s_jie_timer++;
                break;
            }
            s_jie_timer= 0;
            switch(p_BspBuzzerMusic_Lib[s_num])
            {
                case 0x0B:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L1;
                    break;
                case 0x0C:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L2;
                    break;
                case 0x0D:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L3;
                    break;
                case 0x0E:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L4;
                    break;
                case 0x0F:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L5;
                    break;
                case 0x10:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L6;
                    break;
                case 0x11:
                    i = BSP_BUZZER_MUSIC_E_SCALE_L7;
                    break;
                case 0x15:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M1;
                    break;
                case 0x16:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M2;
                    break;
                case 0x17:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M3;
                    break;
                case 0x18:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M4;
                    break;
                case 0x19:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M5;
                    break;
                case 0x1a:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M6;
                    break;
                case 0x1b:
                    i = BSP_BUZZER_MUSIC_E_SCALE_M7;
                    break;
                case 0x1f:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H1;
                    break;
                case 0x20:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H2;
                    break;
                case 0x21:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H3;
                    break;
                case 0x22:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H4;
                    break;
                case 0x23:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H5;
                    break;
                case 0x24:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H6;
                    break;
                case 0x25:
                    i = BSP_BUZZER_MUSIC_E_SCALE_H7;
                    break;
                default:
                    i = 0;//表示无声音
                    break;
            }
            s_num++;
            //
            switch(p_BspBuzzerMusic_Lib[s_num])
            {
                case 0x00:
                    s_jie =160+5;
                    break;
                case 0x01:
                    s_jie =80+5;
                    break;
                case 0x02:
                    s_jie =40+5;
                    break;
                case 0x03:
                    s_jie =20+5;
                    break;
                case 0x04:
                    s_jie= 10+5;
                    break;
                case 0x66:
                    s_jie= 75+5;
                    break;
                default:
                    if(p_BspBuzzerMusic_Lib[s_num]&0x80)
                    {
                        s_jie=p_BspBuzzerMusic_Lib[s_num]-0x80;
                    }
                    else
                    {
                        s_jie=40+5;
                    }
                    break;
            }
            s_num++;
            //
            Bsp_BuzzerMusic_PwmOutWrite(i);
            //如果结束命令，则进入结束流程
            if(BspBuzzerMusic_cmd == OFF)
            {
                s_music_step=3;
            }
            //如果时间超时,则进入结束流程
            else if(BspBuzzerMusiz_CurrentTimeS>(BspBuzzerMusic_MaxTimeS*100) && BspBuzzerMusic_MaxTimeS!=0)
            {
                s_music_step=3;
            }
            //如果播放到结尾，则要判断时间是否结束
            else if((p_BspBuzzerMusic_Lib[s_num]==0 && p_BspBuzzerMusic_Lib[s_num+1]==0))
            {
                //按乐曲播放,则结束
                if(BspBuzzerMusic_MaxTimeS==0)
                {
                    s_music_step=3;
                }
                //按时间播放则重头再播
                else
                {
                    s_num=0;
                }
            }
            break;
        case 3:  // 播放完最后音
            if(s_jie_timer<s_jie)
            {
                s_jie_timer++;
                break;
            }
            s_music_step=4;
        case 4:  // 播放结束
            Bsp_BuzzerMusic_TimPwm_OnOff(OFF);
            p_BspBuzzerMusic_Lib=NULL;
            s_music_step = 0;
            break;
        default:
            Bsp_BuzzerMusic_TimPwm_OnOff(OFF);
            s_music_step = 0;
            break;
    }
}
/*******************************************************************************
* 函数功能: 蜂鸣音乐开始/停止
* 参    数: OnOff       ---   ON/OFF
            p_musicLib  ---   音乐库指针
            maxtime_s   ---   最大音乐时长(0表示追寻音乐库时长,ffff代表长响)
* 说    明: 无
*******************************************************************************/
uint8_t Bsp_BuzzerMusic_PlayStop(uint8_t OnOff,const uint8_t *p_musicLib,uint16_t maxtime_s)
{
    if(OnOff==ON)
    {
        if(p_BspBuzzerMusic_Lib!=NULL)
        {
            return ERR;
        }
        //同一个音频没有播放完毕,则只更新时间,最大播放时间按最长的计算
        if(p_BspBuzzerMusic_Lib==p_musicLib)
        {
            BspBuzzerMusiz_CurrentTimeS =  0;
            if(BspBuzzerMusic_MaxTimeS<maxtime_s)
            {
                BspBuzzerMusic_MaxTimeS =  maxtime_s;
            }
        }
        else
        {
            BspBuzzerMusic_cmd       =  OnOff;
            p_BspBuzzerMusic_Lib     =  p_musicLib;
            BspBuzzerMusic_MaxTimeS  =  maxtime_s;
        }
        //
        BspKey_IfPressKey();
    }
    else
    {
        //单次播放不能关闭
        if(BspBuzzerMusic_MaxTimeS!=0)
        {
            BspBuzzerMusic_cmd       =  OnOff;
        }
    }
    return OK;
}
/*******************************************************************************
* 函数功能: 蜂鸣音乐开始/停止
* 说    明: 无
*******************************************************************************/
uint8_t *Bsp_BuzzerMusic_GetState(void)
{
    return ((uint8_t *)p_BspBuzzerMusic_Lib);
}
void BspBuzzerMusic_DebugTestOnOff(uint8_t OnOff)
{
    OnOff = OnOff;
    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_MORNING,10);
}

