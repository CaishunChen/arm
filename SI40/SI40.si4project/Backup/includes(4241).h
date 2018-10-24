/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-29
***********************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__
/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#include  <stdio.h>
#include  <stdint.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>
#include  <time.h>
//#include  <math.h>
/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/
#if (defined(STM32F1))
//->
#define NRF_LOG_MODULE_NAME "STM32F1"
#include "stm32f10x.h"
#define MCU_SOFT_RESET  *((u32 *)0xE000ED0C) = 0x05fa0004
//<-
#elif (defined(STM32F4))
//->
#define NRF_LOG_MODULE_NAME "STM32F4"
#include "stm32f4xx.h"
#define MCU_SOFT_RESET  NVIC_SystemReset()
//<-
#elif (defined(NRF51))
//->
#define NRF_LOG_MODULE_NAME "NRF51"
#include "sdk_config.h"
#include "nrf51.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "Bsp_WatchDog.h"
#define MCU_SOFT_RESET  Bsp_WatchDog_init(100)
//<-
#elif (defined(NRF52))
//->
#define NRF_LOG_MODULE_NAME "NRF52"
#include "sdk_config.h"
#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "Bsp_WatchDog.h"
#define MCU_SOFT_RESET  Bsp_WatchDog_init(100)
//<-
#endif

#include "MemManager.h"
#include "Module_OS.h"
#include "Count.h"
/*
*********************************************************************************************************
*                                         宏选择
* 注意: 为了文件统一，我们在编译器配置中选择宏定义。
*********************************************************************************************************
*/
//-----CPU-----编译器定义
//#define STM32F1
//#define STM32F4
//-----OS------编译器定义
//#define OS_UCOSII
//#define OS_UCOSIII
//#define OS_NULL
//-----Ver
#if (defined(PROJECT_XKAP_V3))
//---->
#define READ_PROTECT_ONOFF    OFF
//多选一
#define HARDWARE_VER          1     /*熙康      原版*/
//#define HARDWARE_VER          2     /*蓝熙      未知*/
//#define HARDWARE_VER          3     /*志伟竹床  屏幕反转180度*/
//#define HARDWARE_VER          4     /*硕元      开机画面定制*/
//#define HARDWARE_VER          5     /*荟星阁     开机画面定制*/
//#define HARDWARE_VER          6     /*东软      开机画面定制*/
//#define HARDWARE_VER          7     /*ICAM    ALPS定制模块前期研发串口给结果数据*/
//#define HARDWARE_VER          8     /*CFDA    体动为主*/
//#define HARDWARE_VER          9     /*熙康      移动*/
//#define HARDWARE_VER          10    /*金秋医院,睡眠页面定制,呼吸心率页面删除*/
//#define HARDWARE_VER          11    /*天海安龙酒店,继承与金秋医院版本,把定制页面中"请咨询主治医师"改为"请咨询健康顾问"*/
//#define HARDWARE_VER          0xF0    /*TEST LCD自恢复关闭 GSM周期性重启 统计信息*/
//#define HARDWARE_VER          0xF1    /*TEST wifi版*/
#define HARDWARE_SUB_VER      0x00
#define SOFTWARE_VER          0x00
#define SOFTWARE_SUB_VER      0x30
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    7
#define FACTORY_TIME_DAY      2
#define FACTORY_TIME_HOUR     10
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iSleep200G"
//<----
#elif (defined(XKAP_ICARE_B_D_M))
//---->
//多选一
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          1     
#define HARDWARE_SUB_VER      0x00
#define SOFTWARE_VER          0x00
#define SOFTWARE_SUB_VER      27
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareB-D-M"
//<----
#elif (defined(XKAP_GATEWAY))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          22
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    5
#define FACTORY_TIME_DAY      31
#define FACTORY_TIME_HOUR     13
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "Xkap-Gateway"
//<----
#elif (defined(XKAP_ICARE_A_M))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          20
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareA-M"
//<----
#elif (defined(XKAP_ICARE_A_S))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          21
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareA-S"
//<----
#elif (defined(XKAP_ICARE_A_C))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          23
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    6
#define FACTORY_TIME_DAY      4
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareA-C"
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#define READ_PROTECT_ONOFF    OFF
//多选一
// #define HARDWARE_VER       30	/*正常产品*/
#define HARDWARE_VER          35	/*定制产品: 干休所*/ 

#define HARDWARE_SUB_VER      2  	// 1-YJ  2-LJK 3-YJ
#define SOFTWARE_VER          0		// 0-原版	1-易家居定制
#define SOFTWARE_SUB_VER      6		// 对应协议V33 
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    6
#define FACTORY_TIME_DAY      28
#define FACTORY_TIME_HOUR     14
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareB-M"
//<----
#elif (defined(XKAP_ICARE_B_C))		// 作废
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          31
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0		//	0-ble	1-nrf
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareB-C"
//<----
#elif (defined(XKAP_ICARE_B_D))		// 作废
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          32
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareB-D"
//<----
#elif (defined(XKAP_ICARE_S_H))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          33
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    7
#define FACTORY_TIME_DAY      24
#define FACTORY_TIME_HOUR     11
#define FACTORY_TIME_MINUTE   30
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareS-H"
//<----
#elif (defined(XKAP_ICARE_S_S))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          34
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    7
#define FACTORY_TIME_DAY      24
#define FACTORY_TIME_HOUR     11
#define FACTORY_TIME_MINUTE   30
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "iCareS-S"
//<----
#elif (defined(PROJECT_NRF5X_BLE))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          200
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      1
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "XSL-NRF-TAG"
//<----
#elif (defined(TEST_NRF52_V1))
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          201
#define HARDWARE_SUB_VER      1
#define SOFTWARE_VER          1		/*	0-wifi版		1-gprs版*/
#define SOFTWARE_SUB_VER      2
#define FACTORY_TIME_YEAR     2018
#define FACTORY_TIME_MONTH    4
#define FACTORY_TIME_DAY      18
#define FACTORY_TIME_HOUR     10
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "TEST-NRF52-V1"
//<----
#else
//---->
#define READ_PROTECT_ONOFF    OFF
#define HARDWARE_VER          0
#define HARDWARE_SUB_VER      0
#define SOFTWARE_VER          0
#define SOFTWARE_SUB_VER      0
#define FACTORY_TIME_YEAR     2017
#define FACTORY_TIME_MONTH    11
#define FACTORY_TIME_DAY      6
#define FACTORY_TIME_HOUR     15
#define FACTORY_TIME_MINUTE   0
#define FACTORY_TIME_SECONT   0
#define PRODUCT_NAME          "XSL-NULL"
//<----
#endif
/*
*********************************************************************************************************
*                                         SEGGER RTT
*********************************************************************************************************
*/
#define LOG_USES_RTT                            1
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_UP        256
#define SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS    2
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN      64
#define SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS  2
/*
*********************************************************************************************************
*                                         结构定义
*********************************************************************************************************
*/
typedef struct INT8_S_BIT
{
   uint8_t bit0   :1;
   uint8_t bit1   :1;
   uint8_t bit2   :1;
   uint8_t bit3   :1;
   uint8_t bit4   :1;
   uint8_t bit5   :1;
   uint8_t bit6   :1;
   uint8_t bit7   :1;
}INT8_S_BIT;

typedef struct INT16_S_BIT
{
   uint16_t bit0   :1;
   uint16_t bit1   :1;
   uint16_t bit2   :1;
   uint16_t bit3   :1;
   uint16_t bit4   :1;
   uint16_t bit5   :1;
   uint16_t bit6   :1;
   uint16_t bit7   :1;
   uint16_t bit8   :1;
   uint16_t bit9   :1;
   uint16_t bit10  :1;
   uint16_t bit11  :1;
   uint16_t bit12  :1;
   uint16_t bit13  :1;
   uint16_t bit14  :1;
   uint16_t bit15  :1;
}INT16_S_BIT;
/*
*********************************************************************************************************
*                                         常量定义
*********************************************************************************************************
*/
//
#define ERR       0x01
#define OK        0x00
#define EQU       0x02
//
#define TRUE      1
#define FALSE     0
//
#define ON        0x01   
#define OFF       0x00
//
#define GREATER
#define EQUAL
#define LESS
//
#define	________	0x0
#define	_______X	0x1
#define	______X_	0x2
#define	______XX	0x3
#define	_____X__	0x4
#define	_____X_X	0x5
#define	_____XX_	0x6
#define	_____XXX	0x7
#define	____X___	0x8
#define	____X__X	0x9
#define	____X_X_	0xa
#define	____X_XX	0xb
#define	____XX__	0xc
#define	____XX_X	0xd
#define	____XXX_	0xe
#define	____XXXX	0xf
#define	___X____	0x10
#define	___X___X	0x11
#define	___X__X_	0x12
#define	___X__XX	0x13
#define	___X_X__	0x14
#define	___X_X_X	0x15
#define	___X_XX_	0x16
#define	___X_XXX	0x17
#define	___XX___	0x18
#define	___XX__X	0x19
#define	___XX_X_	0x1a
#define	___XX_XX	0x1b
#define	___XXX__	0x1c
#define	___XXX_X	0x1d
#define	___XXXX_	0x1e
#define	___XXXXX	0x1f
#define	__X_____	0x20
#define	__X____X	0x21
#define	__X___X_	0x22
#define	__X___XX	0x23
#define	__X__X__	0x24
#define	__X__X_X	0x25
#define	__X__XX_	0x26
#define	__X__XXX	0x27
#define	__X_X___	0x28
#define	__X_X__X	0x29
#define	__X_X_X_	0x2a
#define	__X_X_XX	0x2b
#define	__X_XX__	0x2c
#define	__X_XX_X	0x2d
#define	__X_XXX_	0x2e
#define	__X_XXXX	0x2f
#define	__XX____	0x30
#define	__XX___X	0x31
#define	__XX__X_	0x32
#define	__XX__XX	0x33
#define	__XX_X__	0x34
#define	__XX_X_X	0x35
#define	__XX_XX_	0x36
#define	__XX_XXX	0x37
#define	__XXX___	0x38
#define	__XXX__X	0x39
#define	__XXX_X_	0x3a
#define	__XXX_XX	0x3b
#define	__XXXX__	0x3c
#define	__XXXX_X	0x3d
#define	__XXXXX_	0x3e
#define	__XXXXXX	0x3f
#define	_X______	0x40
#define	_X_____X	0x41
#define	_X____X_	0x42
#define	_X____XX	0x43
#define	_X___X__	0x44
#define	_X___X_X	0x45
#define	_X___XX_	0x46
#define	_X___XXX	0x47
#define	_X__X___	0x48
#define	_X__X__X	0x49
#define	_X__X_X_	0x4a
#define	_X__X_XX	0x4b
#define	_X__XX__	0x4c
#define	_X__XX_X	0x4d
#define	_X__XXX_	0x4e
#define	_X__XXXX	0x4f
#define	_X_X____	0x50
#define	_X_X___X	0x51
#define	_X_X__X_	0x52
#define	_X_X__XX	0x53
#define	_X_X_X__	0x54
#define	_X_X_X_X	0x55
#define	_X_X_XX_	0x56
#define	_X_X_XXX	0x57
#define	_X_XX___	0x58
#define	_X_XX__X	0x59
#define	_X_XX_X_	0x5a
#define	_X_XX_XX	0x5b
#define	_X_XXX__	0x5c
#define	_X_XXX_X	0x5d
#define	_X_XXXX_	0x5e
#define	_X_XXXXX	0x5f
#define	_XX_____	0x60
#define	_XX____X	0x61
#define	_XX___X_	0x62
#define	_XX___XX	0x63
#define	_XX__X__	0x64
#define	_XX__X_X	0x65
#define	_XX__XX_	0x66
#define	_XX__XXX	0x67
#define	_XX_X___	0x68
#define	_XX_X__X	0x69
#define	_XX_X_X_	0x6a
#define	_XX_X_XX	0x6b
#define	_XX_XX__	0x6c
#define	_XX_XX_X	0x6d
#define	_XX_XXX_	0x6e
#define	_XX_XXXX	0x6f
#define	_XXX____	0x70
#define	_XXX___X	0x71
#define	_XXX__X_	0x72
#define	_XXX__XX	0x73
#define	_XXX_X__	0x74
#define	_XXX_X_X	0x75
#define	_XXX_XX_	0x76
#define	_XXX_XXX	0x77
#define	_XXXX___	0x78
#define	_XXXX__X	0x79
#define	_XXXX_X_	0x7a
#define	_XXXX_XX	0x7b
#define	_XXXXX__	0x7c
#define	_XXXXX_X	0x7d
#define	_XXXXXX_	0x7e
#define	_XXXXXXX	0x7f
#define	X_______	0x80
#define	X______X	0x81
#define	X_____X_	0x82
#define	X_____XX	0x83
#define	X____X__	0x84
#define	X____X_X	0x85
#define	X____XX_	0x86
#define	X____XXX	0x87
#define	X___X___	0x88
#define	X___X__X	0x89
#define	X___X_X_	0x8a
#define	X___X_XX	0x8b
#define	X___XX__	0x8c
#define	X___XX_X	0x8d
#define	X___XXX_	0x8e
#define	X___XXXX	0x8f
#define	X__X____	0x90
#define	X__X___X	0x91
#define	X__X__X_	0x92
#define	X__X__XX	0x93
#define	X__X_X__	0x94
#define	X__X_X_X	0x95
#define	X__X_XX_	0x96
#define	X__X_XXX	0x97
#define	X__XX___	0x98
#define	X__XX__X	0x99
#define	X__XX_X_	0x9a
#define X__XX_XX	0x9b
#define X__XXX__	0x9c
#define X__XXX_X	0x9d
#define	X__XXXX_	0x9e
#define	X__XXXXX	0x9f
#define	X_X_____	0xa0
#define	X_X____X	0xa1
#define	X_X___X_	0xa2
#define	X_X___XX	0xa3
#define	X_X__X__	0xa4
#define	X_X__X_X	0xa5
#define	X_X__XX_	0xa6
#define	X_X__XXX	0xa7
#define	X_X_X___	0xa8
#define	X_X_X__X	0xa9
#define	X_X_X_X_	0xaa
#define	X_X_X_XX	0xab
#define	X_X_XX__	0xac
#define	X_X_XX_X	0xad
#define	X_X_XXX_	0xae
#define	X_X_XXXX	0xaf
#define	X_XX____	0xb0
#define  X_XX___X	0xb1
#define	X_XX__X_	0xb2
#define	X_XX__XX	0xb3
#define	X_XX_X__	0xb4
#define	X_XX_X_X	0xb5
#define	X_XX_XX_	0xb6
#define	X_XX_XXX	0xb7
#define	X_XXX___	0xb8
#define	X_XXX__X	0xb9
#define	X_XXX_X_	0xba
#define	X_XXX_XX	0xbb
#define	X_XXXX__	0xbc
#define	X_XXXX_X	0xbd
#define	X_XXXXX_	0xbe
#define	X_XXXXXX	0xbf
#define	XX______	0xc0
#define	XX_____X	0xc1
#define	XX____X_	0xc2
#define	XX____XX	0xc3
#define	XX___X__	0xc4
#define	XX___X_X	0xc5
#define	XX___XX_	0xc6
#define	XX___XXX	0xc7
#define	XX__X___	0xc8
#define	XX__X__X	0xc9
#define	XX__X_X_	0xca
#define	XX__X_XX	0xcb
#define	XX__XX__	0xcc
#define	XX__XX_X	0xcd
#define	XX__XXX_	0xce
#define  XX__XXXX	0xcf
#define	XX_X____	0xd0
#define	XX_X___X	0xd1
#define	XX_X__X_	0xd2
#define	XX_X__XX	0xd3
#define	XX_X_X__	0xd4
#define	XX_X_X_X	0xd5
#define	XX_X_XX_	0xd6
#define	XX_X_XXX	0xd7
#define	XX_XX___	0xd8
#define	XX_XX__X	0xd9
#define	XX_XX_X_	0xda
#define	XX_XX_XX	0xdb
#define	XX_XXX__	0xdc
#define	XX_XXX_X	0xdd
#define	XX_XXXX_	0xde
#define	XX_XXXXX	0xdf
#define	XXX_____	0xe0
#define	XXX____X	0xe1
#define	XXX___X_	0xe2
#define	XXX___XX	0xe3
#define	XXX__X__	0xe4
#define	XXX__X_X	0xe5
#define	XXX__XX_	0xe6
#define	XXX__XXX	0xe7
#define	XXX_X___	0xe8
#define	XXX_X__X	0xe9
#define	XXX_X_X_	0xea
#define	XXX_X_XX	0xeb
#define	XXX_XX__	0xec
#define	XXX_XX_X	0xed
#define	XXX_XXX_	0xee
#define	XXX_XXXX	0xef
#define	XXXX____	0xf0
#define	XXXX___X	0xf1
#define	XXXX__X_	0xf2
#define	XXXX__XX	0xf3
#define	XXXX_X__	0xf4
#define	XXXX_X_X	0xf5
#define	XXXX_XX_	0xf6
#define	XXXX_XXX	0xf7
#define	XXXXX___	0xf8
#define	XXXXX__X	0xf9
#define	XXXXX_X_	0xfa
#define	XXXXX_XX	0xfb
#define	XXXXXX__	0xfc
#define	XXXXXX_X	0xfd
#define	XXXXXXX_	0xfe
#define	XXXXXXXX	0xff
/*
*********************************************************************************************************
*                                            INCLUDES END
*********************************************************************************************************
*/

#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


