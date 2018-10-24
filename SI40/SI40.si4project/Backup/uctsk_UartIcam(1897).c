/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
/*********************************************************************************
˵��:
   1��ModbusЭ����
               ��ַ  ������   ����  CRC16
   2������Э��
               ��ַ  ������   ����                                            CRC16
      ��(��)   01    03       �Ĵ�����ʼ��ַ2B  �Ĵ�������2B                  XXXX
      ��(��)   01    03       �ֽ���1B          ����nB
      ����     01    83       01(�Ƿ�������)/02(�Ƿ���ַ)/
                              03(�Ƿ���ֵ)/04(���豸����)                     XXXX
      д(��)   01    10       �Ĵ�����ʼ��ַ2B  �Ĵ�������2B   �ֽ���1B ֵnB  XXXX
      д(��)   01    10       �Ĵ�����ʼ��ַ2B  �Ĵ�������2B                  XXXX
      ����     01    90       01(�Ƿ�������)/02(�Ƿ���ַ)/
                              03(�Ƿ���ֵ)/04(���豸����)                     XXXX
      ��˯��(��)01   65       ʱ��3B(������)                                  XXXX
      ���嶯(��)01   65       480B                                            XXXX
      ���嶯(��)01   66       ʱ��3B(������)    ����1B(0-143 10����1��)       XXXX
      ���嶯(��)01   66       ʱ��3B   ����1B   240B                          XXXX
      ������(��)01   67       ʱ��3B(������)                                  XXXX
      ������(��)01   67       �ֽ���1B ��ʼ2B(ʱ��)����2B(ʱ��)����(nB)       XXXX
      ��ʽ��(��)01   6F       ģʽ1B(01-ȫ��ʼ�� 02-���ʼ��)                 XXXX
      ��ʽ��(��)01   6F       ģʽ1B(01-ȫ��ʼ�� 02-���ʼ��)                 XXXX
      ����     01    EF       ģʽ1B(01-ȫ��ʼ�� 02-���ʼ��)                 XXXX
   3����ַӳ���
      ״̬     0000  000F
               0000  ��ֵ��ʱMH  �嶯��ֵH
               0001  ��ֵ��ʱL   �嶯��ֵML
               0008  �嶯��ֵMH  �嶯��ֵH
               0009  �嶯��ֵL   �嶯��ֵML
      ʱ��     0010  001F
               0010  ��    ��
               0011  ʱ    ��
               0012  ��    ��
      ˯������ 0020  002F
               0020  ��    ״̬(0-��Ч 1-��������  2-��������)
               0021  ��    ��
               0022  ��    ״̬
               0023  ��    ��
               0024  ��    ״̬
               0025  ��    ��
               0026  ��    ״̬
               0027  ��    ��
               0028  ��    ״̬
               0029  ��    ��
               002A  ��    ״̬
               002B  ��    ��
               002C  ��    ״̬
               002D  ��    ��
      �嶯���� 0030  003F
               0030  ��    ״̬(0)
               0031  ��    ��
               0032  ��    ״̬(0)
               0033  ��    ��
               0034  ��    ״̬(0)
               0035  ��    ��
               0036  ��    ״̬(0)
               0037  ��    ��
      �豸��ַ 0100  �豸��ַL   NULL
   4��ʵ��
      ��ȡ״̬:      01 03 00 00 10 00 48 0A
      Ӧ��:          01 03 20 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F F8 B9

      ��ȡʱ��:      01 03 10 00 03 00 41 FA
      Ӧ��:          01 03 06 11 02 09 0B 02 33 68 DF

      ����ʱ��:      01 10 10 00 03 00 06 11 02 06 10 1E 02 15 08
      Ӧ��:          01 10 10 00 03 00 C4 39

      ��ȡ˯������:  01 03 20 00 0E 00 4A 6A
      Ӧ��:          01 03 1C 01 11 02 07 01 11 02 07 01 11 02 08 01 11 02 08 01 11 01 18 01 11 02 06 01 11 02 06 AF BE

      ��ȡ�嶯����:  01 03 30 00 08 00 4D 0A
      Ӧ��:          01 03 10 00 11 02 01 00 11 02 02 00 11 02 03 00 11 02 04 C8 C5

      ��ȡ˯��:      01 65 11 01 17 16 97
      Ӧ��:          01 65 480B  XXXX
      ����:          01 E5 02 EB 51

      ��ȡ�嶯:      01 66 11 02 01 00 AC AE
      Ӧ��:          01 66 11 02 01 00 240B XXXX

      ��ȡ˯�߷���:  01 67 11 02 0D 96 14
      Ӧ��:          01 67 E0 15 00 08 00 nB XXXX
      
      ��ʽ������:    01 6F 01 CC 30
      ��ʽ��Ӧ��:    01 6F 01 CC 30

      �޸��豸��ַ:  FF 06 00 01 02 00 CC B4
      ��Ӧ:          02 06 00 01 02 00 D9 59 
**********************************************************************************/
//-------------------���ؿ⺯��------------------------------
#include "uctsk_UartIcam.h"
#include "uctsk_Debug.h"
#include "uctsk_RFMS.h"
#include "Module_Memory.h"
#include "Bsp_Uart.h"
#include "Bsp_Rtc.h"
#include "Bsp_CpuFlash.h"
#if   (HARDWARE_VER==7)
//-------------------���ݽṹ--------------------------------
enum UCTSK_UARTICAM_E_TX_CMD
{
    UCTSK_UARTICAM_E_TX_CMD_FILE_INFO=0,
    COMMPC_E_TX_CMD_FILE_DATA,
};
//-------------------��̬����--------------------------------
//����
MODULE_OS_TASK_TAB(App_TaskUartIcamTCB);
MODULE_OS_TASK_STK(App_TaskUartIcamStk,APP_TASK_UARTICAM_STK_SIZE);
//�ź���
MODULE_OS_SEM(UartIcam_Sem_Rx);
//
static uint8_t UartIcam_RxBuf[20]= {0};
static uint16_t UartIcam_RxLen=0;
//-----------------------------------------------------------
extern struct tm CurrentDate;  //��ǰʱ��
//-------------------��������--------------------------------
static void uctsk_UartIcam (void *pvParameters);
/*********************************************************************************
��������: ����
**********************************************************************************/
uint8_t uctsk_UartIcam_Rx(uint8_t* pbuf, uint16_t len)
{
    // ���в�ѯ
    if((UartIcam_RxLen!=0) || (len==0) || (len>20))
    {
        return ERR;
    }
    // ������ȡ
    memcpy((char*)UartIcam_RxBuf,(char*)pbuf,len);
    UartIcam_RxLen = len;
    // �����ź�
    MODULE_OS_SEM_POST(UartIcam_Sem_Rx);
    return OK;
}
/*********************************************************************************
˵��: ���񴴽�
**********************************************************************************/
void  App_UartIcamTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task UartIcam",\
                          uctsk_UartIcam,\
                          APP_TASK_UARTICAM_PRIO,\
                          App_TaskUartIcamStk,\
                          APP_TASK_UARTICAM_STK_SIZE,\
                          App_TaskUartIcamTCB,\
                          NULL);
}

//--------------------------------------------------------------Modbus����ʵ��
static void uctsk_UartIcam (void *pvParameters)
{
    //���ݳ�ʼ��
    MODULE_MEMORY_S_PARA *pspara;
    UCTSK_S_MOVE_POWER *pUctskSMovePower;
    uint8_t *pbuf,*pbuf1;
    struct tm *ptm;
    uint32_t i32;
    uint16_t addr,len,len1;
    uint8_t res,res1,i,j;
    //
    static uint8_t sbuf[240]= {0};
    static uint16_t slen=0;
    
    //
    pspara  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    Count_ModbusSlaveAddr = pspara->Addr;
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
    //��ʼ���ź���
		MODULE_OS_ERR err;
    MODULE_OS_SEM_CREATE(UartIcam_Sem_Rx,"UartIcam_Sem_Rx",0);
    //Ӳ����ʼ��
    UART_INIT(UCTSK_UART_ICAM_UART,115200);
    //
    for(;;)
    {
        //�ȴ��ź�
        MODULE_OS_SEM_PEND(UartIcam_Sem_Rx,500,TRUE,err);
        //-----------------------
        if(err==MODULE_OS_ERR_NONE)
        {
            len1  =  0;
            //UART_DMA_Tx(UCTSK_UART_ICAM_UART,(uint8_t*)UartIcam_RxBuf,UartIcam_RxLen);
            //���е�ַ,����,CRC16��У��
            res=Count_Modbus_Check(UartIcam_RxBuf,UartIcam_RxLen,UartIcam_RxBuf[0]);
            if(res==OK)
            {
                pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                pbuf1= MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
                //��������
                switch(UartIcam_RxBuf[1])
                {
                    case COUNT_MODBUS_FC_WRITE_SINGLE_REGISTER:
                        addr  =  Count_2ByteToWord(UartIcam_RxBuf[3],UartIcam_RxBuf[2]);
                        len   =  Count_2ByteToWord(UartIcam_RxBuf[5],UartIcam_RxBuf[4]);
                        //
                        if(addr==0x0100)
                        {
                            len&=0x00FF;
                            i = len;
                            if(i==0x00 || i==0xFF)
                            {
                                pbuf1[len1++] =  Count_ModbusSlaveAddr;
                                pbuf1[len1++] =  UartIcam_RxBuf[1]|0x80;
                                pbuf1[len1++] =  0x03;
                            }
                            else
                            {
                                Count_ModbusSlaveAddr = i;
                                //
                                pspara = (MODULE_MEMORY_S_PARA *)pbuf;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                                pspara->Addr=i;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pspara,NULL);
                                //
                                pbuf1[len1++] =  Count_ModbusSlaveAddr;
                                pbuf1[len1++] =  UartIcam_RxBuf[1];
                                pbuf1[len1++] =  UartIcam_RxBuf[2];
                                pbuf1[len1++] =  UartIcam_RxBuf[3];
                                pbuf1[len1++] =  UartIcam_RxBuf[4];
                                pbuf1[len1++] =  UartIcam_RxBuf[5];
                            }
                        }
                        else
                        {
                            pbuf1[len1++] =  Count_ModbusSlaveAddr;
                            pbuf1[len1++] =  UartIcam_RxBuf[1]|0x80;
                            pbuf1[len1++] =  0x02;
                        }
                        len           =  Count_CRC16(pbuf1,len1);
                        pbuf1[len1++] =  len;
                        pbuf1[len1++] =  len>>8;
                        break;
                    case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
                        addr  =  Count_2ByteToWord(UartIcam_RxBuf[3],UartIcam_RxBuf[2]);
                        len   =  Count_2ByteToWord(UartIcam_RxBuf[5],UartIcam_RxBuf[4]);
                        j     =  len*2;
                        res1  =  ERR;
                        //��ȡ״̬
                        if((addr<=0x0F)&&((addr+len)<=0x10))
                        {
                            res1  =  OK;
                            for(i=0; i<=0x1F; i++)
                            {
                                pbuf[i] = i;
                            }
                            pbuf[0] =  uctsk_Rfms_NoBodyThresholdVTimerS>>24;
                            pbuf[1] =  uctsk_Rfms_NoBodyThresholdVTimerS>>16;
                            pbuf[2] =  uctsk_Rfms_NoBodyThresholdVTimerS>>8;
                            pbuf[3] =  uctsk_Rfms_NoBodyThresholdVTimerS>>0;
                            pspara = (MODULE_MEMORY_S_PARA*)pbuf1;
                            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                            pbuf[16]   =  (pspara->ThreshsholdValue1)>>24;
                            pbuf[17]   =  (pspara->ThreshsholdValue1)>>16;
                            pbuf[18]   =  (pspara->ThreshsholdValue1)>>8;
                            pbuf[19]   =  (pspara->ThreshsholdValue1)>>0;
                            //��λ
                            for(i=0; i<=0x1F; i++)
                            {
                                if(i+addr*2>0x1F)
                                {
                                    break;
                                }
                                pbuf[i]=pbuf[i+addr*2];
                            }
                        }
                        //��ȡʱ��
                        else if((addr==0x10)&&(len==0x03))
                        {
                            res1  =  OK;
                            BspRtc_ReadRealTime(NULL,NULL,NULL,pbuf);
                        }
                        //��ȡ��Чʱ��
                        else if((addr==0x20)&&(len==14))
                        {
                            res1  =  OK;
                            MODULE_MEMORY_S_DAY_INFO *pdayInfo;
                            //
                            memset((char*)pbuf,0,32);
                            //
                            pbuf1[300] =  0;
                            for(i=0; i<=6; i++)
                            {
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)pbuf1,&i);
                                pdayInfo = (MODULE_MEMORY_S_DAY_INFO*)pbuf1;
                                if(pdayInfo->BodyMoveNum!=0x00 && pdayInfo->BodyMoveNum!=0xFF)
                                {
                                    //�ó�ʱ��
                                    ptm   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                                    ptm->tm_year  =  Count_2ByteToWord(pdayInfo->BeginTime[0],pdayInfo->BeginTime[1]);
                                    ptm->tm_mon   =  pdayInfo->BeginTime[2]-1;
                                    ptm->tm_mday  =  pdayInfo->BeginTime[3];
                                    ptm->tm_hour  =  pdayInfo->BeginTime[4];
                                    ptm->tm_min   =  pdayInfo->BeginTime[5];
                                    ptm->tm_sec   =  pdayInfo->BeginTime[6];
                                    i32 = Count_Time_ConvCalendarToUnix(*ptm);
                                    i32 = i32 - 9*3600;
                                    *ptm = Count_Time_ConvUnixToCalendar(i32);
                                    //�鿴�ظ�
                                    for(pbuf1[301]=pbuf1[302]=0; pbuf1[301]<pbuf1[300]; pbuf1[301]+=4)
                                    {
                                        if(((ptm->tm_year-2000)==pbuf[pbuf1[301]+1])
                                           &&((ptm->tm_mon+1)==pbuf[pbuf1[301]+2])
                                           &&((ptm->tm_mday)==pbuf[pbuf1[301]+3]))
                                        {
                                            pbuf1[302]=1;
                                            break;
                                        }
                                    }
                                    if(pbuf1[302]==0)
                                    {
                                        pbuf[pbuf1[300]++]  =  1;
                                        pbuf[pbuf1[300]++]  =  ptm->tm_year-2000;
                                        pbuf[pbuf1[300]++]  =  ptm->tm_mon+1;
                                        pbuf[pbuf1[300]++]  =  ptm->tm_mday;
                                    }
                                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
                                }
                            }
                        }
                        else if((addr==0x30)&&(len==8))
                        {
                            res1  =  OK;
                            Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_DATE_R,(uint8_t*)pbuf,NULL);
                        }
                        //����
                        if(res1==OK)
                        {
                            Count_Modbus_Array(pbuf1,\
                                               &len1,\
                                               Count_ModbusSlaveAddr,\
                                               UartIcam_RxBuf[1],\
                                               NULL,\
                                               NULL,\
                                               &j,\
                                               pbuf,\
                                               len*2);
                        }
                        else
                        {
                            len1=0;
                            pbuf1[len1++] =  Count_ModbusSlaveAddr;
                            pbuf1[len1++] =  UartIcam_RxBuf[1]|0x80;
                            if(addr!=0)
                            {
                                pbuf1[len1++] =  0x02;
                            }
                            else if(len!=0x10)
                            {
                                pbuf1[len1++] =  0x03;
                            }
                            len           =  Count_CRC16(pbuf1,3);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        break;
                    case COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
                        addr = Count_2ByteToWord(UartIcam_RxBuf[3],UartIcam_RxBuf[2]);
                        len = Count_2ByteToWord(UartIcam_RxBuf[5],UartIcam_RxBuf[4]);
                        UartIcam_RxBuf[6] =  UartIcam_RxBuf[6];
                        if((addr>=0x08)&&(addr<=0x0F)&&((addr+len)<=0x10))
                        {
                            if(addr==0x08 && len==2)
                            {
                                res1   =  OK;
                                i32    =  Count_4ByteToLong(UartIcam_RxBuf[7],UartIcam_RxBuf[8],UartIcam_RxBuf[9],UartIcam_RxBuf[10]);
                                pspara = (MODULE_MEMORY_S_PARA*)pbuf1;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                                pspara->ThreshsholdValue1 =  i32;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pspara,NULL);
                                uctsk_Rfms_NoBodyThresholdVTimerS =  0;
                                uctsk_Rfms_NoBodyThresholdValue   =  i32;
                            }
                            else
                            {
                                res1   =  ERR;
                            }
                        }
                        else if((addr==0x10)&&(len==0x03))
                        {
                            if((UartIcam_RxBuf[7]>99)\
                               ||(UartIcam_RxBuf[8]==0)\
                               ||(UartIcam_RxBuf[8]>12)\
                               ||(UartIcam_RxBuf[9]>31)\
                               ||(UartIcam_RxBuf[10]>23)\
                               ||(UartIcam_RxBuf[11]>59)\
                               ||(UartIcam_RxBuf[12]>59))
                            {
                                res1  =  ERR;
                            }
                            else
                            {
                                res1  =  OK;
                                BspRtc_SetRealTime(NULL,NULL,NULL,&UartIcam_RxBuf[7]);
                            }
                        }
                        if(res1  == OK)
                        {
                            Count_Modbus_Array(pbuf1,\
                                               &len1,\
                                               Count_ModbusSlaveAddr,\
                                               UartIcam_RxBuf[1],\
                                               &addr,\
                                               &len,\
                                               NULL,\
                                               NULL,\
                                               0);
                        }
                        else
                        {
                            len1=0;
                            pbuf1[len1++] =  Count_ModbusSlaveAddr;
                            pbuf1[len1++] =  UartIcam_RxBuf[1]|0x80;
                            pbuf1[len1++] =  0x03;
                            len           =  Count_CRC16(pbuf1,3);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        break;
                    case 0x65:
                        ptm = (struct tm *)pbuf;
                        ptm->tm_year   =  UartIcam_RxBuf[2]+2000;
                        ptm->tm_mon    =  UartIcam_RxBuf[3]-1;
                        ptm->tm_mday   =  UartIcam_RxBuf[4];
                        ptm->tm_hour   =  9;
                        ptm->tm_min    =  0;
                        ptm->tm_sec    =  0;
                        pbuf1[0]       =  Count_ModbusSlaveAddr;
                        pbuf1[1]       =  UartIcam_RxBuf[1];
                        if(OK==uctsk_Rfms_KeepBodyMoveIndex(ptm,&pbuf1[2],NULL))
                        {
                            len1 = 2+480;
                            len           =  Count_CRC16(pbuf1,len1);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        else
                        {
                            pbuf1[1]      |= 0x80;
                            len1 = 2;
                            pbuf1[len1++] =  3;
                            len           =  Count_CRC16(pbuf1,len1);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        break;
                    case 0x66:
                        pbuf1[0]       =  Count_ModbusSlaveAddr;
                        pbuf1[1]       =  UartIcam_RxBuf[1];
                        pbuf1[2]       =  UartIcam_RxBuf[2];
                        pbuf1[3]       =  UartIcam_RxBuf[3];
                        pbuf1[4]       =  UartIcam_RxBuf[4];
                        pbuf1[5]       =  UartIcam_RxBuf[5];
                        if(OK==Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_10MIN_R,&pbuf1[6],&pbuf1[2]))
                        {
                            len1 = 6+240;
                        }
                        else
                        {
                            pbuf1[1]      |= 0x80;
                            len1 = 2;
                            pbuf1[len1++] =  3;
                        }
                        len           =  Count_CRC16(pbuf1,len1);
                        pbuf1[len1++] =  len;
                        pbuf1[len1++] =  len>>8;
                        break;
                    case 0x67:
                        ptm = (struct tm *)pbuf;
                        ptm->tm_year   =  UartIcam_RxBuf[2]+2000;
                        ptm->tm_mon    =  UartIcam_RxBuf[3]-1;
                        ptm->tm_mday   =  UartIcam_RxBuf[4];
                        ptm->tm_hour   =  9;
                        ptm->tm_min    =  0;
                        ptm->tm_sec    =  0;
                        pbuf1[0]       =  Count_ModbusSlaveAddr;
                        pbuf1[1]       =  UartIcam_RxBuf[1];
                        if(OK==uctsk_Rfms_KeepBodyMove_GetFinalInfo(ptm,&pbuf1[2]))
                        {
                            len1 = 2+1+pbuf1[2];
                            len           =  Count_CRC16(pbuf1,len1);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        else
                        {
                            pbuf1[1]      |= 0x80;
                            len1 = 2;
                            pbuf1[len1++] =  3;
                            len           =  Count_CRC16(pbuf1,len1);
                            pbuf1[len1++] =  len;
                            pbuf1[len1++] =  len>>8;
                        }
                        break;
                    case 0x6F:
                        len1              =  0;
                        pbuf1[len1++]     =  Count_ModbusSlaveAddr;
                        pbuf1[len1++]     =  UartIcam_RxBuf[1];
                        if(UartIcam_RxLen!=5 || UartIcam_RxBuf[2]<1 || UartIcam_RxBuf[2]>2)
                        {
                            pbuf1[len1-1] |= 0x80;
                        }
                        else
                        {
                            //ִ�и�ʽ��
                            if(UartIcam_RxBuf[2]==1)
                            {
                                //ȫ��ʽ��
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_FORMAT,NULL,NULL);
                                i = MODULE_MEMORY_CMD_FACTORY_MODE_ALL;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
                            }
                            else if(UartIcam_RxBuf[2]==2)
                            {
                                //���ʽ��
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_FORMAT,NULL,NULL);
                                i = MODULE_MEMORY_CMD_FACTORY_MODE_HALF;
                                Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
                            }
                        }
                        pbuf1[len1++]     =  UartIcam_RxBuf[2];
                        len               =  Count_CRC16(pbuf1,len1);
                        pbuf1[len1++]     =  len;
                        pbuf1[len1++]     =  len>>8;
                        break;
                    default:
                        len1              =  0;
                        pbuf1[len1++]     =  Count_ModbusSlaveAddr;
                        pbuf1[len1++]     =  UartIcam_RxBuf[1]|0x80;
                        pbuf1[len1++]     =  0x01;
                        len               =  Count_CRC16(pbuf1,len1);
                        pbuf1[len1++]     =  len;
                        pbuf1[len1++]     =  len>>8;
                        break;
                }
                if((len1>0)&&(len1<=512))
                {
                    UART_DMA_Tx(UCTSK_UART_ICAM_UART,(uint8_t*)pbuf1,len1);
                }
                if((UartIcam_RxBuf[1]==0x6F) && ((pbuf1[1]&0x80)==0))
                {
                    MODULE_OS_DELAY_MS(1000);
                    MCU_SOFT_RESET;
                }
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf1);
            }
            UartIcam_RxLen=0;
        }
        else if(err==MODULE_OS_ERR_TIMEOUT)
        {
            //UART_DMA_Tx(UCTSK_UART_ICAM_UART,(uint8_t*)"NULL",4);
            //uctsk_Rfms_SetRunState(UCTSK_RFMS_E_RUNSTATE_IDLE);
            pUctskSMovePower  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pbuf  =  (uint8_t*)pUctskSMovePower;
            res = uctsk_Rfms_BufferLoop1Pop((uint8_t*)pUctskSMovePower,&len);
            if((CurrentDate.tm_min%10==0)&&(CurrentDate.tm_sec<3)&&(slen!=0))
            {
                slen=0;
            }
            //��ȡ����
            if(OK==res && len==sizeof(UCTSK_S_MOVE_POWER))
            {
                //
                for(res=0; res<6; res++)
                {
                    if((slen+4)>=240)break;
                    sbuf[slen++]=(uint8_t)(pUctskSMovePower->Data[res]>>24);
                    sbuf[slen++]=(uint8_t)(pUctskSMovePower->Data[res]>>16);
                    sbuf[slen++]=(uint8_t)(pUctskSMovePower->Data[res]>>8);
                    sbuf[slen++]=(uint8_t)(pUctskSMovePower->Data[res]>>0);
                }
            }
            //
            if(slen>=240)
            {
                //����������
                pbuf[0] =  (uint8_t)(CurrentDate.tm_year-2000);
                pbuf[1] =  (uint8_t)(CurrentDate.tm_mon+1);
                pbuf[2] =  (uint8_t)(CurrentDate.tm_mday);
                pbuf[3] =  (uint8_t)((CurrentDate.tm_hour*60+CurrentDate.tm_min)/10);
                Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_10MIN_W,sbuf,pbuf);
                slen=0;
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pUctskSMovePower);
        }
        else
        {
            ;
        }
    }
}
#endif
//-----------------------------------------------------------

