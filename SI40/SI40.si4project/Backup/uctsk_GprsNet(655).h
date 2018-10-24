/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-29
***********************************************************************************
* 最初版本  ---   支持M10/M12模块
* 20150612  XSL   加入uBlox公司的lisa-U200系列模块
*/
//--------------------------------
#ifndef __UCTSK_GPRS_NET_H
#define __UCTSK_GPRS_NET_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------宏定义----------------------------------
//驱动模式
#if   (defined(XKAP_ICARE_A_M))
#define UCSK_GPRSNET_MODE  1
#else
#define UCSK_GPRSNET_MODE  1 
#endif
//回显功能开启或关闭
#define GPRSNET_ATE  OFF
//-------------------数据结构--------------------------------
//-----通话结构体
typedef struct GPRSNET_S_RING
{
    uint8_t  state;    //状态(0-待机   1-电话进入中 2-电话进入并接通 3-电话拨出中 4-电话拨出并接通)
    uint8_t  order;    //指令(0-无指令 1-接听       2-挂断           3-拨号)
    uint8_t  Phone[20];//电话号(0x00补齐)
    uint16_t Timer_S;  //通话计时(接通后累加,挂断后清零)
} GPRSNET_S_RING;
extern GPRSNET_S_RING Ring_S;
//-----连接结构体
enum GPRSNET_E_CONNECT_STATE
{
    GPRSNET_E_CONNECT_STATE_IDLE=0,
    GPRSNET_E_CONNECT_STATE_CONNECTING,
    GPRSNET_E_CONNECT_STATE_CONNECTED,
};
enum GPRSNET_E_CONNECT_ORDER
{
    GPRSNET_E_CONNECT_ORDER_IDLE=0,
    GPRSNET_E_CONNECT_ORDER_UDP_IP,
    GPRSNET_E_CONNECT_ORDER_UDP_DNS,
    GPRSNET_E_CONNECT_ORDER_TCP_IP,
    GPRSNET_E_CONNECT_ORDER_TCP_DNS,
    GPRSNET_E_CONNECT_ORDER_HTTP,
    GPRSNET_E_CONNECT_ORDER_OFF=0xFF,
};
typedef struct GPRSNET_S_CONNECT
{
    uint8_t  state;        // GPRSNET_E_CONNECT_STATE
    uint8_t  order;        // GPRSNET_E_CONNECT_ORDER
    uint8_t  Port[6];      // 端口号(ASCII)
    uint8_t  *pIpDnmain;    // IP或域名(ASCII)
    uint8_t  *pPathWrite;
    uint8_t  *pPathRead;
    uint8_t  TxState;      // 发送状态(0-空闲状态,1-正在发送中,2-发送成功，3-发送失败)
    uint8_t  ChannelNum;   //用于某些模块连接时才返回通道号
} GPRSNET_S_CONNECT;
extern GPRSNET_S_CONNECT Connect_s[5];//目前最多启用5个连接
//-----信息结构体
typedef struct GprsNetInfo
{
    uint32_t GPS_Longitude;  // 经度(6位小数)
    uint32_t GPS_Latitude;   // 纬度(6位小数)
    uint32_t Flow_B;         // 流量
    uint8_t  IMEI[16];       // IMEI号
    uint8_t  IMSI[16];       // IMSI号
    uint8_t  MoudleStr[20];  // GSM模块版本
    uint8_t  LacCi[4];       // 位置码    (暂时未考虑更新)
    uint8_t  LotalIP[4];     // 本地IP
    uint8_t  PhoneNumber[12];// 手机号
    uint8_t  SignalVal;      // 信号强度
    uint8_t  Apn;            // 0(CMNET) 1(dream.jp)  2(3g-d-2.ocn.ne.jp) 其他-无效
    uint8_t  state;          // 0-没有安装此模块,1-安装了此模块
    uint8_t  OnOff;          // ON-启用模块 OFF-关闭模块
    uint8_t  Err_SimCardInstall; // 0-已初始 1-未安装
} GPRSNET_S_INFO;
//-----消息队列(M10缓存环、GPRS收发、SMS收发)实体结构体
// 1,使用与M10缓存环说明：只有*buf和len有效
// 2,应用程序发送
#define TYPE_TX_NULL 0
#define TYPE_TX_AT   1 //对应para-发送指令类型
#define TYPE_TX_GPRS 2 //对应para-通道号(ASCII)(有效值:'0'~'4')
#define TYPE_TX_SMS  3 //对应para-手机号码(ASCII)
#define TYPE_TX_RING 4 //对应para-手机号码(ASCII)
// 3,应用程序接收
#define TYPE_RX_NULL 0
#define TYPE_RX_GPRS 2 //对应para-通道号(ASCII)(有效值:'0'~'4')
#define TYPE_RX_SMS  3 //对应para-手机号码(ASCII)
#define TYPE_RX_RING 4 //对应para-手机号码(ASCII)
typedef struct gsm_struct1
{
    uint8_t   type;    //类型: 0-无意义,2-GPRS,3-短信
    uint8_t   para[17];//参数: AT-发送指令类型,GPRS-通道号(ASCII),短信-手机号码(ASCII)
    uint16_t  len;     //长度:
    uint8_t   *buf;    //数据:
} GSM_STRUCT1;
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//#define GPRS_PORT_VCCEN
//#define GPRS_PIN_VCCEN
#define GPRS_PORT_PWR         GPIOB
#define GPRS_PIN_PWR          GPIO_Pin_1
//#define GPRS_PORT_RESET
//#define GPRS_PIN_RESET
#define GPRS_UARTX            3
#if   (defined(STM32F1))
#define GPRS_GPIO_RCC_ENABLE  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE)
#elif (defined(STM32F4))
#define GPRS_GPIO_RCC_ENABLE  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB ,ENABLE)
#endif
//
#include "uctsk_GprsAppXkap.h"
#define GPRS_NET_INIT           GprsAppXkap_UpdataState=0
#define GPRS_RX_PARSE_CH0     GprsNetAppXkap_Parse
#define GPRS_BUFFERLOOP_RXBUF_MAX   2048
#define GPRS_RX_MEM_SIZE      E_MEM_MANAGER_TYPE_2KB_BASIC
#define GPRS_TX_MEM_SIZE      E_MEM_MANAGER_TYPE_1KB
#define GPRS_INIT_ON_OFF      OFF
#elif (defined(XKAP_ICARE_A_M))
#define GPRS_PIN_VCCEN        28
#define GPRS_PORT_VCCEN
#define GPRS_PIN_PWR          23
#define GPRS_UARTX            1
#include "uctsk_GprsAppXkap.h"
#define GPRS_RX_PARSE_CH0     GprsNetAppXkap_Parse
#define GPRS_BUFFERLOOP_RXBUF_MAX   1024
#define GPRS_RX_MEM_SIZE      E_MEM_MANAGER_TYPE_256B
#define GPRS_TX_MEM_SIZE      E_MEM_MANAGER_TYPE_256B
#define GPRS_INIT_ON_OFF      ON
#elif (defined(XKAP_ICARE_B_M))
#if   (HARDWARE_SUB_VER==1)
#define GPRS_PIN_PWR          9
#elif (HARDWARE_SUB_VER==2)
#define GPRS_PIN_PWR          23
#define GPRS_PIN_NETLIGHT     21
#endif
#define GPRS_UARTX            1
#include "uctsk_GprsAppXkap.h"
#define GPRS_RX_PARSE_CH0     GprsNetAppXkap_Parse
#define GPRS_BUFFERLOOP_RXBUF_MAX   (1024)
#define GPRS_RX_MEM_SIZE      E_MEM_MANAGER_TYPE_256B
#define GPRS_TX_MEM_SIZE      E_MEM_MANAGER_TYPE_256B
#define GPRS_INIT_ON_OFF      ON
#else
#error Please Set Project to uctsk_GprsNet.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
enum GPRS_E_MOUDLE
{
    GPRS_E_MOUDLE_NULL=0,
    GPRS_E_MOUDLE_QUECTEL_M10,
    GPRS_E_MOUDLE_QUECTEL_M26,
    GPRS_E_MOUDLE_QUECTEL_M35,
    GPRS_E_MOUDLE_AITHINKER_A6,
    GPRS_E_MOUDLE_AITHINKER_A7,
    GPRS_E_MOUDLE_UBLOX_LISAU200,
};
#if   (defined(GPRS_PORT_PWR)&&(defined(STM32F1)||defined(STM32F4)))
#define GPRS_PWR_H         GPIO_SetBits(GPRS_PORT_PWR,GPRS_PIN_PWR)
#define GPRS_PWR_L         GPIO_ResetBits(GPRS_PORT_PWR,GPRS_PIN_PWR)
#elif (defined(GPRS_PIN_PWR)&&(defined(NRF51)||defined(NRF52)))
#define GPRS_PWR_H         nrf_gpio_pin_write(GPRS_PIN_PWR,1);
#define GPRS_PWR_L         nrf_gpio_pin_write(GPRS_PIN_PWR,0);
#else
#define GPRS_PWR_H
#define GPRS_PWR_L
#endif
//
#if   (defined(GPRS_PORT_VCCEN)&&(defined(STM32F1)||defined(STM32F4)))
#define GPRS_VCC_EN_H      GPIO_SetBits(GPRS_PORT_VCCEN,GPRS_PIN_VCCEN)
#define GPRS_VCC_EN_L      GPIO_ResetBits(GPRS_PORT_VCCEN,GPRS_PIN_VCCEN)
#elif (defined(GPRS_PIN_VCCEN)&&(defined(NRF51)||defined(NRF52)))
#define GPRS_VCC_EN_H      nrf_gpio_pin_write(GPRS_PIN_VCCEN,1);
#define GPRS_VCC_EN_L      nrf_gpio_pin_write(GPRS_PIN_VCCEN,0);
#else
#define GPRS_VCC_EN_H
#define GPRS_VCC_EN_L
#endif
//
#if   (defined(GPRS_PORT_RESET)&&(defined(STM32F1)||defined(STM32F4)))
#define GPRS_RESET_H       GPIO_SetBits(GPRS_PORT_RESET,GPRS_PIN_RESET)
#define GPRS_RESET_L       GPIO_ResetBits(GPRS_PORT_RESET,GPRS_PIN_RESET)
#elif (defined(GPRS_PIN_RESET)&&(defined(NRF51)||defined(NRF52)))
#define GPRS_RESET_H       nrf_gpio_pin_write(GPRS_PIN_RESET,1);
#define GPRS_RESET_L       nrf_gpio_pin_write(GPRS_PIN_RESET,0);
#else
#define GPRS_RESET_H
#define GPRS_RESET_L
#endif

//为了防止数据覆盖，建立缓存环模式
#define GSM_APP_TX_STRUCT_MAX_BUF 10
//-------------------接口变量--------------------------------
extern GPRSNET_S_INFO GprsNet_s_Info;
//-------------------接口函数--------------------------------
//任务
extern void App_GsmParseTaskCreate(void);
extern void App_GsmSendTaskCreate(void);
extern void App_GsmTestTaskCreate(void);
//中断
extern void GprsNet_InterruptRx(uint8_t *pbuf,uint16_t len);
//测试
extern void GprsNet_DebugTestOnOff(uint8_t OnOff);
extern void GprsNet_DebugControlOnOff(uint8_t OnOff);
extern void GprsNet_DebugRx(uint8_t *d,uint16_t len);
//控制
extern void GprsNet_OnOff(uint8_t OnOff);
//流量统计
void GprsNet_FlowWrite(uint32_t val);
//-----------------------------------------------------------
#endif

