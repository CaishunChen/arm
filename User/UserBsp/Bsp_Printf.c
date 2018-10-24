/**
  ******************************************************************************
  * @file    Bsp_Printf.c 
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
/*
*********************************************************************************************************
*	                                  
*	模块名称 : printf模块    
*	文件名称 : bsp_printf.c
*	版    本 : V2.0
*	说    明 : 实现printf和scanf函数重定向到串口1，即支持printf信息到USART1
*				实现重定向，只需要添加2个函数:
*				int fputc(int ch, FILE *f);
*				int fgetc(FILE *f);
*				对于KEIL MDK编译器，编译选项中需要在MicorLib前面打钩，否则不会有数据打印到USART1。
*				
*				这个c模块无对应的h文件。
*
*	修改记录 :
*		版本号  日期         作者    说明
*		v1.0    2012-12-17   XSL  ST固件库V1.0.2版本。
*
*********************************************************************************************************
*/
#if 0
#include <stdio.h>
#include  <stdint.h>
#include "uctsk_Debug.h"
//-------------------------------------------------------------------------------------------------------
// With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf set to 'Yes') calls __io_putchar()
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
/*
*********************************************************************************************************
*	函 数 名: fputc
*	功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int PUTCHAR_PROTOTYPE(int ch)
{
	DebugOut((int8_t*)&ch,1);
  return ch;
}

/*
*********************************************************************************************************
*	函 数 名: fgetc
*	功能说明: 重定义getc函数，这样可以使用scanff函数从串口1输入数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int GETCHAR_PROTOTYPE(void)
{
	/* 等待串口1输入数据 */
	/*
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
	*/
	return 0;
}
#else
#include "uctsk_Debug.h"
int SER_PutChar(int ch)
{
	DebugOut((int8_t*)&ch,1);
  return ch;
}
int SER_GetChar(void)
{
	return 0;
}

#endif
