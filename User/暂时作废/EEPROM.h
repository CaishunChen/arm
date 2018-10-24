//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: MPLAB X IDE V1.95                                //
//                    �� �� ��: XC16  V1.20                                      //
//                    оƬ�ͺ�: dsPIC33EP256GM710                                //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: EEPORRM.h                                        //
//                    ��    ��: ����ѧ                                           //
//                    ʱ    ��: 2013-10-21    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
//
// �ļ�������
//
//
// ע�����
//
//
// �޸ļ�¼��
//
//
// �޸�ʱ��:
//
//
//-------------------------------------------------------------------------------
#ifndef __EEPROM_H
#define __EEPROM_H

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define START_EEPROM_ADDR      0x0000              //��ʼ��ַ
#define MAX_EEPROM_ADDR        0x7FFF              //���32K����

#define MAX_EEPROM_BUFFER      128                 //EEPROM��󻺳��ֽ���

#define EE_Block0      0x0000                      //����0  512 byte
#define EE_Block1      0x0200                      //����1  512 byte
#define EE_Block2      0x0400                      //����2  512 byte


extern INT8U I2C_RD_Buffer[MAX_EEPROM_BUFFER];     //I2C���߶�����
extern INT8U I2C_WR_Buffer[MAX_EEPROM_BUFFER];     //I2C����д����


extern void I2C_Init(void);                        //I2C ��ʼ����

extern INT8U PCF8563_ReadTime(struct tm *tm_t);    //��ʱ��

extern INT8U PCF8563_SetTime(struct tm tm_t);      //дʱ��

extern INT8U RTC_Read_1Byte(INT8U Addr);              //EEPROM ��1 Byte
extern void RTC_Write_1Byte(INT8U Addr,INT8U EEdata); //EEPROM д1 Byte

extern INT8U EE_Read_1Byte(INT16U Addr);                //EEPROM ��1 Byte
extern void EE_Write_1Byte(INT16U Addr,INT8U EEdata);   //EEPROM д1 Byte

extern void EE_Read_nByte(INT16U Addr,INT8U *dptr,INT16U len); //EEPROM �� n Byte
extern void EE_Write_nByte(INT16U Addr,INT8U *dptr,INT16U len);//EEPROM д n Byte




#endif


