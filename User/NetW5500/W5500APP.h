//-------------------------------------------------------------------------------//
//                    ��������: RealView MDK-ARM Version 5.10                    //
//                    �� �� ��: RealView MDK-ARM Version 5.10                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: W5500.h                                          //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-08-06    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
#ifndef __W5500APP_H
#define __W5500APP_H
//-------------------------------------------------------------------------------���ļ�
#include <W5500.h>

extern void W5500APP_100ms(void);
extern void W5500APP_ProtocolAnalysis_Ch0(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch1(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch2(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch3(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch4(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch5(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch6(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_ProtocolAnalysis_Ch7(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port);
extern void W5500APP_DebugSetNetPara(void);

#endif
