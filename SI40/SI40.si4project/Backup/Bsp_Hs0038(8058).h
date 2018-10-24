/*
***********************************************************************************
***********************************************************************************
*/
#ifndef __BSP_HS0038_H
#define __BSP_HS0038_H
//-------------------加载库函数------------------------------
//#include "Bsp.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"

#define CAPTURE_TIMER 			3		//使用定时器
#define BSP_HS0038_PIN			8		//使用的引脚
#define	CAPTURE_PPI_CHANNEL NRF_PPI_CHANNEL0 //PPI通道


typedef struct
{
	uint32_t LastCapture;
	uint8_t Status;
	uint8_t RxBuf[4];
	uint8_t RepeatCount;
}IRD_T;

/*全局变量*/
extern bool	Hs0038_Rx_Sem;
extern IRD_T g_tIR;							//全局变量，保存收到的解码数据
extern uint16_t width_buf[66];	//全局变量，保存最新收到的脉冲间隔



extern void Bsp_Hs0038_Init(void);
//-----------------------------------------------------------
#endif
