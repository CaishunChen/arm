/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-12-28
***********************************************************************************
*/
/*亮宁徽章:
void huitu ()
      { 
      	uchar j;
      	for(j=0x80;j<=0x9f;j++)  
		    { send_i(0x36);    //扩充指令动作集；绘图显示开?
			   send_i(j);    //设Y地址
		       send_i(0x80);    //设X地址
		       send_i(0x32);     // 基本指令
		      switch(j)
		          {
	  case 0x80: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break;          //0
	  case 0x81: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break;        
	  case 0x82: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break;        
	  case 0x83: send_d(0x00);send_d(0x40);send_d(0x02);send_d(0x00);break;       
	  case 0x84: send_d(0x01);send_d(0xf0);send_d(0x0f);send_d(0x80);break;       
	  case 0x85: send_d(0x03);send_d(0xf8);send_d(0x1f);send_d(0xc0);break;      
	  case 0x86: send_d(0x06);send_d(0x0c);send_d(0x30);send_d(0x60);break;        
	  case 0x87: send_d(0x0c);send_d(0x46);send_d(0x62);send_d(0x30);break;       
	  case 0x88: send_d(0x1b);send_d(0xfa);send_d(0x5f);send_d(0xd8);break;       
	  case 0x89: send_d(0x10);send_d(0x01);send_d(0xb0);send_d(0x68);break;        
	  
	  case 0x8a: send_d(0x21);send_d(0xf0);send_d(0x20);send_d(0x24);break;     
	  case 0x8b: send_d(0x21);send_d(0x10);send_d(0x4f);send_d(0x94);break;         
	  case 0x8c: send_d(0x61);send_d(0xf0);send_d(0x5f);send_d(0xd6);break;          
	  case 0x8d: send_d(0x60);send_d(0x00);send_d(0x03);send_d(0x06);break;          
	  case 0x8e: send_d(0x27);send_d(0xfc);send_d(0x03);send_d(0x04);break;          
	  case 0x8f: send_d(0x2c);send_d(0x06);send_d(0x03);send_d(0x04);break; 
	  case 0x90: send_d(0x11);send_d(0xf2);send_d(0x13);send_d(0x08);break;          //0
	  case 0x91: send_d(0x1b);send_d(0x10);send_d(0x8b);send_d(0x18);break;        
	  case 0x92: send_d(0x0e);send_d(0x10);send_d(0x8f);send_d(0x30);break;        
	  case 0x93: send_d(0x06);send_d(0x1f);send_d(0x87);send_d(0x60);break;       
	  
	  case 0x94: send_d(0x03);send_d(0x00);send_d(0x00);send_d(0xc0);break;       
	  case 0x95: send_d(0x01);send_d(0x80);send_d(0x01);send_d(0x80);break;      
	  case 0x96: send_d(0x00);send_d(0xe0);send_d(0x07);send_d(0x00);break;        
	  case 0x97: send_d(0x00);send_d(0x78);send_d(0x1e);send_d(0x00);break;       
	  case 0x98: send_d(0x00);send_d(0x1c);send_d(0x38);send_d(0x00);break;       
	  case 0x99: send_d(0x00);send_d(0x06);send_d(0x60);send_d(0x00);break;        
	  case 0x9a: send_d(0x00);send_d(0x03);send_d(0xc0);send_d(0x00);break;     
	  case 0x9b: send_d(0x00);send_d(0x01);send_d(0x80);send_d(0x00);break;         
	  case 0x9c: send_d(0x00);send_d(0x01);send_d(0x80);send_d(0x00);break;          
	  case 0x9d: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break;          
	  
	  case 0x9e: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break;          
	  case 0x9f: send_d(0x00);send_d(0x00);send_d(0x00);send_d(0x00);break; 	         
	  default: break;
	            }

         
          }
      	}
*/
//------------------------------- Includes --------------------
#include "Bsp_Lcd.h"
#include "Bsp_SoilTempHumi.h"
#include "Bsp_Gps.h"
#include "Bsp_Key.h"
#include "Bsp_Rtc.h"
#include "Bsp_Printer.h"
#include "Module_Memory.h"
#include "uctsk_AD.h"
//------------------------------- 用户变量 --------------------
#define x1      0x80
#define x2      0x88
#define y       0x80
#define COM     0
#define DAT     1
#define UNLOCK  0
#define LOCK    1
#define Delay_RW        1
#define Delay_Character 60

#define LOCK_SIGN   0x5A
#define LOCK_SHORT  0xA5
#define UNLOCK_SIGN 0xFFFF

enum PLACE_DATE
{
    PLACE_DATE_Year=1,
    PLACE_DATE_Month,
    PLACE_DATE_Day,
    PLACE_DATE_Hour,
    PLACE_DATE_Minute,
    PLACE_DATE_Second,
};

enum SARE_INTERVAL
{
    SARE_INTERVAL_HAND=0,
    SARE_INTERVAL_AUTO_2,
    SARE_INTERVAL_AUTO_5,
    SARE_INTERVAL_AUTO_10,
    SARE_INTERVAL_AUTO_20,
};

enum PER_ELECTRIC
{    
   PER_ELECTRIC_0=0,
	 PER_ELECTRIC_25,
	 PER_ELECTRIC_50,
	 PER_ELECTRIC_75,
	 PER_ELECTRIC_100,
};
/*
#define KEY_UP          BSP_KEY_E_KEY1
#define KEY_DOWN        BSP_KEY_E_KEY2
#define KEY_LEFT        BSP_KEY_E_KEY3
#define KEY_RIGHT       BSP_KEY_E_KEY4
#define KEY_INTER       BSP_KEY_E_KEY5
#define KEY_OFF         BSP_KEY_E_KEY6
#define KEY_SAVE        BSP_KEY_E_KEY7     
#define KEY_MENU        BSP_KEY_E_KEY8     // res1
#define KEY_PRINT       BSP_KEY_E_KEY9     // res2
//#define KEY_RES       BSP_KEY_E_KEY10    // res3
*/
#define KEY_UP          BSP_KEY_E_KEY1
#define KEY_RIGHT       BSP_KEY_E_KEY2
#define KEY_MENU        BSP_KEY_E_KEY3
#define KEY_LEFT        BSP_KEY_E_KEY4
#define KEY_DOWN        BSP_KEY_E_KEY5
#define KEY_OFF         BSP_KEY_E_KEY6
#define KEY_INTER       BSP_KEY_E_KEY7     
#define KEY_SAVE        BSP_KEY_E_KEY8     // res1
#define KEY_PRINT       BSP_KEY_E_KEY9     // res2

const   uint8_t Lcd_INTERVAL[5][6]= {"OFF  "," 2分钟"," 5分钟","10分钟","10秒  "};
const   uint8_t Lcd_ELECTRIC[5][4]= {"  2%"," 25%"," 50%"," 75%","100%"};

static  uint8_t Logo_Bitmap[]=
{
    /*--  调入了一幅图像：C:\调试助手\取模软件\LOGO.bmp  --*/
    /*--  宽度x高度=128x64  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x03,0xFE,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x07,0xFC,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0F,0xFD,0xFF,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x1F,0xFD,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFD,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xF9,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xF9,0xFF,0xFF,0xFF,0xBE,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x00,0x01,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xC0,0x00,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xE0,0x00,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0x01,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xC3,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xE3,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xF3,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFB,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFB,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xEF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xCF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0x9F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFC,0x1F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xE0,0x1F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x88,0x00,0x03,0xE7,0xC0,0x00,0x00,
0x00,0x00,0x00,0x09,0x00,0x03,0xFF,0xC0,0x00,0x84,0x00,0x02,0x24,0x40,0x00,0x00,
0x00,0x00,0x03,0xFF,0xE0,0x00,0x10,0x00,0x01,0x06,0x80,0x02,0x24,0x40,0x00,0x00,
0x00,0x00,0x02,0x08,0x00,0x00,0x10,0x00,0x01,0x24,0x80,0x03,0xE7,0xC0,0x00,0x00,
0x00,0x00,0x02,0x08,0x00,0x00,0x10,0x00,0x02,0x20,0x80,0x00,0x11,0x00,0x00,0x00,
0x00,0x00,0x03,0xE4,0x80,0x00,0x10,0x00,0x06,0x21,0x00,0x00,0x10,0x80,0x00,0x00,
0x00,0x00,0x02,0x25,0x00,0x07,0xFF,0xE0,0x0A,0x11,0x00,0x0F,0xFF,0xC0,0x00,0x00,
0x00,0x00,0x02,0x22,0x20,0x00,0x10,0x00,0x02,0x11,0x00,0x00,0x6C,0x00,0x00,0x00,
0x00,0x00,0x04,0xAD,0x20,0x00,0x10,0x00,0x02,0x0A,0x00,0x01,0x83,0x00,0x00,0x00,
0x00,0x00,0x04,0x50,0xE0,0x00,0x28,0x00,0x02,0x0A,0x00,0x0E,0x00,0xE0,0x00,0x00,
0x00,0x00,0x09,0xFF,0x80,0x00,0x24,0x00,0x02,0x04,0x00,0x03,0xEF,0xC0,0x00,0x00,
0x00,0x00,0x01,0x24,0x80,0x00,0x42,0x00,0x02,0x0A,0x00,0x02,0x28,0x80,0x00,0x00,
0x00,0x00,0x01,0x24,0x80,0x00,0x81,0x80,0x02,0x11,0x00,0x02,0x28,0x80,0x00,0x00,
0x00,0x00,0x01,0x24,0x80,0x01,0x00,0xE0,0x02,0x20,0xE0,0x02,0x28,0x80,0x00,0x00,
0x00,0x00,0x0F,0xFF,0xE0,0x02,0x00,0x40,0x02,0xC0,0x40,0x03,0xEF,0x80,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x1E,0xEF,0xFB,0xB9,0xC7,0xCF,0x84,0x77,0x0D,0xDF,0x18,0x7C,0x00,0x00,
0x00,0x00,0x22,0x44,0x89,0x92,0x45,0x42,0x04,0x32,0x08,0x84,0x24,0x10,0x00,0x00,
0x00,0x00,0x20,0x44,0x81,0x94,0x01,0x02,0x0A,0x32,0x05,0x04,0x42,0x10,0x00,0x00,
0x00,0x00,0x10,0x44,0x91,0x54,0x01,0x02,0x0A,0x2A,0x05,0x04,0x42,0x10,0x00,0x00,
0x00,0x00,0x0C,0x7C,0xF1,0x54,0x01,0x02,0x0A,0x2A,0x02,0x04,0x42,0x10,0x00,0x00,
0x00,0x00,0x02,0x44,0x91,0x54,0xE1,0x02,0x0E,0x2A,0x02,0x04,0x42,0x10,0x00,0x00,
0x00,0x00,0x02,0x44,0x81,0x34,0x41,0x02,0x11,0x26,0x02,0x04,0x5A,0x10,0x00,0x00,
0x00,0x00,0x22,0x44,0x89,0x32,0x41,0x02,0x11,0x26,0x02,0x04,0x24,0x10,0x00,0x00,
0x00,0x00,0x3C,0xEF,0xFB,0x91,0x83,0x8F,0xBB,0xF2,0x07,0x1F,0x1C,0x7C,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/*	
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x7D,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0xFD,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x07,0xFD,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0F,0xF9,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x1F,0xFB,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFB,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFB,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xF3,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xF3,0xFF,0xFF,0xFF,0x7C,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xC0,0x00,0x03,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0x80,0x00,0x03,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xC0,0x00,0x03,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFE,0x03,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x87,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xC7,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xE7,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xF7,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xF7,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xDF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0x9F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFE,0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xF8,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xC0,0x3F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x00,0x01,0x10,0x00,0x07,0xCF,0x80,0x00,0x00,
0x00,0x00,0x00,0x12,0x00,0x07,0xFF,0x80,0x01,0x08,0x00,0x04,0x48,0x80,0x00,0x00,
0x00,0x00,0x07,0xFF,0xC0,0x00,0x20,0x00,0x02,0x0D,0x00,0x04,0x48,0x80,0x00,0x00,
0x00,0x00,0x04,0x10,0x00,0x00,0x20,0x00,0x02,0x49,0x00,0x07,0xCF,0x80,0x00,0x00,
0x00,0x00,0x04,0x10,0x00,0x00,0x20,0x00,0x04,0x41,0x00,0x00,0x22,0x00,0x00,0x00,
0x00,0x00,0x07,0xC9,0x00,0x00,0x20,0x00,0x0C,0x42,0x00,0x00,0x21,0x00,0x00,0x00,
0x00,0x00,0x04,0x4A,0x00,0x0F,0xFF,0xC0,0x14,0x22,0x00,0x1F,0xFF,0x80,0x00,0x00,
0x00,0x00,0x04,0x44,0x40,0x00,0x20,0x00,0x04,0x22,0x00,0x00,0xD8,0x00,0x00,0x00,
0x00,0x00,0x09,0x5A,0x40,0x00,0x20,0x00,0x04,0x14,0x00,0x03,0x06,0x00,0x00,0x00,
0x00,0x00,0x08,0xA1,0xC0,0x00,0x50,0x00,0x04,0x14,0x00,0x1C,0x01,0xC0,0x00,0x00,
0x00,0x00,0x13,0xFF,0x00,0x00,0x48,0x00,0x04,0x08,0x00,0x07,0xDF,0x80,0x00,0x00,
0x00,0x00,0x02,0x49,0x00,0x00,0x84,0x00,0x04,0x14,0x00,0x04,0x51,0x00,0x00,0x00,
0x00,0x00,0x02,0x49,0x00,0x01,0x03,0x00,0x04,0x22,0x00,0x04,0x51,0x00,0x00,0x00,
0x00,0x00,0x02,0x49,0x00,0x02,0x01,0xC0,0x04,0x41,0xC0,0x04,0x51,0x00,0x00,0x00,
0x00,0x00,0x1F,0xFF,0xC0,0x04,0x00,0x80,0x05,0x80,0x80,0x07,0xDF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x3D,0xDF,0xF7,0x73,0x8F,0x9F,0x08,0xEE,0x1B,0xBE,0x30,0xF8,0x00,0x00,
0x00,0x00,0x44,0x89,0x13,0x24,0x8A,0x84,0x08,0x64,0x11,0x08,0x48,0x20,0x00,0x00,
0x00,0x00,0x40,0x89,0x03,0x28,0x02,0x04,0x14,0x64,0x0A,0x08,0x84,0x20,0x00,0x00,
0x00,0x00,0x20,0x89,0x22,0xA8,0x02,0x04,0x14,0x54,0x0A,0x08,0x84,0x20,0x00,0x00,
0x00,0x00,0x18,0xF9,0xE2,0xA8,0x02,0x04,0x14,0x54,0x04,0x08,0x84,0x20,0x00,0x00,
0x00,0x00,0x04,0x89,0x22,0xA9,0xC2,0x04,0x1C,0x54,0x04,0x08,0x84,0x20,0x00,0x00,
0x00,0x00,0x04,0x89,0x02,0x68,0x82,0x04,0x22,0x4C,0x04,0x08,0xB4,0x20,0x00,0x00,
0x00,0x00,0x44,0x89,0x12,0x64,0x82,0x04,0x22,0x4C,0x04,0x08,0x48,0x20,0x00,0x00,
0x00,0x00,0x79,0xDF,0xF7,0x23,0x07,0x1F,0x77,0xE4,0x0E,0x3E,0x38,0xF8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
*/
};

//------------------------------- 用户变量 --------------------
uint8_t   Lcd_foo,Lcd_Startauto=0;             //刷屏标志位 自动存储开始
uint8_t   Lcd_Lock=UNLOCK;                     //上锁
uint16_t  Lcd_SaveInterval,Lcd_LastIndexNum,Lcd_ShowIndexNum;
uint8_t   Lcd_Showtime[16],Lcd_CurrentTm[6];  //屏幕显示时间 获取当前时间
uint8_t   Lcd_Version[3]      ="1.0";         //软件版本号

BSP_PRINTER_LCD_INFO Lcd_History;
//------------------------------- 用户函数声明 ----------------
void  wr_lcd(uint8_t dat_comm,uint8_t content,uint8_t delaycoeff);
void  LCDTextOut(uint8_t adrh,uint8_t adrl,uint8_t *LCDCharArray,uint8_t len);
void  Menu_Logo(void);
void  Menu_Home(void);
void  Menu_total(void);
void  Menu_measure(void);
void  Menu_historyfind(void);
void  Menu_historyshow(void);
void  Menu_ElecPos(void);
void  Menu_Dateset(void);
void  Menu_Storetime(void);
void  Menu_ModifySensorAddr(void);
void  (*MenuFun)(void);

/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void BspLcd_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_LCD_RCC_ENABLE;
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    //
    GPIO_InitStructure.GPIO_Pin     =  BSP_LCD_CS_PIN;
    GPIO_Init(BSP_LCD_CS_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_LCD_SDI_PIN;
    GPIO_Init(BSP_LCD_SDI_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_LCD_SCLK_PIN;
    GPIO_Init(BSP_LCD_SCLK_PORT, &GPIO_InitStructure);
    //
    BSP_LCD_SCLK_H;
    BSP_LCD_SCLK_L;
    BSP_LCD_SCLK_H;
    BSP_LCD_SCLK_L;
    //
}
/******************************************************************
液晶子程序
起始字节：11111(R/W)(RS)0   [RS=0:指令；RS=1:数据]
写指令：0xF8          写数据：0xFA        读数据：0xFE
********************************************************************************************************/
//液晶发送数据
void wr_lcd(uint8_t dat_comm,uint8_t content,uint8_t delaycoeff)
{
    uint8_t a,i,j;
    a=content;
    BSP_LCD_CS_H;
    BSP_LCD_SCLK_L;
    BSP_LCD_SDI_H;
    for(i=0; i<5; i++)
    {
        BSP_LCD_SCLK_H;
        Count_DelayUs(delaycoeff);
        BSP_LCD_SCLK_L;
        Count_DelayUs(delaycoeff);
    }
    BSP_LCD_SDI_L;
//  Count_DelayUs(delaycoeff);
    BSP_LCD_SCLK_H;
    Count_DelayUs(delaycoeff);
    BSP_LCD_SCLK_L;
    Count_DelayUs(delaycoeff);
    if(dat_comm)
        BSP_LCD_SDI_H;   //data
    else
        BSP_LCD_SDI_L;   //command
    BSP_LCD_SCLK_H;
    Count_DelayUs(delaycoeff);
    BSP_LCD_SCLK_L;
    Count_DelayUs(delaycoeff);
    BSP_LCD_SDI_L;
    BSP_LCD_SCLK_H;
    Count_DelayUs(delaycoeff);
    BSP_LCD_SCLK_L;
    Count_DelayUs(delaycoeff);
    for(j=0; j<2; j++)
    {
        for(i=0; i<4; i++)
        {
            if((a&0x80)==0x80)
                BSP_LCD_SDI_H;
            else BSP_LCD_SDI_L;
            Count_DelayUs(delaycoeff);
            a=a<<1;
            BSP_LCD_SCLK_H;
            Count_DelayUs(delaycoeff);
            BSP_LCD_SCLK_L;
        }
        BSP_LCD_SDI_L;
        for(i=0; i<4; i++)
        {
            BSP_LCD_SCLK_H;
            Count_DelayUs(delaycoeff);
            BSP_LCD_SCLK_L;
            Count_DelayUs(delaycoeff);
        }
    }
}
//addh: 0~3         行地址 80-87  90-97 88-8f 98-9f
//addl: 0~7       列地址
//len  单个字符长度  文字*2
void    LCDTextOut(uint8_t adrh,uint8_t adrl,uint8_t *LCDCharArray,uint8_t len)
{
    wr_lcd (COM,0x30,Delay_RW);
    if((adrh==0)||(adrh==1))
    {
        wr_lcd (COM,x1+adrh*0x10+adrl,Delay_RW);
        while(len--)
        {
            wr_lcd (DAT,*LCDCharArray,Delay_Character);
            LCDCharArray++;
        }
    }
    if((adrh==2)||(adrh==3))
    {
        wr_lcd (COM,x2+(adrh-2)*0x10+adrl,Delay_RW);
        while(len--)
        {
            wr_lcd (DAT,*LCDCharArray,Delay_Character);
            LCDCharArray++;
        }
    }
}
//发送地址   /*开显示，开游标 0x0f 反白闪烁*/
void  YJ_add(unsigned char adrh,unsigned char adrl)
{
    if((adrh==0)||(adrh==1))
    {
        wr_lcd (COM,x1+adrh*0x10+adrl,Delay_RW);
        wr_lcd (COM,0x0f,Delay_RW);
    }
    if((adrh==2)||(adrh==3))
    {
        wr_lcd (COM,x2+(adrh-2)*0x10+adrl,Delay_RW);
        wr_lcd (COM,0x0f,Delay_RW);

    }
}
/*--------------清DDRAM------------------*/
void clrram (void)
{
    wr_lcd (COM,0x30,Delay_RW);     /*30---基本指令动作*/      //功能设定
    wr_lcd (COM,0x01,Delay_RW);     /*清屏，地址指针指向00H*/
    wr_lcd (COM,0x0c,Delay_RW);     /*开显示，关游标*/
}

/*----------------显示图形-----------------*/
//addh: 0~31            行地址
//addl: 0~15          列地址
void img_disp_Full(uint8_t *img)
{
    uint8_t i,j;
    wr_lcd (COM,0x36,Delay_RW);
    for(j=0; j<32; j++)
    {
        for(i=0; i<8; i++)
        {
            wr_lcd (COM,y+j,Delay_RW);
            wr_lcd (COM,x1+i,Delay_RW);
            wr_lcd (DAT,img[j*16+i*2],Delay_RW);
            wr_lcd (DAT,img[j*16+i*2+1],Delay_RW);
        }
    }
    wr_lcd (COM,y-32,Delay_RW);
    wr_lcd (COM,x2,Delay_RW);
    for(j=32; j<64; j++)
    {
        for(i=0; i<8; i++)
        {
            wr_lcd (COM,y+j-32,Delay_RW);
            wr_lcd (COM,x2+i,Delay_RW);
            wr_lcd (DAT,img[j*16+i*2],Delay_RW);
            wr_lcd (DAT,img[j*16+i*2+1],Delay_RW);
        }
    }
//  wr_lcd (comm,0x36);
}
/*-------------下半屏显示图形--------------*/
void img_disp_Down(uint8_t *img)
{
    uint8_t i,j;
    wr_lcd (COM,0x36,Delay_RW);
    for(j=0; j<32; j++)
    {
        for(i=0; i<8; i++)
        {
            wr_lcd (COM,y+j,Delay_RW);
            wr_lcd (COM,x2+i,Delay_RW);
            wr_lcd (DAT,img[j*16+i*2],Delay_RW);
            wr_lcd (DAT,img[j*16+i*2+1],Delay_RW);
        }
    }
//  wr_lcd (comm,0x36);
}
/*--------------显示点阵----------------*/
void lat_disp (uint8_t data1,uint8_t data2)
{
    uint8_t i,j,k,x;
    x=x1;
    wr_lcd (COM,0x36,Delay_RW);
    for(k=0; k<2; k++)
    {
        for(j=0; j<16; j++)
        {
            for(i=0; i<8; i++)
            {
                wr_lcd (COM,y+j*2,Delay_RW);
                wr_lcd (COM,x+i,Delay_RW);
                wr_lcd (DAT,data1,Delay_RW);
                wr_lcd (DAT,data1,Delay_RW);
            }
            for(i=0; i<8; i++)
            {
                wr_lcd (COM,y+j*2+1,Delay_RW);
                wr_lcd (COM,x+i,Delay_RW);
                wr_lcd (DAT,data2,Delay_RW);
                wr_lcd (DAT,data2,Delay_RW);
            }
        }
        x=x2;
    }
//  wr_lcd (comm,0x36);
}
/*-----------------------------------------------*/
//当data1=0xff,data2=0xff时,在x0,y0处反白显示16xl*yl.
void con_disp (uint8_t data1,uint8_t data2,uint8_t x0,uint8_t y0,uint8_t xl,uint8_t yl)
{
    uint8_t i,j;
    wr_lcd (COM,0x36,Delay_RW);
    for(j=0; j<yl; j++)
    {
        for(i=0; i<xl; i++)
        {
            wr_lcd (COM,y0+j,Delay_RW);
            wr_lcd (COM,x0+i,Delay_RW);
            wr_lcd (DAT,data1,Delay_RW);
            wr_lcd (DAT,data2,Delay_RW);
        }
    }
//  wr_lcd (comm,0x36);
}
/*****************************************************************
 函数名：ftostr
 输入参数：待转换数字，存放字符串，浮点小数后边位数
 描述：将浮点型数据转换为字符串
 返回值:字符串大小
*****************************************************************/
int ftostr(float fNum,char str[],int dotsize)
{
    // 定义变量
    int iSize=0;//记录字符串长度的数
    int n=0;//用作循环的临时变量
    char *p=str;//做换向时用的指针
    char *s=str;//做换向时用的指针
    char isnegative=0;//负数标志
    unsigned long int i_predot;//小数点前的数
    unsigned long int i_afterdot;//小数点后的数
    float f_afterdot;//实数型的小数部分

    //判断是否为负数
    if(fNum<0)
    {
        isnegative=1;//设置负数标志
        fNum=0-fNum;//将负数变为正数
    }

    i_predot=(unsigned long int)fNum;//将小数点之前的数变为整数
    f_afterdot=fNum-i_predot;//单独取出小数点之后的数
    //根据设定的要保存的小数点后的位数，将小数点后相应的位数变到小数点之前
    for(n=dotsize; n>0; n--)
    {
        f_afterdot=f_afterdot*10;
    }
    i_afterdot=(unsigned long int)f_afterdot;//将小数点后相应位数的数字变为整数

    //先将小数点后的数转换为字符串
    n=dotsize;
    while(i_afterdot>0|n>0)
    {
        n--;
        str[iSize++]=i_afterdot%10+'0';//对10取余并变为ASCII码
        i_afterdot=i_afterdot/10;//对10取商
    }
    str[iSize++] ='.';//加上小数点
    //处理小数点前为0的情况。
    if(i_predot==0)
        str[iSize++]='0';
    //再将小数点前的数转换为字符串
    while(i_predot>0)
    {
        str[iSize++]=i_predot%10+'0';//对10取余并变为ASCII码
        i_predot=i_predot/10;//对10取商
    }

    if(isnegative==1)
        str[iSize++]='-';//如果是负数，则在最后加上负号
    str[iSize] ='\0';//加上字符串结束标志

    p=str+iSize-1;//将P指针指向字符串结束标志之前

    for(; p-s>0; p--,s++) //将字符串中存储的数调头
    {
        *s^=*p;
        *p^=*s;
        *s^=*p;
    }
    // 返回指针字符串大小
    return iSize;
}
void BspLcd_Test(void)
{	
    BspLcd_Init();
    wr_lcd (COM,0x30,Delay_RW);     /*30---基本指令动作*/      //功能设定
    wr_lcd (COM,0x01,Delay_RW);     /*清屏，地址指针指向00H*/
    wr_lcd (COM,0x0c,Delay_RW);     /*开显示，关游标*/
    wr_lcd (COM,0x06,Delay_RW);     /*光标的移动方向*/
	
    MenuFun = Menu_Logo;
}

void Menu_Dateset(void)
{
    static uint8_t  place=PLACE_DATE_Year,Datevalue=0;
    MODULE_MEMORY_S_RECORD_INFO *ps_info;	
	
    if(Lcd_foo==1)
    {
        Lcd_foo=0;
        LCDTextOut(0,0,"修改当前时间页面",16);      //第一行初始化显示
        LCDTextOut(2,0,"20  年  月  日  ",16);
        LCDTextOut(3,0,"    时  分  秒  ",16);       

        LCDTextOut(2,0,&Lcd_Showtime[0],4);
        LCDTextOut(2,3,&Lcd_Showtime[4],2);
        LCDTextOut(2,5,&Lcd_Showtime[6],2);
        LCDTextOut(3,1,&Lcd_Showtime[8],2);
        LCDTextOut(3,3,&Lcd_Showtime[10],2);
        LCDTextOut(3,5,&Lcd_Showtime[12],2);
			  if(Lcd_Lock==LOCK)    LCDTextOut(1,3,"试用版本",8);		
			 
        switch(place)
        {
            case PLACE_DATE_Year:
                YJ_add(2,1);
                break;
            case PLACE_DATE_Month:
                YJ_add(2,3);
                break;
            case PLACE_DATE_Day:
                YJ_add(2,5);
                break;
            case PLACE_DATE_Hour:
                YJ_add(3,1);
                break;
            case PLACE_DATE_Minute:
                YJ_add(3,3);
                break;
            case PLACE_DATE_Second:
                YJ_add(3,5);
                break;
            default:
                break;
        }

    }

    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_LEFT:
                Lcd_foo=1;
                if(place<=PLACE_DATE_Second&&place>PLACE_DATE_Year)
                    place--;

                break;
            case KEY_RIGHT:
                Lcd_foo=1;
                if(place<PLACE_DATE_Second&&(int)place>=0)
                    place++;

                break;
            case KEY_DOWN:
                Lcd_foo=1;
                switch(place)
                {
                    case PLACE_DATE_Year:
                        Datevalue  = (Lcd_Showtime[2] -'0')*10+(Lcd_Showtime[3] -'0');
										    if(Datevalue==99)
													  Datevalue=0;
                        else if(Datevalue<99&&(int)Datevalue>=0)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[2]=0x30;
                            Lcd_Showtime[3]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[2]=0x31;
                            Lcd_Showtime[3]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[2]=0x32;
                            Lcd_Showtime[3]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[2]=0x33;
                            Lcd_Showtime[3]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[2]=0x34;
                            Lcd_Showtime[3]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[2]=0x35;
                            Lcd_Showtime[3]=Datevalue+0x30-50;
                        }
                        break;
                    case PLACE_DATE_Month:
                        Datevalue  = (Lcd_Showtime[4] -'0')*10+(Lcd_Showtime[5] -'0');
										    if(Datevalue==12)
													  Datevalue=1;										
                        else if(Datevalue<12&&Datevalue>=1)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[4]=0x30;
                            Lcd_Showtime[5]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=12)
                        {
                            Lcd_Showtime[4]=0x31;
                            Lcd_Showtime[5]=Datevalue+0x30-10;
                        }
                        break;
                    case PLACE_DATE_Day:
                        Datevalue  = (Lcd_Showtime[6] -'0')*10+(Lcd_Showtime[7] -'0');
										    if(Datevalue==31)
													  Datevalue=1;										
                        else if(Datevalue<31&&Datevalue>=1)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[6]=0x30;
                            Lcd_Showtime[7]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[6]=0x31;
                            Lcd_Showtime[7]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[6]=0x32;
                            Lcd_Showtime[7]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=31)
                        {
                            Lcd_Showtime[6]=0x33;
                            Lcd_Showtime[7]=Datevalue+0x30-30;
                        }
                        break;
                    case PLACE_DATE_Hour:
                        Datevalue  = (Lcd_Showtime[8] -'0')*10+(Lcd_Showtime[9] -'0');
										    if(Datevalue==23)
													  Datevalue=0;										
                        else if(Datevalue<23)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[8]=0x30;
                            Lcd_Showtime[9]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[8]=0x31;
                            Lcd_Showtime[9]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=24)
                        {
                            Lcd_Showtime[8]=0x32;
                            Lcd_Showtime[9]=Datevalue+0x30-20;
                        }
                        break;
                    case PLACE_DATE_Minute:
                        Datevalue  = (Lcd_Showtime[10] -'0')*10+(Lcd_Showtime[11] -'0');
										    if(Datevalue==59)
													  Datevalue=0;										
                        else if(Datevalue<59&&(int)Datevalue>=0)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[10]=0x30;
                            Lcd_Showtime[11]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[10]=0x31;
                            Lcd_Showtime[11]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[10]=0x32;
                            Lcd_Showtime[11]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[10]=0x33;
                            Lcd_Showtime[11]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[10]=0x34;
                            Lcd_Showtime[11]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[10]=0x35;
                            Lcd_Showtime[11]=Datevalue+0x30-50;
                        }
                        break;
                    case PLACE_DATE_Second:
                        Datevalue  = (Lcd_Showtime[12] -'0')*10+(Lcd_Showtime[13] -'0');
										    if(Datevalue==59)
													  Datevalue=0;										
                        else if(Datevalue<59&&(int)Datevalue>=0)
                            Datevalue++;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[12]=0x30;
                            Lcd_Showtime[13]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[12]=0x31;
                            Lcd_Showtime[13]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[12]=0x32;
                            Lcd_Showtime[13]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[12]=0x33;
                            Lcd_Showtime[13]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[12]=0x34;
                            Lcd_Showtime[13]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[12]=0x35;
                            Lcd_Showtime[13]=Datevalue+0x30-50;
                        }
                        break;
                    default:
                        break;
                }

                break;
            case KEY_UP:
                Lcd_foo=1;
                switch(place)
                {
                    case PLACE_DATE_Year:
                        Datevalue  = (Lcd_Showtime[2] -'0')*10+(Lcd_Showtime[3] -'0');
										    if(Datevalue==0)
													  Datevalue=99;										
                        else if(Datevalue<=99&&(int)Datevalue>0)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[2]=0x30;
                            Lcd_Showtime[3]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[2]=0x31;
                            Lcd_Showtime[3]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[2]=0x32;
                            Lcd_Showtime[3]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[2]=0x33;
                            Lcd_Showtime[3]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[2]=0x34;
                            Lcd_Showtime[3]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[2]=0x35;
                            Lcd_Showtime[3]=Datevalue+0x30-50;
                        }
                        else  if(Datevalue>=60 && Datevalue<=69)
                        {
                            Lcd_Showtime[2]=0x36;
                            Lcd_Showtime[3]=Datevalue+0x30-60;
                        }												
                        break;
                    case PLACE_DATE_Month:
                        Datevalue  = (Lcd_Showtime[4] -'0')*10+(Lcd_Showtime[5] -'0');
										    if(Datevalue==1)
													  Datevalue=12;											
                        else if(Datevalue<=12&&(int)Datevalue>1)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[4]=0x30;
                            Lcd_Showtime[5]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=12)
                        {
                            Lcd_Showtime[4]=0x31;
                            Lcd_Showtime[5]=Datevalue+0x30-10;
                        }
                        break;
                    case PLACE_DATE_Day:
                        Datevalue  = (Lcd_Showtime[6] -'0')*10+(Lcd_Showtime[7] -'0');
										    if(Datevalue==1)
													  Datevalue=31;											
                        else if(Datevalue<=31&&(int)Datevalue>1)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[6]=0x30;
                            Lcd_Showtime[7]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[6]=0x31;
                            Lcd_Showtime[7]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[6]=0x32;
                            Lcd_Showtime[7]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=31)
                        {
                            Lcd_Showtime[6]=0x33;
                            Lcd_Showtime[7]=Datevalue+0x30-30;
                        }
                        break;
                    case PLACE_DATE_Hour:
                        Datevalue  = (Lcd_Showtime[8] -'0')*10+(Lcd_Showtime[9] -'0');
										    if(Datevalue==0)
													  Datevalue=23;											
                        else if(Datevalue<=23&&(int)Datevalue>0)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[8]=0x30;
                            Lcd_Showtime[9]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[8]=0x31;
                            Lcd_Showtime[9]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=24)
                        {
                            Lcd_Showtime[8]=0x32;
                            Lcd_Showtime[9]=Datevalue+0x30-20;
                        }
                        break;
                    case PLACE_DATE_Minute:
                        Datevalue  = (Lcd_Showtime[10] -'0')*10+(Lcd_Showtime[11] -'0');
										    if(Datevalue==0)
													  Datevalue=59;												
                        else if(Datevalue<60&&(int)Datevalue>0)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[10]=0x30;
                            Lcd_Showtime[11]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[10]=0x31;
                            Lcd_Showtime[11]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[10]=0x32;
                            Lcd_Showtime[11]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[10]=0x33;
                            Lcd_Showtime[11]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[10]=0x34;
                            Lcd_Showtime[11]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[10]=0x35;
                            Lcd_Showtime[11]=Datevalue+0x30-50;
                        }
                        break;
                    case PLACE_DATE_Second:
                        Datevalue  = (Lcd_Showtime[12] -'0')*10+(Lcd_Showtime[13] -'0');
										    if(Datevalue==0)
													  Datevalue=59;											
                        else if(Datevalue<60&&(int)Datevalue>0)
                            Datevalue--;
                        if((int)Datevalue>=0 && Datevalue<=9)
                        {
                            Lcd_Showtime[12]=0x30;
                            Lcd_Showtime[13]=Datevalue+0x30;
                        }
                        else  if(Datevalue>=10 && Datevalue<=19)
                        {
                            Lcd_Showtime[12]=0x31;
                            Lcd_Showtime[13]=Datevalue+0x30-10;
                        }
                        else  if(Datevalue>=20 && Datevalue<=29)
                        {
                            Lcd_Showtime[12]=0x32;
                            Lcd_Showtime[13]=Datevalue+0x30-20;
                        }
                        else  if(Datevalue>=30 && Datevalue<=39)
                        {
                            Lcd_Showtime[12]=0x33;
                            Lcd_Showtime[13]=Datevalue+0x30-30;
                        }
                        else  if(Datevalue>=40 && Datevalue<=49)
                        {
                            Lcd_Showtime[12]=0x34;
                            Lcd_Showtime[13]=Datevalue+0x30-40;
                        }
                        else  if(Datevalue>=50 && Datevalue<=59)
                        {
                            Lcd_Showtime[12]=0x35;
                            Lcd_Showtime[13]=Datevalue+0x30-50;
                        }
                        break;
                    default:
                        break;
                }

                break;
            case KEY_SAVE:                    //清空
                      Lcd_foo=1;
                      switch(place)
                      {
                          case PLACE_DATE_Year:
                          case PLACE_DATE_Hour:
                          case PLACE_DATE_Minute:
                          case PLACE_DATE_Second:
                              memset(&Lcd_Showtime[place*2],0x30,2);
                              break;
                          case PLACE_DATE_Month:
                          case PLACE_DATE_Day:
                              Lcd_Showtime[place*2]=0x30;
                              Lcd_Showtime[place*2+1]=0x31;
                              break;
                          default:
                              break;
                      }

                      break;
            case KEY_PRINT:  
							      Lcd_foo=1;
                   if(Lcd_Lock==LOCK)   
                  { 
                   if( memcmp(&Lcd_Showtime[PLACE_DATE_Second*2],"00",0)==0&&(place==PLACE_DATE_Second))         
                    { 
							//			MODULE_MEMORY_S_RECORD_INFO *ps_info;											
                    ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
									  Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
									  ps_info->Lock_Enable=LOCK_SHORT;          //Lock_Enable标志位改变0xA5  暂时解锁 
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);		
											
                    memcpy(Lcd_Showtime, "20160219080000",14);
                    Lcd_CurrentTm[5]=(Lcd_Showtime[12] -'0')*10+(Lcd_Showtime[13] -'0');
                    Lcd_CurrentTm[4]=(Lcd_Showtime[10] -'0')*10+(Lcd_Showtime[11] -'0');
                    Lcd_CurrentTm[3]=(Lcd_Showtime[8] -'0')*10+(Lcd_Showtime[9] -'0');
                    Lcd_CurrentTm[2]=(Lcd_Showtime[6] -'0')*10+(Lcd_Showtime[7] -'0');
                    Lcd_CurrentTm[1]=(Lcd_Showtime[4] -'0')*10+(Lcd_Showtime[5] -'0');
                    Lcd_CurrentTm[0]=(Lcd_Showtime[2] -'0')*10+(Lcd_Showtime[3] -'0');
                    BspRtc_SetRealTime(NULL,NULL,NULL,Lcd_CurrentTm);

										Lcd_Lock=UNLOCK;										//Lock_Enable标志位改变0xA5  暂时解锁	
                    LCDTextOut(1,3,"即将到期",8);											
								    wr_lcd (COM,0x0c,Delay_RW); 
                    MODULE_OS_DELAY_MS(2000);
                  //  MenuFun = Menu_measure;
                  //  clrram();	
                    LCDTextOut(1,3,"        ",8);									
										MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
                    }
                  }
                   break;											
            case KEY_INTER:
                Lcd_foo=1;
								 
								 if( memcmp(Lcd_Showtime,"20110907050301",14)==0)
								 {	
									//	 MODULE_MEMORY_S_RECORD_INFO *ps_info;

                       ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
									     Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
										//	 ps_info->Lock_Enable=0x5A;    
                       ps_info->Lock_Enable=UNLOCK_SIGN; 
                       Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);
                        Lcd_Lock=UNLOCK;										//清Lock_Enable标志位 解锁
										    LCDTextOut(1,3,"恭喜激活",8);	
											  wr_lcd (COM,0x0c,Delay_RW); 
											 // MODULE_OS_DELAY_MS(2000);
									      Count_DelayUs(3000000);
                        MenuFun = Menu_measure;
                        clrram();
											MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
                      break;											
									}	
									else
									{	
											if(Lcd_Lock==LOCK) 
											{	
                       BspRtc_ReadRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
                        Lcd_Showtime[0]  = '2';
                        Lcd_Showtime[1]  = '0';
                        Lcd_Showtime[2] =  Lcd_CurrentTm[0]/10 + '0';
                        Lcd_Showtime[3] =  Lcd_CurrentTm[0]%10  +'0';

                        Lcd_Showtime[4] =  Lcd_CurrentTm[1]/10 + '0';
                        Lcd_Showtime[5] =  Lcd_CurrentTm[1]%10  + '0';

                        Lcd_Showtime[6] =  Lcd_CurrentTm[2]/10 + '0';
                        Lcd_Showtime[7] =  Lcd_CurrentTm[2]%10 + '0';

                        Lcd_Showtime[8] =  Lcd_CurrentTm[3]/10  + '0';
                        Lcd_Showtime[9] =  Lcd_CurrentTm[3]%10  + '0';

                        Lcd_Showtime[10] = Lcd_CurrentTm[4]/10   + '0';
                        Lcd_Showtime[11] = Lcd_CurrentTm[4]%10  + '0';

                        Lcd_Showtime[12] = Lcd_CurrentTm[5]/10   + '0';
                        Lcd_Showtime[13] = Lcd_CurrentTm[5]%10   + '0';	
											
                        LCDTextOut(1,3,"激活失败",8);	
                      }												
                  }											
                
                if(Lcd_Lock==UNLOCK) 
                {
						//		 MODULE_MEMORY_S_RECORD_INFO *ps_info;	
								    ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);	

                    Lcd_CurrentTm[5]=(Lcd_Showtime[12] -'0')*10+(Lcd_Showtime[13] -'0');
                    Lcd_CurrentTm[4]=(Lcd_Showtime[10] -'0')*10+(Lcd_Showtime[11] -'0');
                    Lcd_CurrentTm[3]=(Lcd_Showtime[8] -'0')*10+(Lcd_Showtime[9] -'0');
                    Lcd_CurrentTm[2]=(Lcd_Showtime[6] -'0')*10+(Lcd_Showtime[7] -'0');
                    Lcd_CurrentTm[1]=(Lcd_Showtime[4] -'0')*10+(Lcd_Showtime[5] -'0');
                    Lcd_CurrentTm[0]=(Lcd_Showtime[2] -'0')*10+(Lcd_Showtime[3] -'0');
									
								 if(ps_info->Lock_Enable==LOCK_SHORT) 
								 {	 
									 if(Lcd_CurrentTm[0]<=16) 
									 {
                    if(Lcd_CurrentTm[1]<=2) 
									  {
										   if(Lcd_CurrentTm[2]<=19) 
											 { 										 
                        BspRtc_SetRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
										    LCDTextOut(1,3,"设置成功",8);	
											 } 
											 else 
									    {
									     ps_info->Lock_Enable=LOCK_SIGN; 
									     Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);														
										   Lcd_Lock=LOCK;		
										   LCDTextOut(1,3,"需要激活",8);	
										   }  
									  } 
									  else 
									  {
									   ps_info->Lock_Enable=LOCK_SIGN; 
									   Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);													
										 Lcd_Lock=LOCK;		
										 LCDTextOut(1,3,"需要激活",8);	
										 } 	
                    }
								   else 
								   {  
									   ps_info->Lock_Enable=LOCK_SIGN; 
									   Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);												 
								     Lcd_Lock=LOCK;		
								     LCDTextOut(1,3,"需要激活",8);	
									  } 
									} 
                  else  
									{
                   BspRtc_SetRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
									 LCDTextOut(1,3,"设置成功",8);	 
									}	
							  //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);  
                }
								 wr_lcd (COM,0x0c,Delay_RW); 
                 MODULE_OS_DELAY_MS(1500);
                 LCDTextOut(1,3,"        ",8);
								
                break;
						case KEY_MENU:          //菜单键
               if(Lcd_Lock==UNLOCK) 
								{ 
                 Lcd_foo=1;
                 MenuFun = Menu_total;
								}	
                break;
            case KEY_OFF:
               if(Lcd_Lock==UNLOCK) 
								{ 							
                  Lcd_foo=1;
                  MenuFun = Menu_measure;
                  clrram();
								}		
                break;						
            default:
                break;
        }
    }
}

void Menu_Storetime(void)     //主界面
{
    static uint8_t select=1;

	
    if(Lcd_foo==1)
    {
        Lcd_foo=0; 
        LCDTextOut(0,0,"设置采样间隔页面",16);         //第一行初始化显示              		
        LCDTextOut(2,0,"采样间隔:       ",16);  
			
        switch(Lcd_SaveInterval)
        {
            case 0:
                select=SARE_INTERVAL_HAND;
                break;
            case 120:
                select=SARE_INTERVAL_AUTO_2;
                break;
            case 300:
                select=SARE_INTERVAL_AUTO_5;
                break;
            case 600:
                select=SARE_INTERVAL_AUTO_10;
                break;
            case 10:
                select=SARE_INTERVAL_AUTO_20;
                break;
            default:
                break;
        }
        
				LCDTextOut(2,5,(uint8_t *)Lcd_INTERVAL[select],6);

    }
    YJ_add(2,5);

    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_UP:
						case KEY_LEFT:	
                if(select>0&&select<=4)
                    select--;
                LCDTextOut(2,5,(uint8_t *)Lcd_INTERVAL[select],6);
                YJ_add(2,5);
                break;
            case KEY_DOWN:
						case KEY_RIGHT:	
                if(select<4)
                    select++;
                LCDTextOut(2,5,(uint8_t *)Lcd_INTERVAL[select],6);
                YJ_add(2,5);
                break;
            case KEY_INTER:
                Lcd_foo=1;
                switch(select)
                {
                    case SARE_INTERVAL_HAND:
                        Lcd_SaveInterval=0;
										    Lcd_Startauto=0;
                        break;
                    case SARE_INTERVAL_AUTO_2:
                        Lcd_SaveInterval=120;
                        break;
                    case SARE_INTERVAL_AUTO_5:
                        Lcd_SaveInterval=300;
                        break;
                    case SARE_INTERVAL_AUTO_10:
                        Lcd_SaveInterval=600;
                        break;
                    case SARE_INTERVAL_AUTO_20:
                        Lcd_SaveInterval= 10;
                        break;
                    default:
                        break;
                }
                {
                    MODULE_MEMORY_S_RECORD_INFO *ps_info;
                    ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
									  Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
                    ps_info->SaveInterval_s=Lcd_SaveInterval;
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_info,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
									
                }
								 LCDTextOut(2,5,(uint8_t *)Lcd_INTERVAL[select],6);
                 LCDTextOut(3,3,"设置成功",8);
								 wr_lcd (COM,0x0c,Delay_RW); 
                 MODULE_OS_DELAY_MS(1500);	
								 LCDTextOut(3,3,"        ",8);
                break;
						case KEY_MENU:          //菜单键	
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_measure;
                clrram();
                break;						
            default:
                break;
        }
    }
}

void Menu_ModifySensorAddr(void)     //主界面
{
    static uint8_t  select=1;
	
    if(Lcd_foo==1)
    {
        Lcd_foo=0;
        LCDTextOut(0,0,"设传感器地址页面",16);         //第一行初始化显示
        LCDTextOut(1,0,"注意:SET接高电平",16);         //第一行初始化显示        

			  LCDTextOut(3,1,"输入新地址: ",12);        //第二行初始化显示
        //   select=485_addr;
        if((int)select>=0 && select<=9)
        {
            wr_lcd (DAT,0x30,Delay_RW);
            wr_lcd (DAT,select+0x30,Delay_RW);
        }
        else  if(select>=10 && select<=19)
        {
            wr_lcd (DAT,0x31,Delay_RW);
            wr_lcd (DAT,select+0x30-10,Delay_RW);
        }
        else  if(select>=20 && select<=29)
        {
            wr_lcd (DAT,0x32,Delay_RW);
            wr_lcd (DAT,select+0x30-20,Delay_RW);
        }
    }
    YJ_add(3,7);

    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_UP:
                Lcd_foo=1;
                if(select>1)select--;

                break;
            case KEY_DOWN:
                Lcd_foo=1;
                if(select<29)select++;

                break;
            case KEY_LEFT:
                Lcd_foo=1;
                if(select>1)select--;

                break;
            case KEY_RIGHT:
                Lcd_foo=1;
                if(select<29)select++;

                break;
            case KEY_INTER:
                Lcd_foo=1;
                // 485_addr=select;
                 LCDTextOut(2,3,"设置成功",8);
								 wr_lcd (COM,0x0c,Delay_RW); 
                 MODULE_OS_DELAY_MS(1500);		
						     LCDTextOut(2,3,"        ",8); 
                break;
						case KEY_MENU:          //菜单键	
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_measure;
                clrram();
                break;						
            default:
                break;
        }
    }
}

void Menu_ElecPos(void)
{
uint16_t  ucVolt;
char    ucDisplay[8],ucPrint[14];
uint8_t ucLen,ucPer;	
static    uint16_t stimer=0;
    if(0)
    {
		 g_tGPS.EW = 'E';
	   g_tGPS.JingDu_Du = 121;g_tGPS.JingDu_Fen = 583416;  
	   g_tGPS.NS = 'N';
	   g_tGPS.WeiDu_Du = 37;g_tGPS.WeiDu_Fen = 232475; 
		 g_tGPS.UartOk= 1;
		 g_tGPS.PositionOk= 1;
	  }			
    if(Lcd_foo==0)
    {
        stimer++;
        if(stimer>=1000)
        {
            stimer=0;
            Lcd_foo=1;
        }
    }
    if(Lcd_foo==1)
    {
        Lcd_foo=0;                
        //电量显示    
         LCDTextOut(0,0,"电量位置显示页面",16);         //第一行初始化显示
         LCDTextOut(1,0,"电量:           ",16);
         LCDTextOut(2,0,"经:             ",16);
         LCDTextOut(3,0,"纬:             ",16);				
 				    ucVolt=ADC_mV[AD_E_CHANNEL_PWR]*2;
            if(ucVolt>=3850)
                ucPer=PER_ELECTRIC_100;
            else if(ucVolt<3850&&ucVolt>=3750)
                ucPer=PER_ELECTRIC_75;
            else if(ucVolt<3750&&ucVolt>=3680)
                ucPer=PER_ELECTRIC_50;
            else if(ucVolt<3680&&ucVolt>=3620)
                ucPer=PER_ELECTRIC_25;						
            else if(ucVolt<3620)
                ucPer=PER_ELECTRIC_0;     
            LCDTextOut(1,3,(uint8_t *)Lcd_ELECTRIC[ucPer],4);          
        //GPS位置显示
           if(g_tGPS.UartOk)                      // 1表示串口数据接收正常，即可以收到GPS的命令字符串
           {					
              if(g_tGPS.PositionOk==0)            // 定位有效标志, 0:未定位
              {
               LCDTextOut(2,2,"  正在定位中",12);        
               LCDTextOut(3,2,"  正在定位中",12);
              }
              else
              {						 
                    ucLen=0;
                    memset(ucPrint,0x20,14);
							      ucPrint[ucLen]=  g_tGPS.EW;
						        ucLen		+= 1;
                    memset(ucDisplay,0x20,8);
								    sprintf(ucDisplay, "%d", g_tGPS.JingDu_Du); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    memcpy(ucPrint + ucLen, "'",1);
                    ucLen       += 1;
                    memset(ucDisplay,0x20,8);
								    sprintf(ucDisplay, "%.4f", (float)g_tGPS.JingDu_Fen/10000); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));								    
                    ucLen     += strlen(ucDisplay);
                    LCDTextOut(2,2,(uint8_t *)&ucPrint,ucLen);

                    ucLen=0;
                    memset(ucPrint,0x20,14);
						        ucPrint[ucLen]=  g_tGPS.NS;
						        ucLen		+= 1;
                    memset(ucDisplay,0x20,8);
										sprintf(ucDisplay, "%d", g_tGPS.WeiDu_Du); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    memcpy(ucPrint + ucLen, "'",1);
                    ucLen       += 1;
                    memset(ucDisplay,0x20,8);
										sprintf(ucDisplay, "%.4f", (float)g_tGPS.WeiDu_Fen/10000); 
                    memcpy(ucPrint + ucLen, ucDisplay, strlen(ucDisplay));
                    ucLen     += strlen(ucDisplay);
                    ucPrint[ucLen++]=  0x20;
                    LCDTextOut(3,2,(uint8_t *)&ucPrint,ucLen);		
                }    	
						 } 	
	         else
           {	
             LCDTextOut(2,2,"  无法定位  ",12);         
             LCDTextOut(3,2,"  无法定位  ",12);														
					 } 							
                if(ucPer) 
							  {	
          			LCDTextOut(1,6,"    ",4); 									
								wr_lcd (COM,0x0c,Delay_RW);
                } 
								else 
							 {
									LCDTextOut(1,6,"不足",4);   
									YJ_add(1,4);
								}
    }
    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_INTER:
                Lcd_foo=1;
                Lcd_ShowIndexNum=Lcd_LastIndexNum; 
                MenuFun = Menu_measure;								
                clrram();
                break;
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_measure;								
                clrram();
                break;               
						case KEY_MENU:          //菜单键	
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;							
            default:
                break;
        }
    }
}

void Menu_historyfind(void)
{
char    ucDisplay[8],ucPrint[14];
uint8_t len;	

        	
    if(Lcd_foo==1)
    {
         Lcd_foo=0;   
        LCDTextOut(1,0,"确认键--查看",12);           //第一行初始化显示
        LCDTextOut(2,0,"左右键--条目翻页",16);       //第二行初始化显示
			  LCDTextOut(3,0,"上下键--内容翻页",16); 			
        LCDTextOut(0,0,"当前数据：    条",16);       //第一行初始化显示
			
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8);   
		      	sprintf(ucDisplay, "%d", Lcd_LastIndexNum); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            LCDTextOut(0,5,(uint8_t *)&ucPrint,len);  			
						
            wr_lcd (COM,0x0c,Delay_RW);

    }
    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_INTER:
                Lcd_foo=1;
                Lcd_ShowIndexNum=Lcd_LastIndexNum; 
						    clrram();
                MenuFun = Menu_historyshow;								
                clrram();
                break;
						case KEY_MENU:          //菜单键	
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;					
            default:
                break;
        }
    }
}

void Menu_historyshow(void)     //主界面
{
    char      ucDisplay[8],ucPrint[14];   
	  float     ucTemp,ucHumi;
    uint8_t   len,ucGet=1;	
	  static    uint8_t  page=0;

    if(Lcd_foo==1)
    {
        Lcd_foo=0;
				//时间
            LCDTextOut(0,0,&Lcd_History.TimeBuf[0],16);
				//条目
					  LCDTextOut(3,0,"条目:           ",16);
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8);   
			      sprintf(ucDisplay, "%d", Lcd_ShowIndexNum); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            ucPrint[len]= '/';
            len     += 1;					         
            memset(ucDisplay,0x20,8);   
			      sprintf(ucDisplay, "%d", Lcd_LastIndexNum); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            LCDTextOut(3,3,(uint8_t *)&ucPrint,len);     
                         
       if(page==0)
        {
           LCDTextOut(1,0,"历史湿度:       ",16);
           LCDTextOut(2,0,"历史温度:       ",16);
   	    //传感器
           if(Lcd_History.Humi==0 && Lcd_History.Temp==0)
	             ucGet=1;                                        //未接入传感器
 				   else
					 {	
						  ucGet=0; 
						 
                ucTemp =  Lcd_History.Temp;
                ucHumi=   Lcd_History.Humi;										
           }
					 
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8);   
				   if(ucGet)
					 {
				     memcpy(ucPrint + len,"- - ",4);
             len       += 4;  						 
					 } 
				   else
					 {					 	 
				   if(ucHumi>=100)  ucHumi=100;
						 sprintf(ucDisplay, "%.1f", ucHumi); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
             len     += strlen(ucDisplay);
					 }
				     memcpy(ucPrint + len,"%",1);
             len       += 1;   
             LCDTextOut(1,5,(uint8_t *)&ucPrint,len);         
             
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8); 
				   if(ucGet)
					 {
				     memcpy(ucPrint + len,"- - ",4);
             len       += 4;  						 
					 } 
				   else
					 {					 
           if(ucTemp>=70)  ucTemp=70;	
				   else if(ucTemp<=-40)  ucTemp=-40;	
             sprintf(ucDisplay, "%.1f", ucTemp); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
             len     += strlen(ucDisplay);
					  } 
				     memcpy(ucPrint + len,"C",1);
             len       += 1;   
             LCDTextOut(2,5,(uint8_t *)&ucPrint,len);  					  
					}
			  else if(page==1)
        {	
        //GPS
        LCDTextOut(1,0,"经:             ",16);				
        LCDTextOut(2,0,"纬:             ",16);
         if(Lcd_History.JingDu_EW==0 && Lcd_History.JingDu_Du==0)
         			LCDTextOut(1,2,"  无定位值  ",12);				 
         else
					 {
            len=0;
            memset(ucPrint,0x20,14);
            ucPrint[len]=  Lcd_History.JingDu_EW;
            len     += 1;
            memset(ucDisplay,0x20,8);
						sprintf(ucDisplay, "%d", Lcd_History.JingDu_Du); 
            memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            memcpy(ucPrint + len, "'",1);
            len       += 1;
            memset(ucDisplay,0x20,8);
						sprintf(ucDisplay, "%.4f", (float)Lcd_History.JingDu_Fen/10000); 
            memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            LCDTextOut(1,2,(uint8_t *)&ucPrint,len);
					 }
         					 
         if(Lcd_History.WeiDu_NS==0 || Lcd_History.WeiDu_Du==0)
					   LCDTextOut(2,2,"  无定位值  ",12);  
				 else
					 {
            len=0;
            memset(ucPrint,0x20,14);
            ucPrint[len]=  Lcd_History.WeiDu_NS;
            len     += 1;
            memset(ucDisplay,0x20,8);
						sprintf(ucDisplay, "%d", Lcd_History.WeiDu_Du); 
            memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            memcpy(ucPrint + len, "'",1);
            len       += 1;
            memset(ucDisplay,0x20,8);
						sprintf(ucDisplay, "%.4f", (float)Lcd_History.WeiDu_Fen/10000); 
            memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
            len     += strlen(ucDisplay);
            ucPrint[len]=  0x20;
            len     += 1;						 
            LCDTextOut(2,2,(uint8_t *)&ucPrint,len);
						}
    		}	
				
        wr_lcd (COM,0x0c,Delay_RW);


    }

    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case KEY_LEFT:
                Lcd_foo=1;
									  if(Lcd_ShowIndexNum==1)
                      Lcd_ShowIndexNum=Lcd_LastIndexNum;
                    else if(Lcd_ShowIndexNum>1) Lcd_ShowIndexNum--;
                   // LCDTextOut(0,0,"查询中......     ",16);
                    {
                    MODULE_MEMORY_S_RECORD      *ps_record;

                    ps_record=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_RECORD_R,(uint8_t*)ps_record,(uint8_t*)&Lcd_ShowIndexNum);

                    Lcd_History.JingDu_EW=ps_record->JingDu_EW;
                    Lcd_History.WeiDu_NS=ps_record->WeiDu_NS;
                    Lcd_History.JingDu_Du=ps_record->JingDu_Du;
                    Lcd_History.WeiDu_Du=ps_record->WeiDu_Du;
                    Lcd_History.JingDu_Fen=ps_record->JingDu_Fen;
                    Lcd_History.WeiDu_Fen=ps_record->WeiDu_Fen;
                    Lcd_History.Temp=(float)ps_record->Temp/10;
                    Lcd_History.Humi=(float)ps_record->Humi/10;

                    memcpy(Lcd_CurrentTm, ps_record->TimeBuf, 6);
											
                    Lcd_History.TimeBuf[0] =  '2';
                    Lcd_History.TimeBuf[1] =  '0';
                    Lcd_History.TimeBuf[2] =  Lcd_CurrentTm[0]/10 + '0';
                    Lcd_History.TimeBuf[3] =  Lcd_CurrentTm[0]%10  +'0';
                    Lcd_History.TimeBuf[4] =  0x2F; 
                    Lcd_History.TimeBuf[5] =  Lcd_CurrentTm[1]/10 + '0';
                    Lcd_History.TimeBuf[6] =  Lcd_CurrentTm[1]%10  + '0';
                    Lcd_History.TimeBuf[7] =  0x2D; 
                    Lcd_History.TimeBuf[8] =  Lcd_CurrentTm[2]/10 + '0';
                    Lcd_History.TimeBuf[9] =  Lcd_CurrentTm[2]%10 + '0';
                    Lcd_History.TimeBuf[10] =  0x20; 
                    Lcd_History.TimeBuf[11] =  Lcd_CurrentTm[3]/10  + '0';
                    Lcd_History.TimeBuf[12] =  Lcd_CurrentTm[3]%10  + '0';
                    Lcd_History.TimeBuf[13] =  0x3A; 
                    Lcd_History.TimeBuf[14] = Lcd_CurrentTm[4]/10   + '0';
                    Lcd_History.TimeBuf[15] = Lcd_CurrentTm[4]%10  + '0';
                    
                        /*   if(FindFrequency<4)
                           {
                               LCDTextOut(0,0,"此段缺失数据    ",16);
                               Count_DelayUs(5000000);
                               MenuFun = Menu_historyshow;
                           }*/
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_record);
                    }
                break;
            case KEY_RIGHT:
                Lcd_foo=1; 
									if(Lcd_ShowIndexNum==Lcd_LastIndexNum)
                    Lcd_ShowIndexNum=1;
									else if(Lcd_ShowIndexNum<Lcd_LastIndexNum)
                    Lcd_ShowIndexNum++;
                 //   LCDTextOut(0,0,"查询中......    ",16);
                    {
                        MODULE_MEMORY_S_RECORD      *ps_record;

                        ps_record=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_RECORD_R,(uint8_t*)ps_record,(uint8_t*)&Lcd_ShowIndexNum);

                    Lcd_History.JingDu_EW=ps_record->JingDu_EW;
                    Lcd_History.WeiDu_NS=ps_record->WeiDu_NS;
                    Lcd_History.JingDu_Du=ps_record->JingDu_Du;
                    Lcd_History.WeiDu_Du=ps_record->WeiDu_Du;
                    Lcd_History.JingDu_Fen=ps_record->JingDu_Fen;
                    Lcd_History.WeiDu_Fen=ps_record->WeiDu_Fen;
                    Lcd_History.Temp=(float)ps_record->Temp/10;
                    Lcd_History.Humi=(float)ps_record->Humi/10;

                    memcpy(Lcd_CurrentTm, ps_record->TimeBuf, 6);

                    Lcd_History.TimeBuf[0] =  '2';
                    Lcd_History.TimeBuf[1] =  '0';
                    Lcd_History.TimeBuf[2] =  Lcd_CurrentTm[0]/10 + '0';
                    Lcd_History.TimeBuf[3] =  Lcd_CurrentTm[0]%10  +'0';
                    Lcd_History.TimeBuf[4] =  0x2F; 
                    Lcd_History.TimeBuf[5] =  Lcd_CurrentTm[1]/10 + '0';
                    Lcd_History.TimeBuf[6] =  Lcd_CurrentTm[1]%10  + '0';
                    Lcd_History.TimeBuf[7] =  0x2D; 
                    Lcd_History.TimeBuf[8] =  Lcd_CurrentTm[2]/10 + '0';
                    Lcd_History.TimeBuf[9] =  Lcd_CurrentTm[2]%10 + '0';
                    Lcd_History.TimeBuf[10] =  0x20; 
                    Lcd_History.TimeBuf[11] =  Lcd_CurrentTm[3]/10  + '0';
                    Lcd_History.TimeBuf[12] =  Lcd_CurrentTm[3]%10  + '0';
                    Lcd_History.TimeBuf[13] =  0x3A; 
                    Lcd_History.TimeBuf[14] = Lcd_CurrentTm[4]/10   + '0';
                    Lcd_History.TimeBuf[15] = Lcd_CurrentTm[4]%10  + '0';
                    
                        /*   if(FindFrequency<4)
                           {
                               LCDTextOut(0,0,"此段缺失数据    ",16);
                               Count_DelayUs(5000000);
                               MenuFun = Menu_historyshow;
                           }*/
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_record);
                    }
                break;
 						case KEY_UP:	
						case KEY_DOWN:	
                Lcd_foo=1;
                if(page==0) {page=1; break;}
                if(page==1) {page=0; break;}
                break;               
            case KEY_PRINT:          //打印键								
			
                break;								
            case KEY_MENU:          //菜单键								
            case KEY_INTER:
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_measure;
                clrram();
                break;						
            default:
                break;
        }
    }
}
static int16_t BspLcd_temp=0;
static uint16_t BspLcd_humi=0;

void Menu_measure(void)     //主界面
{
    char      ucDisplay[8],ucPrint[14];   
	  float     ucTemp,ucHumi;
    uint8_t   len,ucGet=1;	
    static    uint16_t stimer=0;
	  BSP_PRINTER_LCD_INFO Print_Measure;

    if(Lcd_foo==0)
    {
        stimer++;
        if(stimer>=300)
        {
            stimer=0;
            Lcd_foo=1;
        }
    }
    if(Lcd_foo==1)
    {
        Lcd_foo=0;
        LCDTextOut(1,0,"当前湿度:       ",16);
        LCDTextOut(2,0,"当前温度:       ",16);
				//时间	
                 BspRtc_ReadRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
                    Lcd_Showtime[0] =  '2';
                    Lcd_Showtime[1] =  '0';
                    Lcd_Showtime[2] =  Lcd_CurrentTm[0]/10 + '0';
                    Lcd_Showtime[3] =  Lcd_CurrentTm[0]%10  +'0';
                    Lcd_Showtime[4] =  0x2F; 
                    Lcd_Showtime[5] =  Lcd_CurrentTm[1]/10 + '0';
                    Lcd_Showtime[6] =  Lcd_CurrentTm[1]%10  + '0';
                    Lcd_Showtime[7] =  0x2D; 
                    Lcd_Showtime[8] =  Lcd_CurrentTm[2]/10 + '0';
                    Lcd_Showtime[9] =  Lcd_CurrentTm[2]%10 + '0';
                    Lcd_Showtime[10] =  0x20; 
                    Lcd_Showtime[11] =  Lcd_CurrentTm[3]/10  + '0';
                    Lcd_Showtime[12] =  Lcd_CurrentTm[3]%10  + '0';
                    Lcd_Showtime[13] =  0x3A; 
                    Lcd_Showtime[14] = Lcd_CurrentTm[4]/10   + '0';
                    Lcd_Showtime[15] = Lcd_CurrentTm[4]%10  + '0';	
/*					
        Lcd_Showtime[0]  = Lcd_CurrentTm[1]/10  + '0';
        Lcd_Showtime[1]  = Lcd_CurrentTm[1]%10  + '0';
        Lcd_Showtime[2] =  0x2D; 
        Lcd_Showtime[3] =  Lcd_CurrentTm[2]/10  + '0';  
        Lcd_Showtime[4] =  Lcd_CurrentTm[2]%10  + '0'; 
		    Lcd_Showtime[5] =  0x20;    
        Lcd_Showtime[6] =  Lcd_CurrentTm[3]/10  + '0';  
        Lcd_Showtime[7] =  Lcd_CurrentTm[3]%10  + '0';
	    	Lcd_Showtime[8] =  0x3A;
        Lcd_Showtime[9] =  Lcd_CurrentTm[4]/10 + '0';
        Lcd_Showtime[10] =  Lcd_CurrentTm[4]%10 + '0';  
		    Lcd_Showtime[11] = 0x3A; 
        Lcd_Showtime[12] = Lcd_CurrentTm[5]/10  + '0';   
        Lcd_Showtime[13] = Lcd_CurrentTm[5]%10  + '0'; 
*/				
        LCDTextOut(0,0,&Lcd_Showtime[0],16);    
         //存储方式 其他自动 0手动						
            if(Lcd_SaveInterval==0)                       
                LCDTextOut(3,0,"手动",4);
            else
                LCDTextOut(3,0,"自动",4);
         //1自动保存开始 0关闭
            if(Lcd_Startauto==0)                        
                LCDTextOut(3,3,"停",2);
            else
                LCDTextOut(3,3,"启",2);  
						
            ucGet=BspSoilTempHumi_Read(&BspLcd_temp,&BspLcd_humi); 
            if(ucGet==OK)
            {
                ucTemp =  (float)BspLcd_temp/10;
                ucHumi=   (float)BspLcd_humi/10;
            }						
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8);   
				   if(ucGet)
					 {
				     memcpy(ucPrint + len,"- - ",4);
             len       += 4;  						 
					 } 
				   else
					 {					 	 
				   if(ucHumi>=100)  ucHumi=100;
						 sprintf(ucDisplay, "%.1f", ucHumi); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
             len     += strlen(ucDisplay);
					  }
				     memcpy(ucPrint + len,"%",1);
             len       += 1;   
             LCDTextOut(1,5,(uint8_t *)&ucPrint,len);         
             
            len=0;
            memset(ucPrint,0x20,14);            
            memset(ucDisplay,0x20,8); 
				   if(ucGet)
					 {
				     memcpy(ucPrint + len,"- - ",4);
             len       += 4;  						 
					 } 
				   else
					 {					 
           if(ucTemp>=70)  ucTemp=70;	
				   else if(ucTemp<=-40)  ucTemp=-40;	
						 sprintf(ucDisplay, "%.1f", ucTemp); 
             memcpy(ucPrint + len, ucDisplay, strlen(ucDisplay));
             len     += strlen(ucDisplay);
					  } 
				     memcpy(ucPrint + len,"C",1);
             len       += 1;   
             LCDTextOut(2,5,(uint8_t *)&ucPrint,len);  
						
            if(g_tGPS.UartOk)                      // 1表示串口数据接收正常，即可以收到GPS的命令字符串
            {
                if(g_tGPS.PositionOk==0)            // 定位有效标志, 0:未定位
                    LCDTextOut(3,5,"定位中",6);
                else
                   LCDTextOut(3,5,"已定位",6);
            }
            else
                LCDTextOut(3,5,"无定位",6);						
				
        wr_lcd (COM,0x0c,Delay_RW);


    }

    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
						case KEY_UP:	
                Lcd_foo=1;
                break;
						case KEY_DOWN:	
                Lcd_foo=1;
                break;
            case KEY_SAVE:               
                if(Lcd_SaveInterval==0)
                {
                    MODULE_MEMORY_S_RECORD      *ps_record;
                    ps_record=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    if(ucGet==OK)
                   {   
                    ps_record->Temp=BspLcd_temp;
                    ps_record->Humi=BspLcd_humi;
                    }	
									 else
	                  {   
                    ps_record->Temp=0;
                    ps_record->Humi=0;
                    }										 
                 if(g_tGPS.UartOk==1 && g_tGPS.PositionOk!=0)
                {
                ps_record->JingDu_EW =  g_tGPS.EW;
                ps_record->JingDu_Du =  g_tGPS.JingDu_Du;
                ps_record->JingDu_Fen=  g_tGPS.JingDu_Fen;
                ps_record->WeiDu_NS  =  g_tGPS.NS;
                ps_record->WeiDu_Du  =  g_tGPS.WeiDu_Du;
                ps_record->WeiDu_Fen =  g_tGPS.WeiDu_Fen;
                }
				       else
               {
                ps_record->JingDu_EW =  0;
                ps_record->JingDu_Du =  0;
                ps_record->JingDu_Fen=  0;
                ps_record->WeiDu_NS  =  0;
                ps_record->WeiDu_Du  =  0;
                ps_record->WeiDu_Fen =  0;
               }
                    BspRtc_ReadRealTime(NULL,NULL,NULL,ps_record->TimeBuf);
                    
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_RECORD_W,(uint8_t*)ps_record,NULL);                   
  
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_record);
				
				        LCDTextOut(3,0," OK ",4);
                }
								else
								{
									 Lcd_foo=1;
                   Lcd_Startauto = ~Lcd_Startauto;
								 }										 
               break;
            case KEY_PRINT:          //打印键	
           // case KEY_INTER:  		
                 if(g_tGPS.PositionOk) 	
									{ 
                   Print_Measure.JingDu_EW =  g_tGPS.EW;
                   Print_Measure.JingDu_Du =  g_tGPS.JingDu_Du;
                   Print_Measure.JingDu_Fen=  g_tGPS.JingDu_Fen;
                   Print_Measure.WeiDu_NS  =  g_tGPS.NS;
                   Print_Measure.WeiDu_Du  =  g_tGPS.WeiDu_Du;
                   Print_Measure.WeiDu_Fen =  g_tGPS.WeiDu_Fen;
									}
                   if(ucGet==0)
                   {
	                 Print_Measure.Temp=ucTemp;
                   Print_Measure.Humi=ucHumi;				 
					         }
									  else
									 { 
                    Print_Measure.Temp=0xff;
                    Print_Measure.Humi=0xff;	                  									
                    }
                  memcpy(Print_Measure.TimeBuf, Lcd_Showtime, 16);	
                  BspPrinter_PrintInfo(Print_Measure);
                break;								
            case KEY_MENU:          //菜单键								
            case KEY_INTER:
            case KEY_OFF:							
                Lcd_foo=1;
                MenuFun = Menu_total;
                break;						
            default:
                break;
        }
    }
}

void Menu_total(void)     //主界面
{
    static uint8_t  place=0;


    if(Lcd_foo==1)
    {
        Lcd_foo=0;
        clrram();
        if(place<=3)
        {
            LCDTextOut(0,0,"1.电量及位置显示",16);  //第一行初始化显示
            LCDTextOut(1,0,"2.时间日期设置  ",14);
            LCDTextOut(2,0,"3.采样间隔设置  ",14);
            LCDTextOut(3,0,"4.历史数据查看  ",14);
            YJ_add(place,0);
        }
        else if(place>3)
        {
            LCDTextOut(0,0,"5.传感器地址设置",16);
            LCDTextOut(1,0,"                ",16);
            LCDTextOut(2,0,"                ",16);
            LCDTextOut(3,0,"                ",16);
            YJ_add(place-4,0);
        }

    }
    BspKey_Main_10ms();
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {

            case KEY_UP:
                Lcd_foo=1;
                if(place<=4&&place>0)
                    place--;
                break;
            case KEY_DOWN:
                Lcd_foo=1;
								if(place==4)
									  { place=0; break;	}					
                if(place<4)
                    place++;
                break;
            case KEY_INTER:
                Lcd_foo=1;
                switch(place)
                {
                    case 0:
                        MenuFun = Menu_ElecPos;
												clrram();
                       break;
                    case 1:
                        BspRtc_ReadRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
                        Lcd_Showtime[0]  = '2';
                        Lcd_Showtime[1]  = '0';
                        Lcd_Showtime[2] =  Lcd_CurrentTm[0]/10 + '0';
                        Lcd_Showtime[3] =  Lcd_CurrentTm[0]%10  +'0';

                        Lcd_Showtime[4] =  Lcd_CurrentTm[1]/10 + '0';
                        Lcd_Showtime[5] =  Lcd_CurrentTm[1]%10  + '0';

                        Lcd_Showtime[6] =  Lcd_CurrentTm[2]/10 + '0';
                        Lcd_Showtime[7] =  Lcd_CurrentTm[2]%10 + '0';

                        Lcd_Showtime[8] =  Lcd_CurrentTm[3]/10  + '0';
                        Lcd_Showtime[9] =  Lcd_CurrentTm[3]%10  + '0';

                        Lcd_Showtime[10] = Lcd_CurrentTm[4]/10   + '0';
                        Lcd_Showtime[11] = Lcd_CurrentTm[4]%10  + '0';

                        Lcd_Showtime[12] = Lcd_CurrentTm[5]/10   + '0';
                        Lcd_Showtime[13] = Lcd_CurrentTm[5]%10   + '0';
                        MenuFun = Menu_Dateset;
												clrram();
                       break;
                    case 2:
                       {
                       MODULE_MEMORY_S_RECORD_INFO *ps_info;
												 
                       ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                       Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
                       Lcd_SaveInterval=ps_info->SaveInterval_s;
                       MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
                       }											
                        MenuFun = Menu_Storetime;
											 	clrram();
                       break;
                    case 3:                        
                    {
                    MODULE_MEMORY_S_RECORD_INFO *ps_info;
                    MODULE_MEMORY_S_RECORD      *ps_record;
                    ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
                    Lcd_LastIndexNum=ps_info->IndexNum;
										if(Lcd_LastIndexNum>=1000)	Lcd_LastIndexNum= 1000;
                    ps_record=(MODULE_MEMORY_S_RECORD*)ps_info;
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_RECORD_R,(uint8_t*)ps_record,(uint8_t*)&Lcd_LastIndexNum);

                    Lcd_History.JingDu_EW=ps_record->JingDu_EW;
                    Lcd_History.WeiDu_NS=ps_record->WeiDu_NS;
                    Lcd_History.JingDu_Du=ps_record->JingDu_Du;
                    Lcd_History.WeiDu_Du=ps_record->WeiDu_Du;
                    Lcd_History.JingDu_Fen=ps_record->JingDu_Fen;
                    Lcd_History.WeiDu_Fen=ps_record->WeiDu_Fen;
                    Lcd_History.Temp=(float)ps_record->Temp/10;
                    Lcd_History.Humi=(float)ps_record->Humi/10;

                    memcpy(Lcd_CurrentTm, ps_record->TimeBuf, 6);

                    Lcd_History.TimeBuf[0] =  '2';
                    Lcd_History.TimeBuf[1] =  '0';
                    Lcd_History.TimeBuf[2] =  Lcd_CurrentTm[0]/10 + '0';
                    Lcd_History.TimeBuf[3] =  Lcd_CurrentTm[0]%10  +'0';
                    Lcd_History.TimeBuf[4] =  0x2F; 
                    Lcd_History.TimeBuf[5] =  Lcd_CurrentTm[1]/10 + '0';
                    Lcd_History.TimeBuf[6] =  Lcd_CurrentTm[1]%10  + '0';
                    Lcd_History.TimeBuf[7] =  0x2D; 
                    Lcd_History.TimeBuf[8] =  Lcd_CurrentTm[2]/10 + '0';
                    Lcd_History.TimeBuf[9] =  Lcd_CurrentTm[2]%10 + '0';
                    Lcd_History.TimeBuf[10] =  0x20; 
                    Lcd_History.TimeBuf[11] =  Lcd_CurrentTm[3]/10  + '0';
                    Lcd_History.TimeBuf[12] =  Lcd_CurrentTm[3]%10  + '0';
                    Lcd_History.TimeBuf[13] =  0x3A; 
                    Lcd_History.TimeBuf[14] = Lcd_CurrentTm[4]/10   + '0';
                    Lcd_History.TimeBuf[15] = Lcd_CurrentTm[4]%10  + '0';
                    //释放缓存
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
                   }	
                         MenuFun = Menu_historyfind;	
                         clrram();									 
                        break;
                    case 4:
                         MenuFun = Menu_ModifySensorAddr;
										     clrram();
                        break;
                    default:
                        break;
                }
                break;
            case KEY_OFF:
                Lcd_foo=1;
                MenuFun = Menu_measure;
                clrram();
                break;
            default:
                break;
        }
    }

}

void Menu_Home(void)     //主界面
{
    static uint16_t uclock,stimer=0;
    static uint8_t  first=1;
    uint32_t CurrentUnixTime;	
		  
		if(first==1)
    {		  
        MODULE_MEMORY_S_RECORD_INFO *ps_info;			
			
        ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
        Lcd_SaveInterval=ps_info->SaveInterval_s;
			  uclock=ps_info->Lock_Enable;
			  
			  LCDTextOut(1,0,"便携式土壤测试仪",16);         //第一行初始化显示
        LCDTextOut(2,2,"Version:",8);                  //第二行初始化显示
        LCDTextOut(2,6,Lcd_Version,3);
   //读取当前时钟
        BspRtc_ReadRealTime(NULL,NULL,(time_t*)&CurrentUnixTime,NULL);
			//   if(CurrentUnixTime>=1455926500&&(uclock==UNLOCK_SIGN||uclock==LOCK_SHORT))  // 0x5A
       if(CurrentUnixTime>=1455926500&&(uclock==LOCK_SIGN||uclock==LOCK_SHORT))   //2016.2.20号以后到期
        {
			  LCDTextOut(3,1,"试用版本已到期",14); 
			  Lcd_Lock=LOCK;
		    }					
			  first=0;			  
			  //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);        		
    }
    stimer++;
    if(stimer>=240)
    {
        stimer=0;
			  if(Lcd_Lock==LOCK) 
				  {
           BspRtc_ReadRealTime(NULL,NULL,NULL,Lcd_CurrentTm);
           Lcd_Showtime[0]  = '2';
           Lcd_Showtime[1]  = '0';
           Lcd_Showtime[2] =  Lcd_CurrentTm[0]/10 + '0';
           Lcd_Showtime[3] =  Lcd_CurrentTm[0]%10  +'0';

           Lcd_Showtime[4] =  Lcd_CurrentTm[1]/10 + '0';
           Lcd_Showtime[5] =  Lcd_CurrentTm[1]%10  + '0';

           Lcd_Showtime[6] =  Lcd_CurrentTm[2]/10 + '0';
           Lcd_Showtime[7] =  Lcd_CurrentTm[2]%10 + '0';

           Lcd_Showtime[8] =  Lcd_CurrentTm[3]/10  + '0';
           Lcd_Showtime[9] =  Lcd_CurrentTm[3]%10  + '0';

           Lcd_Showtime[10] = Lcd_CurrentTm[4]/10   + '0';
           Lcd_Showtime[11] = Lcd_CurrentTm[4]%10  + '0';

           Lcd_Showtime[12] = Lcd_CurrentTm[5]/10   + '0';
           Lcd_Showtime[13] = Lcd_CurrentTm[5]%10   + '0';						
				  MenuFun = Menu_Dateset;
					} 
			 else
          MenuFun = Menu_measure;
        clrram();
        Lcd_foo=1;
    }

}
void Menu_Logo(void)     //主界面
{
    static uint16_t stimer=0;
    static uint8_t first=1;	  
 
    if(first==1)
    {		  	
			  lat_disp (0,0);
			  img_disp_Full (Logo_Bitmap);			
        first=0;			
    }   
    stimer++;
    if(stimer>=300)
    {
        stimer=0;
        MenuFun = Menu_Home;
        clrram();
    }

}	
void BspLcd_100ms(void)
{
    static uint8_t StartAutoBak=0;
    static uint8_t cmt=0;
    static uint32_t FirstTime=0;
    MODULE_MEMORY_S_RECORD_INFO *ps_info;
    MODULE_MEMORY_S_RECORD *ps_record;
    static uint8_t s_savesign=0;
    uint32_t CurrentUnixTime;
    uint32_t i32;
    uint16_t i16;
    cmt++;
    if(cmt<10)
    {
        return;
    }
    else
    {
        cmt=0;
    }

    if(Lcd_Startauto==0)
    {
        StartAutoBak=0;
        return;
    }
    if(StartAutoBak==0)
    {
        StartAutoBak=Lcd_Startauto;
        BspRtc_ReadRealTime(NULL,NULL,(time_t*)&FirstTime,NULL);
        CurrentUnixTime = FirstTime;
    }
    else
    {
        //读取当前时钟
        BspRtc_ReadRealTime(NULL,NULL,(time_t*)&CurrentUnixTime,NULL);
    }
    //申请缓存
    ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
		ps_record=(MODULE_MEMORY_S_RECORD*)ps_info;
    //读取参数
    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,(uint8_t*)ps_info,NULL);
    i16 = ps_info->SaveInterval_s;
    //判断
    i32 = CurrentUnixTime - FirstTime;
    if((i32%i16)<3 && s_savesign==0)
    {
        s_savesign=1;
        //存储
        {
            ps_record->Temp=BspLcd_temp;
            ps_record->Humi=BspLcd_humi;
        }
        if(g_tGPS.UartOk==1 && g_tGPS.PositionOk!=0)
        {
            ps_record->JingDu_EW =  g_tGPS.EW;
            ps_record->JingDu_Du =  g_tGPS.JingDu_Du;
            ps_record->JingDu_Fen=  g_tGPS.JingDu_Fen;
            ps_record->WeiDu_NS  =  g_tGPS.NS;
            ps_record->WeiDu_Du  =  g_tGPS.WeiDu_Du;
            ps_record->WeiDu_Fen =  g_tGPS.WeiDu_Fen;
        }
				else
        {
            ps_record->JingDu_EW =  0;
            ps_record->JingDu_Du =  0;
            ps_record->JingDu_Fen=  0;
            ps_record->WeiDu_NS  =  0;
            ps_record->WeiDu_Du  =  0;
            ps_record->WeiDu_Fen =  0;
        }					
        BspRtc_ReadRealTime(NULL,NULL,NULL,ps_record->TimeBuf);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_RECORD_W,(uint8_t*)ps_record,NULL);
        //
    }
    if(i32%i16>3 && s_savesign==1)
    {
        s_savesign=0;
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
