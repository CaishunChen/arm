/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*
集成环境
   服务器地址:10.32.144.101
   用户名密码:ciadmin/ciadmin
   更新升级文件,通过SFTP连接
      升级文件目录:
      /opt/tomcat-iSleepSmartGateway-9090/webapps/iSleepSmartGateway/WEB-INF/classes/update
   重启应用SSH2连接,指令序列如下:
      ps -ef | grep tomcat-iSleepSmartGateway-9090
      kill -9 序列号
      cd tomcat-iSleepSmartGateway-9090/bin
      ./startup.sh
*/
#ifndef __UCTSK_UCTSK_APP_XKAP_H
#define __UCTSK_UCTSK_APP_XKAP_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------------------------------------------------------------------接口宏定义
#define GPRSAPP_XKAP_CONNECT_ORDER_TEST            GPRSNET_E_CONNECT_ORDER_UDP_IP
#define GPRSAPP_XKAP_DNS_STR_TEST                  "59.46.22.16"
#define GPRSAPP_XKAP_PORT_STR_TEST                 "18080"

#if   (defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M)||defined(XKAP_ICARE_B_D_M))
//---->
//通道
#define GPRSNETAPP_XKAP_CONNECT_CH        0
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//正式
#define GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL       GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_FORMAL                "isleep.xikang.com"/*"ec2-54-223-115-152.cn-north-1.compute.amazonaws.com.cn"*/
#define GPRSAPP_XKAP_PORT_STR_FORMAL               "2012"
//集成
#define GPRSNETAPP_XKAP_CONNECT_ORDER_INTEGRATION  GPRSNET_E_CONNECT_ORDER_UDP_IP
#define GPRSAPP_XKAP_DNS_STR_INTEGRATION           "59.46.22.16"
#define GPRSAPP_XKAP_PORT_STR_INTEGRATION          "2012"
//XK环境
#define GPRSNETAPP_XKAP_CONNECT_ORDER_XK           GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_XK                    "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_XK                   "2012"
#elif (defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M))
//正式
#define GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL       GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_FORMAL                "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_FORMAL               "2012"
//集成
#define GPRSNETAPP_XKAP_CONNECT_ORDER_INTEGRATION  GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_INTEGRATION           "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_INTEGRATION          "2012"
//XK环境
#define GPRSNETAPP_XKAP_CONNECT_ORDER_XK           GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_XK                    "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_XK                   "2012"
#endif

typedef enum GPRSAPP_XKAP_E_CMD
{
    GPRSAPP_XKAP_E_CMD_NULL         =  	0x00,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA1  =  	0x01,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA2  =  	0x02,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA3  =  	0x03,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA4  =  	0x04,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA5  =  	0x05,
    GPRSAPP_XKAP_E_CMD_GET_TIME     =  	0x06,
    GPRSAPP_XKAP_E_CMD_SEND_IMSI    =  	0x07,
    GPRSAPP_XKAP_E_CMD_PARA_R       =  	0x08,
    GPRSAPP_XKAP_E_CMD_PARA_W       =  	0x09,
    GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER  =  0x0A,
    GPRSAPP_XKAP_E_CMD_PARA_UPLOAD  =  	0x0B,
    GPRSAPP_XKAP_E_CMD_SOS          =  	0x0C,
    GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE =  	0x0D,
    GPRSAPP_XKAP_E_CMD_SEND_INFO    =  	0x0E,
    GPRSAPP_XKAP_E_CMD_SEND_ERR     =  	0x0F,
    GPRSAPP_XKAP_E_CMD_PARA_R_1     =   0x10,
    GPRSAPP_XKAP_E_CMD_PARA_W_1     =   0x11,
    GPRSAPP_XKAP_E_CMD_IAP          =  	0x60,
    //
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN,
} GPRSAPP_XKAP_E_CMD;
typedef struct GPRSAPP_XKAP_S
{
    GPRSAPP_XKAP_E_CMD cmd;
    uint8_t state; // 0-无效   1-有效
} GPRSAPP_XKAP_S;
typedef struct GPRSAPP_XKAP_S_WEATHER
{
    // 无效 有效
    uint8_t Flag_Valid     :1;
    // 数据更新
    uint8_t Flag_Updata    :1;
    // 数据提取
    uint8_t Flag_Extract   :1;
    // 备用
    uint8_t Flag_Res       :5;
    // 当前获取时间
    uint8_t DataTimeBuf[6];
    // 温度
    uint8_t TempCurrent;
    uint8_t TempMin;
    uint8_t TempMax;
    uint8_t TempBuf[24];
    // 天气
    //   晴 多云 阴天
    //   小雨 中雨 大雨 暴雨 雷雨
    //   小雪 中雪 大雪 雨雪
    uint8_t Weather;
    // 风向(东 东南 南 西南 西 西北 北 东北)
    uint8_t WindDirection;
    // 风速
    uint8_t WindSpeedMin;
    uint8_t WindSpeedMax;
    // 空气质量()
    uint8_t AirQuality;
    // 日出时间(hh mm)
    uint8_t SunriseTime[2];
    // 日落时间(hh mm)
    uint8_t SunsetTime[2];
    // 农历(月日)
    uint8_t LunarCalendarDate[2];
    // 农历(节气)
    uint8_t LunarCalendar24;
} GPRSAPP_XKAP_S_WEATHER;
typedef struct GPRSAPP_XKAP_S_SOS
{
    uint8_t state; // 0-空闲   1-正在上传 2-应答成功
    uint8_t DateTime[6];
} GPRSAPP_XKAP_S_SOS;
typedef struct GPRSAPP_XKAP_SEND_INFO
{
    uint8_t state;         // 0-空  1-待上传 2-应答成功
    uint8_t DateTime[6];
    uint8_t Tag;
    uint8_t Value[8];
} GPRSAPP_XKAP_SEND_INFO;
typedef struct GPRSAPP_XKAP_SEND_ERR
{
    uint8_t state;         // 0-空  1-待上传 2-应答成功
    uint8_t DateTime[6];
    uint8_t Tag;
    uint8_t Value[8];
} GPRSAPP_XKAP_SEND_ERR;

//<----
#endif
//-------------------------------------------------------------------------------接口常量
//-------------------------------------------------------------------------------接口变量
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
extern uint32_t GprsAppXkap_TxNum;
extern uint32_t GprsAppXkap_RxNum;
extern uint8_t GprsAppXkap_UpdataState;
extern GPRSAPP_XKAP_E_CMD GprsAppXkap_RfmsCmd;
extern uint8_t GprsAppXkap_DataPacketIndex;
//
extern uint8_t Xkap_ConnectOrder;
extern char *Xkap_pConnectDnsStr;
extern char *Xkap_pConnectPortStr;
//
extern uint8_t GprsAppXkap_Sign_Login;
extern uint8_t  GprsAppXkap_Sign_ParaUpload;
//
extern uint16_t GprsAppXkap_MovePowerTxNum;
extern uint16_t GprsAppXkap_MovePowerRxNum;
//
extern GPRSAPP_XKAP_S_WEATHER GprsAppXkap_S_Weather;
extern GPRSAPP_XKAP_S_SOS GprsAppXkap_S_SOS;
#endif
//
extern GPRSAPP_XKAP_SEND_INFO GprsAppXkap_S_Info;
extern uint8_t GprsApp_Xkap_RtcReady;
//
extern uint16_t GprsApp_RxLastToNowS; 
//-------------------------------------------------------------------------------接口函数
extern  void  App_GprsAppXkapTaskCreate(void);
#if   (defined(PROJECT_XKAP_V3) \
   || defined(XKAP_ICARE_A_M) \
   || defined(XKAP_ICARE_B_M) \
     ||defined(XKAP_ICARE_B_D_M))
uint8_t GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD cmd);
void GprsNetAppXkap_Parse(uint8_t *pbuf,uint16_t len);
#endif
#endif

//---------------------END-------------------------------------------------------

