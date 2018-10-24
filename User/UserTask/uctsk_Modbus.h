//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 4.14                    //
//                    编 译 器: RealView MDK-ARM Version 4.14                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: uctsk_Modbus.h                                   //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-01-17    版本:  0.1                         //
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------
#ifndef __MODBUS_H
#define __MODBUS_H
//-------------------------------------------------------------------------------接口宏定义
//串口端口号,在uart.c有宏判断,目前只支持1或2或3
#define MODBUS_UART  3
//-------------------------------------------------------------------------------接口常量
//-------------------------------------------------------------------------------接口变量
//放在1ms中断中执行++
extern uint16_t Modbus_NoTx_Overtime_ms;
//调试-->调试输出使能
extern uint8_t Modbus_Debug_EN;
//调试-->有效收发计数值
extern uint32_t ModbusTxCount;
extern uint32_t ModbusRxCount;
//作为主机时,读取的设备数据
#define MODBUS_MAX_DEVICE  4
#define MODBUS_MAX_REGNUM  1000
extern uint16_t Modbus_S_Regbuf[MODBUS_MAX_DEVICE][MODBUS_MAX_REGNUM];
//-------------------------------------------------------------------------------接口函数
//extern void ModbusDebug(INT8U RxOrTx,INT8U *pBuf,INT8U* pLen);
extern void Modbus_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len);
#endif

//---------------------END-------------------------------------------------------

