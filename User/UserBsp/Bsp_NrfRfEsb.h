/**
  ******************************************************************************
  * @file    Bsp_NrfRfEsb.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Nrf5x芯片的无线私有协议,支持nRF24系列.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
#ifndef __BSP_NRF_RF_ESB_H
#define __BSP_NRF_RF_ESB_H
//-------------------加载库函数------------------------------
#include "includes.h"
#include "nrf_esb.h"
#include "Bsp_CpuId.h"
//-------------------接口宏定义(硬件有关)--------------------
#if   (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_B_M))
//---->
typedef struct BSP_NRFRFESB_S_MOVEPOWER
{
	  uint8_t state;
    uint8_t Num;
    uint8_t Time[6];
    uint32_t Data[6];
} BSP_NRFRFESB_S_MOVEPOWER;
#define BSP_NRFRFESB_BASEADDR_0  {0xA6, 0xC9, 0xAA, 0xAA}
#define BSP_NRFRFESB_BASEADDR_1  {0xC2, 0xC2, 0xC2, 0xC2}
#define BSP_NRFRFESB_PREFIX      {0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5 }
#define BSP_NRFRFESB_TX_MS       100
#define BSP_NRFRFESB_USER_CH_NUM 0	/*0xFF表达无效*/
#define BSP_NRFRFESB_USER_CH     (BspKey_RealValueBuf[BSP_KEY_E_KEY1-1]<<0)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY2-1]<<1)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY3-1]<<2)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY4-1]<<3)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY5-1]<<4)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY6-1]<<5)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY7-1]<<6)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY8-1]<<7)
#define BSP_NRF_ESB_DEFAULT_CONFIG {\
      .protocol               = NRF_ESB_PROTOCOL_ESB_DPL,         \
      .mode                   = NRF_ESB_MODE_PTX,                 \
      .event_handler          = nrf_esb_event_handler,                                \
      .bitrate                = NRF_ESB_BITRATE_250KBPS,            \
      .crc                    = NRF_ESB_CRC_8BIT,                \
      .tx_output_power        = NRF_ESB_TX_POWER_0DBM/*NRF_ESB_TX_POWER_4DBM*/,            \
      .retransmit_delay       = 0,                              \
      .retransmit_count       = 0,                                \
      .tx_mode                = NRF_ESB_TXMODE_AUTO,              \
      .radio_irq_priority     = 1,                                \
      .event_irq_priority     = 2,                                \
      .payload_length         = 32,                               \
      .selective_auto_ack     = true                             \
}

#if   (defined(XKAP_ICARE_B_M))
#include "uctsk_HCI.h"
#define BSP_NRFRFESB_RX_PARSE_PRO   Hci_Rx_Parse
#endif
//<----
#elif (defined(XKAP_ICARE_A_S)||defined(XKAP_ICARE_A_C)||defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
//---->
#define BSP_NRFRFESB_BASEADDR_0 {0xA6, 0xC9, 0xAA, 0xAA}
#define BSP_NRFRFESB_BASEADDR_1 {0xC2, 0xC2, 0xC2, 0x3f}
#define BSP_NRFRFESB_PREFIX      {0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5 }
#define BSP_NRFRFESB_TX_MS       10

#if		(defined(XKAP_ICARE_A_S))
#define BSP_NRFRFESB_USER_CH_NUM 0xFF  /*0xFF表达无效*/
#elif	(defined(XKAP_ICARE_B_C)||defined(XKAP_ICARE_A_C))
#define BSP_NRFRFESB_USER_CH_NUM 		0
#define BSP_NRFRFESB_S_DATA_DEVICETYPE	0x02
#define BSP_NRFRFESB_S_DATA_Ver			0x01
#endif

#define BSP_NRFRFESB_USER_CH     (BspKey_RealValueBuf[BSP_KEY_E_KEY1-1]<<0)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY2-1]<<1)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY3-1]<<2)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY4-1]<<3)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY5-1]<<4)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY6-1]<<5)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY7-1]<<6)\
                                 +(BspKey_RealValueBuf[BSP_KEY_E_KEY8-1]<<7)
#define BSP_NRF_ESB_DEFAULT_CONFIG {\
      .protocol               = NRF_ESB_PROTOCOL_ESB_DPL,         \
      .mode                   = NRF_ESB_MODE_PTX,                 \
      .event_handler          = nrf_esb_event_handler,            \
      .bitrate                = NRF_ESB_BITRATE_250KBPS,          \
      .crc                    = NRF_ESB_CRC_8BIT,                 \
      .tx_output_power        = NRF_ESB_TX_POWER_0DBM/*NRF_ESB_TX_POWER_4DBM*/,            \
      .retransmit_delay       = 0,                                \
      .retransmit_count       = 0,                                \
      .tx_mode                = NRF_ESB_TXMODE_AUTO,              \
      .radio_irq_priority     = 1,                                \
      .event_irq_priority     = 2,                                \
      .payload_length         = 32,                               \
      .selective_auto_ack     = true/*false*/                     \
}
//<----
#else
#error Please Set Project to Bsp_NrfRfEsb.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
typedef struct BSP_NRFRFESB_S_DATA
{
    //-----通讯
    uint8_t DeviceType;		//	设备类型
    uint8_t Ver;			//	协议版本
    uint8_t DataType;		//	数据类型
    //-----
    uint8_t DeviceID[8];	// 	设备ID
    uint8_t Key1_num;
	uint8_t Key2_num;
	uint8_t Key3_num;
	uint8_t Key4_num;
	uint8_t KeyPIR_num;
	uint8_t flag_trigger	:1;
	uint8_t flag_res		:7;
    uint8_t res[2];
	//
	uint8_t Num;
	//-----
	uint8_t sum;
} BSP_NRFRFESB_S_DATA;
extern BSP_NRFRFESB_S_DATA BspBrfRfEsb_s_data;
//-------------------接口函数--------------------------------
void Bsp_NrfRfEsb_Init(void);
void Bsp_NrfRfEsb_Tx(uint8_t *pbuf,uint8_t len);
void Bsp_NrfRfEsb_1ms(void);
void Bsp_NrfRfEsb_AD(uint16_t value);
void Bsp_NrfRfEsb_DebugTestOnOff(uint8_t OnOff);
void Bsp_NrfRfEsb_DebugTestWaveOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif

