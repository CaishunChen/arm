//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: MPLAB X IDE V1.95                                //
//                    �� �� ��: XC16  V1.20                                      //
//                    оƬ�ͺ�: dsPIC33EP256GM710                                //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: RTC.h                                            //
//                    ��    ��: ������                                           //
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
#ifndef __RTC_H
#define __RTC_H


extern INT8U RTC_SEC_FLAG;             //1���жϱ�־

extern INT8U ReadRealTimeflag;         //��ȡʵʱʱ�ӱ�־
extern INT8U SoftTime[7];              //���ʱ��
extern INT8U RTCSelfCheckCnt;          //RTC��Ч����
extern INT8U RTCSelfCheckErrCnt;       //RTC���ϼ���

extern void SoftTimeCount(void);       //���ʱ��
extern void RTCPowerONSelfCheck(void); //RTC�ϵ��Լ�
extern void RTC_SelfCheck(void);       //Ӳ��ʱ�Ӽ��
extern void AutoSetSoftTime(void);     //����Զ���ʱ
extern void RTC_CLKOInit(void);        //1���ж��ź�

extern INT8U ReadRealTime(INT8U *time,struct tm *CurrentTime_tm); //��ȡʵʱʱ��
extern INT8U SetRealTime(INT8U *time); //����ʵʱʱ��

extern INT8U TimeTypeConvert(INT8U mode,struct tm* tm_time,INT8U* time);

#endif


