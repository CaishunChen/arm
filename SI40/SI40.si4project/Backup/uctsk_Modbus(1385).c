/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*************************************˵��*****************************************
һ,��������:
���յ�һ��: Uart_Rx2Buf    ---   ���ڽ�������(DMA�����ж�)(uart.c/stm32f10x_it.c)
            ����: ����У���ַ(���ֽ�)-->�����������ڶ���-->������Ϣ����
���յڶ���: Modbus_RBuf0   ---   ���滷
            ����: ��ȡ���ݵ�������
���յ�����: Modbus_RBuf1   ---   ��������
            ����: CRCУ��-->����-->����Modbus
��,��������:
���͵�һ��: MODBUS_TBuf0   ---   ֱ��DMA����
            ׼����������-->����CRC-->����-->DMA����
**********************************************************************************/
//------------------------------- Includes --------------------
#include <includes.h>
#ifdef __MODBUS_H
//-------------------------------------------------------------Private define
//------------------------------- �û����� --------------------
static  OS_STK  App_TaskModbusStk[APP_TASK_MODBUS_STK_SIZE];
//-----
typedef struct modbus_struct
{
    uint8_t   type;    //����: 0-������,2-GPRS,3-����
    uint8_t   para[11];//����: AT-����ָ������,GPRS-ͨ����(ASCII),����-�ֻ�����(ASCII)
    uint8_t   *buf;    //����:
    uint16_t  len;     //����:
} MODBUS_STRUCT;
//-----��Ϣ����-->��������
//MODBUS_STRUCT struct_gsmtx;//����GSM��ʼ��
#define MODBUS_SUBSECTION_QBUF_MAX  10
OS_EVENT *ModbusSubsectionQ;
void *ModbusSubsectionQBuf[MODBUS_SUBSECTION_QBUF_MAX];
//-----��
#define MODBUS_RBUF0_MAX   1024
//INT16U GsmSubTimeOut_10ms;           //ʱ�������
uint8_t  Modbus_RBuf0[MODBUS_RBUF0_MAX];
uint16_t ModbusRBufNewAddr=0;            //�����洢�µ�ַ
uint16_t ModbusRBufOldAddr=0;            //�����洢�ϵ�ַ
//-----������ȡ������
#define MODBUS_RBUF1_MAX   256
uint8_t  Modbus_RBuf1[MODBUS_RBUF1_MAX];
uint8_t  Modbus_RBuf1_OverBz;//0-���Խ��������ݣ�1-����δ��ɲ��ɽ���������
//-----���ݷ��ͻ�����
#define MODBUS_TBUF0_MAX    256
uint8_t  MODBUS_TBuf0[MODBUS_TBUF0_MAX];
//-----��ʱ������(����һ��ʱ�䲻����)
uint16_t  Modbus_NoTx_Overtime_ms=0;
//-----����
uint8_t Modbus_Debug_EN=0;
uint32_t ModbusTxCount=0;
uint32_t ModbusRxCount=0;
//
//static struct tm Modbus_tm;
//------------------------------- �û��������� ----------------
static void uctsk_Modbus (void);
//-------------------------------------------------------------���ݸ��Ƶ����滷��������Ϣ(ֻ���������ж�)
void Modbus_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len)
{
    //static INT8U res;
    static uint16_t i16;
    static MODBUS_STRUCT s_modbus_q;
    //�ж�Modbus��ַ��
    //res = TransferEnter_JudgeAddr(pRxIqBuf[0]);
    //if(res==0)return;
    //�жϳ���(��ַ+������+CRC16>=4B)
#ifdef MODBUS_USE_LEN
    if(len<5)return;
    if((len-5)!=pRxIqBuf[2])return;
#else
    if(len<4)return;
#endif
    //���ݸ��Ƶ����滷
    for(i16=0; i16<len; i16++)
    {
        Modbus_RBuf0[ModbusRBufNewAddr++]=pRxIqBuf[i16];
        if(ModbusRBufNewAddr>=MODBUS_RBUF0_MAX)
        {
            ModbusRBufNewAddr=0;
        }
    }
    //��ֵ��Ϣʵ��
    s_modbus_q.buf=&Modbus_RBuf0[ModbusRBufOldAddr];
    if(ModbusRBufNewAddr > ModbusRBufOldAddr)
    {
        s_modbus_q.len = ModbusRBufNewAddr - ModbusRBufOldAddr;
    }
    else
    {
        s_modbus_q.len = MODBUS_RBUF0_MAX - ModbusRBufOldAddr + ModbusRBufNewAddr;
    }
    //������Ϣ����
    OSQPost (ModbusSubsectionQ,&s_modbus_q);
    //��ַͬ��
    ModbusRBufOldAddr = ModbusRBufNewAddr;
}

//-------------------------------------------------------------Modbus���񴴽�
void  App_ModbusTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task Modbus",\
                          uctsk_Modbus,\
                          APP_TASK_MODBUS_PRIO,\
                          App_TaskModbusStk,\
                          APP_TASK_MODBUS_STK_SIZE,\
                          AppTaskModbusTCB,\
                          NULL);
}

//--------------------------------------------------------------Modbus����ʵ��
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
    //������Ϣ����
    //ModbusSubsectionQ = OSQCreate (&ModbusSubsectionQBuf[0], MODBUS_SUBSECTION_QBUF_MAX);
    //�ȴ�����������ʼ�����ڲ���
    //OSTimeDlyHMSM(0, 0, 1, 0);
    //��ʼ������Ӳ��
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
        //�ȴ���Ϣ
        prx = OSQPend (ModbusSubsectionQ,100,&err);
        Debug_TestPointAdd(DEBUG_TEST_POINT6_MODBUS);
        sprintf(Debug_CurrentThreadStrBuf,"Thread-Modbus");
        Modbus_NoTx_Overtime_ms=0;
        //��ȷ->����;����->��Ϣ���г�ʼ��
        if(err == OS_ERR_NONE)
        {
            //�����ݻ�����ȡ��Ч����-->Modbus_RBuf1
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
            //CRC16У��
            crc16=GenCRC16(Modbus_RBuf1,RxLen-2);
            if(crc16 != (Modbus_RBuf1[RxLen-1] << 8) + Modbus_RBuf1[RxLen-2])
            {
                continue;
            }
            //���ݽ���
            AES128_decrypt_Project(NULL,&Modbus_RBuf1[1],&Modbus_RBuf1[1],&RxLen);
            //
            //ModbusDebug(0,Modbus_RBuf1,&RxLen);
            //��������
            //TransferParse(Modbus_RBuf1,&RxLen,MODBUS_TBuf0,&TxLen);
            //
            if(TxLen>=(3+MODBUS_LEN_SIZE) && TxLen<=250 )
            {
                //���ݱ���
                AES128_encrypt_Project(NULL,&MODBUS_TBuf0[1],&MODBUS_TBuf0[1],&TxLen);
                //���CRC16
                crc16=GenCRC16(MODBUS_TBuf0,TxLen);
                MODBUS_TBuf0[TxLen++] = crc16;
                MODBUS_TBuf0[TxLen++] = crc16>>8;
                //����
                UART_DMA_Tx(MODBUS_UART,MODBUS_TBuf0,TxLen);
                //
                //ModbusDebug(1,MODBUS_TBuf0,&TxLen);
            }
            //���ݺ��ڴ���
            //TransferWriteParaPro();
        }
        else
        {
            //�����Ϣ����
            OSQFlush(ModbusSubsectionQ);
        }
        */
    }
}
#endif
