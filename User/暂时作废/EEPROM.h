//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: MPLAB X IDE V1.95                                //
//                    编 译 器: XC16  V1.20                                      //
//                    芯片型号: dsPIC33EP256GM710                                //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: EEPORRM.h                                        //
//                    作    者: 李中学                                           //
//                    时    间: 2013-10-21    版本:  0.1                         //
//-------------------------------------------------------------------------------//
//
// 文件描述：
//
//
// 注意事项：
//
//
// 修改记录：
//
//
// 修改时间:
//
//
//-------------------------------------------------------------------------------
#ifndef __EEPROM_H
#define __EEPROM_H

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define START_EEPROM_ADDR      0x0000              //起始地址
#define MAX_EEPROM_ADDR        0x7FFF              //最大32K节数

#define MAX_EEPROM_BUFFER      128                 //EEPROM最大缓冲字节数

#define EE_Block0      0x0000                      //块区0  512 byte
#define EE_Block1      0x0200                      //块区1  512 byte
#define EE_Block2      0x0400                      //块区2  512 byte


extern INT8U I2C_RD_Buffer[MAX_EEPROM_BUFFER];     //I2C总线读缓冲
extern INT8U I2C_WR_Buffer[MAX_EEPROM_BUFFER];     //I2C总线写缓冲


extern void I2C_Init(void);                        //I2C 初始配置

extern INT8U PCF8563_ReadTime(struct tm *tm_t);    //读时钟

extern INT8U PCF8563_SetTime(struct tm tm_t);      //写时钟

extern INT8U RTC_Read_1Byte(INT8U Addr);              //EEPROM 读1 Byte
extern void RTC_Write_1Byte(INT8U Addr,INT8U EEdata); //EEPROM 写1 Byte

extern INT8U EE_Read_1Byte(INT16U Addr);                //EEPROM 读1 Byte
extern void EE_Write_1Byte(INT16U Addr,INT8U EEdata);   //EEPROM 写1 Byte

extern void EE_Read_nByte(INT16U Addr,INT8U *dptr,INT16U len); //EEPROM 读 n Byte
extern void EE_Write_nByte(INT16U Addr,INT8U *dptr,INT16U len);//EEPROM 写 n Byte




#endif


