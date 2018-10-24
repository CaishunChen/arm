/**
  ******************************************************************************
  * @file    Bsp_Camera.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   Camera(For OV7670).
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F4
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
         OV7670驱动程序。本程序适用于 guanfu_wang  的OV7670摄像头（不带FIFO,不带LDO，不带24M晶振)
		 安富莱STM32-V5开发板集成了3.0V LDO给OV7670供电，主板集成了24M有源晶振提供给摄像头。
		 本代码参考了 guanfu_wang 提供的例子。http://mcudiy.taobao.com/
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_CAMERA_H
#define __BSP_CAMERA_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint8_t CaptureOk;		// 捕获完成
}CAM_T;
/* Exported constants --------------------------------------------------------*/
/* Transplant define ---------------------------------------------------------*/
/* Application define --------------------------------------------------------*/
#define OV7670_SLAVE_ADDRESS	0x42
/* Exported macro ------------------------------------------------------------*/
extern CAM_T g_tCam;
/* Exported functions --------------------------------------------------------*/
void bsp_InitCamera(void);
uint16_t OV_ReadID(void);
void CAM_Start(uint32_t _uiDispMemAddr);
void CAM_Stop(void);

#endif
/************************ (C) COPYRIGHT XSLXHN *****END OF FILE****/

