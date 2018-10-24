/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2016-01-21
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "includes.h"
#include "QR_XslTest.h"
#include "QR_Encode.h"
#include "bsp_tft_lcd.h"

/*
//0 0 5 5
//6 6 11 11
const uint8_t QrXslTest_Str[]={//微信名片，可以去百度上搜索二维码名片 查看
"BEGIN:VCARD\r\n"
"VERSION:3.0\r\n"
"N:外星人\r\n"//名字
"TEL:13888888888\r\n"//手机号
"TEL;TYPE=WORK,VOICE:0571-88888888\r\n"//工作号码
"TEL;TYPE=HOME,VOICE:0571-66666666\r\n"//家庭号码
"NOTE:QQ:12345\r\n"//QQ号码
"EMAIL:12345@qq.com\r\n"//邮箱地址
"ORG:公司名称\r\n"//公司
"TITLE:职位名称\r\n"
"URL:http://个人主页\r\n"
"ADR;TYPE=WORK:工作地址\r\n"//地址
"ADR;TYPE=HOME:家庭住址\r\n"
"END:VCARD"
};
*/
const uint8_t QrXslTest_Str[]=
{
    "XSL-测试二维码成功!\r\n"
};

void QR_Xsl_DisplayApp(int16_t xpos,int16_t ypos,uint8_t *qrcode_data)
{
    uint8_t i,j;
    uint16_t i16;
    //uint8_t qrencode_buff[20];
    EncodeData((char *)qrcode_data);
    //LCD_Fill_Rect(0,0,QR_XSL_LCD_YSIZE,QR_XSL_LCD_XSIZE,CL_WHITE);
    //
    i16=COUNT_MIN(QR_XSL_LCD_XSIZE,QR_XSL_LCD_YSIZE);
    if(m_nSymbleSize*2>i16)
    {
#ifdef   QR_XSL_EMWIN_ENABLE
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_Font8x16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_XOR);
        GUI_DispStringAt((char*)"The QR Code is too large!",10,60);
#else
        FONT_T sfont;
        sfont.FontCode=0;
        sfont.FrontColor=CL_YELLOW;
        sfont.BackColor=CL_BLACK;
        sfont.Space=0;
        LCD_DispStr(10,60,(char*)"The QR Code is too large!",&sfont);
#endif        
        return;        
    }
    //动态调整图像尺寸
    for(i=0; i<10; i++)
    {
        //整屏
        //if((m_nSymbleSize*i*2)>240) break;
        //半屏
        if((m_nSymbleSize*i*2)>(i16/2)) break;
    }
    //点大小
    i16=(i-1)*2;
    xpos +=(QR_XSL_LCD_XSIZE-m_nSymbleSize*i16)/2;
    ypos +=(QR_XSL_LCD_YSIZE-m_nSymbleSize*i16)/2;
    //将LCD ID打印到lcd_id数组。
    /*
    sprintf((char*)qrencode_buff,"size:%d piont:%d",m_nSymbleSize,i16);
    {
#ifdef   QR_XSL_EMWIN_ENABLE
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_Font8x16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_XOR);
        GUI_DispStringAt((char*)qrencode_buff,0,0);
#else      
        FONT_T sfont;
        sfont.FontCode=0;
        sfont.FrontColor=CL_YELLOW;
        sfont.BackColor=CL_BLACK;
        sfont.Space=0;
        LCD_DispStr(0,0,(char*)qrencode_buff,&sfont);
#endif          
    }
    */
    //画白色方块
    LCD_Fill_Rect(xpos-i16,\
                  ypos-i16,\
                  m_nSymbleSize*i16+2*i16,\
                  m_nSymbleSize*i16+2*i16,\
                  CL_WHITE);
    //
    for(i=0; i<m_nSymbleSize; i++)
    {
        for(j=0; j<m_nSymbleSize; j++)
        {
            if(m_byModuleData[i][j]==1)
            {
                LCD_Fill_Rect(xpos+i16*i,ypos+i16*j,(xpos+i16*(i+1)-1)-(xpos+i16*i),(ypos+i16*(j+1)-1)-(ypos+i16*j),CL_BLACK);
            }
        }

    }
}

