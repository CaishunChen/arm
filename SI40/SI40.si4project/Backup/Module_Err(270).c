/**
  ******************************************************************************
  * @file    Module_Err.c
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "Module_Err.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   写信息
 * @note    将信息写入Flash
 * @param   s_err       -   统一格式的结构体
 * @return  OK / ERR
 */
uint8_t ModuleErr_Write(MODULE_S_ERR s_err)
{
	s_err=s_err;
	return ERR;
}
/**
 * @brief   读信息
 * @note    将信息读出
 * @param   *s_err      -   统一格式的结构体
 			*pmode		-	读取模式( (时间段 | 存储序列 )            & 类型滤波 & 正倒序    )
 * @return  OK / ERR
 */
uint8_t ModuleErr_Read(MODULE_S_ERR *ps_err,void *ps_mode)
{
	ps_err	=	ps_err;
	ps_mode	=	ps_mode;
	return ERR;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

