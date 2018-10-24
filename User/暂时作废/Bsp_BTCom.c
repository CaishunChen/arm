#include "Bsp_BTCom.h"
#include "Module_Memory.h"
#include "Bsp_CpuFlash.h"

static uint8_t BSP_BTCOM_Hand_Flag=1;
static uint8_t BSP_BTCOM_Result_Flag=1;
static uint8_t BSP_BTCOM_PackageFlag;

static uint8_t BSP_BTCOM_ID=0;
//ͨѶ״̬ (0-����,1-���ݴ�����)
uint8_t BTCom_DataTransmissionState=0;
//
uint8_t BSP_BTCOM_tempbuf[100];
//ww
extern uint8_t Bluetooth_Tx(uint8_t *pBuf,uint16_t len);
//
static uint8_t Bsp_BTCom_handshake_send(void);
static uint8_t Bsp_BTCom_handshake_receive(uint8_t *buf_rx);
static uint8_t Bsp_BTCom_Send(uint16_t DataNum);
/*******************************************************************************
��������:�������ְ�����
*******************************************************************************/
static uint8_t Bsp_BTCom_handshake_send(void)
{
    uint8_t ret;
    uint8_t buf[20]= {0};

    buf[0] = 0x5A;
    buf[1] = 0xA5;
    buf[2] = 20;//��ʱ����
    ret =  Bluetooth_Tx(buf, 20);
    Count_DelayMs(3000);
    return ret;
}
/*******************************************************************************
��������: �������ְ������ж�
*******************************************************************************/
static uint8_t Bsp_BTCom_handshake_receive(uint8_t *buf_rx)
{
    uint8_t ret;
    if ((buf_rx[0] == 0x5A) && (buf_rx[1] == 0xA5))
        ret = OK;
    else
        ret = ERR;
    return ret;
}
/*******************************************************************************
��������: �������ݰ������ж�
*******************************************************************************/
static uint8_t Bsp_BTCom_result_receive(uint8_t *buf_rx)
{
    uint8_t ret;
    if ((buf_rx[0] == 0x5A) && (buf_rx[1] == 0x01) && (buf_rx[2] == 0xA5))
        ret = OK;
    else
        ret = ERR;
    return ret;
}
/*******************************************************************************
* ��������: ���������������ݣ���5�����β�˳��Ϊ����˯�����ݡ�˯�����ݡ��������ݡ�
          �������ݡ��嶯����
* ��    ��: DataNum  -  ���ݰ���(0 ��ʼ)
*******************************************************************************/
//�����ϴ�
static uint8_t Bsp_BTCom_Send(uint16_t DataNum)
{
    uint8_t *pbuf;
    uint8_t res;
    if(BSP_BTCOM_Result_Flag == ERR || BSP_BTCOM_Hand_Flag == ERR)
    {
        Bsp_BTCom_handshake_send();
        BSP_BTCOM_Hand_Flag = Bsp_BTCom_handshake_receive(BSP_BTCOM_tempbuf);
    }
    if(BSP_BTCOM_Hand_Flag == OK)
    {
        //���뻺��
        pbuf = MemManager_Get(256,&res);
        {
            BSP_CPU_FLASH_S_PARA *pspara;
            pspara=(BSP_CPU_FLASH_S_PARA*)pbuf;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
            res = pspara->MemoryDayNextP;
            res = Count_SubCyc(res,DataNum+1,MODULE_MEMORY_ADDR_DAY_NUM-1);
        }
        //��˯������
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,pbuf,&res);
            Bluetooth_Tx(pbuf, 72);
            Count_DelayMs(500);
        }
        //˯������
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,pbuf,&res);
            Bluetooth_Tx(pbuf, 243);
            Count_DelayMs(500);
        }
        //��������
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_R,pbuf,&res);
            Bluetooth_Tx(pbuf, 243);
            Count_DelayMs(500);
        }
        //��������
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,pbuf,&res);
            Bluetooth_Tx(pbuf, 243);
            Count_DelayMs(500);
        }
        //�嶯����
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,pbuf,&res);
            Bluetooth_Tx(pbuf, 243);
            Count_DelayMs(3000);
        }
        BSP_BTCOM_Result_Flag = Bsp_BTCom_result_receive(BSP_BTCOM_tempbuf);
        //�ͷŻ���
        MemManager_Free(pbuf,&res);
    }
    return BSP_BTCOM_Result_Flag;
}
/*******************************************************************************
��������:   �����ϴ�
��    ��:   mode     -  ���λ���(BSP_BTCOM_ONCE_MODE/BSP_BTCOM_FULL_MODE)
            repeat   -  ����ʧ���ش�����
*******************************************************************************/
//
uint8_t Bsp_BTCOM_Upload(uint8_t mode)
{
    uint8_t RET;
    uint8_t repeat=2;
    BTCom_DataTransmissionState = 1;
    if(mode == BSP_BTCOM_ONCE_MODE)
    {
        if(repeat == 0)
            repeat = 1;
        while(repeat--)
        {
            BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(0);
            if(BSP_BTCOM_PackageFlag==0)
            {
                BSP_BTCOM_Hand_Flag=1;
                BSP_BTCOM_Result_Flag=1;
                RET = 1;
                break;
            }
            else
            {
                RET = 0;
                continue;
            }
        }
    }
    else
    {
        if(repeat == 0)
            repeat = 1;
        while(repeat--)
        {
            if(BSP_BTCOM_ID==0)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(0);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=1;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==1)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(1);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=2;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==2)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(2);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=3;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==3)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(3);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=4;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==4)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(4);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=5;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==5)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(5);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=6;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
            if(BSP_BTCOM_ID==6)
            {
                BSP_BTCOM_PackageFlag=Bsp_BTCom_Send(6);
                if(BSP_BTCOM_PackageFlag==0)
                {
                    BSP_BTCOM_ID=0;
                    BSP_BTCOM_Hand_Flag=1;
                    BSP_BTCOM_Result_Flag=1;
                    RET = 1;
                    break;
                }
                else
                {
                    RET = 0;
                    continue;
                }
            }
        }
    }
    BTCom_DataTransmissionState=0;
    return RET;
}
