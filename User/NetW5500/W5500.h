//-------------------------------------------------------------------------------//
//                    ��������: RealView MDK-ARM Version 5.10                    //
//                    �� �� ��: RealView MDK-ARM Version 5.10                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: W5500.h                                          //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-06-26    �汾:  0.3                         //
//-------------------------------------------------------------------------------//
/**********************************************************************************
 * һ,��ֲ����:
 *    1. ����Ӳ���ӿ�(GPIO+SPI)
 *    2. ��W5500_Main_100ms()����100ms���ں���
 *    3. ��W5500_IRQHandler()�������жϺ���
 *    4. ��ɱ��ļ���"�û�����"
 * ��,�汾˵��
 *    V0.1  2014-08-07  ʵ�ֻ����������˼���UDP.TCP�ͻ���ͨѶ
 *    V0.2  2014-08-08  �Ż����ݽ��ս�������,������ָ�뺯�����龫�����.
 *    V0.3  2014-08-08  ʵ��DNS��������(��Ҫ����DNSXsl.c/.h)
 * ��,��ʵ�ֵĹ���
 *    1. telnet
 *    2. DHCP
 *    3. ftp�ͻ���
 * ��,����δ�����Bug
 *    1. XXX
**********************************************************************************/
//-------------------------------------------------------------------------------
#ifndef __W5500_H
#define __W5500_H
//-------------------------------------------------------------------------------
#ifdef __W5500APP_H
//-------------------------------------------------------------------------------
#include <DNS_Xsl.h>

#ifndef STM32
     #include "user.h"
#endif
//--------------------------------------------------------------------------------�û�����-----Begin
//----------W5500 GPIO����

#ifndef STM32               //PIC����W5500���Ŷ���
    //����W5500��CS����
    #define W5500_SCS       _LATG9
    #define W5500_RST       _LATG10
    #define W5500_INT       _RC9
#else
    //����W5500��CS����
    #define W5500_SCS       GPIO_Pin_15
    #define W5500_SCS_PORT  GPIOA
    //����W5500��RST����
    #define W5500_RST       GPIO_Pin_6
    #define W5500_RST_PORT  GPIOF
    //����W5500��INT����
    #define W5500_INT       GPIO_Pin_7
    #define W5500_INT_PORT  GPIOF
#endif

//----------8��ͨ���ĺ�����������(��Ҫ�û���Ӧ�ò��ļ�ʵ��������ʵ��)
//��ʽ:  void func(INT8U *pbuf,INT16U len,INT8U *ipbuf,INT16U port)
#define W5500_ProtocolAnalysis_Ch0  W5500APP_ProtocolAnalysis_Ch0
#define W5500_ProtocolAnalysis_Ch1  W5500APP_ProtocolAnalysis_Ch1
#define W5500_ProtocolAnalysis_Ch2  W5500APP_ProtocolAnalysis_Ch2
#define W5500_ProtocolAnalysis_Ch3  W5500APP_ProtocolAnalysis_Ch3
#define W5500_ProtocolAnalysis_Ch4  W5500APP_ProtocolAnalysis_Ch4
#define W5500_ProtocolAnalysis_Ch5  W5500APP_ProtocolAnalysis_Ch5
#define W5500_ProtocolAnalysis_Ch6  W5500APP_ProtocolAnalysis_Ch6
#define W5500_ProtocolAnalysis_Ch7  W5500APP_ProtocolAnalysis_Ch7
//--------------------------------------------------------------------------------�û�����-----End
//-------------------------------------------------�궨��
#ifndef STM32                         //PIC����W5500���Ŷ���

    #define W5500_PIN_SCS_H   _LATG9  = 1;
    #define W5500_PIN_SCS_L   _LATG9  = 0;
    #define W5500_PIN_RST_H   _LATG10 = 1;
    #define W5500_PIN_RST_L   _LATG10 = 0;

#else

    #define W5500_PIN_SCS_H GPIO_SetBits(W5500_SCS_PORT, W5500_SCS);
    #define W5500_PIN_SCS_L GPIO_ResetBits(W5500_SCS_PORT, W5500_SCS);
    #define W5500_PIN_RST_H GPIO_SetBits(W5500_RST_PORT, W5500_RST);
    #define W5500_PIN_RST_L GPIO_ResetBits(W5500_RST_PORT, W5500_RST);

#endif
//
#define W5500_TRUE  0xff
#define W5500_FALSE 0x00
//-------------------------------------------------���Ͷ���
//Դ�������
typedef struct W5500_S_CONFIG
{
    uint8_t Gateway_IP[4];//����IP��ַ
    uint8_t Sub_Mask[4];  //��������
    uint8_t Phy_Addr[6];  //�����ַ(MAC)
    uint8_t IP_Addr[4];   //����IP��ַ
    uint8_t DNS_IP[4];    //DNS������
    uint16_t DNS_Port;    //DNS�˿�
    uint8_t LOCK;         //������Ϻ�,��Ϊ0x55Ϊ����.��Ϊ0xAAΪ��Ҫ��ʼ������
    uint8_t Sign_IpConflict; //IP��ͻ��־
} W5500_S_CONFIG;
extern W5500_S_CONFIG W5500_S_Config;
//Ŀ���������
typedef enum W5500_E_CONNECT_ORDER
{
    W5500_E_CONNECT_ORDER_NULL=0,
    W5500_E_CONNECT_ORDER_TCP_SERVER,
    W5500_E_CONNECT_ORDER_TCP_IP_CLIENT,
    W5500_E_CONNECT_ORDER_UDP_IP,
    W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT,
    W5500_E_CONNECT_ORDER_UDP_DOMAIN,
}W5500_E_CONNECT_ORDER;
typedef enum W5500_E_CONNECT_STATE
{
    W5500_E_CONNECT_STATE_NULL=0,
    //W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTING, //�����̶���UDP,�����޴�״̬
    W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED,
    W5500_E_CONNECT_STATE_DOMAIN_GETIP_OK,
    W5500_E_CONNECT_STATE_DOMAIN_GETIP_CLOSED,
    W5500_E_CONNECT_STATE_IP_CONNECTING,
    W5500_E_CONNECT_STATE_IP_CONNECTED,
}W5500_E_CONNECT_STATE;
enum S_W5500_SOCKET_SIGNBITMAP
{
    S_W5500_SOCKET_SIGNBITMAP_NULL=0,
    S_W5500_SOCKET_SIGNBITMAP_RX_OK=(1<<0),
    S_W5500_SOCKET_SIGNBITMAP_TX_OK=(1<<1),
};
typedef struct W5500_S_CONNECT
{
    W5500_E_CONNECT_STATE  state;      //״̬: W5500_E_CONNECT_STATE
    W5500_E_CONNECT_ORDER  order;      //����: 0-��ʹ�� 1-����IP����(UDP) 2-������������(UDP) 3-����IP����(TCP) 4-������������(TCP)  5-����
    uint8_t  DestDnmain[30];             //IP������(ASCII)
    uint8_t  DestIp[4];
    uint16_t DestPort;                   //Ŀ�Ķ˿�
    uint16_t LocalPort;                  //Դ�˿�
    uint8_t  SignBitmap;                 //
} W5500_S_CONNECT;
extern W5500_S_CONNECT W5500_S_Connect[8];
//-------------------------------------------------���⺯��
extern void W5500_DelayMs(uint16_t d);
extern void W5500_Main_100ms(uint8_t sign_init);
extern void W5500_IRQHandler(void);
extern uint8_t W5500_App_Tx(uint8_t ch,uint8_t* pbuf ,uint16_t size);

#endif
#endif

