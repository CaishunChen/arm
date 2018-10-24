//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 5.10                    //
//                    编 译 器: RealView MDK-ARM Version 5.10                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: W5500.h                                          //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-08-06    版本:  0.1                         //
//-------------------------------------------------------------------------------//
#ifndef __W5500APP_H
#define __W5500APP_H
//-------------------------------------------------------------------------------库文件
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
