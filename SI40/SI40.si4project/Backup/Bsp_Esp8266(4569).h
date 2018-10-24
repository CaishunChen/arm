/**
  ******************************************************************************
  * @file    bsp_Esp8266.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Esp8266的Wifi驱动.
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

#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#define COM_ESP8266	COM6		/* 选择串口 */

/* 定义下面这句话, 将把收到的字符发送到调试串口1 */
#define ESP8266_TO_COM1_EN

/* 本模块部分函数用到了软件定时器最后1个ID。 因此主程序调用本模块的函数时，请注意回避定时器 TMR_COUNT - 1。
  bsp_StartTimer(3, _usTimeOut);

  TMR_COUNT 在 bsp_timer.h 文件定义
*/
#define ESP8266_TMR_ID	(TMR_COUNT - 1)

/* 串口设备结构体 */
typedef struct
{
	char ssid[33];	/* SSID是一个无线局域网络（WLAN）的名称。SSID是区分大小写的文本字符串，最大长度32个字符 */
	uint8_t ecn;	/* 加密方式 
						0   OPEN
						1   WEP
						2   WPA_PSK
						3   WPA2_PSK
						4   WPA_WPA2_PSK
					*/
	int32_t rssi;		/* 信号强度 */
	uint8_t mac[20];	/* MAC地址字符串*/
	uint8_t ch;			/* 信道 */
}WIFI_AP_T;

/* 加密方式 */
enum
{
	ECN_OPEN = 0,
	ECN_WEP = 1,
	ECN_WPA_PSK = 2,
	ECN_WPA2_PSK = 3,
	ECN_WPA_WPA2_PSK = 4,
};

/* 供外部调用的函数声明 */
void bsp_InitESP8266(void);
void ESP8266_Reset(void);
void ESP8266_PowerOn(void);
void ESP8266_PowerOff(void);
void ESP8266_EnterISP(void);
void ESP8266_ExitISP(void);
void ESP8266_SendAT(char *_Cmd);

uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut);
void ESP8266_PrintRxData(uint8_t _ch);
void ESP8266_JoinAP(char *_ssid, char *_pwd);
int16_t ESP8266_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum);
uint16_t ESP8266_RxNew(uint8_t *_pRxBuf);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
