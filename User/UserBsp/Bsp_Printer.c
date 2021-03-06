﻿/**
  ******************************************************************************
  * @file    Bsp_Printer.c 
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
//-------------------加载库函数------------------------------
#include "includes.h"
#include "Bsp_Printer.h"
#include "Bsp_Uart.h"
#include "uctsk_Debug.h"
#include "Bsp_Gps.h"
//------------------------------- 用户变量 --------------------
static uint8_t BspPrinter_DebugTest_Enable=0;
static uint8_t Logo_Print[76][52]=
{
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x1F,0xFF,0xFF,0xFF,0xFC,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x1F,0xFF,0xFF,0xFF,0xFC,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x1F,0xFF,0xFF,0xFF,0xFC,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFE,0x1F,0xFF,0xFF,0xFF,0xFC,0x30,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0x3F,0xFF,0xFF,0xFF,0xFC,0x67,0xE3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFE,0x3F,0xFF,0xFF,0xFF,0xF8,0x46,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFE,0x3F,0xFF,0xFF,0xFF,0xF8,0xC6,0x19,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFE,0x3F,0xFF,0xFF,0xFF,0xF8,0x86,0x10,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x00,0x00,0x0F,0xFC,0x00,0x87,0xF0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xF0,0x00,0x00,0x0F,0xF8,0x00,0x86,0x60,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x00,0x00,0x0F,0xF8,0x00,0x86,0x60,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x00,0x00,0x0F,0xF0,0x00,0xC6,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xE0,0x00,0x00,0x0F,0xF0,0x00,0x46,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF0,0x00,0x00,0x1F,0xF0,0x00,0x20,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xE0,0x1F,0xF0,0x00,0x30,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x1F,0xF0,0x00,0x0C,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFC,0x1F,0xE0,0x00,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFE,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x3F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x3F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0x7F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0x7F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFE,0x7F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFE,0x7F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFC,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xF8,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xF0,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xE0,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0x81,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFC,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x60,0x00,0x00,0x00,0x01,0x81,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x60,0x1F,0xFF,0xFC,0x03,0x80,0x80,0x07,0xF8,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFC,0x00,0x1C,0x00,0x03,0x30,0xC2,0x06,0x08,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0C,0x00,0x00,0x1C,0x00,0x03,0x30,0xC6,0x06,0x08,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0C,0x10,0x00,0x1C,0x00,0x06,0x18,0x46,0x06,0x18,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x04,0x30,0x00,0x1C,0x00,0x0E,0x18,0x06,0x07,0xF8,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xC4,0x60,0x00,0x1C,0x00,0x1E,0x18,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x46,0xE0,0x00,0x1C,0x00,0x3E,0x0C,0x0C,0x00,0x03,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x43,0xC0,0x3F,0xFF,0xFE,0x26,0x0C,0x0C,0x00,0x07,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x43,0x04,0x3F,0xFF,0xFE,0x06,0x0C,0x18,0x0F,0xFF,0xF3,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0xC7,0x8C,0x00,0x1C,0x00,0x06,0x06,0x18,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x61,0x98,0xF8,0x00,0x1C,0x00,0x06,0x06,0x30,0x00,0x1C,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x3C,0x00,0x06,0x03,0x30,0x00,0x70,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x06,0x03,0xE0,0x0F,0x80,0x0F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xF0,0x00,0x77,0x00,0x06,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0xC6,0x30,0x00,0xE3,0x00,0x06,0x01,0xC0,0x03,0xF8,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x86,0x30,0x00,0xC1,0x80,0x06,0x03,0xE0,0x06,0x18,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x86,0x30,0x03,0x80,0xC0,0x06,0x07,0x38,0x06,0x08,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x86,0x30,0x07,0x00,0x70,0x06,0x0C,0x0C,0x06,0x08,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0xC6,0x30,0x1C,0x00,0x1C,0x06,0x38,0x06,0x06,0x08,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFC,0x30,0x00,0x06,0x06,0x60,0x01,0x07,0xF8,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x42,0x1C,0x00,0x0E,0x11,0x00,0x81,0x08,0x20,0x00,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64,0x42,0x60,0xE3,0x31,0x8E,0x21,0xC3,0x88,0x31,0x33,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x42,0x40,0xD1,0x20,0x04,0x21,0x42,0x88,0x1B,0x32,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x66,0x60,0xD1,0x60,0x04,0x21,0x22,0x48,0x0A,0x36,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x46,0x60,0xC9,0x63,0x84,0x22,0x22,0x48,0x0E,0x36,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x42,0x40,0xC9,0x21,0x84,0x23,0xF2,0x28,0x04,0x32,0x19,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x42,0x60,0xC7,0x31,0x84,0x26,0x12,0x38,0x04,0x33,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x1F,0x10,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x42,0x7C,0x47,0x0F,0x04,0x24,0x12,0x18,0x04,0x31,0xF9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

};
//------------------------------- 用户函数声明 ----------------
void  BspPrinter_Uartx_App(uint8_t Cmd,uint8_t pPara);
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/

void BspPrinter_Init(void)
{
    //硬件初始化
    UART_INIT(BSP_PRINTER_UARTX,9600);	  
	 // BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_BAUD_115200,NULL);
}

/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspPrinter_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    if(BspPrinter_DebugTest_Enable==1)
    {
        DebugOutHex("Printer Rx:",pbuf,len);
    }
}
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/


void  BspPrinter_Uartx_App(uint8_t Cmd,uint8_t pPara)
{
uint8_t	  ucLen;                   
uint8_t	  ucPrint[6];	
	
  	     ucLen	= 0;      
         switch(Cmd)
        {
            case PRINTER_UARTX_APP_CMD_WORD_12SIZE12:
							      ucPrint[ucLen++]=0x1B;
                    ucPrint[ucLen++]=0x4D;   
                    ucPrint[ucLen++]=0X01;		
                break;
            case PRINTER_UARTX_APP_CMD_WORD_24SIZE24:
							      ucPrint[ucLen++]=0x1b;
                    ucPrint[ucLen++]=0x4D;   
                    ucPrint[ucLen++]=0X00;		
                break;	
            case PRINTER_UARTX_APP_CMD_WORD_WIDTH_UNABLE:  
							      ucPrint[ucLen++]=0x1B;
                    ucPrint[ucLen++]=0x45;   
                    ucPrint[ucLen++]=0X00;		
                break;
            case PRINTER_UARTX_APP_CMD_WORD_WIDTH_ANABLE:  //粗体
							      ucPrint[ucLen++]=0x1B;
                    ucPrint[ucLen++]=0x45;   
                    ucPrint[ucLen++]=0X01;		
                break;				
            case PRINTER_UARTX_APP_CMD_WORD_DOUBLE_UNABLE: //正常
								   ucPrint[ucLen++]=0x1D;
                   ucPrint[ucLen++]=0x21;   
                   ucPrint[ucLen++]=0;	   
                break;
            case PRINTER_UARTX_APP_CMD_WORD_DOUBLE_ANABLE: //放大2倍
						       ucPrint[ucLen++]=0x1D;
                   ucPrint[ucLen++]=0x21;   
                   ucPrint[ucLen++]=0x11;	   
                break;						
            case PRINTER_UARTX_APP_CMD_LINE_SPACE: //热敏默认值0 为换行命令设置 n/2点行间距,n的值应在不大于96
						    	 ucPrint[ucLen++]=0x1B;
                   ucPrint[ucLen++]=0x33;   
                   ucPrint[ucLen++]=0x24;	
                break;
            case PRINTER_UARTX_APP_CMD_RIGHT_LIMIT: 
							      ucPrint[ucLen++]=0x1B;
                    ucPrint[ucLen++]=0x20;   
                    ucPrint[ucLen++]=pPara;               
                break;
            case PRINTER_UARTX_APP_CMD_LEFT_LIMIT: //左侧不打印的字符数为pPara
							     ucPrint[ucLen++]=0x1B;
                   ucPrint[ucLen++]=0x6C;   
                   ucPrint[ucLen++]=pPara;                
                break;
            case PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE:  //
							     ucPrint[ucLen++]=0x1C;
                   ucPrint[ucLen++]=0x2D;   
                   ucPrint[ucLen++]=0X00;                
                break;	
            case PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE:  //下划线使能
							     ucPrint[ucLen++]=0x1C;
                   ucPrint[ucLen++]=0x2D;   
                   ucPrint[ucLen++]=0X01;                
                break;						
            case PRINTER_UARTX_APP_CMD_INIT:         //打印机初始化
							     ucPrint[ucLen++]=0x1B;
                   ucPrint[ucLen++]=0x40; 
                break;
            case PRINTER_UARTX_APP_CMD_BAUD_115200:  //115200/4800
							     ucPrint[ucLen++]=0x1B;
                   ucPrint[ucLen++]=0x02;
                   ucPrint[ucLen++]=0x18;     						
                break; 
            case PRINTER_UARTX_APP_CMD_BAUD_9600:    //9600/4800
							     ucPrint[ucLen++]=0x1B;
                   ucPrint[ucLen++]=0x02;
                   ucPrint[ucLen++]=0x02;     						
                break; 						
            case PRINTER_UARTX_APP_CMD_ENTER:
                   ucPrint[ucLen++]=0x0A;   
                break;		
            case PRINTER_UARTX_APP_CMD_IMAGE:
							     for(ucLen=0; ucLen<76; ucLen++)
                  {
						       UART_DMA_Tx(BSP_PRINTER_UARTX,Logo_Print[ucLen],52);	
									// Count_DelayUs(1);
                  }  										
                break;							
            default:
                break;
        }
      if(Cmd!=PRINTER_UARTX_APP_CMD_IMAGE) 
				UART_DMA_Tx(BSP_PRINTER_UARTX,ucPrint,ucLen);
			Count_DelayUs(1);
 }   
    
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspPrinter_PrintInfo(BSP_PRINTER_LCD_INFO pinfo)
{
	  char      ucDisplay[8],ucPrint[14];   
    uint8_t   ucLen;	
    if(0)
    {
		 pinfo.JingDu_EW = 'E';
	   pinfo.JingDu_Du = 121;pinfo.JingDu_Fen = 583416;  
	   pinfo.WeiDu_NS = 'N';
	   pinfo.WeiDu_Du = 37;pinfo.WeiDu_Fen = 232475; 
		 g_tGPS.UartOk= 1;
		 g_tGPS.PositionOk= 1;
		 pinfo.Humi=2.9;
		 pinfo.Temp=18.1;
	  }
    BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_WORD_12SIZE12,NULL);
	  BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_WORD_DOUBLE_ANABLE,NULL);
	  UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"  土壤温湿度测试结果\r\n",22);
	  UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"        报告单\r\n\r\n",18);
//	  BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_ENTER,NULL);
	
    BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_WORD_24SIZE24,NULL);
	  BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_WORD_DOUBLE_UNABLE,NULL);	
	 // 打印 时间
  	UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"测试时间: ",10);
	  UART_DMA_Tx(BSP_PRINTER_UARTX,pinfo.TimeBuf,16);  
//	  BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_ENTER,NULL);
	  UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"\r\n",2);
   //GPS位置显示
  	UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"测试地点: 经度 ",15);
    BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE,NULL);
           if(g_tGPS.UartOk)                      // 1表示串口数据接收正常，即可以收到GPS的命令字符串
           {					
              if(g_tGPS.PositionOk==0)            // 定位有效标志, 0:未定位
              {
               UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"正在定位中\r\n",12);        
              }
              else
              {						 
                    ucLen=0;
                    memset(ucPrint,0x20,14);
							      ucPrint[ucLen]=  pinfo.JingDu_EW;
						        ucLen		+= 1;
                    memset(ucDisplay,0x20,8);
                    sprintf(ucDisplay, "%d", pinfo.JingDu_Du); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    memcpy(ucPrint + ucLen, "°",2);
                    ucLen       += 2;								
                    memset(ucDisplay,0x20,8);
								    sprintf(ucDisplay, "%.4f", (float)pinfo.JingDu_Fen/10000); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)ucPrint,ucLen);		
										BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_ENTER,NULL);
                }    	
					 } 	
	         else
           {	
             UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"无法定位\r\n",10);         													
					 }
           BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE,NULL);
           UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"          纬度 ",15);
					 BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE,NULL);					 
           if(g_tGPS.UartOk)                      // 1表示串口数据接收正常，即可以收到GPS的命令字符串
           {					
              if(g_tGPS.PositionOk==0)            // 定位有效标志, 0:未定位
              {
               UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"正在定位中\r\n",12);        
              }
              else
              {						 
                   ucLen=0;
                    memset(ucPrint,0x20,14);
						        ucPrint[ucLen]=  pinfo.WeiDu_NS;
						        ucLen		+= 1;
                    memset(ucDisplay,0x20,8);
								    sprintf(ucDisplay, "%d", pinfo.WeiDu_Du); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    memcpy(ucPrint + ucLen, "°",2);
                    ucLen       += 2;
                    memset(ucDisplay,0x20,8);
								    sprintf(ucDisplay, "%.4f", (float)pinfo.WeiDu_Fen/10000); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)ucPrint,ucLen);		
										BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_ENTER,NULL);	
                }    	
						 } 	
	         else
           {	
             UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"无法定位\r\n",10);         													
					 } 	
		 		  BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE,NULL);
			    UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"地点说明: \r\n\r\n",14);
				//	UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"    ▁▁▁▁▁▁▁▁▁▁▁▁\r\n",31);	
          UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"    ▂▂▂▂▂▂▂▂▂▂▂▂\r\n\r\n",33);							 
	  // 打印 温湿度
          UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"当前土壤湿度:   ",16);
					 BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE,NULL);						 
            ucLen=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8); 
				   if(pinfo.Humi==0xff && pinfo.Temp==0xff)
					 {
				     memcpy(ucPrint + ucLen,"- - ",4);
             ucLen       += 4;  						 
					 } 
				   else
					 {					 	 
				   if(pinfo.Humi>=100)  pinfo.Humi=100;
						 sprintf(ucDisplay, "%.1f", pinfo.Humi); 
             memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
             ucLen     += strlen(ucDisplay);
					  }
				     memcpy(ucPrint + ucLen,"%\r\n",3);
             ucLen       += 3;   
             UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)ucPrint,ucLen);		
						
           BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE,NULL);
           UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"当前土壤温度:   ",16);
					 BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_ENABLE,NULL);	
						
            ucLen=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8); 
				   if(pinfo.Humi==0xff && pinfo.Temp==0xff)
					 {
				     memcpy(ucPrint + ucLen,"- - ",4);
             ucLen       += 4;  						 
					 } 
				   else
					 {					 
           if(pinfo.Temp>=70)  pinfo.Temp=70;	
				   else if(pinfo.Temp<=-40)  pinfo.Temp=-40;	
						 sprintf(ucDisplay, "%.1f", pinfo.Temp); 
             memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
             ucLen     += strlen(ucDisplay);
					  } 
				     memcpy(ucPrint + ucLen,"℃\r\n",4);
             ucLen       += 4;   
             UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)ucPrint,ucLen);	
             BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_UNDERLINE_UNABLE,NULL);					
	    // 打印 Logo
						UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"\r\n",2);	
						BspPrinter_Uartx_App(PRINTER_UARTX_APP_CMD_IMAGE,NULL);							
			// 打印 结尾	
            UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"\r\n",2);		
            UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"********************************\r\n",34);							
					  UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"            欢迎使用\r\n",22);             
            UART_DMA_Tx(BSP_PRINTER_UARTX,(uint8_t*)"********************************\r\n\r\n\r\n",38);	 							
}
/*
*********************************************************************************************************
*   功能说明:
*   形    参:
*   返 回 值:
*********************************************************************************************************
*/
void BspPrinter_DebugTestOnOff(uint8_t OnOff)
{
    static uint8_t s_i=0;
    if(OnOff==ON)
    {
        BspPrinter_DebugTest_Enable=1;
    }
    else
    {
        BspPrinter_DebugTest_Enable=0;
    }
    //
    if(BspPrinter_DebugTest_Enable==1)
    {
        s_i++;
        if(s_i>=10)
        {
            s_i=0;
            UART_DMA_Tx(3,(uint8_t*)"0123456789",10);
        }
    }
    OnOff=OnOff;
}
//-----------------------------------------------------------

