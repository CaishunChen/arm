//-------------------------------------------------------------------------------//
//                           丹东华通测控有限公司                                //
//                                版权所有                                       //
//                    企业网址：http://www.htong.com                             //
//                    开发环境: MPLAB X IDE V1.95                                //
//                    编 译 器: XC16  V1.20                                      //
//                    芯片型号: dsPIC33EP256GM710                                //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: RTC.h                                            //
//                    作    者: 徐松亮                                           //
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
#ifndef __RTC_H
#define __RTC_H


extern INT8U RTC_SEC_FLAG;             //1秒中断标志

extern INT8U ReadRealTimeflag;         //计取实时时钟标志
extern INT8U SoftTime[7];              //软件时间
extern INT8U RTCSelfCheckCnt;          //RTC有效计数
extern INT8U RTCSelfCheckErrCnt;       //RTC故障计数

extern void SoftTimeCount(void);       //软件时钟
extern void RTCPowerONSelfCheck(void); //RTC上电自检
extern void RTC_SelfCheck(void);       //硬件时钟检测
extern void AutoSetSoftTime(void);     //软件自动较时
extern void RTC_CLKOInit(void);        //1秒中断信号

extern INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm); //读取实时时间
extern INT8U SetRealTime(INT8U *time); //设置实时时间

extern INT8U TimeTypeConvert(INT8U mode,struct tm* tm_time,INT8U* time);

#endif


