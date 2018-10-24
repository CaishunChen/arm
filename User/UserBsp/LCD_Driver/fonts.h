/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*										
*********************************************************************************************************
*	                                  
*	ģ������ : �ֿ�ģ��
*	�ļ����� : fonts.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __FONTS_H_
#define __FONTS_H_

extern unsigned char const g_Ascii64[];

extern unsigned char const g_Hz64[];

extern unsigned char const g_Ascii32[];

extern unsigned char const g_Hz32[];

extern unsigned char const g_Ascii24[];

extern unsigned char const g_Hz24[];

extern unsigned char const g_Ascii16[];

extern unsigned char const g_Hz16[];

extern unsigned char const g_Ascii12[];

extern unsigned char const g_Hz12[];

#define USE_SMALL_FONT	/* ������б�ʾʹ��С�ֿ⣬ �����ֻ��bsp_tft+lcd.c��ʹ�� */

#define HZK16_ADDR 0x803EA00		/* ���ֿ��ַ�� �����ַ��CPU�ڲ�Flash��λ�����������ĺ���һ���� */
#define HZK12_ADDR 0x803EA00		/* XSL - �����ַ�����ܾ���Ӧ��                                  */
#define HZK24_ADDR 0x803EA00		/* XSL - �����ַ�����ܾ���Ӧ��                                  */
#define HZK32_ADDR 0x803EA00		/* XSL - �����ַ�����ܾ���Ӧ��                                  */
#define HZK64_ADDR 0x803EA00		/* XSL - �����ַ�����ܾ���Ӧ��                                  */
#endif
