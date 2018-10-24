//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: RealView MDK-ARM Version 4.14                    //
//                    �� �� ��: RealView MDK-ARM Version 4.14                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: Memory.c                                         //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-01-17    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
//
// �ļ������� Ӧ�ò�洢��д����
//
//
// ע�����1,�в���δ���ƴ�����(����: ������),��д��¼��������¼������Խ�紦��
//           2,Ŀǰֻ��FLASH�����Ŀ�Ȩ���޸Ĳ����жϵ�ָ�����.�������ִ�����.
//
// �޸ļ�¼��
//    ���  ����     ����     ����
//-------------------------------------------------------------------------------

#include <includes.h>
#ifndef STM32
#include "user.h"
#endif
//-------------------------------------------------------------------------------
//��ʱʹ��: ����FLASH���ٲ����㷨��û�г�ֲ���
#define MEMORY_FLASH_FAST_ERASE_ENABLE
//-------------------------------------------------------------------------------�ⲿ��������
/*
//-----�ȴ��ź�
#ifdef UCOS
    INT8U err;
    OSSemPend(Sem_EepromFlashUse,0,&err);
#endif
//-----�ͷ��ź�
#ifdef UCOS
    OSSemPost(Sem_EepromFlashUse);
#endif
*/
#ifdef UCOS
extern OS_EVENT *Sem_EepromFlashUse;
extern OS_EVENT *Sem_EepromUse;
extern OS_EVENT *Sem_FlashUse;
extern OS_EVENT *Sem_FlashModofictionUse;
#endif
//----------ר�������뺯���޸�
INT8U MemberBuf[MEMORY_FLASH_MAX_WRITE_SIZE];
S_MEMORY_EEPROM_FLASH* p_sEepromFlash_MemoryEnterFuction;
//----------ר�����������
//INT32U Member_SBuf[MEMORY_FLASH_MAX_WRITE_SIZE/4];
INT32U Member_I32;
INT32U Member_J32;
INT8U *p_I8;
INT16U *p_I16;
S_MEMORY_VER *p_Ver_MemoryApp;
S_CHECK *p_Check_MemoryApp;
S_MEMORY_CHECK *p_MemoryCheck_MemoryApp;
S_MEMORY_KEEPPARA *p_KeepPara_MemoryApp;
S_MEMORY_CONTROL *p_Control_MemoryApp;
S_MEMORY_ALARM *p_Alarm_MemoryApp;
S_MEMORY_DOOR *p_Door_MemoryApp;
//S_MEMORY_DOORGBITMAP *p_DoorGBitmap_MemoryApp;
S_MEMORY_CARDG_BITMAP *p_CardGBitmap_MemoryApp;
S_MEMORY_READER *p_Reader_MemoryApp;
S_MEMORY_RELAY *p_Relay_MemoryApp;
S_MEMORY_INPUT *p_Input_MemoryApp;
S_MEMORY_DOOR_GROUP *p_DoorG_MemoryApp;
S_MEMORY_CARDGROUP_LIMIT *p_CardG_MemoryApp;
S_MEMORY_AREA *p_Area_MemoryApp;
S_MEMORY_HOLIDAYGROUP *p_HolidayG_MemoryApp;
S_MEMORY_TIMEGROUP *p_TimeG_MemoryApp;
S_MEMORY_USER_PERMISSION *p_User_MemoryApp;
S_MEMORY_EEPROM_FLASH *p_EepromFlash_MemoryApp;
S_MEMORY_REVERSE *p_Reverse_MemoryApp;
S_MEMORY_CARD_RECORD *p_card_record_MemoryApp;
S_MEMORY_ALARM_RECORD *p_alarm_record_MemoryApp;
S_MEMORY_LOGICBLOCK_INOUTAND *p_LogicBlockInOutAnd_MemoryApp;
/*
S_MEMORY_LOGICBLOCK_INPUT *p_LogicBlockInput_MemoryApp;
S_MEMORY_LOGICBLOCK_LOGIC *p_LogicBlockLogic_MemoryApp;
S_MEMORY_LOGICBLOCK_OUTPUT *p_LogicBlockOutput_MemoryApp;
*/
//----------ȫ��ͨ�ñ���(�������������������)
INT8U Member_PrintBuf[100];
//�ṹ����
S_MEMORY_VER *p_Ver;
S_CHECK *p_Check;
S_MEMORY_CHECK *p_MemoryCheck;
S_MEMORY_KEEPPARA *p_KeepPara;
S_MEMORY_CONTROL *p_Control;
S_MEMORY_ALARM *p_Alarm;
S_MEMORY_DOOR *p_Door;
//S_MEMORY_DOORGBITMAP *p_DoorGBitmap;
S_MEMORY_CARDG_BITMAP *p_CardGBitmap;
S_MEMORY_READER *p_Reader;
S_MEMORY_RELAY *p_Relay;
S_MEMORY_INPUT *p_Input;
S_MEMORY_DOOR_GROUP *p_DoorG;
S_MEMORY_CARDGROUP_LIMIT *p_CardG;
S_MEMORY_AREA *p_Area;
S_MEMORY_HOLIDAYGROUP *p_HolidayG;
S_MEMORY_TIMEGROUP *p_TimeG;
S_MEMORY_USER_PERMISSION *p_User;
S_MEMORY_ALARM_RECORD *p_AlarmRecord;
S_MEMORY_CARD_RECORD *p_CardRecord;
S_MEMORY_REVERSE *p_Reverse1;
S_MEMORY_LOGICBLOCK_INOUTAND *p_LogicBlockInOutAnd;
/*
S_MEMORY_LOGICBLOCK_INPUT *p_LogicBlockInput;
S_MEMORY_LOGICBLOCK_LOGIC *p_LogicBlockLogic;
S_MEMORY_LOGICBLOCK_OUTPUT *p_LogicBlockOutput;
*/
static struct tm Memory_tm,Memory_tm1;
//
INT8U Memory_RecordNum_ms=0;//ÿ����¼/�����ı�Ų�����,Ŀǰ�õ���αmsʱ�������.
//-------------------------------------------------------------------------------ö�ٶ���
enum MEMORY_ENTRY_CMD
{
    MEMORY_ENTRY_CMD_VAR_LOAD=0,
    MEMORY_ENTRY_CMD_EEPROM_INIT,
    MEMORY_ENTRY_CMD_EEPROM_READ,
    MEMORY_ENTRY_CMD_EEPROM_WRITE,
    MEMORY_ENTRY_CMD_FLASH_INIT,
    MEMORY_ENTRY_CMD_FLASH_CHIP_ERASE,
    MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,
    MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
    MEMORY_ENTRY_CMD_FLASH_READ,
    MEMORY_ENTRY_CMD_FLASH_WRITE,
    MEMORY_ENTRY_CMD_FLASH_MODIFICATION,
};
//-------------------------------------------------------------------------------
//                     ��ں���---����ַд����
//-------------------------------------------------------------------------------
static INT8U Memory_MachineAddr=0xFF;
static INT8U Memory_UartBaudRate=0xFF;
static INT32U Memory_msCount = 0;
INT8U Memory_EnterFunction(INT8U cmd,INT32U addr,INT8U *buf,INT16U len)
{
    INT8U res=OK;
    INT8U i;
    INT16U i16,j16;
    INT32U i32,j32,k32;
    INT8U buf1[2];
    INT8U err_count=0;
    switch(cmd)
    {
        //------------------------------
        case MEMORY_ENTRY_CMD_VAR_LOAD:
            GetHardWareSetup(&Memory_MachineAddr,&Memory_UartBaudRate,NULL,NULL);
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_EEPROM_INIT:
#ifdef STM32
#ifdef UCOS
            OSSemPend(Sem_EepromUse,0,&i);
#endif
            EEPROM_Init();
#ifdef UCOS
            OSSemPost(Sem_EepromUse);
#endif
#else
            I2C_Init();
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_EEPROM_READ:
#ifdef UCOS
            OSSemPend(Sem_EepromUse,0,&i);
#endif
            EE_Read_nByte((INT16U)addr,buf,len);
#ifdef UCOS
            OSSemPost(Sem_EepromUse);
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_EEPROM_WRITE:
#ifdef UCOS
            OSSemPend(Sem_EepromUse,0,&i);
#endif
            err_count=0;
            while(1)
            {
                EE_Write_nByte((INT16U)addr,buf,len);
                //У��(�����������ֽ�д��ȷ,����Ϊ��֡����д��ȷ)
                EE_Read_nByte((INT16U)addr+len-2,buf1,2);
                if((buf[len-2]==buf1[0])&&(buf[len-1]==buf1[1]))
                {
                    res=OK;
                    break;
                }
                else
                {
                    err_count++;
                }
                if(err_count>=3)
                {
                    res=ERR;
                    break;
                }
            }
#ifdef UCOS
            OSSemPost(Sem_EepromUse);
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_FLASH_INIT:
#ifdef STM32
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            Flash_Init();
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
#else
            Spi1MasterInit();
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_FLASH_CHIP_ERASE:
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            Flash_Chip_Erase();
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_FALSH_4KB_ERASE:
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            Flash_Block_Erase(addr,_4K_);
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE:
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            Flash_Fast_Erase(addr/MEMORY_FLASH_MIN_ERASE_SIZE,addr/MEMORY_FLASH_MIN_ERASE_SIZE+len);
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
            break;
        //------------------------------
        case MEMORY_ENTRY_CMD_FLASH_READ:
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            err_count=0;
            while(1)
            {
                if(res==OK)
                {
                    res=Flash_Read_nByte(addr,buf,len);
                    if(res==OK)
                    {
                        if(res==OK)
                        {
                            res=OK;
                            break;
                        }
                        else
                        {
                            err_count++;
                        }
                    }
                    else
                    {
                        err_count++;
                    }
                }
                else
                {
                    err_count++;
                }
                if(err_count>=3)
                {
                    res=ERR;
                    break;
                }
                else
                {
                    err_count++;
                }
            }
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
            break;
        case MEMORY_ENTRY_CMD_FLASH_WRITE:
#ifdef UCOS
            OSSemPend(Sem_FlashUse,0,&i);
#endif
            res=Flash_Write_nByte(addr,buf,len);
            if(res==OK)
            {
                ;
                //    res=Flash_Wait_Busy();
            }
#ifdef UCOS
            OSSemPost(Sem_FlashUse);
#endif
            break;
        case MEMORY_ENTRY_CMD_FLASH_MODIFICATION:
#ifdef UCOS
            OSSemPend(Sem_FlashModofictionUse,0,&i);
#endif
            //����Ӧ����ͷ��ַ-->i32
            i32 = (addr/MEMORY_FLASH_MIN_ERASE_SIZE)*MEMORY_FLASH_MIN_ERASE_SIZE;
            //��ȡ���ݿ���-->i
            {
                p_sEepromFlash_MemoryEnterFuction = (S_MEMORY_EEPROM_FLASH*)MemberBuf;
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_EEPROM_FLASH_ADDR,(INT8U*)p_sEepromFlash_MemoryEnterFuction,sizeof(S_MEMORY_EEPROM_FLASH));
                if(res==ERR)goto GotoMemoryEnter;
                i=p_sEepromFlash_MemoryEnterFuction->SaveBlockNum;
                if(i>=MEMORY_FLASH_MAX_MODIFICATION)
                {
                    i=0;
                }
            }
            //�屸����
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,MEMORY_FLASH_MODIFICATION_ADDR+i*MEMORY_FLASH_MODIFICATION_SIZE,0,0);
            if(res==ERR)goto GotoMemoryEnter;
            //��ȡ���Ե�ַ-->K32 , ����-->i16
            k32=addr;
            i16=len;
            //Ӧ����(4K)-->������
            for(j32=i32,j16=0; j32<(i32+MEMORY_FLASH_MIN_ERASE_SIZE); j32+=MEMORY_FLASH_MAX_WRITE_SIZE)
            {
                //��ȡӦ����
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,j32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res==ERR)break;
                //�޸�Ӧ����
                if((j32+MEMORY_FLASH_MAX_WRITE_SIZE)<=k32)
                {
                    //����Ҫ���ĵ����ݶ�
                    ;
                }
                else if((j32<=k32)&&((j32+MEMORY_FLASH_MAX_WRITE_SIZE)>=(k32+i16)))
                {
                    //��Խ��
                    memcpy(&MemberBuf[k32-j32],&buf[j16],i16);
                    j16+=i16;
                }
                else if((j32<=k32)&&((j32+MEMORY_FLASH_MAX_WRITE_SIZE)<(k32+i16)))
                {
                    //Խ��
                    memcpy(&MemberBuf[k32-j32],&buf[j16],j32+MEMORY_FLASH_MAX_WRITE_SIZE-k32);
                    j16 += (j32+MEMORY_FLASH_MAX_WRITE_SIZE-k32);
                    //���µ�ַ�ͳ���
                    k32=j32+MEMORY_FLASH_MAX_WRITE_SIZE;
                    i16=addr+len-(j32+MEMORY_FLASH_MAX_WRITE_SIZE);
                }
                //Ӧ����-->������
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_FLASH_MODIFICATION_ADDR+i*MEMORY_FLASH_MODIFICATION_SIZE+(j32-i32),MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
            }
            if(res==ERR)goto GotoMemoryEnter;
            //дEEPROM��־
            {
                p_sEepromFlash_MemoryEnterFuction = (S_MEMORY_EEPROM_FLASH*)MemberBuf;
                memset((INT8U*)p_sEepromFlash_MemoryEnterFuction,0xFF,sizeof(S_MEMORY_EEPROM_FLASH));
                memcpy((INT8U*)p_sEepromFlash_MemoryEnterFuction,MEMORY_EEPROM_FLASH_HEAD_STR,6);
                p_sEepromFlash_MemoryEnterFuction->FalshAppAddr=i32;
                p_sEepromFlash_MemoryEnterFuction->SaveBlockNum=i;
                p_sEepromFlash_MemoryEnterFuction->Sum=Count_Sum((INT8U*)p_sEepromFlash_MemoryEnterFuction,sizeof(S_MEMORY_EEPROM_FLASH)-2);
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EEPROM_FLASH_ADDR,(INT8U*)p_sEepromFlash_MemoryEnterFuction,sizeof(S_MEMORY_EEPROM_FLASH));
            }
            if(res==ERR)goto GotoMemoryEnter;
            //����Ӧ������
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
            if(res==ERR)goto GotoMemoryEnter;
            //������-->Ӧ����
            for(j32=i32; j32<(i32+MEMORY_FLASH_MIN_ERASE_SIZE); j32+=MEMORY_FLASH_MAX_WRITE_SIZE)
            {
                //��ȡ����
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_MODIFICATION_ADDR+i*MEMORY_FLASH_MODIFICATION_SIZE+(j32-i32),MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res==ERR)goto GotoMemoryEnter;
                //������-->Ӧ����
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,j32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res==ERR)goto GotoMemoryEnter;
            }
            if(res==ERR)goto GotoMemoryEnter;
            //��EEPROM
            {
                p_sEepromFlash_MemoryEnterFuction = (S_MEMORY_EEPROM_FLASH*)MemberBuf;
                memset((INT8U*)p_sEepromFlash_MemoryEnterFuction,0xFF,sizeof(S_MEMORY_EEPROM_FLASH));
                p_sEepromFlash_MemoryEnterFuction->SaveBlockNum=(i+1)%MEMORY_FLASH_MAX_MODIFICATION;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EEPROM_FLASH_ADDR,(INT8U*)p_sEepromFlash_MemoryEnterFuction,sizeof(S_MEMORY_EEPROM_FLASH));
            }
            if(res==ERR)goto GotoMemoryEnter;
        GotoMemoryEnter:
#ifdef UCOS
            OSSemPost(Sem_FlashModofictionUse);
#endif
            break;
        default:
            break;
    }
    return res;
}
//-------------------------------------------------------------------------------
//                     ���ں���---����ַд����
//ʹ����ʾ: ������1mS�ж�
//-------------------------------------------------------------------------------
void Memory_1msPro(void)
{
    if(Memory_msCount!=0xFFFFFFFF)  //1mS������
    {
        Memory_msCount++;
    }
}

//-------------------------------------------------------------------------------
//                     ���ں���---����ַд����
//����˵��: Cmd    --- ��memory.h��MEMORY_APP_CMDϵ������ö��
//          *Buf   --- ����
//          Number --- ��0Ϊ��ʼ
//          SerialNumber --- ��ˮ��(��ƽ̨���ò���ʱ�·�)
//�� �� ֵ: MEMORY_APP_ERR�ṹ��
//ʹ����ʾ: ���Ҫ�޸�ĳ���ṹ���и������,������Ҫ�ȶ����޸��ٴ洢.
//-------------------------------------------------------------------------------
//��ϸʹ��˵������:
//-----������д����
//    MEMORY_APP_CMD_VER_R=0,            //���汾
//       *Buf:          S_MEMORY_VER�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_VER_W,              //д�汾
//       *Buf:          S_MEMORY_VER�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CHECK_R,            //��У��
//       *Buf:          S_MEMORY_CHECK�ṹ��ָ��
//       Number:        MEMORY_APP_CHECK_ENUM����
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CHECK_W,            //дУ��
//       *Buf:          S_MEMORY_CHECK�ṹ��ָ��
//       Number:        MEMORY_APP_CHECK_ENUM����
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CONTROL_R,          //��������Ϣ
//       *Buf:          S_MEMORY_CONTROL�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CONTROL_W,          //д������Ϣ
//       *Buf:          S_MEMORY_CONTROL�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_DOOR_R,             //����
//       *Buf:          S_MEMORY_DOOR�ṹ��ָ��
//       Number:        0<=Number<MEMORY_DOOR_MAX_NUM
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_DOOR_W,             //д��
//       *Buf:          S_MEMORY_DOOR�ṹ��ָ��
//       Number:        0<=Number<MEMORY_DOOR_MAX_NUM
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_ALARM_R,            //������
//       *Buf:          S_MEMORY_ALARM�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_ALARM_W,            //д����
//       *Buf:          S_MEMORY_ALARM�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_DOORGROUP_R,        //������Ȩ��
//       *Buf:          S_MEMORY_DOOR_GROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_DOORGROUP_MAX_NUM
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_DOORGROUP_W,        //д����Ȩ��
//       *Buf:          S_MEMORY_DOOR_GROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_DOORGROUP_MAX_NUM
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_CARDGROUP_LIMIT_R,  //������Ȩ��
//       *Buf:          S_MEMORY_CARDGROUP_LIMIT�ṹ��ָ��
//       Number:        0<=Number<MEMORY_CARDGROUPLIMIT_MAX_NUM
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CARDGROUP_LIMIT_W,  //д����Ȩ��
//       *Buf:          S_MEMORY_CARDGROUP_LIMIT�ṹ��ָ��
//       Number:        0<=Number<MEMORY_CARDGROUPLIMIT_MAX_NUM
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_AREA_R,             //������Ȩ��
//       *Buf:          S_MEMORY_AREA�ṹ��ָ��
//       Number:        0<=Number<MEMORY_AREA_MAX_NUM
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_AREA_W,             //д����Ȩ��
//       *Buf:          S_MEMORY_AREA�ṹ��ָ��
//       Number:        0<=Number<MEMORY_AREA_MAX_NUM
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_TIMEGROUP_R,        //��ʱ������
//       *Buf:          S_MEMORY_TIMEGROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_TIMEGROUP
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_TIMEGROUP_W,        //дʱ������
//       *Buf:          S_MEMORY_TIMEGROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_TIMEGROUP
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_HOLIDAYGROUP_R,     //���ڼ�����
//       *Buf:          S_MEMORY_HOLIDAYGROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_HOLIDAYGROUP
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_HOLIDAYGROUP_W,     //д�ڼ�����
//       *Buf:          S_MEMORY_HOLIDAYGROUP�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_HOLIDAYGROUP
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_CARD_RECORD_R,      //��ˢ����¼
//       *Buf:          S_MEMORY_CARD_RECORD�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_CARD_RECORD
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_CARD_RECORD_W,      //дˢ����¼
//       *Buf:          S_MEMORY_CARD_RECORD�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_ALARM_RECORD_R,     //��������¼
//       *Buf:          S_MEMORY_ALARM_RECORD�ṹ��ָ��
//       Number:        0<=Number<MEMORY_FLASH_MAX_ALARM_RECORD
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_ALARM_RECORD_W,     //д������¼
//       *Buf:          S_MEMORY_ALARM_RECORD�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_USER_PERMISSION_R,  //���û�
//       ģʽ1:   ���Ų���
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��(��Ҫ�û��Ȱѿ��Ÿ�ֵ��0)
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//       ģʽ2:   �������
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��(��Ҫ�û��Ȱѿ��Ÿ�ֵ0,�����븳ֵ��0x00000000��0xFFFFFFFF)
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//       ģʽ3:   ��������
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��(��Ҫ�û��Ȱѿ��Ÿ�ֵΪ0)
//       Number:        0<=Number<MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
//       SerialNumber:  ��ͨ��(0<=Number<MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)
//                      �ÿ���(Number = MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)
//    MEMORY_APP_CMD_NEW_USER_W,         //�½���ͨ�û�
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_MODY_USER_W,        //�޸���ͨ�û�
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_NEW_VISITOR_W,      //�½���ʱ�û�
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_MODY_VISITOR_W,     //�޸���ʱ�û�
//       *Buf:          S_MEMORY_USER_PERMISSION�ṹ��ָ��
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//-----������ָ��
//    MEMORY_APP_CMD_REVERSE_R,          //������
//    MEMORY_APP_CMD_REVERSE_W,          //д����
//       *Buf:          S_MEMORY_REVERSE�ṹ��ָ��
//       Number:        �ź�(0-3)
//       SerialNumber:  �̶�Ϊ0
//    MEMORY_APP_CMD_REVERSE_C,          //�巴��
//       *Buf:          S_MEMORY_REVERSE�ṹ��ָ��
//                      S_MEMORY_REVERSE.CardNumber=0xFFFFFFFF ��������ȫ��
//                      S_MEMORY_REVERSE.CardNumber!=0xFFFFFFFF ���������
//       Number:        �ź�(0-3)
//       SerialNumber:  �̶�Ϊ0
//-----����ָ��
//    MEMORY_APP_CMD_FORMAT,             //��ʽ��EEPROM��FLASH,��д�����ͷ��Ϣ
//       *Buf:          �̶�Ϊ0
//       Number:        ��modbus�ĸ�ʽ��λͼ����
//       SerialNumber:  0x00000000-0xFFFFFFFF
//    MEMORY_APP_CMD_INIT,               //��ʼ��IIC��SPI�����DMA
//       *Buf:          �̶�Ϊ0
//       Number:        �̶�Ϊ0
//       SerialNumber:  �̶�Ϊ0
//       ˵��:    ���˽��л�����IIC��SPI��ʼ����,��ִ�������²���:
//                1,FLASH����ʱ����ϵ�,���ϵ����лָ�(����).
//                2,������汾��MEMORY_MAIN_VER�ı�,��ִ�и�ʽ��.
//                3,���EEPROMͷ��ΪMEMORY_PRODUCT_HEAD,��ִ�и�ʽ��.
//-------------------------------------------------------------------------------
static INT8U Memory_InternalApp(INT16U Cmd,INT8U *Buf,INT32U Number,INT32U SerialNumber)
{
    INT8U i;
    INT16U i16;
    INT32U i32,j32;
    INT32U *p_i32;
    INT8U res=OK;
    INT16U count;
    //���뻺��
    INT8U *Member_SBuf;
    Member_SBuf=MemManager_Get(256,&res);
    //
    switch(Cmd)
    {
        //�����ʼ������
        case MEMORY_APP_CMD_EEPROM_INIT_TEST:
            Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_INIT,NULL,NULL,NULL);
            for(i=0; i<100; i++)
            {
                Member_SBuf[0]=Member_SBuf[1]=0;
                Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EERPOM_END_ADDR,Member_SBuf,2);
                Member_SBuf[0]=0x55;
                Member_SBuf[1]=0xAA;
                Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EERPOM_END_ADDR,Member_SBuf,2);
                Member_SBuf[0]=Member_SBuf[1]=0;
                Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_EERPOM_END_ADDR,Member_SBuf,2);
                if(Member_SBuf[0]==0x55&&Member_SBuf[1]==0xAA)
                {
                    break;
                }
            }
            if(i<100)
            {
                res=OK;
            }
            else
            {
                res=ERR;
            }
            break;
        //FLASH��ʼ������
        case MEMORY_APP_CMD_FLASH_INIT_TEST:
            Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_INIT,NULL,NULL,NULL);
            for(i=0; i<100; i++)
            {
                Flash_Block_Erase(MEMORY_FLASH_END_ADDR,_4K_);
                Member_SBuf[0]=0x55;
                Member_SBuf[1]=0xAA;
                Flash_Write_nByte(MEMORY_FLASH_END_ADDR,Member_SBuf,2);
                Member_SBuf[0]=Member_SBuf[1]=0;
                Flash_Read_nByte(MEMORY_FLASH_END_ADDR,Member_SBuf,2);
                if(Member_SBuf[0]==0x55&&Member_SBuf[1]==0xAA)
                {
                    break;
                }
            }
            if(i<100)
            {
                res=OK;
            }
            else
            {
                res=ERR;
            }
            break;
        //���汾
        case MEMORY_APP_CMD_VER_R:
        {
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_VER_ADDR,Buf,sizeof(S_MEMORY_VER));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Ver_MemoryApp = (S_MEMORY_VER*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_VER)-2);
            if(i16==p_Ver_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д�汾(��У��)
        case MEMORY_APP_CMD_VER_W:
        {
            p_Ver_MemoryApp = (S_MEMORY_VER*)Buf;
            p_Ver_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_VER)-2);
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_VER_ADDR,Buf,sizeof(S_MEMORY_VER));
            break;
        }
        //��У��
        case MEMORY_APP_CMD_CHECK_R:
        {
            switch(Number)
            {
                case MEMORY_APP_CHECK_ALL:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR,Buf,sizeof(S_MEMORY_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_RECORD:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR,Buf,2);
                    break;
                case MEMORY_APP_CHECK_ALARM_RECORD:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+2,Buf,2);
                    break;
                case MEMORY_APP_CHECK_DOOR:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*0),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_DOORGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*1),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARDGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*2),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_ALARM:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*3),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_AREA:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*4),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_HOLIDAYGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*5),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_TIMEGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*6),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_VISITOR:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*7),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER1:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*8),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER2:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*9),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER3:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*10),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER4:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*11),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER5:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*12),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER6:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*13),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER7:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*14),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER8:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*15),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER9:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*16),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER10:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*17),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER11:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*18),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER12:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*19),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER13:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*20),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER14:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*21),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER15:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*22),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER16:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*23),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_READER:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*24),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_RELAY:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*25),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_INPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*26),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*27),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARDG_BITMAP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*28),Buf,sizeof(S_CHECK));
                    break;
                /*    
                case MEMORY_APP_CHECK_LOGICBLOCK_INPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*29),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_LOGIC:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*30),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*31),Buf,sizeof(S_CHECK));
                    break;
                */    
                default:
                    res=MEMORY_APP_ERR_PARA;
                    break;
            }
            break;
        }
        //дУ��
        case MEMORY_APP_CMD_CHECK_W:
        {
            switch(Number)
            {
                case MEMORY_APP_CHECK_ALL:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR,Buf,sizeof(S_MEMORY_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_RECORD:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR,Buf,2);
                    break;
                case MEMORY_APP_CHECK_ALARM_RECORD:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+2,Buf,2);
                    break;
                case MEMORY_APP_CHECK_DOOR:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*0),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_DOORGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*1),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARDGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*2),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_ALARM:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*3),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_AREA:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*4),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_HOLIDAYGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*5),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_TIMEGROUP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*6),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_VISITOR:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*7),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER1:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*8),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER2:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*9),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER3:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*10),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER4:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*11),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER5:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*12),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER6:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*13),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER7:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*14),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER8:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*15),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER9:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*16),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER10:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*17),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER11:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*18),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER12:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*19),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER13:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*20),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER14:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*21),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER15:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*22),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARD_USER16:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*23),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_READER:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*24),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_RELAY:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*25),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_INPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*26),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*27),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_CARDG_BITMAP:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*28),Buf,sizeof(S_CHECK));
                    break;
                /*    
                case MEMORY_APP_CHECK_LOGICBLOCK_INPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*29),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_LOGIC:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*30),Buf,sizeof(S_CHECK));
                    break;
                case MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT:
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CHECK_ADDR+4+(sizeof(S_CHECK)*31),Buf,sizeof(S_CHECK));
                    break;
                */    
                default:
                    res=MEMORY_APP_ERR_PARA;
                    break;
            }
            break;
        }
        //��Ҫ����Ĳ���
        case MEMORY_APP_CMD_KEEPPARA_R:
        {
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_KEEPPARA_ADDR,Buf,sizeof(S_MEMORY_KEEPPARA));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_KeepPara_MemoryApp = (S_MEMORY_KEEPPARA*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_KEEPPARA)-2);
            if(i16==p_KeepPara_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //дҪ����Ĳ���
        case MEMORY_APP_CMD_KEEPPARA_W:
        {
            p_KeepPara_MemoryApp = (S_MEMORY_KEEPPARA*)Buf;
            p_KeepPara_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_KEEPPARA)-2);
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_KEEPPARA_ADDR,Buf,sizeof(S_MEMORY_KEEPPARA));
            break;
        }
        //������
        case MEMORY_APP_CMD_CONTROL_R:
        {
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CONTROL_ADDR,Buf,sizeof(S_MEMORY_CONTROL));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Control_MemoryApp = (S_MEMORY_CONTROL*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_CONTROL)-2);
            if(i16==p_Control_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д����
        case MEMORY_APP_CMD_CONTROL_W:
        {
            p_Control_MemoryApp = (S_MEMORY_CONTROL*)Buf;
            p_Control_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_CONTROL)-2);
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CONTROL_ADDR,Buf,sizeof(S_MEMORY_CONTROL));
            break;
        }
        //����
        case MEMORY_APP_CMD_DOOR_R:
        {
            if(Number>=MEMORY_DOOR_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_DOOR_ADDR+Number*MEMORY_DOOR_SIZE,Buf,sizeof(S_MEMORY_DOOR));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Door_MemoryApp = (S_MEMORY_DOOR*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_DOOR)-2);
            if(i16==p_Door_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д��
        case MEMORY_APP_CMD_DOOR_W:
        {
            p_Door_MemoryApp = (S_MEMORY_DOOR*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_Door_MemoryApp->Number==0)
            {
                p_Door_MemoryApp->Number=0xFF;
                p_Door_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_Door_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_DOOR)-2);
            }
            if(Number>=MEMORY_DOOR_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_DOOR_ADDR+Number*MEMORY_DOOR_SIZE,Buf,sizeof(S_MEMORY_DOOR));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOOR,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У��ָ��
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    for(count=i=0; i<MEMORY_DOOR_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_DOOR_ADDR+i*MEMORY_DOOR_SIZE+sizeof(S_MEMORY_DOOR)-2,(INT8U *)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOOR,0);
                }
            }
            break;
        }
        //������λͼ
        /*
        case MEMORY_APP_CMD_DOORGBITMAP_R:
        {
            if(Number>=MEMORY_DOORGBITMAP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_DOORGBITMAP_ADDR+Number*MEMORY_DOORGBITMAP_SIZE,Buf,sizeof(S_MEMORY_DOORGBITMAP));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_DoorGBitmap_MemoryApp= (S_MEMORY_DOORGBITMAP*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_DOORGBITMAP)-2);
            if(i16==p_DoorGBitmap_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        */
        //д����λͼ
        /*
        case MEMORY_APP_CMD_DOORGBITMAP_W:
        {
            p_DoorGBitmap_MemoryApp = (S_MEMORY_DOORGBITMAP*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_DoorGBitmap_MemoryApp->Number==0)
            {
                p_DoorGBitmap_MemoryApp->Number=0xFF;
                p_DoorGBitmap_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_DoorGBitmap_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_DOORGBITMAP)-2);
            }
            if(Number>=MEMORY_DOORGBITMAP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_DOORGBITMAP_ADDR+Number*MEMORY_DOORGBITMAP_SIZE,Buf,sizeof(S_MEMORY_DOORGBITMAP));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORG_BITMAP,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У���
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<MEMORY_DOORGBITMAP_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_DOORGBITMAP_ADDR+i*MEMORY_DOORGBITMAP_SIZE+sizeof(S_MEMORY_DOORGBITMAP)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORG_BITMAP,0);
                }
            }
            break;
        }
        */
        //������λͼ
        case MEMORY_APP_CMD_CARDGBITMAP_R:
        {
            if(Number>=MEMORY_CARDGBITMAP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CARDGBITMAP_ADDR+Number*MEMORY_CARDGBITMAP_SIZE,Buf,sizeof(S_MEMORY_CARDG_BITMAP));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_CardGBitmap_MemoryApp= (S_MEMORY_CARDG_BITMAP*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_CARDG_BITMAP)-2);
            if(i16==p_CardGBitmap_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д����λͼ
        case MEMORY_APP_CMD_CARDGBITMAP_W:
        {
            p_CardGBitmap_MemoryApp = (S_MEMORY_CARDG_BITMAP*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_CardGBitmap_MemoryApp->Number==0)
            {
                p_CardGBitmap_MemoryApp->Number=0xFF;
                p_CardGBitmap_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_CardGBitmap_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_CARDG_BITMAP)-2);
            }
            if(Number>=MEMORY_CARDGBITMAP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_CARDGBITMAP_ADDR+Number*MEMORY_CARDGBITMAP_SIZE,Buf,sizeof(S_MEMORY_CARDG_BITMAP));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDG_BITMAP,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У���
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<MEMORY_CARDGBITMAP_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CARDGBITMAP_ADDR+i*MEMORY_CARDGBITMAP_SIZE+sizeof(S_MEMORY_CARDG_BITMAP)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDG_BITMAP,0);
                }
            }
            break;
        }
        //��������
        case MEMORY_APP_CMD_READER_R:
        {
            if(Number>=MEMORY_READER_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_READER_ADDR+Number*MEMORY_READER_SIZE,Buf,sizeof(S_MEMORY_READER));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Reader_MemoryApp= (S_MEMORY_READER*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_READER)-2);
            if(i16==p_Reader_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д������
        case MEMORY_APP_CMD_READER_W:
        {
            p_Reader_MemoryApp = (S_MEMORY_READER*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_Reader_MemoryApp->Number==0)
            {
                p_Reader_MemoryApp->Number=0xFF;
                p_Reader_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_Reader_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_READER)-2);
            }
            if(Number>=MEMORY_READER_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_READER_ADDR+Number*MEMORY_READER_SIZE,Buf,sizeof(S_MEMORY_READER));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_READER,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У��ָ��
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<MEMORY_READER_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_READER_ADDR+i*MEMORY_READER_SIZE+sizeof(S_MEMORY_READER)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_READER,0);
                }
            }
            break;
        }
        //���̵���
        case MEMORY_APP_CMD_RELAY_R:
        {
            if(Number>=MEMORY_RELAY_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_RELAY_ADDR+Number*MEMORY_RELAY_SIZE,Buf,sizeof(S_MEMORY_RELAY));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Relay_MemoryApp= (S_MEMORY_RELAY*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_RELAY)-2);
            if(i16==p_Relay_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д�̵���
        case MEMORY_APP_CMD_RELAY_W:
        {
            p_Relay_MemoryApp = (S_MEMORY_RELAY*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_Relay_MemoryApp->Number==0)
            {
                p_Relay_MemoryApp->Number=0xFF;
                p_Relay_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_Relay_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_RELAY)-2);
            }
            if(Number>=MEMORY_RELAY_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_RELAY_ADDR+Number*MEMORY_RELAY_SIZE,Buf,sizeof(S_MEMORY_RELAY));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_RELAY,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У��ָ��
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<MEMORY_RELAY_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_RELAY_ADDR+i*MEMORY_RELAY_SIZE+sizeof(S_MEMORY_RELAY)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_RELAY,0);
                }
            }
            break;
        }
        //������
        case MEMORY_APP_CMD_INPUT_R:
        {
            if(Number>=MEMORY_INPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_INPUT_ADDR+Number*MEMORY_INPUT_SIZE,Buf,sizeof(S_MEMORY_INPUT));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Input_MemoryApp= (S_MEMORY_INPUT*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_INPUT)-2);
            if(i16==p_Input_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д����
        case MEMORY_APP_CMD_INPUT_W:
        {
            p_Input_MemoryApp = (S_MEMORY_INPUT*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_Input_MemoryApp->Number==0)
            {
                p_Input_MemoryApp->Number=0xFF;
                p_Input_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_Input_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_INPUT)-2);
            }
            if(Number>=MEMORY_INPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_INPUT_ADDR+Number*MEMORY_INPUT_SIZE,Buf,sizeof(S_MEMORY_INPUT));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_INPUT,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //��У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У��ָ��
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<MEMORY_INPUT_MAX_NUM; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_INPUT_ADDR+i*MEMORY_INPUT_SIZE+sizeof(S_MEMORY_INPUT)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_INPUT,0);
                }
            }
            break;
        }
        //������
        case MEMORY_APP_CMD_ALARM_R:
        {
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_ALARM_ADDR,Buf,sizeof(S_MEMORY_ALARM));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_Alarm_MemoryApp = (S_MEMORY_ALARM*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_ALARM)-2);
            if(i16==p_Alarm_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        }
        //д����
        case MEMORY_APP_CMD_ALARM_W:
        {
            p_Alarm_MemoryApp = (S_MEMORY_ALARM*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            p_Alarm_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_ALARM)-2);
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_ALARM_ADDR,Buf,sizeof(S_MEMORY_ALARM));
            if(res==MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_ALARM,0);
                if(res==MEMORY_APP_ERR_NO)
                {
                    //��ˮ��
                    p_Check_MemoryApp->SerialNumber=SerialNumber;
                    //����У��
                    p_Check_MemoryApp->CheckSum=0;
                    //У��ָ��
                    p_I16=(INT16U*)&Member_I32;
                    *p_I16=0;
                    //
                    for(count=i=0; i<1; i++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_ALARM_ADDR+i*MEMORY_ALARM_SIZE+sizeof(S_MEMORY_ALARM)-2,(INT8U*)p_I16,2);
                        if(res!=OK)break;
                        if((*p_I16!=0xFFFF)&&(*p_I16!=0))
                        {
                            count++;
                            p_Check_MemoryApp->CheckSum+=(*p_I16);
                        }
                    }
                    //��������
                    p_Check_MemoryApp->Number=count;
                    if(res!=OK)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_ALARM,0);
                }
            }
            break;
        }
        //������
        case MEMORY_APP_CMD_DOORGROUP_R:
        {
            //�жϲ���
            if(Number>=MEMORY_DOORGROUP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //��ȡ
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_DOORGROUP_ADDR+Number*MEMORY_DOORGROUP_SIZE,Buf,sizeof(S_MEMORY_DOOR_GROUP));
            if(res==ERR)
            {
                res=MEMORY_APP_ERR_OPERATION;
                break;
            }
            //У��
            p_DoorG_MemoryApp = (S_MEMORY_DOOR_GROUP*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_DOOR_GROUP)-2);
            if(p_DoorG_MemoryApp->Sum != i16)
            {
                res = MEMORY_APP_ERR_SUM;
                break;
            }
            res = MEMORY_APP_ERR_NO;
            break;
        }
        //д����
        case MEMORY_APP_CMD_DOORGROUP_W:
        {
            //�жϲ���
            if(Number>=MEMORY_DOORGROUP_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_DoorG_MemoryApp = (S_MEMORY_DOOR_GROUP*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_DoorG_MemoryApp->Number[0]==0&&p_DoorG_MemoryApp->Number[1]==0)
            {
                p_DoorG_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_DoorG_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_DOOR_GROUP)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORGROUP,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ ,MEMORY_DOORGROUP_ADDR+Number*MEMORY_DOORGROUP_SIZE+sizeof(S_MEMORY_DOOR_GROUP)-2,(INT8U*)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_DoorG_MemoryApp->Number[0]!=0)||(p_DoorG_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_DoorG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_DOORGROUP_ADDR+Number*MEMORY_DOORGROUP_SIZE,Buf,MEMORY_DOORGROUP_SIZE);
            }
            else if((p_DoorG_MemoryApp->Number[0]==0)&&(p_DoorG_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16==0xFFFF)
                {
                    //
                    p_DoorG_MemoryApp->Number[0]=p_DoorG_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_DOORGROUP_ADDR+Number*MEMORY_DOORGROUP_SIZE,Buf,MEMORY_DOORGROUP_SIZE);
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_DoorG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_DOORGROUP_ADDR+Number*MEMORY_DOORGROUP_SIZE,Buf,MEMORY_DOORGROUP_SIZE);
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORGROUP,0);
            break;
        }
        //������
        case MEMORY_APP_CMD_CARDGROUP_LIMIT_R:
        {
#if 0
            //
            if(Number>=MEMORY_CARDGROUPLIMIT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE,Buf,sizeof(S_MEMORY_CARDGROUP_LIMIT));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_CardG = (S_MEMORY_CARDGROUP_LIMIT*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_CARDGROUP_LIMIT)-2);
            if(i16==p_CardG->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
            break;
#endif
            //�жϲ���
            if(Number>=MEMORY_CARDGROUPLIMIT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //��ȡ
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE,Buf,sizeof(S_MEMORY_CARDGROUP_LIMIT));
            if(res==ERR)
            {
                res=MEMORY_APP_ERR_OPERATION;
                break;
            }
            //У��
            p_CardG_MemoryApp = (S_MEMORY_CARDGROUP_LIMIT*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_CARDGROUP_LIMIT)-2);
            if(p_CardG_MemoryApp->Sum != i16)
            {
                res = MEMORY_APP_ERR_SUM;
                break;
            }
            res = MEMORY_APP_ERR_NO;
            break;
        }
        //д����
        case MEMORY_APP_CMD_CARDGROUP_LIMIT_W:
        {
            //�жϲ���
            if(Number>=MEMORY_CARDGROUPLIMIT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_CardG_MemoryApp = (S_MEMORY_CARDGROUP_LIMIT*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_CardG_MemoryApp->Number[0]==0&&p_CardG_MemoryApp->Number[1]==0)
            {
                p_CardG_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_CardG_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_CARDGROUP_LIMIT)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDGROUP,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ ,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE+sizeof(S_MEMORY_CARDGROUP_LIMIT)-2,(INT8U*)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_CardG_MemoryApp->Number[0]!=0)||(p_CardG_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_CardG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE,Buf,MEMORY_CARDGROUPLIMIT_SIZE);
            }
            else if((p_CardG_MemoryApp->Number[0]==0)&&(p_CardG_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    p_CardG_MemoryApp->Number[0]=p_CardG_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE,Buf,MEMORY_CARDGROUPLIMIT_SIZE);
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_CardG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_CARDGROUPLIMIT_ADDR+Number*MEMORY_CARDGROUPLIMIT_SIZE,Buf,MEMORY_CARDGROUPLIMIT_SIZE);
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDGROUP,0);
            break;
        }
        //������
        case MEMORY_APP_CMD_AREA_R:
        {
            //�жϲ���
            if(Number>=MEMORY_AREA_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //��ȡ
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_AREA_ADDR+Number*MEMORY_AREA_SIZE,Buf,sizeof(S_MEMORY_AREA));
            if(res==ERR)
            {
                res=MEMORY_APP_ERR_OPERATION;
                break;
            }
            //У��
            p_Area_MemoryApp = (S_MEMORY_AREA*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_AREA)-2);
            if(p_Area_MemoryApp->Sum != i16)
            {
                res = MEMORY_APP_ERR_SUM;
                break;
            }
            res = MEMORY_APP_ERR_NO;
            break;
        }
        //д����
        case MEMORY_APP_CMD_AREA_W:
        {
            //�жϲ���
            if(Number>=MEMORY_AREA_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_Area_MemoryApp = (S_MEMORY_AREA*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_Area_MemoryApp->Number[0]==0&&p_Area_MemoryApp->Number[1]==0)
            {
                p_Area_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_Area_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_AREA)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_AREA,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ ,MEMORY_AREA_ADDR+Number*MEMORY_AREA_SIZE+sizeof(S_MEMORY_AREA)-2,(INT8U*)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_Area_MemoryApp->Number[0]!=0)||(p_Area_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_Area_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_AREA_ADDR+Number*MEMORY_AREA_SIZE,Buf,MEMORY_AREA_SIZE);
            }
            else if((p_Area_MemoryApp->Number[0]==0)&&(p_Area_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_Area_MemoryApp->Number[0]=p_Area_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_AREA_ADDR+Number*MEMORY_AREA_SIZE,Buf,MEMORY_AREA_SIZE);
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_Area_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_AREA_ADDR+Number*MEMORY_AREA_SIZE,Buf,MEMORY_AREA_SIZE);
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_AREA,0);
            break;
        }
        //��ʱ����
        case MEMORY_APP_CMD_TIMEGROUP_R:
        {
            //�жϲ���
            if(Number>=MEMORY_FLASH_MAX_TIMEGROUP)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //��ȡ
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_TIMEGROUP_ADDR+Number*MEMORY_FLASH_TIMEGROUP_SIZE,Buf,MEMORY_FLASH_TIMEGROUP_SIZE);
            if(res==ERR)
            {
                res=MEMORY_APP_ERR_OPERATION;
                break;
            }
            //У��
            p_TimeG_MemoryApp=(S_MEMORY_TIMEGROUP*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_TIMEGROUP)-2);
            if(p_TimeG_MemoryApp->Sum != i16)
            {
                res = MEMORY_APP_ERR_SUM;
                break;
            }
            res = MEMORY_APP_ERR_NO;
            break;
        }
        //дʱ����
        case MEMORY_APP_CMD_TIMEGROUP_W:
        {
            //�жϲ���
            if(Number>=MEMORY_FLASH_MAX_TIMEGROUP)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_TimeG_MemoryApp=(S_MEMORY_TIMEGROUP*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_TimeG_MemoryApp->Number==0)
            {
                p_TimeG_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_TimeG_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_TIMEGROUP)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_TIMEGROUP,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ ,MEMORY_FLASH_TIMEGROUP_ADDR+Number*MEMORY_FLASH_TIMEGROUP_SIZE+sizeof(S_MEMORY_TIMEGROUP)-2,(INT8U*)p_I16,2);
            if((*p_I16!=0xFFFF)&& p_TimeG_MemoryApp->Number!=0)
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_TimeG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_FLASH_TIMEGROUP_ADDR+Number*MEMORY_FLASH_TIMEGROUP_SIZE,Buf,MEMORY_FLASH_TIMEGROUP_SIZE);
            }
            else if(p_TimeG_MemoryApp->Number==0)
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_TimeG_MemoryApp->Number=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_FLASH_TIMEGROUP_ADDR+Number*MEMORY_FLASH_TIMEGROUP_SIZE,Buf,MEMORY_FLASH_TIMEGROUP_SIZE);
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_TimeG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_FLASH_TIMEGROUP_ADDR+Number*MEMORY_FLASH_TIMEGROUP_SIZE,Buf,MEMORY_FLASH_TIMEGROUP_SIZE);
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_TIMEGROUP,0);
            break;
        }
        //���ڼ�����
        case MEMORY_APP_CMD_HOLIDAYGROUP_R:
        {
            //�жϲ���
            if(Number>=MEMORY_FLASH_MAX_HOLIDAYGROUP)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //��ȡ
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_HOLIDAYGROUP_ADDR+Number*MEMORY_FLASH_HOLIDAYGROUP_SIZE,Buf,MEMORY_FLASH_HOLIDAYGROUP_SIZE);
            if(res==ERR)
            {
                res=MEMORY_APP_ERR_OPERATION;
                break;
            }
            //У��
            p_HolidayG_MemoryApp=(S_MEMORY_HOLIDAYGROUP*)Buf;
            i16 = Count_Sum(Buf,sizeof(S_MEMORY_HOLIDAYGROUP)-2);
            if(p_HolidayG_MemoryApp->Sum != i16)
            {
                res = MEMORY_APP_ERR_SUM;
                break;
            }
            res = MEMORY_APP_ERR_NO;
            break;
        }
        //д�ڼ�����
        case MEMORY_APP_CMD_HOLIDAYGROUP_W:
        {
            //�жϲ���
            if(Number>=MEMORY_FLASH_MAX_HOLIDAYGROUP)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_HolidayG_MemoryApp=(S_MEMORY_HOLIDAYGROUP*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_HolidayG_MemoryApp->Number==0)
            {
                p_HolidayG_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_HolidayG_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_HOLIDAYGROUP)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_HOLIDAYGROUP,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ ,MEMORY_FLASH_HOLIDAYGROUP_ADDR+Number*MEMORY_FLASH_HOLIDAYGROUP_SIZE+sizeof(S_MEMORY_HOLIDAYGROUP)-2,(INT8U*)p_I16,2);
            if((*p_I16!=0xFFFF)&& p_HolidayG_MemoryApp->Number!=0)
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_HolidayG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_FLASH_HOLIDAYGROUP_ADDR+Number*MEMORY_FLASH_HOLIDAYGROUP_SIZE,Buf,MEMORY_FLASH_HOLIDAYGROUP_SIZE);
            }
            else if(p_HolidayG_MemoryApp->Number==0)
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_HolidayG_MemoryApp->Number=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,MEMORY_FLASH_HOLIDAYGROUP_ADDR+Number*MEMORY_FLASH_HOLIDAYGROUP_SIZE,Buf,MEMORY_FLASH_HOLIDAYGROUP_SIZE);
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_HolidayG_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_FLASH_HOLIDAYGROUP_ADDR+Number*MEMORY_FLASH_HOLIDAYGROUP_SIZE,Buf,MEMORY_FLASH_HOLIDAYGROUP_SIZE);
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_HOLIDAYGROUP,0);
            break;
        }
        //���û�Ȩ��
        case MEMORY_APP_CMD_USER_PERMISSION_R:
            //����ͨ�û���
            //---��ȡ����(*p_i32)
            p_i32=(INT32U*)Buf;
            p_User_MemoryApp=(S_MEMORY_USER_PERMISSION*)Buf;
            //�����Ŷ�ȡ
            if(*p_i32!=0)
            {
                //---����β��(i)
                i32=Buf[0];
                i32<<=8;
                i32+=Buf[1];
                i32<<=8;
                i32+=Buf[2];
                i32<<=8;
                i32+=Buf[3];
                i=i32%16;
                //---��ȡ��ǰ��¼����(s_check)
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                if(p_Check_MemoryApp->Number>MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
                {
                    res=MEMORY_APP_ERR_PARA;
                    break;
                }
                //---���ҿ���λ��(i16)
                for(i16=0; i16<p_Check_MemoryApp->Number; i16++)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                             (INT8U*)&Member_J32,4);
                    if(res==ERR)break;
                    if((*p_i32) == Member_J32)
                    {
                        break;
                    }
                    else if(Member_J32==0xFFFFFFFF)
                    {
                        i16=0xFFFF;
                        break;
                    }
                }
                if(res!=MEMORY_APP_ERR_NO)break;
                //---
                if(i16!=p_Check_MemoryApp->Number && i16!=0xFFFF)
                {
                    //�����Ե�ַ��ȡ(д��)�û�Ȩ������
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                             Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                    break;
                }
                //����ʱ�û���
                //---��ȡ����(*p_i32)
                p_i32=(INT32U*)Buf;
                //---��ȡ��ǰ��¼����(s_check)
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
                if(p_Check_MemoryApp->Number>MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
                {
                    res=MEMORY_APP_ERR_PARA;
                    break;
                }
                //---���ҿ���λ��(i16)
                for(i16=0; i16<p_Check_MemoryApp->Number; i16++)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                             (INT8U*)&Member_J32,4);
                    if(res==ERR)break;
                    if((*p_i32) == Member_J32)
                    {
                        break;
                    }
                    else if(Member_J32==0xFFFFFFFF)
                    {
                        i16=0xFFFF;
                        break;
                    }
                }
                if(res!=MEMORY_APP_ERR_NO)break;
                //---
                if(i16!=p_Check_MemoryApp->Number && i16!=0xFFFF)
                {
                    //�����Ե�ַ��ȡ(д��)�û�Ȩ������
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                             Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                    break;
                }
                else
                {
                    res = MEMORY_APP_ERR_PARA;
                    break;
                }

            }
            //�������ȡ
            else if((p_User_MemoryApp->Password[0]!=0||p_User_MemoryApp->Password[1]!=0||p_User_MemoryApp->Password[2]!=0||p_User_MemoryApp->Password[3]!=0))
            {
                //
                if(p_User_MemoryApp->Password[0]==0xFF&&p_User_MemoryApp->Password[1]==0xFF&&p_User_MemoryApp->Password[2]==0xFF&&p_User_MemoryApp->Password[3]==0xFF)
                {
                    res = MEMORY_APP_ERR_PARA;
                    break;
                }
                //�Ȳ���16����ͨ�û���
                for(i=0; i<MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1; i++)
                {
                    //---��ȡ��ǰ��¼����(s_check)
                    p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                    res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                    if(p_Check_MemoryApp->Number>MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
                    {
                        res=MEMORY_APP_ERR_PARA;
                        break;
                    }
                    //---���ҿ���λ��(i16)
                    for(i16=0; i16<p_Check_MemoryApp->Number; i16++)
                    {
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                                 MEMORY_FLASH_USER_PERMISSION_ADDR+
                                                 i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                                 i16*MEMORY_FLASH_USER_PERMISSION_SIZE+(4+2+5+5),
                                                 (INT8U*)Member_PrintBuf,4);
                        if(res==ERR)break;
                        if(i16==127)
                        {
                            i16=127;
                        }
                        if(  (p_User_MemoryApp->Password[0] == Member_PrintBuf[0])
                             &&(p_User_MemoryApp->Password[1] == Member_PrintBuf[1])
                             &&(p_User_MemoryApp->Password[2] == Member_PrintBuf[2])
                             &&(p_User_MemoryApp->Password[3] == Member_PrintBuf[3])
                          )
                        {
                            break;
                        }
                    }
                    if(res!=MEMORY_APP_ERR_NO)break;
                    //---
                    if(i16!=p_Check_MemoryApp->Number)
                    {
                        //�����Ե�ַ��ȡ(д��)�û�Ȩ������
                        res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                                 MEMORY_FLASH_USER_PERMISSION_ADDR+
                                                 i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                                 i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                                 Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                        break;
                    }

                }
                //
                if(i!=(MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1))break;
                //����1����ʱ�ͻ���
                //---��ȡ��ǰ��¼����(s_check)
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
                if(p_Check_MemoryApp->Number>MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
                {
                    res=MEMORY_APP_ERR_PARA;
                    break;
                }
                //---���ҿ���λ��(i16)
                for(i16=0; i16<p_Check_MemoryApp->Number; i16++)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE+(4+2+5+5),
                                             (INT8U*)Member_PrintBuf,4);
                    if(res==ERR)break;
                    if(  (p_User_MemoryApp->Password[0] == Member_PrintBuf[0])
                         &&(p_User_MemoryApp->Password[1] == Member_PrintBuf[1])
                         &&(p_User_MemoryApp->Password[2] == Member_PrintBuf[2])
                         &&(p_User_MemoryApp->Password[3] == Member_PrintBuf[3])
                      )
                    {
                        break;
                    }
                }
                if(res!=MEMORY_APP_ERR_NO)break;
                //---
                if(i16!=p_Check_MemoryApp->Number)
                {
                    //�����Ե�ַ��ȡ(д��)�û�Ȩ������
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_FLASH_USER_PERMISSION_ADDR+
                                             (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                             i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                             Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                    break;
                }
                else
                {
                    res = MEMORY_APP_ERR_VOID_RETURN;
                    break;
                }
            }
            //��������ȡ
            else
            {
                //��ȡ�����(i)
                if(SerialNumber>=MEMORY_FLASH_USER_PERMISSION_GROUP_NUM)
                {
                    res=MEMORY_APP_ERR_PARA;
                    break;
                }
                //��ȡλ��(i16)
                if(Number>=MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
                {
                    res=MEMORY_APP_ERR_PARA;
                    break;
                }
                //��ȡ����
                //�����Ե�ַ��ȡ(д��)�û�Ȩ������
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         SerialNumber*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         Number*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                         Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
            }
            break;
        //д�û�Ȩ��(�½�)
        case MEMORY_APP_CMD_NEW_USER_W:
        {
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //����У��ͼ���
            p_User_MemoryApp=(S_MEMORY_USER_PERMISSION*)Buf;
            p_User_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_USER_PERMISSION)-2);
            //��ȡ����(*p_i32)
            i32=Buf[0];
            i32<<=8;
            i32+=Buf[1];
            i32<<=8;
            i32+=Buf[2];
            i32<<=8;
            i32+=Buf[3];
            //����β��
            i=i32%16;
            //��ȡ��ǰ��¼����
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
            //����
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,
                                     MEMORY_FLASH_USER_PERMISSION_ADDR+
                                     i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                     (p_Check_MemoryApp->Number)*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                     Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
            if(res==ERR)break;


            //����EEPROM����...
            p_Check_MemoryApp->Number += 1;
            p_Check_MemoryApp->CheckSum+=p_User_MemoryApp->Sum;
            p_Check_MemoryApp->SerialNumber=SerialNumber;
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
            break;
        }
        //д�ÿ�Ȩ��(�½�)
        case MEMORY_APP_CMD_NEW_VISITOR_W:
        {
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //����У��ͼ���
            p_User_MemoryApp=(S_MEMORY_USER_PERMISSION*)Buf;
            p_User_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_USER_PERMISSION)-2);
            //��ȡ��ǰ��¼����
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
            //����
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,
                                     MEMORY_FLASH_USER_PERMISSION_ADDR+
                                     (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                     (p_Check_MemoryApp->Number)*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                     Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
            if(res==ERR)break;
            //����EEPROM����...
            p_Check_MemoryApp->Number += 1;
            p_Check_MemoryApp->CheckSum+=p_User_MemoryApp->Sum;
            p_Check_MemoryApp->SerialNumber=SerialNumber;
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
            break;
        }
        //�޸���ͨ�û�
        case MEMORY_APP_CMD_MODY_USER_W:
        {
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //����У��ͼ���
            p_User_MemoryApp=(S_MEMORY_USER_PERMISSION*)Buf;
            p_User_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_USER_PERMISSION)-2);
            //��ȡ����(*p_i32)
            p_i32=(INT32U*)Buf;
            //����β��(i)
            //i=(*p_i32)%16;
            i=p_User_MemoryApp->CardNumber[3]%16;
            //��ȡ��ǰ��¼����(s_check)
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
            //���ҿ���λ��(i16)
            if(p_Check_MemoryApp->Number>=MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
            {
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                res=MEMORY_APP_ERR_PARA;
                break;
            }
            for(i16=0; i16<MEMORY_FLASH_USER_PERMISSION_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                         (INT8U*)&Member_J32,4);
                if(res==ERR)break;
                if((*p_i32) == Member_J32)
                {
                    break;
                }
                else if(Member_J32==0xFFFFFFFF)
                {
                    break;
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                break;
            }
            if(i16>=MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
            {
                //�޿ռ�
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                //
                res = MEMORY_APP_ERR_VOID_RETURN;
            }
            else if(Member_J32==0xFFFFFFFF)
            {
                //��Ҫ����
                res=Memory_InternalApp(MEMORY_APP_CMD_NEW_USER_W,Buf,0,SerialNumber);
            }
            else
            {
                //��ȡǰУ��
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE+sizeof(S_MEMORY_USER_PERMISSION)-2,
                                         (INT8U*)p_I16,2);
                //�޸�
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                         Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                if(res==ERR)break;
                //����EEPROM����...
                p_Check_MemoryApp->Number += 0;
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_User_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
            }
            break;
        }
        //�޸ķÿ�
        case MEMORY_APP_CMD_MODY_VISITOR_W:
        {
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //����У��ͼ���
            p_User_MemoryApp=(S_MEMORY_USER_PERMISSION*)Buf;
            p_User_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_USER_PERMISSION)-2);
            //��ȡ����(*p_i32)
            p_i32=(INT32U*)Buf;
            //��ȡ��ǰ��¼����(s_check)
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
            //���ҿ���λ��(i16)
            if(p_Check_MemoryApp->Number>=MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
            {
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                res=MEMORY_APP_ERR_PARA;
                break;
            }
            for(i16=0; i16<MEMORY_FLASH_USER_PERMISSION_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                         (INT8U*)&Member_J32,4);
                if(res==ERR)break;
                if((*p_i32) == Member_J32)
                {
                    break;
                }
                else if(Member_J32==0xFFFFFFFF)
                {
                    //��Ҫ��������
                    break;
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1+i,0);
                break;
            }
            if(i16>=MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
            {
                //�޿ռ�
                res = MEMORY_APP_ERR_VOID_RETURN;
            }
            else if(Member_J32==0xFFFFFFFF)
            {
                //��Ҫ����
                res=Memory_InternalApp(MEMORY_APP_CMD_NEW_VISITOR_W,Buf,0,SerialNumber);
            }
            else
            {
                //�����޸�����
                //��ȡǰУ��
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE+sizeof(S_MEMORY_USER_PERMISSION)-2,
                                         (INT8U*)p_I16,2);
                //�޸�
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+
                                         (MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+
                                         i16*MEMORY_FLASH_USER_PERMISSION_SIZE,
                                         Buf,MEMORY_FLASH_USER_PERMISSION_SIZE);
                if(res==ERR)break;
                //����EEPROM����...
                p_Check_MemoryApp->Number += 0;
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_User_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber=SerialNumber;
                res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
            }
            break;
        }
        //��ˢ����¼
        case MEMORY_APP_CMD_CARD_RECORD_R:
#if 0
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //��ȡ��ǰ��¼����
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_I16,MEMORY_APP_CHECK_CARD_RECORD,0);
            if(res==ERR)break;
            if((Number+1)>(*p_I16))
            {
                res=ERR;
                break;
            }
            else
            {
                Number = Number%MEMORY_FLASH_MAX_CARD_RECORD;
            }
#else
            Number = Number%MEMORY_FLASH_MAX_CARD_RECORD;
#endif
            //��ȡ��¼
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_CARD_RECORD_ADDR+Number*MEMORY_FLASH_CARD_RECORD_SIZE,Buf,MEMORY_FLASH_CARD_RECORD_SIZE);
            if(Buf[0]==0xFF&&Buf[1]==0xFF)
            {
                res=MEMORY_APP_ERR_PARA;
            }
            else
            {
                res=MEMORY_APP_ERR_NO;
            }
            //
            break;
        //дˢ����¼
        case MEMORY_APP_CMD_CARD_RECORD_W:
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //��ȡ��ǰ��¼����
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_I16,MEMORY_APP_CHECK_CARD_RECORD,0);
            if(res==ERR)break;
            //�鿴�Ƿ�Խ��
            if((*p_I16)%(MEMORY_FLASH_MIN_ERASE_SIZE/MEMORY_FLASH_CARD_RECORD_SIZE)==0)
            {
                //Խ��:������������
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,MEMORY_FLASH_CARD_RECORD_ADDR+(((*p_I16)%MEMORY_FLASH_MAX_CARD_RECORD)*MEMORY_FLASH_CARD_RECORD_SIZE),0,0);
            }
            //��֤
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_CARD_RECORD_ADDR+((*p_I16)%MEMORY_FLASH_MAX_CARD_RECORD)*MEMORY_FLASH_CARD_RECORD_SIZE,MemberBuf,MEMORY_FLASH_CARD_RECORD_SIZE);
            if(res==ERR)break;
            for(i16=0; i16<MEMORY_FLASH_CARD_RECORD_SIZE; i16++)
            {
                if(MemberBuf[i16]!=0xFF)break;
            }
            if(i16!=MEMORY_FLASH_CARD_RECORD_SIZE)
            {
                //���¼����¼����,������
                ;
            }
            p_card_record_MemoryApp=(S_MEMORY_CARD_RECORD*)Buf;
            //p_card_record_MemoryApp->Number[0]=(*p_I16)>>8;
            //p_card_record_MemoryApp->Number[1]=(*p_I16);
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_FLASH_CARD_RECORD_ADDR+((*p_I16)%MEMORY_FLASH_MAX_CARD_RECORD)*MEMORY_FLASH_CARD_RECORD_SIZE,Buf,MEMORY_FLASH_CARD_RECORD_SIZE);
            if(res==ERR)break;
            //���¼�¼����
            (*p_I16)=(*p_I16)%MEMORY_FLASH_MAX_CARD_RECORD;
            (*p_I16)++;
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_I16,MEMORY_APP_CHECK_CARD_RECORD,0);
            break;
        //��������¼
        case MEMORY_APP_CMD_ALARM_RECORD_R:
#if 0
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //��ȡ��ǰ��¼����
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_I16,MEMORY_APP_CHECK_ALARM_RECORD,0);
            if(res==ERR)break;
            if((Number+1)>(*p_I16))
            {
                res=ERR;
                break;
            }
            else
            {
                Number = Number%MEMORY_FLASH_MAX_ALARM_RECORD;
            }
#else
            Number = Number%MEMORY_FLASH_MAX_ALARM_RECORD;
#endif
            //��ȡ��¼
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_ALARM_RECORD_ADDR+Number*MEMORY_FLASH_ALARM_RECORD_SIZE,Buf,MEMORY_FLASH_ALARM_RECORD_SIZE);
            if(Buf[0]==0xFF&&Buf[1]==0xFF)
            {
                res=MEMORY_APP_ERR_PARA;
            }
            else
            {
                res=MEMORY_APP_ERR_NO;
            }
            //
            break;
        //д������¼
        case MEMORY_APP_CMD_ALARM_RECORD_W:
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //��ȡ��ǰ��¼����
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_I16,MEMORY_APP_CHECK_ALARM_RECORD,0);
            if(res==ERR)break;
            //�鿴�Ƿ�Խ��
            if((*p_I16)%(MEMORY_FLASH_MIN_ERASE_SIZE/MEMORY_FLASH_ALARM_RECORD_SIZE)==0)
            {
                //Խ��:������������
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,MEMORY_FLASH_ALARM_RECORD_ADDR+(((*p_I16)%MEMORY_FLASH_MAX_ALARM_RECORD)*MEMORY_FLASH_ALARM_RECORD_SIZE),0,0);
            }
            //��֤
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_ALARM_RECORD_ADDR+((*p_I16)%MEMORY_FLASH_MAX_ALARM_RECORD)*MEMORY_FLASH_ALARM_RECORD_SIZE,MemberBuf,MEMORY_FLASH_ALARM_RECORD_SIZE);
            if(res==ERR)break;
            for(i16=0; i16<MEMORY_FLASH_ALARM_RECORD_SIZE; i16++)
            {
                if(MemberBuf[i16]!=0xFF)break;
            }
            if(i16!=MEMORY_FLASH_ALARM_RECORD_SIZE)
            {
                //���¼����¼����,������
                ;
            }
            p_alarm_record_MemoryApp=(S_MEMORY_ALARM_RECORD*)Buf;
            //p_alarm_record_MemoryApp->Number[0]=(*p_I16)>>8;
            //p_alarm_record_MemoryApp->Number[1]=(*p_I16);
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,MEMORY_FLASH_ALARM_RECORD_ADDR+((*p_I16)%MEMORY_FLASH_MAX_ALARM_RECORD)*MEMORY_FLASH_ALARM_RECORD_SIZE,Buf,MEMORY_FLASH_ALARM_RECORD_SIZE);
            if(res==ERR)break;
            //���¼�¼����
            (*p_I16)=(*p_I16)%MEMORY_FLASH_MAX_ALARM_RECORD;
            (*p_I16)++;
            res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_I16,MEMORY_APP_CHECK_ALARM_RECORD,0);
            break;
        //�����������ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_R:
            if(Number>=MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_INOUTAND_ADDR+Number*MEMORY_LOGICBLOCK_INOUTAND_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_LogicBlockInOutAnd_MemoryApp = (S_MEMORY_LOGICBLOCK_INOUTAND*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND)-2);
            if(i16==p_LogicBlockInOutAnd_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        //д���������ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_W:
            //�жϲ���
            if(Number>=MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_LogicBlockInOutAnd_MemoryApp = (S_MEMORY_LOGICBLOCK_INOUTAND*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_LogicBlockInOutAnd_MemoryApp->Number==0)
            {
                p_LogicBlockInOutAnd_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_LogicBlockInOutAnd_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_INOUTAND_ADDR+Number*MEMORY_LOGICBLOCK_INOUTAND_SIZE+sizeof(S_MEMORY_LOGICBLOCK_INOUTAND)-2,(INT8U *)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_LogicBlockInOutAnd_MemoryApp->Number!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_LogicBlockInOutAnd_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                //
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INOUTAND_ADDR+Number*MEMORY_LOGICBLOCK_INOUTAND_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
            }
            else if(p_LogicBlockInOutAnd_MemoryApp->Number==0)
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_LogicBlockInOutAnd_MemoryApp->Number=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INOUTAND_ADDR+Number*MEMORY_LOGICBLOCK_INOUTAND_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_LogicBlockInOutAnd_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INOUTAND_ADDR+Number*MEMORY_LOGICBLOCK_INOUTAND_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,0);
            break;    
        /*    
        //���߼�ģ��֮����ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_INPUT_R:
            if(Number>=MEMORY_LOGICBLOCK_INPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_INPUT_ADDR+Number*MEMORY_LOGICBLOCK_INPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_LogicBlockInput_MemoryApp = (S_MEMORY_LOGICBLOCK_INPUT*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT)-2);
            if(i16==p_LogicBlockInput_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        //д�߼�ģ��֮����ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_INPUT_W:
            //�жϲ���
            if(Number>=MEMORY_LOGICBLOCK_INPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_LogicBlockInput_MemoryApp = (S_MEMORY_LOGICBLOCK_INPUT*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_LogicBlockInput_MemoryApp->Number[0]==0&&p_LogicBlockInput_MemoryApp->Number[1]==0)
            {
                p_LogicBlockInput_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_LogicBlockInput_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INPUT,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_INPUT_ADDR+Number*MEMORY_LOGICBLOCK_INPUT_SIZE+sizeof(S_MEMORY_LOGICBLOCK_INPUT)-2,(INT8U *)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_LogicBlockInput_MemoryApp->Number[0]!=0)||(p_LogicBlockInput_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_LogicBlockInput_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                //
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INPUT_ADDR+Number*MEMORY_LOGICBLOCK_INPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
            }
            else if((p_LogicBlockInput_MemoryApp->Number[0]==0)&&(p_LogicBlockInput_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_LogicBlockInput_MemoryApp->Number[0]=p_LogicBlockInput_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INPUT_ADDR+Number*MEMORY_LOGICBLOCK_INPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_LogicBlockInput_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_INPUT_ADDR+Number*MEMORY_LOGICBLOCK_INPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INPUT,0);
            break;
        //���߼�ģ��֮�߼�ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_LOGIC_R:
            if(Number>=MEMORY_LOGICBLOCK_LOGIC_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_LOGIC_ADDR+Number*MEMORY_LOGICBLOCK_LOGIC_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_LogicBlockLogic_MemoryApp = (S_MEMORY_LOGICBLOCK_LOGIC*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC)-2);
            if(i16==p_LogicBlockLogic_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        //д�߼�ģ��֮�߼�ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_LOGIC_W:
            //�жϲ���
            if(Number>=MEMORY_LOGICBLOCK_LOGIC_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_LogicBlockLogic_MemoryApp = (S_MEMORY_LOGICBLOCK_LOGIC*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_LogicBlockLogic_MemoryApp->Number[0]==0&&p_LogicBlockLogic_MemoryApp->Number[1]==0)
            {
                p_LogicBlockLogic_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_LogicBlockLogic_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_LOGIC_ADDR+Number*MEMORY_LOGICBLOCK_LOGIC_SIZE+sizeof(S_MEMORY_LOGICBLOCK_LOGIC)-2,(INT8U *)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_LogicBlockLogic_MemoryApp->Number[0]!=0)||(p_LogicBlockLogic_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_LogicBlockLogic_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                //
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_LOGIC_ADDR+Number*MEMORY_LOGICBLOCK_LOGIC_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
            }
            else if((p_LogicBlockLogic_MemoryApp->Number[0]==0)&&(p_LogicBlockLogic_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_LogicBlockLogic_MemoryApp->Number[0]=p_LogicBlockLogic_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_LOGIC_ADDR+Number*MEMORY_LOGICBLOCK_LOGIC_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_LogicBlockLogic_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_LOGIC_ADDR+Number*MEMORY_LOGICBLOCK_LOGIC_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,0);
            break;
        //���߼�ģ��֮���ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_R:
            if(Number>=MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_OUTPUT_ADDR+Number*MEMORY_LOGICBLOCK_OUTPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
            if(res==ERR)
            {
                res = MEMORY_APP_ERR_OPERATION;
                break;
            }
            p_LogicBlockOutput_MemoryApp = (S_MEMORY_LOGICBLOCK_OUTPUT*)Buf;
            i16=Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT)-2);
            if(i16==p_LogicBlockOutput_MemoryApp->Sum)
            {
                res=MEMORY_APP_ERR_NO;
                break;
            }
            else
            {
                res=MEMORY_APP_ERR_SUM;
                break;
            }
        //д�߼�ģ��֮���ģ��
        case  MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_W:
            //�жϲ���
            if(Number>=MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM)
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            p_LogicBlockOutput_MemoryApp = (S_MEMORY_LOGICBLOCK_OUTPUT*)Buf;
            //���Ϊɾ��ָ��,У��̶�дΪ0xFFFF
            if(p_LogicBlockOutput_MemoryApp->Number[0]==0&&p_LogicBlockOutput_MemoryApp->Number[1]==0)
            {
                p_LogicBlockOutput_MemoryApp->Sum = 0xFFFF;
            }
            else
            {
                p_LogicBlockOutput_MemoryApp->Sum = Count_Sum(Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT)-2);
            }
            //������У���
            p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
            //У��ָ��
            p_I16=(INT16U*)&Member_I32;
            *p_I16=0;
            //
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,0);
            if(res!=OK)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_LOGICBLOCK_OUTPUT_ADDR+Number*MEMORY_LOGICBLOCK_OUTPUT_SIZE+sizeof(S_MEMORY_LOGICBLOCK_OUTPUT)-2,(INT8U *)p_I16,2);
            if((*p_I16!=0xFFFF)&& ((p_LogicBlockOutput_MemoryApp->Number[0]!=0)||(p_LogicBlockOutput_MemoryApp->Number[1]!=0)))
            {
                //�޸�
                p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                p_Check_MemoryApp->CheckSum+=p_LogicBlockOutput_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                //
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_OUTPUT_ADDR+Number*MEMORY_LOGICBLOCK_OUTPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
            }
            else if((p_LogicBlockOutput_MemoryApp->Number[0]==0)&&(p_LogicBlockOutput_MemoryApp->Number[1]==0))
            {
                //ɾ��
                //(����Ч�����������)
                if(*p_I16!=0xFFFF)
                {
                    //
                    p_LogicBlockOutput_MemoryApp->Number[0]=p_LogicBlockOutput_MemoryApp->Number[1]=0xFF;
                    p_Check_MemoryApp->CheckSum = p_Check_MemoryApp->CheckSum-(*p_I16);
                    p_Check_MemoryApp->Number--;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_OUTPUT_ADDR+Number*MEMORY_LOGICBLOCK_OUTPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
                }
                p_Check_MemoryApp->SerialNumber = SerialNumber;
            }
            else
            {
                //�½�
                p_Check_MemoryApp->Number++;
                p_Check_MemoryApp->CheckSum+=p_LogicBlockOutput_MemoryApp->Sum;
                p_Check_MemoryApp->SerialNumber = SerialNumber;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_LOGICBLOCK_OUTPUT_ADDR+Number*MEMORY_LOGICBLOCK_OUTPUT_SIZE,Buf,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
            }
            //д��У���
            res=Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,0);
            break;
        */    
        //������
        case  MEMORY_APP_CMD_REVERSE_R:
        //д����
        case  MEMORY_APP_CMD_REVERSE_W:
            //---��ȡ����(*p_i32)
            p_Reverse_MemoryApp=(S_MEMORY_REVERSE*)Buf;
            //����У��
            if(Number>=MEMORY_REVERSEG_NUM
               ||(p_Reverse_MemoryApp->CardNumber[0]==0x00&&p_Reverse_MemoryApp->CardNumber[1]==0x00&&p_Reverse_MemoryApp->CardNumber[2]==0x00&&p_Reverse_MemoryApp->CardNumber[3]==0x00)
               ||(p_Reverse_MemoryApp->CardNumber[0]==0xFF&&p_Reverse_MemoryApp->CardNumber[1]==0xFF&&p_Reverse_MemoryApp->CardNumber[2]==0xFF&&p_Reverse_MemoryApp->CardNumber[3]==0xFF))
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //�����Ŷ�ȡ
            p_I8=(INT8U*)Member_PrintBuf;
            //---���ҿ���λ��(i16)
            for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,
                                         MEMORY_REVERSE_ADDR+
                                         Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+
                                         i16*MEMORY_REVERSE_SIZE,
                                         p_I8,4);
                if(res==ERR)break;
                if((p_Reverse_MemoryApp->CardNumber[0] == p_I8[0])
                   &&(p_Reverse_MemoryApp->CardNumber[1] == p_I8[1])
                   &&(p_Reverse_MemoryApp->CardNumber[2] == p_I8[2])
                   &&(p_Reverse_MemoryApp->CardNumber[3] == p_I8[3]))
                {
                    break;
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                //��ȷ��
                p_Reverse_MemoryApp->State=0;
            }
            else if(i16==MEMORY_REVERSE_MAX_NUM)
            {
                //������
                p_Reverse_MemoryApp->State=2;
            }
            else
            {
                //����
                p_Reverse_MemoryApp->State=1;
            }
            //д
            if(Cmd==MEMORY_APP_CMD_REVERSE_W&&p_Reverse_MemoryApp->State==2)
            {
                //�ҳ�������
                //---���ҿ���λ��(i16)
                for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,
                                             MEMORY_REVERSE_ADDR+
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+
                                             i16*MEMORY_REVERSE_SIZE,
                                             p_I8,4);
                    if(res==ERR)break;
                    if((0xFF == p_I8[0])&&(0xFF == p_I8[1])&&(0xFF == p_I8[2])&&(0xFF == p_I8[3]))
                    {
                        break;
                    }
                }
                //
                if(i16>=MEMORY_REVERSE_MAX_NUM)
                {
                    //����������
                    p_Reverse_MemoryApp->State=3;
                }
                else
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,
                                             MEMORY_REVERSE_ADDR+
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+
                                             i16*MEMORY_REVERSE_SIZE,
                                             &(p_Reverse_MemoryApp->CardNumber[0]),4);
                    if(res==OK)
                    {
                        //����
                        p_Reverse_MemoryApp->State=1;
                    }
                    else
                    {
                        //δ֪
                        p_Reverse_MemoryApp->State=0;
                    }
                }
            }
            break;

        //�巴��
        case  MEMORY_APP_CMD_REVERSE_C:
            //---��ȡ����(*p_i32)
            p_Reverse_MemoryApp=(S_MEMORY_REVERSE*)Buf;
            //����У��
            if(Number>=MEMORY_REVERSEG_NUM
               ||(p_Reverse_MemoryApp->CardNumber[0]==0x00&&p_Reverse_MemoryApp->CardNumber[1]==0x00&&p_Reverse_MemoryApp->CardNumber[2]==0x00&&p_Reverse_MemoryApp->CardNumber[3]==0x00)
               //||(p_Reverse->CardNumber[0]==0xFF&&p_Reverse->CardNumber[1]==0xFF&&p_Reverse->CardNumber[2]==0xFF&&p_Reverse->CardNumber[3]==0xFF)
              )
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //�ж�������ɾ�����ǵ���ɾ��
            if(p_Reverse_MemoryApp->CardNumber[0]==0xFF&&p_Reverse_MemoryApp->CardNumber[1]==0xFF&&p_Reverse_MemoryApp->CardNumber[2]==0xFF&&p_Reverse_MemoryApp->CardNumber[3]==0xFF)
            {
                //�������
                memset(MemberBuf,0xFF,MEMORY_REVERSE_SIZE);
                for(i16=0; i16<+MEMORY_REVERSE_MAX_NUM; i16++)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_REVERSE_ADDR+Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+i16*MEMORY_REVERSE_SIZE,MemberBuf,MEMORY_REVERSE_SIZE);
                }
                if(res==ERR)
                {
                    p_Reverse_MemoryApp->State=0;
                }
                else
                {
                    p_Reverse_MemoryApp->State=2;
                }
                break;
            }
            //�����Ŷ�ȡ
            p_I8=(INT8U*)Member_PrintBuf;
            //---���ҿ���λ��(i16)
            for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_REVERSE_ADDR+Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+i16*MEMORY_REVERSE_SIZE,p_I8,4);
                if(res==ERR)break;
                if((p_Reverse_MemoryApp->CardNumber[0] == p_I8[0])
                   &&(p_Reverse_MemoryApp->CardNumber[1] == p_I8[1])
                   &&(p_Reverse_MemoryApp->CardNumber[2] == p_I8[2])
                   &&(p_Reverse_MemoryApp->CardNumber[3] == p_I8[3]))
                {
                    p_I8[0]=p_I8[1]=p_I8[2]=p_I8[3]=0x00;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,\
                                             MEMORY_REVERSE_ADDR+\
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+\
                                             i16*MEMORY_REVERSE_SIZE,\
                                             p_I8,4);
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                //��ȷ��
                p_Reverse_MemoryApp->State=0;
            }
            else
            {
                //������
                p_Reverse_MemoryApp->State=2;
            }
            //
            break;
#if 0
        //������
        case  MEMORY_APP_CMD_REVERSE_R:
            //---��ȡ����(*p_i32)
            p_Reverse_MemoryApp=(S_MEMORY_REVERSE*)Buf;
            //����У��
            if(Number>=MEMORY_REVERSEG_NUM
               ||(p_Reverse_MemoryApp->CardNumber[0]==0x00&&p_Reverse_MemoryApp->CardNumber[1]==0x00&&p_Reverse_MemoryApp->CardNumber[2]==0x00&&p_Reverse_MemoryApp->CardNumber[3]==0x00)
               ||(p_Reverse_MemoryApp->CardNumber[0]==0xFF&&p_Reverse_MemoryApp->CardNumber[1]==0xFF&&p_Reverse_MemoryApp->CardNumber[2]==0xFF&&p_Reverse_MemoryApp->CardNumber[3]==0xFF))
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //�����Ŷ�ȡ
            p_I8=(INT8U*)Member_PrintBuf;
            //---���ҿ���λ��(i16)
            for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                         MEMORY_REVERSE_ADDR+
                                         Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+
                                         i16*MEMORY_REVERSE_SIZE,
                                         p_I8,4);
                if(res==ERR)break;
                if((p_Reverse_MemoryApp->CardNumber[0] == p_I8[0])
                   &&(p_Reverse_MemoryApp->CardNumber[1] == p_I8[1])
                   &&(p_Reverse_MemoryApp->CardNumber[2] == p_I8[2])
                   &&(p_Reverse_MemoryApp->CardNumber[3] == p_I8[3]))
                {
                    break;
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                //��ȷ��
                p_Reverse_MemoryApp->State=0;
            }
            else if(i16==MEMORY_REVERSE_MAX_NUM)
            {
                //������
                p_Reverse_MemoryApp->State=2;
            }
            else
            {
                //����
                p_Reverse_MemoryApp->State=1;
            }
            //
            break;
        //д����
        case MEMORY_APP_CMD_REVERSE_W:
            //---��ȡ����(*p_i32)
            p_Reverse_MemoryApp=(S_MEMORY_REVERSE*)Buf;
            //����У��
            if(Number>=MEMORY_REVERSEG_NUM
               ||(p_Reverse_MemoryApp->CardNumber[0]==0x00&&p_Reverse_MemoryApp->CardNumber[1]==0x00&&p_Reverse_MemoryApp->CardNumber[2]==0x00&&p_Reverse_MemoryApp->CardNumber[3]==0x00)
               ||(p_Reverse_MemoryApp->CardNumber[0]==0xFF&&p_Reverse_MemoryApp->CardNumber[1]==0xFF&&p_Reverse_MemoryApp->CardNumber[2]==0xFF&&p_Reverse_MemoryApp->CardNumber[3]==0xFF))
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //�����Ŷ�ȡ
            p_I8=(INT8U*)Member_PrintBuf;
            //���ҿ�д�ռ�(����i16)
            for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_REVERSE_ADDR+Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+i16*MEMORY_REVERSE_SIZE,p_I8,4);
                if(res==ERR)break;
                if((p_Reverse_MemoryApp->CardNumber[0] == p_I8[0])
                   &&(p_Reverse_MemoryApp->CardNumber[1] == p_I8[1])
                   &&(p_Reverse_MemoryApp->CardNumber[2] == p_I8[2])
                   &&(p_Reverse_MemoryApp->CardNumber[3] == p_I8[3]))
                {
                    //����
                    i16=0xFFFF;
                    break;
                }
                else if((p_I8[0] == 0xFF)
                        &&(p_I8[1] == 0xFF)
                        &&(p_I8[2] == 0xFF)
                        &&(p_I8[3] == 0xFF))
                {
                    //��¼��дλ��
                    break;
                }
            }
            //�ж�
            if(i16==0xFFFF)
            {
                //�Ѿ����ڴ˺�,���ô���
                p_Reverse_MemoryApp->State=1;
            }
            else if(i16==MEMORY_REVERSE_MAX_NUM)
            {
                p_Reverse_MemoryApp->State=2;
                //û�д洢�ռ�,��Ҫ������Ƭ
                for(i=0; i<((MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE)/MEMORY_FLASH_MAX_WRITE_SIZE); i++)
                {
                    //��ȡ
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,
                                             MEMORY_REVERSE_ADDR+
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE,
                                             (INT8U*)&Member_SBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                    //�޸�����
                    for(i=0; i<(256/4); i++)
                    {
                        if(Member_SBuf[i]==0x00000000)
                        {
                            if(p_Reverse_MemoryApp->State!=1)
                            {
                                p_Reverse_MemoryApp->State=1;
                                Member_SBuf[i]=p_Reverse_MemoryApp->CardNumber[0];
                                Member_SBuf[i]<<=8;
                                Member_SBuf[i]=p_Reverse_MemoryApp->CardNumber[1];
                                Member_SBuf[i]<<=8;
                                Member_SBuf[i]=p_Reverse_MemoryApp->CardNumber[2];
                                Member_SBuf[i]<<=8;
                                Member_SBuf[i]=p_Reverse_MemoryApp->CardNumber[3];
                            }
                            else
                            {
                                Member_SBuf[i]=0xFFFFFFFF;
                            }
                        }
                    }
                    //����д��
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,
                                             MEMORY_REVERSE_ADDR+
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE,
                                             (INT8U*)&Member_SBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                }
                //�жϻ����Ƿ�����
                if(p_Reverse_MemoryApp->State!=1)
                {
                    //����
                    p_Reverse_MemoryApp->State=2;
                }
            }
            else
            {
                //������д��
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,
                                         MEMORY_REVERSE_ADDR+
                                         Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+
                                         i16*MEMORY_REVERSE_SIZE,
                                         &(p_Reverse_MemoryApp->CardNumber[0]),4);
                p_Reverse_MemoryApp->State=1;
            }
            break;
        //�巴��
        case MEMORY_APP_CMD_REVERSE_C:
            //---��ȡ����(*p_i32)
            p_Reverse_MemoryApp=(S_MEMORY_REVERSE*)Buf;
            //����У��
            if(Number>=MEMORY_REVERSEG_NUM
               ||(p_Reverse_MemoryApp->CardNumber[0]==0x00&&p_Reverse_MemoryApp->CardNumber[1]==0x00&&p_Reverse_MemoryApp->CardNumber[2]==0x00&&p_Reverse_MemoryApp->CardNumber[3]==0x00)
               //||(p_Reverse->CardNumber[0]==0xFF&&p_Reverse->CardNumber[1]==0xFF&&p_Reverse->CardNumber[2]==0xFF&&p_Reverse->CardNumber[3]==0xFF)
              )
            {
                res = MEMORY_APP_ERR_PARA;
                break;
            }
            //�ж�������ɾ�����ǵ���ɾ��
            if(p_Reverse_MemoryApp->CardNumber[0]==0xFF&&p_Reverse_MemoryApp->CardNumber[1]==0xFF&&p_Reverse_MemoryApp->CardNumber[2]==0xFF&&p_Reverse_MemoryApp->CardNumber[3]==0xFF)
            {
                //�����ȡ
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,
                                         MEMORY_REVERSE_ADDR+
                                         Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE,
                                         0,0);
                if(res==ERR)
                {
                    p_Reverse_MemoryApp->State=0;
                }
                else
                {
                    p_Reverse_MemoryApp->State=2;
                }
                break;
            }
            //�����Ŷ�ȡ
            p_I8=(INT8U*)Member_PrintBuf;
            //---���ҿ���λ��(i16)
            for(i16=0; i16<MEMORY_REVERSE_MAX_NUM; i16++)
            {
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_REVERSE_ADDR+Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+i16*MEMORY_REVERSE_SIZE,p_I8,4);
                if(res==ERR)break;
                if((p_Reverse_MemoryApp->CardNumber[0] == p_I8[0])
                   &&(p_Reverse_MemoryApp->CardNumber[1] == p_I8[1])
                   &&(p_Reverse_MemoryApp->CardNumber[2] == p_I8[2])
                   &&(p_Reverse_MemoryApp->CardNumber[3] == p_I8[3]))
                {
                    p_I8[0]=p_I8[1]=p_I8[2]=p_I8[3]=0x00;
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_MODIFICATION,\
                                             MEMORY_REVERSE_ADDR+\
                                             Number*MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE+\
                                             i16*MEMORY_REVERSE_SIZE,\
                                             p_I8,4);
                }
            }
            if(res!=MEMORY_APP_ERR_NO)
            {
                //��ȷ��
                p_Reverse_MemoryApp->State=0;
            }
            else
            {
                //������
                p_Reverse_MemoryApp->State=2;
            }
            //
            break;
#endif
        case MEMORY_APP_CMD_FORMAT:
            memset(MemberBuf,0xFF,256);
            i16=0;
            if(Number==MEMORY_FORMAT_ALL)
            {
                //��EEPROM
                for(i32=0; i32<MEMORY_EEPROM_MAX_ADDR; i32+=256)
                {
                    res = Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,256);
                    if(res==ERR)
                    {
                        res = MEMORY_APP_ERR_OPERATION;
                        break;
                    }
                }
                if(res!=MEMORY_APP_ERR_NO)break;
                //дEEPROMͷ
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EEPROM_HEAD_ADDR,(INT8U*)MEMORY_PRODUCT_HEAD,strlen(MEMORY_PRODUCT_HEAD));
                if(res==ERR)
                {
                    res = MEMORY_APP_ERR_OPERATION;
                    break;
                }
                //д�汾
                {
                    Memory_EnterFunction(MEMORY_ENTRY_CMD_VAR_LOAD,0,0,0);
                    p_Ver_MemoryApp = (S_MEMORY_VER*)Member_SBuf;
                    memset((INT8U*)p_Ver_MemoryApp,0,sizeof(S_MEMORY_VER));
                    p_Ver_MemoryApp->ProductionDate=MEMORY_PRODUCTION_DATE;
                    p_Ver_MemoryApp->SerialNumber=MEMORY_PRODUCT_SERIAL_NUMBER;
                    p_Ver_MemoryApp->ProductCode=MEMORY_PRODUCT_CODE;
                    p_Ver_MemoryApp->HardwareVer=MEMORY_HARDWARE_VER;
                    p_Ver_MemoryApp->SoftwareVer=MEMORY_SOFTWARE_VER;
                    p_Ver_MemoryApp->MemoryMainVer=MEMORY_MAIN_VER;
                    p_Ver_MemoryApp->MemorySubVer=MEMORY_SUB_VER;
                    p_Ver_MemoryApp->addr=Memory_MachineAddr;
                    p_Ver_MemoryApp->BaudRate=Memory_UartBaudRate;
                    p_Ver_MemoryApp->Type = MEMORY_PRODUCT_TYPE;
                    p_Ver_MemoryApp->Sum=Count_Sum((INT8U*)p_Ver_MemoryApp,sizeof(S_MEMORY_VER)-2);
                    res = Memory_InternalApp(MEMORY_APP_CMD_VER_W,(INT8U*)p_Ver_MemoryApp,0,0);
                    if(res!=MEMORY_APP_ERR_NO)
                    {
                        break;
                    }
                }
                //��ʼ������������
                {
                    p_KeepPara_MemoryApp=(S_MEMORY_KEEPPARA*)Member_SBuf;
                    memset((INT8U*)p_KeepPara_MemoryApp,0xFF,sizeof(S_MEMORY_KEEPPARA));
                    p_KeepPara_MemoryApp->TransferAlarmTxNum=0;
                    p_KeepPara_MemoryApp->TransferAlarmTxLastNum=0;
                    p_KeepPara_MemoryApp->TransferEventTxNum=0;
                    p_KeepPara_MemoryApp->TransferEventTxLastNum=0;
                    p_KeepPara_MemoryApp->MonitorSign[0]=0;
                    p_KeepPara_MemoryApp->MonitorSign[1]=0;
                    p_KeepPara_MemoryApp->MonitorSign[2]=0;
                    p_KeepPara_MemoryApp->MonitorSign[3]=0;
                    p_KeepPara_MemoryApp->PowerDownCount=0;
                    p_KeepPara_MemoryApp->Sum=Count_Sum((INT8U*)p_KeepPara_MemoryApp,sizeof(S_MEMORY_KEEPPARA)-2);
                    res = Memory_InternalApp(MEMORY_APP_CMD_KEEPPARA_W,(INT8U*)p_KeepPara_MemoryApp,0,0);
                    if(res!=MEMORY_APP_ERR_NO)
                    {
                        break;
                    }
                }
                //дУ��
                {
                    p_MemoryCheck_MemoryApp=(S_MEMORY_CHECK*)Member_SBuf;
                    memset((INT8U*)p_MemoryCheck_MemoryApp,0x00,sizeof(S_MEMORY_CHECK));
                    p_MemoryCheck_MemoryApp->Sum=Count_Sum((INT8U*)p_MemoryCheck_MemoryApp,sizeof(S_MEMORY_CHECK)-2);
                    res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_MemoryCheck_MemoryApp,MEMORY_APP_CHECK_ALL,0);
                    if(res!=MEMORY_APP_ERR_NO)
                    {
                        break;
                    }
                }
                //д����
                {
                    p_Control_MemoryApp=(S_MEMORY_CONTROL*)Member_SBuf;
                    memset((INT8U*)p_Control_MemoryApp,0x00,sizeof(S_MEMORY_CONTROL));
                    p_Control_MemoryApp->DeviceEnable=ENABLE;
                    p_Control_MemoryApp->Sum=Count_Sum((INT8U*)p_Control_MemoryApp,sizeof(S_MEMORY_CONTROL)-2);
                    res = Memory_InternalApp(MEMORY_APP_CMD_CONTROL_W,(INT8U*)p_Control_MemoryApp,0,0);
                    if(res!=MEMORY_APP_ERR_NO)
                    {
                        break;
                    }
                }
                //��FLASH
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_CHIP_ERASE,0,0,0);
                    if(res==ERR)
                    {
                        res = MEMORY_APP_ERR_OPERATION;
                        break;
                    }
                }
                //д��Ϣ
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,0,(INT8U*)MEMORY_PRODUCT_HEAD,strlen(MEMORY_PRODUCT_HEAD));
                    if(res==ERR)
                    {
                        res = MEMORY_APP_ERR_OPERATION;
                        break;
                    }
                }
                break;
            }
            if(Number&MEMORY_FORMAT_ALARM_RECORD)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,MEMORY_FLASH_ALARM_RECORD_ADDR,0,(MEMORY_FLASH_ALARM_RECORD_ADDR_END-MEMORY_FLASH_ALARM_RECORD_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_ALARM_RECORD_ADDR; i32<MEMORY_FLASH_ALARM_RECORD_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_I16=(INT16U*)&Member_I32;
                *p_I16=0;
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_I16,MEMORY_APP_CHECK_ALARM_RECORD,0);
            }
            if(Number&MEMORY_FORMAT_CARD_RECORD)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,MEMORY_FLASH_CARD_RECORD_ADDR,0,(MEMORY_FLASH_CARD_RECORD_ADDR_END-MEMORY_FLASH_CARD_RECORD_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_CARD_RECORD_ADDR; i32<MEMORY_FLASH_CARD_RECORD_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_I16=(INT16U*)&Member_I32;
                *p_I16=0;
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_I16,MEMORY_APP_CHECK_CARD_RECORD,0);
            }
            if(Number&MEMORY_FORMAT_VISITOR)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+16*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+16*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+16*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_VISITOR,0);
            }
            if(Number&MEMORY_FORMAT_USER1)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+0*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+0*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+0*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER1,0);
            }
            if(Number&MEMORY_FORMAT_USER2)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+1*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+1*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+1*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER2,0);
            }
            if(Number&MEMORY_FORMAT_USER3)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+2*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+2*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+2*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER3,0);
            }
            if(Number&MEMORY_FORMAT_USER4)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+3*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+3*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+3*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER4,0);
            }
            if(Number&MEMORY_FORMAT_USER5)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+4*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+4*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+4*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER5,0);
            }
            if(Number&MEMORY_FORMAT_USER6)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+5*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+5*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+5*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER6,0);
            }
            if(Number&MEMORY_FORMAT_USER7)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+6*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+6*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+6*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER7,0);
            }
            if(Number&MEMORY_FORMAT_USER8)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+7*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+7*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+7*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER8,0);
            }
            if(Number&MEMORY_FORMAT_USER9)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+8*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+8*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+8*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER9,0);
            }
            if(Number&MEMORY_FORMAT_USER10)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+9*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+9*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+9*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER10,0);
            }
            if(Number&MEMORY_FORMAT_USER11)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+10*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+10*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+10*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER11,0);
            }
            if(Number&MEMORY_FORMAT_USER12)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+11*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+11*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+11*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER12,0);
            }
            if(Number&MEMORY_FORMAT_USER13)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+12*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+12*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+12*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER13,0);
            }
            if(Number&MEMORY_FORMAT_USER14)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+13*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+13*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+13*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER14,0);
            }
            if(Number&MEMORY_FORMAT_USER15)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+14*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+14*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+14*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER15,0);
            }
            if(Number&MEMORY_FORMAT_USER16)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_USER_PERMISSION_ADDR+15*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                         (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE-1)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_USER_PERMISSION_ADDR+15*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32<MEMORY_FLASH_USER_PERMISSION_ADDR+15*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE+MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE;
                    i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARD_USER16,0);
            }
            if(Number&MEMORY_FORMAT_HOLIDAYG)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_HOLIDAYGROUP_ADDR,0,
                                         (MEMORY_FLASH_HOLIDAYGROUP_ADDR_END-MEMORY_FLASH_HOLIDAYGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_HOLIDAYGROUP_ADDR; i32<MEMORY_FLASH_HOLIDAYGROUP_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_HOLIDAYGROUP,0);
            }
            if(Number&MEMORY_FORMAT_TIMEG)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_FLASH_TIMEGROUP_ADDR,0,
                                         (MEMORY_FLASH_TIMEGROUP_ADDR_END-MEMORY_FLASH_TIMEGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_FLASH_TIMEGROUP_ADDR; i32<MEMORY_FLASH_TIMEGROUP_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_TIMEGROUP,0);
            }
            if(Number&MEMORY_FORMAT_DOOR)
            {
                //������
                for(i32=MEMORY_DOOR_ADDR; i32<MEMORY_DOOR_ADDR_END; i32+=MEMORY_DOOR_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_DOOR_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOOR,0);
            }
            if(Number&MEMORY_FORMAT_DOORG)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_DOORGROUP_ADDR,0,
                                         (MEMORY_DOORGROUP_ADDR_END-MEMORY_DOORGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_DOORGROUP_ADDR; i32<MEMORY_DOORGROUP_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORGROUP,0);
            }
            if(Number&MEMORY_FORMAT_CARDG)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_CARDGROUPLIMIT_ADDR,0,
                                         (MEMORY_CARDGROUPLIMIT_ADDR_END-MEMORY_CARDGROUPLIMIT_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_CARDGROUPLIMIT_ADDR; i32<MEMORY_CARDGROUPLIMIT_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDGROUP,0);
            }
            if(Number&MEMORY_FORMAT_AREA)
            {
                //������
#ifdef MEMORY_FLASH_FAST_ERASE_ENABLE
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                         MEMORY_AREA_ADDR,0,
                                         (MEMORY_AREA_ADDR_END-MEMORY_AREA_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
#else
                for(i32=MEMORY_AREA_ADDR; i32<MEMORY_AREA_ADDR_END; i32+=MEMORY_FLASH_MIN_ERASE_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                }
#endif
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_AREA,0);
            }
            /*
            if(Number&MEMORY_FORMAT_DOORGB)
            {
                //������
                for(i32=MEMORY_DOORGBITMAP_ADDR; i32<MEMORY_DOORGBITMAP_ADDR_END; i32+=MEMORY_DOORGBITMAP_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_DOORGBITMAP_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_DOORG_BITMAP,0);
            }
            */
            if(Number&MEMORY_FORMAT_CARDGB)
            {
                //������
                for(i32=MEMORY_CARDGBITMAP_ADDR; i32<MEMORY_CARDGBITMAP_ADDR_END; i32+=MEMORY_CARDGBITMAP_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_CARDGBITMAP_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_CARDG_BITMAP,0);
            }
            if(Number&MEMORY_FORMAT_READER)
            {
                //������
                for(i32=MEMORY_READER_ADDR; i32<MEMORY_READER_ADDR_END; i32+=MEMORY_READER_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_READER_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_READER,0);
            }
            if(Number&MEMORY_FORMAT_RELAY)
            {
                //������
                for(i32=MEMORY_RELAY_ADDR; i32<MEMORY_RELAY_ADDR_END; i32+=MEMORY_RELAY_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_RELAY_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_RELAY,0);
            }
            if(Number&MEMORY_FORMAT_INPUT)
            {
                //������
                for(i32=MEMORY_INPUT_ADDR; i32<MEMORY_INPUT_ADDR_END; i32+=MEMORY_INPUT_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_INPUT_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_INPUT,0);
            }
            if(Number&MEMORY_FORMAT_INOUTAND)
            {
                //������
                for(i32=MEMORY_LOGICBLOCK_INOUTAND_ADDR; i32<MEMORY_LOGICBLOCK_INOUTAND_ADDR_END; i32+=MEMORY_LOGICBLOCK_INOUTAND_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_LOGICBLOCK_INOUTAND_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,0);
            }
            /*
            if(Number&MEMORY_FORMAT_LOGICBLOCK)
            {
                //������
                for(i32=MEMORY_LOGICBLOCK_INPUT_ADDR; i32<MEMORY_LOGICBLOCK_INPUT_ADDR_END; i32+=MEMORY_LOGICBLOCK_INPUT_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_LOGICBLOCK_INPUT_SIZE);
                }
                for(i32=MEMORY_LOGICBLOCK_LOGIC_ADDR; i32<MEMORY_LOGICBLOCK_LOGIC_ADDR_END; i32+=MEMORY_LOGICBLOCK_LOGIC_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_LOGICBLOCK_LOGIC_SIZE);
                }
                for(i32=MEMORY_LOGICBLOCK_OUTPUT_ADDR; i32<MEMORY_LOGICBLOCK_OUTPUT_ADDR_END; i32+=MEMORY_LOGICBLOCK_OUTPUT_SIZE)
                {
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_LOGICBLOCK_OUTPUT_SIZE);
                }
                //��У��
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_INPUT,0);
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,0);
                p_Check_MemoryApp=(S_CHECK*)Member_SBuf;
                memset((INT8U*)p_Check_MemoryApp,0x00,sizeof(S_CHECK));
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_Check_MemoryApp,MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,0);
            }
            */            
            break;
        case MEMORY_APP_CMD_INIT:
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_INIT,0,0,0);
            if(res==ERR)break;
            res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_INIT,0,0,0);
            if(res==ERR)break;
            //��ȡ�����FLASH,�鿴�Ƿ���Ч,�����Ч,����и�ʽ��
            {
                //��֤����ͷ
                memset(Member_PrintBuf,0,10);
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_EEPROM_HEAD_ADDR,(INT8U*)Member_PrintBuf,strlen(MEMORY_PRODUCT_HEAD));
                if(0!=strcmp((const char*)Member_PrintBuf,MEMORY_PRODUCT_HEAD))
                {
                    res = Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
                    res = MEMORY_APP_ERR_FIRST;
                    break;
                }
                //��֤FLASHͷ
                memset(Member_PrintBuf,0,10);
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,0,(INT8U*)Member_PrintBuf,strlen(MEMORY_PRODUCT_HEAD));
                if(0!=strcmp((const char*)Member_PrintBuf,MEMORY_PRODUCT_HEAD))
                {
                    res = Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
                    res = MEMORY_APP_ERR_FIRST;
                    break;
                }
            }
            //�������ϵ����
            {
                p_KeepPara_MemoryApp = (S_MEMORY_KEEPPARA*)Member_SBuf;
                Memory_InternalApp(MEMORY_APP_CMD_KEEPPARA_R,(INT8U*)p_KeepPara_MemoryApp,0,0);
                if(p_KeepPara_MemoryApp->PowerDownCount<0xFF)
                {
                    p_KeepPara_MemoryApp->PowerDownCount+=1;
                }
                else
                {
                    p_KeepPara_MemoryApp->PowerDownCount=0;
                }
                Memory_InternalApp(MEMORY_APP_CMD_KEEPPARA_W,(INT8U*)p_KeepPara_MemoryApp,0,0);
            }
            //��ȡ������,���Ƿ���������Ҫ�ָ�
            //��EEPROM��־-->addr
            {
                p_EepromFlash_MemoryApp = (S_MEMORY_EEPROM_FLASH*)Member_SBuf;
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,MEMORY_EEPROM_FLASH_ADDR,(INT8U*)p_EepromFlash_MemoryApp,sizeof(S_MEMORY_EEPROM_FLASH));
                if(res==ERR)break;
                memcpy(Member_PrintBuf,MEMORY_EEPROM_FLASH_HEAD_STR,6);
                Member_PrintBuf[5]=0;
                p_EepromFlash_MemoryApp->Head[5]=0;
                if(0!=strcmp((const char*)Member_PrintBuf,(const char*)&(p_EepromFlash_MemoryApp->Head[0])))
                {
                    break;
                }
                i16=Count_Sum((INT8U*)p_EepromFlash_MemoryApp,sizeof(S_MEMORY_EEPROM_FLASH)-2);
                if(i16!=p_EepromFlash_MemoryApp->Sum)
                {
                    break;
                }
                //��ȡ���ݿ��
                i=p_EepromFlash_MemoryApp->SaveBlockNum;
                if(i>=MEMORY_FLASH_MAX_MODIFICATION)
                {
                    break;
                }
                //��ȡ��ַ
                i32=p_EepromFlash_MemoryApp->FalshAppAddr;
                //����Ӧ������
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,i32,0,0);
                if(res==ERR)break;
                //������-->Ӧ����
                for(j32=i32; j32<(i32+MEMORY_FLASH_MIN_ERASE_SIZE); j32+=MEMORY_FLASH_MAX_WRITE_SIZE)
                {
                    //��ȡ����
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,MEMORY_FLASH_MODIFICATION_ADDR+i*MEMORY_FLASH_MODIFICATION_SIZE+(j32-i32),MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                    if(res==ERR)break;
                    //������-->Ӧ����
                    res=Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,j32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                    if(res==ERR)break;
                }
                //��EEPROM
                memset((INT8U*)p_EepromFlash_MemoryApp,0xFF,sizeof(S_MEMORY_EEPROM_FLASH));
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,MEMORY_EEPROM_FLASH_ADDR,(INT8U*)p_EepromFlash_MemoryApp,sizeof(S_MEMORY_EEPROM_FLASH));
                if(res==ERR)break;
            }
            break;
        default:
            break;
    }
    //�ͷŻ���
    MemManager_Free(Member_SBuf,&i);
    //
    return res;
}
//-------------------------------------------------------------------------------
//��������: ���ڲ�������װΪ�ⲿ��
//-------------------------------------------------------------------------------
INT8U Memory_AppFunction(INT16U Cmd,INT8U *Buf,INT32U Number,INT32U SerialNumber)
{
    INT8U res;
#ifdef UCOS
    /*
    INT8U err;
    OSSemPend(Sem_EepromFlashUse,0,&err);
    */
#endif
    res = Memory_InternalApp(Cmd,Buf,Number,SerialNumber);
#ifdef UCOS
    /*
    OSSemPost(Sem_EepromFlashUse);
    */
#endif
    return res;
}
//-------------------------------------------------------------------------------
//                     ���Ժ���---���ڲ��Ը��������ڼ���״̬���Ƿ����
//˵��: 1,ֻ��UartBaudRate=3ʱ(Ŀǰ������Ϊ115200),��ִ��
//      2,ִ�й��˺�����,���Զ�ִ�д洢����ʽ������(����ֻ����Memory_TEST_ADDR)
//-------------------------------------------------------------------------------
void Memory_TEST(INT32U cmd)
{
    INT8U sign_exe=FALSE;
    INT8U res;
    INT8U i;
    INT8U *p;
    INT16U i16,j16,k16,m16;
    INT32U i32,j32;
    INT32U *MemberTestBuf;
#ifdef UCOS
    INT8U err;
    OSSemPend(Sem_EepromFlashUse,0,&err);
#endif
    //���뻺��
    MemberTestBuf = MemManager_Get(256,&res);
    //-----��ַ�ռ����
    if(cmd & Memory_TEST_READ_BASEINFO)
    {
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Addr Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        sprintf((char *)Member_PrintBuf,"EEPROM Begin Addr: %08ld\r\n",MEMORY_EEPROM_HEAD_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Ver   Addr: %08ld\r\n",MEMORY_VER_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Check Addr: %08ld\r\n",MEMORY_CHECK_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Contr Addr: %08ld\r\n",MEMORY_CONTROL_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Door  Addr: %08ld\r\n",MEMORY_DOOR_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM DooGB Addr: %08ld\r\n",MEMORY_DOORGBITMAP_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM CrdGB Addr: %08ld\r\n",MEMORY_CARDGBITMAP_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM ReaderAddr: %08ld\r\n",MEMORY_READER_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Relay Addr: %08ld\r\n",MEMORY_RELAY_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Input Addr: %08ld\r\n",MEMORY_INPUT_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Alarm Addr: %08ld\r\n",MEMORY_ALARM_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Flash Addr: %08ld\r\n",MEMORY_EEPROM_FLASH_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM InOut Addr: %08ld\r\n",MEMORY_LOGICBLOCK_INOUTAND_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM REVER Addr: %08ld\r\n",MEMORY_REVERSE_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM End   Addr: %08ld\r\n",MEMORY_EERPOM_END_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"EEPROM Max   Addr: %08ld\r\n",MEMORY_EEPROM_MAX_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH  Head  Addr: %08ld\r\n",MEMORY_FLASH_HEAD_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH  User  Addr: %08ld\r\n",MEMORY_FLASH_USER_PERMISSION_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH CardR  Addr: %08ld\r\n",MEMORY_FLASH_CARD_RECORD_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH AlarmR Addr: %08ld\r\n",MEMORY_FLASH_ALARM_RECORD_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH TimeG  Addr: %08ld\r\n",MEMORY_FLASH_TIMEGROUP_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH HdayG  Addr: %08ld\r\n",MEMORY_FLASH_HOLIDAYGROUP_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH Mody.. Addr: %08ld\r\n",MEMORY_FLASH_MODIFICATION_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH DoorG  Addr: %08ld\r\n",MEMORY_DOORGROUP_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH CardG  Addr: %08ld\r\n",MEMORY_CARDGROUPLIMIT_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH Area   Addr: %08ld\r\n",MEMORY_AREA_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH ReverseAddr: %08ld\r\n",MEMORY_REVERSE_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH End    Addr: %08ld\r\n",MEMORY_FLASH_END_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"FLASH Max    Addr: %08ld\r\n",MEMORY_FLASH_MAX_ADDR);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        p_Ver = (S_MEMORY_VER*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_VER_R,(INT8U*)p_Ver,0,0);
        sprintf((char*)Member_PrintBuf,"  ��������:%02d��%02d��\r\n",(p_Ver->ProductionDate)>>8,(p_Ver->ProductionDate)&0x00FF);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��Ʒ���:%05d\r\n",p_Ver->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��Ʒ����:%08ld\r\n",p_Ver->ProductCode);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  Ӳ���汾:%05d\r\n",p_Ver->HardwareVer);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ����汾:%05d\r\n",p_Ver->SoftwareVer);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �洢����:%05d\r\n",p_Ver->MemoryMainVer);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �洢�Ӱ�:%05d\r\n",p_Ver->MemorySubVer);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �豸����:%03d\r\n",p_Ver->Type);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �豸��ַ:%03d\r\n",p_Ver->addr);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ͨѶ����:%05d\r\n",p_Ver->BaudRate);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //
        p_Control = (S_MEMORY_CONTROL*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CONTROL_R,(INT8U*)p_Control,0,0);
        sprintf((char*)Member_PrintBuf,"  �豸ʹ��:%03d\r\n",p_Control->DeviceEnable);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������Τ���ź�ʹ��λͼ:0x%x\r\n",p_Control->ReaderEnableBitmap);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��������������ʹ��λͼ:0x%x\r\n",p_Control->ReaderPasswordEnableBitmap);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������׼��ʱ��ʹ��λͼ:0x%x\r\n",p_Control->ReaderTimeGroupEnableBitmap);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �������࿨����ʹ��λͼ:0x%x\r\n",p_Control->ReaderManyCardOpenDoorEnableBitmap);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //��������
        p_KeepPara = (S_MEMORY_KEEPPARA*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_KEEPPARA_R,(INT8U*)p_KeepPara,0,0);
        sprintf((char*)Member_PrintBuf,"  �豸��������:%03d\r\n",p_KeepPara->PowerDownCount);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //ͨѶ����
        sprintf((char*)Member_PrintBuf,"  Modbus������ַ��ЧͨѶ����: Tx(%ld)Rx(%ld)\r\n",ModbusTxCount,ModbusRxCount);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Addr End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
    }
    //-----ֻ��У��
    if(cmd & Memory_TEST_READ_CHECK)
    {
        p_MemoryCheck = (S_MEMORY_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_MemoryCheck,MEMORY_APP_CHECK_ALL,0);
        sprintf((char*)Member_PrintBuf,"  ˢ����¼����:%05d,������¼����:%05d\r\n",p_MemoryCheck->CardRecordNumber,p_MemoryCheck->AlarmRecordNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ����ˮ  :%05ld,������  :%05d,��У��  :%05d\r\n",\
                p_MemoryCheck->Door.SerialNumber,p_MemoryCheck->Door.Number,p_MemoryCheck->Door.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������ˮ:%05ld,��������:%05d,����У��:%05d\r\n",\
                p_MemoryCheck->DoorGroup.SerialNumber,p_MemoryCheck->DoorGroup.Number,p_MemoryCheck->DoorGroup.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������ˮ:%05ld,��������:%05d,����У��:%05d\r\n",\
                p_MemoryCheck->CardGroup.SerialNumber,p_MemoryCheck->CardGroup.Number,p_MemoryCheck->CardGroup.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������ˮ:%05ld,��������:%05d,����У��:%05d\r\n",\
                p_MemoryCheck->Alarm.SerialNumber,p_MemoryCheck->Alarm.Number,p_MemoryCheck->Alarm.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������ˮ:%05ld,��������:%05d,����У��:%05d\r\n",\
                p_MemoryCheck->Area.SerialNumber,p_MemoryCheck->Area.Number,p_MemoryCheck->Area.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ������ˮ:%05ld,��������:%05d,����У��:%05d\r\n",\
                p_MemoryCheck->HolidayGroup.SerialNumber,p_MemoryCheck->HolidayGroup.Number,p_MemoryCheck->HolidayGroup.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ʱ����ˮ:%05ld,ʱ������:%05d,ʱ��У��:%05d\r\n",\
                p_MemoryCheck->TimeGroup.SerialNumber,p_MemoryCheck->TimeGroup.Number,p_MemoryCheck->TimeGroup.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �ÿ���ˮ:%05ld,�ÿ�����:%05d,�ÿ�У��:%05d\r\n",\
                p_MemoryCheck->Card_Visitor.SerialNumber,p_MemoryCheck->Card_Visitor.Number,p_MemoryCheck->Card_Visitor.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��01��ˮ:%05ld,��01����:%05d,��01У��:%05d\r\n",\
                p_MemoryCheck->Card_User[0].SerialNumber,p_MemoryCheck->Card_User[0].Number,p_MemoryCheck->Card_User[0].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��02��ˮ:%05ld,��02����:%05d,��02У��:%05d\r\n",\
                p_MemoryCheck->Card_User[1].SerialNumber,p_MemoryCheck->Card_User[1].Number,p_MemoryCheck->Card_User[1].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��03��ˮ:%05ld,��03����:%05d,��03У��:%05d\r\n",\
                p_MemoryCheck->Card_User[2].SerialNumber,p_MemoryCheck->Card_User[2].Number,p_MemoryCheck->Card_User[2].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��04��ˮ:%05ld,��04����:%05d,��04У��:%05d\r\n",\
                p_MemoryCheck->Card_User[3].SerialNumber,p_MemoryCheck->Card_User[3].Number,p_MemoryCheck->Card_User[3].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��05��ˮ:%05ld,��05����:%05d,��05У��:%05d\r\n",\
                p_MemoryCheck->Card_User[4].SerialNumber,p_MemoryCheck->Card_User[4].Number,p_MemoryCheck->Card_User[4].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��06��ˮ:%05ld,��06����:%05d,��06У��:%05d\r\n",\
                p_MemoryCheck->Card_User[5].SerialNumber,p_MemoryCheck->Card_User[5].Number,p_MemoryCheck->Card_User[5].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��07��ˮ:%05ld,��07����:%05d,��07У��:%05d\r\n",\
                p_MemoryCheck->Card_User[6].SerialNumber,p_MemoryCheck->Card_User[6].Number,p_MemoryCheck->Card_User[6].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��08��ˮ:%05ld,��08����:%05d,��08У��:%05d\r\n",\
                p_MemoryCheck->Card_User[7].SerialNumber,p_MemoryCheck->Card_User[7].Number,p_MemoryCheck->Card_User[7].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��09��ˮ:%05ld,��09����:%05d,��09У��:%05d\r\n",\
                p_MemoryCheck->Card_User[8].SerialNumber,p_MemoryCheck->Card_User[8].Number,p_MemoryCheck->Card_User[8].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��10��ˮ:%05ld,��10����:%05d,��10У��:%05d\r\n",\
                p_MemoryCheck->Card_User[9].SerialNumber,p_MemoryCheck->Card_User[9].Number,p_MemoryCheck->Card_User[9].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��11��ˮ:%05ld,��11����:%05d,��11У��:%05d\r\n",\
                p_MemoryCheck->Card_User[10].SerialNumber,p_MemoryCheck->Card_User[10].Number,p_MemoryCheck->Card_User[10].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��12��ˮ:%05ld,��12����:%05d,��12У��:%05d\r\n",\
                p_MemoryCheck->Card_User[11].SerialNumber,p_MemoryCheck->Card_User[11].Number,p_MemoryCheck->Card_User[11].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��13��ˮ:%05ld,��13����:%05d,��13У��:%05d\r\n",\
                p_MemoryCheck->Card_User[12].SerialNumber,p_MemoryCheck->Card_User[12].Number,p_MemoryCheck->Card_User[12].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��14��ˮ:%05ld,��14����:%05d,��14У��:%05d\r\n",\
                p_MemoryCheck->Card_User[13].SerialNumber,p_MemoryCheck->Card_User[13].Number,p_MemoryCheck->Card_User[13].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��15��ˮ:%05ld,��15����:%05d,��15У��:%05d\r\n",\
                p_MemoryCheck->Card_User[14].SerialNumber,p_MemoryCheck->Card_User[14].Number,p_MemoryCheck->Card_User[14].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ��16��ˮ:%05ld,��16����:%05d,��16У��:%05d\r\n",\
                p_MemoryCheck->Card_User[15].SerialNumber,p_MemoryCheck->Card_User[15].Number,p_MemoryCheck->Card_User[15].CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //
        sprintf((char*)Member_PrintBuf,"  ������  :%05ld,������  :%05d,������  :%05d\r\n",\
                p_MemoryCheck->Reader.SerialNumber,p_MemoryCheck->Reader.Number,p_MemoryCheck->Reader.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  �̵���  :%05ld,�̵���  :%05d,�̵���  :%05d\r\n",\
                p_MemoryCheck->Relay.SerialNumber,p_MemoryCheck->Relay.Number,p_MemoryCheck->Relay.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ����    :%05ld,����    :%05d,����    :%05d\r\n",\
                p_MemoryCheck->Input.SerialNumber,p_MemoryCheck->Input.Number,p_MemoryCheck->Input.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ����λͼ:%05ld,����λͼ:%05d,����λͼ:%05d\r\n",\
                p_MemoryCheck->DoorGBitmap.SerialNumber,p_MemoryCheck->DoorGBitmap.Number,p_MemoryCheck->DoorGBitmap.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        sprintf((char*)Member_PrintBuf,"  ����λͼ:%05ld,����λͼ:%05d,����λͼ:%05d\r\n",\
                p_MemoryCheck->CardGBitmap.SerialNumber,p_MemoryCheck->CardGBitmap.Number,p_MemoryCheck->CardGBitmap.CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ��������Ϣ
    if(cmd & Memory_TEST_READ_DOORG)
    {
        i16=j16=0;
        p_DoorG = (S_MEMORY_DOOR_GROUP*)MemberTestBuf;
        for(k16=0; k16<MEMORY_DOORGROUP_MAX_NUM; k16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_DOORGROUP_R,(INT8U*)p_DoorG,k16,0);
            if(p_DoorG->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_DoorG->Sum;
                m16=p_DoorG->Number[0];
                m16<<=8;
                m16+=p_DoorG->Number[1];
                sprintf((char*)Member_PrintBuf,"  ����%03d: ���-%03d,У��-%05d\r\n",k16+1,m16,p_DoorG->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            }
        }
        //
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOORGROUP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ��������Ϣ
    if(cmd & Memory_TEST_READ_CARDG)
    {
        i16=j16=0;
        p_CardG = (S_MEMORY_CARDGROUP_LIMIT*)MemberTestBuf;
        for(k16=0; k16<MEMORY_CARDGROUPLIMIT_MAX_NUM; k16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CARDGROUP_LIMIT_R,(INT8U*)p_CardG,k16,0);
            if(p_CardG->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_CardG->Sum;
                m16=p_CardG->Number[0];
                m16<<=8;
                m16+=p_CardG->Number[1];
                sprintf((char*)Member_PrintBuf,"  ����%05d: ���-%05d,У��-%05d\r\n",k16+1,m16,p_CardG->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ������1-%03d ������2-%03d ������3-%03d ������4-%03d\r\n",
                        p_CardG->AreaGroupNumber[0],
                        p_CardG->AreaGroupNumber[1],
                        p_CardG->AreaGroupNumber[2],
                        p_CardG->AreaGroupNumber[3]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��·ID    -%05d\r\n",p_CardG->WayId[0]*256+p_CardG->WayId[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��Ǳ��ѡ��-%03d\r\n",p_CardG->RedioSAB);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ʱ������  -%03d\r\n",p_CardG->TimeGroupNumber);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            }
        }
        //
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARDGROUP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ��������Ϣ
    if(cmd & Memory_TEST_READ_AREA)
    {
        i16=j16=0;
        p_Area = (S_MEMORY_AREA*)MemberTestBuf;
        for(k16=0; k16<MEMORY_AREA_MAX_NUM; k16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_Area,k16,0);
            if(p_Area->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_Area->Sum;
                m16=p_Area->Number[0];
                m16<<=8;
                m16+=p_Area->Number[1];
                sprintf((char*)Member_PrintBuf,"  ����%05d: ���-%03d,У��-%05d\r\n",k16+1,m16,p_Area->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ������ID    -%05d\r\n",p_Area->AreaGroupNumber);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ʱ����ID    -%03d\r\n",p_Area->TimeGroupNumber);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��ȫ�ȼ�    -%03d\r\n",p_Area->SafetyLevel);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��������    -%05d\r\n",p_Area->PeopleMax[0]*256+p_Area->PeopleMax[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��������    -%05d\r\n",p_Area->PeopleMin[0]*256+p_Area->PeopleMin[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ����λͼ    -[%02x]\r\n",p_Area->FunctionSelect);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"      ��������    -%05d\r\n",p_Area->InterlockArea[0]*256+p_Area->InterlockArea[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            }
        }
        //
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_AREA,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ���ڼ�����
    if(cmd & Memory_TEST_READ_HOLIDAYGROUP)
    {
        i16=j16=0;
        p_HolidayG = (S_MEMORY_HOLIDAYGROUP*)MemberTestBuf;
        for(k16=0; k16<MEMORY_FLASH_MAX_HOLIDAYGROUP; k16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_HOLIDAYGROUP_R,(INT8U*)p_HolidayG,k16,0);
            if(p_HolidayG->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_HolidayG->Sum;
                sprintf((char*)Member_PrintBuf,"  �ڼ�����%03d: ���-%03d,У��-%05d\r\n",k16+1,p_HolidayG->Number,p_HolidayG->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                for(i=0; i<12; i++)
                {
                    sprintf((char*)Member_PrintBuf,"  %02d��:[%02x][%02x][%02x][%02x]\r\n",i+1,p_HolidayG->DayBitMap[i*4+3],p_HolidayG->DayBitMap[i*4+2],p_HolidayG->DayBitMap[i*4+1],p_HolidayG->DayBitMap[i*4]);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                }
            }
        }
        //
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_HOLIDAYGROUP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ��ʱ����
    if(cmd & Memory_TEST_READ_TIMEGROUP)
    {
        i16=j16=0;
        p_TimeG = (S_MEMORY_TIMEGROUP*)MemberTestBuf;
        for(k16=0; k16<MEMORY_FLASH_MAX_TIMEGROUP; k16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_TIMEGROUP_R,(INT8U*)p_TimeG,k16,0);
            if(p_TimeG->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_TimeG->Sum;
                sprintf((char*)Member_PrintBuf,"  ʱ������%03d: ���-%03d,�ڼ���-%03d,У��-%05d\r\n",k16+1,p_TimeG->Number,p_TimeG->HolidayGroupNumber,p_TimeG->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                for(i=0; i<7; i++)
                {
                    sprintf((char*)Member_PrintBuf,"  ��%d:[%02x][%02x][%02x][%02x][%02x][%02x]\r\n",i,p_TimeG->TimeBitmap[i][5],p_TimeG->TimeBitmap[i][4],p_TimeG->TimeBitmap[i][3],p_TimeG->TimeBitmap[i][2],p_TimeG->TimeBitmap[i][1],p_TimeG->TimeBitmap[i][0]);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                }
            }
        }
        //
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_TIMEGROUP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----ֻ���û�
    if(cmd & Memory_TEST_READ_USER)
    {
        for(i=0; i<MEMORY_FLASH_USER_PERMISSION_GROUP_NUM; i++)
        {
            //
            if(i==(MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1))
            {
                sprintf((char*)Member_PrintBuf,"  �ÿͿ���%03d\r\n",i+1);
            }
            else
            {
                sprintf((char*)Member_PrintBuf,"  ����%03d\r\n",i+1);
            }
            DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            //
            i16=j16=0;
            p_User = (S_MEMORY_USER_PERMISSION*)MemberTestBuf;
            for(k16=0; ; k16++)
            {
                memset(p_User,0,sizeof(S_MEMORY_USER_PERMISSION));
                p_User->CardNumber[0]=0;
                p_User->CardNumber[1]=0;
                p_User->CardNumber[2]=0;
                p_User->CardNumber[3]=0;
                Memory_InternalApp(MEMORY_APP_CMD_USER_PERMISSION_R,(INT8U*)p_User,k16,i);
                if(p_User->Sum!=0xFFFF)
                {
                    i16++;
                    j16+=p_User->Sum;
                    i32=p_User->CardNumber[0];
                    i32<<=8;
                    i32+=p_User->CardNumber[1];
                    i32<<=8;
                    i32+=p_User->CardNumber[2];
                    i32<<=8;
                    i32+=p_User->CardNumber[3];
                    sprintf((char*)Member_PrintBuf,"  ���%03d: ����-%05ld,����-%05d,����-0x%02x%02x%02x%02x,��Ƭ״̬[%02x],������[%02x],����ѡ��[%02x%02x],ͨ�еȼ�[%02x],в�ȵȼ�[%02x],У��-%05d\r\n",
                            k16+1,
                            i32&0x0000FFFF,
                            p_User->CardGroupNumber[0]*256+p_User->CardGroupNumber[1],
                            p_User->Password[0],
                            p_User->Password[1],
                            p_User->Password[2],
                            p_User->Password[3],
                            p_User->Status,
                            p_User->Type,
                            p_User->Privilege[0],
                            p_User->Privilege[1],
                            p_User->PassLevel,
                            p_User->ForceLevel,
                            p_User->Sum);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                    TimeTypeConvert(2,&Memory_tm,p_User->EffectBeginTime);
                    TimeTypeConvert(2,&Memory_tm1,p_User->EffectEndTime);
                    sprintf((char*)Member_PrintBuf,"            ��Чʱ��:%04d-%02d-%02d  ʧЧʱ��:%04d-%02d-%02d\r\n",
                            Memory_tm.tm_year,
                            Memory_tm.tm_mon+1,
                            Memory_tm.tm_mday,
                            Memory_tm1.tm_year,
                            Memory_tm1.tm_mon+1,
                            Memory_tm1.tm_mday);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                }
                else
                {
                    break;
                }
            }
            //
            sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            //��ȡ����
            p_Check = (S_CHECK*)MemberTestBuf;
            if(i==(MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1))
            {
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARD_VISITOR,0);
            }
            else
            {
                Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARD_USER1+i,0);
            }
            sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        }
    }
#ifdef STM32
    //-----��д����
    if(cmd & Memory_TEST_WR_TYPE)
    {
        //---------------------�汾
        p_Ver=(S_MEMORY_VER*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Ver Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        memset((INT8U*)p_Ver,0x00,sizeof(S_MEMORY_VER));
        p=(INT8U*)p_Ver;
        for(i=0; i<sizeof(S_MEMORY_VER); i++)
        {
            p[i]=i+1;
        }
        Memory_InternalApp(MEMORY_APP_CMD_VER_W,(INT8U*)p_Ver,0,0);
        memset((INT8U*)p_Ver,0x00,sizeof(S_MEMORY_VER));
        res = Memory_InternalApp(MEMORY_APP_CMD_VER_R,(INT8U*)p_Ver,0,0);
        //�ȶ�
        for(i=0; i<MEMORY_VER_SIZE-2; i++)
        {
            if(p[i]==(i+1));
            else break;
        }
        if(i==MEMORY_VER_SIZE-2 && MEMORY_VER_SIZE==sizeof(S_MEMORY_VER) && res==MEMORY_APP_ERR_NO)
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Ver Ok!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Ver Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Ver End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------У����Ϣ
        p_MemoryCheck=(S_MEMORY_CHECK*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Check Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        memset((INT8U*)p_MemoryCheck,0x00,sizeof(S_MEMORY_CHECK));
        p=(INT8U*)p_MemoryCheck;
        for(i16=0; i16<sizeof(S_MEMORY_CHECK); i16++)
        {
            p[i16]=i16+1;
        }
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_MemoryCheck,MEMORY_APP_CHECK_ALL,0);
        memset((INT8U*)p_MemoryCheck,0x00,sizeof(S_MEMORY_CHECK));
        res = Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_MemoryCheck,MEMORY_APP_CHECK_ALL,0);
        //�ȶ�
        for(i16=0; i16<MEMORY_CHECK_SIZE-2; i16++)
        {
            if(p[i16]==(i16+1));
            else break;
        }
        if(i16==MEMORY_CHECK_SIZE-2 && MEMORY_CHECK_SIZE==sizeof(S_MEMORY_CHECK) && res==MEMORY_APP_ERR_NO)
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Check Ok!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Check Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //������
        memset((INT8U*)p_MemoryCheck,0x00,sizeof(S_MEMORY_CHECK));
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_W,(INT8U*)p_MemoryCheck,MEMORY_APP_CHECK_ALL,0);
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Check End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����
        p_Control=(S_MEMORY_CONTROL*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Control Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        memset((INT8U*)p_Control,0x00,sizeof(S_MEMORY_CONTROL));
        p=(INT8U*)p_Control;
        for(i=0; i<sizeof(S_MEMORY_CONTROL); i++)
        {
            p[i]=i+1;
        }
        Memory_InternalApp(MEMORY_APP_CMD_CONTROL_W,(INT8U*)p_Control,0,0);
        memset((INT8U*)p_Control,0x00,sizeof(S_MEMORY_CONTROL));
        res = Memory_InternalApp(MEMORY_APP_CMD_CONTROL_R,(INT8U*)p_Control,0,0);
        //�ȶ�
        for(i=0; i<MEMORY_CONTROL_SIZE-2; i++)
        {
            if(p[i]==(i+1));
            else break;
        }
        if(i==MEMORY_CONTROL_SIZE-2 && MEMORY_CONTROL_SIZE==sizeof(S_MEMORY_CONTROL) && res==MEMORY_APP_ERR_NO)
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Control Ok!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Control Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Control End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------��
        p_Door=(S_MEMORY_DOOR*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Door Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_DOOR_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Door,0x00,sizeof(S_MEMORY_DOOR));
            p=(INT8U*)p_Door;
            for(j16=0; j16<sizeof(S_MEMORY_DOOR); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_DOOR_W,(INT8U*)p_Door,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_DOOR_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Door,0x00,sizeof(S_MEMORY_DOOR));
            res = Memory_InternalApp(MEMORY_APP_CMD_DOOR_R,(INT8U*)p_Door,i16,0);
            for(j16=0; j16<(MEMORY_DOOR_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_DOOR_SIZE-2==j16 && MEMORY_DOOR_SIZE==sizeof(S_MEMORY_DOOR) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_DOOR_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOOR,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Door OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Door Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Door End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        /*
        //---------------------����λͼ����        
        p_DoorGBitmap=(S_MEMORY_DOORGBITMAP*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test DoorGB Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_DOORGBITMAP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_DoorGBitmap,0x00,sizeof(S_MEMORY_DOORGBITMAP));
            p=(INT8U*)p_DoorGBitmap;
            for(j16=0; j16<sizeof(S_MEMORY_DOORGBITMAP); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_DOORGBITMAP_W,(INT8U*)p_DoorGBitmap,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_DOORGBITMAP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_DoorGBitmap,0x00,sizeof(S_MEMORY_DOORGBITMAP));
            res = Memory_InternalApp(MEMORY_APP_CMD_DOORGBITMAP_R,(INT8U*)p_DoorGBitmap,i16,0);
            for(j16=0; j16<(MEMORY_DOORGBITMAP_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_DOORGBITMAP_SIZE-2==j16 && MEMORY_DOORGBITMAP_SIZE==sizeof(S_MEMORY_DOORGBITMAP) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_DOORGBITMAP_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOORG_BITMAP,0);
            sprintf((char *)Member_PrintBuf,"Memory Test DoorGB OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test DoorGB Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test DoorGB End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        */
        //---------------------����λͼ����
        p_CardGBitmap=(S_MEMORY_CARDG_BITMAP*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test CardGB Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_CARDGBITMAP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_CardGBitmap,0x00,sizeof(S_MEMORY_CARDG_BITMAP));
            p=(INT8U*)p_CardGBitmap;
            for(j16=0; j16<sizeof(S_MEMORY_CARDG_BITMAP); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_CARDGBITMAP_W,(INT8U*)p_CardGBitmap,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_CARDGBITMAP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_CardGBitmap,0x00,sizeof(S_MEMORY_CARDG_BITMAP));
            res = Memory_InternalApp(MEMORY_APP_CMD_CARDGBITMAP_R,(INT8U*)p_CardGBitmap,i16,0);
            for(j16=0; j16<(MEMORY_CARDGBITMAP_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_CARDGBITMAP_SIZE-2==j16 && MEMORY_CARDGBITMAP_SIZE==sizeof(S_MEMORY_CARDG_BITMAP) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_CARDGBITMAP_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARDG_BITMAP,0);
            sprintf((char *)Member_PrintBuf,"Memory Test CardGB OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test CardGB Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test CardGB End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����������
        p_Reader=(S_MEMORY_READER*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Reader Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_READER_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Reader,0x00,sizeof(S_MEMORY_READER));
            p=(INT8U*)p_Reader;
            for(j16=0; j16<sizeof(S_MEMORY_READER); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_READER_W,(INT8U*)p_Reader,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_READER_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Reader,0x00,sizeof(S_MEMORY_READER));
            res = Memory_InternalApp(MEMORY_APP_CMD_READER_R,(INT8U*)p_Reader,i16,0);
            for(j16=0; j16<(MEMORY_READER_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_READER_SIZE-2==j16 && MEMORY_READER_SIZE==sizeof(S_MEMORY_READER) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_READER_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_READER,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Reader OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Reader Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Reader End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------�̵�������
        p_Relay=(S_MEMORY_RELAY*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Relay Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_RELAY_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Relay,0x00,sizeof(S_MEMORY_RELAY));
            p=(INT8U*)p_Relay;
            for(j16=0; j16<sizeof(S_MEMORY_RELAY); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_RELAY_W,(INT8U*)p_Relay,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_RELAY_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Relay,0x00,sizeof(S_MEMORY_RELAY));
            res = Memory_InternalApp(MEMORY_APP_CMD_RELAY_R,(INT8U*)p_Relay,i16,0);
            for(j16=0; j16<(MEMORY_RELAY_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_RELAY_SIZE-2==j16 && MEMORY_RELAY_SIZE==sizeof(S_MEMORY_RELAY) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_RELAY_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_RELAY,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Relay OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Relay Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Relay End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------�������
        p_Input=(S_MEMORY_INPUT*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Input Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_INPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Input,0x00,sizeof(S_MEMORY_INPUT));
            p=(INT8U*)p_Input;
            for(j16=0; j16<sizeof(S_MEMORY_INPUT); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_INPUT_W,(INT8U*)p_Input,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_INPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Input,0x00,sizeof(S_MEMORY_INPUT));
            res = Memory_InternalApp(MEMORY_APP_CMD_INPUT_R,(INT8U*)p_Input,i16,0);
            for(j16=0; j16<(MEMORY_INPUT_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_INPUT_SIZE-2==j16 && MEMORY_INPUT_SIZE==sizeof(S_MEMORY_INPUT) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_INPUT_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_INPUT,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Input OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Input Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Input End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����
        p_Alarm=(S_MEMORY_ALARM*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Alarm Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        memset((INT8U*)p_Alarm,0x00,sizeof(S_MEMORY_ALARM));
        p=(INT8U*)p_Alarm;
        for(i=0; i<sizeof(S_MEMORY_ALARM); i++)
        {
            p[i]=i+1;
        }
        Memory_InternalApp(MEMORY_APP_CMD_ALARM_W,(INT8U*)p_Alarm,0,0);
        memset((INT8U*)p_Alarm,0x00,sizeof(S_MEMORY_ALARM));
        res = Memory_InternalApp(MEMORY_APP_CMD_ALARM_R,(INT8U*)p_Alarm,0,0);
        //�ȶ�
        for(i=0; i<MEMORY_ALARM_SIZE-2; i++)
        {
            if(p[i]==(i+1));
            else break;
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i==MEMORY_ALARM_SIZE-2 && MEMORY_ALARM_SIZE==sizeof(S_MEMORY_ALARM) && res==MEMORY_APP_ERR_NO)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_ALARM,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Alarm OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Alarm Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Alarm End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����
        p_DoorG=(S_MEMORY_DOOR_GROUP*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test DoorGroup Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_DOORGROUP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_DoorG,0x00,sizeof(S_MEMORY_DOOR_GROUP));
            p=(INT8U*)p_DoorG;
            for(j16=0; j16<sizeof(S_MEMORY_DOOR_GROUP); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_DOORGROUP_W,(INT8U*)p_DoorG,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_DOORGROUP_MAX_NUM; i16++)
        {
            memset((INT8U*)p_DoorG,0x00,sizeof(S_MEMORY_DOOR_GROUP));
            res = Memory_InternalApp(MEMORY_APP_CMD_DOORGROUP_R,(INT8U*)p_DoorG,i16,0);
            for(j16=0; j16<(MEMORY_DOORGROUP_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_DOORGROUP_SIZE-2==j16 && MEMORY_DOORGROUP_SIZE==sizeof(S_MEMORY_DOOR_GROUP) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_DOORGROUP_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOORGROUP,0);
            sprintf((char *)Member_PrintBuf,"Memory Test DoorG OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test DoorGroup Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test DoorGroup End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����
        p_CardG=(S_MEMORY_CARDGROUP_LIMIT*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test CardGroup Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_CARDGROUPLIMIT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_CardG,0x00,sizeof(S_MEMORY_CARDGROUP_LIMIT));
            p=(INT8U*)p_CardG;
            for(j16=0; j16<sizeof(S_MEMORY_CARDGROUP_LIMIT); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_CARDGROUP_LIMIT_W,(INT8U*)p_CardG,i16,i16);
        }
        //
        for(i16=0; i16<MEMORY_CARDGROUPLIMIT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_CardG,0x00,sizeof(S_MEMORY_CARDGROUP_LIMIT));
            res = Memory_InternalApp(MEMORY_APP_CMD_CARDGROUP_LIMIT_R,(INT8U*)p_CardG,i16,0);
            for(j16=0; j16<(MEMORY_CARDGROUPLIMIT_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_CARDGROUPLIMIT_SIZE-2==j16 && MEMORY_CARDGROUPLIMIT_SIZE==sizeof(S_MEMORY_CARDGROUP_LIMIT) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_CARDGROUPLIMIT_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARDGROUP,0);
            sprintf((char *)Member_PrintBuf,"Memory Test CardG OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test CardGroup Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test CardGroup End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------����
        p_Area=(S_MEMORY_AREA*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Area Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_AREA_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Area,0x00,sizeof(S_MEMORY_AREA));
            p=(INT8U*)p_Area;
            for(j16=0; j16<sizeof(S_MEMORY_AREA); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_AREA_W,(INT8U*)p_Area,i16,i16);
            //������
            Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_Area,i16,i16);
            p[j16]=0;
        }
        //
        for(i16=0; i16<MEMORY_AREA_MAX_NUM; i16++)
        {
            memset((INT8U*)p_Area,0x00,sizeof(S_MEMORY_AREA));
            res = Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_Area,i16,0);
            for(j16=0; j16<(MEMORY_AREA_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_AREA_SIZE-2==j16 && MEMORY_AREA_SIZE==sizeof(S_MEMORY_AREA) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_AREA_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_AREA,0);
            sprintf((char *)Member_PrintBuf,"Memory Test Area OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test Area Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Area End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------�߼�ģ��-->���������
        p_LogicBlockInOutAnd=(S_MEMORY_LOGICBLOCK_INOUTAND*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test InOutAnd Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockInOutAnd,0x00,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
            p=(INT8U*)p_LogicBlockInOutAnd;
            for(j16=0; j16<sizeof(S_MEMORY_LOGICBLOCK_INOUTAND); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_W,(INT8U*)p_LogicBlockInOutAnd,i16,i16);
            //������
            //Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_LogicBlockInput,i16,i16);
            p[j16]=0;
        }
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockInOutAnd,0x00,sizeof(S_MEMORY_LOGICBLOCK_INOUTAND));
            res = Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_R,(INT8U*)p_LogicBlockInOutAnd,i16,0);
            for(j16=0; j16<(MEMORY_LOGICBLOCK_INOUTAND_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_LOGICBLOCK_INOUTAND_SIZE-2==j16 && MEMORY_LOGICBLOCK_INOUTAND_SIZE==sizeof(S_MEMORY_LOGICBLOCK_INOUTAND) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,0);
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockInOutAnd OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockInOutAnd Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test LogicBlockInOutAnd End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        /*
        //---------------------�߼�ģ��-->����ģ��
        p_LogicBlockInput=(S_MEMORY_LOGICBLOCK_INPUT*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test LogicBlockInput Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_INPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockInput,0x00,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
            p=(INT8U*)p_LogicBlockInput;
            for(j16=0; j16<sizeof(S_MEMORY_LOGICBLOCK_INPUT); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_INPUT_W,(INT8U*)p_LogicBlockInput,i16,i16);
            //������
            //Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_LogicBlockInput,i16,i16);
            p[j16]=0;
        }
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_INPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockInput,0x00,sizeof(S_MEMORY_LOGICBLOCK_INPUT));
            res = Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_INPUT_R,(INT8U*)p_LogicBlockInput,i16,0);
            for(j16=0; j16<(MEMORY_LOGICBLOCK_INPUT_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_LOGICBLOCK_INPUT_SIZE-2==j16 && MEMORY_LOGICBLOCK_INPUT_SIZE==sizeof(S_MEMORY_LOGICBLOCK_INPUT) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_LOGICBLOCK_INPUT_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_LOGICBLOCK_INPUT,0);
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockInput OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockInput Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test LogicBlockInput End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------�߼�ģ��-->�߼�ģ��
        p_LogicBlockLogic=(S_MEMORY_LOGICBLOCK_LOGIC*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test LogicBlockLogic Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_LOGIC_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockLogic,0x00,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
            p=(INT8U*)p_LogicBlockLogic;
            for(j16=0; j16<sizeof(S_MEMORY_LOGICBLOCK_LOGIC); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_LOGIC_W,(INT8U*)p_LogicBlockLogic,i16,i16);
            //������
            //Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_LogicBlockInput,i16,i16);
            p[j16]=0;
        }
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_LOGIC_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockLogic,0x00,sizeof(S_MEMORY_LOGICBLOCK_LOGIC));
            res = Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_LOGIC_R,(INT8U*)p_LogicBlockLogic,i16,0);
            for(j16=0; j16<(MEMORY_LOGICBLOCK_LOGIC_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_LOGICBLOCK_LOGIC_SIZE-2==j16 && MEMORY_LOGICBLOCK_LOGIC_SIZE==sizeof(S_MEMORY_LOGICBLOCK_LOGIC) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_LOGICBLOCK_LOGIC_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,0);
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockLogic OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockLogic Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test LogicBlockLogic End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        //---------------------�߼�ģ��-->���ģ��
        p_LogicBlockOutput=(S_MEMORY_LOGICBLOCK_OUTPUT*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test LogicBlockOutput Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockOutput,0x00,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
            p=(INT8U*)p_LogicBlockOutput;
            for(j16=0; j16<sizeof(S_MEMORY_LOGICBLOCK_OUTPUT); j16++)
            {
                p[j16]=j16+1;
            }
            Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_W,(INT8U*)p_LogicBlockOutput,i16,i16);
            //������
            //Memory_InternalApp(MEMORY_APP_CMD_AREA_R,(INT8U*)p_LogicBlockInput,i16,i16);
            p[j16]=0;
        }
        //
        for(i16=0; i16<MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM; i16++)
        {
            memset((INT8U*)p_LogicBlockOutput,0x00,sizeof(S_MEMORY_LOGICBLOCK_OUTPUT));
            res = Memory_InternalApp(MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_R,(INT8U*)p_LogicBlockOutput,i16,0);
            for(j16=0; j16<(MEMORY_LOGICBLOCK_OUTPUT_SIZE)-2; j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            if(MEMORY_LOGICBLOCK_OUTPUT_SIZE-2==j16 && MEMORY_LOGICBLOCK_OUTPUT_SIZE==sizeof(S_MEMORY_LOGICBLOCK_OUTPUT) &&res==MEMORY_APP_ERR_NO)
            {
                ;
            }
            else
            {
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        if(i16==MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,0);
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockOutput OK :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Test LogicBlockOutput Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test LogicBlockOutput End.\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
        */
    }
#endif
    //-----д��ʱ���������(д��256��)
    if(cmd& Memory_TEST_WR_TIMEGROUP)
    {
        p=(INT8U *)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test TimeGroup Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_FLASH_MAX_TIMEGROUP; i16++)
        {
            //��ʼ������
            for(j16=0; j16<sizeof(S_MEMORY_TIMEGROUP); j16++)
            {
                p[j16]=j16+1;
            }
            //д��
            res = Memory_InternalApp(MEMORY_APP_CMD_TIMEGROUP_W,(INT8U*)p,i16,i16);
            //�建��
            for(j16=0; j16<sizeof(S_MEMORY_TIMEGROUP); j16++)
            {
                p[j16]=0;
            }
            //��ȡ
            res = Memory_InternalApp(MEMORY_APP_CMD_TIMEGROUP_R,(INT8U*)p,i16,i16);
            //У��
            for(j16=0; j16<(sizeof(S_MEMORY_TIMEGROUP)-2); j16++)
            {
                if(p[j16]==(j16+1));
                else break;
            }
            //
            if((j16==(sizeof(S_MEMORY_TIMEGROUP)-2))&&(res==MEMORY_APP_ERR_NO))
            {
                sprintf((char *)Member_PrintBuf,"TimeGroup(%3d) OK!\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"TimeGroup(%3d) ERR!\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_TIMEGROUP,0);
        sprintf((char *)Member_PrintBuf,"Memory Test TimeG :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"-----Memory Test TimeGroup End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
    //-----д�����������(д��256��)
    if(cmd& Memory_TEST_WR_HOLIDAYGROUP)
    {
        p=(INT8U *)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test HolidayGroup Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i16=0; i16<MEMORY_FLASH_MAX_HOLIDAYGROUP; i16++)
        {
            //��ʼ������
            for(j16=0; j16<sizeof(S_MEMORY_HOLIDAYGROUP); j16++)
            {
                p[j16]=j16+1;
            }
            //д��
            res = Memory_InternalApp(MEMORY_APP_CMD_HOLIDAYGROUP_W,(INT8U*)p,i16,i16);
            //�建��
            for(j16=0; j16<sizeof(S_MEMORY_HOLIDAYGROUP); j16++)
            {
                p[j16]=0;
            }
            //��ȡ
            res = Memory_InternalApp(MEMORY_APP_CMD_HOLIDAYGROUP_R,(INT8U*)p,i16,i16);
            //У��
            for(j16=0; j16<sizeof(S_MEMORY_HOLIDAYGROUP)-2; j16++)
            {
                if(p[j16]==j16+1);
                else break;
            }
            //
            if((j16==sizeof(S_MEMORY_HOLIDAYGROUP)-2)&&(res==MEMORY_APP_ERR_NO))
            {
                sprintf((char *)Member_PrintBuf,"HolidayGroup(%3d) OK!\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"HolidayGroup(%3d) ERR!\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
        }
        //
        p_Check=(S_CHECK*)MemberTestBuf;
        //
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_HOLIDAYGROUP,0);
        sprintf((char *)Member_PrintBuf,"Memory Test TimeG :Num-%05d,Sum-%05d!\r\n",p_Check->Number,p_Check->CheckSum);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"-----Memory Test HolidayGroup End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
    //-----��дˢ����¼����(д��10240����У��)
    if(cmd& Memory_TEST_WR_CARDRECORD)
    {
        p_CardRecord=(S_MEMORY_CARD_RECORD*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------CardRecord Test Begin!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //������ƬMemory
        Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        //д��10000����¼
        for(i32=0; i32<MEMORY_FLASH_MAX_CARD_RECORD; i32++)
        {
            //д
            memset((INT8U*)p_CardRecord,0x01,sizeof(S_MEMORY_CARD_RECORD));
            p_CardRecord->CardNumber[0]=i32>>24;
            p_CardRecord->CardNumber[1]=i32>>16;
            p_CardRecord->CardNumber[2]=i32>>8;
            p_CardRecord->CardNumber[3]=i32;
            res = Memory_InternalApp(MEMORY_APP_CMD_CARD_RECORD_W,(INT8U*)p_CardRecord,0,0);
            if(res!=MEMORY_APP_ERR_NO)
            {
                sprintf((char *)Member_PrintBuf,"ID-%05ld Write Err!\r\n",i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
            else
            {
                if(i32%256==0)
                {
                    DebugOut(".",1);
                }
            }
        }
        if(i32==MEMORY_FLASH_MAX_CARD_RECORD)
        {
            sprintf((char *)Member_PrintBuf,"%05ld Write OK!\r\n",i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //����¼
        for(i32=0; i32<MEMORY_FLASH_MAX_CARD_RECORD; i32++)
        {
            //��
            memset((INT8U*)p_CardRecord,0,sizeof(S_MEMORY_CARD_RECORD));
            res = Memory_InternalApp(MEMORY_APP_CMD_CARD_RECORD_R,(INT8U*)p_CardRecord,i32,0);
            j32 = p_CardRecord->CardNumber[0];
            j32<<=8;
            j32 += p_CardRecord->CardNumber[1];
            j32<<=8;
            j32 += p_CardRecord->CardNumber[2];
            j32<<=8;
            j32 += p_CardRecord->CardNumber[3];
            i16=p_CardRecord->Event[0];
            i16<<=8;
            i16+=p_CardRecord->Event[1];
            if((j32==i32) && (i16==0x0101))
            {
                if(i32%256==0)
                {
                    DebugOut(".",1);
                }
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"ID-%05ld Read ERR!\r\n",i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
        }
        if(i32==MEMORY_FLASH_MAX_CARD_RECORD)
        {
            sprintf((char *)Member_PrintBuf,"%05ld Read OK!\r\n",i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        p_MemoryCheck=(S_MEMORY_CHECK*)MemberTestBuf;
        //��ӡ��¼����
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_MemoryCheck,0,0);
        sprintf((char *)Member_PrintBuf,"CardRecord Count: %05d!\r\n",p_MemoryCheck->CardRecordNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"-----CardRecord Test End!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
    //-----��д������¼����(д��10240����У��)
    if(cmd& Memory_TEST_WR_ALARMRECORD)
    {
        p_AlarmRecord=(S_MEMORY_ALARM_RECORD*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------AlarmRecord Test Begin!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //������ƬMemory
        Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        //д��10000����¼
        for(i32=0; i32<MEMORY_FLASH_MAX_ALARM_RECORD; i32++)
        {
            //д
            memset((INT8U*)p_AlarmRecord,0x01,sizeof(S_MEMORY_ALARM_RECORD));
            p_AlarmRecord->CardNumber[0]=i32>>24;
            p_AlarmRecord->CardNumber[1]=i32>>16;
            p_AlarmRecord->CardNumber[2]=i32>>8;
            p_AlarmRecord->CardNumber[3]=i32;
            res = Memory_InternalApp(MEMORY_APP_CMD_ALARM_RECORD_W,(INT8U*)p_AlarmRecord,0,0);
            if(res!=MEMORY_APP_ERR_NO)
            {
                sprintf((char *)Member_PrintBuf,"ID-%05ld Write Err!\r\n",i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
            else
            {
                if(i32%256==0)
                {
                    DebugOut(".",1);
                }
            }
        }
        if(i32==MEMORY_FLASH_MAX_ALARM_RECORD)
        {
            sprintf((char *)Member_PrintBuf,"%05ld Write OK!\r\n",i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //����¼
        for(i32=0; i32<MEMORY_FLASH_MAX_ALARM_RECORD; i32++)
        {
            //��
            memset((INT8U*)p_AlarmRecord,0,sizeof(S_MEMORY_ALARM_RECORD));
            res = Memory_InternalApp(MEMORY_APP_CMD_ALARM_RECORD_R,(INT8U*)p_AlarmRecord,i32,0);
            j32 = p_AlarmRecord->CardNumber[0];
            j32<<=8;
            j32 += p_AlarmRecord->CardNumber[1];
            j32<<=8;
            j32 += p_AlarmRecord->CardNumber[2];
            j32<<=8;
            j32 += p_AlarmRecord->CardNumber[3];
            i16=p_AlarmRecord->Event[0];
            i16<<=8;
            i16+=p_AlarmRecord->Event[1];
            if((j32==i32) && (i16==0x0101))
            {
                if(i32%256==0)
                {
                    DebugOut(".",1);
                }
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"ID-%05ld Read ERR!\r\n",i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                break;
            }
        }
        if(i32==MEMORY_FLASH_MAX_ALARM_RECORD)
        {
            sprintf((char *)Member_PrintBuf,"%05ld Read OK!\r\n",i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        p_MemoryCheck=(S_MEMORY_CHECK*)MemberTestBuf;
        //��ӡ��¼����
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_MemoryCheck,0,0);
        sprintf((char *)Member_PrintBuf,"AlarmRecord Count: %05d!\r\n",p_MemoryCheck->AlarmRecordNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sprintf((char *)Member_PrintBuf,"-----AlarmRecord Test End!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
//-----��д�û�Ȩ�޲���(����+�½�10000��+��ȡ10000��,��ӡ��ÿ����дռ�õ�ʱ��,�����Ե�10000�ſ���д��116ms(4M)����)
    if(cmd& Memory_TEST_WR_USER_NEW)
    {
        p_User=(S_MEMORY_USER_PERMISSION*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------USER_NEW TEST Begin!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //������ƬMemory
        //Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        for(i16=0; i16<16; i16++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_USER1+i16,0);
        }
        //д
        for(i16=0; i16<MEMORY_FLASH_MAX_USER; i16++)
        {
            i32=Memory_msCount=0;
            memset((INT8U*)p_User,0x55,sizeof(S_MEMORY_USER_PERMISSION));
            p_User->CardNumber[0]=0;
            p_User->CardNumber[1]=0;
            p_User->CardNumber[2]=i16>>8;
            p_User->CardNumber[3]=i16;
            p_User->Password[0]=0;
            p_User->Password[1]=0;
            p_User->Password[2]=(i16+1)>>8;
            p_User->Password[3]=(i16+1);
            res = Memory_InternalApp(MEMORY_APP_CMD_NEW_USER_W,(INT8U*)p_User,0,0);
            if(res==OK)
            {
                //������Ϣ���
                i32=Memory_msCount;
                sprintf((char *)Member_PrintBuf,"ID-%05d WTime:%ldms\r\n",i16,i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                res=ERR;
                break;
            }
        }
        //
        sprintf((char *)Member_PrintBuf,"�Ƿ���Ҫ�����Ŷ�ȡ?(��1��ȡ,����������)\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        i16=0;
        DebugInputNum(&i16,1);
        //��(������)
        if(res==OK && i16==1)
        {
            for(i16=0; i16<MEMORY_FLASH_MAX_USER; i16++)
            {
                i32=Memory_msCount=0;
                memset((INT8U*)p_User,0x00,sizeof(S_MEMORY_USER_PERMISSION));
                p_User->CardNumber[0]=0;
                p_User->CardNumber[1]=0;
                p_User->CardNumber[2]=i16>>8;
                p_User->CardNumber[3]=i16;
                res = Memory_InternalApp(MEMORY_APP_CMD_USER_PERMISSION_R,(INT8U*)p_User,0,0);
                if(res==OK)
                {
                    j16=p_User->CardNumber[2];
                    j16<<=8;
                    j16+=p_User->CardNumber[3];
                    k16=p_User->Password[2];
                    k16<<=8;
                    k16+=p_User->Password[3];
                    if(j16!=i16 || p_User->Status!=0x55)
                    {
                        res=ERR;
                        break;
                    }
                }
                else
                {
                    res=ERR;
                    break;
                }
                //������Ϣ���
                i32=Memory_msCount;
                sprintf((char *)Member_PrintBuf,"ID-%05d Password-%05d RTime:%ldms\r\n",j16,k16,i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            //
            sprintf((char *)Member_PrintBuf,"�����Ŷ�ȡ������ȷ\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"�Ƿ���԰������ȡ?(��1����,������������)\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        i16=0;
        DebugInputNum(&i16,1);
        //��(������)
        if(res==OK && i16==1)
        {
            for(i16=0; i16<MEMORY_FLASH_MAX_USER; i16++)
            {
                i32=Memory_msCount=0;
                memset((INT8U*)p_User,0,sizeof(S_MEMORY_USER_PERMISSION));
                p_User->Password[0]=0;
                p_User->Password[1]=0;
                p_User->Password[2]=(i16+1)>>8;
                p_User->Password[3]=(i16+1);
                if(i16==127)
                {
                    i16=127;
                }
                res = Memory_InternalApp(MEMORY_APP_CMD_USER_PERMISSION_R,(INT8U*)p_User,0,0);
                if(res==OK)
                {
                    j16=p_User->Password[2];
                    j16<<=8;
                    j16+=p_User->Password[3];
                    if(j16!=(i16+1) || p_User->Status!=0x55)
                    {
                        res=ERR;
                        break;
                    }
                }
                else
                {
                    res=ERR;
                    break;
                }
                //������Ϣ���
                i32=Memory_msCount;
                sprintf((char *)Member_PrintBuf,"PASSWORD-%05d RTime:%ldms\r\n",j16,i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //��ӡ���
        if(res==OK)
        {
            sprintf((char *)Member_PrintBuf,"-----USER_NEW TEST OK!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"-----USER_NEW TEST ERR!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"�Ƿ���Ҫ��ʽ��?(��1��ʽ��,��2����)\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        i16=0;
        DebugInputNum(&i16,1);
        if(i16==1)
        {
            sign_exe=TRUE;
        }
        else
        {
            sign_exe=FALSE;
        }
    }
#ifdef STM32
    if(cmd& Memory_TEST_WR_VISITOR_NEW)
    {
        p_User=(S_MEMORY_USER_PERMISSION*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------VISITOR_NEW TEST Begin!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //������ƬMemory
        Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        //д
        for(i16=0; i16<MEMORY_FLASH_USER_PERMISSION_MAX_NUM; i16++)
        {
            i32=Memory_msCount=0;
            memset((INT8U*)p_User,0x55,sizeof(S_MEMORY_USER_PERMISSION));
            p_User->CardNumber[0]=0;
            p_User->CardNumber[1]=0;
            p_User->CardNumber[2]=i16>>8;
            p_User->CardNumber[3]=i16;
            res = Memory_InternalApp(MEMORY_APP_CMD_NEW_VISITOR_W,(INT8U*)p_User,0,i16);
            //������Ϣ���
            i32=Memory_msCount;
            sprintf((char *)Member_PrintBuf,"ID-%05d WTime:%ldms\r\n",i16,i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //��
        if(res==OK)
        {
            for(i16=0; i16<MEMORY_FLASH_USER_PERMISSION_MAX_NUM; i16++)
            {
                i32=Memory_msCount=0;
                memset((INT8U*)p_User,0xAA,sizeof(S_MEMORY_USER_PERMISSION));
                p_User->CardNumber[0]=0;
                p_User->CardNumber[1]=0;
                p_User->CardNumber[2]=i16>>8;
                p_User->CardNumber[3]=i16;
                res = Memory_InternalApp(MEMORY_APP_CMD_USER_PERMISSION_R,(INT8U*)p_User,0,0);
                if(res==OK)
                {
                    j16=p_User->CardNumber[2];
                    j16<<=8;
                    j16+=p_User->CardNumber[3];
                    if(j16!=i16 || p_User->Status!=0x55)
                    {
                        res=ERR;
                        break;
                    }
                }
                //������Ϣ���
                i32=Memory_msCount;
                sprintf((char *)Member_PrintBuf,"ID-%05d RTime:%ldms\r\n",i16,i32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //��ӡ���
        if(res==OK)
        {
            sprintf((char *)Member_PrintBuf,"-----VISITOR_NEW TEST OK!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"-----VISITOR_NEW TEST ERR!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        sign_exe=TRUE;
    }
#endif
#if 0
    if(cmd&Memory_TEST_WR_REVERSE)
    {
        p_Reverse1=(S_MEMORY_REVERSE*)MemberTestBuf;
        //
        sprintf((char *)Member_PrintBuf,"----------REVERSE TEST Begin!\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //������ƬMemory
        Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        //д
        for(i16=1; i16<=MEMORY_REVERSE_MAX_NUM; i16++)
        {
            p_Reverse1->CardNumber[0]=0;
            p_Reverse1->CardNumber[1]=0;
            p_Reverse1->CardNumber[2]=i16>>8;
            p_Reverse1->CardNumber[3]=i16;
            res=Memory_InternalApp(MEMORY_APP_CMD_REVERSE_W,(INT8U*)p_Reverse1,0,0);
            res=Memory_InternalApp(MEMORY_APP_CMD_REVERSE_W,(INT8U*)p_Reverse1,1,0);
            res=Memory_InternalApp(MEMORY_APP_CMD_REVERSE_W,(INT8U*)p_Reverse1,2,0);
            res=Memory_InternalApp(MEMORY_APP_CMD_REVERSE_W,(INT8U*)p_Reverse1,3,0);
            if(res==OK)
            {
                sprintf((char *)Member_PrintBuf,"Wirte %d OK.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"Wirte %d ERR.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //
        if(res==OK)
        {
            sprintf((char *)Member_PrintBuf,"Wirte 256*4 OK.\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Wirte 256*4 ERR.\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //��
        p_Reverse1=(S_MEMORY_REVERSE*)MemberTestBuf;
        if(res==OK)
        {
            for(i16=1; i16<=MEMORY_REVERSE_MAX_NUM; i16++)
            {
                i32=Memory_msCount=0;
                p_Reverse1->CardNumber[0]=0;
                p_Reverse1->CardNumber[1]=0;
                p_Reverse1->CardNumber[2]=i16>>8;
                p_Reverse1->CardNumber[3]=i16;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,0,0);
                if(res!=OK||p_Reverse1->State!=1)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,1,0);
                if(res!=OK||p_Reverse1->State!=1)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,2,0);
                if(res!=OK||p_Reverse1->State!=1)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,3,0);
                if(res!=OK||p_Reverse1->State!=1)break;

                sprintf((char *)Member_PrintBuf,"Read %d OK.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            if(i16>MEMORY_REVERSE_MAX_NUM)
            {
                sprintf((char *)Member_PrintBuf,"Read 256*4 OK.\r\n");
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"Read 256*4 ERR(%d).\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //����
        if(res==OK)
        {
            for(i16=1; i16<=MEMORY_REVERSE_MAX_NUM/2; i16++)
            {
                p_Reverse1->CardNumber[0]=0;
                p_Reverse1->CardNumber[1]=0;
                p_Reverse1->CardNumber[2]=i16>>8;
                p_Reverse1->CardNumber[3]=i16;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,0,0);
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,1,0);
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,2,0);
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,3,0);
                //
                sprintf((char *)Member_PrintBuf,"ClearSingle %d.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            for(i16=1; i16<=MEMORY_REVERSE_MAX_NUM; i16++)
            {
                p_Reverse1->CardNumber[0]=0;
                p_Reverse1->CardNumber[1]=0;
                p_Reverse1->CardNumber[2]=i16>>8;
                p_Reverse1->CardNumber[3]=i16;

                if(i16<=MEMORY_REVERSE_MAX_NUM/2)
                {
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,0,0);
                    if(res!=OK||p_Reverse1->State!=2)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,1,0);
                    if(res!=OK||p_Reverse1->State!=2)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,2,0);
                    if(res!=OK||p_Reverse1->State!=2)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,3,0);
                    if(res!=OK||p_Reverse1->State!=2)break;
                }
                else
                {
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,0,0);
                    if(res!=OK||p_Reverse1->State!=1)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,1,0);
                    if(res!=OK||p_Reverse1->State!=1)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,2,0);
                    if(res!=OK||p_Reverse1->State!=1)break;
                    res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,3,0);
                    if(res!=OK||p_Reverse1->State!=1)break;
                }
                //
                sprintf((char *)Member_PrintBuf,"ReadClearSingle %d OK.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            if(i16>MEMORY_REVERSE_MAX_NUM)
            {
                sprintf((char *)Member_PrintBuf,"ClearSingle OK.\r\n");
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"ClearSingle ERR(%d).\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //ȫ��
        if(res==OK)
        {
            p_Reverse1->CardNumber[0]=0xFF;
            p_Reverse1->CardNumber[1]=0xFF;
            p_Reverse1->CardNumber[2]=0xFF;
            p_Reverse1->CardNumber[3]=0xFF;
            res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,0,0);
            res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,1,0);
            res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,2,0);
            res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_C,(INT8U*)p_Reverse1,3,0);
            for(i16=1; i16<=MEMORY_REVERSE_MAX_NUM; i16++)
            {
                p_Reverse1->CardNumber[0]=0;
                p_Reverse1->CardNumber[1]=0;
                p_Reverse1->CardNumber[2]=i16>>8;
                p_Reverse1->CardNumber[3]=i16;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,0,0);
                if(res!=OK||p_Reverse1->State!=2)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,1,0);
                if(res!=OK||p_Reverse1->State!=2)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,2,0);
                if(res!=OK||p_Reverse1->State!=2)break;
                res = Memory_InternalApp(MEMORY_APP_CMD_REVERSE_R,(INT8U*)p_Reverse1,3,0);
                if(res!=OK||p_Reverse1->State!=2)break;
                //
                sprintf((char *)Member_PrintBuf,"ClearAll %d OK.\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            if(i16>MEMORY_REVERSE_MAX_NUM)
            {
                sprintf((char *)Member_PrintBuf,"ClearAll OK.\r\n");
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                sprintf((char *)Member_PrintBuf,"ClearAll ERR(%d).\r\n",i16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
        }
        //��ӡ���
        if(res==OK && i16>MEMORY_REVERSE_MAX_NUM)
        {
            sprintf((char *)Member_PrintBuf,"-----REVERSE TEST OK!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"-----REVERSE TEST ERR!\r\n");
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        sign_exe=TRUE;
    }
#endif
    //-----��ȡ�¼���¼
    if(cmd& Memory_TEST_READ_EVENT)
    {
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Read Event Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ӡ��ǰ�¼���¼����(i16)
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)&i16,MEMORY_APP_CHECK_CARD_RECORD,0);//�¼���¼����
        sprintf((char *)Member_PrintBuf,"  Event Number: %05d\r\n",i16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ʾ���ȴ��û�������ʼ��¼��ַ(j16)
        sprintf((char *)Member_PrintBuf,"  Please Input Begin Addr: ");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        res = DebugInputNum(&j16,5);
        if(res==ERR || j16>i16)
        {
            goto Goto_MemoryTest1;
        }
        //��ʾ���ȴ��û������ȡ����(k16)
        sprintf((char *)Member_PrintBuf,"  Please Input Len: ");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        res = DebugInputNum(&k16,5);
        if(res==ERR)
        {
            goto Goto_MemoryTest1;
        }
        //���Ƚ���(i16)
        if(j16+k16<i16)
        {
            i16=j16+k16;
        }
        //��ӡ��¼����
        p_CardRecord=(S_MEMORY_CARD_RECORD*)MemberTestBuf;
        for(j16=j16; j16<i16; j16++)
        {
            memset((INT8U*)p_CardRecord,0,sizeof(S_MEMORY_CARD_RECORD));
            res = Memory_InternalApp(MEMORY_APP_CMD_CARD_RECORD_R,(INT8U*)p_CardRecord,j16,0);
            i32 =  p_CardRecord->CardNumber[0];
            i32 <<= 8;
            i32 += p_CardRecord->CardNumber[1];
            i32 <<= 8;
            i32 += p_CardRecord->CardNumber[2];
            i32 <<= 8;
            i32 += p_CardRecord->CardNumber[3];
            i32&=0x0000FFFF;
            k16 = p_CardRecord->Event[0];
            k16 <<=8;
            k16 += p_CardRecord->Event[1];
            TimeTypeConvert(2,&Memory_tm,p_CardRecord->RecordTime);
            sprintf((char *)Member_PrintBuf,"  Num-%05d Card-%05ld ID-%05d CH-%d Time-%04d-%02d-%02d %02d:%02d:%02d\r\n",
                    j16+1,i32,k16,p_CardRecord->ReaderOrWireOrRelayNum,Memory_tm.tm_year,Memory_tm.tm_mon+1,Memory_tm.tm_mday,Memory_tm.tm_hour,Memory_tm.tm_min,Memory_tm.tm_sec);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Read Event End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
    }
    //-----��ȡ������¼
    if(cmd& Memory_TEST_READ_ALARM)
    {
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test Read Alarm Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ӡ��ǰ�¼���¼����(i16)
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)&i16,MEMORY_APP_CHECK_ALARM_RECORD,0);//�¼���¼����
        sprintf((char *)Member_PrintBuf,"  Alarm Number: %05d\r\n",i16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ʾ���ȴ��û�������ʼ��¼��ַ(j16)
        sprintf((char *)Member_PrintBuf,"  Please Input Begin Addr: ");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        res = DebugInputNum(&j16,5);
        if(res==ERR || j16>i16)
        {
            goto Goto_MemoryTest1;
        }
        //��ʾ���ȴ��û������ȡ����(k16)
        sprintf((char *)Member_PrintBuf,"  Please Input Len: ");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        res = DebugInputNum(&k16,5);
        if(res==ERR)
        {
            goto Goto_MemoryTest1;
        }
        //���Ƚ���(i16)
        if(j16+k16<i16)
        {
            i16=j16+k16;
        }
        //��ӡ��¼����
        p_AlarmRecord=(S_MEMORY_ALARM_RECORD*)MemberTestBuf;
        for(j16=j16; j16<i16; j16++)
        {
            memset((INT8U*)p_AlarmRecord,0,sizeof(S_MEMORY_ALARM_RECORD));
            res = Memory_InternalApp(MEMORY_APP_CMD_ALARM_RECORD_R,(INT8U*)p_AlarmRecord,j16,0);
            i32 =  p_AlarmRecord->CardNumber[0];
            i32 <<= 8;
            i32 += p_AlarmRecord->CardNumber[1];
            i32 <<= 8;
            i32 += p_AlarmRecord->CardNumber[2];
            i32 <<= 8;
            i32 += p_AlarmRecord->CardNumber[3];
            i32&=0x0000FFFF;
            k16 = p_AlarmRecord->Event[0];
            k16 <<=8;
            k16 += p_AlarmRecord->Event[1];
            TimeTypeConvert(2,&Memory_tm,p_AlarmRecord->RecordTime);
            sprintf((char *)Member_PrintBuf,"  Num-%05d Card-%05ld ID-%05d CH-%d Time-%04d-%02d-%02d %02d:%02d:%02d\r\n",
                    j16+1,i32,k16,p_CardRecord->ReaderOrWireOrRelayNum,Memory_tm.tm_year,Memory_tm.tm_mon+1,Memory_tm.tm_mday,Memory_tm.tm_hour,Memory_tm.tm_min,Memory_tm.tm_sec);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Read Alarm End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
    }
    //PIC->Լ3%������
    if(cmd & Memory_TEST_READ_DOOR)
    {
        //�Ż�����Ϣ
        sprintf((char*)Member_PrintBuf,"(1)�Ż�����Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_Door = (S_MEMORY_DOOR*)MemberTestBuf;
        for(i=0; i<MEMORY_DOOR_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_DOOR_R,(INT8U*)p_Door,i,0);
            if(p_Door->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_Door->Sum;
                sprintf((char*)Member_PrintBuf,"  ��%03d: ���-%03d,У��-%05d\r\n",
                        i+1,
                        p_Door->Number,


                        p_Door->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
                sprintf((char*)Member_PrintBuf,"     �������� -  %05d\r\n",p_Door->InDoorNumber[0]*256+p_Door->InDoorNumber[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �������� -  %05d\r\n",p_Door->OutDoorNumber[0]*256+p_Door->OutDoorNumber[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ����λͼ -  [%02x]\r\n",p_Door->FunctionSelect);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOOR,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //����λͼ
        /*
        sprintf((char*)Member_PrintBuf,"(2)����λͼ��Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_DoorGBitmap = (S_MEMORY_DOORGBITMAP*)MemberTestBuf;
        for(i=0; i<MEMORY_DOORGBITMAP_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_DOORGBITMAP_R,(INT8U*)p_DoorGBitmap,i,0);
            if(p_DoorGBitmap->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_DoorGBitmap->Sum;
                sprintf((char*)Member_PrintBuf,"  ����λͼ%03d: ���-%03d,У��-%05d\r\n  ",
                        i+1,
                        p_DoorGBitmap->Number,
                        p_DoorGBitmap->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                for(k16=0; k16<128; k16++)
                {
                    if(k16%10==0)
                    {
                        sprintf((char*)Member_PrintBuf,"\r\n  ");
                        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                    }
                    sprintf((char*)Member_PrintBuf,"[%02x]",p_DoorGBitmap->Bitmap[k16]);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                }
                sprintf((char*)Member_PrintBuf,"\r\n");
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_DOORG_BITMAP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        */
        //����λͼ
        sprintf((char*)Member_PrintBuf,"(3)����λͼ��Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_CardGBitmap = (S_MEMORY_CARDG_BITMAP*)MemberTestBuf;
        for(i=0; i<MEMORY_CARDGBITMAP_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_CARDGBITMAP_R,(INT8U*)p_CardGBitmap,i,0);
            if(p_CardGBitmap->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_CardGBitmap->Sum;
                sprintf((char*)Member_PrintBuf,"  ����λͼ%03d: ���-%03d,У��-%05d\r\n  ",
                        i+1,
                        p_CardGBitmap->Number,
                        p_CardGBitmap->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                for(k16=0; k16<128; k16++)
                {
                    if(k16%10==0)
                    {
                        sprintf((char*)Member_PrintBuf,"\r\n  ");
                        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                    }
                    sprintf((char*)Member_PrintBuf,"[%02x]",p_CardGBitmap->Bitmap[k16]);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                }
                sprintf((char*)Member_PrintBuf,"\r\n");
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_CARDG_BITMAP,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //������
        sprintf((char*)Member_PrintBuf,"(4)��������Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_Reader = (S_MEMORY_READER*)MemberTestBuf;
        for(i=0; i<MEMORY_READER_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_READER_R,(INT8U*)p_Reader,i,0);
            if(p_Reader->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_Reader->Sum;
                sprintf((char*)Member_PrintBuf,"  ������%03d: ���-%03d,У��-%05d\r\n",
                        i+1,
                        p_Reader->Number,
                        p_Reader->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
                sprintf((char*)Member_PrintBuf,"     �ӿ�           -  %03d\r\n",p_Reader->InterfaceType);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ��֤��ʽ       -  %03d\r\n",p_Reader->AuthenticationType);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �������Դ���   -  %03d\r\n",p_Reader->PasswordRetryCount);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ��������ʱ��   -  %03d\r\n",p_Reader->PasswordMaxInputTime_s);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ��������       -  [%02x][%02x][%02x][%02x]\r\n",p_Reader->Password[0],p_Reader->Password[1],p_Reader->Password[2],p_Reader->Password[3]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �����������ʱ��  -  %03d\r\n",p_Reader->PasswordErrLockTime_s);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ��Чʱ��       -  %03d\r\n",p_Reader->EffectTimeGroupNumber);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ����λͼ       -  [%02x]\r\n",p_Reader->functionOption);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �׿���Чʱ��   -  %03d\r\n",p_Reader->FirstCardEffectTime_m[0]*256+p_Reader->FirstCardEffectTime_m[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����01��� -  %03d\r\n",p_Reader->CardG1Num[0]*256+p_Reader->CardG1Num[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����01���� -  %03d\r\n",p_Reader->CardG1Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����02��� -  %03d\r\n",p_Reader->CardG2Num[0]*256+p_Reader->CardG2Num[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����02���� -  %03d\r\n",p_Reader->CardG2Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����03��� -  %03d\r\n",p_Reader->CardG3Num[0]*256+p_Reader->CardG3Num[1]);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     �࿨����03���� -  %03d\r\n",p_Reader->CardG3Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //sprintf((char*)Member_PrintBuf,"     �࿨���ų�ʱ   -  %03d\r\n",p_Reader->ManyCardInterval);
                //DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_READER,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //�̵���
        sprintf((char*)Member_PrintBuf,"(5)�̵�����Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_Relay = (S_MEMORY_RELAY*)MemberTestBuf;
        for(i=0; i<MEMORY_RELAY_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_RELAY_R,(INT8U*)p_Relay,i,0);
            if(p_Relay->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_Relay->Sum;
                sprintf((char*)Member_PrintBuf,"  �̵���%03d: ���-%03d,У��-%05d\r\n",
                        i+1,
                        p_Relay->Number,
                        p_Relay->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
                sprintf((char*)Member_PrintBuf,"     ģʽ     -  %03d\r\n",p_Relay->Relay_Mode);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ��ʱ��� -  %03d\r\n",p_Relay->Relay_ExtendOpenDoorTime);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ������� -  %03d\r\n",p_Relay->Relay_OpenDoorKeepTime);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ȫ�̵ּ�����   -  %03d\r\n",p_Relay->Relay_LinkageRelayGroupNumber_Global);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ���ؼ̵�����   -  %03d\r\n",p_Relay->Relay_LinkageRelayGroupNumber_Local);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_RELAY,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        //��ģ��
        sprintf((char*)Member_PrintBuf,"(6)��ģ����Ϣ:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        i16=j16=0;
        p_Input = (S_MEMORY_INPUT*)MemberTestBuf;
        for(i=0; i<MEMORY_INPUT_MAX_NUM; i++)
        {
            Memory_InternalApp(MEMORY_APP_CMD_INPUT_R,(INT8U*)p_Input,i,0);
            if(p_Input->Sum!=0xFFFF)
            {
                i16++;
                j16+=p_Input->Sum;
                sprintf((char*)Member_PrintBuf,"  ��ģ��%03d: ���-%03d,У��-%05d\r\n",
                        i+1,
                        p_Input->Number,
                        p_Input->Sum);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
                sprintf((char*)Member_PrintBuf,"     ����     -  %03d\r\n",p_Input->Input_LineModeType);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ����λͼ -  [%02x]\r\n",p_Input->Input_FunctionSelect);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ������ʱ -  %03d\r\n",p_Input->Input_DelayTime_s);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ���뱣�� -  %03d\r\n",p_Input->Input_KeepTime_s);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ����ʱ�� -  %03d\r\n",p_Input->Input_DisableTimeGroupNumber);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ȫ�̵ּ�����   -  %03d\r\n",p_Input->Input_LinkageRelayGroupNumber_Global);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                sprintf((char*)Member_PrintBuf,"     ���ؼ̵�����   -  %03d\r\n",p_Input->Input_LinkageRelayGroupNumber_Local);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
                //
            }
        }
        sprintf((char*)Member_PrintBuf,"  ����:����-%05d,У��-%05d\r\n",i16,j16);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
        p_Check = (S_CHECK*)MemberTestBuf;
        Memory_InternalApp(MEMORY_APP_CMD_CHECK_R,(INT8U*)p_Check,MEMORY_APP_CHECK_INPUT,0);
        sprintf((char*)Member_PrintBuf,"  ��ȡ:����-%05d,У��-%05d,��ˮ-%08ld\r\n",p_Check->Number,p_Check->CheckSum,p_Check->SerialNumber);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char*)Member_PrintBuf));
    }
    //-----����EEPROM��д����
    if(cmd& Memory_TEST_WR_EEPROM)
    {
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Test EEPROM CHIP Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ʾ�û��������
        sprintf((char *)Member_PrintBuf,"Please Input Test Count(1-100):\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        res = DebugInputNum(&j16,5);
        DebugOut((INT8S*)"\r\n",strlen("\r\n"));
        //
        if(res!=OK || j16==0 || j16>100)
        {
            DebugOut((INT8S*)"Input Error\r\n",strlen("Input Error\r\n"));
            goto Goto_MemoryTest1;
        }
        for(k16=1; k16<=j16; k16++)
        {
            //�ַ���д(256B)
            j32=Memory_msCount=0;
            for(i32=0; i32<MEMORY_EEPROM_MAX_ADDR; i32+=MEMORY_FLASH_MAX_WRITE_SIZE)
            {
                for(i16=0; i16<MEMORY_FLASH_MAX_WRITE_SIZE; i16++)
                {
                    MemberBuf[i16]=i16;
                }
                //д256
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_WRITE,i32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res!=OK)break;
                //�建��
                memset(MemberBuf,0,MEMORY_FLASH_MAX_WRITE_SIZE);
                //��256
                res=Memory_EnterFunction(MEMORY_ENTRY_CMD_EEPROM_READ,i32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res!=OK)break;
                //�ȶ�
                for(i16=0; i16<MEMORY_FLASH_MAX_WRITE_SIZE; i16++)
                {
                    if(i16==MemberBuf[i16]);
                    else break;
                }
                if(i16==MEMORY_FLASH_MAX_WRITE_SIZE)
                {
                    //�ȶԳɹ�
                    if((i32!=0) &&((i32/MEMORY_FLASH_MAX_WRITE_SIZE)%(1024L/MEMORY_FLASH_MAX_WRITE_SIZE)==0))
                    {
                        //sprintf((char *)Member_PrintBuf,"%ld KB OK.\r\n",(i32/1024));
                        //DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                    }
                }
                else
                {
                    //�ȶ�ʧ��
                    sprintf((char *)Member_PrintBuf,"EEPROM CHIP TEST Addr %ld ERR!\r\n",i32);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                    res=ERR;
                    break;
                }
            }
            j32=Memory_msCount;
            if(res==OK)
            {
                sprintf((char *)Member_PrintBuf,"%ld KB OK.\r\n",(i32/1024));
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                sprintf((char *)Member_PrintBuf,"EEPROM CHIP TEST(Count:%d,Timer:%ld(ms)) OK!\r\n",k16,j32);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                break;
            }
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test EEPROM CHIP End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
    //-----����FLASH��д����
    if(cmd& Memory_TEST_WR_FLASH)
    {
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Flash CHIP Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //��ʾ�û��������
        sprintf((char *)Member_PrintBuf,"Please Input Test Count(1-100):\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        res = DebugInputNum(&j16,5);
        DebugOut((INT8S*)"\r\n",strlen("\r\n"));
        //
        if(res!=OK || j16==0 || j16>100)
        {
            DebugOut((INT8S*)"Input Error\r\n",strlen("Input Error\r\n"));
            goto Goto_MemoryTest1;
        }
        //
        for(k16=1; k16<=j16; k16++)
        {
            //��Ƭ����
            res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_CHIP_ERASE,0,0,0);
            //�ַ���д(256B)
            for(i32=0; i32<MEMORY_FLASH_MAX_ADDR; i32+=MEMORY_FLASH_MAX_WRITE_SIZE)
            {
                for(i16=0; i16<MEMORY_FLASH_MAX_WRITE_SIZE; i16++)
                {
                    MemberBuf[i16]=i16;
                }
                //�鴦��ҳ
                /*
                if(addr%MEMORY_FLASH_MAX_WRITE_SIZE==0)
                {
                    res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_4KB_ERASE,addr,0,0);
                    if(res!=OK)break;
                }
                */
                //д256
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_WRITE,i32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res!=OK)break;
                //�建��
                memset(MemberBuf,0,MEMORY_FLASH_MAX_WRITE_SIZE);
                //��256
                res = Memory_EnterFunction(MEMORY_ENTRY_CMD_FLASH_READ,i32,MemberBuf,MEMORY_FLASH_MAX_WRITE_SIZE);
                if(res!=OK)break;
                //�ȶ�
                for(i16=0; i16<MEMORY_FLASH_MAX_WRITE_SIZE; i16++)
                {
                    if(i16==MemberBuf[i16]);
                    else break;
                }
                if(i16==MEMORY_FLASH_MAX_WRITE_SIZE)
                {
                    //�ȶԳɹ�
                    if((i32!=0)&&(i32/MEMORY_FLASH_MAX_WRITE_SIZE)%(1024L*1024/MEMORY_FLASH_MAX_WRITE_SIZE)==0)
                    {
                        sprintf((char *)Member_PrintBuf,".%ld MB OK.\r\n",i32/(1024L*1024));
                        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                    }
                    else if((i32/MEMORY_FLASH_MAX_WRITE_SIZE)%(1024L*32/MEMORY_FLASH_MAX_WRITE_SIZE)==0)
                    {
                        DebugOut(".",1);
                    }
                }
                else
                {
                    //�ȶ�ʧ��
                    sprintf((char *)Member_PrintBuf,"Flash CHIP TEST Addr %ld ERR!\r\n",i32);
                    DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                    res=ERR;
                    break;
                }
            }
            if(res==OK)
            {
                sprintf((char *)Member_PrintBuf,".%ld MB OK.\r\n",i32/(1024L*1024));
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
                sprintf((char *)Member_PrintBuf,"Flash CHIP TEST(Count:%d) OK!\r\n",k16);
                DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
            }
            else
            {
                ;
            }
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Flash CHIP End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
    }
#ifdef STM32
    //-----FLASH�ֿ��ʽ������
    if(cmd&Memory_TEST_FLASH_FORMAT)
    {
#if 1
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Flash Format Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,MEMORY_FLASH_ALARM_RECORD_ADDR,0,(MEMORY_FLASH_ALARM_RECORD_ADDR_END-MEMORY_FLASH_ALARM_RECORD_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"ALARM_RECORD Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,MEMORY_FLASH_CARD_RECORD_ADDR,0,(MEMORY_FLASH_CARD_RECORD_ADDR_END-MEMORY_FLASH_CARD_RECORD_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"CARD_RECORD Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_FLASH_USER_PERMISSION_ADDR+16*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                             (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"VISITOR Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        for(i=0; i<16; i++)
        {
            i32=Memory_msCount=0;
            Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                                 MEMORY_FLASH_USER_PERMISSION_ADDR+i*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE,0,
                                 (MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_SIZE)/MEMORY_FLASH_MIN_ERASE_SIZE);
            i32=Memory_msCount;
            sprintf((char *)Member_PrintBuf,"USER%02d Time:%ldms\r\n",i,i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_FLASH_HOLIDAYGROUP_ADDR,0,
                             (MEMORY_FLASH_HOLIDAYGROUP_ADDR_END-MEMORY_FLASH_HOLIDAYGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"HOLIDAYGROUP Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_FLASH_TIMEGROUP_ADDR,0,
                             (MEMORY_FLASH_TIMEGROUP_ADDR_END-MEMORY_FLASH_TIMEGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"TIMEGROUP Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_DOORGROUP_ADDR,0,
                             (MEMORY_DOORGROUP_ADDR_END-MEMORY_DOORGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"DOORGROUP Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_CARDGROUPLIMIT_ADDR,0,
                             (MEMORY_CARDGROUPLIMIT_ADDR_END-MEMORY_CARDGROUPLIMIT_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"CARDGROUPLIMIT Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_AREA_ADDR,0,
                             (MEMORY_AREA_ADDR_END-MEMORY_AREA_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"AREA Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Test Flash Format End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=TRUE;
#else
        i32=Memory_msCount=0;
        Memory_EnterFunction(MEMORY_ENTRY_CMD_FALSH_MANY_SECTOR_ERASE,
                             MEMORY_FLASH_TIMEGROUP_ADDR,0,
                             (MEMORY_FLASH_TIMEGROUP_ADDR_END-MEMORY_FLASH_TIMEGROUP_ADDR)/MEMORY_FLASH_MIN_ERASE_SIZE);
        i32=Memory_msCount;
        sprintf((char *)Member_PrintBuf,"TIMEGROUP Time:%ldms\r\n",i32);
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        sign_exe=FALSE;
#endif
    }
#endif
    //-----��ʼ��(������ƬEEPROM��д��ͷ(523ms����),������ƬFLASH(35ms����))
    if(sign_exe==TRUE || cmd&Memory_TEST_PRE_CONF)
    {
        //
        sprintf((char *)Member_PrintBuf,"----------Memory Init Begin:\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        //
        i32=Memory_msCount=0;
        res = Memory_InternalApp(MEMORY_APP_CMD_FORMAT,0,MEMORY_FORMAT_ALL,0);
        if(res==MEMORY_APP_ERR_NO)
        {
            i32=Memory_msCount;
            sprintf((char *)Member_PrintBuf,"Memory Init Time:%ldms\r\n",i32);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        else
        {
            sprintf((char *)Member_PrintBuf,"Memory Init Err:%d\r\n",res);
            DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
        }
        //
        sprintf((char *)Member_PrintBuf,"-----Memory Init End\r\n");
        DebugOut((INT8S*)Member_PrintBuf,strlen((char *)Member_PrintBuf));
    }
Goto_MemoryTest1:
    //�ͷŻ���
    MemManager_Free(MemberTestBuf,&res);
#ifdef UCOS
    OSSemPost(Sem_EepromFlashUse);
#else
    i=0;
#endif
}
//-------------------------------------------------------------------------------

