/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/

//------------------------------- Includes --------------------
#include "includes.h"
#include "uctsk_GprsAppXkap.h"
#include "uctsk_GprsNet.h"
//
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#include "Bsp_CpuId.h"
#include "Bsp_CpuFlash.h"
#include "Module_Memory.h"
#include "Bsp_Rtc.h"
#include "IAP.h"
#include "Bsp_BkpRam.h"
#include "Bsp_CpuFlash.h"
#include "uctsk_Sensor.h"
#elif (defined(XKAP_ICARE_A_M))
#include "Bsp_CpuId.h"
#include "Bsp_Rtc.h"
#include "Bsp_NrfRfEsb.h"
#define  UCTSK_S_MOVE_POWER   BSP_NRFRFESB_S_MOVEPOWER
#elif (defined(XKAP_ICARE_B_M))
#include "Bsp_CpuId.h"
#include "Bsp_Rtc.h"
#include "Module_Memory.h"
#include "uctsk_HCI.h"
#endif
//-------------------------------------------------------------宏定义(配置)
//-------------------------------------------------------------用户变量
MODULE_OS_TASK_TAB(App_GprsAppXkapTaskTCB);
MODULE_OS_TASK_STK(App_GprsAppXkapTaskStk,APP_TASK_GPRSAPPXKAP_STK_SIZE);
uint8_t GprsApp_Xkap_RtcReady=0;
uint16_t GprsApp_RxLastToNowS=0;
/******************************** 说明 ************************
文件功能：TCI服务器
功    能: 用于TCI对设备的管理
说    明: 1,256k芯片内部FLASH分配:0-15(bootload),16-17(主参数区),18-19(更改参数区),20-137(主程序区),138-255(升级程序区)
          2,TCI通讯协议见PC程序
**************************************************************/
/*
协议:
---------------------------HTTP-------------------------------
标签  名称           含义              数值范围(ASCII)
T1    ID             本机ID            XXXXXXXXXXXX
T2    Type           数据类型          1-定时
                                       2-按钮
                                       3-信息钮
T3    PeakCount      1分钟剧烈变化次数 X-XXX
                                       范围: 0～100
T4    AverageVoltage 1分钟平均电压(mV) X-XXXX
                                       范围: 0～3300
T5    Illum          实时照度（lx）    X～XXXXXX
                                       范围：0～300000
T6    Temp           实时温度(0.1度)   -XXX～XXXX
                                       范围：-55.0～125.0
                                       表示：-550～1250
T7    Humi           实时湿度(0.1%)    X～XXXX
                                       范围：0.0～100.0
                                       表示：0～1000
T8    RFID           射频信息钮数据    XXXXXXXXXXXX-XXX
                                       (ID-电压(X.XXv))
---------------------------TYPE && E.G.------------------------
一，定时数据实例（包含T1，T2，T3，T4，T5，T6，T7数据）
      T1=9E01000F0710&T2=1&T3=20&T4=1630&T5=824&T6=251&T7=402
      本机ID         9E:01:00:0F:07:10
      数据类型       定时
      1分钟变化次数  20次
      1分钟平均电压  1630 mV
      照度           824 Lx
      温度           25.1摄氏度
      湿度           40.2%
二，按键数据实例（包含T1，T2，T5，T6，T7数据）
      T1=9E01000F0710&T2=2&T5=824&T6=251&T7=402
      本机ID         9E:01:00:0F:07:10
      数据类型       按钮
      照度           824 Lx
      温度           25.1摄氏度
      湿度           40.2%
三，RFID数据实例（包含T1，T2，T8数据）
      T1=9E01000F0710&T2=3&T8=9E01000F0711-258
      本机ID         9E:01:00:0F:07:10
      数据类型       信息钮
      信息钮数据     信息钮ID：9E01000F0711 电压：2.58V
*/
#if   ( defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M)||defined(XKAP_ICARE_B_D_M))
//---->
extern MODULE_OS_Q(GsmAppTxQ);
//
uint8_t  GprsAppXkap_Sign_Login  =  0;
// 0-需要上传(实时) 1-需要上传(定时) 2-需要上传(08应答) 3-需要上传(10应答)  254-上传中 255-完成
uint8_t  GprsAppXkap_Sign_ParaUpload   =  0;
//
uint32_t GprsAppXkap_TxNum  =  0;
uint32_t GprsAppXkap_RxNum  =  0;
//
uint16_t GprsAppXkap_MovePowerTxNum =  0;
uint16_t GprsAppXkap_MovePowerRxNum =  0;
//<----
#endif
//--------------------------------------------------------------XKAP任务创建
static void uctsk_GprsAppXkap (void *pvParameters) ;
void  App_GprsAppXkapTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-GprsAppXkap",\
                          uctsk_GprsAppXkap,\
                          APP_TASK_GPRSAPPXKAP_PRIO,\
                          App_GprsAppXkapTaskStk,\
                          APP_TASK_GPRSAPPXKAP_STK_SIZE,\
                          App_GprsAppXkapTaskTCB,\
                          NULL);
}
//--------------------------------------------------------------XKAP平台任务实体
#if (defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M)||defined(XKAP_ICARE_B_D_M))
//------------------------------- 全局变量 -----------------------------------
#define GPRSAPPXKAP_Q_MAX     5
MODULE_OS_Q(GprsAppXkap_Q);
// 用与协议 类似流水号
static uint8_t Xkap_IsleepDataNum=0;
// 接收计时器,用于监控数据链路
static uint8_t Xkap_RxSTimer=0;
// 通常数据结果标识(0-初始值 1-正在上传 2-上传成功 3-上传失败 4-无有效数据不需上传)
uint8_t GprsAppXkap_UpdataState=0;
// Rfms监测结束后,由于网络需要重新连,所以需要置标志
GPRSAPP_XKAP_E_CMD GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
// 单组数据重传次数
static uint8_t Xkap_ReTxRxCount=0;
// 传输数据整包索引(1-7)
uint8_t GprsAppXkap_DataPacketIndex=0;
// 参数-地址,长度,结果
static uint16_t Xkap_ParaAddr;
static uint8_t Xkap_ParaLen;
static uint8_t Xkap_ParaRes;
static uint16_t Xkap_ServerSoftVer  =  SOFTWARE_VER*256+SOFTWARE_SUB_VER;
static uint16_t Xkap_ServerHardVer  =  HARDWARE_VER*256+HARDWARE_SUB_VER;
// 0-初始化 1-启动上传 2-上传成功
static uint8_t Xkap_SleepDataScanStep  =  0;
static uint8_t Xkap_SleepDataScanTimer =  0;
//
uint8_t Xkap_ConnectOrder     =  0;
char *Xkap_pConnectDnsStr     =  NULL;
char *Xkap_pConnectPortStr    =  NULL;

static uint8_t Xkap_TxTimeOutTimer  =  0;

GPRSAPP_XKAP_S_WEATHER  GprsAppXkap_S_Weather;
GPRSAPP_XKAP_S_SOS GprsAppXkap_S_SOS;

GPRSAPP_XKAP_SEND_INFO  GprsAppXkap_S_Info;
GPRSAPP_XKAP_SEND_ERR   GprsAppXkap_S_Err;
//------------------------------- 局部函数 -----------------------------------
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
static uint16_t Xkap_IapAllPacket      =  0;
static uint16_t Xkap_IapCurrentPacket  =  0;
static uint32_t Xkap_IapSum            =  0;
static uint16_t Xkap_IapNextPacket     =  1;
static uint8_t Xkap_IapTimer           =  0;
static uint8_t Xkap_IapErrCmt          =  0;
static uint16_t Xkap_IapErrRecoverTimer=  0;
static uint8_t Xkap_DayMoveOvertimeCmt =  0;
UCTSK_S_MOVE_POWER uctsk_s_MovePower1;
static void Xkap_IsleepData1(uint8_t index);
static void Xkap_IsleepData2(uint8_t index);
static void Xkap_IsleepData3(uint8_t index);
static void Xkap_IsleepData4(uint8_t index);
static void Xkap_IsleepData5(uint8_t index);
static void Xkap_SendDayMove(void);
static void Xkap_GetIapData(void);
static void Xkap_SendIMSI(void);
static void GprsNetAppXkap_Mark(uint8_t index);
static void Xkap_SendMovePower(UCTSK_S_MOVE_POWER *psMovePower);
#elif   (defined(XKAP_ICARE_A_M))
UCTSK_S_MOVE_POWER uctsk_s_MovePower1;
static void Xkap_SendMovePower(UCTSK_S_MOVE_POWER *psMovePower);
#elif   (defined(XKAP_ICARE_B_M))
#endif
static void Xkap_SendPara(uint8_t cmd);
static void Xkap_SendInfo(GPRSAPP_XKAP_SEND_INFO *psInfo);
static void Xkap_SendErr(GPRSAPP_XKAP_SEND_ERR *psErr);
static void Xkap_GetTime(void);
static void Xkap_ParseAckPara(GPRSAPP_XKAP_E_CMD cmd);
static void Xkap_SendSOS(void);

//------------------------------- 函数实体 -----------------------------------
uint8_t GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD cmd)
{
    uint8_t i;
    static GPRSAPP_XKAP_S GprsAppXkap_SBuf[GPRSAPPXKAP_Q_MAX];
    //上传数据特殊处理
    if(cmd == GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL\
       ||cmd == GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)
    {
        GprsAppXkap_RfmsCmd =  cmd;
        Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=1;
        GprsAppXkap_UpdataState     =  0;
        GprsAppXkap_DataPacketIndex =  1;
        return OK;
    }
    //获取空消息空间
    for(i=0; i<GPRSAPPXKAP_Q_MAX; i++)
    {
        if(GprsAppXkap_SBuf[i].state==0)
        {
            break;
        }
    }
    if(i>=GPRSAPPXKAP_Q_MAX)
    {
        return ERR;
    }
    GprsAppXkap_SBuf[i].state  =  1;
    GprsAppXkap_SBuf[i].cmd    =  cmd;
    //发送消息队列
    MODULE_OS_Q_POST(GprsAppXkap_Q,GprsAppXkap_SBuf[i],sizeof(GPRSAPP_XKAP_S));
    return OK;
}

INT8_S_BIT s_first_up;
static void uctsk_GprsAppXkap (void *pvParameters)
{
    //初始化
    static GPRSAPP_XKAP_S sGprsAppXkapS;
    GPRSAPP_XKAP_E_CMD cmd=GPRSAPP_XKAP_E_CMD_NULL;
    GPRSAPP_XKAP_S *ps;
    uint16_t i16=0,j16=0;
    uint8_t cmt1=0;
    uint8_t res=0;

    uint8_t timingCounter=0;
    uint16_t heart_up_timer=0;
    MODULE_OS_ERR err;
    MODULE_OS_Q_CREATE(GprsAppXkap_Q,"GsmAppXkap_Q",GPRSAPPXKAP_Q_MAX);
    //变量初始化
    j16=j16;
    res=res;
    Xkap_ServerHardVer=Xkap_ServerHardVer;
    Xkap_SleepDataScanStep  =  Xkap_SleepDataScanStep;
    Xkap_SleepDataScanTimer =  Xkap_SleepDataScanTimer;
    memset((char*)&GprsAppXkap_S_Weather,0,sizeof(GprsAppXkap_S_Weather));
    memset((char*)&GprsAppXkap_S_Info,0,sizeof(GprsAppXkap_S_Info));
    s_first_up.bit0=0;
    s_first_up.bit1=0;
    s_first_up.bit2=1;
    s_first_up.bit3=0;
    s_first_up.bit4=0;
    s_first_up.bit5=0;
    s_first_up.bit6=0;
    s_first_up.bit7=0;
    //等待初始化完成
    MODULE_OS_DELAY_MS(10000);
    //网络连接
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
    {
        MODULE_MEMORY_S_PARA *pspara;
        //申请缓存
        pspara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //数据包
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
        GprsAppXkap_Sign_Login=pspara->Flag_GprsRegister;
        if(pspara->GprsServer==0)
        {
            Xkap_ConnectOrder    =  GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL;
            Xkap_pConnectDnsStr  =  GPRSAPP_XKAP_DNS_STR_FORMAL;
            Xkap_pConnectPortStr =  GPRSAPP_XKAP_PORT_STR_FORMAL;
        }
        else if(pspara->GprsServer==1)
        {
            Xkap_ConnectOrder    =  GPRSNETAPP_XKAP_CONNECT_ORDER_INTEGRATION;
            Xkap_pConnectDnsStr  =  GPRSAPP_XKAP_DNS_STR_INTEGRATION;
            Xkap_pConnectPortStr =  GPRSAPP_XKAP_PORT_STR_INTEGRATION;
        }
        else if(pspara->GprsServer==2)
        {
            Xkap_ConnectOrder    =  GPRSNETAPP_XKAP_CONNECT_ORDER_XK;
            Xkap_pConnectDnsStr  =  GPRSAPP_XKAP_DNS_STR_XK;
            Xkap_pConnectPortStr =  GPRSAPP_XKAP_PORT_STR_XK;
        }
        else
        {
            Xkap_ConnectOrder    =  GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL;
            Xkap_pConnectDnsStr  =  GPRSAPP_XKAP_DNS_STR_FORMAL;
            Xkap_pConnectPortStr =  GPRSAPP_XKAP_PORT_STR_FORMAL;
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
    }
#else
    GprsAppXkap_Sign_Login = 1;
    Xkap_ConnectOrder    =  GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL;
    Xkap_pConnectDnsStr  =  GPRSAPP_XKAP_DNS_STR_FORMAL;
    Xkap_pConnectPortStr =  GPRSAPP_XKAP_PORT_STR_FORMAL;
#endif
    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state =  GPRSNET_E_CONNECT_STATE_IDLE;
    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].order =  Xkap_ConnectOrder;
    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].pIpDnmain   =  (uint8_t *)Xkap_pConnectDnsStr;
    strcpy((char *)(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].Port),Xkap_pConnectPortStr);
    //等待网络连接
    while(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)
    {
        MODULE_OS_DELAY_MS(1000);
    }
    i16 = 3600;
    for(;;)
    {
#if 0
        MODULE_OS_DELAY_MS(1000);
        Xkap_Heart();
#else
        //GSM接收总解析
        MODULE_OS_Q_PEND(ps,GprsAppXkap_Q,1000,TRUE,err);
        if(err==MODULE_OS_ERR_NONE)
        {
            if(ps->state==0)
            {
                continue;
            }
            GprsAppXkap_TxNum++;
            cmd = ps->cmd;
            switch(ps->cmd)
            {
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA1:
                    Xkap_IsleepData1(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA2:
                    Xkap_IsleepData2(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA3:
                    Xkap_IsleepData3(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA4:
                    Xkap_IsleepData4(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA5:
                    Xkap_IsleepData5(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE:
                    Xkap_SendDayMove();
                    break;
                case GPRSAPP_XKAP_E_CMD_IAP:
                    Xkap_IapTimer=0;
                    Xkap_GetIapData();
                    break;
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL:
                case GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN:
                    Xkap_IsleepData1(GprsAppXkap_DataPacketIndex);
                    break;
                case GPRSAPP_XKAP_E_CMD_SEND_IMSI:
                    Xkap_SendIMSI();
                    break;
#endif
#if   (defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_B_D_M))
                case GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER:
                    Xkap_SendMovePower(&uctsk_s_MovePower1);
                    break;
#endif
                case GPRSAPP_XKAP_E_CMD_PARA_R:
                case GPRSAPP_XKAP_E_CMD_PARA_R_1:
                case GPRSAPP_XKAP_E_CMD_PARA_UPLOAD:
                    Xkap_SendPara(cmd);
                    break;
                case GPRSAPP_XKAP_E_CMD_SEND_INFO:
                    Xkap_SendInfo(&GprsAppXkap_S_Info);
                    break;
                case GPRSAPP_XKAP_E_CMD_SEND_ERR:
                    Xkap_SendErr(&GprsAppXkap_S_Err);
                    break;
                case GPRSAPP_XKAP_E_CMD_GET_TIME:
                    Xkap_GetTime();
                    break;
                case GPRSAPP_XKAP_E_CMD_PARA_W:
                case GPRSAPP_XKAP_E_CMD_PARA_W_1:
                    Xkap_ParseAckPara(ps->cmd);
                    break;
                case GPRSAPP_XKAP_E_CMD_SOS:
                    Xkap_SendSOS();
                    break;
                default:
                    break;
            }
            ps->state = 0;
        }
        else if(err == MODULE_OS_ERR_TIMEOUT)
        {
            //
            if(GprsApp_RxLastToNowS!=0xFFFF)
            {
                GprsApp_RxLastToNowS++;
            }
            i16++;
            heart_up_timer++;
            if(i16>60000)
            {
                i16=1;
                heart_up_timer=1;
            }
            //
            if(i16%3600==1)
            {
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                GprsAppXkap_S_Err.state=1;
                memcpy(GprsAppXkap_S_Err.DateTime,BspRtc_CurrentTimeBuf,6);
                GprsAppXkap_S_Err.Tag=1;
                memset(GprsAppXkap_S_Err.Value,0,sizeof(GprsAppXkap_S_Err.Value));
                if(Sensor_Err_Temp==1||Sensor_Err_Humi==1)
                {
                    GprsAppXkap_S_Err.Value[0]|=1;
                }
                if(uctsk_Rfms_Err==1)
                {
                    GprsAppXkap_S_Err.Value[0]|=2;
                }
#endif
            }
            //超时处理
            if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState==1)
            {
                Xkap_TxTimeOutTimer++;
                if(Xkap_TxTimeOutTimer>=10)
                {
                    Xkap_TxTimeOutTimer  =  0;
                    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState =  0;
                }
                continue;
            }
            //如果未注册则注册
            if(GprsAppXkap_Sign_Login==0)
            {
                if((i16%5==0) && GprsAppXkap_UpdataState==0)
                {
                    GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_IMSI);
                }
            }
            //信息上传
            else if(GprsAppXkap_S_Info.state==1 && GprsAppXkap_UpdataState==0)
            {
                GprsAppXkap_S_Info.state = 2;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_INFO);
            }
            //参数上传---实时上传
            else if(GprsAppXkap_Sign_ParaUpload==0 && GprsAppXkap_UpdataState==0)
            {
                GprsAppXkap_Sign_ParaUpload=1;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_PARA_UPLOAD);
            }
            //参数上传---定时上传
#if	(defined(XKAP_ICARE_B_M))          
			else if((i16%30==0||s_first_up.bit0==0) && GprsAppXkap_Sign_ParaUpload==1 && GprsAppXkap_UpdataState==0)
#else
            else if((i16%3600==0||s_first_up.bit0==0) && GprsAppXkap_Sign_ParaUpload==1 && GprsAppXkap_UpdataState==0)
#endif				
            {
                s_first_up.bit0=1;
                GprsAppXkap_Sign_ParaUpload=1;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_PARA_UPLOAD);
            }
            //参数上传---应答上传
            else if(GprsAppXkap_Sign_ParaUpload==2 && GprsAppXkap_UpdataState==0)
            {
                GprsAppXkap_Sign_ParaUpload=1;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_PARA_R);
            }
            //参数上传---应答上传
            else if(GprsAppXkap_Sign_ParaUpload==3 && GprsAppXkap_UpdataState==0)
            {
                GprsAppXkap_Sign_ParaUpload=1;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_PARA_R_1);
            }
            //故障上传
            else if(GprsAppXkap_S_Err.state==1 && GprsAppXkap_UpdataState==0)
            {
                GprsAppXkap_S_Err.state = 2;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_ERR);
            }
            //每分钟一次(作为心跳)
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
            else if(((Xkap_ServerSoftVer<=SOFTWARE_VER*256+SOFTWARE_SUB_VER)||(Xkap_IapErrCmt>60)) && GprsAppXkap_UpdataState==0)
#else
            else if(((Xkap_ServerSoftVer<=SOFTWARE_VER*256+SOFTWARE_SUB_VER)) && GprsAppXkap_UpdataState==0)
#endif
            {
                while(1)
                {
                    //每5s提取一次体动数据，有则上传
                    if(i16%5==0)
                    {
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                        res = uctsk_Rfms_BufferLoop1Pop((uint8_t*)&uctsk_s_MovePower1,&j16);
                        //提取数据
                        if(OK==res && j16==sizeof(uctsk_s_MovePower1))
#elif (defined(XKAP_ICARE_A_M))
                        if(1)
#else
                        if(0)
#endif
                        {
                            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER);
                            break;
                        }
                    }
#if   (defined(XKAP_ICARE_B_M))
                    //前3次5秒间隔，之后每60s同步一下时钟
                    if(((heart_up_timer>=3600)&&(timingCounter>=3))\
						||((heart_up_timer>=60)&&(GprsApp_Xkap_RtcReady==0)&&(timingCounter>=3)))
#else
                    //前3次5秒间隔，之后每3600s同步一下时钟
                    if((heart_up_timer>=3600)&&(timingCounter>=3))
#endif
                    {
                        heart_up_timer  =   0;
                        GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_GET_TIME);
                        break;
                    }
                    else if((heart_up_timer>=5)&&(timingCounter<3))
                    {
                        timingCounter++;
                        heart_up_timer  =   0;
                        GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_GET_TIME);
                        break;
                    }
                    //每30分钟,上传全日体动数据
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
                    if(ModuleMemory_psPara->Flag_GprsUploadDayMove==ON)
                    {
                        if((BspRtc_CurrentTimeBuf[4]%30==0)&&(s_first_up.bit1==0))
                        {
                            s_first_up.bit1=1;
                            s_first_up.bit2=1;
                            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE);
                        }
                        else if((BspRtc_CurrentTimeBuf[4]%30!=0)&&(s_first_up.bit1==1))
                        {
                            s_first_up.bit1=0;
                        }
                        else if(s_first_up.bit2==1 && BspRtc_CurrentTimeBuf[5]%5==0 && Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState==0)
                        {
                            /*
                            if(Xkap_DayMoveOvertimeCmt>5)
                            {
                                Xkap_DayMoveOvertimeCmt=0;
                                s_first_up.bit2=0;
                            }
                            else
                            */
                            {
                                //查看是否传递完毕
                                if(ERR==Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_VALID_R,NULL,NULL))
                                {
                                    s_first_up.bit2=0;
                                }
                                else
                                {
                                    GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE);
                                }
                            }
                        }
                    }
#else
#endif
                    break;
                }
            }
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
            //IAP每5秒无数据则重新激发
            else if(Xkap_IapTimer>=5  && GprsAppXkap_UpdataState==0 \
                    && (Xkap_IapCurrentPacket<Xkap_IapAllPacket || Xkap_IapAllPacket==0) \
                    && (Xkap_ServerSoftVer>SOFTWARE_VER*256+SOFTWARE_SUB_VER) \
                    && (Xkap_SleepDataScanStep==2) \
                    && (Xkap_IapErrCmt<=60))
            {
                Xkap_IapTimer=0;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_IAP);
                if(Xkap_IapErrCmt<0xFF)
                {
                    Xkap_IapErrCmt++;
                }
                Xkap_IapErrRecoverTimer=0;
            }
            // 5秒重发机制
            else if(Xkap_IapTimer<5)
            {
                Xkap_IapTimer++;
            }
            //恢复错误表示: 1小时后可以重新开始IAP
            if(Xkap_IapErrCmt>=60)
            {
                if(++Xkap_IapErrRecoverTimer>=3600)
                {
                    Xkap_IapErrRecoverTimer=0;
                    Xkap_IapErrCmt=0;
                }
            }
#endif
            //补传
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
            if(Xkap_SleepDataScanTimer>=30 && Xkap_SleepDataScanStep==1)
            {
                Xkap_SleepDataScanTimer=0;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN);
            }
            else if(Xkap_SleepDataScanTimer<30)
            {
                Xkap_SleepDataScanTimer++;
            }
#endif
            // 10s内未成功上传n包数据,置位错误标识
            if(GprsAppXkap_UpdataState==1)
            {
                cmt1++;
                if(cmt1>10)
                {
                    //重传三次
                    if(Xkap_ReTxRxCount<3)
                    {
                        //重新发送
                        switch(cmd)
                        {
                            case GPRSAPP_XKAP_E_CMD_SLEEP_DATA1:
                            case GPRSAPP_XKAP_E_CMD_SLEEP_DATA2:
                            case GPRSAPP_XKAP_E_CMD_SLEEP_DATA3:
                            case GPRSAPP_XKAP_E_CMD_SLEEP_DATA4:
                            case GPRSAPP_XKAP_E_CMD_SLEEP_DATA5:
                                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA1);
                                break;
                            default:
                                break;
                        }
                        //计数器累加
                        Xkap_ReTxRxCount++;
                        //
                        cmt1=0;
                    }
                    else
                    {
                        //置位错误标识
                        GprsAppXkap_UpdataState=3;
                        GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
                        Xkap_ReTxRxCount = 0;
                        GprsAppXkap_DataPacketIndex =  0;
                        //释放发送状态
                        Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
                    }
                }
            }
            else
            {
                cmt1 = 0;
            }
            // 连续2分钟未接到新数据,则重新建立连接
            Xkap_RxSTimer++;
            if(Xkap_RxSTimer>=120)
            {
                Xkap_RxSTimer = 0;
                //Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state =  GPRSNET_E_CONNECT_STATE_CONNECTING;
                Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state =  GPRSNET_E_CONNECT_STATE_IDLE;
                Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].order =  Xkap_ConnectOrder;
            }
            // RFMS监测完毕,并且模块连接网络,则可以上传数据
            if((GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL || GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)\
               && (Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state==GPRSNET_E_CONNECT_STATE_CONNECTED)\
               && (GprsAppXkap_UpdataState==0))
            {
                //协议流水
                Xkap_IsleepDataNum++;
                //上传状态标识
                GprsAppXkap_UpdataState=1;
                //发送消息队列
                sGprsAppXkapS.state =  1;
                sGprsAppXkapS.cmd   =  GprsAppXkap_RfmsCmd;
                MODULE_OS_Q_POST(GprsAppXkap_Q,sGprsAppXkapS,sizeof(GPRSAPP_XKAP_S));
            }
        }
#endif
    }
}
#if   (defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_B_D_M))
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
static void Xkap_IsleepData1(uint8_t index)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    uint16_t i16,crc16;
    uint8_t res;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf       =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    pspara     =  (MODULE_MEMORY_S_PARA*)&pbuf[256*2];
    psDayInfo  =  (MODULE_MEMORY_S_DAY_INFO*)&pbuf[256*3];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SLEEP_DATA1;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  Xkap_IsleepDataNum;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
    memcpy((char*)&pbuf[i16],(char*)&(psDayInfo->BeginByte),69);
    i16+=69;
    //经纬度赋值
    pbuf[i16++]   =  psDayInfo->GPS_Latitude>>24;
    pbuf[i16++]   =  psDayInfo->GPS_Latitude>>16;
    pbuf[i16++]   =  psDayInfo->GPS_Latitude>>8;
    pbuf[i16++]   =  psDayInfo->GPS_Latitude;
    pbuf[i16++]   =  psDayInfo->GPS_Longitude>>24;
    pbuf[i16++]   =  psDayInfo->GPS_Longitude>>16;
    pbuf[i16++]   =  psDayInfo->GPS_Longitude>>8;
    pbuf[i16++]   =  psDayInfo->GPS_Longitude;
    //基站数据
    pbuf[i16++]   =  psDayInfo->Gsm_LacCi[0];
    pbuf[i16++]   =  psDayInfo->Gsm_LacCi[1];
    pbuf[i16++]   =  psDayInfo->Gsm_LacCi[2];
    pbuf[i16++]   =  psDayInfo->Gsm_LacCi[3];
    //标志
    pbuf[i16]   =  psDayInfo->Flag_DataValid;
    res           =  psDayInfo->Flag_StartMode;
    res<<=1;
    pbuf[i16]     += res;
    res           =  psDayInfo->Flag_StopMode;
    res<<=2;
    pbuf[i16]     += res;
    i16++;
    //赋值结束字节
    pbuf[i16++]   =  psDayInfo->EndByte;
    //补齐100个字节
    memset((char*)&pbuf[i16],0,100-83);
    i16+=100-83;
    //无效数据处理
    if(psDayInfo->BeginByte!=0xcc)
    {
        if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL)
        {
            if(GprsAppXkap_DataPacketIndex>=2&&GprsAppXkap_DataPacketIndex<=7)
            {
                GprsAppXkap_UpdataState = 2;
            }
            else
            {
                GprsAppXkap_UpdataState = 4;
            }
        }
        else if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)
        {
            Xkap_SleepDataScanStep  =  2;
            GprsAppXkap_UpdataState =  0;
            /*
            if(GprsAppXkap_DataPacketIndex>=2&&GprsAppXkap_DataPacketIndex<=7)
            {
                GprsAppXkap_UpdataState = 2;
            }
            else
            {
                GprsAppXkap_UpdataState = 4;
            }
            */
        }
        GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
        Xkap_ReTxRxCount = 0;
        Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
        GprsAppXkap_DataPacketIndex=0;
    }
    else
    {
        if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN && psDayInfo->UpdataToServerCmt!=0)
        {
            if(GprsAppXkap_DataPacketIndex<7)
            {
                GprsAppXkap_DataPacketIndex++;
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA1);
            }
            else
            {

                GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
                Xkap_ReTxRxCount = 0;
                Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
                GprsAppXkap_DataPacketIndex=0;
                //
                Xkap_SleepDataScanStep    =  2;
                GprsAppXkap_UpdataState   =  0;
            }
        }
        else
        {
            //校验
            crc16 = Count_CRC16(pbuf,i16);
            pbuf[i16++]   =  (uint8_t)(crc16);
            pbuf[i16++]   =  (uint8_t)(crc16>>8);
            //
            pGprs_s_tx->type     =  TYPE_TX_GPRS;
            pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
            pGprs_s_tx->buf      =  pbuf;
            pGprs_s_tx->len      =  i16;
            MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
            //等待传输完毕
            while(pGprs_s_tx->type!=TYPE_TX_NULL)
            {
                MODULE_OS_DELAY_MS(10);
            }
#if (GPRSNET_ATE==ON)
            MODULE_OS_DELAY_MS(200);
#else
#endif
        }
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_IsleepData2(uint8_t index)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_SLEEPLEVEL *psDaySleepLevel;
    uint16_t i16,crc16;
    uint8_t res;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    pspara     =  (MODULE_MEMORY_S_PARA*)&pbuf[256*2];
    psDaySleepLevel  =  (MODULE_MEMORY_S_DAY_SLEEPLEVEL*)&pbuf[256*3];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SLEEP_DATA2;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  Xkap_IsleepDataNum;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,(uint8_t*)psDaySleepLevel,&res);
    memcpy((char*)&pbuf[i16],psDaySleepLevel->DataBuf,241);
    i16+=241;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_IsleepData3(uint8_t index)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_BREATH *psDayBreath;
    uint16_t i16,crc16;
    uint8_t res;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    pspara     =  (MODULE_MEMORY_S_PARA*)&pbuf[256*2];
    psDayBreath=  (MODULE_MEMORY_S_DAY_BREATH*)&pbuf[256*3];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SLEEP_DATA3;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  Xkap_IsleepDataNum;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_R,(uint8_t*)psDayBreath,&res);
    memcpy((char*)&pbuf[i16],psDayBreath->DataBuf,241);
    i16+=241;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_IsleepData4(uint8_t index)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_HEARTRATE *psDayHeartRate;
    uint16_t i16,crc16;
    uint8_t res;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    pspara     =  (MODULE_MEMORY_S_PARA*)&pbuf[256*2];
    psDayHeartRate  =  (MODULE_MEMORY_S_DAY_HEARTRATE*)&pbuf[256*3];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SLEEP_DATA4;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  Xkap_IsleepDataNum;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,(uint8_t*)psDayHeartRate,&res);
    memcpy((char*)&pbuf[i16],psDayHeartRate->DataBuf,241);
    i16+=241;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_IsleepData5(uint8_t index)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove;
    uint16_t i16,crc16;
    uint8_t res;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    pspara     =  (MODULE_MEMORY_S_PARA*)&pbuf[256*2];
    psDayBodyMove  =  (MODULE_MEMORY_S_DAY_BODYMOVE*)&pbuf[256*3];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SLEEP_DATA5;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  Xkap_IsleepDataNum;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,(uint8_t*)psDayBodyMove,&res);
    memcpy((char*)&pbuf[i16],psDayBodyMove->DataBuf,241);
    i16+=241;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_SendDayMove(void)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[800];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_R,&pbuf[i16],NULL);
    i16+=(6+6+720);
    //---开始时间6B
    //memset(&pbuf[i16],0,6);
    //i16+=6;
    //---结束时间6B
    //memset(&pbuf[i16],0,6);
    //i16+=6;
    //---数据
    //memset(&pbuf[i16],0,720);
    //i16+=720;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(500);
#else
    MODULE_OS_DELAY_MS(500);
#endif
    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=1;
    Xkap_TxTimeOutTimer  =  0;
    Xkap_DayMoveOvertimeCmt++;
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    //GprsAppXkap_UpdataState=1;
}

static void Xkap_GetIapData(void)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    if(BspExtFlash_s_Flash.TotalSize==0)
    {
        return;
    }
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_IAP;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---硬件版本
    pbuf[i16++]   =  Xkap_ServerHardVer>>8;
    pbuf[i16++]   =  Xkap_ServerHardVer;
    //---软件版本
    pbuf[i16++]   =  Xkap_ServerSoftVer>>8;
    pbuf[i16++]   =  Xkap_ServerSoftVer;
    //---最小缺失包
    pbuf[i16++]   =  Xkap_IapNextPacket>>8;
    pbuf[i16++]   =  Xkap_IapNextPacket;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    Xkap_ParaAddr =  0;
    Xkap_ParaLen  =  0;
    Xkap_ParaRes  =  0;
    //GprsAppXkap_UpdataState=1;
}
/*******************************************************************************
* 函数功能: 标记上传
* 参    数: index ---   1-7
*******************************************************************************/
void GprsNetAppXkap_Mark(uint8_t index)
{
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    uint8_t res;
    //
    if(index<1 || index>7)
    {
        return;
    }
    //申请缓存
    pspara     =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    psDayInfo  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //数据包
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,index,MODULE_MEMORY_ADDR_DAY_NUM-1);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
    if(psDayInfo->BeginByte==0xCC && psDayInfo->UpdataToServerCmt==0)
    {
        psDayInfo->UpdataToServerCmt=1;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_W,(uint8_t*)psDayInfo,&res);
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
}
static void Xkap_SendIMSI(void)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SEND_IMSI;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---硬件版本
    pbuf[i16++]   =  HARDWARE_VER;
    pbuf[i16++]   =  HARDWARE_SUB_VER;
    //---软件版本
    pbuf[i16++]   =  SOFTWARE_VER;
    pbuf[i16++]   =  SOFTWARE_SUB_VER;
    //---IMSI
    memcpy((char*)&pbuf[i16],(char*)GprsNet_s_Info.IMSI,15);
    i16+=15;
    //---手机号
    memcpy((char*)&pbuf[i16],(char*)GprsNet_s_Info.PhoneNumber,11);
    i16+=11;
    //---IMEI
    memcpy((char*)&pbuf[i16],(char*)GprsNet_s_Info.IMEI,15);
    i16+=15;
    //---MAC
    memset((char*)&pbuf[i16],0,6);
    i16+=6;
    //---备用
    memset((char*)&pbuf[i16],0,10);
    i16+=10;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    GprsAppXkap_UpdataState=1;
}
#endif
/*******************************************************************************
* 函数功能: 上传体动能量数据
*******************************************************************************/
uint16_t uctsk_Xkap_ExRfmsTxNumBuf[6]= {0};
static void Xkap_SendMovePower(UCTSK_S_MOVE_POWER *psMovePower)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //
    GprsAppXkap_MovePowerTxNum++;
    //
    if(psMovePower->Num<6)
    {
        uctsk_Xkap_ExRfmsTxNumBuf[psMovePower->Num]++;
    }
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---时间戳
    pbuf[i16++]   =  psMovePower->Time[0];
    pbuf[i16++]   =  psMovePower->Time[1];
    pbuf[i16++]   =  psMovePower->Time[2];
    pbuf[i16++]   =  psMovePower->Time[3];
    pbuf[i16++]   =  psMovePower->Time[4];
    pbuf[i16++]   =  psMovePower->Time[5];
    //---类型
    pbuf[i16++]   =  0x01;
    //---编号
    pbuf[i16++]   =  psMovePower->Num;
    //---数据
    pbuf[i16++]   =  psMovePower->Data[0]>>24;
    pbuf[i16++]   =  psMovePower->Data[0]>>16;
    pbuf[i16++]   =  psMovePower->Data[0]>>8;
    pbuf[i16++]   =  psMovePower->Data[0];
    pbuf[i16++]   =  psMovePower->Data[1]>>24;
    pbuf[i16++]   =  psMovePower->Data[1]>>16;
    pbuf[i16++]   =  psMovePower->Data[1]>>8;
    pbuf[i16++]   =  psMovePower->Data[1];
    pbuf[i16++]   =  psMovePower->Data[2]>>24;
    pbuf[i16++]   =  psMovePower->Data[2]>>16;
    pbuf[i16++]   =  psMovePower->Data[2]>>8;
    pbuf[i16++]   =  psMovePower->Data[2];
    pbuf[i16++]   =  psMovePower->Data[3]>>24;
    pbuf[i16++]   =  psMovePower->Data[3]>>16;
    pbuf[i16++]   =  psMovePower->Data[3]>>8;
    pbuf[i16++]   =  psMovePower->Data[3];
    pbuf[i16++]   =  psMovePower->Data[4]>>24;
    pbuf[i16++]   =  psMovePower->Data[4]>>16;
    pbuf[i16++]   =  psMovePower->Data[4]>>8;
    pbuf[i16++]   =  psMovePower->Data[4];
    pbuf[i16++]   =  psMovePower->Data[5]>>24;
    pbuf[i16++]   =  psMovePower->Data[5]>>16;
    pbuf[i16++]   =  psMovePower->Data[5]>>8;
    pbuf[i16++]   =  psMovePower->Data[5];
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
#endif
static void Xkap_SendPara(uint8_t cmd)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  cmd;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    if(cmd==GPRSAPP_XKAP_E_CMD_PARA_R_1)
    {
        pbuf[i16++]   =  HARDWARE_VER;
        pbuf[i16++]   =  HARDWARE_SUB_VER;
    }
#if (defined(PROJECT_XKAP_V3))
    //---硬件版本
    pbuf[i16++]   =  HARDWARE_VER;
    pbuf[i16++]   =  HARDWARE_SUB_VER;
    //---软件版本
    pbuf[i16++]   =  SOFTWARE_VER;
    pbuf[i16++]   =  SOFTWARE_SUB_VER;
    //---Boot版本
    pbuf[i16++]   =  ModuleMemory_psPara->BootVer;
    pbuf[i16++]   =  0;
    //---上电次数
    crc16 = BSP_BKPRAM_READ(BSP_BKPRAM_POWER_CMT);
    pbuf[i16++]   =  crc16;
    pbuf[i16++]   =  crc16>>8;
    //---温度
    pbuf[i16++]   =  Sensor_Temp;
    pbuf[i16++]   =  Sensor_Temp>>8;
    //---湿度
    pbuf[i16++]   =  Sensor_Humi;
    pbuf[i16++]   =  Sensor_Humi>>8;
    //---年月
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[0];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[1];
    //---日时
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[2];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[3];
    //---分秒
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[4];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[5];
    //---闹钟1 开启关闭
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarmMode[0];
    pbuf[i16++]   =  0;
    //---闹钟1 设定时间
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarm[0];
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarm[0]>>8;
    //---闹钟2 开启关闭
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarmMode[1];
    pbuf[i16++]   =  0;
    //---闹钟2 设定时间
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarm[1];
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAlarm[1]>>8;
    //---自动检测 开启关闭
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAutoOnOff;
    pbuf[i16++]   =  0;
    //---自动检测 开启时间
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAutoBeginTime;
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAutoBeginTime>>8;
    //---自动检测 关闭时间
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAutoEndTime;
    pbuf[i16++]   =  ModuleMemory_psPara->RtcAutoEndTime>>8;
    //---入睡提醒 开启关闭
    pbuf[i16++]   =  ModuleMemory_psPara->RtcWarnOnOff[0];
    pbuf[i16++]   =  0;
    //---入睡提醒 设定时间
    pbuf[i16++]   =  ModuleMemory_psPara->RtcWarnTime[0];
    pbuf[i16++]   =  ModuleMemory_psPara->RtcWarnTime[0]>>8;
    //---入睡背光 通常背光
    pbuf[i16++]   =  ModuleMemory_psPara->LcdBackLight2;
    pbuf[i16++]   =  ModuleMemory_psPara->LcdBackLight1;
    //---GPRS发送
    pbuf[i16++]   =  GprsAppXkap_TxNum;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>8;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>16;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>24;
    //---GPRS接收
    pbuf[i16++]   =  GprsAppXkap_RxNum;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>8;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>16;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>24;
    //---总流量
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>8;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>16;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>24;
    //---信号强度
    pbuf[i16++]   =  GprsNet_s_Info.SignalVal;
    //---FLASH型号
    switch(BspExtFlash_s_Flash.ChipID)
    {
        case BSP_EXTFLASH_SST25VF016B_ID:
            pbuf[i16++] =  1;
            break;
        case BSP_EXTFLASH_SST25VF064C_ID:
            pbuf[i16++] =  2;
            break;
        case BSP_EXTFLASH_MX25L1606E_ID:
            pbuf[i16++] =  3;
            break;
        case BSP_EXTFLASH_W25Q80DV_ID:
            pbuf[i16++] =  4;
            break;
        case BSP_EXTFLASH_W25Q16BV_ID:
            pbuf[i16++] =  5;
            break;
        case BSP_EXTFLASH_W25Q32BV_ID:
            pbuf[i16++] =  6;
            break;
        case BSP_EXTFLASH_W25Q64BV_ID:
            pbuf[i16++] =  7;
            break;
        default:
            pbuf[i16++] =  0;
            break;
    }
    //
    //---上电次数
    crc16 = BSP_BKPRAM_READ(BSP_BKPRAM_BELL_TIME_H);
    pbuf[i16++]   =  crc16;
    pbuf[i16++]   =  crc16>>8;
    //
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue1>>24;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue1>>16;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue1>>8;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue1;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>24;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>16;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>8;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2;
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0;
#elif   (defined(XKAP_ICARE_B_M))
    //---硬件版本
    pbuf[i16++]   =  HARDWARE_VER;
    pbuf[i16++]   =  HARDWARE_SUB_VER;
    //---软件版本
    pbuf[i16++]   =  SOFTWARE_VER;
    pbuf[i16++]   =  SOFTWARE_SUB_VER;
    //---Boot版本
    pbuf[i16++]   =  ModuleMemory_psPara->BootVer;
    pbuf[i16++]   =  0;
    //---上电次数
    crc16 = 0;
    pbuf[i16++]   =  crc16;
    pbuf[i16++]   =  crc16>>8;
    //---温度
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0>>8;
    //---湿度
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0>>8;
    //---年月
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[0];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[1];
    //---日时
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[2];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[3];
    //---分秒
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[4];
    pbuf[i16++]   =  BspRtc_CurrentTimeBuf[5];
    //---GPRS发送
    pbuf[i16++]   =  GprsAppXkap_TxNum;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>8;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>16;
    pbuf[i16++]   =  GprsAppXkap_TxNum>>24;
    //---GPRS接收
    pbuf[i16++]   =  GprsAppXkap_RxNum;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>8;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>16;
    pbuf[i16++]   =  GprsAppXkap_RxNum>>24;
    //---总流量
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>8;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>16;
    pbuf[i16++]   =  GprsNet_s_Info.Flow_B>>24;
    //---信号强度
    pbuf[i16++]   =  GprsNet_s_Info.SignalVal;
    //---FLASH型号
    switch(BspExtFlash_s_Flash.ChipID)
    {
        case BSP_EXTFLASH_SST25VF016B_ID:
            pbuf[i16++] =  1;
            break;
        case BSP_EXTFLASH_SST25VF064C_ID:
            pbuf[i16++] =  2;
            break;
        case BSP_EXTFLASH_MX25L1606E_ID:
            pbuf[i16++] =  3;
            break;
        case BSP_EXTFLASH_W25Q80DV_ID:
            pbuf[i16++] =  4;
            break;
        case BSP_EXTFLASH_W25Q16BV_ID:
            pbuf[i16++] =  5;
            break;
        case BSP_EXTFLASH_W25Q32BV_ID:
            pbuf[i16++] =  6;
            break;
        case BSP_EXTFLASH_W25Q64BV_ID:
            pbuf[i16++] =  7;
            break;
        default:
            pbuf[i16++] =  0;
            break;
    }
    //
    //---上电次数
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0;
    //---空床/在床校准值
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>8;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>16;
    pbuf[i16++]   =  ModuleMemory_psPara->ThreshsholdValue2>>24;
    //---基于空床校准的在床判断偏移量
    pbuf[i16++]   =  ModuleMemory_psPara->Offset_H[0];
    //---基于空床校准的离床判断偏移量
    pbuf[i16++]   =  ModuleMemory_psPara->Offset_L[0];
    //---基于在床校准的在床判断偏移量
    pbuf[i16++]   =  ModuleMemory_psPara->Offset_H[1];
    //---基于在床校准的离床判断偏移量
    pbuf[i16++]   =  ModuleMemory_psPara->Offset_L[1];
    //---夜灯响应采集时长-100ms
    pbuf[i16++]   =  ModuleMemory_psPara->Offset_TimerS;
    //---夜灯响应时长-1s
    pbuf[i16++]   =  ModuleMemory_psPara->Reponse_TimerS;
    pbuf[i16++]   =  ModuleMemory_psPara->Reponse_TimerS>>8;
    //---照明灯功能开关(0-关闭 1-开启)
    pbuf[i16++]   =  ModuleMemory_psPara->Flag_NightLight;
    //---呼叫器连接状态(0-断开,1-连接)
    pbuf[i16++]   =  0;
    //---故障位图
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0;
    pbuf[i16++]   =  0;
	//---前30s的重量原始数据
	Hci_GetWeight30s(&pbuf[i16]);
	i16+=121;
	//---
#endif
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    //Xkap_ParaAddr =  0;
    //Xkap_ParaLen  =  0;
    Xkap_ParaRes  =  0;
    //GprsAppXkap_UpdataState=1;
}

static void Xkap_SendInfo(GPRSAPP_XKAP_SEND_INFO *psInfo)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[200];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SEND_INFO;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---硬件版本号
    pbuf[i16++]   =  HARDWARE_VER;
    pbuf[i16++]   =  HARDWARE_SUB_VER;
    //---时间戳
    pbuf[i16++]   =  psInfo->DateTime[0];
    pbuf[i16++]   =  psInfo->DateTime[1];
    pbuf[i16++]   =  psInfo->DateTime[2];
    pbuf[i16++]   =  psInfo->DateTime[3];
    pbuf[i16++]   =  psInfo->DateTime[4];
    pbuf[i16++]   =  psInfo->DateTime[5];
    //---标签
    pbuf[i16++]   =  psInfo->Tag;
    //---数据
    pbuf[i16++]   =  psInfo->Value[0];
    pbuf[i16++]   =  psInfo->Value[1];
    pbuf[i16++]   =  psInfo->Value[2];
    pbuf[i16++]   =  psInfo->Value[3];
    pbuf[i16++]   =  psInfo->Value[4];
    pbuf[i16++]   =  psInfo->Value[5];
    pbuf[i16++]   =  psInfo->Value[6];
    pbuf[i16++]   =  psInfo->Value[7];
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}

static void Xkap_SendErr(GPRSAPP_XKAP_SEND_ERR *psErr)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[200];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SEND_ERR;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---时间戳
    pbuf[i16++]   =  psErr->DateTime[0];
    pbuf[i16++]   =  psErr->DateTime[1];
    pbuf[i16++]   =  psErr->DateTime[2];
    pbuf[i16++]   =  psErr->DateTime[3];
    pbuf[i16++]   =  psErr->DateTime[4];
    pbuf[i16++]   =  psErr->DateTime[5];
    //---标签
    pbuf[i16++]   =  psErr->Tag;
    //---数据
    pbuf[i16++]   =  psErr->Value[0];
    pbuf[i16++]   =  psErr->Value[1];
    pbuf[i16++]   =  psErr->Value[2];
    pbuf[i16++]   =  psErr->Value[3];
    pbuf[i16++]   =  psErr->Value[4];
    pbuf[i16++]   =  psErr->Value[5];
    pbuf[i16++]   =  psErr->Value[6];
    pbuf[i16++]   =  psErr->Value[7];
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}

static void Xkap_GetTime(void)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_GET_TIME;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包(无)
    //---硬件版本
    pbuf[i16++]   =  HARDWARE_VER;
    pbuf[i16++]   =  HARDWARE_SUB_VER;
    //---软件版本
    pbuf[i16++]   =  SOFTWARE_VER;
    pbuf[i16++]   =  SOFTWARE_SUB_VER;
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}
static void Xkap_SendSOS(void)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  GPRSAPP_XKAP_E_CMD_SOS;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    //---时间
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[0];
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[1];
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[2];
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[3];
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[4];
    pbuf[i16++]   =  GprsAppXkap_S_SOS.DateTime[5];
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    //GprsAppXkap_UpdataState=1;
}
static void Xkap_ParseAckPara(GPRSAPP_XKAP_E_CMD cmd)
{
    uint8_t *pbuf;
    GSM_STRUCT1 *pGprs_s_tx;
    uint16_t i16,crc16;
    //
    if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)return;
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //
    pGprs_s_tx =  (GSM_STRUCT1*)&pbuf[400];
    i16 = 0;
    //功能码
    pbuf[i16++]   =  cmd;
    //MCU-ID
    BspCpuId_Get(&pbuf[i16],NULL);
    i16+=12;
    //整包序号
    pbuf[i16++]   =  0;
    //数据包
    if(cmd==GPRSAPP_XKAP_E_CMD_PARA_W_1)
    {
        pbuf[i16++]   =  HARDWARE_VER;
        pbuf[i16++]   =  HARDWARE_SUB_VER;
    }
    if(Xkap_ParaRes==0)
    {
        //---起始地址
        pbuf[i16++]   =  Xkap_ParaAddr;
        pbuf[i16++]   =  Xkap_ParaAddr;
        //---字节长度
        pbuf[i16++]   =  Xkap_ParaLen;
    }
    else
    {
        //---错误码
        pbuf[i16++]   =  Xkap_ParaRes;
    }
    //校验
    crc16 = Count_CRC16(pbuf,i16);
    pbuf[i16++]   =  (uint8_t)(crc16);
    pbuf[i16++]   =  (uint8_t)(crc16>>8);
    //
    pGprs_s_tx->type     =  TYPE_TX_GPRS;
    pGprs_s_tx->para[0]  =  GPRSNETAPP_XKAP_CONNECT_CH+'0';
    pGprs_s_tx->buf      =  (uint8_t*)pbuf;
    pGprs_s_tx->len      =  i16;
    MODULE_OS_Q_POST (GsmAppTxQ,*pGprs_s_tx,sizeof(GSM_STRUCT1));
    //等待传输完毕
    while(pGprs_s_tx->type!=TYPE_TX_NULL)
    {
        MODULE_OS_DELAY_MS(10);
    }
#if (GPRSNET_ATE==ON)
    MODULE_OS_DELAY_MS(200);
#else
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
    //
    Xkap_ParaAddr =  0;
    Xkap_ParaLen  =  0;
    Xkap_ParaRes  =  0;
    //GprsAppXkap_UpdataState=1;
}
/*******************************************************************************
* 函数功能: 解析远程升级监控服务器数据
* 说    明: 由于HTTP为短连接,设备不能被动接受数据,只能主动查询数据,目前无此应用
*******************************************************************************/
//uint8_t *pTest1=NULL;
//uint16_t Test16=0;
void GprsNetAppXkap_Parse(uint8_t *pBuf,uint16_t len)
{
    uint16_t crc1,crc2;
    struct tm *ptm1,*ptm2;
    uint8_t *pbuf1;
    //
    pbuf1=pbuf1;
    //验证长度
    if(len<16)
    {
        return;
    }
    //验证CRC
    crc1 = Count_CRC16(pBuf,len-2);
    crc2 = Count_2ByteToWord(pBuf[len-1],pBuf[len-2]);
    if(crc1!=crc2)
    {
        return;
    }
    GprsAppXkap_RxNum++;
    Xkap_RxSTimer = 0;
    GprsApp_RxLastToNowS=0;
    //解析
    switch(pBuf[0])
    {
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        case GPRSAPP_XKAP_E_CMD_SLEEP_DATA1:
            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA2);
            break;
        case GPRSAPP_XKAP_E_CMD_SLEEP_DATA2:
            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA3);
            break;
        case GPRSAPP_XKAP_E_CMD_SLEEP_DATA3:
            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA4);
            break;
        case GPRSAPP_XKAP_E_CMD_SLEEP_DATA4:
            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA5);
            break;
        case GPRSAPP_XKAP_E_CMD_SLEEP_DATA5:
            GprsNetAppXkap_Mark(GprsAppXkap_DataPacketIndex);
            if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL||GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)
            {
                if(GprsAppXkap_DataPacketIndex<7)
                {
                    GprsAppXkap_DataPacketIndex++;
                    GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA1);
                    if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)
                    {
                        Xkap_SleepDataScanTimer = 0;
                    }
                }
                else
                {
                    GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
                    Xkap_ReTxRxCount = 0;
                    Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
                    GprsAppXkap_DataPacketIndex=0;
                    if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN)
                    {
                        Xkap_SleepDataScanStep  =  2;
                        GprsAppXkap_UpdataState =  0;
                    }
                    else
                    {
                        GprsAppXkap_UpdataState =  2;
                    }
                }
            }
            break;
		case GPRSAPP_XKAP_E_CMD_SEND_IMSI:
            GprsAppXkap_UpdataState = 2;
            GprsAppXkap_RfmsCmd=GPRSAPP_XKAP_E_CMD_NULL;
            Xkap_ReTxRxCount = 0;
            Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
            GprsAppXkap_DataPacketIndex =  0;
            crc1=0;
            //写标志
            {
                MODULE_MEMORY_S_PARA *pPara;
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
                if(pPara->Flag_GprsRegister==0 && pBuf[14]==0)
                {
                    crc1 = 1;
                    pPara->Flag_GprsRegister=1;
                }
                if(len>=(14+1+24+2))
                {
                    for(crc2=0; crc2<24; crc2++)
                    {
                        if(pPara->FactoryID[crc2]!=pBuf[15+crc2])
                        {
                            break;
                        }
                    }
                    if(crc2<24)
                    {
                        crc1 = 1;
                        memcpy((char*)pPara->FactoryID,&pBuf[15],24);
                    }
                }
                if(crc1==1)
                {
                    Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
                }
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            }
            GprsAppXkap_Sign_Login=1;
            break;	
        case GPRSAPP_XKAP_E_CMD_IAP:
            if(len<=14+1+2)
            {
                //设备重启
                MCU_SOFT_RESET;
            }
            Xkap_IapErrCmt          =  0;
            //解析数据
            //---硬件版本  14 15
            //---软件版本  16 17
            //---应答使能  18
            //---总包数    19 20
            Xkap_IapAllPacket       =  Count_2ByteToWord(pBuf[19],pBuf[20]);
            //---当前包    21 22
            Xkap_IapCurrentPacket   =  Count_2ByteToWord(pBuf[21],pBuf[22]);
            //---校验和    23 24 25 26
            Xkap_IapSum             =  Count_4ByteToLong(pBuf[23],pBuf[24],pBuf[25],pBuf[26]);
            //---数据包    27

            IAP_APP(IAP_APP_CMD_DATA,
                    &pBuf[27],
                    len-27-2,
                    Xkap_IapAllPacket,
                    Xkap_IapCurrentPacket,
                    Xkap_IapSum,
                    1,
                    &Xkap_IapNextPacket);

            /*
            Xkap_IapNextPacket   =  10;
            */

            if(Xkap_IapNextPacket==0xFFFE)
            {
                Xkap_ServerSoftVer        =  SOFTWARE_VER*256+SOFTWARE_SUB_VER;
                GprsAppXkap_UpdataState   =  0;
            }
            else
            {
                //准备再要数据
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_IAP);
            }
            break;
        case GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE:
            //根据回应设定已发标志
            Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_W,&pBuf[14],NULL);
            //
            Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].TxState=0;
            Xkap_DayMoveOvertimeCmt=0;
            break;
#endif
        case GPRSAPP_XKAP_E_CMD_SEND_INFO:
            if(GprsAppXkap_S_Info.state==1)
            {
                GprsAppXkap_S_Info.state=2;
            }
#if   (defined(XKAP_ICARE_B_M))
            if(len!=14+19+2)
            {
                break;
            }
            else
            {
                MODULE_MEMORY_S_PARA *pPara;
                uint32_t i32;
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                //---硬件版本号--------------2B
                //pBuf[14]  -   pBuf[15]
                //参数写入
                //---时钟--------------------6B
                //pBuf[16]  -   pBuf[21]
                ptm1 = (struct tm *)pPara;
                ptm2 = &ptm1[1];
                BspRtc_ReadRealTime(NULL,ptm1,NULL,NULL);
                Count_TimeTypeConvert(4,ptm2,&pBuf[16]);
                if(10<abs(Count_TimeCompare(ptm1,ptm2)))
                {
                    BspRtc_SetRealTime(NULL,NULL,NULL,&pBuf[16]);
                }
                //---
                crc1=0;
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                //---空床(在床)校准值--------4B
                //pBuf[22]  -   pBuf[25]
                i32=Count_4ByteToLong(pBuf[25], pBuf[24], pBuf[23], pBuf[22]);
                if((i32!=0) && (i32!=0xFFFFFFFF) && (i32!=pPara->ThreshsholdValue2))
                {
                    pPara->ThreshsholdValue2=i32;
                    crc1=1;
                }
                //---空床偏移量-在床---------1B
                //pBuf[26]
                if((pBuf[26]!=0) && (pBuf[26]!=0xFF) && (pBuf[26]!=pPara->Offset_H[0]))
                {
                    pPara->Offset_H[0]=pBuf[26];
                    crc1=1;
                }
                //---空床偏移量-离床---------1B
                //pBuf[27]
                if((pBuf[27]!=0) && (pBuf[27]!=0xFF) && (pBuf[27]!=pPara->Offset_L[0]))
                {
                    pPara->Offset_L[0]=pBuf[27];
                    crc1=1;
                }
                //---在床偏移量-在床---------1B
                //pBuf[28]
                if((pBuf[28]!=0) && (pBuf[28]!=0xFF) && (pBuf[28]!=pPara->Offset_H[1]))
                {
                    pPara->Offset_H[1]=pBuf[28];
                    crc1=1;
                }
                //---在床偏移量-离床---------1B
                //pBuf[29]
                if((pBuf[29]!=0) && (pBuf[29]!=0xFF) && (pBuf[29]!=pPara->Offset_L[1]))
                {
                    pPara->Offset_L[1]=pBuf[29];
                    crc1=1;
                }
                //---夜灯响应采集时长(100ms)-1B
                //pBuf[30]
                if((pBuf[30]!=0) && (pBuf[30]!=0xFF) && (pBuf[30]!=pPara->Offset_TimerS))
                {
                    pPara->Offset_TimerS=pBuf[30];
                    crc1=1;
                }
                //---夜灯响应时长(秒)--------2B
                //pBuf[31]  -   pBuf[32]
                crc2=Count_2ByteToWord(pBuf[32], pBuf[31]);
                if(crc2!=pPara->Reponse_TimerS)
                {
                    pPara->Reponse_TimerS=crc2;
                    crc1=1;
                }
                //
                if(crc1==1)
                {
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                }
                //
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            }
#endif
            break;
        case GPRSAPP_XKAP_E_CMD_PARA_W:
        case GPRSAPP_XKAP_E_CMD_PARA_W_1:
            if(pBuf[0]==GPRSAPP_XKAP_E_CMD_PARA_W)
            {
                pbuf1   =   &pBuf[14];
            }
            else
            {
                pbuf1   =   &pBuf[14+2];
            }
            Xkap_ParaAddr  =  Count_2ByteToWord(pbuf1[1],pbuf1[0]);
            Xkap_ParaLen   =  pbuf1[2];
            //参数写入
            //参数存储
            if(1)
            {
                MODULE_MEMORY_S_PARA *pPara;
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                memcpy((char*)pPara,(char*)ModuleMemory_psPara,sizeof(MODULE_MEMORY_S_PARA));
                Xkap_ParaRes   =  0;
                if(Xkap_ParaAddr==0x0016 && Xkap_ParaLen==8)
                {
                    Xkap_ParaRes    =   1;
                    pPara->ThreshsholdValue1=Count_4ByteToLong(pbuf1[3],pbuf1[4],pbuf1[5],pbuf1[6]);
                    pPara->ThreshsholdValue1=Count_4ByteToLong(pbuf1[7],pbuf1[8],pbuf1[9],pbuf1[10]);
                }
                else if(Xkap_ParaAddr==0x000D && Xkap_ParaLen==6)
                {
                    Xkap_ParaRes    =   1;
                    pPara->RtcAutoOnOff     =   Count_2ByteToWord(pbuf1[3], pbuf1[4]);
                    pPara->RtcAutoBeginTime =   Count_2ByteToWord(pbuf1[5], pbuf1[6]);
                    pPara->RtcAutoEndTime   =   Count_2ByteToWord(pbuf1[7], pbuf1[8]);
                }
                if(Xkap_ParaRes==1)
                {
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                }
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            }
            Xkap_ParaRes=0;
            //指令回应
            GprsAppXkap_WrCmd((GPRSAPP_XKAP_E_CMD)pBuf[0]);
            break;

        case GPRSAPP_XKAP_E_CMD_SEND_ERR:
            if(GprsAppXkap_S_Err.state==1)
            {
                GprsAppXkap_S_Err.state=2;
            }
            break;
        case GPRSAPP_XKAP_E_CMD_GET_TIME:
            /*
            if(len!=14+10+2)
            {
                break;
            }
            */
            ptm1 = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
			ptm2 = &ptm1[1];
            //时间差大于10s再修正时钟
            BspRtc_ReadRealTime(NULL,ptm1,NULL,NULL);
            Count_TimeTypeConvert(4,ptm2,&pBuf[14]);
            //秒差大于10,则修改时钟
            if((10<abs(Count_TimeCompare(ptm1,ptm2)))
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
               &&(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())
#endif
              )
            {
                BspRtc_SetRealTime(NULL,NULL,NULL,&pBuf[14]);
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm1);
            //判断升级包
            if(len>=14+10+2)
            {
                Xkap_ServerHardVer = pBuf[20]*256+pBuf[21];
                Xkap_ServerSoftVer = pBuf[22]*256+pBuf[23];
            }
            if(Xkap_SleepDataScanStep==0)
            {
                Xkap_SleepDataScanStep =  1;
                Xkap_SleepDataScanTimer=  0xFF;
            }
            GprsApp_Xkap_RtcReady = 1;
            break;
        case GPRSAPP_XKAP_E_CMD_PARA_R:
            //Xkap_ParaAddr  =  Count_2ByteToWord(pBuf[15],pBuf[14]);
            //Xkap_ParaLen   =  pBuf[16];
            Xkap_ParaRes   =  0;
            //数据回应
            GprsAppXkap_Sign_ParaUpload=2;
            break;
        case GPRSAPP_XKAP_E_CMD_PARA_R_1:
            Xkap_ParaRes   =  0;
            GprsAppXkap_Sign_ParaUpload=3;
        case GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER:
            //清除本条标识
            GprsAppXkap_MovePowerRxNum++;
            break;
        case GPRSAPP_XKAP_E_CMD_PARA_UPLOAD:
            GprsAppXkap_Sign_ParaUpload=1;
            break;
        case GPRSAPP_XKAP_E_CMD_SOS:
            if(GprsAppXkap_S_SOS.state==1)
            {
                GprsAppXkap_S_SOS.state=2;
            }
            break;
        default:
            break;
    }
}
#endif
//------------------------------------------------------------

