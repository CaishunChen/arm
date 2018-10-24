/**
  ******************************************************************************
  * @file    Bsp_PS2.c 
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
#if 0

#include<reg52.h>
sbit Key_Data =P3^4;//定义Keyboard数据端口引脚
sbit Key_CLK=P3^3;	//中断端口，时钟线
static unsigned char KeyV=0X00; //键值
static unsigned char BF = 0; //标识是否有字符被收到
static unsigned char IntNum = 0; //中断次数计数
//unsigned char  lie,hang;//lie为列值, hang为行值
bit dx=0;          //大小写标志 dx==1时大写状态

void delay(int x)   //延时程序
{	int i,j;
	for(i=0;i<600;i++)
	for(j=0;j<x;j++); }

void Keyboard_out(void) interrupt 2 //键盘中断处理 键值存储在 KeyV 中
{
  if ((IntNum>0) && (IntNum <9))
	{			
		KeyV = KeyV >> 1; //因键盘数据是低>>高，结合上一句所以右移一位
		if (Key_Data==1)  //当键盘数据线为1时   
		 {KeyV = KeyV | 0x80;}  //存储一位
	}
  IntNum++;   //中断次数加一(中断一次接收一位数据)
  if (IntNum > 10) //中断11次后数据发送完毕
	{   
		IntNum = 0; //当中断11次后表示一帧数据收完，清变量准备下一次接收
		BF = 1;    //标识有字符输入完了
		EA = 0;    //关中断等显示完后再开中
  // SBUF=KeyV;
	}
}
void Decode() //键值处理
{
	unsigned char data TempCyc;
    signed char data k;
	TempCyc=KeyV;
    if(BF==1)	//接收完一个有效数据时	
	{ 
	 BF=0;  //准备下一次接收
	 switch ( TempCyc ) //键值与显示字符的对应关系
	  {    //键值//        //对应字符//
		case 0x8A: k=0;  break; //0
		case 0x2C: k=1;  break; //1
		case 0x3C: k=2;  break; //2
		case 0x4C: k=3;  break; //3
		case 0x4A: k=4;  break; //4
		case 0x5C: k=5;  break; //5
		case 0x6C: k=6;  break; //6
		case 0x7A: k=7;  break; //7
		case 0x7C: k=8;  break; //8
		case 0x8C: k=9;  break; //9
		case 0x38: k=10; break; //a
		case 0x64: k=11; break; //b
		case 0x42: k=12; break; //c
		case 0x46: k=13; break; //d
		case 0x48: k=14; break; //e
		case 0x56: k=15; break; //f
		case 0x68: k=16; break; //g
		case 0x66: k=17; break; //h
		case 0x86: k=18; break; //i
		case 0x76: k=19; break; //j
		case 0x84: k=20; break; //k
		case 0x96: k=21; break; //l
		case 0x74: k=22; break; //m
		case 0x62: k=23; break; //n
		case 0x88: k=24; break; //o
		case 0x9A: k=25; break; //p 
		case 0x2A: k=26; break; //q
		case 0x5A: k=27; break; //r
		case 0x36: k=28; break; //s
		case 0x58: k=29; break; //t
		case 0x78: k=30; break; //u
		case 0x54: k=31; break; //v
		case 0x3A: k=32; break; //w
		case 0x44: k=33; break; //x
		case 0x6A: k=34; break; //y
		case 0x34: k=35; break; //z
	//	case 0x98: k=63; break; //;
	//	case 0xAA: k=65; break; //=
	//	case 0xF2: k=68; break; //+
	//	case 0x52: k=-16;break; //空格键
		case 0xB0: k=101;break;  //大小写控制键Caps lock

	    case 0xe0: k=0;break;	//小键盘0
		case 0xD2: k=1;break;	//小键盘1
    	case 0xE4: k=2;break;	//小键盘2
		case 0xF4: k=3;break;	//小键盘3
		case 0xD6: k=4;break;	//小键盘4
		case 0xE6: k=5;break;	//小键盘5
    	case 0xE8: k=6;break;	//小键盘6
		case 0xD8: k=7;break;	//小键盘7
		case 0xEA: k=8;break;	//小键盘8
		case 0xFA: k=9;break;	//小键盘9
    //	case 0xE2: k=;break;	//小键盘.
	//	case 0xB0: k=101;break;
    } 
	 
	 if(k==101) {dx=!dx;} //按下大小写控制键时 大小写标志取反
	 if(k==-16) //空格键按下时
	  {
	  }
   if(dx==0) //小写状态时显示	   （好使）	 不知什么原因，按后经常跟个0，这点注意
	  {
	    if(k>0 && k<=35) SBUF=k;
	//	  if(k>9 && k<=35)
	//		  {
	//		   SBUF=k;
	//		  }
	//	  if(k>=0 && k<=9) 
	//		  {
    //         SBUF=k;
	//		  }						
	  }
   if(dx==1)//大写状态时显示		(好使)
	  {
		  if(k>9 && k<=35)
			  {
			   SBUF=k;
			  }
		  if(k>=0 && k<=9) 
			  {
             SBUF=k;
			  }
	  }
   EA=1;
	  }	
}
	
void UART_CSH()
     {TMOD=0x20;
	  SCON=0x50;
	  PCON=0x00;
      TH1=0xFD;TL1=0xFD;
	  TR1=1;
	  TI=1;
	  }
void main() 
{   
    UART_CSH();
	IT1 = 1;  //设外部中断1为下降沿触发
	EA = 1;   //开总中断
	EX1=1;    //开中断 1
	while(1)
    {       
		Decode();	
		delay(100);
	}
}

#endif
