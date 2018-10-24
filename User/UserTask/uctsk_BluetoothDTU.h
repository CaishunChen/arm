/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/

#ifndef __BLUETOOTH_DTU_H
#define __BLUETOOTH_DTU_H
//-------------------加载库函数------------------------------
#include "includes.h"
#include "Bsp_NrfBle.h"
#include "ble_gap.h"
//-------------------接口宏定义(硬件相关)--------------------
//GPIO
#if   (defined(STM32F1))
#define BLUETOOTH_RCC_ENABLE     RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , ENABLE)
#define BLUETOOTH_RCC_DISABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE , DISABLE)
#elif (defined(STM32F4))
#define BLUETOOTH_RCC_ENABLE     RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE)
#define BLUETOOTH_RCC_DISABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , DISABLE)
#endif
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
#define BLUETOOTH_PWR_PORT       GPIOB
#define BLUETOOTH_PWR_PIN        GPIO_Pin_8
#define BLUETOOTH_CONNECT_PORT   GPIOE
#define BLUETOOTH_CONNECT_PIN    GPIO_Pin_2
#define BLUETOOTH_RADLATE_PORT   GPIOE
#define BLUETOOTH_RADLATE_PIN    GPIO_Pin_3
#define BLUETOOTH_TX_PORT        GPIOA
#define BLUETOOTH_TX_PIN         GPIO_Pin_2
#define BLUETOOTH_RX_PORT        GPIOA
#define BLUETOOTH_RX_PIN         GPIO_Pin_3
#define BLUETOOTH_UARTX          2
#define BLUETOOTH_UART_BPS       115200
//<----
#endif
//控制电源
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#define BLUETOOTH_PWR_ON         GPIO_SetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#define BLUETOOTH_PWR_OFF        GPIO_ResetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN)
#endif
//-------------------接口宏定义(硬件无关)--------------------
//从机地址
#define BLUETOOTH_DTU_SLAVE_NUM  1
#define BLUETOOTH_DTU_SLAVE_ADDR 0x01
//控制无线
#define BLUETOOTH_RADLATE_L      GPIO_ResetBits(BLUETOOTH_RADLATE_PORT , BLUETOOTH_RADLATE_PIN)
#define BLUETOOTH_RADLATE_H      GPIO_SetBits(BLUETOOTH_RADLATE_PORT , BLUETOOTH_RADLATE_PIN)
//读取连接状态
#define BLUETOOTH_CONNECT_R     (BLUETOOTH_CONNECT_PORT->IDR & BLUETOOTH_CONNECT_PIN)
typedef enum BLUETOOTH_E_CMD
{
   BLUETOOTH_CMD_NULL=0,
   BLUETOOTH_CMD_UPDATA_1DAY,
   BLUETOOTH_CMD_UPDATA_7DAY, 
   BLUETOOTH_CMD_IAP,
   BLUETOOTH_CMD_READ_CMD,
   BLUETOOTH_CMD_KEEP_CONNECT_ONOFF,
}BLUETOOTH_E_CMD;
//
typedef struct BLUETOOTH_S_INFO
{
    // S200B固定使用
    uint8_t  Mode;      // 状态(0-识别MAC模式 1-正常工作模式)
    // 广播透传模块
    uint8_t  MacBuf[6]; // MAC地址
    uint8_t  Type;      // 0-默认   1- 广播透传模块   其他-无效
    uint8_t  HardwareVer_m;   // 模块硬件版本-主号
    uint8_t  HardwareVer_s;   // 模块硬件版本-子号
    uint8_t  SoftwareVer_m;   // 模块软件版本-主号
    uint8_t  SoftwareVer_s;   // 模块软件版本-子号
    // 统计
    uint32_t  UartRxNum;
    uint32_t  UartTxNum;
} BLUETOOTH_S_INFO;
//
typedef struct BLUETOOTH_S_SLAVE  
{
    ble_gap_evt_adv_report_t AdvReport;
    uint8_t  state;           // 0-空闲 1-数据更新中 2--数据就绪
    uint8_t  Type;            // 模块类型
    uint16_t timeover_s;      // 距离上次数据获取的秒数
}BLUETOOTH_S_SLAVE;
//-------------------接口变量--------------------------------
extern BLUETOOTH_S_INFO Bluetooth_s_info;
//长供电使能
extern uint8_t Bluetooth_PowerOnHold_Enable; 
//连接使能
extern uint8_t Bluetooth_ConnectOnOff;
//连接标识
extern uint8_t Bluetooth_ConnectSign;
//传输正确与错误
extern uint16_t Bluetooth_HciTxRxOkErr;
//统计数据
extern uint16_t Bluetooth_ChipPowerCount;
extern uint16_t Bluetooth_ChipOkCount;
extern uint16_t Bluetooth_ChipErrCount;
extern uint16_t Bluetooth_ChipNCCount;
//
#define BLUETOOTH_DTU_S_SLAVEBUF_NUM   20
extern BLUETOOTH_S_SLAVE BluetoothDtu_s_SlaveBuf[BLUETOOTH_DTU_S_SLAVEBUF_NUM];
//-------------------接口函数--------------------------------
//-----任务模式
//移植相关
extern void Bluetooth_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len);
extern void App_BluetoothTaskCreate(void);
extern void Bluetooth_DebugTestOnOff(uint8_t OnOff);
//应用
extern void Bluetooth_App(BLUETOOTH_E_CMD cmd,uint8_t *pbuf);
//-----非任务模式
extern void BluetoothDtu_Init(void);
extern void BluetoothDtu_100ms(void);
extern void BluetoothDtu_RxIrq(uint8_t *pRx,uint16_t len);
//-----------------------------------------------------------
#endif


