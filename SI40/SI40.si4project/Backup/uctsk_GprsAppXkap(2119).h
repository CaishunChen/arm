/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*
���ɻ���
   ��������ַ:10.32.144.101
   �û�������:ciadmin/ciadmin
   ���������ļ�,ͨ��SFTP����
      �����ļ�Ŀ¼:
      /opt/tomcat-iSleepSmartGateway-9090/webapps/iSleepSmartGateway/WEB-INF/classes/update
   ����Ӧ��SSH2����,ָ����������:
      ps -ef | grep tomcat-iSleepSmartGateway-9090
      kill -9 ���к�
      cd tomcat-iSleepSmartGateway-9090/bin
      ./startup.sh
*/
#ifndef __UCTSK_UCTSK_APP_XKAP_H
#define __UCTSK_UCTSK_APP_XKAP_H
//-------------------���ؿ⺯��------------------------------
#include "includes.h"
//-------------------------------------------------------------------------------�ӿں궨��
#define GPRSAPP_XKAP_CONNECT_ORDER_TEST            GPRSNET_E_CONNECT_ORDER_UDP_IP
#define GPRSAPP_XKAP_DNS_STR_TEST                  "59.46.22.16"
#define GPRSAPP_XKAP_PORT_STR_TEST                 "18080"

#if   (defined(PROJECT_XKAP_V3) || defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M)||defined(XKAP_ICARE_B_D_M))
//---->
//ͨ��
#define GPRSNETAPP_XKAP_CONNECT_CH        0
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//��ʽ
#define GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL       GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_FORMAL                "isleep.xikang.com"/*"ec2-54-223-115-152.cn-north-1.compute.amazonaws.com.cn"*/
#define GPRSAPP_XKAP_PORT_STR_FORMAL               "2012"
//����
#define GPRSNETAPP_XKAP_CONNECT_ORDER_INTEGRATION  GPRSNET_E_CONNECT_ORDER_UDP_IP
#define GPRSAPP_XKAP_DNS_STR_INTEGRATION           "59.46.22.16"
#define GPRSAPP_XKAP_PORT_STR_INTEGRATION          "2012"
//XK����
#define GPRSNETAPP_XKAP_CONNECT_ORDER_XK           GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_XK                    "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_XK                   "2012"
#elif (defined(XKAP_ICARE_A_M) || defined(XKAP_ICARE_B_M))
//��ʽ
#define GPRSNETAPP_XKAP_CONNECT_ORDER_FORMAL       GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_FORMAL                "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_FORMAL               "2012"
//����
#define GPRSNETAPP_XKAP_CONNECT_ORDER_INTEGRATION  GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_INTEGRATION           "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_INTEGRATION          "2012"
//XK����
#define GPRSNETAPP_XKAP_CONNECT_ORDER_XK           GPRSNET_E_CONNECT_ORDER_UDP_DNS
#define GPRSAPP_XKAP_DNS_STR_XK                    "dlisleep.xikang.com"
#define GPRSAPP_XKAP_PORT_STR_XK                   "2012"
#endif

typedef enum GPRSAPP_XKAP_E_CMD
{
    GPRSAPP_XKAP_E_CMD_NULL         =  0x00,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA1  =  0x01,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA2  =  0x02,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA3  =  0x03,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA4  =  0x04,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA5  =  0x05,
    GPRSAPP_XKAP_E_CMD_GET_TIME     =  0x06,
    GPRSAPP_XKAP_E_CMD_SEND_IMSI    =  0x07,
    GPRSAPP_XKAP_E_CMD_PARA_R       =  0x08,
    GPRSAPP_XKAP_E_CMD_PARA_W       =  0x09,
    GPRSAPP_XKAP_E_CMD_SEND_MOVEPOWER  =  0x0A,
    GPRSAPP_XKAP_E_CMD_PARA_UPLOAD  =  0x0B,
    GPRSAPP_XKAP_E_CMD_SOS          =  0x0C,
    GPRSAPP_XKAP_E_CMD_SEND_DAYMOVE =  0x0D,
    GPRSAPP_XKAP_E_CMD_SEND_INFO    =  0x0E,
    GPRSAPP_XKAP_E_CMD_SEND_ERR     =  0x0F,
    GPRSAPP_XKAP_E_CMD_IAP          =  0x60,
    //
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL,
    GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN,
} GPRSAPP_XKAP_E_CMD;
typedef struct GPRSAPP_XKAP_S
{
    GPRSAPP_XKAP_E_CMD cmd;
    uint8_t state; // 0-��Ч   1-��Ч
} GPRSAPP_XKAP_S;
typedef struct GPRSAPP_XKAP_S_WEATHER
{
    // ��Ч ��Ч
    uint8_t Flag_Valid     :1;
    // ���ݸ���
    uint8_t Flag_Updata    :1;
    // ������ȡ
    uint8_t Flag_Extract   :1;
    // ����
    uint8_t Flag_Res       :5;
    // ��ǰ��ȡʱ��
    uint8_t DataTimeBuf[6];
    // �¶�
    uint8_t TempCurrent;
    uint8_t TempMin;
    uint8_t TempMax;
    uint8_t TempBuf[24];
    // ����
    //   �� ���� ���� 
    //   С�� ���� ���� ���� ���� 
    //   Сѩ ��ѩ ��ѩ ��ѩ
    uint8_t Weather; 
    // ����(�� ���� �� ���� �� ���� �� ����)
    uint8_t WindDirection;
    // ����
    uint8_t WindSpeedMin;
    uint8_t WindSpeedMax;
    // ��������()
    uint8_t AirQuality;
    // �ճ�ʱ��(hh mm)
    uint8_t SunriseTime[2];
    // ����ʱ��(hh mm)
    uint8_t SunsetTime[2];
    // ũ��(����)
    uint8_t LunarCalendarDate[2];
    // ũ��(����)
    uint8_t LunarCalendar24;
} GPRSAPP_XKAP_S_WEATHER;
typedef struct GPRSAPP_XKAP_S_SOS
{
    uint8_t state; // 0-����   1-�����ϴ� 2-Ӧ��ɹ�
    uint8_t DateTime[6];
} GPRSAPP_XKAP_S_SOS;
typedef struct GPRSAPP_XKAP_SEND_INFO
{
    uint8_t state;         // 0-��  1-���ϴ� 2-Ӧ��ɹ�
    uint8_t DateTime[6];
    uint8_t Tag;
    uint8_t Value[8];
}GPRSAPP_XKAP_SEND_INFO;
typedef struct GPRSAPP_XKAP_SEND_ERR
{
    uint8_t state;         // 0-��  1-���ϴ� 2-Ӧ��ɹ�
    uint8_t DateTime[6];
    uint8_t Tag;
    uint8_t Value[8];
}GPRSAPP_XKAP_SEND_ERR;

//<----
#endif
//-------------------------------------------------------------------------------�ӿڳ���
//-------------------------------------------------------------------------------�ӿڱ���
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
//-------------------------------------------------------------------------------�ӿں���
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

