/**
  ******************************************************************************
  * @file    Bsp_Relay.c 
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
#include "Bsp_Relay.h"
//------------------------------- 用户变量 --------------------
static uint32_t Relay_Timer_100ms_Buf[BSP_RELAY_MAX_NUM]= {0};       //0-关闭,0x00000001-0xFFFFFFFD启动的ms时间,0xFFFFFFFE长关闭,0xFFFFFFFF长启动
static uint16_t Relay_RelayTimer_100ms_Buf[BSP_RELAY_MAX_NUM]= {0};  //延时操作计时器
static uint8_t Relay_Level[BSP_RELAY_MAX_NUM];
uint8_t Relay_PcSign[BSP_RELAY_MAX_NUM];        //0-无效,1-强开,2-强闭,3-临时开,4-临时闭
uint8_t Relay_LinkageSign[BSP_RELAY_MAX_NUM];   //继电器需要报联动信息的标志(0-不需要上报,1-需要上报)
static const uint32_t RelaySafeMaxTime100ms[BSP_RELAY_MAX_NUM]= {BSP_RELAY1_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY2_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY3_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY4_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY5_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY6_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY7_SAFE_MAX_TIME_100MS,
                                                                 BSP_RELAY8_SAFE_MAX_TIME_100MS
                                                                };
#ifdef RELAY_RECORD_ENABLE
S_MEMORY_ALARM_RECORD S_RelayRecord;//
#endif
//
static uint8_t BspRelay_DebugTest_Enable=0;
//------------------------------- 用户函数声明 ----------------
//-------------------------------------------------------------------------------
// 函数名称: void Kout_GPIO_Configuration(void)
// 函数功能: 入口函数,GPIO口配置
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
static void BspRelay_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_RELAY_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    //
#ifdef BSP_RELAY_RELAY1_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY1_PIN;
    GPIO_Init(BSP_RELAY_RELAY1_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY1_OFF;
#endif
#ifdef BSP_RELAY_RELAY2_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY2_PIN;
    GPIO_Init(BSP_RELAY_RELAY2_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY2_OFF;
#endif
#ifdef BSP_RELAY_RELAY3_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY3_PIN;
    GPIO_Init(BSP_RELAY_RELAY3_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY3_OFF;
#endif
#ifdef BSP_RELAY_RELAY4_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY4_PIN;
    GPIO_Init(BSP_RELAY_RELAY4_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY4_OFF;
#endif
#ifdef BSP_RELAY_RELAY5_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY5_PIN;
    GPIO_Init(BSP_RELAY_RELAY5_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY5_OFF;
#endif
#ifdef BSP_RELAY_RELAY6_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY6_PIN;
    GPIO_Init(BSP_RELAY_RELAY6_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY6_OFF;
#endif
#ifdef BSP_RELAY_RELAY7_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY7_PIN;
    GPIO_Init(BSP_RELAY_RELAY7_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY7_OFF;
#endif
#ifdef BSP_RELAY_RELAY8_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_RELAY_RELAY8_PIN;
    GPIO_Init(BSP_RELAY_RELAY8_PORT, &GPIO_InitStructure);
    BSP_RELAY_RELAY8_OFF;
#endif
#elif(defined(NRF52)||defined(NRF51))
#ifdef BSP_RELAY_RELAY1_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY1_PIN);
    BSP_RELAY_RELAY1_OFF;
#endif
#ifdef BSP_RELAY_RELAY2_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY2_PIN);
    BSP_RELAY_RELAY2_OFF;
#endif
#ifdef BSP_RELAY_RELAY3_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY3_PIN);
    BSP_RELAY_RELAY3_OFF;
#endif
#ifdef BSP_RELAY_RELAY4_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY4_PIN);
    BSP_RELAY_RELAY4_OFF;
#endif
#ifdef BSP_RELAY_RELAY5_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY5_PIN);
    BSP_RELAY_RELAY5_OFF;
#endif
#ifdef BSP_RELAY_RELAY6_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY6_PIN);
    BSP_RELAY_RELAY6_OFF;
#endif
#ifdef BSP_RELAY_RELAY7_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY7_PIN);
    BSP_RELAY_RELAY7_OFF;
#endif
#ifdef BSP_RELAY_RELAY8_PIN
    nrf_gpio_cfg_output(BSP_RELAY_RELAY8_PIN);
    BSP_RELAY_RELAY8_OFF;
#endif
#endif
}
//-------------------------------------------------------------------------------
// 函数名称: void MonitorPro_100ms(void)
// 函数功能: 输入函数-调用报警/记录函数
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
#ifdef RELAY_RECORD_ENABLE
void Relay_Record(S_MEMORY_ALARM_RECORD* p_record)
{
    uint16_t Event;
    uint8_t time5buf[5];
    Event=p_record->Event[0];
    Event<<=8;
    Event+=p_record->Event[1];
    //
    S_TransferRam.LastCardAndAlarmCode=Event;
    //获取当前时间
    BspRtc_ReadRealTime(time5buf,NULL,NULL,NULL);//读取当前时间--5字节
    p_record->RecordTime[0]=time5buf[0];   //时间
    p_record->RecordTime[1]=time5buf[1];   //时间
    p_record->RecordTime[2]=time5buf[2];   //时间
    p_record->RecordTime[3]=time5buf[3];   //时间
    p_record->RecordTime[4]=time5buf[4];   //时间
    //修正ms
    p_record->RecordTime[0] &= 0x80;
    p_record->RecordTime[0] += (Memory_RecordNum_ms&0x7F);
    Memory_RecordNum_ms++;
    //报警记录
    if(MONITOR_ALARM_BEGIN<=Event&&MONITOR_ALARM_END>=Event)
    {
        Memory_AppFunction(MEMORY_APP_CMD_ALARM_RECORD_W,(uint8_t*)p_record,0,0);
    }
    //事件记录
    else if(MONITOR_RECORD_BEGIN<=Event&&MONITOR_RECORD_END>=Event)
    {
        Memory_AppFunction(MEMORY_APP_CMD_CARD_RECORD_W,(uint8_t*)p_record,0,0);
    }
    //更新记录到EEPROM
    //Memory_AppFunction(MEMORY_APP_CMD_KEEPPARA_R,(INT8U*)&s_Monitor_KeepPara,0,0);
    //memcpy((INT8U*)&s_Monitor_KeepPara.MonitorSign,(INT8U*)&S_Monitor_Sign_Now,sizeof(S_MONITOR_SIGN));
    //Memory_AppFunction(MEMORY_APP_CMD_KEEPPARA_W,(INT8U*)&s_Monitor_KeepPara,0,0);
}
#endif
//-------------------------------------------------------------------------------
// 函数名称: void Relay_Pro(INT8U ch,INT8U state)
// 函数功能: 继电器处理
// 调用函数: 无
// 入口参数: ch - 1~RELAY_MAX_NUM
//           state - ON/OFF
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
static void Relay_Pro(uint8_t ch,uint8_t state)
{
    switch(ch)
    {
#ifdef   BSP_RELAY_RELAY1_PIN
        case 1:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY1_ON;
            else
                BSP_RELAY_RELAY1_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY2_PIN
        case 2:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY2_ON;
            else
                BSP_RELAY_RELAY2_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY3_PIN
        case 3:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY3_ON;
            else
                BSP_RELAY_RELAY3_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY4_PIN
        case 4:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY4_ON;
            else
                BSP_RELAY_RELAY4_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY5_PIN
        case 5:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY5_ON;
            else
                BSP_RELAY_RELAY5_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY6_PIN
        case 6:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY6_ON;
            else
                BSP_RELAY_RELAY6_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY7_PIN
        case 7:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY7_ON;
            else
                BSP_RELAY_RELAY7_OFF;
            break;
#endif
#ifdef   BSP_RELAY_RELAY8_PIN
        case 8:
            if(state==BSP_RELAY_ON)
                BSP_RELAY_RELAY8_ON;
            else
                BSP_RELAY_RELAY8_OFF;
            break;
#endif
        default:
            break;
    }
}
//-------------------------------------------------------------------------------
// 函数功能: 接口函数---继电器主函数,调用于100ms定时程序
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
void BspRelay_100ms(void)
{
    static uint8_t first=1;
    static uint16_t state=0;
    //
    static uint8_t si=0;
    static uint8_t step=0;
    //
    uint8_t i,k;
    // 调试状态不进行其他操作
    if(BspRelay_DebugTest_Enable==0)
    {
        if(first==1)
        {
            first=0;
            BspRelay_Init();
            for(i=0; i<BSP_RELAY_MAX_NUM; i++)
            {
                Relay_Level[i]=0;
                Relay_Pro(i+1,BSP_RELAY_OFF);
            }
            state=0;
        }
        //继电器处理
        for(i=0; i<BSP_RELAY_MAX_NUM; i++)
        {
            //延时操作
            if(Relay_RelayTimer_100ms_Buf[i]!=0)
            {
                Relay_RelayTimer_100ms_Buf[i]--;
                continue;
            }
            //执行保护
            if(Relay_Timer_100ms_Buf[i]>RelaySafeMaxTime100ms[i])
            {
                Relay_Timer_100ms_Buf[i]=RelaySafeMaxTime100ms[i];
            }
            //关闭
            if(Relay_Timer_100ms_Buf[i]==0)
            {
                Relay_Level[i]=0;
                Relay_Pro(i+1,BSP_RELAY_OFF);
                state&=~(1<<i);
            }
            //常开
            else if(Relay_Timer_100ms_Buf[i]==0xFFFFFFFF)
            {
                Relay_Pro(i+1,BSP_RELAY_ON);
                if((state &(1<<i))==0)
                {
                    state|=(1<<i);
                }
            }
            //常关
            else if(Relay_Timer_100ms_Buf[i]==0xFFFFFFFE)
            {
                Relay_Pro(i+1,BSP_RELAY_OFF);
                state&=~(1<<i);
            }
            else
            {
                Relay_Pro(i+1,BSP_RELAY_ON);
                Relay_Timer_100ms_Buf[i]--;
                if((state&(1<<i))==0)
                {
                    state|=(1<<i);
                }
            }
        }
        //事件生成
        i = BspRelay_Read();
        for(k=0; k<8; k++)
        {
            if(0!=(i&(1<<k))  && (Relay_Timer_100ms_Buf[k]==1))
            {
                Relay_PcSign[k]=0;
                Relay_LinkageSign[k]=0;
                //
#ifdef RELAY_RECORD_ENABLE
                S_RelayRecord.ReaderOrWireOrRelayNum=(k+1)|(Relay_LinkageSign[k]<<7);
                S_RelayRecord.Event[0]=MONITOR_RECORD_READER_LOCK_CLOSE>>8;
                S_RelayRecord.Event[1]=MONITOR_RECORD_READER_LOCK_CLOSE&0xFF;
                Relay_Record(&S_RelayRecord);
#endif
            }
        }
    }
    //DEBUG
    while(1)
    {
        si++;
        if(si<10)
            break;
        si=0;
        if(BspRelay_DebugTest_Enable==1)
        {
            switch(step)
            {
                case 0:
#ifdef BSP_RELAY_RELAY1_PIN
                    BSP_RELAY_RELAY1_ON;
                    step++;
                    break;
                case 1:
                    BSP_RELAY_RELAY1_OFF;
                    step++;
#else
                    step=2;
#endif
#ifdef BSP_RELAY_RELAY2_PIN
                case 2:
                    BSP_RELAY_RELAY2_ON;
                    step++;
                    break;
                case 3:
                    BSP_RELAY_RELAY2_OFF;
                    step++;
#else
                    step=4;
#endif
#ifdef BSP_RELAY_RELAY3_PIN
                case 4:
                    BSP_RELAY_RELAY3_ON;
                    step++;
                    break;
                case 5:
                    BSP_RELAY_RELAY3_OFF;
                    step++;
#else
                    step=6;
#endif
#ifdef BSP_RELAY_RELAY4_PIN
                case 6:
                    BSP_RELAY_RELAY4_ON;
                    step++;
                    break;
                case 7:
                    BSP_RELAY_RELAY4_OFF;
                    step++;
#else
                    step=8;
#endif
#ifdef BSP_RELAY_RELAY5_PIN
                case 8:
                    BSP_RELAY_RELAY5_ON;
                    step++;
                    break;
                case 9:
                    BSP_RELAY_RELAY5_OFF;
                    step++;
#else
                    step=10;
#endif
#ifdef BSP_RELAY_RELAY6_PIN
                case 10:
                    BSP_RELAY_RELAY6_ON;
                    step++;
                    break;
                case 11:
                    BSP_RELAY_RELAY6_OFF;
                    step++;
#else
                    step=12;
#endif
#ifdef BSP_RELAY_RELAY7_PIN
                case 12:
                    BSP_RELAY_RELAY7_ON;
                    step++;
                    break;
                case 13:
                    BSP_RELAY_RELAY7_OFF;
                    step++;
#else
                    step=14;
#endif
#ifdef BSP_RELAY_RELAY8_PIN
                case 14:
                    BSP_RELAY_RELAY8_ON;
                    step++;
                    break;
                case 15:
                    BSP_RELAY_RELAY8_OFF;
                    step++;
#endif
                default:
                    step=0;
                    break;
            }
        }
        break;
    }
}
//-------------------------------------------------------------------------------
// 函数功能: 接口函数---控制继电器
// 调用函数:   level :  RELAY_LEVEL_ENUM
//             ch    :  0~(RELAY_MAX_NUM-1)
//             time  :  单位100mS的时间(0-关闭,0xFFFFFFFF-常启动)
//             delaytime: 单位100mS的时间(延时启动)
// 入口参数: 无
// 返回参数: RELAY_WIRTE_ERR_ENUM
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint8_t BspRelay_Write(uint8_t level,uint8_t ch,uint32_t time,uint16_t delaytime)
{
    //参数验证
    if(ch>=BSP_RELAY_MAX_NUM)
    {
        return BSP_RELAY_WIRTE_ERR_PARA;
    }
    //等级验证
    if(level<Relay_Level[ch])
    {
        return BSP_RELAY_WIRTE_ERR_LEVEL;
    }
    //Relay_PcSign赋值
    if(level==BSP_RELAY_LEVEL_PC)
    {
        //强开
        if(time==0xFFFFFFFF)
        {
            Relay_PcSign[ch]=1;
#ifdef RELAY_RECORD_ENABLE
            S_RelayRecord.ReaderOrWireOrRelayNum=ch+1;
            S_RelayRecord.Event[0]=MONITOR_RECORD_PC_DOOR_KEEPOPEN>>8;
            S_RelayRecord.Event[1]=MONITOR_RECORD_PC_DOOR_KEEPOPEN&0xFF;
            Relay_Record(&S_RelayRecord);
#endif
        }
        //强关
        else if(time==0xFFFFFFFE)
        {
            Relay_PcSign[ch]=2;
#ifdef RELAY_RECORD_ENABLE
            S_RelayRecord.ReaderOrWireOrRelayNum=ch+1;
            S_RelayRecord.Event[0]=MONITOR_RECORD_PC_DOOR_KEEPCLOSE>>8;
            S_RelayRecord.Event[1]=MONITOR_RECORD_PC_DOOR_KEEPCLOSE&0xFF;
            Relay_Record(&S_RelayRecord);
#endif
        }
        //临时开
        else if(time!=0)
        {
            Relay_PcSign[ch]=3;
#ifdef RELAY_RECORD_ENABLE
            S_RelayRecord.ReaderOrWireOrRelayNum=ch+1;
            S_RelayRecord.Event[0]=MONITOR_RECORD_PC_DOOR_OPEN>>8;
            S_RelayRecord.Event[1]=MONITOR_RECORD_PC_DOOR_OPEN&0xFF;
            Relay_Record(&S_RelayRecord);
#endif
        }
        //临时关(解除平台控制)
        else
        {
            Relay_PcSign[ch]=0;
#ifdef RELAY_RECORD_ENABLE
            S_RelayRecord.ReaderOrWireOrRelayNum=ch+1;
            S_RelayRecord.Event[0]=MONITOR_RECORD_PC_DOOR_CLOSE>>8;
            S_RelayRecord.Event[1]=MONITOR_RECORD_PC_DOOR_CLOSE&0xFF;
            Relay_Record(&S_RelayRecord);
#endif
        }
    }
    //继电器控制(OLED控制继电器导致硬件错误死机,原因待查)
    if(time==0 && (level==BSP_RELAY_LEVEL_PC||level==BSP_RELAY_LEVEL_LOGIC))
    {
        Relay_LinkageSign[ch]=0;
#ifdef RELAY_RECORD_ENABLE
        S_RelayRecord.ReaderOrWireOrRelayNum=(ch+1)|(Relay_LinkageSign[ch]<<7);
        S_RelayRecord.Event[0]=MONITOR_RECORD_READER_LOCK_CLOSE>>8;
        S_RelayRecord.Event[1]=MONITOR_RECORD_READER_LOCK_CLOSE&0xFF;
        Relay_Record(&S_RelayRecord);
#endif
    }
    //
    Relay_Level[ch]=level;
    Relay_Timer_100ms_Buf[ch]=time;
    Relay_RelayTimer_100ms_Buf[ch]=delaytime;
    return BSP_RELAY_WIRTE_ERR_NO;
}
//-------------------------------------------------------------------------------
// 函数功能: 接口函数---读取继电器
// 调用函数: 无
// 入口参数: 无
// 返回参数: 无
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint8_t BspRelay_Read(void)
{
    uint8_t i;
    uint8_t res=0;
    for(i=0; i<BSP_RELAY_MAX_NUM; i++)
    {
        res>>=1;
        if(Relay_Timer_100ms_Buf[i]==0 || Relay_Timer_100ms_Buf[i]==0xFFFFFFFE)
        {
            res&=~0x80;
        }
        else if(Relay_Timer_100ms_Buf[i]==0xFFFFFFFF)
        {
            res|=0x80;
        }
        else
        {
            res|=0x80;
        }
    }
    return res;
}
//-------------------------------------------------------------------------------
// 函数功能: 接口函数---读取继电器
// 调用函数: 无
// 入口参数: 无
// 返回参数: TRUE(测试中) FALSE(测试完毕)
// 修改说明：
// 修改时间：
//-------------------------------------------------------------------------------
uint8_t BspRelay_Test(void)
{
    static uint8_t step=0;
    static uint8_t count=0;
    uint8_t res=TRUE;
    uint8_t i;
    switch(step)
    {
        case 0://开启继电器
            step++;
            for(i=0; i<BSP_RELAY_MAX_NUM; i++)
            {
                BspRelay_Write(BSP_RELAY_LEVEL_PC,i,100+10*(i+1),0);
            }
            count=0;
            //
            res=FALSE;
            //
            break;
        case 1://延时2秒
            count++;
            if(count>20)
            {
                step++;
            }
            break;
        case 2://等级测试:用RELAY_LEVEL_1关闭1,用RELAY_LEVEL_2关闭2,用RELAY_LEVEL_3关闭3
            BspRelay_Write(BSP_RELAY_LEVEL_LOGIC,0,0,0);
            BspRelay_Write(BSP_RELAY_LEVEL_PC,1,0,0);
            BspRelay_Write(BSP_RELAY_LEVEL_DEGUG,2,0,0);
            step++;
            break;
        case 3://退出
            step=0;
            res=FALSE;
            break;
        default:
            break;
    }
    return res;
}
void BspRelay_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspRelay_DebugTest_Enable=1;
    }
    else
    {
        BspRelay_DebugTest_Enable=0;
#ifdef BSP_RELAY_RELAY1_PIN
        BSP_RELAY_RELAY1_OFF;
#endif
#ifdef BSP_RELAY_RELAY2_PIN
        BSP_RELAY_RELAY2_OFF;
#endif
#ifdef BSP_RELAY_RELAY3_PIN
        BSP_RELAY_RELAY3_OFF;
#endif
#ifdef BSP_RELAY_RELAY4_PIN
        BSP_RELAY_RELAY4_OFF;
#endif
#ifdef BSP_RELAY_RELAY5_PIN
        BSP_RELAY_RELAY5_OFF;
#endif
#ifdef BSP_RELAY_RELAY6_PIN
        BSP_RELAY_RELAY6_OFF;
#endif
#ifdef BSP_RELAY_RELAY7_PIN
        BSP_RELAY_RELAY7_OFF;
#endif
#ifdef BSP_RELAY_RELAY8_PIN
        BSP_RELAY_RELAY8_OFF;
#endif
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
