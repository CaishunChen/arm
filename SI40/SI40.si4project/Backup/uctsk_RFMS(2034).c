/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-08-03
* ˵��: ��Ƶ�˶�����������(RFMS-->Radio Frequency Motion Sensor)
***********************************************************************************
*/
//-------------------���ؿ⺯��------------------------------
#include "uctsk_RFMS.h"
#include "Bsp_Uart.h"
#include "uctsk_Debug.h"
#include "Module_Memory.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Rtc.h"
//#include "Bsp_TempHumidSensor.h"
#include "Bsp_BuzzerMusic.h"
#include "uctsk_Sensor.h"

#include "Qed_cgen.h"

#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "Module_SdFat.h"
#include "bsp_tft_lcd.h"
#include "uctsk_GprsNet.h"
#include "Bsp_Pwr.h"
#include "uctsk_Hci.h"
#endif
//-------------------���ݽṹ--------------------------------
typedef struct RFMS_S_DATA_SAVE_TO_TF
{
    uint8_t    *pData;
    uint16_t   DataLen;
    uint16_t   ErrCmt;
    uint8_t    SaveSign;
} RFMS_S_DATA_SAVE_TO_TF;
//-------------------��̬����--------------------------------
// ��0   ---   ���ڽ�����������
#define UCTSK_RFMS_BUFFERLOOP_BUF_MAX   256
#define UCTSK_RFMS_BUFFERLOOP_NODE_MAX  120
static uint8_t UctskRfms_BufferLoop_Buf[UCTSK_RFMS_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskRfms_BufferLoop_Node[UCTSK_RFMS_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskRfms_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S UctskRfms_BufferLoop;
uint16_t UctskRfms_BufferLoopAllErrCmt=0;
uint16_t UctskRfms_BufferLoopMaxErrCmt=0;
// ��1   ---   ���ڻ���ƽ������
#define UCTSK_RFMS_BUFFERLOOP1_BUF_MAX  256
#define UCTSK_RFMS_BUFFERLOOP1_NODE_MAX 12
static uint8_t UctskRfms_BufferLoop1_Buf[UCTSK_RFMS_BUFFERLOOP1_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE UctskRfms_BufferLoop1_Node[UCTSK_RFMS_BUFFERLOOP1_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT UctskRfms_BufferLoop1_Limit;
static COUNT_BUFFERLOOP_S UctskRfms_BufferLoop1;
// ����
MODULE_OS_TASK_TAB(AppTaskRfmsTCB);
MODULE_OS_TASK_STK(App_TaskRfmsStk,APP_TASK_RFMS_STK_SIZE);
MODULE_OS_TASK_TAB(AppTaskRfmsTxTCB);
MODULE_OS_TASK_STK(App_TaskRfmsTxStk,APP_TASK_RFMS_TX_STK_SIZE);
MODULE_OS_TASK_TAB(AppTaskRfmsTestTCB);
MODULE_OS_TASK_STK(App_TaskRfmsTestStk,APP_TASK_RFMS_TEST_STK_SIZE);
// �ź���
MODULE_OS_SEM(UctskRfms_Sem_Rx);
MODULE_OS_SEM(UctskRfms_Sem_Tx);
MODULE_OS_SEM(UctskRfms_Sem_GetFileData);
// ͨ��
static uint8_t Uctsk_Rfms_Data_DebugTest_Enable       =  0;
static uint8_t Uctsk_Rfms_Wave_DebugTest_Enable       =  0;
uint8_t Uctsk_Rfms_RunFileData_DebugTest_Enable =  OFF;
static const uint8_t UCTSK_RFMS_POWER_ON[]      =  {0xAA,0x01,0x55^0xAA^0x01,0xA5};
static const uint8_t UCTSK_RFMS_POWER_OFF[]     =  {0xAA,0x02,0x55^0xAA^0x02,0xA5};
static const uint8_t UCTSK_RFMS_DATA_GET[]      =  {0x03};
static uint8_t Uctsk_Rfms_RunState     =  UCTSK_RFMS_E_RUNSTATE_IDLE;
static uint16_t Uctsk_Rfms_RxZeroCmt   =  0;
uint16_t Uctsk_Rfms_RxZeroCmtInfo      =  0;
// �ڵ�ģʽ
#define UCTSK_RFMS_POWER_SAVING_ON_TIME_S          (60*3)
#define UCTSK_RFMS_POWER_SAVING_MAX_VALUE          (15000)
uint16_t Uctsk_Rfms_PowerSavingTimer_s   =  UCTSK_RFMS_POWER_SAVING_ON_TIME_S;
static uint8_t  Uctsk_Rfms_PowerSaving_Backlight=  100;
static uint16_t Uctsk_Rfms_KeyCountBak          =  0;
uint8_t uctsk_Rfms_PowerSavingOff               =  0;
// TF���洢
static RFMS_S_DATA_SAVE_TO_TF Rfms_s_DataSaveToTf;
//-------------------ȫ�ֱ���--------------------------------
uint32_t uctsk_Rfms_TxNum=0;
uint32_t uctsk_Rfms_RxNum=0;
uint32_t uctsk_Rfms_MonitorRxNum=0;
uint16_t uctsk_Rfms_RxValue=0;
uint8_t uctsk_SmartAlarmRunBitmap=0;
uint8_t uctsk_CurrentSleepLevel=0;
uint8_t  uctsk_Rfms_Err=0;
//δ��Ҫ�ر���������Ŀ���,ͳһ��Flash����ȡ����
MODULE_MEMORY_S_DAY_INFO         uctsk_Rfms_s_DayInfo;
MODULE_MEMORY_S_DAY_SLEEPLEVEL   uctsk_Rfms_s_DaySleepLevel;
MODULE_MEMORY_S_DAY_BREATH       uctsk_Rfms_s_DayBreath;
MODULE_MEMORY_S_DAY_HEARTRATE    uctsk_Rfms_s_HeartRate;
MODULE_MEMORY_S_DAY_BODYMOVE     uctsk_Rfms_s_BodyMove;
// 1->��ǰ����Ϊ�洢ԭʼ���ݵķ������,0->��ʵ��¼���
uint8_t uctsk_Rfms_ExtFlashReadSign=0;
// RFMS�汾��Ϣ
uint8_t uctsk_Rfms_Hardware=0,uctsk_Rfms_Software=0;
// �嶯������ֵ��ʱ��
uint32_t   uctsk_Rfms_NoBodyThresholdValue   =  0;
uint32_t   uctsk_Rfms_NoBodyThresholdVTimerS =  0;
//-------------------��̬����--------------------------------
static void uctsk_Rfms(void *pvParameters);
static void uctsk_RfmsTx(void *pvParameters);
static void uctsk_RfmsTest(void *pvParameters);
static void Uctsk_RfmsTempHumi(uint8_t step);
static void Arithmetic_SmartAlarm(uint8_t step,uint16_t dat);
static void Arithmetic_BodyMove(uint8_t step,uint16_t dat);
static void Arithmetic_Breath(uint8_t step,uint16_t dat);
static void Arithmetic_HeartRate(uint8_t step,uint16_t dat);
static void uctsk_Rfms_DataSaveToTf(uint8_t step,uint16_t dat);
static void Arithmetic_MovePower(uint8_t step,uint16_t dat);
//�����㷨
typedef struct SLEEP_S_MARK
{
    //-----����
    //ǰ4Сʱ����˯ʱ��
    uint16_t DeepLevelTime_4h_m;
    //ǰ4Сʱ����˯ʱ��
    uint16_t MiddleLevelTime_4h_m;
    //ǰ4Сʱ����˯ʱ��
    uint16_t DeepLevelTime_After4h_m;
    //ǰ4Сʱ����˯ʱ��
    uint16_t MiddleLevelTime_After4h_m;
    //��˯ʱ��
    uint16_t TotalLevelTime_m;
    //��˯ʱ��
    uint16_t FallAsleepTime_m;
    //�嶯����
    uint16_t BodyMoveCount;
    //ʼ˯ʱ��仯
    uint16_t BeginSleepTimeChange_m;
    //���ӻ���ǰ״̬(������ʱ��״̬(������/��Ȼ������))
    uint8_t AlarmWakeState;
    //�¶�
    int8_t TempBuf[12];
    //ʪ��
    uint8_t HumiBuf[12];
    //����
    uint8_t UserAge;
    //-----���
    //��˯ʱ������ϵ��
    uint16_t Mart_TotalSleep;
    //˯�߽ṹ�÷�
    uint8_t Mark_SleepStruct;
    //��˯ʱ���÷�
    uint8_t Mart_SleepIntoPeriod;
    //�嶯�����÷�
    uint8_t Mark_BodyMovement;
    //����״̬�÷�
    uint8_t Mark_WakeState;
    //˯��ϰ�ߵ÷�
    uint8_t Mark_BeginSleepTimeChange;
    //˯���¶ȵ÷�
    uint8_t Mark_Temp;
    //˯��ʪ�ȵ÷�
    uint8_t Mark_Humi;
    //˯���ܷ�
    uint8_t Mart_All;
} SLEEP_S_MARK;
static uint8_t Arithmetic_SleepMarkInfoLoad(MODULE_MEMORY_S_DAY_INFO* psInfo,MODULE_MEMORY_S_DAY_SLEEPLEVEL* psSleepLevel,SLEEP_S_MARK* psMark);
static uint8_t Arithmetic_SleepMark(SLEEP_S_MARK *psSleepMark);
//���ݲɼ�ʱ���(0-12)
extern struct tm CurrentDate;
static uint8_t uctsk_Rfms_KeepBodyMove_AnalyzeSleepTime(uint8_t *p_buf,uint8_t *BeginTime,uint8_t *EndTime);
/*******************************************************************************
��������: ��ȡRFMS״̬
*******************************************************************************/
uint8_t uctsk_Rfms_GetRunState(void)
{
    return Uctsk_Rfms_RunState;
}
/*******************************************************************************
��������: �����������
*******************************************************************************/
/*
void uctsk_Rfms_Power(uint8_t OnOff)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(OnOff==ON)
    {
        //
    }
    else if(OnOff==OFF)
    {
        //�رմ���
#if   (defined(STM32F1))
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
        }
#elif (defined(STM32F4))
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
        }
#endif
        //�رմ���
        GPIO_InitStructure.GPIO_Pin   = UART1_GPIO_TX_PIN;
        GPIO_Init(UART1_GPIO_TX_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin  = UART1_GPIO_RX_PIN;
        GPIO_Init(UART1_GPIO_RX_PORT, &GPIO_InitStructure);
        GPIO_ResetBits(UART1_GPIO_TX_PORT , UART1_GPIO_TX_PIN);
        GPIO_ResetBits(UART1_GPIO_RX_PORT , UART1_GPIO_RX_PIN);
        //
        UCTSK_RFMS_POWER_PIN_OFF;
    }
}
*/
/*******************************************************************************
��������: ǿ������RFMS״̬
*******************************************************************************/
void uctsk_Rfms_SetRunState(uint8_t state)
{
    if(state==UCTSK_RFMS_E_RUNSTATE_IDLE && Uctsk_Rfms_RunState!=UCTSK_RFMS_E_RUNSTATE_IDLE)
    {
        // ִ��STOP���������ͷŻ���
        if(Uctsk_Rfms_RunState>UCTSK_RFMS_E_RUNSTATE_START && Uctsk_Rfms_RunState<UCTSK_RFMS_E_RUNSTATE_STOP)
        {
            Arithmetic_BodyMove(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            Arithmetic_Breath(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            Arithmetic_HeartRate(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            Uctsk_RfmsTempHumi(UCTSK_RFMS_E_RUNSTATE_STOP);
            Arithmetic_SmartAlarm(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            uctsk_Rfms_DataSaveToTf(UCTSK_RFMS_E_RUNSTATE_STOP,0);
        }
        //
        memset((char*)&uctsk_Rfms_s_DayInfo,0,sizeof(uctsk_Rfms_s_DayInfo));
        memset((char*)&uctsk_Rfms_s_DaySleepLevel,0,sizeof(uctsk_Rfms_s_DaySleepLevel));
        memset((char*)&uctsk_Rfms_s_DayBreath,0,sizeof(uctsk_Rfms_s_DayBreath));
        memset((char*)&uctsk_Rfms_s_HeartRate,0,sizeof(uctsk_Rfms_s_HeartRate));
        memset((char*)&uctsk_Rfms_s_BodyMove,0,sizeof(uctsk_Rfms_s_BodyMove));
        //
        if(BSP_PWR_E_POWER_AC==Hci_Power)
        {
            GprsNet_OnOff(ON);
        }
    }
    Uctsk_Rfms_RunState=state;
}

/*******************************************************************************
��������: �������ݽ���
*******************************************************************************/
UCTSK_S_MOVE_POWER uctsk_s_RfmsMovePower;
uint16_t uctsk_Rfms_ExRxNumBuf[6]= {0};
void uctsk_Rfms_RxIrq(uint8_t *pbuf,uint16_t len)
{
    static uint16_t s16=0;
    //�������
    if(Uctsk_Rfms_Data_DebugTest_Enable==ON)
    {
        DebugOutHex("Rx:",pbuf,len);
    }
    else if(Uctsk_Rfms_Wave_DebugTest_Enable==ON && len==2)
    {
        uint16_t i16=Count_2ByteToWord(pbuf[0],pbuf[1]);
        uint8_t buf[7];
        sprintf((char *)buf,"%d\r\n",i16);
        DebugOutStr((int8_t*)buf);
    }
    if(len==2||len==4)
    {
        //����ѹջ
        if(OK==Count_BufferLoopPush(&UctskRfms_BufferLoop,pbuf,len))
        {
            s16=0;
        }
        else
        {
            UctskRfms_BufferLoopAllErrCmt++;
            s16++;
            if(s16>UctskRfms_BufferLoopMaxErrCmt)
            {
                UctskRfms_BufferLoopMaxErrCmt=s16;
            }
            if(s16>500)
            {
                s16=0;
                Count_BufferLoopInit(&UctskRfms_BufferLoop,\
                                     UctskRfms_BufferLoop_Buf,\
                                     UCTSK_RFMS_BUFFERLOOP_BUF_MAX,\
                                     UctskRfms_BufferLoop_Node,\
                                     UCTSK_RFMS_BUFFERLOOP_NODE_MAX,\
                                     &UctskRfms_BufferLoop_Limit);
            }
        }
        //������Ϣ
        MODULE_OS_SEM_POST(UctskRfms_Sem_Rx);
    }
    else if(len==20)
    {
        if(pbuf[19]==(uint8_t)Count_Sum(0,pbuf,19))
        {
            memset((uint8_t*)&uctsk_s_RfmsMovePower,0,sizeof(UCTSK_S_MOVE_POWER));
            uctsk_s_RfmsMovePower.Num=pbuf[0];
            BspRtc_ReadRealTime(NULL,NULL,NULL,uctsk_s_RfmsMovePower.Time);
            uctsk_s_RfmsMovePower.Data[0]=Count_4ByteToLong(0,pbuf[1],pbuf[2],pbuf[3]);
            uctsk_s_RfmsMovePower.Data[1]=Count_4ByteToLong(0,pbuf[4],pbuf[5],pbuf[6]);
            uctsk_s_RfmsMovePower.Data[2]=Count_4ByteToLong(0,pbuf[7],pbuf[8],pbuf[9]);
            uctsk_s_RfmsMovePower.Data[3]=Count_4ByteToLong(0,pbuf[10],pbuf[11],pbuf[12]);
            uctsk_s_RfmsMovePower.Data[4]=Count_4ByteToLong(0,pbuf[13],pbuf[14],pbuf[15]);
            uctsk_s_RfmsMovePower.Data[5]=Count_4ByteToLong(0,pbuf[16],pbuf[17],pbuf[18]);
            Count_BufferLoopPush(&UctskRfms_BufferLoop1,(uint8_t*)&uctsk_s_RfmsMovePower,sizeof(uctsk_s_RfmsMovePower));
            if(pbuf[0]<6)
            {
                uctsk_Rfms_ExRxNumBuf[pbuf[0]]++;
            }
        }
    }
}

/*******************************************************************************
��������: ��������
*******************************************************************************/
static void uctsk_Rfms_Tx(uint8_t *pbuf,uint16_t len)
{
    if(Uctsk_Rfms_Data_DebugTest_Enable==ON)
    {
        DebugOutHex("Tx:",pbuf,len);
    }
    if(Uctsk_Rfms_RunFileData_DebugTest_Enable==OFF)
    {
        UART_DMA_Tx(UCTSK_RFMS_UARTX,pbuf,len);
    }
}
/*******************************************************************************
��������:
*******************************************************************************/
void uctsk_Rfms_PowerOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        UCTSK_RFMS_ON;
    }
    else
    {
        UCTSK_RFMS_OFF;
    }
}
/*******************************************************************************
��������:
��    ��: ManualOrAuto  0---�ֶ� 1---�Զ�
*******************************************************************************/
void uctsk_Rfms_SetOnOff(uint8_t OnOff,uint8_t ManualOrAuto)
{
    if(OnOff==ON)
    {
        // ���ݴ���
        memset((char*)&uctsk_Rfms_s_DayInfo,0,sizeof(uctsk_Rfms_s_DayInfo));
        memset((char*)&uctsk_Rfms_s_DaySleepLevel,0,sizeof(uctsk_Rfms_s_DaySleepLevel));
        memset((char*)&uctsk_Rfms_s_DayBreath,0,sizeof(uctsk_Rfms_s_DayBreath));
        memset((char*)&uctsk_Rfms_s_HeartRate,0,sizeof(uctsk_Rfms_s_HeartRate));
        memset((char*)&uctsk_Rfms_s_BodyMove,0,sizeof(uctsk_Rfms_s_BodyMove));
        uctsk_Rfms_TxNum=0;
        uctsk_Rfms_RxNum=0;
        uctsk_Rfms_MonitorRxNum=0;
        uctsk_CurrentSleepLevel=0;
        //
        BspRtc_ReadRealTime(NULL,NULL,NULL,uctsk_Rfms_s_DayInfo.BeginTime);
        uctsk_Rfms_s_DayInfo.BeginTime[6]=uctsk_Rfms_s_DayInfo.BeginTime[5];
        uctsk_Rfms_s_DayInfo.BeginTime[5]=uctsk_Rfms_s_DayInfo.BeginTime[4];
        uctsk_Rfms_s_DayInfo.BeginTime[4]=uctsk_Rfms_s_DayInfo.BeginTime[3];
        uctsk_Rfms_s_DayInfo.BeginTime[3]=uctsk_Rfms_s_DayInfo.BeginTime[2];
        uctsk_Rfms_s_DayInfo.BeginTime[2]=uctsk_Rfms_s_DayInfo.BeginTime[1];
        uctsk_Rfms_s_DayInfo.BeginTime[1]=(uint8_t)(uctsk_Rfms_s_DayInfo.BeginTime[0]+2000);
        uctsk_Rfms_s_DayInfo.BeginTime[0]=(uint8_t)((uctsk_Rfms_s_DayInfo.BeginTime[0]+2000)>>8);
        //
        uctsk_Rfms_s_DayInfo.Flag_StartMode  =  ManualOrAuto;
        // Ӧ�ô���
        Uctsk_Rfms_RunState=UCTSK_RFMS_E_RUNSTATE_START;
        Arithmetic_BodyMove(UCTSK_RFMS_E_RUNSTATE_START,0);
        Arithmetic_Breath(UCTSK_RFMS_E_RUNSTATE_START,0);
        Arithmetic_HeartRate(UCTSK_RFMS_E_RUNSTATE_START,0);
        Uctsk_RfmsTempHumi(UCTSK_RFMS_E_RUNSTATE_START);
        Arithmetic_SmartAlarm(UCTSK_RFMS_E_RUNSTATE_START,0);
        uctsk_Rfms_DataSaveToTf(UCTSK_RFMS_E_RUNSTATE_START,0);
        Uctsk_Rfms_RunState=UCTSK_RFMS_E_RUNSTATE_START_DELAY;
        //
        Uctsk_Rfms_RxZeroCmt=100*30;
        MODULE_OS_DELAY_MS(500);
        UCTSK_RFMS_ON;
        MODULE_OS_DELAY_MS(500);
        Uctsk_Rfms_RxZeroCmt=0;
    }
    else
    {
        uctsk_Rfms_s_DayInfo.Flag_StopMode  =  ManualOrAuto;
        Uctsk_Rfms_RunState = UCTSK_RFMS_E_RUNSTATE_STOP;
    }
}
/*******************************************************************************
��������: ����
*******************************************************************************/
void uctsk_Rfms_1mS(void)
{
    static uint8_t s_count=0;
    if(Uctsk_Rfms_RunFileData_DebugTest_Enable==ON)
    {
        return;
    }
    s_count++;
    if(s_count>=10)
    {
        s_count=0;
        if(Uctsk_Rfms_RxZeroCmt<100*30)
        {
            //������Ϣ
            MODULE_OS_SEM_POST(UctskRfms_Sem_Tx);
        }
    }
}
/*
void uctsk_Rfms_1s(void)
{

    if(Uctsk_Rfms_FinalData_DebugTest_Enable==ON)
    {
        uint8_t *pbuf;
        //���뻺��
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        sprintf((char*)pbuf,"BodyMoveCount=%d\r\n",Count_2ByteToWord(uctsk_Rfms_s_DayInfo.BodyMoveCount[0],uctsk_Rfms_s_DayInfo.BodyMoveCount[1]));
        DebugOutStr((int8_t*)pbuf);
        sprintf((char*)pbuf,"BodyMoveNum=%d\r\n",uctsk_Rfms_s_DayInfo.BodyMoveNum);
        DebugOutStr((int8_t*)pbuf);
        DebugOutHex((char*)"uctsk_Rfms_s_BodyMove.DataBuf=",uctsk_Rfms_s_BodyMove.DataBuf,sizeof(uctsk_Rfms_s_BodyMove.DataBuf));
        MODULE_OS_DELAY_MS(500);
        DebugOutHex((char*)"uctsk_Rfms_s_DayInfo="       ,(uint8_t*)&uctsk_Rfms_s_DayInfo       ,sizeof(uctsk_Rfms_s_DayInfo));
        MODULE_OS_DELAY_MS(500);
        DebugOutHex((char*)"uctsk_Rfms_s_DaySleepLevel=" ,(uint8_t*)&uctsk_Rfms_s_DaySleepLevel ,sizeof(uctsk_Rfms_s_DaySleepLevel));
        MODULE_OS_DELAY_MS(500);
        DebugOutHex((char*)"uctsk_Rfms_s_DayBreath="     ,(uint8_t*)&uctsk_Rfms_s_DayBreath     ,sizeof(uctsk_Rfms_s_DayBreath));
        MODULE_OS_DELAY_MS(500);
        DebugOutHex((char*)"uctsk_Rfms_s_HeartRate="     ,(uint8_t*)&uctsk_Rfms_s_HeartRate     ,sizeof(uctsk_Rfms_s_HeartRate));
        MODULE_OS_DELAY_MS(500);
        DebugOutHex((char*)"uctsk_Rfms_s_BodyMove="      ,(uint8_t*)&uctsk_Rfms_s_BodyMove      ,sizeof(uctsk_Rfms_s_BodyMove));
        MODULE_OS_DELAY_MS(500);
        //�ͷŻ���
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
*/
/*******************************************************************************
��������: ����������
*******************************************************************************/
//Debug�������ݴ�ӡ
void uctsk_Rfms_DataStream_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Uctsk_Rfms_Data_DebugTest_Enable=ON;
    }
    else
    {
        Uctsk_Rfms_Data_DebugTest_Enable=OFF;
    }
}
//RFMS���δ�ӡ
void uctsk_Rfms_WaveStream_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Uctsk_Rfms_Wave_DebugTest_Enable=ON;
    }
    else
    {
        Uctsk_Rfms_Wave_DebugTest_Enable=OFF;
    }
}
//��������ӡ
void uctsk_Rfms_FinalData_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf,*pbuf1;
    MODULE_MEMORY_S_DAY_INFO *pdayInfo;
    uint8_t i;
    OnOff=OnOff;
    //���뻺��
    pbuf    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pbuf1   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    for(i=0; i<=6; i++)
    {
        //��ȡ����
        sprintf((char*)pbuf1,"----------(%d)----------\r\n",i);
        DebugOutStr((int8_t*)pbuf1);
        MODULE_OS_DELAY_MS(100);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)pbuf,&i);
        pdayInfo = (MODULE_MEMORY_S_DAY_INFO*)pbuf;
        if(pdayInfo->BodyMoveNum!=0x00 && pdayInfo->BodyMoveNum!=0xFF)
        {
            sprintf((char*)pbuf1,\
                    "Begin Time:%04d-%02d-%02d %02d:%02d:%02d\r\nEnd   Time:%04d-%02d-%02d %02d:%02d:%02d Move Count:%d\r\n",\
                    Count_2ByteToWord(pdayInfo->BeginTime[0],pdayInfo->BeginTime[1]),\
                    pdayInfo->BeginTime[2],\
                    pdayInfo->BeginTime[3],\
                    pdayInfo->BeginTime[4],\
                    pdayInfo->BeginTime[5],\
                    pdayInfo->BeginTime[6],\
                    Count_2ByteToWord(pdayInfo->EndTime[0],pdayInfo->EndTime[1]),\
                    pdayInfo->EndTime[2],\
                    pdayInfo->EndTime[3],\
                    pdayInfo->EndTime[4],\
                    pdayInfo->EndTime[5],\
                    pdayInfo->EndTime[6],\
                    pdayInfo->BodyMoveNum);
            DebugOutStr((int8_t*)pbuf1);
            MODULE_OS_DELAY_MS(100);
        }
        DebugOutHex((char*)"DayInfo=\r\n"       ,(uint8_t*)pbuf     ,sizeof(MODULE_MEMORY_S_DAY_INFO));
        MODULE_OS_DELAY_MS(100);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,(uint8_t*)pbuf,&i);
        DebugOutHex((char*)"DaySleepLevel=\r\n" ,(uint8_t*)pbuf     ,sizeof(MODULE_MEMORY_S_DAY_SLEEPLEVEL));
        MODULE_OS_DELAY_MS(100);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_R,(uint8_t*)pbuf,&i);
        DebugOutHex((char*)"DayBreath=\r\n"     ,(uint8_t*)pbuf     ,sizeof(MODULE_MEMORY_S_DAY_BREATH));
        MODULE_OS_DELAY_MS(100);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,(uint8_t*)pbuf,&i);
        DebugOutHex((char*)"HeartRate=\r\n"     ,(uint8_t*)pbuf     ,sizeof(MODULE_MEMORY_S_DAY_HEARTRATE));
        MODULE_OS_DELAY_MS(100);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,(uint8_t*)pbuf,&i);
        DebugOutHex((char*)"BodyMove=\r\n"      ,(uint8_t*)pbuf      ,sizeof(MODULE_MEMORY_S_DAY_BODYMOVE));
        MODULE_OS_DELAY_MS(100);
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
    //
    {
        struct tm *ptm;
        pbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pbuf1  =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
        ptm    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //��24Сʱ��������
        for(i=0; i<=6; i++)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)pbuf,&i);
            pdayInfo = (MODULE_MEMORY_S_DAY_INFO*)pbuf;
            if(pdayInfo->BodyMoveNum==0x00 || pdayInfo->BodyMoveNum==0xFF)
            {
                continue;
            }
            ptm->tm_year   =  Count_2ByteToWord(pdayInfo->BeginTime[0],pdayInfo->BeginTime[1]);
            ptm->tm_mon    =  pdayInfo->BeginTime[2]-1;
            ptm->tm_mday   =  pdayInfo->BeginTime[3];
            ptm->tm_hour   =  UCTSK_RFMS_KEEP_BODY_MOVE_TIME;
            ptm->tm_min    =  0;
            ptm->tm_sec    =  0;

            sprintf((char*)pbuf1,\
                    "Index %d\r\nBegin Time:%04d-%02d-%02d %02d:%02d:%02d\r\nEnd   Time:%04d-%02d-%02d %02d:%02d:%02d Move Count:%d\r\n",\
                    i,\
                    Count_2ByteToWord(pdayInfo->BeginTime[0],pdayInfo->BeginTime[1]),\
                    pdayInfo->BeginTime[2],\
                    pdayInfo->BeginTime[3],\
                    pdayInfo->BeginTime[4],\
                    pdayInfo->BeginTime[5],\
                    pdayInfo->BeginTime[6],\
                    Count_2ByteToWord(pdayInfo->EndTime[0],pdayInfo->EndTime[1]),\
                    pdayInfo->EndTime[2],\
                    pdayInfo->EndTime[3],\
                    pdayInfo->EndTime[4],\
                    pdayInfo->EndTime[5],\
                    pdayInfo->EndTime[6],\
                    pdayInfo->BodyMoveNum);
            DebugOutStr((int8_t*)pbuf1);
            MODULE_OS_DELAY_MS(100);
            // ��ȡ�嶯ֵ
            if(OK==uctsk_Rfms_KeepBodyMoveIndex(ptm,pbuf1,NULL))
            {
                DebugOutHex((char*)"24 Hour Data=\r\n"     ,(uint8_t*)pbuf1     ,480);
            }
            else
            {
                DebugOutStr("24 Hour Data=Err\r\n");
            }
            MODULE_OS_DELAY_MS(100);
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf1);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
    }
}
//����ԭʼ�����ļ��е�����
void uctsk_Rfms_RunFileData_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        if(Uctsk_Rfms_RunFileData_DebugTest_Enable==OFF && uctsk_Rfms_ExtFlashReadSign==0)
        {
            Uctsk_Rfms_RunFileData_DebugTest_Enable =  ON;
            GprsNet_OnOff(OFF);
            uctsk_Rfms_SetOnOff(ON,0);
        }
    }
    else
    {
        if(Uctsk_Rfms_RunFileData_DebugTest_Enable==ON)
        {
            if(BSP_PWR_E_POWER_AC==Hci_Power)
            {
                GprsNet_OnOff(ON);
            }
            uctsk_Rfms_SetOnOff(OFF,0);
            Uctsk_Rfms_RunFileData_DebugTest_Enable =  OFF;
            uctsk_Rfms_ExtFlashReadSign=1;
        }
    }
}
/*******************************************************************************
��������: Rfms���񴴽�
*******************************************************************************/
void  App_RfmsTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task Rfms",\
                          uctsk_Rfms,\
                          APP_TASK_RFMS_PRIO,\
                          App_TaskRfmsStk,\
                          APP_TASK_RFMS_STK_SIZE,\
                          AppTaskRfmsTCB,\
                          NULL);
}
void  App_RfmsTaskTestCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task Rfms Test",\
                          uctsk_RfmsTest,\
                          APP_TASK_RFMS_TEST_PRIO,\
                          App_TaskRfmsTestStk,\
                          APP_TASK_RFMS_TEST_STK_SIZE,\
                          AppTaskRfmsTestTCB,\
                          NULL);
}
void  App_RfmsTaskTxCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task Rfms Tx",\
                          uctsk_RfmsTx,\
                          APP_TASK_RFMS_TX_PRIO,\
                          App_TaskRfmsTxStk,\
                          APP_TASK_RFMS_TX_STK_SIZE,\
                          AppTaskRfmsTxTCB,\
                          NULL);
}
/*******************************************************************************
��������: Rfms��������ʵ��
*******************************************************************************/
uint8_t dataInitCnt=0;
uint32_t UctskRfms_TestfileDataNum=0;
uint32_t UctskRfms_TestfileDataAllByteNum=0;
static void uctsk_RfmsTest(void *pvParameters)
{
    static uint16_t s16=0;
    uint8_t *pbuf=NULL;
    uint8_t step=0;
    MODULE_OS_ERR err;
    MODULE_OS_SEM_CREATE(UctskRfms_Sem_GetFileData,"UctskRfms_Sem_GetFileData",0);
    for(;;)
    {
        MODULE_OS_SEM_PEND(UctskRfms_Sem_GetFileData,1000,TRUE,err);
        if(err == MODULE_OS_ERR_NONE)
        {
            if(pbuf==NULL)
            {
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
                memset(pbuf,0,5*1024);
                //��ȡ�ļ������ֽ���
                if(ERR==SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_GETSIZE,"/test.adc",NULL,&UctskRfms_TestfileDataAllByteNum,NULL,NULL))
                {
                    UctskRfms_TestfileDataAllByteNum=0;
                }
#endif
            }
            //��һ��Ҫ��,ģ�ⷢ��RFMS����
            if(step==0)
            {
                step=1;
                pbuf[0]=UCTSK_RFMS_POWER_ON[0];
                pbuf[1]=UCTSK_RFMS_POWER_ON[1];
                pbuf[2]=UCTSK_RFMS_POWER_ON[2];
                pbuf[3]=UCTSK_RFMS_POWER_ON[3];
                uctsk_Rfms_RxIrq(&pbuf[0],4);
                UctskRfms_TestfileDataNum=0;
                continue;
            }
            //�ڶ���Ҫ��,����6000��0����,ģ��RFMS��ʼ��
            else if(step==1)
            {
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                UctskRfms_TestfileDataNum=0;
                step=2;
#endif
                continue;
            }
            //��ȡFlash����
            if(UctskRfms_TestfileDataNum<UctskRfms_TestfileDataAllByteNum)
            {
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                uint16_t i16=2;
                uint32_t i32;
                if(UctskRfms_TestfileDataNum%(5*1024)==0)
                {
                    //��ȡ��������
                    i16 = (5*1024);
                    SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_R,"/test.adc",pbuf,&UctskRfms_TestfileDataNum,&i16,&i32);
                }
                s16 = UctskRfms_TestfileDataNum%(5*1024);
                uctsk_Rfms_RxIrq(&pbuf[s16],2);
#endif
                UctskRfms_TestfileDataNum+=2;
            }
            else
            {
                uctsk_Rfms_RunFileData_DebugTestOnOff(OFF);
                //�¶�ʪ�ȸ�ֵ
                uctsk_Rfms_s_DayInfo.TempBuf[0]=20;
                uctsk_Rfms_s_DayInfo.HumiBuf[0]=50;
            }

        }
        else if(err == MODULE_OS_ERR_TIMEOUT)
        {
            dataInitCnt++;
            if(dataInitCnt>=10)
            {
                dataInitCnt=0;
                UctskRfms_TestfileDataNum=0;
                if(pbuf!=NULL)
                {
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pbuf);
#endif
                    pbuf=NULL;
                    step=0;
                }
            }
            // ʡ�翪����ر��ж�
            if(Uctsk_Rfms_RunState==UCTSK_RFMS_E_RUNSTATE_IDLE\
               && uctsk_Rfms_PowerSavingOff==0)
            {
                // �а�������,�򱳹����
                if(Uctsk_Rfms_KeyCountBak!=BspKey_Count)
                {
                    Uctsk_Rfms_KeyCountBak         =  BspKey_Count;
                    Uctsk_Rfms_PowerSavingTimer_s  =  UCTSK_RFMS_POWER_SAVING_ON_TIME_S;
                }
                // ����ָ�
                if(Uctsk_Rfms_PowerSavingTimer_s==UCTSK_RFMS_POWER_SAVING_ON_TIME_S)
                {
                    MODULE_MEMORY_S_PARA *pPara;
                    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                    if(pPara->LcdBackLight1!=Uctsk_Rfms_PowerSaving_Backlight)
                    {
                        Uctsk_Rfms_PowerSaving_Backlight=pPara->LcdBackLight1;
                    }
                    if(Uctsk_Rfms_PowerSaving_Backlight!=LCD_GetBackLight())
                    {
                        LCD_SetBackLight(Uctsk_Rfms_PowerSaving_Backlight);
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
                    Uctsk_Rfms_PowerSavingTimer_s--;
                }
                // ������Ϣ����,�ұ���Ϩ��
                else if(Uctsk_Rfms_PowerSavingTimer_s==1)
                {
                    Uctsk_Rfms_PowerSavingTimer_s--;
                    Uctsk_Rfms_PowerSaving_Backlight=LCD_GetBackLight();
                    LCD_SetBackLight(1);
                }
                // ����ʱ�����
                else if(Uctsk_Rfms_PowerSavingTimer_s!=0)
                {
                    Uctsk_Rfms_PowerSavingTimer_s--;
                }
            }
        }
        else
        {
            ;
        }
    }
}
/*******************************************************************************
��������: Rfms������ʵ��
*******************************************************************************/
static void uctsk_RfmsTx (void *pvParameters)
{
    MODULE_OS_ERR err;
    MODULE_OS_SEM_CREATE(UctskRfms_Sem_Tx,"UctskRfms_Sem_Tx",0);
    for(;;)
    {
        MODULE_OS_SEM_PEND(UctskRfms_Sem_Tx,0,TRUE,err);
        if(err == MODULE_OS_ERR_NONE)
        {
            uctsk_Rfms_Tx((uint8_t*)UCTSK_RFMS_DATA_GET,sizeof(UCTSK_RFMS_DATA_GET));
            uctsk_Rfms_TxNum++;
        }
    }
}
/*******************************************************************************
��������: Rfms�������ʵ��
*******************************************************************************/
static void uctsk_Rfms (void *pvParameters)
{
    static uint16_t s_count=0;
    static uint8_t buf[20];
    static uint16_t *ps16=NULL;
    static uint8_t step=0;
    static uint32_t txNumBak=0;
    static uint32_t rxNumBak=0;
    uint8_t i;
    uint16_t len;
    uint16_t i16;
    uint32_t i32;
    MODULE_OS_ERR err;
    MODULE_OS_SEM_CREATE(UctskRfms_Sem_Rx,"UctskRfms_Sem_Rx",0);
    //�������ϵ�
    /*
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        UCTSK_RFMS_POWER_RCC_ENABLE;
    #if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
    #elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
    #endif
        GPIO_InitStructure.GPIO_Pin     =  UCTSK_RFMS_POWER_PIN;
        GPIO_Init(UCTSK_RFMS_POWER_PORT, &GPIO_InitStructure);
        UCTSK_RFMS_POWER_PIN_ON;
    }
    */
    //�������ӿڳ�ʼ��
    UART_INIT(UCTSK_RFMS_UARTX,UCTSK_RFMS_UART_BPS);
    //��ʼ�����ݽṹ
    Count_BufferLoopInit(&UctskRfms_BufferLoop,\
                         UctskRfms_BufferLoop_Buf,\
                         UCTSK_RFMS_BUFFERLOOP_BUF_MAX,\
                         UctskRfms_BufferLoop_Node,\
                         UCTSK_RFMS_BUFFERLOOP_NODE_MAX,\
                         &UctskRfms_BufferLoop_Limit);
    Count_BufferLoopInit(&UctskRfms_BufferLoop1,\
                         UctskRfms_BufferLoop1_Buf,\
                         UCTSK_RFMS_BUFFERLOOP1_BUF_MAX,\
                         UctskRfms_BufferLoop1_Node,\
                         UCTSK_RFMS_BUFFERLOOP1_NODE_MAX,\
                         &UctskRfms_BufferLoop1_Limit);
    memset((char*)&uctsk_Rfms_s_DayInfo,0,sizeof(uctsk_Rfms_s_DayInfo));
    memset((char*)&uctsk_Rfms_s_DaySleepLevel,0,sizeof(uctsk_Rfms_s_DaySleepLevel));
    memset((char*)&uctsk_Rfms_s_DayBreath,0,sizeof(uctsk_Rfms_s_DayBreath));
    memset((char*)&uctsk_Rfms_s_HeartRate,0,sizeof(uctsk_Rfms_s_HeartRate));
    memset((char*)&uctsk_Rfms_s_BodyMove,0,sizeof(uctsk_Rfms_s_BodyMove));
    //����������
    UCTSK_RFMS_ON;
    //�ȴ��������װ�����
    MODULE_OS_DELAY_MS(1000);
    Uctsk_Rfms_PowerSaving_Backlight=LCD_GetBackLight();
    //װ����ֵ
    {
        MODULE_MEMORY_S_PARA *pPara;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        uctsk_Rfms_NoBodyThresholdValue   =  pPara->ThreshsholdValue1;
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
    //������ѭ��
    for(;;)
    {
        //-----------------------
        MODULE_OS_SEM_PEND(UctskRfms_Sem_Rx,1000,TRUE,err);
        // ������
        if(txNumBak==uctsk_Rfms_TxNum)
        {
            uctsk_Rfms_Err=0;
        }
        else
        {
            txNumBak=uctsk_Rfms_TxNum;
            if(rxNumBak==uctsk_Rfms_RxNum)
            {
                uctsk_Rfms_Err=1;
            }
            else
            {
                rxNumBak=uctsk_Rfms_RxNum;
                uctsk_Rfms_Err=0;
            }
        }
        // ����RFMS����
        if(err == MODULE_OS_ERR_NONE)
        {
            //��ȡ����
            Count_BufferLoopPop(&UctskRfms_BufferLoop,buf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
            //��������
            if(len==2)
            {
                //����mVֵ
                uctsk_Rfms_RxValue=Count_2ByteToWord(buf[0],buf[1]);
                if(Uctsk_Rfms_RunState==UCTSK_RFMS_E_RUNSTATE_IDLE)
                {
                    if(ps16==NULL)
                    {
                        ps16=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        memset((char*)ps16,0,256);
                        ps16[100]=0;
                    }
                    i16=abs(uctsk_Rfms_RxValue-UCTSK_RFMS_DATA_DC);
                    //ѭ����ֵ
                    ps16[ps16[100]]=i16;
                    if(ps16[100]<100)
                    {
                        ps16[100]++;
                    }
                    else
                    {
                        ps16[100]=0;
                    }
                    //�����ۼӺ�
                    i32 = Count_Sum16(0,ps16,100);
                    if(i32>UCTSK_RFMS_POWER_SAVING_MAX_VALUE)
                    {
                        Uctsk_Rfms_PowerSavingTimer_s=UCTSK_RFMS_POWER_SAVING_ON_TIME_S;
                    }
                }
                else
                {
                    if(ps16!=NULL)
                    {
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps16);
                        ps16=NULL;
                    }
                }
                //�˶���������
                if(1)
                {
                    Arithmetic_MovePower(Uctsk_Rfms_RunState,uctsk_Rfms_RxValue);
                }
                //
                if(Uctsk_Rfms_RunState==UCTSK_RFMS_E_RUNSTATE_GET)
                {
                    Arithmetic_BodyMove(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Arithmetic_Breath(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Arithmetic_HeartRate(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Uctsk_RfmsTempHumi(UCTSK_RFMS_E_RUNSTATE_GET);
                    Arithmetic_SmartAlarm(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    uctsk_Rfms_DataSaveToTf(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                }
                else if(Uctsk_Rfms_RunState==UCTSK_RFMS_E_RUNSTATE_START_DELAY)
                {
                    //-----����ʱ��ͳ��Ҫ�߶ȱ���һ��,ǰһ��������ҲҪ�������
                    Arithmetic_BodyMove(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Arithmetic_Breath(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Arithmetic_HeartRate(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    Uctsk_RfmsTempHumi(UCTSK_RFMS_E_RUNSTATE_GET);
                    Arithmetic_SmartAlarm(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    uctsk_Rfms_DataSaveToTf(UCTSK_RFMS_E_RUNSTATE_GET,uctsk_Rfms_RxValue);
                    //-----
                    s_count++;
                    if(s_count>100*60)
                    {
                        Uctsk_Rfms_RunState = UCTSK_RFMS_E_RUNSTATE_GET;
                    }
                }
                //ͳ��
                if(Uctsk_Rfms_RunState>=UCTSK_RFMS_E_RUNSTATE_START&& Uctsk_Rfms_RunState<=UCTSK_RFMS_E_RUNSTATE_GET)
                {
                    uctsk_Rfms_MonitorRxNum++;
                    //12Сʱ�Զ�����
                    if(uctsk_Rfms_MonitorRxNum>=(12*3600*100))
                    {
                        //������
                        /*
                        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BLB,0);
                        */
                        if(BSP_PWR_E_POWER_AC==Hci_Power)
                        {
                            GprsNet_OnOff(ON);
                        }
                        uctsk_Rfms_SetOnOff(OFF,1);
                    }
                }
                //���ռ����ۼ�
                uctsk_Rfms_RxNum++;
                //���չ�������0ֵ,���ٴ�����RFMS
                if(uctsk_Rfms_RxValue==0)
                {
                    Uctsk_Rfms_RxZeroCmt++;
                    if(Uctsk_Rfms_RxZeroCmt>=100*30)
                    {
                        MODULE_OS_DELAY_MS(1000);
                        UCTSK_RFMS_ON;
                        MODULE_OS_DELAY_MS(1000);
                        Uctsk_Rfms_RxZeroCmt=0;
                        Uctsk_Rfms_RxZeroCmtInfo++;
                    }
                }
                else
                {
                    Uctsk_Rfms_RxZeroCmt=0;
                }
            }
            else if(len==4)
            {
                ;
            }
            else
            {
                //���մ�����
            }
            if(Uctsk_Rfms_RunFileData_DebugTest_Enable == ON)
            {
                uctsk_Rfms_TxNum++;
                //������Ϣ
                MODULE_OS_SEM_POST(UctskRfms_Sem_GetFileData);
            }
        }
        //˯�����ݽ�������
        {
            if(Uctsk_Rfms_RunState==UCTSK_RFMS_E_RUNSTATE_STOP&&step==0)
            {
                step=1;
            }
            if(step==1)
            {
                step++;
                Arithmetic_BodyMove(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            }
            else if(step==2)
            {
                step++;
                Arithmetic_Breath(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            }
            else if(step==3)
            {
                step++;
                Arithmetic_HeartRate(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            }
            else if(step==4)
            {
                step++;
                Uctsk_RfmsTempHumi(UCTSK_RFMS_E_RUNSTATE_STOP);
            }
            else if(step==5)
            {
                step++;
                Arithmetic_SmartAlarm(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            }
            else if(step==6)
            {
                step++;
                uctsk_Rfms_DataSaveToTf(UCTSK_RFMS_E_RUNSTATE_STOP,0);
            }
            else if(step==7)
            {
                //�������
                uctsk_CurrentSleepLevel=0;
                //
                if(uctsk_Rfms_s_DayInfo.BodyMoveNum==0)
                {
                    memset((char*)&uctsk_Rfms_s_DayInfo,0,sizeof(uctsk_Rfms_s_DayInfo));
                    memset((char*)&uctsk_Rfms_s_DaySleepLevel,0,sizeof(uctsk_Rfms_s_DaySleepLevel));
                    memset((char*)&uctsk_Rfms_s_DayBreath,0,sizeof(uctsk_Rfms_s_DayBreath));
                    memset((char*)&uctsk_Rfms_s_HeartRate,0,sizeof(uctsk_Rfms_s_HeartRate));
                    memset((char*)&uctsk_Rfms_s_BodyMove,0,sizeof(uctsk_Rfms_s_BodyMove));
                }
                else
                {
                    //���ݸ�ֵ
                    uctsk_Rfms_s_DayInfo.ProtocolVer        =  MODULE_MEMORY_FORMAT_VER;
                    uctsk_Rfms_s_DayInfo.DeviceType         =  MODULE_MEMORY_DEVICE_TYPE;
                    //����ʱ��
                    BspRtc_ReadRealTime(NULL,NULL,NULL,uctsk_Rfms_s_DayInfo.EndTime);
                    uctsk_Rfms_s_DayInfo.EndTime[6]=uctsk_Rfms_s_DayInfo.EndTime[5];
                    uctsk_Rfms_s_DayInfo.EndTime[5]=uctsk_Rfms_s_DayInfo.EndTime[4];
                    uctsk_Rfms_s_DayInfo.EndTime[4]=uctsk_Rfms_s_DayInfo.EndTime[3];
                    uctsk_Rfms_s_DayInfo.EndTime[3]=uctsk_Rfms_s_DayInfo.EndTime[2];
                    uctsk_Rfms_s_DayInfo.EndTime[2]=uctsk_Rfms_s_DayInfo.EndTime[1];
                    uctsk_Rfms_s_DayInfo.EndTime[1]=(uint8_t)(uctsk_Rfms_s_DayInfo.EndTime[0]+2000);
                    uctsk_Rfms_s_DayInfo.EndTime[0]=(uint8_t)((uctsk_Rfms_s_DayInfo.EndTime[0]+2000)>>8);
                    //����,ǳ˯,��˯,��˯ʱ��,��˯ʱ��
                    for(len=i=0; len<240; len++)
                    {
                        if(1==uctsk_Rfms_s_DaySleepLevel.DataBuf[len])
                        {
                            //����
                            i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.WakeLevelTime_m[0],uctsk_Rfms_s_DayInfo.WakeLevelTime_m[1]);
                            i16+=3;
                            uctsk_Rfms_s_DayInfo.WakeLevelTime_m[0]=i16>>8;
                            uctsk_Rfms_s_DayInfo.WakeLevelTime_m[1]=i16;
                            if(i!=0xFF)
                            {
                                i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0],uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]);
                                i16+=3;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0]=i16>>8;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]=i16;
                                i=0;
                            }
                        }
                        else if(2==uctsk_Rfms_s_DaySleepLevel.DataBuf[len])
                        {
                            //ǳ˯
                            i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.LightLevelTime_m[0],uctsk_Rfms_s_DayInfo.LightLevelTime_m[1]);
                            i16+=3;
                            uctsk_Rfms_s_DayInfo.LightLevelTime_m[0]=i16>>8;
                            uctsk_Rfms_s_DayInfo.LightLevelTime_m[1]=i16;
                            if(i!=0xFF)
                            {
                                i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0],uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]);
                                i16+=3;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0]=i16>>8;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]=i16;
                                i=0;
                            }
                        }
                        else if(3==uctsk_Rfms_s_DaySleepLevel.DataBuf[len])
                        {
                            //��˯
                            i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.MiddleLevelTime_m[0],uctsk_Rfms_s_DayInfo.MiddleLevelTime_m[1]);
                            i16+=3;
                            uctsk_Rfms_s_DayInfo.MiddleLevelTime_m[0]=i16>>8;
                            uctsk_Rfms_s_DayInfo.MiddleLevelTime_m[1]=i16;
                            //
                            if(i!=0xFF)
                            {
                                i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0],uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]);
                                i16+=3;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0]=i16>>8;
                                uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]=i16;
                                i++;
                                if(i>=4)
                                {
                                    i=0xFF;
                                    i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0],uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]);
                                    i16-=12;
                                    uctsk_Rfms_s_DayInfo.FallAsleepTime_m[0]=i16>>8;
                                    uctsk_Rfms_s_DayInfo.FallAsleepTime_m[1]=i16;
                                }
                            }
                        }
                        else if(4==uctsk_Rfms_s_DaySleepLevel.DataBuf[len])
                        {
                            //��˯
                            i16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.DeepLevelTime_m[0],uctsk_Rfms_s_DayInfo.DeepLevelTime_m[1]);
                            i16+=3;
                            uctsk_Rfms_s_DayInfo.DeepLevelTime_m[0]=i16>>8;
                            uctsk_Rfms_s_DayInfo.DeepLevelTime_m[1]=i16;
                            //
                            i=0xFF;
                        }
                    }
                    //����
                    {
                        SLEEP_S_MARK *psMark;
                        //���뻺��
                        psMark = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        Arithmetic_SleepMarkInfoLoad(&uctsk_Rfms_s_DayInfo,&uctsk_Rfms_s_DaySleepLevel,psMark);
                        Arithmetic_SleepMark(psMark);
                        uctsk_Rfms_s_DayInfo.Scoring = psMark->Mart_All;
                        //
                        uctsk_Rfms_s_DayInfo.Mark_SleepStruct           =  psMark->Mark_SleepStruct;
                        uctsk_Rfms_s_DayInfo.Mart_SleepIntoPeriod       =  psMark->Mart_SleepIntoPeriod;
                        uctsk_Rfms_s_DayInfo.Mark_BodyMovement          =  psMark->Mark_BodyMovement;
                        uctsk_Rfms_s_DayInfo.Mark_WakeState             =  psMark->Mark_WakeState;
                        uctsk_Rfms_s_DayInfo.Mark_BeginSleepTimeChange  =  psMark->Mark_BeginSleepTimeChange;
                        uctsk_Rfms_s_DayInfo.Mark_Temp                  =  psMark->Mark_Temp;
                        uctsk_Rfms_s_DayInfo.Mark_Humi                  =  psMark->Mark_Humi;
                        uctsk_Rfms_s_DayInfo.Mart_TotalSleep            =  psMark->Mart_TotalSleep;
                        //�ͷŻ���
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psMark);
                    }
                    //���ݽ���
                    uctsk_Rfms_s_DayInfo.Protocol_BeginByte  =  0x00;
                    uctsk_Rfms_s_DayInfo.Protocol_Len        =  0x48;
                    uctsk_Rfms_s_DayInfo.BeginByte           =  0xCC;
                    uctsk_Rfms_s_DayInfo.EndByte             =  0xAA;
                    uctsk_Rfms_s_DaySleepLevel.Protocol_BeginByte  =  0x01;
                    uctsk_Rfms_s_DaySleepLevel.Protocol_Len        =  0xF3;
                    uctsk_Rfms_s_DaySleepLevel.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum]  =  0xA5;
                    uctsk_Rfms_s_DayBreath.Protocol_BeginByte      =  0x02;
                    uctsk_Rfms_s_DayBreath.Protocol_Len            =  0xF3;
                    uctsk_Rfms_s_DayBreath.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum]      =  0xB5;
                    if(uctsk_Rfms_s_DayBreath.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-1]==0)
                    {
                        uctsk_Rfms_s_DayBreath.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-1]=uctsk_Rfms_s_DayBreath.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-2];
                    }
                    uctsk_Rfms_s_HeartRate.Protocol_BeginByte      =  0x03;
                    uctsk_Rfms_s_HeartRate.Protocol_Len            =  0xF3;
                    uctsk_Rfms_s_HeartRate.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum]      =  0xE5;
                    if(uctsk_Rfms_s_HeartRate.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-1]==0)
                    {
                        uctsk_Rfms_s_HeartRate.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-1]=uctsk_Rfms_s_HeartRate.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum-2];
                    }
                    uctsk_Rfms_s_BodyMove.Protocol_BeginByte       =  0x04;
                    uctsk_Rfms_s_BodyMove.Protocol_Len             =  0xF3;
                    uctsk_Rfms_s_BodyMove.DataBuf[uctsk_Rfms_s_DayInfo.BodyMoveNum]       =  0xFC;
                    //��γ�ȸ�ֵ
                    uctsk_Rfms_s_DayInfo.GPS_Latitude    =  GprsNet_s_Info.GPS_Latitude;
                    uctsk_Rfms_s_DayInfo.GPS_Longitude   =  GprsNet_s_Info.GPS_Longitude;
                    uctsk_Rfms_s_DayInfo.Gsm_LacCi[0]    =  GprsNet_s_Info.LacCi[0];
                    uctsk_Rfms_s_DayInfo.Gsm_LacCi[1]    =  GprsNet_s_Info.LacCi[1];
                    uctsk_Rfms_s_DayInfo.Gsm_LacCi[2]    =  GprsNet_s_Info.LacCi[2];
                    uctsk_Rfms_s_DayInfo.Gsm_LacCi[3]    =  GprsNet_s_Info.LacCi[3];
                    //������Ч���ж�
                    i=0;
                    for(i16=0; i16<240; i16++) //���᷶ΧΪ0-239,��240�����ص�
                    {
                        if((uctsk_Rfms_s_DaySleepLevel.DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)\
                           ||(uctsk_Rfms_s_DaySleepLevel.DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)\
                           ||(uctsk_Rfms_s_DaySleepLevel.DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE))
                        {
                            i=0;
                        }
                        else if(uctsk_Rfms_s_DaySleepLevel.DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                        {
                            i++;
                            if(i>=30)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    if((Count_2ByteToWord(uctsk_Rfms_s_DayInfo.BodyMoveCount[0],uctsk_Rfms_s_DayInfo.BodyMoveCount[1])==0 && uctsk_Rfms_s_DayInfo.BodyMoveNum>30)\
                       ||(Count_2ByteToWord(uctsk_Rfms_s_DayInfo.BodyMoveCount[0],uctsk_Rfms_s_DayInfo.BodyMoveCount[1])>=66)\
                       ||i>=30)
                    {
                        uctsk_Rfms_s_DayInfo.Flag_DataValid=1;
                    }
                    else
                    {
                        uctsk_Rfms_s_DayInfo.Flag_DataValid=0;
                    }
                    //������ʽ
                    //uctsk_Rfms_s_DayInfo.Flag_StartMode  =  0;
                    //ֹͣ��ʽ
                    //uctsk_Rfms_s_DayInfo.Flag_StopMode   =  0;
                    //���ݴ洢
                    {
                        MODULE_MEMORY_S_PARA *pPara;
                        uint8_t res;
                        pPara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                        res=pPara->MemoryDayNextP;
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_W      ,(uint8_t*)&uctsk_Rfms_s_DayInfo       ,  &res);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_W,(uint8_t*)&uctsk_Rfms_s_DaySleepLevel ,  &res);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_W    ,(uint8_t*)&uctsk_Rfms_s_DayBreath     ,  &res);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_W ,(uint8_t*)&uctsk_Rfms_s_HeartRate     ,  &res);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_W  ,(uint8_t*)&uctsk_Rfms_s_BodyMove      ,  &res);
                        pPara->MemoryDayNextP++;
                        if(pPara->MemoryDayNextP >= MODULE_MEMORY_ADDR_DAY_NUM)
                        {
                            pPara->MemoryDayNextP=0;
                        }
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
                    }
                    //��������
                    Bluetooth_App(BLUETOOTH_CMD_UPDATA_1DAY,NULL);
                    GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN);
                }
                Uctsk_Rfms_RunState = UCTSK_RFMS_E_RUNSTATE_IDLE;
                step =  0;
            }
            //��ȡ�ļ�����
            if(Uctsk_Rfms_RunFileData_DebugTest_Enable == ON)
            {
                //������Ϣ
                MODULE_OS_SEM_POST(UctskRfms_Sem_GetFileData);
            }
        }
    }
}
/*******************************************************************************
��������: ��ʪ��
*******************************************************************************/
static void Uctsk_RfmsTempHumi(uint8_t step)
{
    //ms��ʱ��
    static uint32_t s_count_ms=0;
    //uint8_t i;
    //int16_t temp,temp1;
    //uint16_t humi,humi1;
    uint32_t i32;
    s_count_ms+=10;
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_START:
            memset((char*)&uctsk_Rfms_s_DayInfo.TempBuf,0xFF,sizeof(uctsk_Rfms_s_DayInfo.TempBuf));
            memset((char*)&uctsk_Rfms_s_DayInfo.HumiBuf,0xFF,sizeof(uctsk_Rfms_s_DayInfo.HumiBuf));
            s_count_ms=0;
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            if(s_count_ms>=15*60*1000L)
            {
                i32=s_count_ms-(15*60*1000L);
                if(i32%(3600*1000L)==0)
                {
                    i32=i32/(3600*1000L);
                    /*
                    i=3;
                    while(i--)
                    {
                        BspTempHumidSensor_Read(&temp,&humi);
                        BspTempHumidSensor_Read(&temp1,&humi1);
                        if((3>=abs(temp-temp1))&&(3>=abs(humi-humi1)))
                        {
                            break;
                        }
                    }
                    */
                    uctsk_Rfms_s_DayInfo.TempBuf[i32]=Sensor_Temp/10;
                    uctsk_Rfms_s_DayInfo.HumiBuf[i32]=Sensor_Humi/10;
                }
            }
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            s_count_ms+=10;
            if(uctsk_Rfms_s_DayInfo.TempBuf[0]<=5)
            {
                uctsk_Rfms_s_DayInfo.TempBuf[0]=Sensor_Temp/10;
            }
            if(uctsk_Rfms_s_DayInfo.HumiBuf[0]<=5)
            {
                uctsk_Rfms_s_DayInfo.HumiBuf[0]=Sensor_Humi/10;
            }
            break;
        default:
            break;
    }
}
/*******************************************************************************
* ��������: ��������
*******************************************************************************/
static void Arithmetic_SmartAlarm(uint8_t step,uint16_t dat)
{
    //��һ�εĲ���ֵ
    static uint16_t s_dat_bak=0;
    //ms��ʱ��
    static uint32_t s_count_ms=0;
    //ÿ3600s�ĵ���������
    static uint32_t s_3600s_LowPowerSum=0;
    //ÿ3600s�ĵ���������
    static uint32_t s_3600s_LowPowerNum=0;
    //
    static uint32_t s_90sSum  =  0;
    //
    uint32_t   result=0;
    //
    //
    /*
    uint8_t i;
    uint8_t res;
    for(i=0; i<BSP_RTC_ALARM_MAX_NUM; i++)
    {
        BspRtc_ReadAlarm(i+1,NULL,&res);
        // ����������������
        if(res==2)
        {
            break;
        }
    }
    if(i>=BSP_RTC_ALARM_MAX_NUM)
    {
        return;
    }
    */
    //
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_IDLE:
            uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            break;
        case UCTSK_RFMS_E_RUNSTATE_START:
            s_count_ms                 =  0;
            s_3600s_LowPowerSum        =  0;
            s_3600s_LowPowerNum        =  0;
            s_90sSum                   =  0;
            uctsk_Rfms_s_DayInfo.SleepLevelBeforeAlarm   =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            break;
        case UCTSK_RFMS_E_RUNSTATE_START_DELAY:
            uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            //----------ֻ����12Сʱ���ڵ�����
            //�������12Сʱ
            if(s_count_ms>=(1000*60*60*12))
            {
                //����������
                break;
            }
            //���С��12Сʱ
            else
            {
                //�������ۼ�
                s_count_ms+=10;
            }
            //----------ȥ������(dat)
            //���ֵ�ж�
            if(dat>0x3FF)
            {
                dat = 0x3FF;
            }
            //�仯����,���˵�
            if(abs(s_dat_bak-dat)>0x1F0)
            {
                dat = s_dat_bak;
            }
            else
            {
                s_dat_bak=dat;
            }
            //----------ȥֱ��
            dat=abs(dat-UCTSK_RFMS_DATA_DC);
            //----------ÿ10ms����
            if(s_count_ms%10==0)
            {
                //���ݷֶ�
                if(dat<68)
                {
                    //���������
                    s_3600s_LowPowerSum+=dat;
                    s_3600s_LowPowerNum++;
                }
                //���ݻ���
                s_90sSum+=(dat*2);
            }
            if(s_count_ms%(90*1000)==0)
            {
                //��������100��
                s_90sSum*=100;
                //���������ݻ���ƽ��ֵ����չ
                if(s_3600s_LowPowerNum)
                {
                    s_3600s_LowPowerSum /= s_3600s_LowPowerNum;
                    s_3600s_LowPowerSum *= 2*(9000-1);
                }
                if(s_3600s_LowPowerSum==0)
                {
                    s_3600s_LowPowerSum = 1;
                }
                //����
                result=s_90sSum/s_3600s_LowPowerSum;
                //if(uctsk_Rfms_s_DayInfo.SleepLevelBeforeAlarm == MODULE_MEMORY_E_SLEEPLEVEL_NULL)
                {
                    if(result>ARITHMETIC_BODY_MOVE_LEVEL_WAKE)
                    {
                        uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_WAKE;
                    }
                    else if(result>ARITHMETIC_BODY_MOVE_LEVEL_LIGHT)
                    {
                        uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_LIGHT;
                    }
                    else if(result>ARITHMETIC_BODY_MOVE_LEVEL_MIDDLE)
                    {
                        uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE;
                    }
                    else
                    {
                        uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_DEEP;
                    }
                }
                //
                s_90sSum = 0;
                //
                s_3600s_LowPowerSum = 0;
                s_3600s_LowPowerNum = 0;
            }
            if(s_count_ms%(3600*1000)==0)
            {
                s_3600s_LowPowerSum=0;
                s_3600s_LowPowerNum=0;
            }
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            s_count_ms+=10;
            uctsk_CurrentSleepLevel =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            break;
        default:
            break;
    }
}
/*******************************************************************************
* ��������: �嶯�㷨(�������ڶ�ʱ����,ÿ10mSִ��һ��)
*******************************************************************************/
void Arithmetic_AI(unsigned char *s,unsigned char length);
static float DSCFilter(uint16_t dat,BiquadSections *filter_name);
//
static void Arithmetic_BodyMove(uint8_t step,uint16_t dat)
{
    //��һ�εĲ���ֵ
    static uint16_t s_dat_bak=0;
    //-----10ms����
    //ms��ʱ��
    static uint32_t s_count_ms=0;
    //ÿ90s������,��3600s/90s=40��Buf
    //static uint32_t ps_3600s_90sSumBuf[ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM]={0};
    static uint32_t *ps_3600s_90sSumBuf   =  NULL;
    //ÿ90s���е��������ݴ���,��3600s/90s=40��Buf
    //static uint32_t ps_3600s_90sMLNumBuf[ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM];
    static uint32_t *ps_3600s_90sMLNumBuf =  NULL;
    //ÿ3600s�ĵ���������
    static uint32_t s_3600s_LowPowerSum=0;
    static uint32_t s_3600s_LowPowerSumBuffer=0;
    //ÿ3600s�ĵ���������
    static uint32_t s_3600s_LowPowerNum=0;
    //ÿ3600s�ĸ���������
    static uint32_t s_3600s_HighPowerNum=0;
    //ÿ1800s���и���������
    static uint32_t s_1800s_MHPowerNum=0;
    //-----90s����
    //-----1800s����
    //ÿ1800s�ۼ�һ��,��¼12Сʱ�����嶯����
    static uint8_t s_43200s_1800sNotMoveNum=0;
    //-----3600s����
    //-----�ռ�����
    //ÿ1Сʱ���� 90s�Ľ��
    //static uint8_t   s_43200s_90sMoveBuf[ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM*12];
    static uint8_t   *ps_43200s_90sMoveBuf   =  NULL;
    static uint16_t  s_43200s_90sDataNum      =  0;
    //-----
    static uint8_t   *psbuf =  NULL;
    uint8_t    res;
    uint16_t   i16=0,j16=0;
    uint32_t   result=0;
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_IDLE:
            break;
        case UCTSK_RFMS_E_RUNSTATE_START:
            //���뻺��
            psbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
            ps_43200s_90sMoveBuf =  (uint8_t*)&psbuf[0];
            ps_3600s_90sMLNumBuf =  (uint32_t*)(&psbuf[ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM*12]);
            ps_3600s_90sSumBuf   =  (uint32_t*)(&psbuf[ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM*16]);
            //���ݳ�ʼ��
            s_dat_bak                  =  0;
            s_count_ms                 =  0;
            s_3600s_LowPowerSum        =  0;
            s_3600s_LowPowerNum        =  0;
            s_3600s_HighPowerNum       =  0;
            s_1800s_MHPowerNum         =  0;
            s_43200s_1800sNotMoveNum   =  0;
            s_43200s_90sDataNum        =  0;
            uctsk_Rfms_ExtFlashReadSign=  0;
            memset((char*)psbuf,0,1024);
            memset((char*)uctsk_Rfms_s_BodyMove.DataBuf,0,sizeof(uctsk_Rfms_s_BodyMove.DataBuf));
            //
            UctskRfms_BufferLoopAllErrCmt=0;
            UctskRfms_BufferLoopMaxErrCmt=0;
            //
            uctsk_Rfms_s_DayInfo.SleepLevelBeforeAlarm   =  MODULE_MEMORY_E_SLEEPLEVEL_NULL;
            break;
        case UCTSK_RFMS_E_RUNSTATE_START_DELAY:
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            //----------ֻ����12Сʱ���ڵ�����
            //�������12Сʱ
            if(s_count_ms>=(1000*60*60*12))
            {
                //����������
                break;
            }
            //���С��12Сʱ
            else
            {
                //�������ۼ�
                s_count_ms+=10;
            }
            //----------ȥ������(dat)
            //���ֵ�ж�
            if(dat>0x3FF)
            {
                dat = 0x3FF;
            }
            //�仯����,���˵�
            if(abs(s_dat_bak-dat)>0x1F0)
            {
                dat = s_dat_bak;
            }
            else
            {
                s_dat_bak=dat;
            }
            //----------ȥֱ��
            dat=abs(dat-UCTSK_RFMS_DATA_DC);
            //----------ÿ10ms����
            if(s_count_ms%10==0)
            {
                //���ݷֶ�
                if(dat<68)
                {
                    //���������
                    s_3600s_LowPowerSum+=dat;
                    s_3600s_LowPowerNum++;
                }
                else if(dat<150)
                {
                    //���ڼ�Ȩ
                    ps_3600s_90sMLNumBuf[(s_count_ms%(1000*3600))/(1000*90)]++;
                }
                else if(dat<477)
                {
                    //�ж��Ƿ�����
                    s_1800s_MHPowerNum++;
                }
                else
                {
                    //���ڼ�Ȩ
                    s_3600s_HighPowerNum++;
                }
                //���ݻ���
                res = (s_count_ms%(1000*3600L))/(1000*90L);
                ps_3600s_90sSumBuf[res] += (uint32_t)(dat*2L);
            }
            //----------ÿ90s����
            if(s_count_ms%(90*1000)==0)
            {
                s_43200s_90sDataNum++;
            }
            //----------ÿ1800s����
            if(s_count_ms%(1800*1000)==0)
            {
                if(s_1800s_MHPowerNum<5)
                {
                    s_43200s_1800sNotMoveNum++;
                }
                else
                {
                    if(s_43200s_1800sNotMoveNum<3)
                    {
                        s_43200s_1800sNotMoveNum=0;
                    }
                }
                s_1800s_MHPowerNum=0;
            }
            //----------ÿ3600s����
            if(s_count_ms%(3600*1000)==0)
            {
                if(s_3600s_LowPowerNum)
                {
                    //���������ݻ���ƽ��ֵ
                    s_3600s_LowPowerSum /= s_3600s_LowPowerNum;
                    //ƽ��ֵ��չ
                    s_3600s_LowPowerSum *= 2*(9000-1);
                }
                else
                {
                    s_3600s_LowPowerSum=1;
                }
                //�������������
                for(i16=0; i16<ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM; i16++)
                {
                    if(!ps_3600s_90sMLNumBuf[i16])
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)100;
                    }
                    else if(s_3600s_HighPowerNum>6)
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)150;
                    }
                    else
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)100;
                    }
                    result=ps_3600s_90sSumBuf[i16]/s_3600s_LowPowerSum;
                    if(result>250)
                    {
                        ps_43200s_90sMoveBuf[((s_count_ms/(3600*1000))-1)*ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM+i16]=250;
                    }
                    else
                    {
                        ps_43200s_90sMoveBuf[((s_count_ms/(3600*1000))-1)*ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM+i16]=result;
                    }
                }
                //�建��
                s_3600s_LowPowerSumBuffer = s_3600s_LowPowerSum;
                s_3600s_LowPowerSum = 0;
                s_3600s_LowPowerNum = 0;
                s_3600s_HighPowerNum= 0;
                for(i16=0; i16<ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM; i16++)
                {
                    ps_3600s_90sSumBuf[i16]    =  0;
                    ps_3600s_90sMLNumBuf[i16]  =  0;
                }
            }
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            s_count_ms+=10;
            //�����������1Сʱ
            if(s_count_ms>=1000*3600)
            {
                //�������12Сʱ
                if(s_count_ms>=(1000*3600*12))
                {
                    s_43200s_90sDataNum = 480;
                }
                //����ʣ������
                j16=(uint16_t)((s_count_ms%(3600*1000))/(90*1000));
                //��Ȩ
                for(i16=0; i16<j16; i16++)
                {
                    if(!ps_3600s_90sMLNumBuf[i16])
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)100;
                    }
                    else if(s_3600s_HighPowerNum>6)
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)150;
                    }
                    else
                    {
                        ps_3600s_90sSumBuf[i16] *= (uint32_t)100;
                    }
                    if(s_3600s_LowPowerSumBuffer == 0)
                    {
                        s_3600s_LowPowerSumBuffer = 1;
                    }
                    result=ps_3600s_90sSumBuf[i16]/s_3600s_LowPowerSumBuffer;
                    if(result>250)
                    {
                        ps_43200s_90sMoveBuf[((s_count_ms/(3600*1000)))*ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM+i16]=250;
                    }
                    else
                    {
                        ps_43200s_90sMoveBuf[((s_count_ms/(3600*1000)))*ARITHMETIC_BODY_MOVE_90S_PER_3600S_NUM+i16]=result;
                    }
                }
                //����
                for(i16=0; i16<s_43200s_90sDataNum; i16+=2)
                {
                    uctsk_Rfms_s_BodyMove.DataBuf[i16/2]=(ps_43200s_90sMoveBuf[i16]+ps_43200s_90sMoveBuf[i16+1])/2;
                    if(uctsk_Rfms_s_BodyMove.DataBuf[i16/2]>250)
                    {
                        uctsk_Rfms_s_BodyMove.DataBuf[i16/2]=250;
                    }
                    if(uctsk_Rfms_s_BodyMove.DataBuf[i16/2]>ARITHMETIC_BODY_MOVE_LEVEL_WAKE)
                    {
                        uctsk_Rfms_s_DaySleepLevel.DataBuf[i16/2]=1;
                        j16 = Count_2ByteToWord(uctsk_Rfms_s_DayInfo.BodyMoveCount[0],uctsk_Rfms_s_DayInfo.BodyMoveCount[1]);
                        j16++;
                        uctsk_Rfms_s_DayInfo.BodyMoveCount[0]=j16>>8;
                        uctsk_Rfms_s_DayInfo.BodyMoveCount[1]=j16;
                    }
                    else if(uctsk_Rfms_s_BodyMove.DataBuf[i16/2]>ARITHMETIC_BODY_MOVE_LEVEL_LIGHT)
                    {
                        uctsk_Rfms_s_DaySleepLevel.DataBuf[i16/2]=2;
                    }
                    else if(uctsk_Rfms_s_BodyMove.DataBuf[i16/2]>ARITHMETIC_BODY_MOVE_LEVEL_MIDDLE)
                    {
                        uctsk_Rfms_s_DaySleepLevel.DataBuf[i16/2]=3;
                    }
                    else
                    {
                        uctsk_Rfms_s_DaySleepLevel.DataBuf[i16/2]=4;
                    }
                }
                //����嶯����
                Count_MaxMinAve_Byte(uctsk_Rfms_s_BodyMove.DataBuf,s_count_ms/(180*1000),0x00,0xFF,\
                                     &(uctsk_Rfms_s_DayInfo.BodyMoveMax),NULL,NULL);
                //�嶯����
                if(Count_2ByteToWord(uctsk_Rfms_s_DayInfo.BodyMoveCount[0],uctsk_Rfms_s_DayInfo.BodyMoveCount[1])>127)
                {
                    uctsk_Rfms_s_DayInfo.BodyMoveCount[0]=0;
                    uctsk_Rfms_s_DayInfo.BodyMoveCount[1]=127;
                }
                //�嶯������Ч����
                uctsk_Rfms_s_DayInfo.BodyMoveNum = s_count_ms/(180*1000);
                if(s_count_ms%(180*1000)!=0 && uctsk_Rfms_s_DayInfo.BodyMoveNum<240)
                {
                    uctsk_Rfms_s_DayInfo.BodyMoveNum++;
                }
                //����
                uctsk_Rfms_s_DayInfo.Scoring=0;
                //
                Arithmetic_AI(uctsk_Rfms_s_DaySleepLevel.DataBuf,uctsk_Rfms_s_DayInfo.BodyMoveNum-1);
            }
            //�������С��1Сʱ
            else
            {
                s_43200s_90sDataNum = 0;
                memset((char*)uctsk_Rfms_s_BodyMove.DataBuf,0,sizeof(uctsk_Rfms_s_BodyMove.DataBuf));
                uctsk_Rfms_s_DayInfo.BodyMoveNum = 0;
            }
            //�ͷŻ���
            MemManager_Free(E_MEM_MANAGER_TYPE_1KB,psbuf);
            break;
        default:
            break;
    }
}
/*******************************************************************************
* ��������: �����㷨
* ˵    ��: AI-->�˹����ܵ���д
*******************************************************************************/
void Arithmetic_AI(unsigned char *s,unsigned char length)
{
    unsigned short i,j = 0;
    unsigned char u8_k = 0,u8_temp = 0;

    unsigned char   DeepSleepLength = 0;
    // ��һ����̶�Ϊ����
    s[0] = 1;
    // ����
    while(s[u8_temp]==4)
    {
        u8_temp++;
    }
    //STRATEGE 2
    // �ڶ����������"ǳ˯"���Ϊ"����"
    for(i=0; i<2; i++)
    {
        if(s[i]==2)
        {
            s[i] = 1;
        }
    }
    //STRATEGE 3
    // �������������"��˯"���Ϊ"ǳ˯"
    for(i=0; i<3; i++)
    {
        if(s[i]==3)
        {
            s[i] = 2;
        }
    }
    //STRATEGE 4
    // ���ĸ��������"��˯"���Ϊ"��˯"
    for(i=0; i<4; i++)
    {
        if(s[i]==4)
        {
            s[i] = 3;
        }
    }
    //STRATEGE 5
    // ǰ12����(36����),���������2�����,��֮ǰ���Ǿ���
    for(i=1; i<12; i++)
    {
        if((s[i]==1)&&(s[i+1]==1)&&(s[i-1]!=1))
        {
            for(u8_k=i; u8_k>0; u8_k--)
            {
                s[u8_k] = 1;
            }
        }
    }
    //STRATEGE 6
    // 6����(18����)�Ժ�,��"232"��Ϊ"222"
    for(i=6; i<length-1; i++)
    {
        if((s[i]==3)&&(s[i-1]==2)&&(s[i+1]==2))
        {
            s[i] = 2;
        }
    }
    //STRATEGE 7
    // 6����(18����)�Ժ�,��"1331"��Ϊ"1221"
    // 6����(18����)�Ժ�,��"1332"��Ϊ"1222"
    // 6����(18����)�Ժ�,��"1341"��Ϊ"1221"
    // 6����(18����)�Ժ�,��"1342"��Ϊ"1222"
    // 6����(18����)�Ժ�,��"1431"��Ϊ"1221"
    // 6����(18����)�Ժ�,��"1432"��Ϊ"1222"
    // 6����(18����)�Ժ�,��"1441"��Ϊ"1221"
    // 6����(18����)�Ժ�,��"1442"��Ϊ"1222"
    // 6����(18����)�Ժ�,��"2331"��Ϊ"2221"
    // 6����(18����)�Ժ�,��"2332"��Ϊ"2222"
    // 6����(18����)�Ժ�,��"2341"��Ϊ"2221"
    // 6����(18����)�Ժ�,��"2342"��Ϊ"2222"
    // 6����(18����)�Ժ�,��"2431"��Ϊ"2221"
    // 6����(18����)�Ժ�,��"2432"��Ϊ"2222"
    // 6����(18����)�Ժ�,��"2441"��Ϊ"2221"
    // 6����(18����)�Ժ�,��"2442"��Ϊ"2222"
    for(i=6; i<length-5; i++)
    {
        if((s[i]<=2)&&(s[i+1]>=3)&&(s[i+2]>=3)&&(s[i+3]<=2))
        {
            s[i+1] = s[i+2] = 2;
        }
    }
    //STRATEGE 8
    // 6����(18����)�Ժ�,��"131"��Ϊ"111"
    // 6����(18����)�Ժ�,��"132"��Ϊ"122"
    // 6����(18����)�Ժ�,��"141"��Ϊ"111"
    // 6����(18����)�Ժ�,��"142"��Ϊ"122"
    // 6����(18����)�Ժ�,��"231"��Ϊ"221"
    // 6����(18����)�Ժ�,��"232"��Ϊ"222"
    // 6����(18����)�Ժ�,��"241"��Ϊ"221"
    // 6����(18����)�Ժ�,��"242"��Ϊ"222"
    for(i=6; i<length-1; i++)
    {
        if((s[i]>=3)&&(s[i-1]<=2)&&(s[i+1]<=2))
        {
            s[i] = COUNT_MAX(s[i-1],s[i+1]);
        }
    }
    //STRATEGE 9
    // 1����(3����)�Ժ�,��"13"��Ϊ"12"
    // 1����(3����)�Ժ�,��"14"��Ϊ"12"
    for(i=1; i<length; i++)
    {
        if((s[i]>=3)&&(s[i-1]==1))
        {
            s[i] = 2;
        }
    }
    //STRATEGE 10
    // 1����(3����)�Ժ�,��"24"��Ϊ"23"
    for(i=1; i<length; i++)
    {
        if((s[i]==4)&&(s[i-1]==2))
        {
            s[i] = 3;
        }
    }
    //STRATEGE 11
    // ���һ��̶�Ϊ1
    s[length] = 1;
    //u8_temp = length/2;
    u8_temp = 60;
    DeepSleepLength=0;
    for(i=0; i<=length; i++)
    {
        if(s[i] == 4)
        {
            DeepSleepLength ++;
        }
        else
        {
            // ǰ3Сʱ������6����˯��Ϊ6������˯
            // 3Сʱ��С��9���ӵ���˯��Ϊ��˯
            if((DeepSleepLength < 4 && i<u8_temp)
               ||(DeepSleepLength < 3 && i>=u8_temp))
            {
                for(j=1; j<=DeepSleepLength; j++)
                {
                    s[i-j] = 3;
                }
            }
            DeepSleepLength = 0;
        }
    }
}
/*******************************************************************************
��������: �����㷨(�������ڶ�ʱ����,ÿ10mSִ��һ��)
*******************************************************************************/
extern BiquadSections IIR_Breath_BPF;
static uint8_t B_SplitData_2(float data_input[]);
static void Arithmetic_Breath(uint8_t step,uint16_t dat)
{
    //��һ�εĲ���ֵ
    static uint16_t s_dat_bak=0;
    //-----10ms����
    static uint32_t s_count_ms=0;
    //-----50mS����
    //����IIR�˲��ĺ���ԭʼ���ݻ�(90s*20Hz)
    //static float s_BreathDataBuf[90*20]= {0};
    static float *ps_BreathDataBuf;
    //-----180s����(3���ӷֱ���(��12Сʱ)=240)
    static uint8_t *pBreathFinalBuf=NULL;
    //
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_IDLE:
            break;
        case UCTSK_RFMS_E_RUNSTATE_START:
            s_dat_bak   =  0;
            s_count_ms  =  0;
            //���뻺��
            ps_BreathDataBuf=MemManager_Get(E_MEM_MANAGER_TYPE_10KB);
            memset((uint8_t*)ps_BreathDataBuf,0,10*1024);
            //���뻺��
            pBreathFinalBuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            memset((uint8_t*)pBreathFinalBuf,0,256);
            break;
        case UCTSK_RFMS_E_RUNSTATE_START_DELAY:
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            //----------ֻ����12Сʱ���ڵ�����
            //�������12Сʱ
            if(s_count_ms>=(1000*60*60*12))
            {
                //����������
                break;
            }
            //���С��12Сʱ
            else
            {
                //�������ۼ�
                s_count_ms+=10;
            }
            //----------������ֵ,��ȥֱ��(dat)
            //���ֵ�ж�
            if(dat>0x3FF)
            {
                dat = 0x3FF;
            }
            //�仯����,���˵�
            if(abs(s_dat_bak-dat)>0x1F0)
            {
                dat = s_dat_bak;
            }
            else
            {
                s_dat_bak=dat;
            }
            //ȥֱ��
            //dat=abs(dat-UCTSK_RFMS_DATA_DC);
            //----------ÿ50ms����
            if(s_count_ms%50==0)
            {
                ps_BreathDataBuf[(s_count_ms%(90*1000))/50-1]=DSCFilter(dat,&IIR_Breath_BPF);
            }
            //----------ÿ180s����
            if(s_count_ms%(180*1000)==0)
            {
                pBreathFinalBuf[s_count_ms/(180*1000)-1]=B_SplitData_2(ps_BreathDataBuf);
            }
            //----------
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            s_count_ms+=10;
            memcpy((char*)uctsk_Rfms_s_DayBreath.DataBuf,(char*)pBreathFinalBuf,240);
            //�ͷŻ���
            MemManager_Free(E_MEM_MANAGER_TYPE_10KB,ps_BreathDataBuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pBreathFinalBuf);
            uctsk_Rfms_s_DayInfo.BreathNum=Count_MaxMinAve_Byte(uctsk_Rfms_s_DayBreath.DataBuf,\
                                           s_count_ms/(180*1000),0x00,0xE0,\
                                           &(uctsk_Rfms_s_DayInfo.BreathMax),\
                                           &(uctsk_Rfms_s_DayInfo.BreathMin),\
                                           &(uctsk_Rfms_s_DayInfo.BreathAverage));
            break;
        default:
            break;
    }
}
/*******************************************************************************
��������: �����㷨(�������ڶ�ʱ����,ÿ10mSִ��һ��)
*******************************************************************************/
extern BiquadSections IIR_Heart_BPF;
static uint8_t H_SplitData_2(float data_input[]);
static void Arithmetic_HeartRate(uint8_t step,uint16_t dat)
{
    //��һ�εĲ���ֵ
    static uint16_t s_dat_bak=0;
    //-----10ms����
    static uint32_t s_count_ms=0;
    //����IIR�˲��ĺ���ԭʼ���ݻ�(90s*20Hz)
    //static float s_HeartDataBuf[90*20];
    static float *ps_HeartDataBuf;
    //-----180s����(3���ӷֱ���(��12Сʱ)=240)
    static uint8_t *pHeartFinalBuf=NULL;
    //
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_IDLE:
            break;
        case UCTSK_RFMS_E_RUNSTATE_START:
            s_dat_bak   =  0;
            s_count_ms  =  0;
            //���뻺��
            ps_HeartDataBuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_10KB);
            memset((uint8_t*)ps_HeartDataBuf,0,10*1024);
            //���뻺��
            pHeartFinalBuf    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            memset((uint8_t*)pHeartFinalBuf,0,256);
            break;
        case UCTSK_RFMS_E_RUNSTATE_START_DELAY:
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            //----------ֻ����12Сʱ���ڵ�����
            //�������12Сʱ
            if(s_count_ms>=(1000*60*60*12))
            {
                //����������
                break;
            }
            //���С��12Сʱ
            else
            {
                //�������ۼ�
                s_count_ms+=10;
            }
            //----------������ֵ,��ȥֱ��(dat)
            //���ֵ�ж�
            if(dat>0x3FF)
            {
                dat = 0x3FF;
            }
            //�仯����,���˵�
            if(abs(s_dat_bak-dat)>0x1F0)
            {
                dat = s_dat_bak;
            }
            else
            {
                s_dat_bak=dat;
            }
            //ȥֱ��
            //dat=abs(dat-UCTSK_RFMS_DATA_DC);
            //----------ÿ50ms����
            if(s_count_ms%50==0)
            {
                ps_HeartDataBuf[(s_count_ms%(90*1000))/50-1]=DSCFilter(dat,&IIR_Heart_BPF);
            }
            //----------ÿ180s����
            if(s_count_ms%(180*1000)==0)
            {
                pHeartFinalBuf[s_count_ms/(180*1000)-1]=H_SplitData_2(ps_HeartDataBuf);
            }
            //----------
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            s_count_ms+=10;
            memcpy((char*)uctsk_Rfms_s_HeartRate.DataBuf,(char*)pHeartFinalBuf,240);
            //�ͷŻ���
            MemManager_Free(E_MEM_MANAGER_TYPE_10KB,ps_HeartDataBuf);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pHeartFinalBuf);
            uctsk_Rfms_s_DayInfo.HeartRateNum=Count_MaxMinAve_Byte(uctsk_Rfms_s_HeartRate.DataBuf,\
                                              s_count_ms/(180*1000),0x00,0xE0,\
                                              &(uctsk_Rfms_s_DayInfo.HeartRateMax),\
                                              &(uctsk_Rfms_s_DayInfo.HeartRateMin),\
                                              &(uctsk_Rfms_s_DayInfo.HeartRateAverage));
            break;
        default:
            break;
    }
}
//
static float DSCFilter(uint16_t dat,BiquadSections *filter_name)
{
    float outputSignal[1];
    float inputSignal[1];

    inputSignal[0] = (float)dat;
    IIR_Breath_BPF.filter(inputSignal, outputSignal, 1, filter_name);
    return(outputSignal[0]);
}
//

#define M_PI_2    1.5707963267948966
#define CORR_NUM  1800
#define SN_TH     0.35f
static uint8_t B_SplitData_2(float data_input[])
{
    static uint8_t u8_s_breath_peak_flag = 0;
    uint8_t u8_delay;
    uint8_t u8_corB_peak_num=0;
    short s16_i,s16_j = 0;
    long double f16_sum = 0;
    //-----
    float *f8_a_data=data_input;
    float *f8_a_corB=NULL;
    //-----
    float f8_TH         = 0;
    //float f8_corB_peak  =  0;
    float f8_corB       =  0;
    //
    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        if(f8_a_data[s16_i]>=120)
        {
            u8_s_breath_peak_flag = 1;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]>=0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        else if(f8_a_data[s16_i]<=-120)
        {
            u8_s_breath_peak_flag = 2;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]<=0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        if(u8_s_breath_peak_flag==1)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]>=0)&&(f8_a_data[s16_i+1]<0))             // failing edge
            {
                u8_s_breath_peak_flag = 0;
            }
        }
        else if(u8_s_breath_peak_flag==2)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]<=0)&&(f8_a_data[s16_i+1]>0))             // failing edge
            {
                u8_s_breath_peak_flag = 0;
            }
        }
    }

    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        f16_sum += abs(f8_a_data[s16_i]);
    }
    f8_TH = 2*(M_PI_2*(f16_sum/50));
    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        if(f8_a_data[s16_i]>=f8_TH)
        {
            u8_s_breath_peak_flag = 1;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]>0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        else if(f8_a_data[s16_i]<=-f8_TH)
        {
            u8_s_breath_peak_flag = 2;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]<0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        if(u8_s_breath_peak_flag==1)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]>=0)&&(f8_a_data[s16_i+1]<0))             // failing edge
            {
                u8_s_breath_peak_flag = 0;
            }
        }
        else if(u8_s_breath_peak_flag==2)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]<=0)&&(f8_a_data[s16_i+1]>0))             // failing edge
            {
                u8_s_breath_peak_flag = 0;
            }
        }
    }
    //���뻺��
    f8_a_corB  =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //
    for(u8_delay=0; u8_delay<125; u8_delay++)
    {
        for(s16_i=0; s16_i<1200; s16_i++)
        {
            f8_corB =  f8_corB + (f8_a_data[s16_i]*f8_a_data[s16_i+u8_delay]);
        }
        f8_a_corB[u8_delay] = f8_corB;
        f8_corB = 0;
    }

    for(u8_delay=60; u8_delay<120; u8_delay++)
    {
        if((f8_a_corB[u8_delay]>=f8_a_corB[u8_delay+1])&&(f8_a_corB[u8_delay]>=f8_a_corB[u8_delay-1]))
        {
            //f8_corB_peak = f8_a_corB[u8_delay];
            u8_corB_peak_num = u8_delay;
        }
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,f8_a_corB);
    //
    if((u8_corB_peak_num<=60)||(u8_corB_peak_num>=100))
    {
        f8_corB = 0;
        return(0xFF);

    }/*
    else if(f8_corB_peak<12000)
    {
        f8_corB = 0;
        return(0xFE);

    }*/
    else if((u8_corB_peak_num>60)&&(u8_corB_peak_num<100))
    {
        f8_corB = 0;
        return(1200/u8_corB_peak_num);

    }
    else
    {
        f8_corB = 0;
        return(0xFC);
    }
}
//
static uint8_t H_SplitData_2(float data_input[])
{
    static uint8_t u8_s_heart_peak_flag = 0;
    uint8_t u8_delay;
    uint8_t u8_corB_peak_num  =  0;
    short s16_i,s16_j = 0;
    long double f16_sum = 0;
    //-----
    float *f8_a_data=data_input;
    float *f8_a_corB=NULL;
    //-----
    float f8_TH         =  0;
    //float f8_corB_peak  =  0;
    float f8_corB;
    //
    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        if(f8_a_data[s16_i]>=120)
        {
            u8_s_heart_peak_flag = 1;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]>=0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        else if(f8_a_data[s16_i]<=-120)
        {
            u8_s_heart_peak_flag = 2;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]<=0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        if(u8_s_heart_peak_flag==1)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]>=0)&&(f8_a_data[s16_i+1]<0))             // failing edge
            {
                u8_s_heart_peak_flag = 0;
            }
        }
        else if(u8_s_heart_peak_flag==2)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]<=0)&&(f8_a_data[s16_i+1]>0))             // failing edge
            {
                u8_s_heart_peak_flag = 0;
            }
        }
    }

    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        f16_sum += abs(f8_a_data[s16_i]);
    }
    f8_TH = 2*(M_PI_2*(f16_sum/50));    // ????50
    for(s16_i=0; s16_i<CORR_NUM; s16_i++)
    {
        if(f8_a_data[s16_i]>=f8_TH)
        {
            u8_s_heart_peak_flag = 1;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]>0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        else if(f8_a_data[s16_i]<=-f8_TH)
        {
            u8_s_heart_peak_flag = 2;
            for(s16_j=0; s16_j<=s16_i; s16_j++)
            {
                if(f8_a_data[s16_i-s16_j]<0)
                {
                    f8_a_data[s16_i-s16_j] = 0;
                }
                else
                {
                    f8_a_data[s16_i-s16_j] = 0;
                    break;
                }
            }
        }
        if(u8_s_heart_peak_flag==1)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]>=0)&&(f8_a_data[s16_i+1]<0))             // failing edge
            {
                u8_s_heart_peak_flag = 0;
            }
        }
        else if(u8_s_heart_peak_flag==2)
        {
            f8_a_data[s16_i] = 0;
            if((f8_a_data[s16_i]<=0)&&(f8_a_data[s16_i+1]>0))             // failing edge
            {
                u8_s_heart_peak_flag = 0;
            }
        }
    }
    //���뻺��
    f8_a_corB  =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //
    for(u8_delay=0; u8_delay<125; u8_delay++)
    {
        for(s16_i=0; s16_i<1200; s16_i++)
        {
            f8_corB =  f8_corB + (f8_a_data[s16_i]*f8_a_data[s16_i+u8_delay]);
        }
        f8_a_corB[u8_delay] = f8_corB;
        f8_corB = 0;
    }

    for(u8_delay=10; u8_delay<40; u8_delay++)
    {
        //find peak value
        if((f8_a_corB[u8_delay]>=f8_a_corB[u8_delay+1])&&(f8_a_corB[u8_delay]>f8_a_corB[u8_delay-1]))
        {
            //f8_corB_peak = f8_a_corB[u8_delay];
            u8_corB_peak_num = u8_delay;
        }
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,f8_a_corB);
    //
    if((u8_corB_peak_num<15)||(u8_corB_peak_num>24))
    {
        f8_corB = 0;
        return(0xFF);
    }/*
//    else if((f8_corB_peak/f8_corB_max)<SN_TH)
    else if(f8_corB_peak<27500)
    {
        f8_corB = 0;
        return(0xFE);
    }*/
    else if((u8_corB_peak_num>=15)&&(u8_corB_peak_num<=24))
    {
        f8_corB = 0;
        return(1200/u8_corB_peak_num);
    }
    else
    {
        f8_corB = 0;
        return(0xFC);
    }
}

/*******************************************************************************
*��������: ˯�������㷨
*�� �� ֵ: OK/ERR
*******************************************************************************/
static uint8_t Arithmetic_SleepMarkInfoLoad(MODULE_MEMORY_S_DAY_INFO* psInfo,MODULE_MEMORY_S_DAY_SLEEPLEVEL* psSleepLevel,SLEEP_S_MARK* psMark)
{
    uint16_t i16,j16;
    //��˯ʱ��
    psMark->TotalLevelTime_m = 0;
    psMark->TotalLevelTime_m += Count_2ByteToWord(psInfo->WakeLevelTime_m[0],psInfo->WakeLevelTime_m[1]);
    psMark->TotalLevelTime_m += Count_2ByteToWord(psInfo->LightLevelTime_m[0],psInfo->LightLevelTime_m[1]);
    psMark->TotalLevelTime_m += Count_2ByteToWord(psInfo->MiddleLevelTime_m[0],psInfo->MiddleLevelTime_m[1]);
    psMark->TotalLevelTime_m += Count_2ByteToWord(psInfo->DeepLevelTime_m[0],psInfo->DeepLevelTime_m[1]);
    //ǰ4Сʱ����˯ʱ��/��˯ʱ��
    psMark->MiddleLevelTime_4h_m =  0;
    psMark->DeepLevelTime_4h_m   =  0;
    i16=COUNT_MIN(psMark->TotalLevelTime_m,(4*60));
    i16=i16/3;
    for(j16=0; j16<i16; j16++)
    {
        switch(psSleepLevel->DataBuf[j16])
        {
            case MODULE_MEMORY_E_SLEEPLEVEL_WAKE:
                break;
            case MODULE_MEMORY_E_SLEEPLEVEL_LIGHT:
                break;
            case MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE:
                psMark->MiddleLevelTime_4h_m += 3;
                break;
            case MODULE_MEMORY_E_SLEEPLEVEL_DEEP:
                psMark->DeepLevelTime_4h_m += 3;
                break;
            default:
                break;
        }
    }
    psMark->MiddleLevelTime_After4h_m  =  Count_2ByteToWord(psInfo->MiddleLevelTime_m[0],psInfo->MiddleLevelTime_m[1])-psMark->MiddleLevelTime_4h_m;
    psMark->DeepLevelTime_After4h_m    =  Count_2ByteToWord(psInfo->DeepLevelTime_m[0],psInfo->DeepLevelTime_m[1])-psMark->DeepLevelTime_4h_m;
    //��˯ʱ��
    psMark->FallAsleepTime_m = Count_2ByteToWord(psInfo->FallAsleepTime_m[0],psInfo->FallAsleepTime_m[1]);
    //�嶯����
    psMark->BodyMoveCount = Count_2ByteToWord(psInfo->BodyMoveCount[0],psInfo->BodyMoveCount[1]);
    //�¶�
    memcpy(psMark->TempBuf,psInfo->TempBuf,sizeof(psMark->TempBuf));
    //ʪ��
    memcpy(psMark->HumiBuf,psInfo->HumiBuf,sizeof(psMark->HumiBuf));
    //------------------------------------------------
    //���ӻ���ǰ״̬
    //��ȡ����ģʽ
    psMark->AlarmWakeState=psInfo->SleepLevelBeforeAlarm;
    //ʼ˯ʱ��䶯
    {
        MODULE_MEMORY_S_SLEEP_BEGIN_TIME *psSleepBeginTime;
        uint8_t i,res;
        uint16_t i16,j16;
        //���뻺��
        psSleepBeginTime = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //��ȡ��ʷ����
        Module_Memory_App(MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_R,(uint8_t*)psSleepBeginTime,NULL);
        //�״�ʹ�ó�ʼ��
        if(psSleepBeginTime->NextHhMmNum==0xFF)
        {
            psSleepBeginTime->NextHhMmNum=0;
            memset(&psSleepBeginTime->Minute[0],0xFF,sizeof(psSleepBeginTime->Minute));
        }
        //����ʱ��䶯
        //---����ʱ��
        i16 = psInfo->BeginTime[4]*60+psInfo->BeginTime[5];
        if(i16<=6*60)
        {
            i16+=24*60;
        }
        //---��ʷʱ���ֵ
        j16=0;
        i=0;
        for(res=0; res<(sizeof(psSleepBeginTime->Minute)/2); res++)
        {
            if(psSleepBeginTime->Minute[res]==0xFFFF)
            {
                continue;
            }
            else
            {
                if(psSleepBeginTime->Minute[res]>=18*60 && psSleepBeginTime->Minute[res]<24*60)
                {
                    j16+=psSleepBeginTime->Minute[res];
                    i++;
                }
                else if(psSleepBeginTime->Minute[res]<=6*60)
                {
                    j16+=psSleepBeginTime->Minute[res]+24*60;
                    i++;
                }
                else
                {
                    continue;
                }
            }
        }
        //�������Ч��ʷ����,������
        if(i==0)
        {
            j16 = i16;
        }
        else
        {
            j16 = j16/i;
        }
        //���ݸ�ֵ
        psMark->BeginSleepTimeChange_m=abs(i16-j16);
        //�޸�����
        if(i16>=24*60)
        {
            i16-=24*60;
        }
        if((i16>=18*60 && i16<24*60)
           ||(i16<=6*60))
        {
            psSleepBeginTime->Minute[psSleepBeginTime->NextHhMmNum]=i16;
            //
            /*
            psSleepBeginTime->NextHhMmNum++;
            if(psSleepBeginTime->NextHhMmNum>=28)
            {
                psSleepBeginTime->NextHhMmNum=0;
            }
            */
            //���ϳ������������������
            psSleepBeginTime->NextHhMmNum=(psSleepBeginTime->NextHhMmNum+1)%28;
            //д�뵽��ʷ����
            Module_Memory_App(MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_W,(uint8_t*)psSleepBeginTime,NULL);
        }
        //�ͷŻ���
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psSleepBeginTime);
    }
    //
    return OK;
}
static uint8_t Arithmetic_SleepMark(SLEEP_S_MARK *psSleepMark)
{
    int16_t temp;
    uint32_t totalsleepcof;
    int16_t result=0;
    float f1;
    //-----------------------
    //˯�߽ṹ��ص÷�
    //f1=((float)psSleepMark->DeepLevelTime_4h_m + (float)psSleepMark->MiddleLevelTime_4h_m/2)/(COUNT_MIN((float)(psSleepMark->TotalLevelTime_m),240));
    //f1=((float)psSleepMark->DeepLevelTime_4h_m + ((float)psSleepMark->MiddleLevelTime_4h_m)/1.5f)/(COUNT_MIN((float)(psSleepMark->TotalLevelTime_m),240));
    /*
    f1=((float)psSleepMark->DeepLevelTime_4h_m + ((float)psSleepMark->MiddleLevelTime_4h_m)*0.65f)/(COUNT_MIN((float)(psSleepMark->TotalLevelTime_m),240));
    f1*=WEIGHTSLEEPSTRCT;
    f1=(float)(f1/STDSLEEPSTRUCTURE);
    temp = (int16_t)f1;
    if(temp > WEIGHTSLEEPSTRCT)
        temp = WEIGHTSLEEPSTRCT;
        */
    //ǰ��
    f1=((float)psSleepMark->DeepLevelTime_4h_m + ((float)psSleepMark->MiddleLevelTime_4h_m)*0.65f)/(COUNT_MIN((float)(psSleepMark->TotalLevelTime_m),240));
    f1*=WEIGHTSLEEPSTRCT_HEAD;
    f1=(float)(f1/STDSLEEPSTRUCTURE_HEAD);
    temp = (int16_t)f1;
    //��
    if(psSleepMark->TotalLevelTime_m>240)
    {
        f1=((float)psSleepMark->DeepLevelTime_After4h_m + ((float)psSleepMark->MiddleLevelTime_After4h_m)*0.65f)/(psSleepMark->TotalLevelTime_m-240);
        f1*=WEIGHTSLEEPSTRCT_TAIL;
        f1=(float)(f1/STDSLEEPSTRUCTURE_TAIL);
    }
    else
    {
        f1=0;
    }
    temp += (int16_t)f1;
    if(temp > WEIGHTSLEEPSTRCT)
        temp = WEIGHTSLEEPSTRCT;
    //
    result += temp;
    psSleepMark->Mark_SleepStruct=temp;
    //��˯ʱ����ص÷�
    if(psSleepMark->FallAsleepTime_m>STDINTOPERIOD)
        temp = WEIGHTSLEEPINTOPERIOD-((int16_t)psSleepMark->FallAsleepTime_m-(int16_t)STDINTOPERIOD )/3;
    else
        temp = WEIGHTSLEEPINTOPERIOD;
    if(temp <0)
        temp= 0;
    //
    result += temp;
    psSleepMark->Mart_SleepIntoPeriod=temp;
    //�嶯������ص÷�
    if(psSleepMark->BodyMoveCount >STDBODYMOVEMENTCOUNT_HI)
        temp =  WEIGHTBODYMOVEMENTCOUNT-((int16_t)psSleepMark->BodyMoveCount-(int16_t)STDBODYMOVEMENTCOUNT_HI)/2 ;
    else if (psSleepMark->BodyMoveCount <STDBODYMOVEMENTCOUNT_LOW)
        temp =  WEIGHTBODYMOVEMENTCOUNT-((int16_t)STDBODYMOVEMENTCOUNT_LOW-(int16_t)psSleepMark->BodyMoveCount)/2;
    else
        temp = WEIGHTBODYMOVEMENTCOUNT;
    if(temp <0)
        temp= 0;
    //
    result += temp;
    psSleepMark->Mark_BodyMovement=temp;
    //����״̬��ص÷�
    if(MODULE_MEMORY_E_SLEEPLEVEL_WAKE <= psSleepMark->AlarmWakeState \
       && MODULE_MEMORY_E_SLEEPLEVEL_DEEP >= psSleepMark->AlarmWakeState)
        temp =WEIGHTAWAKESTATE + 1 -(int16_t)psSleepMark->AlarmWakeState  ;
    else
        temp =WEIGHTAWAKESTATE;
    //
    result += temp;
    psSleepMark->Mark_WakeState=temp;
    //˯��ϰ��(ʼ˯�䶯)��ص÷�
    if(psSleepMark->BeginSleepTimeChange_m>STDSARTTIMEDIS)
        temp = WEIGHTSTARTTIMEDIS-((int16_t)psSleepMark->BeginSleepTimeChange_m-(int16_t)STDSARTTIMEDIS)/15;
    else
        temp = WEIGHTSTARTTIMEDIS;
    if(temp <0)
        temp= 0;
    //
    result += temp;
    psSleepMark->Mark_BeginSleepTimeChange=temp;
    //˯�߻���(�¶�)��ص÷�
    if(psSleepMark->TempBuf[0] > 50 || psSleepMark->TempBuf[0] < (-9))
    {
        temp=0;
    }
    else
    {
        if(psSleepMark->TempBuf[0] >STDSLEEPTEMP_HI)
            temp= WEIGHTSLEEPTEMP-( psSleepMark->TempBuf[0]-STDSLEEPTEMP_HI);
        else if(psSleepMark->TempBuf[0]<STDSLEEPTEMP_LOW)
            temp = WEIGHTSLEEPTEMP-(STDSLEEPTEMP_LOW- psSleepMark->TempBuf[0]);
        else
            temp = WEIGHTSLEEPTEMP;

        if(temp <0)
            temp= 0;
    }
    //
    result += temp;
    psSleepMark->Mark_Temp=temp;
    //˯�߻���(ʪ��)��ص÷�
    if(psSleepMark->HumiBuf[0] > 100)
    {
        temp=0;
    }
    else
    {
        if(psSleepMark->HumiBuf[0]>STDSLEEPHUM_HI)
            temp = WEIGHTSLEEPHUM-(psSleepMark->HumiBuf[0]-STDSLEEPHUM_HI)/ 5;
        else if(psSleepMark->HumiBuf[0]<STDSLEEPHUM_LOW)
            temp = WEIGHTSLEEPHUM-(STDSLEEPHUM_LOW -psSleepMark->HumiBuf[0])/5;
        else
            temp = WEIGHTSLEEPHUM;
        if(temp <0)
            temp= 0;
    }
    //
    result += temp;
    psSleepMark->Mark_Humi=temp;
    //��˯ʱ������ϵ��
    if(psSleepMark->TotalLevelTime_m > STDTOTALSLEEPPERIOD_HI)
        totalsleepcof= (uint32_t)STDTOTALSLEEPPERIOD_HI*1000/((uint32_t)psSleepMark->TotalLevelTime_m);
    else if(psSleepMark->TotalLevelTime_m<STDTOTALSLEEPPERIOD_LOW)
        totalsleepcof= (uint32_t)(psSleepMark->TotalLevelTime_m)*1000/STDTOTALSLEEPPERIOD_LOW;
    else
        totalsleepcof= 1000;
    if(totalsleepcof<150)
        totalsleepcof=150;
    psSleepMark->Mart_TotalSleep=totalsleepcof;
    //˯�߷�������
    psSleepMark->Mart_All = totalsleepcof*AGERATIO*(uint32_t)result/1000;
    //˯�߷�����ΧУ��(25-99)
    if(psSleepMark->Mart_All >= 100)
        psSleepMark->Mart_All = 99;
    if(psSleepMark->Mart_All < 25)
        psSleepMark->Mart_All = 25;
    return OK;
}
//---------------------------------------------------------------------------------
#include "bsp_tft_lcd.h"//ww
/*******************************************************************************
* ��������: ��ֵ�㷨
* ��    ��:
* ˵    ��:
*******************************************************************************/
void uctsk_Rfms_Interpolation_Proc(uint8_t* source, uint8_t* parameter,uint8_t* destination, uint8_t length ,uint8_t min,float coeff, uint8_t endMark)
{
    uint8_t i =0, invalidStart =0, invalidCount=0;
    float step;

    while((*(source+i) != endMark)&&(i < length-1))
    {
        //��Ч����
        if( *(source+i) >>7)
        {
            //��һ����Ч���ݵĵ�ַ
            if(invalidCount == 0 )
            {
                invalidStart = i;
            }

            invalidCount ++;
            i ++;
        }
        //��Ч���ݳ��֣���ֵ
        else
        {
            if(invalidCount == 0)
            {
            }
            //������Ч����С�ڵ���3����ƽ������ֵ
            else //if(invalidCount <= 3)//ww
            {
                //ǰ3������������Ч
                if(invalidStart == 0)
                {
                    i = 0;
                    while( i < invalidCount)
                    {
                        *(destination+i) = *(source+invalidCount )|0x80 ;
                        i ++;
                    }
                }
                else
                {
                    //step�����ɸ����Ǹ�С��
                    step =( *(source+i)-*(source+invalidStart-1))/(invalidCount+1);
                    i = invalidStart;
                    while( i < invalidStart + invalidCount)
                    {
                        *(destination+i) = (uint8_t)((float)*(destination+i - 1) + step)|0x80;
                        i ++;
                    }
                }

            }
            //������Ч���ݴ���3�������嶯��ֵ
//            else//ww,�������������ֵ��Сֵȷ��֮���ٴ�����
//            {
//                 i = invalidStart;
//                while( i < invalidStart + invalidCount)
//                {
//                    *(destination+i) =(uint8_t)( coeff*(*(parameter+i )) +min + 128);
//                    i ++;
//                }
//            }

            *(destination+i) =*(source + i );
            invalidCount = 0;
            i ++;

        }
    }
    //ĩβ������Ч
    if(invalidCount> 0)
    {
        //������Ч����С�ڵ���3����ƽ������ֵ
//        if(invalidCount <= 3)//ww,�������������ֵ��Сֵȷ��֮���ٴ�����
//        {
        i = invalidStart;
        while( i < invalidStart + invalidCount)
        {
            *(destination+i) = *(source+invalidStart-1 ) +128;
            i ++;
        }
//        }
//        //������Ч���ݴ���3�������嶯��ֵ
//        else//ww,�������������ֵ��Сֵȷ��֮���ٴ�����
//        {
//            i = invalidStart;
//            while( i < invalidStart + invalidCount)
//            {
//                *(destination+i) =(uint8_t)( (coeff * (*(parameter+i ) ))+min + 128);
//                i ++;
//            }
//
//        }
    }
    *(destination+i) =*(source+i);
}
/*******************************************************************************
* ��������: ƽ������
* ��    ��:
* ˵    ��:
*******************************************************************************/
void uctsk_Rfms_Average_Proc(uint8_t* valueArray, uint8_t average,uint16_t* coordirateArray,uint16_t centerCoordinate,uint16_t limit ,uint8_t length,uint8_t endMark, float step)//
{
    uint8_t i ;
    int16_t sum;

    //����Ч����
    if((*(valueArray+0) == endMark)||length == 0)
    {
        coordirateArray[0]  = endMark;
        return;
    }

    //����һ����Ч����
    if((*(valueArray+1) == endMark)||length == 1)
    {
        coordirateArray[0]  =centerCoordinate-(int16_t)(((int16_t)((*(valueArray+0))&0x7F)-average)*step);
        if(coordirateArray[0]  > centerCoordinate + limit)
            coordirateArray[0]  = centerCoordinate + limit;
        else if( coordirateArray[0]  < centerCoordinate - limit)
            coordirateArray[0]  = centerCoordinate - limit;

        coordirateArray[1]  = endMark;
        return;
    }

    //��2����Ч����
    if((*(valueArray+2) == endMark)||length ==2)
    {
        coordirateArray[1]  =centerCoordinate-(int16_t)((((int16_t)(*(valueArray+1))&0x7F)-average)*step);

        if(coordirateArray[1]  > centerCoordinate + limit)
            coordirateArray[1]  = centerCoordinate + limit;
        else if( coordirateArray[1]  < centerCoordinate - limit)
            coordirateArray[1]  = centerCoordinate - limit;

        coordirateArray[2]  = endMark;
        return;
    }

    sum =( *(valueArray+0)&0x7F)*3;
    sum +=*(valueArray+1)&0x7F;
    sum +=*(valueArray+2)&0x7F;

    coordirateArray[0]  =centerCoordinate-(int16_t)(((int16_t)sum/5-average)*step);
    if(coordirateArray[0]  > centerCoordinate + limit)
        coordirateArray[0]  = centerCoordinate + limit;
    else if( coordirateArray[0]  < centerCoordinate - limit)
        coordirateArray[0]  = centerCoordinate - limit;

    if((*(valueArray+3) == endMark)||length == 3)
    {
        coordirateArray[2]  =centerCoordinate-(int16_t)(((int16_t)((*(valueArray+2))&0x7F)-average)*step);

        if(coordirateArray[2]  > centerCoordinate + limit)
            coordirateArray[2]  = centerCoordinate + limit;
        else if( coordirateArray[2]  < centerCoordinate - limit)
            coordirateArray[2]  = centerCoordinate - limit;

        coordirateArray[3]  = endMark;
        return;
    }

    sum -=( *(valueArray+0)&0x7F);
    sum +=*(valueArray+3)&0x7F;
    coordirateArray[1]=centerCoordinate-(int16_t)(((int16_t)sum/5-average)*step);

    if(coordirateArray[1]  > centerCoordinate + limit)
        coordirateArray[1]  = centerCoordinate + limit;
    else if( coordirateArray[1]  < centerCoordinate - limit)
        coordirateArray[1]  = centerCoordinate - limit;

    sum -=( *(valueArray+0)&0x7F);
    sum +=*(valueArray+4)&0x7F;
    coordirateArray[2]=centerCoordinate-(int16_t)(((int16_t)sum/5-average)*step);

    if(coordirateArray[2]  > centerCoordinate + limit)
        coordirateArray[2]  = centerCoordinate + limit;
    else if( coordirateArray[2]  < centerCoordinate - limit)
        coordirateArray[2]  = centerCoordinate - limit;


    i = 3;
    while ((* (valueArray+i+2) != endMark)&&(i +2 < length))//&&(i+2 < sizeof(valueArray)))
    {
        sum -=*(valueArray+i-3)&0x7F;
        sum +=* (valueArray+i+2)&0x7F;

        coordirateArray[i]  =centerCoordinate-(int16_t)(((s16)sum/5-average)*step);

        if(coordirateArray[i]  > centerCoordinate + limit)
            coordirateArray[i]  = centerCoordinate + limit;
        else if( coordirateArray[i]  < centerCoordinate - limit)
            coordirateArray[i]  = centerCoordinate - limit;

        i ++;
    }

    while((* (valueArray+i) != endMark)&&(i < length))
    {
        sum -=*(valueArray+i-3)&0x7F;
        sum +=*(valueArray+length-2)&0x7F;
        coordirateArray[i]  =centerCoordinate-(int16_t)(((int16_t)sum/5-average)*step);

        if(coordirateArray[i]  > centerCoordinate + limit)
            coordirateArray[i]  = centerCoordinate + limit;
        else if( coordirateArray[i]  < centerCoordinate - limit)
            coordirateArray[i]  = centerCoordinate - limit;

        i ++;
    }

    coordirateArray[i]  = endMark;

}
/*******************************************************************************
* ��������: TF���洢ԭʼ����
*******************************************************************************/
void uctsk_Rfms_DataSaveToTf_100ms(void)
{
    uint32_t i32;
    if(Rfms_s_DataSaveToTf.SaveSign==0)
    {
        return;
    }
    if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_ADD,"/test1.adc",Rfms_s_DataSaveToTf.pData,NULL,&Rfms_s_DataSaveToTf.DataLen,&i32))
    {
        Rfms_s_DataSaveToTf.ErrCmt++;
    }
    Rfms_s_DataSaveToTf.SaveSign=0;
}
static void uctsk_Rfms_DataSaveToTf(uint8_t step,uint16_t dat)
{
    static uint8_t *pbuf=NULL;
    static uint8_t *pbuf1=NULL;
    static uint16_t si16=0;
    //�����TF��ȡ״̬,���ٴ�д��
    if(Uctsk_Rfms_RunFileData_DebugTest_Enable==ON)
    {
        return;
    }
    //tf��δ��װ��ֱ�ӷ���
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_IDLE:
            if(pbuf!=NULL)
            {
                //�ͷŻ���
                MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
                pbuf  =  NULL;
            }
            break;
        case UCTSK_RFMS_E_RUNSTATE_START:
            if(pbuf!=NULL)
            {
                break;
            }
            //���뻺��
            pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
            pbuf1 =  &pbuf[512];
            //
            Rfms_s_DataSaveToTf.ErrCmt  =  0;
            //����adc�ļ�ͷ
            si16  =  0;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x59;
            pbuf1[si16++]   =  0x40;
            pbuf1[si16++]   =  0x02;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x01;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            pbuf1[si16++]   =  0x00;
            Rfms_s_DataSaveToTf.SaveSign  =  1;
            Rfms_s_DataSaveToTf.pData     =  pbuf1;
            Rfms_s_DataSaveToTf.DataLen   =  si16;
            si16 = 0;
            break;
        case UCTSK_RFMS_E_RUNSTATE_START_DELAY:
        case UCTSK_RFMS_E_RUNSTATE_GET:
            //���뻺��
            pbuf[si16++]   =  dat>>8;
            pbuf[si16++]   =  dat;
            //����TF
            if(si16>=512)
            {
                memcpy((char*)pbuf1,(char*)pbuf,si16);
                Rfms_s_DataSaveToTf.SaveSign  =  1;
                Rfms_s_DataSaveToTf.pData     =  pbuf1;
                Rfms_s_DataSaveToTf.DataLen   =  si16;
                si16 = 0;
            }
            break;
        case UCTSK_RFMS_E_RUNSTATE_STOP:
            if(pbuf==NULL)
            {
                break;
            }
            //����TF
            pbuf[si16++]=Rfms_s_DataSaveToTf.ErrCmt>>8;
            pbuf[si16++]=Rfms_s_DataSaveToTf.ErrCmt;
            memcpy((char*)pbuf1,(char*)pbuf,si16);
            Rfms_s_DataSaveToTf.SaveSign  =  1;
            Rfms_s_DataSaveToTf.pData     =  pbuf1;
            Rfms_s_DataSaveToTf.DataLen   =  si16;
            break;
        default:
            break;
    }
}
void uctsk_RfmsSetSleepLevelBeforeAlarm(void)
{
    if(uctsk_Rfms_s_DayInfo.SleepLevelBeforeAlarm == MODULE_MEMORY_E_SLEEPLEVEL_NULL \
       &&Uctsk_Rfms_RunState!=UCTSK_RFMS_E_RUNSTATE_IDLE)
    {
        uctsk_Rfms_s_DayInfo.SleepLevelBeforeAlarm = uctsk_CurrentSleepLevel;
    }
}
//

UCTSK_S_MOVE_POWER uctsk_s_MovePower;
static void Arithmetic_MovePower(uint8_t step,uint16_t dat)
{
    //static UCTSK_S_MOVE_POWER uctsk_s_MovePower;
    //
    uint16_t i16;
    static uint8_t s_step=0;
    static uint8_t snum=0;
    static uint16_t s_counter=0;
    static uint32_t s_value_sum=0;
    // ����2����֮�ڲ�����,����ͬ��ʱ�ӺͿ�Խ��ʼ����
    if(120>BspRtc_GetBeginToNowS())
    {
        return;
    }
    //
    if(s_step==0)
    {
        if(CurrentDate.tm_sec<=1)
        {
            s_step=1;
            memset((uint8_t*)&uctsk_s_MovePower,0,sizeof(uctsk_s_MovePower));
            uctsk_s_MovePower.Num=0;
            uctsk_s_MovePower.Time[0]=CurrentDate.tm_year-2000;
            uctsk_s_MovePower.Time[1]=CurrentDate.tm_mon+1;
            uctsk_s_MovePower.Time[2]=CurrentDate.tm_mday;
            uctsk_s_MovePower.Time[3]=CurrentDate.tm_hour;
            uctsk_s_MovePower.Time[4]=CurrentDate.tm_min;
            uctsk_s_MovePower.Time[5]=CurrentDate.tm_sec;
            // ƫ�ƴ洢
            Count_TimeOffset((uint8_t*)uctsk_s_MovePower.Time,(uint8_t*)uctsk_s_MovePower.Time,UCTSK_RFMS_KEEP_BODY_MOVE_TIME*3600);
            snum        =  0;
            s_counter   =  0;
            s_value_sum =  0;
        }
        else
        {
            return;
        }
    }
    // �ۼ�
    i16=abs(dat-UCTSK_RFMS_DATA_DC);
    s_value_sum += i16;
    s_counter += 1;
    // 10s�ۼ�ֵ(ʵ����9.9���ۼ�,��ֹʱ��ƫ��)
    if(s_counter>(100*10-10))
    {
        //-----���ݴ���
        uctsk_s_MovePower.Data[snum++]=s_value_sum;
        //
        if(s_value_sum<uctsk_Rfms_NoBodyThresholdValue)
        {
            uctsk_Rfms_NoBodyThresholdVTimerS   += 10;
        }
        else
        {
            uctsk_Rfms_NoBodyThresholdVTimerS   =  0;
        }
        //-----
        s_counter   =  0;
    }
    // ÿ60�����ڴ�������
    if(snum>=6)
    {
        //���ݴ��뻺�滷(����GPRS/UartIcam)
        Count_BufferLoopPush(&UctskRfms_BufferLoop1,(uint8_t*)&uctsk_s_MovePower,sizeof(uctsk_s_MovePower));
        s_step = 0;
        //���ݴ��뻺��
        Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_1MIN_W,(uint8_t*)uctsk_s_MovePower.Data,(uint8_t*)uctsk_s_MovePower.Time);
    }
}

//��ȡ����
uint8_t uctsk_Rfms_BufferLoop1Pop(uint8_t *pbuf,uint16_t *plen)
{
    return Count_BufferLoopPop(&UctskRfms_BufferLoop1,pbuf,plen,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
}
//------------------------------------------------------------------
/*
* ��������: ��ʱ����������
* ��    ��: *p_tm    ---   ʱ���
* ��    ��: *p_buf   ---   ����
*/
uint8_t uctsk_Rfms_KeepBodyMoveIndex(struct tm *p_tm,uint8_t *p_buf,uint8_t *index)
{
    struct tm *ptm1;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove;
    uint8_t i,res=ERR;
    int32_t i32;
    //
    memset((char*)p_buf,0,480);
    if(index!=NULL)
    {
        *index = 0xFF;
    }
    //���뻺��
    ptm1            =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    psDayInfo       =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    psDayBodyMove   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    for(i=0; i<=6; i++)
    {
        //��ȡ����
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&i);
        //У������
        if((psDayInfo->BodyMoveNum==0)||(psDayInfo->BodyMoveNum>250)||(psDayInfo->BeginByte!=0xCC))
        {
            continue;
        }
        else
        {
            ptm1->tm_year  =  Count_2ByteToWord(psDayInfo->BeginTime[0],psDayInfo->BeginTime[1]);
            ptm1->tm_mon   =  psDayInfo->BeginTime[2]-1;
            ptm1->tm_mday  =  psDayInfo->BeginTime[3];
            ptm1->tm_hour  =  psDayInfo->BeginTime[4];
            ptm1->tm_min   =  psDayInfo->BeginTime[5];
            ptm1->tm_sec   =  psDayInfo->BeginTime[6];
        }
        //У��ʱ��
        i32 =  Count_TimeCompare(ptm1,p_tm);
        if(0 > i32 || 24*3600 < i32)
        {
            continue;
        }
        //У�����ݳ���
        /*
        if(psDayInfo->BodyMoveNum>(i32/180))
        {
            continue;
        }
        */
        //��ȡ����
        res = OK;
        if(index!=NULL)
        {
            if(*index>6)
            {
                *index=i;
            }
        }
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,(uint8_t*)psDayBodyMove,&i);
        memcpy((char*)&p_buf[i32/180],psDayBodyMove->DataBuf,psDayInfo->BodyMoveNum);
        //���ݲ�ȫ
        if(p_buf[0]==0)
            p_buf[0]=p_buf[1];
        if(p_buf[239]==0)
            p_buf[239]=p_buf[238];
        if(p_buf[240]==0)
            p_buf[240]=p_buf[241];
        if(p_buf[479]==0)
            p_buf[479]=p_buf[478];
        //����У��(�ٳ����ӿ�ֵ)
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayBodyMove);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm1);
    return res;
}
/*
��������: ���ܼ��㿪ʼʱ�������ʱ��
��    ��:
*/
static uint8_t uctsk_Rfms_KeepBodyMove_AnalyzeSleepTime(uint8_t *p_buf,uint8_t *BeginTime,uint8_t *EndTime)
{
    p_buf       =  p_buf;
    BeginTime   =  BeginTime;
    EndTime     =  EndTime;
    BeginTime[0]  =  21-9;
    BeginTime[1]  =  0;
    BeginTime[2]  =  0;
    EndTime[0]    =  23;
    EndTime[1]    =  0;
    EndTime[2]    =  0;
    return OK;
}
uint8_t uctsk_Rfms_KeepBodyMove_GetFinalInfo(struct tm *p_tm,uint8_t *pOutbuf)
{
    struct tm *ptm;
    MODULE_MEMORY_S_DAY_INFO *psdayInfo;
    MODULE_MEMORY_S_DAY_SLEEPLEVEL *psdaySleepLevel;
    uint8_t *pdayBodyMove;
    uint8_t *pbuf;
    uint16_t i16,j16;
    //uint32_t i32;
    uint8_t i,res=OK;
    pbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //��ȡ�ڴ�����  -  i
    /*
    MODULE_MEMORY_S_PARA *pspara;
    pspara=(MODULE_MEMORY_S_PARA *)pbuf;
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    i = Count_SubCyc(pspara->MemoryDayNextP,1,MODULE_MEMORY_ADDR_DAY_NUM-1);
    */
    //��ȡ�嶯����  -  pbuf[0:479]
    ptm=(struct tm *)&pbuf[512];
    ptm->tm_year  =  p_tm->tm_year;
    ptm->tm_mon   =  p_tm->tm_mon;
    ptm->tm_mday  =  p_tm->tm_mday;
    ptm->tm_hour  =  UCTSK_RFMS_KEEP_BODY_MOVE_TIME;
    ptm->tm_min   =  0;
    ptm->tm_sec   =  0;
    uctsk_Rfms_KeepBodyMoveIndex(ptm,pbuf,&i);
    //û��������Ч�������˳�
    if(i>=7)
    {
        res = ERR;
        goto KeepBodyMove_GetFinalInfo;
    }
    //
    psdayInfo       =  (MODULE_MEMORY_S_DAY_INFO*)&pbuf[512];

    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psdayInfo,&i);
    //������˯ʱ��
    uctsk_Rfms_KeepBodyMove_AnalyzeSleepTime(pbuf,&pbuf[480],&pbuf[483]);
    //����ʱ��������±��볤��
    /*
    if(pbuf[480]<9)
    {
        i16=((pbuf[480]+(24-9))*3600+pbuf[481]*60+pbuf[482])/180;
    }
    else((pbuf[480]>=9) && (pbuf[480]<24))
    {
        i16=((pbuf[480]-9)*3600+pbuf[481]*60+pbuf[482])/180;
    }
    */
    // ��λ��Ч��������
    i16=(pbuf[480]*3600+pbuf[481]*60+pbuf[482])/180;
    // ���㿪ʼ˯��ʱ��
    /*
    ptm=(struct tm *)&pbuf[512+256];
    ptm->tm_year  =  p_tm->tm_year;
    ptm->tm_mon   =  p_tm->tm_mon;
    ptm->tm_mday  =  p_tm->tm_mday;
    ptm->tm_hour  =  pbuf[480];
    ptm->tm_min   =  pbuf[481];
    ptm->tm_sec   =  pbuf[482];
    i32=Count_Time_ConvCalendarToUnix(*ptm);
    i32=i32-((24-UCTSK_RFMS_KEEP_BODY_MOVE_TIME)*3600);
    *ptm =  Count_Time_ConvUnixToCalendar(i32);
    psdayInfo->BeginTime[0]  =  (uint8_t)(ptm->tm_year>>8);
    psdayInfo->BeginTime[1]  =  (uint8_t)(ptm->tm_year);
    psdayInfo->BeginTime[2]  =  p_tm->tm_mon+1;
    psdayInfo->BeginTime[3]  =  p_tm->tm_mday;
    psdayInfo->BeginTime[4]  =  p_tm->tm_hour;
    psdayInfo->BeginTime[5]  =  p_tm->tm_min;
    psdayInfo->BeginTime[6]  =  p_tm->tm_sec;
    // �������˯��ʱ��
    ptm->tm_year  =  p_tm->tm_year;
    ptm->tm_mon   =  p_tm->tm_mon;
    ptm->tm_mday  =  p_tm->tm_mday;
    ptm->tm_hour  =  pbuf[483];
    ptm->tm_min   =  pbuf[484];
    ptm->tm_sec   =  pbuf[485];
    i32=Count_Time_ConvCalendarToUnix(*ptm);
    i32=i32-((24-UCTSK_RFMS_KEEP_BODY_MOVE_TIME)*3600);
    *ptm =  Count_Time_ConvUnixToCalendar(i32);
    psdayInfo->EndTime[0]  =  (uint8_t)(ptm->tm_year>>8);
    psdayInfo->EndTime[1]  =  (uint8_t)(ptm->tm_year);
    psdayInfo->EndTime[2]  =  p_tm->tm_mon+1;
    psdayInfo->EndTime[3]  =  p_tm->tm_mday;
    psdayInfo->EndTime[4]  =  p_tm->tm_hour;
    psdayInfo->EndTime[5]  =  p_tm->tm_min;
    psdayInfo->EndTime[6]  =  p_tm->tm_sec;
    psdayInfo->EndTime[0]    =  0;
    */
    //
    psdayInfo->BodyMoveNum   =  ((pbuf[483]*3600+pbuf[484]*60+pbuf[485])\
                                 -(pbuf[480]*3600+pbuf[481]*60+pbuf[482]))/180;
    //
    pdayBodyMove = &pbuf[i16];
    //��ʼ��
    psdayInfo->BodyMoveCount[0]  =  0;
    psdayInfo->BodyMoveCount[1]  =  0;
    //
    psdaySleepLevel =  (MODULE_MEMORY_S_DAY_SLEEPLEVEL*)&pbuf[512+256];
    //�����㷨
    for(i16=0; i16<psdayInfo->BodyMoveNum; i16++)
    {
        if(pdayBodyMove[i16]>ARITHMETIC_BODY_MOVE_LEVEL_WAKE)
        {
            psdaySleepLevel->DataBuf[i16]=1;
            j16 = Count_2ByteToWord(psdayInfo->BodyMoveCount[0],psdayInfo->BodyMoveCount[1]);
            if(j16<127)
            {
                j16++;
                psdayInfo->BodyMoveCount[0]=j16>>8;
                psdayInfo->BodyMoveCount[1]=j16;
            }
        }
        else if(pdayBodyMove[i16]>ARITHMETIC_BODY_MOVE_LEVEL_LIGHT)
        {
            psdaySleepLevel->DataBuf[i16]=2;
        }
        else if(pdayBodyMove[i16]>ARITHMETIC_BODY_MOVE_LEVEL_MIDDLE)
        {
            psdaySleepLevel->DataBuf[i16]=3;
        }
        else
        {
            psdaySleepLevel->DataBuf[i16]=4;
        }
    }
    //����嶯����
    Count_MaxMinAve_Byte(pdayBodyMove,psdayInfo->BodyMoveNum,0x00,0xFF,\
                         &(psdayInfo->BodyMoveMax),NULL,NULL);
    //�˹���������
    Arithmetic_AI(psdaySleepLevel->DataBuf,psdayInfo->BodyMoveNum-1);
    //���ս��
    /*
    if(NULL!=pOutDayInfo)
    {
        memcpy(pOutDayInfo,psdayInfo,sizeof(MODULE_MEMORY_S_DAY_INFO));
    }
    if(NULL!=pOutDaySleepLevel)
    {
        memcpy(pOutDaySleepLevel,psdaySleepLevel,sizeof(MODULE_MEMORY_S_DAY_SLEEPLEVEL));
    }
    if(NULL!=pOutDayBodyMove)
    {
        memcpy(pOutDayBodyMove,pdayBodyMove,psdayInfo->BodyMoveNum);
    }
    */
    i16=0;
    pOutbuf[i16++]   =  psdayInfo->BodyMoveNum+4;
    pOutbuf[i16++]   =  (pbuf[480]+9)%24;
    pOutbuf[i16++]   =  pbuf[481];
    pOutbuf[i16++]   =  (pbuf[483]+9)%24;
    pOutbuf[i16++]   =  pbuf[484];
    memcpy(&pOutbuf[i16],psdaySleepLevel->DataBuf,psdayInfo->BodyMoveNum);
    //
KeepBodyMove_GetFinalInfo:
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    return res;
}
/*
static void Arithmetic_KeepBodyMove(uint16_t dat)
{
    //��һ�εĲ���ֵ
    static uint16_t s_dat_bak       =  0;
    static uint8_t step             =  0;
    static uint32_t s_count_ms      =  0;
    static uint32_t s_lowPowerSum   =  0;
    static uint32_t s_lowPowerNum   =  0;
    static uint32_t s_Sum           =  0;
    uint32_t i32;
    //-----10ms����
    static UCTSK_RFMS_S_KEEP_BODY_MOVE   *ps_KeepBodyMove =  NULL;
    uint32_t   result=0;

    // ��
    if(CurrentDate.tm_hour==16 && CurrentDate.tm_min==0 && step!=UCTSK_RFMS_E_RUNSTATE_START)
    {
        // ��ʼ
        step = UCTSK_RFMS_E_RUNSTATE_START;
    }
    //
    switch(step)
    {
        case UCTSK_RFMS_E_RUNSTATE_START:
            //���ݳ�ʼ��
            s_dat_bak                  =  0;
            s_count_ms                 =  0;
            break;
        case UCTSK_RFMS_E_RUNSTATE_GET:
            s_count_ms  += 10;
            //���ֵ�ж�
            if(dat>0x3FF)
            {
                dat = 0x3FF;
            }
            //�仯����,���˵�
            if(abs(s_dat_bak-dat)>0x1F0)
            {
                dat = s_dat_bak;
            }
            else
            {
                s_dat_bak=dat;
            }
            //ȥֱ��
            dat=abs(dat-UCTSK_RFMS_DATA_DC);
            //���ݷֶ�
            if(dat<68)
            {
                //���������
                s_lowPowerSum+=dat;
                s_lowPowerNum++;
            }
            //���ݻ���
            s_Sum += (uint32_t)(dat);
            if(s_count_ms%(180*1000)==0)
            {
                //���������ݴ���
                if(s_lowPowerNum)
                {
                    i32   =  s_lowPowerSum/s_lowPowerNum;
                    i32   *= 18000;
                }
                else
                {
                    i32   =  1;
                }
                //�����
                result=s_Sum/i32;
                if(result>250)
                {
                    result=250;
                }
                S_UctskRfms_KeepBodyMove.buf[s_count_ms/180*1000] =  result;
                //��ձ���
                s_lowPowerSum  =  0;
                s_lowPowerNum  =  0;
            }
            break;
        default:
            break;
    }
}
*/

//-----------------------------------------------------------------------------

