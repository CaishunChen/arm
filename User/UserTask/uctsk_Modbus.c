/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*************************************说明*****************************************
一,接收理念:
接收第一层: Uart_Rx2Buf    ---   串口接收数据(DMA接收中断)(uart.c/stm32f10x_it.c)
            操作: 解析校验地址(首字节)-->复制数据至第二层-->发送消息队列
接收第二层: Modbus_RBuf0   ---   缓存环
            操作: 提取数据到第三层
接收第三层: Modbus_RBuf1   ---   待处理缓存
            操作: CRC校验-->解密-->解析Modbus
二,发送理念:
发送第一层: MODBUS_TBuf0   ---   直接DMA发送
            准备发送数据-->计算CRC-->加密-->DMA发送
**********************************************************************************/
//------------------------------- Includes --------------------
#include <includes.h>
#ifdef __MODBUS_H
//-------------------------------------------------------------Private define
//------------------------------- 用户变量 --------------------
static  OS_STK  App_TaskModbusStk[APP_TASK_MODBUS_STK_SIZE];
//-----
typedef struct modbus_struct
{
    uint8_t   type;    //类型: 0-无意义,2-GPRS,3-短信
    uint8_t   para[11];//参数: AT-发送指令类型,GPRS-通道号(ASCII),短信-手机号码(ASCII)
    uint8_t   *buf;    //数据:
    uint16_t  len;     //长度:
} MODBUS_STRUCT;
//-----消息队列-->串口数据
//MODBUS_STRUCT struct_gsmtx;//用于GSM初始化
#define MODBUS_SUBSECTION_QBUF_MAX  10
OS_EVENT *ModbusSubsectionQ;
void *ModbusSubsectionQBuf[MODBUS_SUBSECTION_QBUF_MAX];
//-----环
#define MODBUS_RBUF0_MAX   1024
//INT16U GsmSubTimeOut_10ms;           //时间计数器
uint8_t  Modbus_RBuf0[MODBUS_RBUF0_MAX];
uint16_t ModbusRBufNewAddr=0;            //环儿存储新地址
uint16_t ModbusRBufOldAddr=0;            //环儿存储老地址
//-----数据提取缓存区
#define MODBUS_RBUF1_MAX   256
uint8_t  Modbus_RBuf1[MODBUS_RBUF1_MAX];
uint8_t  Modbus_RBuf1_OverBz;//0-可以接收新数据，1-处理未完成不可接收新数据
//-----数据发送缓存区
#define MODBUS_TBUF0_MAX    256
uint8_t  MODBUS_TBuf0[MODBUS_TBUF0_MAX];
//-----超时计数器(超过一定时间不回数)
uint16_t  Modbus_NoTx_Overtime_ms=0;
//-----测试
uint8_t Modbus_Debug_EN=0;
uint32_t ModbusTxCount=0;
uint32_t ModbusRxCount=0;
//
//static struct tm Modbus_tm;
//------------------------------- 用户函数声明 ----------------
static void uctsk_Modbus (void);
//-------------------------------------------------------------数据复制到缓存环并发送消息(只被调用于中断)
void Modbus_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len)
{
    //static INT8U res;
    static uint16_t i16;
    static MODBUS_STRUCT s_modbus_q;
    //判断Modbus地址码
    //res = TransferEnter_JudgeAddr(pRxIqBuf[0]);
    //if(res==0)return;
    //判断长度(地址+功能码+CRC16>=4B)
#ifdef MODBUS_USE_LEN
    if(len<5)return;
    if((len-5)!=pRxIqBuf[2])return;
#else
    if(len<4)return;
#endif
    //数据复制到缓存环
    for(i16=0; i16<len; i16++)
    {
        Modbus_RBuf0[ModbusRBufNewAddr++]=pRxIqBuf[i16];
        if(ModbusRBufNewAddr>=MODBUS_RBUF0_MAX)
        {
            ModbusRBufNewAddr=0;
        }
    }
    //赋值消息实体
    s_modbus_q.buf=&Modbus_RBuf0[ModbusRBufOldAddr];
    if(ModbusRBufNewAddr > ModbusRBufOldAddr)
    {
        s_modbus_q.len = ModbusRBufNewAddr - ModbusRBufOldAddr;
    }
    else
    {
        s_modbus_q.len = MODBUS_RBUF0_MAX - ModbusRBufOldAddr + ModbusRBufNewAddr;
    }
    //发送消息队列
    OSQPost (ModbusSubsectionQ,&s_modbus_q);
    //地址同步
    ModbusRBufOldAddr = ModbusRBufNewAddr;
}

//-------------------------------------------------------------Modbus任务创建
void  App_ModbusTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Modbus",\
                          uctsk_Modbus,\
                          APP_TASK_MODBUS_PRIO,\
                          App_TaskModbusStk,\
                          APP_TASK_MODBUS_STK_SIZE,\
                          AppTaskModbusTCB,\
                          NULL);
}

//--------------------------------------------------------------Modbus任务实体
static void uctsk_Modbus (void)
{
    //MODBUS_STRUCT *prx;
    //INT8U err;
    //INT16U m;
    //
    //INT8U RxLen;
    //INT8U TxLen;
    //
    //INT16U crc16;
    //创建消息队列
    //ModbusSubsectionQ = OSQCreate (&ModbusSubsectionQBuf[0], MODBUS_SUBSECTION_QBUF_MAX);
    //等待其他变量初始化后在操作
    //OSTimeDlyHMSM(0, 0, 1, 0);
    //初始化串口硬件
    //
    //INT8U i;
    //INT16U i16;
    //INT8U buf[10];
    //for(i=0; i<10; i++)
    //{
    //    buf[i]=i+1;
    //}
    for(;;)
    {
        OSTimeDlyHMSM(0, 0, 1, 0);
        /*
        BufferLoopInit();
        for(i=0; i<BUFFERLOOP_NODE_NUM; i++)
        {
            BufferLoopPush(0,buf,sizeof(buf));
        }
        for(i=0; i<BUFFERLOOP_NODE_NUM; i++)
        {
            memset((char*)buf,0,sizeof(buf));
            BufferLoopPop(0,buf,&i16);
        }
        for(i=0; i<BUFFERLOOP_NODE_NUM; i++)
        {
            BufferLoopPush(0,buf,sizeof(buf));
        }
        for(i=0; i<BUFFERLOOP_NODE_NUM; i++)
        {
            memset((char*)buf,0,sizeof(buf));
            BufferLoopPop(0,buf,&i16);
        }
        */
        /*
        //Transfer_100mS();
        //等待消息
        prx = OSQPend (ModbusSubsectionQ,100,&err);
        Debug_TestPointAdd(DEBUG_TEST_POINT6_MODBUS);
        sprintf(Debug_CurrentThreadStrBuf,"Thread-Modbus");
        Modbus_NoTx_Overtime_ms=0;
        //正确->处理;错误->消息队列初始化
        if(err == OS_ERR_NONE)
        {
            //从数据环中提取有效数据-->Modbus_RBuf1
            RxLen = (*prx).len;
            memset(Modbus_RBuf1,0x00,MODBUS_RBUF1_MAX);
            for(m=0; m<RxLen; m++)
            {
                Modbus_RBuf1[m]= *(*prx).buf;
                if((*prx).buf==&Modbus_RBuf0[MODBUS_RBUF0_MAX-1])
                {
                    (*prx).buf = &Modbus_RBuf0[0];
                }
                else
                {
                    (*prx).buf++;
                }
                if(m==MODBUS_RBUF1_MAX-1)
                {
                    Modbus_RBuf1[MODBUS_RBUF1_MAX-1]=0x00;
                    break;
                }
            }
            //CRC16校验
            crc16=GenCRC16(Modbus_RBuf1,RxLen-2);
            if(crc16 != (Modbus_RBuf1[RxLen-1] << 8) + Modbus_RBuf1[RxLen-2])
            {
                continue;
            }
            //数据解码
            AES128_decrypt_Project(NULL,&Modbus_RBuf1[1],&Modbus_RBuf1[1],&RxLen);
            //
            //ModbusDebug(0,Modbus_RBuf1,&RxLen);
            //处理数据
            //TransferParse(Modbus_RBuf1,&RxLen,MODBUS_TBuf0,&TxLen);
            //
            if(TxLen>=(3+MODBUS_LEN_SIZE) && TxLen<=250 )
            {
                //数据编码
                AES128_encrypt_Project(NULL,&MODBUS_TBuf0[1],&MODBUS_TBuf0[1],&TxLen);
                //添加CRC16
                crc16=GenCRC16(MODBUS_TBuf0,TxLen);
                MODBUS_TBuf0[TxLen++] = crc16;
                MODBUS_TBuf0[TxLen++] = crc16>>8;
                //发送
                UART_DMA_Tx(MODBUS_UART,MODBUS_TBuf0,TxLen);
                //
                //ModbusDebug(1,MODBUS_TBuf0,&TxLen);
            }
            //数据后期处理
            //TransferWriteParaPro();
        }
        else
        {
            //清空消息队列
            OSQFlush(ModbusSubsectionQ);
        }
        */
    }
}
#endif
