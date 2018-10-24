/*
*********************************************************************************************************
*
*	模块名称 : 重定向文件
*	文件名称 : Retarget.c
*	版    本 : V1.0
*	说    明 : 此重定向文档，串口和FlashFS都要使用。
*              1. FlashFS不支持使用MDK中的MicroLib功能，不使用MicroLib的话，默认半主模式是开启的，
*                 因此需要重新做重定向文件，以支持串口打印和FlashFS的使用。
*              2. 本文件来自KEIL官方，用户只需提供
*                  extern int SER_PutChar (int ch);
*                  extern int SER_GetChar (void);
*                  两个函数即可，这两个函数在bsp_uart_fifo.c文件实现
*
*	修改记录 :
*		版本号    日期         作者            说明
*       V1.0    2015-09-10   Eric2013          首发
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
#if 0
#include <stdio.h>
#include <rt_misc.h>


#pragma import(__use_no_semihosting_swi)

extern int SER_PutChar(int ch);
extern int SER_GetChar(void);

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
FILE __stderr;

int fputc(int c, FILE *f) {
  return (SER_PutChar(c));
}


int fgetc(FILE *f) {
  return (SER_GetChar());
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int c) {
   SER_PutChar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#endif
