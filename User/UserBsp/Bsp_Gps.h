/**
  ******************************************************************************
  * @file    Bsp_Gps.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   Gps驱动.
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

#ifndef __BSP_GPS_H
#define __BSP_GPS_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
#if   (defined(PROJECT_STLH_V10))
//---->
#define  BSP_GPS_UARTX  (2)
//时钟
#if   (defined(STM32F1))
#define BSP_GPS_PWR_ENABLE    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BSP_GPS_PWR_ENABLE    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
#endif
//端口
#define BSP_GPS_PWR_PORT     GPIOD
#define BSP_GPS_PWR_PIN      GPIO_Pin_1
//<----
#else
#error Please Set Project to Bsp_Gps.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#ifdef BSP_GPS_PWR_PORT
#define BSP_GPS_PWR_ON       GPIO_ResetBits(BSP_GPS_PWR_PORT , BSP_GPS_PWR_PIN)
#define BSP_GPS_PWR_OFF      GPIO_SetBits(BSP_GPS_PWR_PORT , BSP_GPS_PWR_PIN)
#endif
//
typedef struct
{
   // 1表示串口数据接收正常，即可以收到GPS的命令字符串
	uint8_t UartOk;
	// 从GGA 命令 : 时间、位置、定位类型  (位置信息可有 RMC命令获得)
	// 定位有效标志, 0:未定位 1:SPS模式，定位有效 2:差分，SPS模式，定位有效 3:PPS模式，定位有效
	uint8_t PositionOk;
   // 海拔高度，相对海平面多少米   x.x
	uint32_t Altitude;
	// GLL：经度、纬度、UTC时间
	// 样例数据：$GPGLL,3723.2475,N,12158.3416,W,161229.487,A*2C
	// GSA：GPS接收机操作模式，定位使用的卫星，DOP值
	// 样例数据：$GPGSA,A,3,07,02,26,27,09,04,15, , , , , ,1.8,1.0,1.5*33
	// M=手动（强制操作在2D或3D模式），A=自动
	uint8_t ModeAM;
   // 定位类型 1=未定位，2=2D定位，3=3D定位
	uint8_t Mode2D3D;
   // ID of 1st satellite used for fix
	uint8_t SateID[12];
   // 位置精度, 0.1米
	uint16_t PDOP;
   // 水平精度, 0.1米
	uint16_t HDOP;
   // 垂直精度, 0.1米
	uint16_t VDOP;
	// GSV：可见GPS卫星信息、仰角、方位角、信噪比（SNR）
	//	$GPGSV,2,1,07,07,79,048,42,02,51,062,43,26,36,256,42,27,27,138,42*71
	//	$GPGSV,2,2,07,09,23,313,42,04,19,159,41,15,12,041,42*41
	// 可见卫星个数
	uint8_t ViewNumber;
   // 仰角 elevation in degrees  度,最大90°
	uint8_t Elevation[12];
   // 方位角 azimuth in degrees to true  度,0-359°
	uint16_t Azimuth[12];
   // 载噪比（C/No）  信噪比？ ， dBHz   范围0到99，没有跟踪时为空
	uint8_t SNR[12];
	// RMC：时间、日期、位置、速度
	// 样例数据：$GPRMC,161229.487,A,3723.2475,N,12158.3416,W,0.13,309.62,120598, ,*10
	// 纬度，度
	uint16_t WeiDu_Du;
   // 纬度，分. 232475；  小数点后4位, 表示 23.2475 分
	uint32_t WeiDu_Fen;
   // 纬度半球N（北半球）或S（南半球）
	char     NS;
   // 经度，单位度
	uint16_t JingDu_Du;
   // 经度，单位度
	uint32_t JingDu_Fen;
   // 经度半球E（东经）或W（西经）
	char     EW;
   // 日期 120598 ddmmyy
	uint16_t Year;
	uint8_t  Month;
	uint8_t  Day;
   // UTC 时间。 hhmmss.sss
	uint8_t  Hour;
   // 分
	uint8_t  Min;
   // 秒
	uint8_t  Sec;
   // 毫秒
	uint16_t mSec;
   // A=UTC时间数据有效；V=数据无效
	char   TimeOk;
	//---VTG：地面速度信息
	// 样例数据：$GPVTG,309.62,T, ,M,0.13,N,0.2,K*6E
	// 以真北为参考基准的地面航向（000~359度，前面的0也将被传输）
	uint16_t TrackDegTrue;
   // 以磁北为参考基准的地面航向（000~359度，前面的0也将被传输）
	uint16_t TrackDegMag;
   // 地面速率（000.0~999.9节，前面的0也将被传输）
	uint32_t SpeedKnots;
   // 地面速率（000.0~999.9节，前面的0也将被传输）
	uint32_t SpeedKM;
}GPS_T;
//-------------------接口变量--------------------------------
extern GPS_T g_tGPS;
//-------------------接口函数--------------------------------
void BspGps_Init(void);
void Gps_InterruptRx(uint8_t *pbuf,uint16_t len);
void Gps_DebugTestOnOff(uint8_t OnOff);
//   
//uint32_t gps_FenToDu(uint32_t _fen);
//uint16_t gps_FenToMiao(uint32_t _fen);
//-----------------------------------------------------------
#endif

