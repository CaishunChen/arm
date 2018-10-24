/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*********************************************************************************
˵��: PCͨѶ����
**********************************************************************************/
//-------------------���ؿ⺯��------------------------------
#include "uctsk_CommPC.h"
#include "uctsk_Debug.h"
#include "Module_Memory.h"
//-------------------���ݽṹ--------------------------------
enum COMMPC_E_TX_CMD
{
    COMMPC_E_TX_CMD_FILE_INFO=0,
    COMMPC_E_TX_CMD_FILE_DATA,
};
//-------------------��̬����--------------------------------
//����
MODULE_OS_TASK_TAB(App_TaskCommPcTCB);
MODULE_OS_TASK_STK(App_TaskCommPcStk,APP_TASK_COMMPC_STK_SIZE);
//�ź���
MODULE_OS_SEM(CommPc_Sem_Rx);
//����ʹ��
static uint8_t CommPc_Enable=0;
static uint8_t CommPc_Buf[256];
static uint16_t CommPc_BufLen;
//
static uint16_t CommPc_FileVer;
static uint32_t Commpc_FileAllPackNum;
static uint32_t Commpc_FileAllByteNum;
static uint32_t Commpc_FileSum,Commpc_FileSum1;
static uint32_t Commpc_FileCurrentPackNum;
//-------------------��������--------------------------------
static void CommPc_Init(void);
static void CommPc_TxCmd(uint8_t cmd,uint32_t para);
static void uctsk_CommPc (void);
/*********************************************************************************
��������: Ӳ����ʼ��
**********************************************************************************/
static void CommPc_Init(void)
{
    //DebugInit();
}
/*********************************************************************************
��������: ����
**********************************************************************************/
static void CommPc_Tx(uint8_t* pbuf, uint16_t len)
{
    DebugOut((int8_t*)pbuf,len);
}
/*********************************************************************************
��������: ����
ע    ��: ����Ŀǰ��Ŀ��USB����,���ݰ��ֶ��봮�ڷ�ʽ��ͬ,�����ڴ�����ֶ�
**********************************************************************************/
uint8_t uctsk_CommPc_Rx(uint8_t* pbuf, uint16_t len)
{
    //
    if(CommPc_Enable==0)
    {
        return ERR;
    }
    //������ȡ
    memcpy(&CommPc_Buf[CommPc_BufLen],pbuf,len);
    CommPc_BufLen += len;
    return OK;
}
void uctsk_CommPc_1ms(void)
{
    static uint16_t CommPc_BufLenBak=0;
    static uint16_t scount=0;
    if(CommPc_BufLen!=CommPc_BufLenBak || CommPc_BufLen==0)
    {
        scount=0;
        CommPc_BufLenBak=CommPc_BufLen;
        return;
    }
    else if(scount<0xFFFF)
    {
        scount++;
    }
    if(scount==3)
    {

        //������Ϣ
        MODULE_OS_SEM_POST(CommPc_Sem_Rx);
    }
}
static void CommPc_TxCmd(uint8_t cmd,uint32_t para)
{
    uint8_t* pbuf;
    uint8_t res;
    uint16_t crc;
    switch(cmd)
    {
        case COMMPC_E_TX_CMD_FILE_INFO:
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            res=0;
            pbuf[res++]   =  0x01;
            pbuf[res++]   =  0x03;
            pbuf[res++]   =  0x00;
            pbuf[res++]   =  0x10;
            pbuf[res++]   =  0x0A;
            crc=Count_CRC16(pbuf,res);
            pbuf[res++]=crc;
            crc=crc>>8;
            pbuf[res++]=crc;
            CommPc_Tx(pbuf,res);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            break;
        case COMMPC_E_TX_CMD_FILE_DATA:
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            res=0;
            pbuf[res++]   =  0x01;
            pbuf[res++]   =  0x61;
            pbuf[res++]   =  para;
            pbuf[res++]   =  para>>8;
            pbuf[res++]   =  para>>16;
            pbuf[res++]   =  para>>24;
            crc=Count_CRC16(pbuf,res);
            pbuf[res++]=crc;
            crc=crc>>8;
            pbuf[res++]=crc;
            CommPc_Tx(pbuf,res);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            break;
        default:
            break;
    }

}
/*******************************************************************************
��������: DebugӦ�ò��Ժ���
*******************************************************************************/
void CommPc_DebugTestOnOff(uint8_t OnOff)
{
    OnOff=OnOff;
    CommPc_Enable=1;
    CommPc_FileVer         =  0;
    Commpc_FileAllPackNum  =  0;
    Commpc_FileSum         =  0;
    Commpc_FileCurrentPackNum=0;
    while(1)
    {
        if(Commpc_FileCurrentPackNum>Commpc_FileAllPackNum)
        {
            //д���ļ���Ϣ
            break;
        }
        else
        {
            MODULE_OS_DELAY_MS(1000);
        }
    }
    CommPc_Enable=0;
}
void CommPc_Result_DebugTestOnOff(uint8_t OnOff)
{
    uint32_t i32=0;
    MODULE_MEMORY_S_FILEINFO *psFileInfo;
    psFileInfo = (MODULE_MEMORY_S_FILEINFO *)CommPc_Buf;
    Module_Memory_App(MODULE_MEMORY_APP_CMD_FILE_INFO_R,(uint8_t*)psFileInfo,(uint8_t*)&i32);
    Commpc_FileAllByteNum  =  psFileInfo->FileAllByteNum;
    Commpc_FileAllPackNum  =  (uint32_t)((Commpc_FileAllByteNum - 1) / 128 + 1);
    Commpc_FileSum         =  psFileInfo->FileAllSum;
    Commpc_FileSum1        =  0;
    CommPc_FileVer         =  psFileInfo->FileVer;
    for(i32=1; i32<=Commpc_FileAllPackNum; i32++)
    {
        //
        Module_Memory_App(MODULE_MEMORY_APP_CMD_FILE_DATA_R,CommPc_Buf,(uint8_t*)&i32);
        if(i32!=Commpc_FileAllPackNum)
        {
            Commpc_FileSum1=Count_SumMax(Commpc_FileSum1,CommPc_Buf,128);
        }
        else
        {
            Commpc_FileSum1=Count_SumMax(Commpc_FileSum1,CommPc_Buf,Commpc_FileAllByteNum%128);
        }
    }
    //
    if(Commpc_FileSum1==Commpc_FileSum)
    {      
        DebugOutStr((int8_t*)"ExtFlash Have Data:\r\n");
        sprintf((char*)CommPc_Buf,"  File All Byte:%ld\r\n",Commpc_FileAllByteNum);
        DebugOutStr((int8_t*)CommPc_Buf);
        sprintf((char*)CommPc_Buf,"  File All Pack:%ld\r\n",Commpc_FileAllPackNum);
        DebugOutStr((int8_t*)CommPc_Buf);
        sprintf((char*)CommPc_Buf,"  File Sum     :%ld\r\n",Commpc_FileSum);
        DebugOutStr((int8_t*)CommPc_Buf);
        sprintf((char*)CommPc_Buf,"  File Ver     :%ld\r\n",CommPc_FileVer);
        DebugOutStr((int8_t*)CommPc_Buf);
    }
    else
    {
        DebugOutStr((int8_t*)"ExtFlash have not data!\r\n");
    }
    Commpc_FileAllPackNum = 0;
    Commpc_FileAllByteNum = 0;
    Commpc_FileSum        = 0;
    Commpc_FileSum1       = 0;
}
/*********************************************************************************
˵��: ���񴴽�
**********************************************************************************/
void  App_CommPcTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task CommPc",\
                          uctsk_CommPc,\
                          APP_TASK_COMMPC_PRIO,\
                          App_TaskCommPcStk,\
                          APP_TASK_COMMPC_STK_SIZE,\
                          App_TaskCommPcTCB,\
                          NULL);
}

//--------------------------------------------------------------Modbus����ʵ��
static void uctsk_CommPc (void)
{
    //���ݳ�ʼ��
    //��ʼ���ź���
    MODULE_OS_ERR(err);
    MODULE_OS_SEM_CREATE(CommPc_Sem_Rx,"Bluetooth_Sem_Rx",0);
    //Ӳ����ʼ��
    CommPc_Init();
    //
    for(;;)
    {
        //�ȴ��ź�
        MODULE_OS_SEM_PEND(CommPc_Sem_Rx,500,TRUE,err);
        //-----------------------
        if(err==OS_ERR_NONE)
        {
            uint8_t res;
            //���е�ַ,����,CRC16��У��
            res=Count_Modbus_Check(CommPc_Buf,CommPc_BufLen,1);
            if(res==OK)
            {
                //��������
                switch(CommPc_Buf[1])
                {
                    case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
                        //��ȡ�汾
                        CommPc_FileVer=Count_2ByteToWord(CommPc_Buf[4],CommPc_Buf[3]);
                        //���ֽ���
                        Commpc_FileAllByteNum=Count_4ByteToLong(CommPc_Buf[8],CommPc_Buf[7],CommPc_Buf[6],CommPc_Buf[5]);
                        //��ȡ�ܰ���
                        Commpc_FileAllPackNum=(uint32_t)((Commpc_FileAllByteNum - 1) / 128 + 1);
                        //��ȡУ���
                        Commpc_FileSum=Count_4ByteToLong(CommPc_Buf[12],CommPc_Buf[11],CommPc_Buf[10],CommPc_Buf[9]);
                        //��ʽ��FLASH
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_FORMAT,NULL ,NULL);
                        //Ҫ��һ������
                        Commpc_FileCurrentPackNum = 1;
                        {
                            CommPc_TxCmd(COMMPC_E_TX_CMD_FILE_DATA,Commpc_FileCurrentPackNum);
                        }
                        break;
                    case COUNT_MODBUS_FC_LOADFILE:
                        //�洢����
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_FILE_DATA_W,&CommPc_Buf[6],(uint8_t*)&Commpc_FileCurrentPackNum);
                        //Ҫ��һ������
                        if(Commpc_FileCurrentPackNum<Commpc_FileAllPackNum)
                        {
                            Commpc_FileCurrentPackNum++;
                            CommPc_TxCmd(COMMPC_E_TX_CMD_FILE_DATA,Commpc_FileCurrentPackNum);
                        }
                        else
                        {
                            //У��
                            {
                                uint32_t i32;
                                uint32_t sum=0;
                                uint8_t *pbuf;
                                pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                                for(i32=1; i32<=Commpc_FileAllPackNum; i32++)
                                {
                                    //
                                    Module_Memory_App(MODULE_MEMORY_APP_CMD_FILE_DATA_R,pbuf,(uint8_t*)&i32);
                                    if(i32!=Commpc_FileAllPackNum)
                                    {
                                        sum=Count_SumMax(sum,pbuf,128);
                                    }
                                    else
                                    {
                                        sum=Count_SumMax(sum,pbuf,Commpc_FileAllByteNum%128);
                                    }
                                }
                                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                                if(sum==Commpc_FileSum)
                                {
                                    res=OK;
                                }
                                else
                                {
                                    res=ERR;
                                }
                            }
                            if(res==OK)
                            {
                                //
                                Commpc_FileCurrentPackNum=Commpc_FileAllPackNum+1;
                                //�洢��Ϣ
                                {
                                    MODULE_MEMORY_S_FILEINFO *psFileInfo=(MODULE_MEMORY_S_FILEINFO*)CommPc_Buf;
                                    psFileInfo->FileAllByteNum   =  Commpc_FileAllByteNum;
                                    psFileInfo->FileAllSum       =  Commpc_FileSum;
                                    psFileInfo->FileVer          =  CommPc_FileVer;
                                    Module_Memory_App(MODULE_MEMORY_APP_CMD_FILE_INFO_W,(uint8_t*)psFileInfo,NULL);
                                }
                            }
                            else
                            {
                                Commpc_FileCurrentPackNum=Commpc_FileAllPackNum+2;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            CommPc_BufLen=0;
            uctsk_CommPc_1ms();
        }
        else if(err==OS_ERR_TIMEOUT)
        {
            if(CommPc_Enable==1)
            {
                if(CommPc_FileVer               == 0
                   &&Commpc_FileAllPackNum      == 0
                   && Commpc_FileSum            == 0
                   && Commpc_FileCurrentPackNum == 0)
                {
                    //��ȡ�ļ���Ϣ
                    CommPc_TxCmd(COMMPC_E_TX_CMD_FILE_INFO,NULL);
                }
                else if(Commpc_FileCurrentPackNum>=1 && Commpc_FileCurrentPackNum<=Commpc_FileAllPackNum)
                {
                    //��ȡ�ļ�����
                    CommPc_TxCmd(COMMPC_E_TX_CMD_FILE_DATA,Commpc_FileCurrentPackNum);
                }
            }
        }
        else
        {
            ;
        }
    }
}
//-----------------------------------------------------------

