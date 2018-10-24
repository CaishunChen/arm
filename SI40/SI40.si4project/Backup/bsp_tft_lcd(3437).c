/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*
*********************************************************************************************************
*
*   ģ������ :  TFTҺ����ʾ������ģ��
*   �ļ����� :  bsp_tft_lcd.c
*   ��    �� :  V3.8
*   ˵    �� :  ֧�ֵ�LCD�ڲ�����оƬ�ͺ���: SPFD5420A��OTM4001A��R61509V ILI9341 ILI9488
*               ����оƬ�ķ��ʵ�ַΪ:  0x60000000
*   �޸ļ�¼ :
*       �汾��  ����       ����    ˵��
*       v1.0    2011-08-21 armfly  ST�̼���汾 V3.5.0�汾��
*                   a) ȡ�����ʼĴ����Ľṹ�壬ֱ�Ӷ���
*       V2.0    2011-10-16 armfly  ����R61509V������ʵ��ͼ����ʾ����
*       V2.1    2012-07-06 armfly  ����RA8875������֧��4.3����
*       V2.2    2012-07-13 armfly  �Ľ�LCD_DispStr������֧��12�����ַ�;�޸�LCD_DrawRect,�����һ����������
*       V2.3    2012-08-08 armfly  ���ײ�оƬ�Ĵ���������صĺ����ŵ��������ļ���֧��RA8875
*       V3.0    2013-05-20          ����ͼ��ṹ; �޸�   LCD_DrawIconActive  �޸�DispStr����֧���ı�͸��
*       V3.1    2013-06-12          ���LCD_DispStr()����BUG�������Ƕ�ֿ��к��ָ�������256���������ѭ����
*       V3.2    2013-06-28          ����Label�ؼ�, ����ʾ�ַ�����֮ǰ��ʱ���Զ�������������
*       V3.3    2013-06-29          FSMC��ʼ��ʱ������ʱ��дʱ��Ͷ�ʱ��ֿ����á� LCD_FSMCConfig ������
*       V3.4    2013-07-06          ������ʾ32λ��Alphaͼ��ĺ��� LCD_DrawIcon32
*       V3.5    2013-07-24          ������ʾ32λ��AlphaͼƬ�ĺ��� LCD_DrawBmp32
*       V3.6    2013-07-30          �޸� DispEdit() ֧��12�����ֶ���
*       V3.7    2015-05-22 XSL      ���� ILI9341 ����
*       V3.8    2017-11-24 XSL      ���� ILI9488 ����
*   Copyright (C), 2010-2011, ������
*********************************************************************************************************
*/

/*
    ������ʾ:
    TFT��������һ���12864������ʾ���Ŀ����������������������˴��ڻ�ͼ�Ļ��ƣ�������ƶ��ڻ��ƾֲ�ͼ��
    �Ƿǳ���Ч�ġ�TFT������ָ��һ����ͼ���ڣ�Ȼ�����еĶ�д�Դ�Ĳ��������������֮�ڣ����������ҪCPU��
    �ڴ��б���������Ļ���������ݡ�
*/

#include "includes.h"
#include "LCD_SPFD5420.h"
#include "LCD_RA8875.h"
#include "LCD_ILI9341.h"
#include "bsp_tft_lcd.h"
#include "fonts.h"

/* ����3����������Ҫ����ʹ����ͬʱ֧�ֲ�ͬ���� */
uint16_t g_ChipID    = IC_9341;        //����оƬID
uint16_t g_LcdHeight = LCD_28_HEIGHT;  //��ʾ���ֱ���-�߶�
uint16_t g_LcdWidth  = LCD_28_WIDTH;   //��ʾ���ֱ���-����
uint8_t s_ucBright   =  0;             //�������Ȳ���

static void LCD_CtrlLinesConfig(void);
static void LCD_FSMCConfig(void);

/*
*********************************************************************************************************
*   �� �� ��: LCD_InitHard
*   ����˵��: ��ʼ��LCD
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_InitHard(void)
{
    uint16_t id;
    //��ʱ
    //
#if   (defined (LCD_RA8875_SPI_EN))
    RA8875_InitSPI();
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO \
      || LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
    LCD_ILI9341_HwInit();
#endif
    //����LCD���ƿ���GPIO
    LCD_CtrlLinesConfig();
    //����FSMC�ӿڣ���������
    LCD_FSMCConfig();

    //FSMC���ú������ӳٲ��ܷ��������豸
    MODULE_OS_DELAY_MS(20);
    //��ȡLCD����оƬID
    id = SPFD5420_ReadID();
    if ((id == IC_5420) || (id ==  IC_61509) || (id == 0x5520))
    {
        SPFD5420_InitHard();
        //g_ChipID �ں����ڲ�������
    }
    else if (RA8875_ReadID() == 0x75)
    {
        g_ChipID = IC_8875;
        RA8875_InitHard();
    }
    else if (LCD_ILI9341_ReadID() == 0X9341)
    {
        LCD_ILI9341_Init();
        g_ChipID      = IC_9341;
        g_LcdHeight   = Lcd_Ili9341_s_dev.height;
        g_LcdWidth    = Lcd_Ili9341_s_dev.width;
    }
    else if (LCD_ILI9341_ReadID() == 0X9488)
    {
        LCD_ILI9341_Init();
        g_ChipID      = IC_9488;
        g_LcdHeight   = Lcd_Ili9341_s_dev.height;
        g_LcdWidth    = Lcd_Ili9341_s_dev.width;
    }
    else
    {
        //���δ�ܳɹ�ʶ����ȱʡ��4001������
        g_ChipID = IC_4001;
        g_LcdHeight = LCD_30_HEIGHT;
        g_LcdWidth = LCD_30_WIDTH;
    }
    //����
    LCD_ClrScr(CL_BLACK);

    //�򿪱��⣬����Ϊȱʡ����
    //LCD_SetBackLight(BRIGHT_DEFAULT);
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_GetChipDescribe
*   ����˵��: ��ȡLCD����оƬ���������ţ�������ʾ
*   ��    ��: char *_str : �������ַ�������˻�����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_GetChipDescribe(char *_str)
{
    switch (g_ChipID)
    {
        case IC_5420:
            strcpy(_str, CHIP_STR_5420);
            break;

        case IC_4001:
            strcpy(_str, CHIP_STR_4001);
            break;

        case IC_61509:
            strcpy(_str, CHIP_STR_61509);
            break;

        case IC_8875:
            strcpy(_str, CHIP_STR_8875);
            break;
        case IC_9341:
            strcpy(_str, CHIP_STR_9341);
            break;
        case IC_9488:
            strcpy(_str, CHIP_STR_9488);
            break;
        default:
            strcpy(_str, "Unknow");
            break;
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_GetHeight
*   ����˵��: ��ȡLCD�ֱ���֮�߶�
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
    return g_LcdHeight;
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_GetWidth
*   ����˵��: ��ȡLCD�ֱ���֮����
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
    return g_LcdWidth;
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DispOn
*   ����˵��: ����ʾ
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DispOn();
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DisplayOn();
    }
    else    /* 61509, 5420, 4001 */
    {
        SPFD5420_DispOn();
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DispOff
*   ����˵��: �ر���ʾ
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DispOff();
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DisplayOff();
    }
    else    /* 61509, 5420, 4001 */
    {
        SPFD5420_DispOff();
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_ClrScr
*   ����˵��: �����������ɫֵ����
*   ��    ��: _usColor : ����ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_ClrScr(uint32_t _usColor)
{
    if (g_ChipID == IC_8875)    /* RA8875 ��֧ */
    {
        RA8875_ClrScr(_usColor);
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_Clear(_usColor);
    }
    else    /* 5420��4001��61509 ��֧ */
    {
        SPFD5420_ClrScr(_usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DispStr
*   ����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ���
*   ��    ��:
*       _usX : X���꣬����3.0���������ΧΪ��0 - 399��
*       _usY : Y���꣬����3.0���������ΧΪ ��0 - 239��
*       _ptr  : �ַ���ָ��
*       _tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
    uint32_t i;
    uint8_t code1;
    uint8_t code2;
    uint32_t address;
    //uint8_t buf[24 * 24 / 8];   /* ���֧��24������ */
    uint8_t width;
    uint16_t m;
    uint8_t font_width,font_height;
    uint16_t font_bytes;
    uint16_t x, y;
    const uint8_t *pAscDot;
    uint8_t *pbuf;
    uint8_t res;

#ifdef USE_SMALL_FONT
    const uint8_t *pHzDot;
#else
    uint32_t AddrHZK;
#endif

    /* �������ṹΪ��ָ�룬��ȱʡ��16���� */
    if (_tFont->FontCode == FC_ST_12)
    {
        font_height = 12;
        font_width = 12;
        font_bytes = (12*16)/8;
        pAscDot = g_Ascii12;
#ifdef USE_SMALL_FONT
        pHzDot = g_Hz12;
#else
        AddrHZK = HZK12_ADDR;
#endif
    }
    else if (_tFont->FontCode == FC_ST_24)
    {
        font_height = 24;
        font_width = 24;
        font_bytes = (24*24)/8;
        pAscDot = g_Ascii24;

#ifdef USE_SMALL_FONT
        pHzDot = g_Hz24;
#else
        AddrHZK = HZK24_ADDR;
#endif
    }
    else if (_tFont->FontCode == FC_ST_32)
    {
        font_height = 32;
        font_width = 32;
        font_bytes = (32*32)/8;
        pAscDot = g_Ascii32;

#ifdef USE_SMALL_FONT
        pHzDot = g_Hz32;
#else
        AddrHZK = HZK32_ADDR;
#endif
    }
    else if (_tFont->FontCode == FC_ST_64)
    {
        font_height = 64;
        font_width = 64;
        font_bytes = (64*64)/8;
        pAscDot = g_Ascii64;

#ifdef USE_SMALL_FONT
        pHzDot = g_Hz64;
#else
        AddrHZK = HZK64_ADDR;
#endif
    }
    else
    {
        /* ȱʡ��16���� */
        font_height = 16;
        font_width = 16;
        font_bytes = (16*16)/8;
        pAscDot = g_Ascii16;

#ifdef USE_SMALL_FONT
        pHzDot = g_Hz16;
#else
        AddrHZK = HZK16_ADDR;
#endif
    }
    //���뻺��
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    /* ��ʼѭ�������ַ� */
    while (*_ptr != 0)
    {
        code1 = *_ptr;  /* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
        if (code1 < 0x80)
        {
            //��ascii�ַ������Ƶ�buf
            //---�����ֿ�
            if(_tFont->FontCode == FC_ST_16)
            {
                uint16_t k = 0;
                if('0'<=code1 && '9'>=code1)
                    k = (1*16)*(0 + code1 - '0') ;
                else if ('a'<=code1 && 'z'>=code1)
                    k = (1*16)*(10 + code1 - 'a');
                else if ('A'<=code1 && 'Z'>=code1)
                    k = (1*16)*(36 + code1 - 'A');
                else if (':' == code1)
                    k = (1*16)*62;
                else if ('-' == code1)
                    k = (1*16)*63;
                else if (' ' == code1)
                    k = (1*16)*64;
                else if ('(' == code1)
                    k = (1*16)*65;
                else if (')' == code1)
                    k = (1*16)*66;
                else if ('/' == code1)
                    k = (1*16)*67;
                else if ('*' == code1)
                    k = (1*16)*68;
                else if (',' == code1)
                    k = (1*16)*69;
                else if ('.' == code1)
                    k = (1*16)*70;
                else if ('!' == code1)
                    k = (1*16)*71;
                else if ('?' == code1)
                    k = (1*16)*72;
                else if ('_' == code1)
                    k = (1*16)*73;
                else
                    k = 0;
                memcpy(pbuf, &g_Ascii16[k], 2*24);
            }
            else if(_tFont->FontCode == FC_ST_24)
            {
                uint16_t k = 0;
                if('0'<=code1 && '9'>=code1)
                    k = (2*24)*(0 + code1 - '0') ;
                else if ('a'<=code1 && 'z'>=code1)
                    k = (2*24)*(10 + code1 - 'a');
                else if ('A'<=code1 && 'Z'>=code1)
                    k = (2*24)*(36 + code1 - 'A');
                else if (':' == code1)
                    k = (2*24)*62;
                else if ('-' == code1)
                    k = (2*24)*63;
                else if (' ' == code1)
                    k = (2*24)*64;
                else
                    k = 0;
                memcpy(pbuf, &g_Ascii24[k], 2*24);
            }
            else if(_tFont->FontCode == FC_ST_32)
            {
                uint16_t k = 0;
                if (':' == code1)
                    k = (16*32/8)*10;
                else if ('-' == code1)
                    k = (16*32/8)*11;
                else if ('%' == code1)
                    k = (16*32/8)*12;
                else if ('C' == code1)
                    k = (16*32/8)*13;
                else if (' ' == code1)
                {
                    k = (16*32/8)*14;
                }
                else
                    k = (16*32/8)*(code1 - '0') ;
                memcpy(pbuf, &g_Ascii32[k], 2*64);
            }
            else if(_tFont->FontCode == FC_ST_64)
            {
                uint16_t k = 0;
                if (':' == code1)
                    k = (32*64/8)*10;
                else if ('A' == code1)
                    k = (32*64/8)*11;
                else if ('P' == code1)
                    k = (32*64/8)*12;
                else if ('K' == code1)
                    k = (32*64/8)*13;
                else if ('X' == code1)
                {
                    k = (32*64/8)*14;
                }
                else if (' ' == code1)
                {
                    k = (32*64/8)*15;
                }
                else
                    k = (32*64/8)*(code1 - '0') ;
                memcpy(pbuf, &g_Ascii64[k], 4*64);
            }
            //---��׼�ֿ�
            else
            {
                memcpy(pbuf, &pAscDot[code1 * (font_bytes / 2)], (font_bytes / 2));
            }
            width = font_width / 2;
        }
        else
        {
            code2 = *++_ptr;
            if (code2 == 0)
            {
                break;
            }

            /* ����16�����ֵ����ַ
                ADDRESS = [(code1-0xa1) * 94 + (code2-0xa1)] * 32
                ;
            */
#ifdef USE_SMALL_FONT
            m = 0;
            while(1)
            {
                address = m * (font_bytes + 2);
                m++;
                if ((code1 == pHzDot[address + 0]) && (code2 == pHzDot[address + 1]))
                {
                    address += 2;
                    memcpy(pbuf, &pHzDot[address], font_bytes);
                    break;
                }
                else if ((pHzDot[address + 0] == 0xFF) && (pHzDot[address + 1] == 0xFF))
                {
                    /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
                    memset(pbuf, 0xFF, font_bytes);
                    break;
                }
            }
#else   /* ��ȫ�ֿ� */
            /* �˴���Ҫ�����ֿ��ļ����λ�ý����޸� */
            if (code1 >=0xA1 && code1 <= 0xA9 && code2 >=0xA1)
            {
                address = ((code1 - 0xA1) * 94 + (code2 - 0xA1)) * font_bytes + AddrHZK;
            }
            else if (code1 >=0xB0 && code1 <= 0xF7 && code2 >=0xA1)
            {
                address = ((code1 - 0xB0) * 94 + (code2 - 0xA1) + 846) * font_bytes + AddrHZK;
            }
            memcpy(pbuf, (const uint8_t *)address, font_bytes);
#endif

            width = font_width;
        }

        y = _usY;
        /* ��ʼˢLCD */
        for (m = 0; m < font_height; m++)   /* �ַ��߶� */
        {
            x = _usX;
            for (i = 0; i < width; i++) /* �ַ����� */
            {
                if(width%8)
                {
                    res=width/8+1;
                }
                else
                {
                    res=width/8;
                }
                if ((pbuf[m * res + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
                {
                    LCD_PutPixel(x, y, _tFont->FrontColor); /* ����������ɫΪ����ɫ */
                }
                else
                {
                    if (_tFont->BackColor != CL_MASK)   /* ͸��ɫ */
                    {
                        LCD_PutPixel(x, y, _tFont->BackColor);  /* ����������ɫΪ���ֱ���ɫ */
                    }
                }

                x++;
            }
            y++;
        }
        if (_tFont->Space > 0)
        {
            /* ������ֵ�ɫ��_tFont->usBackColor�������ּ����ڵ���Ŀ��ȣ���ô��Ҫ������֮�����(��ʱδʵ��) */
        }
        _usX += width + _tFont->Space;  /* �е�ַ���� */
        _ptr++;         /* ָ����һ���ַ� */
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf);
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_PutPixel
*   ����˵��: ��1������
*   ��    ��:
*           _usX,_usY : ��������
*           _usColor  : ������ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_PutPixel(_usX, _usY, _usColor);
    }
    else if(g_ChipID == IC_9341 \
            || g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawPoint(_usX, _usY, _usColor);
    }
    else
    {
        SPFD5420_PutPixel(_usX, _usY, _usColor);
    }
}
/*ww
*********************************************************************************************************
*   �� �� ��: LCD_DrawVDotLine
*   ����˵��: ������������
*   ��    ��:
*           _usX1,_usY1,_usY2: ��������
*           _usColor  : ������ɫ
*   �� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
void LCD_DrawVDotLine(uint16_t _usX1 ,uint16_t _usY1 , uint16_t _usY2, uint32_t _usColor)
{
    uint8_t i;
    for(i=_usY1; i<_usY2; i++)
    {
        if(i%2)
            LCD_PutPixel(_usX1,i,_usColor);
    }
}
/*ww
*********************************************************************************************************
*   �� �� ��: LCD_DrawHDotLine
*   ����˵��: ���ƺ�������
*   ��    ��:
*           _usX1,_usX2,_usY1: ��������
*           _usColor  : ������ɫ
*   �� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
void LCD_DrawHDotLine(uint16_t _usX1 ,uint16_t _usX2 , uint16_t _usY1, uint32_t _usColor)
{
    uint8_t i;
    for(i=_usX1; i<_usX2; i++)
    {
        if(i%2)
            LCD_PutPixel(i,_usY1,_usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_GetPixel
*   ����˵��: ��ȡ1������
*   ��    ��:
*           _usX,_usY : ��������
*           _usColor  : ������ɫ
*   �� �� ֵ: RGB��ɫֵ
*********************************************************************************************************
*/
uint32_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
    uint32_t usRGB;

    if (g_ChipID == IC_8875)
    {
        usRGB = RA8875_GetPixel(_usX, _usY);
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        usRGB = LCD_ILI9341_ReadPoint(_usX, _usY);
    }
    else
    {
        usRGB = SPFD5420_GetPixel(_usX, _usY);
    }

    return usRGB;
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawLine
*   ����˵��: ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
*   ��    ��:
*           _usX1, _usY1 : ��ʼ������
*           _usX2, _usY2 : ��ֹ��Y����
*           _usColor     : ��ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
    }
    else if(g_ChipID == IC_9341\
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
    }
    else
    {
        SPFD5420_DrawLine(_usX1 , _usY1 , _usX2, _usY2 , _usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawPoints
*   ����˵��: ���� Bresenham �㷨������һ��㣬������Щ�����������������ڲ�����ʾ��
*   ��    ��:
*           x, y     : ��������
*           _usColor : ��ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint32_t _usColor)
{
    uint16_t i;

    for (i = 0 ; i < _usSize - 1; i++)
    {
        LCD_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawRect
*   ����˵��: ����ˮƽ���õľ��Ρ�
*   ��    ��:
*           _usX,_usY: �������Ͻǵ�����
*           _usHeight : ���εĸ߶�
*           _usWidth  : ���εĿ���
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
    }
    else if(g_ChipID == IC_9341\
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
    }
    else
    {
        SPFD5420_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_Fill_Rect
*   ����˵��: ��һ����ɫֵ���һ�����Ρ���emWin ����ͬ������ LCD_FillRect����˼����»������֡�
*   ��    ��:
*           _usX,_usY: �������Ͻǵ�����
*           _usHeight : ���εĸ߶�
*           _usWidth  : ���εĿ���
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint32_t _usColor)
{
    uint16_t i;

    if (g_ChipID == IC_8875)
    {
        RA8875_FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);
    }
    else if(g_ChipID == IC_9341\
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_Fill(_usX, _usY, _usX+_usWidth,_usY+_usHeight, _usColor);
    }
    else
    {
        for (i = 0; i < _usHeight; i++)
        {
            SPFD5420_DrawHLine(_usX, _usY, _usX + _usWidth - 1, _usColor);
        }
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawCircle
*   ����˵��: ����һ��Բ���ʿ�Ϊ1������
*   ��    ��:
*           _usX,_usY  : Բ�ĵ�����
*           _usRadius  : Բ�İ뾶
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawCircle(_usX, _usY, _usRadius, _usColor);
    }
    else if(g_ChipID == IC_9341\
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawCircle(_usX, _usY, _usRadius, _usColor);
    }
    else
    {
        SPFD5420_DrawCircle(_usX, _usY, _usRadius, _usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawBMP
*   ����˵��: ��LCD����ʾһ��BMPλͼ��λͼ����ɨ�����: �����ң����ϵ���
*   ��    ��:
*           _usX, _usY : ͼƬ������
*           _usHeight  : ͼƬ�߶�
*           _usWidth   : ͼƬ����
*           _ptr       : ͼƬ����ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
    }
    else if(g_ChipID == IC_9341\
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
    }
    else
    {
        SPFD5420_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawWin
*   ����˵��: ��LCD�ϻ���һ������
*   ��    ��: �ṹ��ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawWin(WIN_T *_pWin)
{
#if 1
    uint16_t TitleHegiht;

    TitleHegiht = 20;

    // ���ƴ������
    LCD_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
    LCD_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);
    // ���ڱ�����
    LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);
    // �������
    LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht,
                  _pWin->Width - 4, WIN_BODY_COLOR);
    // �������ı�
    LCD_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->pCaption, _pWin->Font);
    // �����ı�
    if(_pWin->pText!=NULL)
    {
        char *pbuf;
        uint8_t i,res,y;
        _pWin->Font->BackColor=WIN_BODY_COLOR;
        pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        res = 0;
        i = 0;
        y = 0;
        while(1)
        {
            if(_pWin->pText[res] == 0)
            {
                pbuf[i++]=0;
                LCD_DispStr(_pWin->Left + 3, _pWin->Top + y + TitleHegiht + 2, pbuf, _pWin->Font);
                break;
            }
            else if(_pWin->pText[res] == '\r' || _pWin->pText[res] == '\n')
            {
                if(i!=0)
                {
                    pbuf[i++]=0;
                    LCD_DispStr(_pWin->Left + 3, _pWin->Top + y + TitleHegiht + 2, pbuf, _pWin->Font);
                    i=0;
                    if(_pWin->Font->FontCode==FC_ST_12)
                    {
                        y+=12;
                    }
                    else if(_pWin->Font->FontCode==FC_ST_16)
                    {
                        y+=16;
                    }
                    else if(_pWin->Font->FontCode==FC_ST_24)
                    {
                        y+=24;
                    }
                    else if(_pWin->Font->FontCode==FC_ST_32)
                    {
                        y+=32;
                    }
                    else if(_pWin->Font->FontCode==FC_ST_64)
                    {
                        y+=64;
                    }
                }
            }
            else
            {
                pbuf[i++] = _pWin->pText[res];
            }
            res++;
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#else
    if (g_ChipID == IC_8875)
    {
        uint16_t TitleHegiht;

        TitleHegiht = 28;

        /* ���ƴ������ */
        RA8875_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
        RA8875_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);

        /* ���ڱ����� */
        RA8875_FillRect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);

        /* ������� */
        RA8875_FillRect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht, _pWin->Width - 4, WIN_BODY_COLOR);

        //RA8875_SetFont(_pWin->Font.FontCode, 0, 0);
        RA8875_SetFont(RA_FONT_24, 0, 0);

        RA8875_SetBackColor(WIN_TITLE_COLOR);
        RA8875_SetFrontColor(WIN_CAPTION_COLOR);
        RA8875_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->Caption);
    }
    else
    {
        ;
    }
#endif
}


/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawIcon
*   ����˵��: ��LCD�ϻ���һ��ͼ�꣬�Ľ��Զ���Ϊ����
*   ��    ��: _pIcon : ͼ��ṹ
*             _tFont : ��������
*             _ucFocusMode : ����ģʽ��0 ��ʾ����ͼ��  1��ʾѡ�е�ͼ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
    const uint16_t *p;
    uint16_t usNewRGB;
    uint16_t x, y;      /* ���ڼ�¼�����ڵ�������� */

    p = _tIcon->pBmp;
    for (y = 0; y < _tIcon->Height; y++)
    {
        for (x = 0; x < _tIcon->Width; x++)
        {
            usNewRGB = *p++;    /* ��ȡͼ�����ɫֵ��ָ���1 */
            /* ��ͼ���4��ֱ���и�Ϊ���ǣ��������Ǳ���ͼ�� */
            if ((y == 0 && (x < 6 || x > _tIcon->Width - 7)) ||
                (y == 1 && (x < 4 || x > _tIcon->Width - 5)) ||
                (y == 2 && (x < 3 || x > _tIcon->Width - 4)) ||
                (y == 3 && (x < 2 || x > _tIcon->Width - 3)) ||
                (y == 4 && (x < 1 || x > _tIcon->Width - 2)) ||
                (y == 5 && (x < 1 || x > _tIcon->Width - 2))    ||

                (y == _tIcon->Height - 1 && (x < 6 || x > _tIcon->Width - 7)) ||
                (y == _tIcon->Height - 2 && (x < 4 || x > _tIcon->Width - 5)) ||
                (y == _tIcon->Height - 3 && (x < 3 || x > _tIcon->Width - 4)) ||
                (y == _tIcon->Height - 4 && (x < 2 || x > _tIcon->Width - 3)) ||
                (y == _tIcon->Height - 5 && (x < 1 || x > _tIcon->Width - 2)) ||
                (y == _tIcon->Height - 6 && (x < 1 || x > _tIcon->Width - 2))
               )
            {
                ;
            }
            else
            {
                if (_ucFocusMode != 0)  /* 1��ʾѡ�е�ͼ�� */
                {
                    /* ����ԭʼ���ص����ȣ�ʵ��ͼ�걻����ѡ�е�Ч�� */
                    uint16_t R,G,B;
                    uint16_t bright = 15;

                    /* rrrr rggg gggb bbbb */
                    R = (usNewRGB & 0xF800) >> 11;
                    G = (usNewRGB & 0x07E0) >> 5;
                    B =  usNewRGB & 0x001F;
                    if (R > bright)
                    {
                        R -= bright;
                    }
                    else
                    {
                        R = 0;
                    }
                    if (G > 2 * bright)
                    {
                        G -= 2 * bright;
                    }
                    else
                    {
                        G = 0;
                    }
                    if (B > bright)
                    {
                        B -= bright;
                    }
                    else
                    {
                        B = 0;
                    }
                    usNewRGB = (R << 11) + (G << 5) + B;
                }

                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
        }
    }

    /* ����ͼ���µ����� */
    {
        uint16_t len;
        uint16_t width;

        len = strlen(_tIcon->Text);

        if  (len == 0)
        {
            return; /* ���ͼ���ı�����Ϊ0������ʾ */
        }

        /* �����ı����ܿ��� */
        if (_tFont->FontCode == FC_ST_12)       /* 12���� */
        {
            width = 6 * (len + _tFont->Space);
        }
        else    /* FC_ST_16 */
        {
            width = 8 * (len + _tFont->Space);
        }


        /* ˮƽ���� */
        x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
        y = _tIcon->Top + _tIcon->Height + 2;
        LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
    }
}

/*
    alpha��Χ0~32��
    src�Ǵ����ж��ص����ݣ�dst��Ҫ��ϵ���ɫ��dst�ð׵ľ�����ͨ͸��Ч������������ɫ�����˾�һ����
*/
uint16_t Blend565(uint16_t src, uint16_t dst, uint8_t alpha)
{
    uint32_t src2;
    uint32_t dst2;

    src2 = ((src << 16) |src) & 0x07E0F81F;
    dst2 = ((dst << 16) | dst) & 0x07E0F81F;
    dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
    return (dst2 >> 16) | dst2;
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawIcon32
*   ����˵��: ��LCD�ϻ���һ��ͼ��, ����͸����Ϣ��λͼ(32λ�� RGBA). ͼ���´�����
*   ��    ��: _pIcon : ͼ��ṹ
*             _tFont : ��������
*             _ucFocusMode : ����ģʽ��0 ��ʾ����ͼ��  1��ʾѡ�е�ͼ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
    const uint8_t *p;
    uint16_t usOldRGB, usNewRGB;
    int16_t x, y;       /* ���ڼ�¼�����ڵ�������� */
    uint8_t R1,G1,B1,A; /* ������ɫ�ʷ��� */
    uint8_t R0,G0,B0;   /* ������ɫ�ʷ��� */

    p = (const uint8_t *)_tIcon->pBmp;
    p += 54;        /* ֱ��ָ��ͼ�������� */

    /* ����BMPλͼ���򣬴������ң���������ɨ�� */
    for (y = _tIcon->Height - 1; y >= 0; y--)
    {
        for (x = 0; x < _tIcon->Width; x++)
        {
            B1 = *p++;
            G1 = *p++;
            R1 = *p++;
            A = *p++;   /* Alpha ֵ(͸����)��0-255, 0��ʾ͸����1��ʾ��͸��, �м�ֵ��ʾ͸���� */

            if (A == 0x00)  /* ��Ҫ͸��,��ʾ���� */
            {
                ;   /* ����ˢ�±��� */
            }
            else if (A == 0xFF) /* ��ȫ��͸���� ��ʾ������ */
            {
                usNewRGB = RGB(R1, G1, B1);
                if (_ucFocusMode == 1)
                {
                    usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                }
                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
            else    /* ��͸�� */
            {
                /* ���㹫ʽ�� ʵ����ʾ��ɫ = ǰ����ɫ * Alpha / 255 + ������ɫ * (255-Alpha) / 255 */
                usOldRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);
                R0 = RGB565_R(usOldRGB);
                G0 = RGB565_G(usOldRGB);
                B0 = RGB565_B(usOldRGB);

                R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
                G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
                B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
                usNewRGB = RGB(R1, G1, B1);
                if (_ucFocusMode == 1)
                {
                    usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                }
                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
        }
    }

    /* ����ͼ���µ����� */
    {
        uint16_t len;
        uint16_t width;

        len = strlen(_tIcon->Text);

        if  (len == 0)
        {
            return; /* ���ͼ���ı�����Ϊ0������ʾ */
        }

        /* �����ı����ܿ��� */
        if (_tFont->FontCode == FC_ST_12)       /* 12���� */
        {
            width = 6 * (len + _tFont->Space);
        }
        else    /* FC_ST_16 */
        {
            width = 8 * (len + _tFont->Space);
        }


        /* ˮƽ���� */
        x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
        y = _tIcon->Top + _tIcon->Height + 2;
        LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawBmp32
*   ����˵��: ��LCD�ϻ���һ��32λ��BMPͼ, ����͸����Ϣ��λͼ(32λ�� RGBA)
*   ��    ��: _usX, _usY : ��ʾ����
*             _usHeight, _usWidth : ͼƬ�߶ȺͿ���
*             _pBmp : ͼƬ���ݣ���BMP�ļ�ͷ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawBmp32(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_pBmp)
{
    const uint8_t *p;
    uint16_t usOldRGB, usNewRGB;
    int16_t x, y;       /* ���ڼ�¼�����ڵ�������� */
    uint8_t R1,G1,B1,A; /* ������ɫ�ʷ��� */
    uint8_t R0,G0,B0;   /* ������ɫ�ʷ��� */

    p = (const uint8_t *)_pBmp;
    p += 54;        /* ֱ��ָ��ͼ�������� */

    /* ����BMPλͼ���򣬴������ң���������ɨ�� */
    for (y = _usHeight - 1; y >= 0; y--)
    {
        for (x = 0; x < _usWidth; x++)
        {
            B1 = *p++;
            G1 = *p++;
            R1 = *p++;
            A = *p++;   /* Alpha ֵ(͸����)��0-255, 0��ʾ͸����1��ʾ��͸��, �м�ֵ��ʾ͸���� */

            if (A == 0x00)  /* ��Ҫ͸��,��ʾ���� */
            {
                ;   /* ����ˢ�±��� */
            }
            else if (A == 0xFF) /* ��ȫ��͸���� ��ʾ������ */
            {
                usNewRGB = RGB(R1, G1, B1);
                //if (_ucFocusMode == 1)
                //{
                //  usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                //}
                LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
            }
            else    /* ��͸�� */
            {
                /* ���㹫ʽ�� ʵ����ʾ��ɫ = ǰ����ɫ * Alpha / 255 + ������ɫ * (255-Alpha) / 255 */
                usOldRGB = LCD_GetPixel(x + _usX, y + _usY);
                R0 = RGB565_R(usOldRGB);
                G0 = RGB565_G(usOldRGB);
                B0 = RGB565_B(usOldRGB);

                R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
                G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
                B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
                usNewRGB = RGB(R1, G1, B1);
                //if (_ucFocusMode == 1)
                //{
                //  usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                //}
                LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
            }
        }
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawLabel
*   ����˵��: ����һ���ı���ǩ
*   ��    ��: �ṹ��ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLabel(LABEL_T *_pLabel)
{
#if 1
    char dispbuf[256];
    uint16_t i;
    uint16_t NewLen;

    NewLen = strlen(_pLabel->pCaption);

    if (NewLen > _pLabel->MaxLen)
    {
        LCD_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->pCaption, _pLabel->Font);
        _pLabel->MaxLen = NewLen;
    }
    else
    {
        for (i = 0; i < NewLen; i++)
        {
            dispbuf[i] = _pLabel->pCaption[i];
        }
        for (; i < _pLabel->MaxLen; i++)
        {
            dispbuf[i] = ' ';       /* ĩβ���ո� */
        }
        dispbuf[i] = 0;
        LCD_DispStr(_pLabel->Left, _pLabel->Top, dispbuf, _pLabel->Font);
    }
#else
    if (g_ChipID == IC_8875)
    {
        RA8875_SetFont(_pLabel->Font->FontCode, 0, 0);  /* ����32�������壬�м��=0���ּ��=0 */

        RA8875_SetBackColor(_pLabel->Font->BackColor);
        RA8875_SetFrontColor(_pLabel->Font->FrontColor);

        RA8875_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->Caption);
    }
    else
    {

    }
#endif
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawCheckBox
*   ����˵��: ����һ������
*   ��    ��: �ṹ��ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawCheckBox(CHECK_T *_pCheckBox)
{
#if 1
    uint16_t x, y;

    /* Ŀǰֻ����16�����ֵĴ�С */

    /* ������� */
    x = _pCheckBox->Left;
    LCD_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
    LCD_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
    LCD_Fill_Rect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

    /* �����ı���ǩ */
    x = _pCheckBox->Left + CHECK_BOX_W + 2;
    y = _pCheckBox->Top + CHECK_BOX_H / 2 - 8;
    LCD_DispStr(x, y, _pCheckBox->pCaption, _pCheckBox->Font);

    if (_pCheckBox->Checked)
    {
        FONT_T font;

        font.FontCode = FC_ST_16;
        font.BackColor = CL_MASK;
        font.FrontColor = CHECK_BOX_CHECKED_COLOR;  /* ������ɫ */
        font.Space = 0;
        x = _pCheckBox->Left;
        LCD_DispStr(x + 3, _pCheckBox->Top + 3, "��", &font);
    }
#else
    if (g_ChipID == IC_8875)
    {
        uint16_t x;

        RA8875_SetFont(_pCheckBox->Font.FontCode, 0, 0);    /* ����32�������壬�м��=0���ּ��=0 */

        /* ���Ʊ�ǩ */
        //RA8875_SetBackColor(_pCheckBox->Font.BackColor);
        RA8875_SetBackColor(WIN_BODY_COLOR);
        RA8875_SetFrontColor(_pCheckBox->Font.FrontColor);
        RA8875_DispStr(_pCheckBox->Left, _pCheckBox->Top, _pCheckBox->Caption);

        /* ������� */
        x = _pCheckBox->Left + _pCheckBox->Width - CHECK_BOX_W;
        RA8875_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
        RA8875_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
        RA8875_FillRect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

        if (_pCheckBox->Checked)
        {
            RA8875_SetBackColor(CHECK_BOX_BACK_COLOR);
            RA8875_SetFrontColor(CL_RED);
            RA8875_DispStr(x + 3, _pCheckBox->Top + 3, "��");
        }
    }
    else
    {

    }
#endif

}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawEdit
*   ����˵��: ��LCD�ϻ���һ���༭��
*   ��    ��: _pEdit �༭��ṹ��ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawEdit(EDIT_T *_pEdit)
{
#if 1
    uint16_t len, x, y;
    uint8_t width;

    /* ��XP���ƽ��༭�� */
    LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
    LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR);

    /* ���־��� */
    if (_pEdit->Font->FontCode == FC_ST_12)
    {
        width = 6;
    }
    else
    {
        width = 8;
    }
    len = strlen(_pEdit->pCaption);
    x = _pEdit->Left +  (_pEdit->Width - len * width) / 2;
    y = _pEdit->Top + _pEdit->Height / 2 - width;

    LCD_DispStr(x, y, _pEdit->pCaption, _pEdit->Font);
#else
    if (g_ChipID == IC_8875)
    {
        uint16_t len, x;

        /* ��XP���ƽ��༭�� */
        RA8875_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
        RA8875_FillRect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR);

        RA8875_SetFont(_pEdit->Font.FontCode, 0, 0);    /* ����32�������壬�м��=0���ּ��=0 */
        RA8875_SetFrontColor(_pEdit->Font.FrontColor);
        RA8875_SetBackColor(EDIT_BACK_COLOR);

        /* ���־��� */
        len = strlen(_pEdit->Caption);
        x = (_pEdit->Width - len * 16) / 2;

        RA8875_DispStr(_pEdit->Left + x, _pEdit->Top + 3, _pEdit->Caption);
    }
    else
    {
        //SPFD5420_DrawCircle(_usX, _usY, _usRadius, _usColor);
    }
#endif
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawEdit
*   ����˵��: ��LCD�ϻ���һ���༭��
*   ��    ��:
*           _usX, _usY : ͼƬ������
*           _usHeight  : ͼƬ�߶�
*           _usWidth   : ͼƬ����
*           _ptr       : ͼƬ����ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawButton(BUTTON_T *_pBtn)
{
#if 1
    uint16_t len, fwidth, x, y;

    if (_pBtn->Focus == 1)
    {
        /* ��XP���ƽ��༭�� */
        LCD_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
        LCD_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
        LCD_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

        LCD_Fill_Rect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_ACTIVE_COLOR);   /* ѡ�к�ĵ�ɫ */
    }
    else
    {
        /* ��XP���ƽ��༭�� */
        LCD_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
        LCD_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
        LCD_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

        LCD_Fill_Rect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_BACK_COLOR);     /* δѡ�еĵ�ɫ */
    }

    /* ���־��� */
    len = strlen(_pBtn->pCaption);

    if (_pBtn->Font->FontCode == FC_ST_16)
    {
        fwidth = 8;     /* ������� */
    }
    else if (_pBtn->Font->FontCode == FC_ST_12)
    {
        fwidth = 6;     /* ������� */
    }
    else
    {
        fwidth = 8;     /* ������� */
    }
    x = _pBtn->Left + _pBtn->Width / 2 - (len * fwidth) / 2;
    y = _pBtn->Top + _pBtn->Height / 2 - fwidth;

    LCD_DispStr(x, y, _pBtn->pCaption, _pBtn->Font);
#else
    if (g_ChipID == IC_8875)
    {
        uint16_t len, x, y;

        if (_pBtn->Focus == 1)
        {
            /* ��XP���ƽ��༭�� */
            RA8875_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
            RA8875_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
            RA8875_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

            RA8875_FillRect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_ACTIVE_COLOR);

            RA8875_SetBackColor(BUTTON_ACTIVE_COLOR);
        }
        else
        {
            /* ��XP���ƽ��༭�� */
            RA8875_DrawRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, BUTTON_BORDER_COLOR);
            RA8875_DrawRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, BUTTON_BORDER1_COLOR);
            RA8875_DrawRect(_pBtn->Left + 2, _pBtn->Top + 2, _pBtn->Height - 4, _pBtn->Width - 4, BUTTON_BORDER2_COLOR);

            RA8875_FillRect(_pBtn->Left + 3, _pBtn->Top + 3, _pBtn->Height - 6, _pBtn->Width - 6, BUTTON_BACK_COLOR);

            RA8875_SetBackColor(BUTTON_BACK_COLOR);
        }

#if 1   /* ��ť�����������ɫ�̶� */
        if (strcmp(_pBtn->Caption, "��") == 0)  /* �˸�����⴦�� */
        {
            /* �˸�������ǵ����رʻ���̫ϸ�˲�Э����������⴦�� */
            RA8875_SetFont(RA_FONT_16, 0, 0);
            RA8875_SetFrontColor(CL_BLACK);
            RA8875_SetTextZoom(RA_SIZE_X2, RA_SIZE_X2); /* �Ŵ�2�� */
        }
        else
        {
            RA8875_SetFont(RA_FONT_16, 0, 0);
            RA8875_SetFrontColor(CL_BLACK);
            RA8875_SetTextZoom(RA_SIZE_X1, RA_SIZE_X1); /* �Ŵ�1�� */
        }
#else   /* ��ť�����������ɫ��Ӧ�ó���ָ�� */
        RA8875_SetFont(_pBtn->Font.FontCode, 0, 0);
        RA8875_SetFrontColor(_pBtn->Font.FrontColor);
#endif

        /* ���־��� */
        len = strlen(_pBtn->Caption);

        /* �˴�ͳ�Ʋ��ȿ��ַ������⡣��ʱ���⴦���� */
        if (len != 3)
        {
            x = _pBtn->Left + (_pBtn->Width - len * 16) / 2;
        }
        else
        {
            x = _pBtn->Left + (_pBtn->Width - len * 20) / 2;
        }

        /* �������ַ����⴦�� */
        if ((len == 1) && (_pBtn->Caption[0] == '.'))
        {
            y = _pBtn->Top + 3;
            x += 3;
        }
        else
        {
            y = _pBtn->Top + 3;
        }

        RA8875_DispStr(x, y, _pBtn->Caption);

        RA8875_SetTextZoom(RA_SIZE_X1, RA_SIZE_X1); /* ��ԭ�Ŵ�1�� */
    }
#endif
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawGroupBox
*   ����˵��: ��LCD�ϻ���һ�������
*   ��    ��: _pBox �����
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawGroupBox(GROUP_T *_pBox)
{
    uint16_t x, y;

    /* ����Ӱ�� */
    LCD_DrawRect(_pBox->Left + 1, _pBox->Top + 5, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER2);

    /* �������� */
    LCD_DrawRect(_pBox->Left, _pBox->Top + 4, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER1);

    /* ��ʾ�������⣨���������Ͻǣ� */
    x = _pBox->Left + 9;
    y = _pBox->Top;
    LCD_DispStr(x, y, _pBox->pCaption, _pBox->Font);
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DispControl
*   ����˵��: ���ƿؼ�
*   ��    ��: _pControl �ؼ�ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispControl(void *_pControl)
{
    uint8_t id;

    id = *(uint8_t *)_pControl; /* ��ȡID */

    switch (id)
    {
        case ID_ICON:
            //void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
            break;

        case ID_WIN:
            LCD_DrawWin((WIN_T *)_pControl);
            break;

        case ID_LABEL:
            LCD_DrawLabel((LABEL_T *)_pControl);
            break;

        case ID_BUTTON:
            LCD_DrawButton((BUTTON_T *)_pControl);
            break;

        case ID_CHECK:
            LCD_DrawCheckBox((CHECK_T *)_pControl);
            break;

        case ID_EDIT:
            LCD_DrawEdit((EDIT_T *)_pControl);
            break;

        case ID_GROUP:
            LCD_DrawGroupBox((GROUP_T *)_pControl);
            break;
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawHLine
*   ����˵��: ����һ��ˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*   ��    �Σ�_usX1    ����ʼ��X����
*             _usY1    ��ˮƽ�ߵ�Y����
*             _usX2    ��������X����
*             _usColor : ��ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLineH(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2, uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawHLine(_usX1, _usY1, _usX2, _usColor);
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawHLine(_usX1, _usY1, _usX2, _usColor);
    }
    else
    {
        SPFD5420_DrawHLine(_usX1, _usY1, _usX2, _usColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawHColorLine
*   ����˵��: ����һ����ɫˮƽ�� ����Ҫ����UCGUI�Ľӿں�����
*   ��    �Σ�_usX1    ����ʼ��X����
*             _usY1    ��ˮƽ�ߵ�Y����
*             _usWidth ��ֱ�ߵĿ���
*             _pColor : ��ɫ������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawHColorLine(_usX1, _usY1, _usWidth, _pColor);
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawHColorLine(_usX1, _usY1, _usWidth, _pColor);
    }
    else
    {
        SPFD5420_DrawHColorLine(_usX1, _usY1, _usWidth, _pColor);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_DrawLineV
*   ����˵��: ����һ����ֱ���� ����Ҫ����UCGUI�Ľӿں�����
*   ��    �Σ� _usX1    : ��ֱ�ߵ�X����
*             _usY1    : ��ʼ��Y����
*             _usY2    : ������Y����
*             _usColor : ��ɫ
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DrawLineV(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2, uint32_t _usColor)
{
    if (g_ChipID == IC_8875)
    {
        RA8875_DrawVLine(_usX1 , _usY1 , _usY2 , _usColor);
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_DrawVLine(_usX1 , _usY1 , _usY2 , _usColor);
    }
    else
    {
        SPFD5420_DrawVLine(_usX1 , _usY1 , _usY2, _usColor);
    }
}

/*
*********************************************************************************************************
*   ����˵��: �������
*           ����ͼ��ռ�Ϸ�1/2��չʾ��ռ�·�1/2
*   ��    �Σ�
*              x0    :����ͼ������X����
*              y0    : ����ͼ������Y����
*              x1    :����ͼ������X����
*              y1    : ����ͼ������Y����
*              colorArray* : ��ɫ����
*               colorSum:��ɫ����
*               index :����ָ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_Draw_Bar1(uint16_t x0 , uint16_t y0 , uint16_t x1 , uint16_t y1, uint16_t* colorArray,uint8_t colorSum , uint8_t index)
{
    uint16_t rectx0,rectx1;
    uint16_t recty0,recty1;
    uint16_t temp;
    uint8_t i;

    //��������
    rectx0 = x0;
    recty0 = y0+(y1-y0)/2;
    //ÿ��x�᷽�򳤶�
    if ( 0!= colorSum)
        temp = (x1-x0)/colorSum;
    else
        return;

    rectx1 = x0 + temp;
    recty1 = y1;

    for (i=0; i<colorSum; i++)
    {
        LCD_Fill_Rect(rectx0, recty0, recty1-recty0, rectx1-rectx0, colorArray[i]);
        rectx0=rectx1;
        rectx1+=temp;
    }
    //������������
    LCD_Fill_Rect(x0, y0, (y1-y0)/2, x1-x0, 0x0000);
    //�����ϵ�����
    rectx0=rectx1=temp =x0+ index*(x1-x0)/100;
    recty0=y0+(y1-y0)/2-1;

    for( i = 0 ; i < (y1-y0)/2 ; i++)
    {
        LCD_DrawLineH(rectx0, recty0, rectx1, 0xffff);
        rectx0   =  (uint16_t)(temp - (uint16_t)(0.577*i));
        if(rectx0<x0)
            rectx0   =  x0;
        rectx1   =  (uint16_t)(temp + (uint16_t)(0.577*i)+1);
        if(rectx1>x1)
            rectx1   =  x1;
        recty0--;
    }
}
/****************************************************************************
* ��    �ܣ������ݿ�
* ��    ��:
* ˵    ��: XSL������
****************************************************************************/
void LCD_DrawBlock(uint16_t x, uint16_t y, uint16_t m, uint16_t n, uint8_t *block, uint16_t front_color, uint16_t back_color)
{
    uint32_t i, j;
    uint16_t color_RG;
    uint16_t color_GB;
    uint8_t character;
    if (g_ChipID == IC_8875)
    {
        ;
    }
    else if(g_ChipID == IC_9341 \
            ||g_ChipID == IC_9488)
    {
        LCD_ILI9341_SetWindow(x, y, m, n);

        LCD_ILI9341_WR_REG(0x2c);
        for (i=0; i<m*n/8; i++)
        {
            character = block[i];

            for (j=0; j<8; j++)
            {

                if (character & 0x01)
                {
                    color_RG = front_color>>8;
                    color_GB = front_color;
                }
                else
                {
                    color_RG = back_color>>8;
                    color_GB = back_color;
                }

                LCD_ILI9341_WR_DATA(color_RG);
                LCD_ILI9341_WR_DATA(color_GB);
                character>>=1;
            }
        }
        // �˳�����
        LCD_ILI9341_QuitWinow();
    }
    else
    {
        ;
    }
}
/****************************************************************************
* ��    �ܣ���ͼ��
* ��    ��:
* ˵    ��: XSL������
****************************************************************************/
void LCD_DrawImage(uint16_t x, uint16_t y, uint8_t m, uint8_t n, const uint8_t *image)
{
    uint32_t i = 0;
    if (g_ChipID == IC_8875)
    {
        ;
    }
    else if(g_ChipID == IC_9341)
    {
        // ���ô���
        LCD_ILI9341_SetWindow(x, y, m, n);
        LCD_ILI9341_WR_REG(0x2c);
        for(; i<m*n; i++)
        {
            LCD_ILI9341_WR_DATA(*(image+2*i+1));
            LCD_ILI9341_WR_DATA(*(image+2*i));
        }
        // �˳�����
        LCD_ILI9341_QuitWinow();
    }
    else if(g_ChipID == IC_9488)
    {
        uint8_t color_buf[3];
        uint32_t i32;
        
        // ���ô���
        LCD_ILI9341_SetWindow(x, y, m, n);
        LCD_ILI9341_WR_REG(0x2c);
        
        for(; i<m*n; i++)
        {
            i32 = Count_4ByteToLong(0,0,*(image+2*i+1),*(image+2*i));
            Count_ColorConver(0,&i32,color_buf);
            LCD_ILI9341_WR_DATA(color_buf[0]);
            LCD_ILI9341_WR_DATA(color_buf[1]);
            LCD_ILI9341_WR_DATA(color_buf[2]);
        }
        // �˳�����
        LCD_ILI9341_QuitWinow();
    }
    else
    {
        ;
    }
}
/*
*********************************************************************************************************
*   �� �� ��: LCD_CtrlLinesConfig
*   ����˵��: ����LCD���ƿ��ߣ�FSMC�ܽ�����Ϊ���ù���
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
/*
    ������STM32-V5��������߷���:

    PD0/FSMC_D2
    PD1/FSMC_D3
    PD4/FSMC_NOE        --- �������źţ�OE = Output Enable �� N ��ʾ����Ч
    PD5/FSMC_NWE        --- д�����źţ�WE = Output Enable �� N ��ʾ����Ч
    PD8/FSMC_D13
    PD9/FSMC_D14
    PD10/FSMC_D15
    PD13/FSMC_A18       --- ��ַ RS
    PD14/FSMC_D0
    PD15/FSMC_D1

    PE4/FSMC_A20        --- ����Ƭѡһ������
    PE5/FSMC_A21        --- ����Ƭѡһ������
    PE7/FSMC_D4
    PE8/FSMC_D5
    PE9/FSMC_D6
    PE10/FSMC_D7
    PE11/FSMC_D8
    PE12/FSMC_D9
    PE13/FSMC_D10
    PE14/FSMC_D11
    PE15/FSMC_D12

    PG12/FSMC_NE4       --- ��Ƭѡ��TFT, OLED �� AD7606��

    PC3/TP_INT          --- ����оƬ�ж� ������RA8875������RA8875������ж�)  ������δʹ��Ӳ���ж�

    ---- ������ TFT LCD�ӿ������ź� ��FSMCģʽ��ʹ�ã�----
    PD3/LCD_BUSY        --- ����оƬæ       ��RA8875����RA8875оƬ��æ�ź�)
    PF6/LCD_PWM         --- LCD����PWM����  ��RA8875������˽ţ�������RA8875����)

    PI10/TP_NCS         --- ����оƬ��Ƭѡ      (RA8875������SPI�ӿڴ���оƬ��
    PB3/SPI3_SCK        --- ����оƬSPIʱ��     (RA8875������SPI�ӿڴ���оƬ��
    PB4/SPI3_MISO       --- ����оƬSPI������MISO(RA8875������SPI�ӿڴ���оƬ��
    PB5/SPI3_MOSI       --- ����оƬSPI������MOSI(RA8875������SPI�ӿڴ���оƬ��
*/
static void LCD_CtrlLinesConfig(void)
{
#if 0
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��FSMCʱ�� */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    /* ʹ�� GPIOʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG, ENABLE);

    /* ���� PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) Ϊ����������� */

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) Ϊ����������� */

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* ���� PD.13(A18 (RS))  Ϊ����������� */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* ���� PG12 (LCD/CS)) Ϊ����������� */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_FSMCConfig
*   ����˵��: ����FSMC���ڷ���ʱ��
*   ��    ��: ��
*   �� �� ֵ: ��
*********************************************************************************************************
*/
static void LCD_FSMCConfig(void)
{
#if 0
    FSMC_NORSRAMInitTypeDef  init;
    FSMC_NORSRAMTimingInitTypeDef  timingWrite;
    FSMC_NORSRAMTimingInitTypeDef  timingRead;

    /*-- FSMC Configuration ------------------------------------------------------*/
    /*----------------------- SRAM Bank 4 ----------------------------------------*/
    /* FSMC_Bank1_NORSRAM4 configuration */
    /* ����ͷDMA��㣬������ 4 0 5 2 0 0 */
    timingWrite.FSMC_AddressSetupTime = 4;
    timingWrite.FSMC_AddressHoldTime = 0;
    timingWrite.FSMC_DataSetupTime = 6;
    timingWrite.FSMC_BusTurnAroundDuration = 1;
    timingWrite.FSMC_CLKDivision = 0;
    timingWrite.FSMC_DataLatency = 0;
    timingWrite.FSMC_AccessMode = FSMC_AccessMode_A;

    timingRead.FSMC_AddressSetupTime = 4;
    timingRead.FSMC_AddressHoldTime = 0;
    timingRead.FSMC_DataSetupTime = 8;
    timingRead.FSMC_BusTurnAroundDuration = 1;
    timingRead.FSMC_CLKDivision = 0;
    timingRead.FSMC_DataLatency = 0;
    timingRead.FSMC_AccessMode = FSMC_AccessMode_A;


    /*
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable
    */
    init.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    init.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    init.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    init.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    init.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    init.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable; /* ע��ɿ��������Ա */
    init.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    init.FSMC_WrapMode = FSMC_WrapMode_Disable;
    init.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    init.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    init.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    init.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    init.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

    init.FSMC_ReadWriteTimingStruct = &timingRead;
    init.FSMC_WriteTimingStruct = &timingWrite;

    FSMC_NORSRAMInit(&init);

    /* - BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
#endif
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_SetBackLight
*   ����˵��: ��ʼ������LCD�������GPIO,����ΪPWMģʽ��
*           ���رձ���ʱ����CPU IO����Ϊ��������ģʽ���Ƽ�����Ϊ������������������͵�ƽ)����TIM3�ر� ʡ��
*   ��    ��: _bright ���ȣ�0����255������
*   �� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_SetBackLight(uint8_t _bright)
{
    s_ucBright =  _bright;  /* ���汳��ֵ */

    if (g_ChipID == IC_8875)
    {
        RA8875_SetBackLight(_bright);
    }
    else if (g_ChipID == IC_9341 || g_ChipID == IC_9488)
    {
        LCD_ILI9341_SetBackLight(_bright);
    }
    else
    {
        SPFD5420_SetBackLight(_bright);
    }
}

/*
*********************************************************************************************************
*   �� �� ��: LCD_GetBackLight
*   ����˵��: ��ñ������Ȳ���
*   ��    ��: ��
*   �� �� ֵ: �������Ȳ���
*********************************************************************************************************
*/
uint8_t LCD_GetBackLight(void)
{
    return s_ucBright;
}
/****************************************************************************
* ��    �ܣ��ж�оƬ����
****************************************************************************/
#include "uctsk_Debug.h"
static uint8_t DebugFlag=0;
uint8_t LCD_ErrReset(void)
{
    uint8_t buf[2];
    uint8_t buf1[5];
    uint8_t res=OK;
    if(DebugFlag==1)
    {
        DebugOutStr((int8_t*)"LCD_ErrReset_1\r\n");
    }
    if (g_ChipID == IC_9341)
    {
        if(DebugFlag==1)
        {
            DebugOutStr((int8_t*)"LCD_ErrReset_2\r\n");
        }
        LCD_ILI9341_WR_REG(0x0A);
        buf[0]=LCD_ILI9341_RD_DATA();
        buf[1]=LCD_ILI9341_RD_DATA();
        LCD_ILI9341_WR_REG(0x09);
        buf1[0]=LCD_ILI9341_RD_DATA();
        buf1[1]=LCD_ILI9341_RD_DATA();
        buf1[2]=LCD_ILI9341_RD_DATA();
        buf1[3]=LCD_ILI9341_RD_DATA();
        buf1[4]=LCD_ILI9341_RD_DATA();
        if(DebugFlag==1)
        {
            DebugOutHex("buf1",buf1,5);
        }
        //˯��ģʽ
        if(((buf[1]&0x10)==0)/*||((buf1[1]&0x50)!=0)*/)
        {
            if(DebugFlag==1)
            {
                DebugOutStr((int8_t*)"LCD_ErrReset_3\r\n");
            }
            if(LCD_ILI9341_SleepFlag==0)
            {
                if(DebugFlag==1)
                {
                    DebugOutStr((int8_t*)"LCD_ErrReset_4\r\n");
                }
                LCD_InitHard();
                res = ERR;
            }
        }
    }
    return res;
}
uint8_t LCD_ErrRead(void)
{
    uint8_t buf[2];
    uint8_t res=OK;
    if (g_ChipID == IC_9341)
    {
        LCD_ILI9341_WR_REG(0x0A);
        buf[0]=LCD_ILI9341_RD_DATA();
        buf[1]=LCD_ILI9341_RD_DATA();
        //˯��ģʽ
        if(((buf[1]&0x10)==0))
        {
            res = ERR;
        }
    }
    return res;
}
void LCD_ErrReset_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        DebugFlag=1;
    }
    else
    {
        DebugFlag=0;
    }
}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/