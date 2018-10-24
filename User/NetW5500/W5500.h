//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 5.10                    //
//                    编 译 器: RealView MDK-ARM Version 5.10                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: W5500.h                                          //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-06-26    版本:  0.3                         //
//-------------------------------------------------------------------------------//
/**********************************************************************************
 * 一,移植步骤:
 *    1. 配置硬件接口(GPIO+SPI)
 *    2. 将W5500_Main_100ms()放入100ms周期函数
 *    3. 将W5500_IRQHandler()放入外中断函数
 *    4. 完成本文件的"用户配置"
 * 二,版本说明
 *    V0.1  2014-08-07  实现基于面向对象思想的UDP.TCP客户端通讯
 *    V0.2  2014-08-08  优化数据接收解析代码,运用了指针函数数组精简代码.
 *    V0.3  2014-08-08  实现DNS域名解析(需要加载DNSXsl.c/.h)
 * 三,待实现的功能
 *    1. telnet
 *    2. DHCP
 *    3. ftp客户端
 * 四,发现未解决的Bug
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
//--------------------------------------------------------------------------------用户配置-----Begin
//----------W5500 GPIO定义

#ifndef STM32               //PIC连接W5500引脚定义
    //定义W5500的CS引脚
    #define W5500_SCS       _LATG9
    #define W5500_RST       _LATG10
    #define W5500_INT       _RC9
#else
    //定义W5500的CS引脚
    #define W5500_SCS       GPIO_Pin_15
    #define W5500_SCS_PORT  GPIOA
    //定义W5500的RST引脚
    #define W5500_RST       GPIO_Pin_6
    #define W5500_RST_PORT  GPIOF
    //定义W5500的INT引脚
    #define W5500_INT       GPIO_Pin_7
    #define W5500_INT_PORT  GPIOF
#endif

//----------8个通道的函数解析名称(需要用户在应用层文件实力化函数实体)
//格式:  void func(INT8U *pbuf,INT16U len,INT8U *ipbuf,INT16U port)
#define W5500_ProtocolAnalysis_Ch0  W5500APP_ProtocolAnalysis_Ch0
#define W5500_ProtocolAnalysis_Ch1  W5500APP_ProtocolAnalysis_Ch1
#define W5500_ProtocolAnalysis_Ch2  W5500APP_ProtocolAnalysis_Ch2
#define W5500_ProtocolAnalysis_Ch3  W5500APP_ProtocolAnalysis_Ch3
#define W5500_ProtocolAnalysis_Ch4  W5500APP_ProtocolAnalysis_Ch4
#define W5500_ProtocolAnalysis_Ch5  W5500APP_ProtocolAnalysis_Ch5
#define W5500_ProtocolAnalysis_Ch6  W5500APP_ProtocolAnalysis_Ch6
#define W5500_ProtocolAnalysis_Ch7  W5500APP_ProtocolAnalysis_Ch7
//--------------------------------------------------------------------------------用户配置-----End
//-------------------------------------------------宏定义
#ifndef STM32                         //PIC连接W5500引脚定义

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
//-------------------------------------------------类型定义
//源网络参数
typedef struct W5500_S_CONFIG
{
    uint8_t Gateway_IP[4];//网关IP地址
    uint8_t Sub_Mask[4];  //子网掩码
    uint8_t Phy_Addr[6];  //物理地址(MAC)
    uint8_t IP_Addr[4];   //本机IP地址
    uint8_t DNS_IP[4];    //DNS服务器
    uint16_t DNS_Port;    //DNS端口
    uint8_t LOCK;         //设置完毕后,设为0x55为锁定.设为0xAA为需要初始化网络
    uint8_t Sign_IpConflict; //IP冲突标志
} W5500_S_CONFIG;
extern W5500_S_CONFIG W5500_S_Config;
//目的网络参数
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
    //W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTING, //域名固定用UDP,所以无此状态
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
    W5500_E_CONNECT_STATE  state;      //状态: W5500_E_CONNECT_STATE
    W5500_E_CONNECT_ORDER  order;      //命令: 0-不使用 1-启用IP连接(UDP) 2-启用域名连接(UDP) 3-启用IP连接(TCP) 4-启用域名连接(TCP)  5-断线
    uint8_t  DestDnmain[30];             //IP或域名(ASCII)
    uint8_t  DestIp[4];
    uint16_t DestPort;                   //目的端口
    uint16_t LocalPort;                  //源端口
    uint8_t  SignBitmap;                 //
} W5500_S_CONNECT;
extern W5500_S_CONNECT W5500_S_Connect[8];
//-------------------------------------------------对外函数
extern void W5500_DelayMs(uint16_t d);
extern void W5500_Main_100ms(uint8_t sign_init);
extern void W5500_IRQHandler(void);
extern uint8_t W5500_App_Tx(uint8_t ch,uint8_t* pbuf ,uint16_t size);

#endif
#endif

