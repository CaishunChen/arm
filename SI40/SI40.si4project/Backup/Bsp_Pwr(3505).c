/**
  ******************************************************************************
  * @file    Bsp_Pwr.c
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
//------------------------------------------------------------------------------
#include "includes.h"
#include "Bsp_Pwr.h"
#include "uctsk_Debug.h"

#if   (defined(NRF51)||defined(NRF52))
#include "nrf_drv_clock.h"
#endif
//------------------------------------------------------------------------------
BSP_PWR_E_RESET_FLAG Bsp_Pwr_ResetFlag;
#ifdef   BSP_PWR_CMD_ENABLE
#define BSP_PWR_CMD_BUFFERLOOP_BUF_MAX   10*sizeof(BSP_PWR_S_CMD)
#define BSP_PWR_CMD_BUFFERLOOP_NODE_MAX  10
static uint8_t BspPwr_Cmd_BufferLoop_Buf[BSP_PWR_CMD_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE BspPwr_Cmd_BufferLoop_Node[BSP_PWR_CMD_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT BspPwr_Cmd_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S BspPwr_Cmd_BufferLoop;
//信号量
MODULE_OS_SEM(BspPwr_Sem_Cmd);
//static uint8_t *pBspPwr_Cmd=NULL;
void BspPwr_Cmd_timer_1s(void)
{
    BSP_PWR_S_CMD s_cmd;
#if     (defined(PROJECT_NRF5X_BLE))
    static uint8_t s_led_timer=0;
    s_led_timer++;
    if(s_led_timer>=10)
    {
        s_led_timer=0;
        s_cmd.cmd    =  BSP_PWR_CMD_LED;
        s_cmd.num    =  BSP_PWR_CMD_LED_NUM_1;
        s_cmd.para1  =  BSP_PWR_CMD_LED_PARA_ONOFF;
        Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
        //发送消息
        MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
    }
#endif
#if     (defined(XKAP_ICARE_A_C))
    static uint8_t s_led_timer=0;
    s_led_timer++;

    // LED闪烁
    if(s_led_timer>=10)
    {
        s_cmd.cmd    =  BSP_PWR_CMD_LED;
        s_cmd.num    =  BSP_PWR_CMD_LED_NUM_1;
        s_cmd.para1  =  BSP_PWR_CMD_LED_PARA_ONOFF;
        Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
        MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
    }
    // 按键
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        s_cmd.cmd    =  BSP_PWR_CMD_KEY;
        s_cmd.num    =  BSP_PWR_CMD_KEY_NUM_1 + (BspKey_Value-BSP_KEY_E_KEY1);
        Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
        //发送消息
        MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
    }
    // 周期
    s_cmd.cmd    =  BSP_PWR_CMD_TIMER;
    Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
    MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
#endif
#if     (defined(PROJECT_NRF5X_BLE))
    static uint8_t s_bmp180_timer=0;
    s_bmp180_timer++;
    if(s_bmp180_timer>=5)
    {
        s_bmp180_timer=0;
        s_cmd.cmd    =  BSP_PWR_CMD_BMP180;
        s_cmd.para1  =  BSP_PWR_CMD_BMP180_GET;
        Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
        //发送消息
        MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
    }
    static uint16_t s_vcc_timer=3600-5;
    s_vcc_timer++;
    if(s_vcc_timer>=3600)
    {
        s_vcc_timer=0;
        s_cmd.cmd    =  BSP_PWR_CMD_VCC;
        Count_BufferLoopPush(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,sizeof(BSP_PWR_S_CMD));
        //发送消息
        MODULE_OS_SEM_POST(BspPwr_Sem_Cmd);
    }
#endif
}
#endif
void BspPwr_Cmd_parse(void)
{
#ifdef   BSP_PWR_CMD_ENABLE
    BSP_PWR_S_CMD s_cmd;
    MODULE_OS_ERR err;
    uint8_t i;
    uint16_t i16;
    MODULE_OS_SEM_PEND(BspPwr_Sem_Cmd,60000,TRUE,err);
    if(err!=MODULE_OS_ERR_NONE)
    {
        return;
    }
    i = Count_BufferLoopPop(&BspPwr_Cmd_BufferLoop,(uint8_t*)&s_cmd,&i16,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
    if(i!=OK)
    {
        return;
    }
    switch(s_cmd.cmd)
    {
        case BSP_PWR_CMD_LED:
            switch(s_cmd.num)
            {
                case BSP_PWR_CMD_LED_NUM_1:
#ifdef  BSP_LED_LED1_PIN
                    if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_OFF)
                    {
                        BSP_LED_LED1_OFF;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ON)
                    {
                        BSP_LED_LED1_ON;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_TOGGLE)
                    {
                        BSP_LED_LED1_TOGGLE;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ONOFF)
                    {
                        BSP_LED_LED1_ON;
                        MODULE_OS_DELAY_MS(1);
                        BSP_LED_LED1_OFF;
                    }
#endif
                    break;
                case BSP_PWR_CMD_LED_NUM_2:
#ifdef  BSP_LED_LED2_PIN
                    if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_OFF)
                    {
                        BSP_LED_LED2_OFF;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ON)
                    {
                        BSP_LED_LED2_ON;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_TOGGLE)
                    {
                        BSP_LED_LED2_TOGGLE;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ONOFF)
                    {
                        BSP_LED_LED2_ON;
                        MODULE_OS_DELAY_MS(1);
                        BSP_LED_LED2_OFF;
                    }
#endif
                    break;
                case BSP_PWR_CMD_LED_NUM_3:
#ifdef  BSP_LED_LED3_PIN
                    if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_OFF)
                    {
                        BSP_LED_LED3_OFF;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ON)
                    {
                        BSP_LED_LED3_ON;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_TOGGLE)
                    {
                        BSP_LED_LED3_TOGGLE;
                    }
                    else if(s_cmd.para1==BSP_PWR_CMD_LED_PARA_ONOFF)
                    {
                        BSP_LED_LED3_ON;
                        MODULE_OS_DELAY_MS(1);
                        BSP_LED_LED3_OFF;
                    }
#endif
                    break;
                default:
                    break;
            }
            break;
        case BSP_PWR_CMD_KEY:
            switch(s_cmd.num)
            {
                case BSP_PWR_CMD_KEY_NUM_1:
                    DebugLogOutStr(DEBUG_E_LEVEL_INFO,"KEY1 PRESS\r\n");
                    break;
                case BSP_PWR_CMD_KEY_NUM_2:
                    DebugLogOutStr(DEBUG_E_LEVEL_INFO,"KEY2 PRESS\r\n");
                    break;
                case BSP_PWR_CMD_KEY_NUM_3:
                    DebugLogOutStr(DEBUG_E_LEVEL_INFO,"KEY3 PRESS\r\n");
                    break;
                case BSP_PWR_CMD_KEY_NUM_4:
                    DebugLogOutStr(DEBUG_E_LEVEL_INFO,"KEY4 PRESS\r\n");
                    break;
                default:
                    break;
            }
            break;
        case BSP_PWR_CMD_BMP180:
#if     (defined(PROJECT_NRF5X_BLE))
            Hci_Timer(1000);
#endif
            break;
        case BSP_PWR_CMD_VCC:
#if     (defined(PROJECT_NRF5X_BLE))
            AD_VBAT_Read_mv();
#endif
            break;
        case BSP_PWR_CMD_TIMER:
#if     (defined(XKAP_ICARE_A_C))
            Bsp_NrfRfEsb_Tx("123456789012345678901",21);
#endif
            break;
        default:
            break;
    }
#else
    MODULE_OS_DELAY_MS(10000);
#endif
}

//------------------------------------------------------------------------------
static void Bsp_Pwr_Ready(void);
//------------------------------------------------------------------------------
//初始化
void Bsp_Pwr_Init(void)
{
#ifdef   BSP_PWR_CMD_ENABLE
    //初始化数据结构
    Count_BufferLoopInit(&BspPwr_Cmd_BufferLoop,\
                         BspPwr_Cmd_BufferLoop_Buf,\
                         BSP_PWR_CMD_BUFFERLOOP_BUF_MAX,\
                         BspPwr_Cmd_BufferLoop_Node,\
                         BSP_PWR_CMD_BUFFERLOOP_NODE_MAX,\
                         &BspPwr_Cmd_BufferLoop_Limit);
    //初始化信号量
    MODULE_OS_SEM_CREATE(BspPwr_Sem_Cmd,"BspPwr_Sem_Cmd",0);
#endif
#if   (defined(STM32F1)||defined(STM32F4))
    //设定中断组(freertos应用官方建议使用组4)
    /*
    概念：
        抢占式优先级          允许中断嵌套
        子优先级            不允许中断嵌套
    特殊：
        Reset、NMI、HardFault优先级为负数，高于普通中断优先级，且优先级不可配置。
        PendSV、SVC、SysTick优先级不特殊，跟普通外设一样需要设置即可。
    对于stm32F1与F4来说，中断配置如下(设置时，数值越小优先级越高)：
        优先级分组         抢占优先级     子优先级        高4位使用情况
        0(默认)           0级            0-15级     0   -   4
        1               0-1级      0-7级      1   -   3
        2               0-3级      0-3级      2   -   2
        3               0-7级      0-1级      3   -   1
        4               0-15级     0级            4   -   0
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    //
#if   (defined(STM32F1))
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
#elif (defined(STM32F4))
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
#endif
    // Allow access to BKP Domain
    PWR_BackupAccessCmd(ENABLE);
    // 等待 RTC APB 寄存器同步
    RTC_WaitForSynchro();
    //掉电复位
    // software reset
    if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_SFTRST;
    }
    else if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
        uint16_t i16;
        i16 = BSP_BKPRAM_READ(BSP_BKPRAM_POWER_CMT);
        i16++;
        BSP_BKPRAM_WRITE(BSP_BKPRAM_POWER_CMT, i16);
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_PORRST;
    }
    //复位引脚复位
    else  if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_PINRST;
    }
    // independent WatchDog reset
    else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_IWDGRST;
    }
    // Window Watchdog reset
    else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_WWDGRST;
    }
    // Low Power reset
    else if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_LPWRRST;
    }
#if   (defined(STM32F1))
#elif (defined(STM32F4))
    //POR/PDR 复位
    else if(RCC_GetFlagStatus(RCC_FLAG_BORRST) != RESET)
    {
        Bsp_Pwr_ResetFlag = BSP_PWR_E_RESET_FLAG_BORRST;
    }
#endif
    // 清除RCC中复位标志
#if   (defined(STM32F1))
    RCC_ClearFlag();
#elif (defined(STM32F4))
    RTC_ClearFlag(RTC_FLAG_ALRAF);
    //----------PVD
    if(1)
    {
        /*
        //中断
        NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        //
        EXTI_ClearITPendingBit(EXTI_Line16);
        EXTI_InitStructure.EXTI_Line = EXTI_Line16;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
        */
        // 0-2.0
        // 1-2.1
        // 2-2.3
        // 3-2.5
        // 4-2.6
        // 5-2.7
        // 6-2.8
        // 7-2.9
        PWR_PVDLevelConfig(PWR_PVDLevel_6);
        PWR_PVDCmd(ENABLE);
    }
#endif
    PWR_BackupAccessCmd(DISABLE);
#elif (defined(NRF51)||defined(NRF52))
    ret_code_t err_code;
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
#endif
}
//停止模式
void Bsp_Pwr_EnterStop(void)
{
    //准备
    Bsp_Pwr_Ready();
#if   (defined(STM32F1)||defined(STM32F4))
    //使能PWR外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    //使能唤醒管脚功能,PA0
    //PWR_WakeUpPinCmd(ENABLE);
    //进入停止（STOP）模式
    PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);
#elif (defined(NRF51)||defined(NRF52))
    {
#ifdef   SOFTDEVICE_PRESENT
        sd_power_system_off();
#else
        NRF_POWER->SYSTEMOFF = 1;
#endif
    }
#endif
}
//待机模式
void Bsp_Pwr_EnterStandby(void)
{
    //准备
    Bsp_Pwr_Ready();
#if   (defined(STM32F1)||defined(STM32F4))
    //使能PWR外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    //使能唤醒管脚功能,PA0
    PWR_WakeUpPinCmd(ENABLE);
    //进入待命（STANDBY）模式
    PWR_EnterSTANDBYMode();
#elif (defined(NRF51)||defined(NRF52))
    {
#ifdef   SOFTDEVICE_PRESENT
        uint32_t err_code = sd_app_evt_wait();
        APP_ERROR_CHECK(err_code);
#else
        // 标准方案
        __SEV();
        __WFE();
        __WFE();
        // 直接方案
        //SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif
    }
#endif
}

static void Bsp_Pwr_Ready(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
    uint8_t i=0;
    uint16_t i16=0;
    uint8_t *pbuf;
    //标记
    BSP_BKPRAM_WRITE(BSP_BKPRAM_WAKE_UP, 0x1);
    //关闭GPRS
    GprsNet_OnOff(OFF);
    //关闭RFMS
    uctsk_Rfms_PowerOnOff(OFF);
    //关闭RFMS电源
    //uctsk_Rfms_Power(OFF);
    //关闭温湿度传感器
    BSP_TEMPHUMIDSENSOR_POWER_OFF;
    BspDac_DeInit(1);
    BspDac_DeInit(2);
    //关闭屏显
    uctsk_Rfms_PowerSavingOff=1;
    //等待GPRS关闭
    while(i<20)
    {
        if(GprsNet_s_Info.OnOff==OFF || GprsNet_s_Info.state==0)
        {
            break;
        }
        else
        {
            MODULE_OS_DELAY_MS(1000);
            i++;
            GprsNet_OnOff(OFF);
        }
    }
    //关闭LCD驱动
    LCD_ILI9341_EnterSleep();
    //关闭背光
    LCD_SetBackLight(0);
    //关闭SDIO
    SDIO_ClockCmd(DISABLE);
    SDIO_SetPowerState(SDIO_PowerState_OFF);
    SDIO_DeInit();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);
    DMA_Cmd(DMA2_Stream3, DISABLE);
    //关闭USB

    //
    MODULE_OS_DELAY_MS(100);
    //复位所有IO口
    //RCC_AHB1PeriphResetCmd(0X01FF,ENABLE);
    //开启顶部按键中断(中断内重启设备)
    Bsp_Exti_Init();
    //闹钟设置
    {
        struct tm *ptm;
        int32_t i32=-1;
        int32_t j32;
        pbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        ptm    =  (struct tm *)&pbuf[64];
        // 自动检测
        if(ModuleMemory_psPara->RtcAutoOnOff==ON)
        {
            ptm->tm_year =  CurrentDate.tm_year;
            ptm->tm_mon  =  CurrentDate.tm_mon;
            ptm->tm_mday =  CurrentDate.tm_mday;
            ptm->tm_hour =  (uint8_t)((ModuleMemory_psPara->RtcAutoBeginTime)>>8);
            ptm->tm_min  =  (uint8_t)((ModuleMemory_psPara->RtcAutoBeginTime));
            ptm->tm_sec  =  0x00;
            j32 = Count_TimeCompare(ptm,&CurrentDate);
            if(j32>5&&(i32<0||j32<i32))
            {
                i32=j32;
            }
        }
        // 定时提醒
        BspRtc_ReadWarn(1,&i,&i16);
        if(i==ON)
        {
            ptm->tm_year =  CurrentDate.tm_year;
            ptm->tm_mon  =  CurrentDate.tm_mon;
            ptm->tm_mday =  CurrentDate.tm_mday;
            ptm->tm_hour =  (uint8_t)(i16>>8);
            ptm->tm_min  =  (uint8_t)(i16);
            ptm->tm_sec  =  0x00;
            j32 = Count_TimeCompare(ptm,&CurrentDate);
            if(j32>5&&(i32<0||j32<i32))
            {
                i32=j32;
            }
        }
        // 闹钟1
        BspRtc_ReadAlarm(1,&i16,&i);
        if(i!=0)
        {
            ptm->tm_year =  CurrentDate.tm_year;
            ptm->tm_mon  =  CurrentDate.tm_mon;
            ptm->tm_mday =  CurrentDate.tm_mday;
            ptm->tm_hour =  (uint8_t)(i16>>8);
            ptm->tm_min  =  (uint8_t)(i16);
            ptm->tm_sec  =  0x00;
            j32 = Count_TimeCompare(ptm,&CurrentDate);
            if(j32>5&&(i32<0||j32<i32))
            {
                i32=j32;
            }
        }
        // 闹钟2
        BspRtc_ReadAlarm(2,&i16,&i);
        if(i!=0)
        {
            ptm->tm_year =  CurrentDate.tm_year;
            ptm->tm_mon  =  CurrentDate.tm_mon;
            ptm->tm_mday =  CurrentDate.tm_mday;
            ptm->tm_hour =  (uint8_t)(i16>>8);
            ptm->tm_min  =  (uint8_t)(i16);
            ptm->tm_sec  =  0x00;
            j32 = Count_TimeCompare(ptm,&CurrentDate);
            if(j32>5&&(i32<0||j32<i32))
            {
                i32=j32;
            }
        }
        //
        if(i32>5)
        {
            struct tm t_tm;
            //
            BspRtc_ReadRealTime(NULL,NULL,(uint32_t*)&j32,NULL);
            j32+=(i32-5);
            t_tm = Count_Time_ConvUnixToCalendar(j32);
            pbuf[3]=t_tm.tm_hour;
            pbuf[4]=t_tm.tm_min;
            pbuf[5]=t_tm.tm_sec;
            BspRtc_AlarmConfig(1,pbuf);
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#endif
#elif (defined(NRF51)||defined(NRF52))
#endif
}


BSP_PWR_E_POWER Bsp_Pwr_Monitor(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
#if (defined(PROJECT_XKAP_V3))
    static BSP_PWR_E_POWER powerBak = BSP_PWR_E_POWER_NULL;
    BSP_PWR_E_POWER power = BSP_PWR_E_POWER_NULL;
    uint8_t i;
    // 供电电压小于3000mV进入停止模式
    if(ADC_Vref_mV<BSP_PWR_BAT_LOW_MV)
    {
        Bsp_Pwr_EnterStop();
    }
    // AC供电
    else if((ADC_mV[AD_E_CHANNEL_AC]*2)>=BSP_PWR_AC_MV)
    {
        power = BSP_PWR_E_POWER_AC;
    }
    // 电池正常供电
    else if((ADC_mV[AD_E_CHANNEL_BAT]*2)>=BSP_PWR_BAT_FULL_MV)
    {
        power = BSP_PWR_E_POWER_BAT_FULL;
        uctsk_Rfms_PowerSavingOff=1;
    }
    // 电池中等供电
    else if((ADC_mV[AD_E_CHANNEL_BAT]*2)>=BSP_PWR_BAT_MID_MV)
    {
        power = BSP_PWR_E_POWER_BAT_MID;
        uctsk_Rfms_PowerSavingOff=1;
    }
    // 电池低电
    else if((ADC_mV[AD_E_CHANNEL_BAT]*2)>=BSP_PWR_BAT_LOW_MV)
    {
        power = BSP_PWR_E_POWER_LOW;
        uctsk_Rfms_PowerSavingOff=1;
    }
    // 电池电压过低进入停止模式
    else
    {
        Bsp_Pwr_EnterStop();
    }
    //电池供电定时关屏
    if(powerBak==BSP_PWR_E_POWER_NULL)
    {
        powerBak=power;
    }
    else if(powerBak!=power)
    {
        powerBak=power;
        if(power==BSP_PWR_E_POWER_AC)
        {
            //如果不是测量中,则系统重启
            if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())
            {
                MCU_SOFT_RESET;
            }
        }
        else
        {
            i = LCD_GetBackLight();
            if(i>BSP_PWR_LIGHT_LEVEL)
            {
                i=BSP_PWR_LIGHT_LEVEL;
                LCD_SetBackLight(i);
            }
            uctsk_Rfms_PowerSavingOff=1;
            GprsNet_OnOff(OFF);
        }
    }
    //电池供电关闭GPRS
    if(power != BSP_PWR_E_POWER_AC)
    {
        GprsNet_OnOff(OFF);
    }
    //
    return power;
#elif (defined(XKAP_ICARE_B_D_M))
    return BSP_PWR_E_POWER_AC;
#else
    return BSP_PWR_E_POWER_NULL;
#endif
#elif (defined(NRF51)||defined(NRF52))
    return BSP_PWR_E_POWER_NULL;
#endif
}
//-------------------------------------
void Bsp_Pwr_Reset_DebugTestOnOff(uint8_t OnOff)
{
    OnOff=OnOff;
    MCU_SOFT_RESET;
}
