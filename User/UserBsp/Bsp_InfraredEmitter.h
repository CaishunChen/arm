/*
***********************************************************************************
***********************************************************************************
*/
#ifndef __BSP_INFRARED_EMITTER_H
#define __BSP_INFRARED_EMITTER_H

#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"

#define	BSP_INFRARED_PIN	25
#define	BSP_INFRARED_38K_TIMER	4	//38K 定时器
#define	BSP_10US_TIMER					2	//调制波定时器

#define	BSP_38K_PPI_CHANNEL	NRF_PPI_CHANNEL1



extern void	Bsp_Infrared_Emitter_Init(void);

extern bool Bsp_NEC_Coding_Modulate_1Frame(uint8_t * data);


#endif
