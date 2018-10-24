//-------------------------------------------------------------------------------//
//                    ��������: RealView MDK-ARM Version 5.10                    //
//                    �� �� ��: RealView MDK-ARM Version 5.10                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: W5500.c                                          //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-08-06    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif

#ifdef __W5500APP_H
//------------------------------- �û����� --------------------------
#ifdef STM32
static  OS_STK  App_TaskW5500AppStk[APP_TASK_W5500APP_STK_SIZE];
#endif

#ifdef STM32

//-------------------------------------------------------------------
/*******************************************************************************
��������: NET���񴴽�
*******************************************************************************/
static void uctsk_W5500App(void);
void  App_W5500AppTaskCreate (void)
{
    CPU_INT08U  os_err;
    os_err = os_err;
    os_err = OSTaskCreateExt(//������
                 (void (*)(void *))uctsk_W5500App,
                 //����
                 (void *)0,
                 //��ջ
                 (OS_STK *)&App_TaskW5500AppStk[APP_TASK_W5500APP_STK_SIZE-1],
                 //���ȼ�
                 APP_TASK_W5500APP_PRIO,
                 APP_TASK_W5500APP_PRIO,
                 (OS_STK *)&App_TaskW5500AppStk[0],
                 APP_TASK_W5500APP_STK_SIZE,
                 (void *)0,
                 OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
             );
#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_W5500APP_PRIO, "Task W5500App", &os_err);
#endif
}
#endif
/*******************************************************************************
��������: NET����ʵ��
*******************************************************************************/
#ifdef STM32
static void uctsk_W5500App (void)
{
    for(;;)
    {
        OSTimeDlyHMSM(0, 0, 0, 100);
        //-----------------------
        //W5500APP
        W5500APP_100ms();
    }
}
#endif
//-------------------------------------------------------------------
//�������������ʼ��
static void W5500APP_LocalNetInit(void)
{
    uint8_t res;
    S_MEMORY_NET *p_s_net;
    p_s_net = MemManager_Get(256,&res);
    Memory_AppFunction(MEMORY_APP_CMD_NET_R,(uint8_t*)p_s_net,0,0);
    //��ʼ���ṹ��
    memset((uint8_t*)&W5500_S_Config,0x00,sizeof(W5500_S_CONFIG));
    //����     -> 192.168.1.1
    W5500_S_Config.Gateway_IP[0] = p_s_net->Gateway_IP[0];
    W5500_S_Config.Gateway_IP[1] = p_s_net->Gateway_IP[1];
    W5500_S_Config.Gateway_IP[2] = p_s_net->Gateway_IP[2];
    W5500_S_Config.Gateway_IP[3] = p_s_net->Gateway_IP[3];
    //�������� -> 255.255.255.0
    W5500_S_Config.Sub_Mask[0]=p_s_net->Sub_Mask[0];
    W5500_S_Config.Sub_Mask[1]=p_s_net->Sub_Mask[1];
    W5500_S_Config.Sub_Mask[2]=p_s_net->Sub_Mask[2];
    W5500_S_Config.Sub_Mask[3]=p_s_net->Sub_Mask[3];
    //�����ַ -> ����
    W5500_S_Config.Phy_Addr[0]=p_s_net->Phy_Addr[0];
    W5500_S_Config.Phy_Addr[1]=p_s_net->Phy_Addr[1];
    W5500_S_Config.Phy_Addr[2]=p_s_net->Phy_Addr[2];
    W5500_S_Config.Phy_Addr[3]=p_s_net->Phy_Addr[3];
    W5500_S_Config.Phy_Addr[4]=p_s_net->Phy_Addr[4];
    W5500_S_Config.Phy_Addr[5]=p_s_net->Phy_Addr[5];
    //����IP   -> 192.168.1.233
    W5500_S_Config.IP_Addr[0]=p_s_net->IP_Local[0];
    W5500_S_Config.IP_Addr[1]=p_s_net->IP_Local[1];
    W5500_S_Config.IP_Addr[2]=p_s_net->IP_Local[2];
    W5500_S_Config.IP_Addr[3]=p_s_net->IP_Local[3];
    //DNS������
    W5500_S_Config.DNS_IP[0]=p_s_net->DNS_IP[0];
    W5500_S_Config.DNS_IP[1]=p_s_net->DNS_IP[1];
    W5500_S_Config.DNS_IP[2]=p_s_net->DNS_IP[2];
    W5500_S_Config.DNS_IP[3]=p_s_net->DNS_IP[3];
    W5500_S_Config.DNS_Port=53;
    //
    W5500_S_Config.LOCK=0x55;
    //�ͷŻ���
    MemManager_Free(p_s_net,&res);
}
//Ŀ�������ʼ��
static void W5500APP_DestNetInit(void)
{
    uint8_t res;
    S_MEMORY_NET *p_s_net;
    p_s_net = MemManager_Get(256,&res);
    Memory_AppFunction(MEMORY_APP_CMD_NET_R,(uint8_t*)p_s_net,0,0);
    //----------���ö˿�0----------DEBUG����
    memset((uint8_t*)&W5500_S_Connect[0],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[0].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[0].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[0].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[0].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[0].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[0].DestPort = 8240;
    W5500_S_Connect[0].LocalPort= 5000;
    //----------���ö˿�1----------��ʽ����
    memset((uint8_t*)&W5500_S_Connect[1],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[1].order=W5500_E_CONNECT_ORDER_TCP_IP_CLIENT;
    W5500_S_Connect[1].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[1].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[1].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[1].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[1].DestPort =p_s_net->PORT_Dest1;
    W5500_S_Connect[1].LocalPort= 6001;
    //----------���ö˿�2----------������������
    memset((uint8_t*)&W5500_S_Connect[2],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[2].order=W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT;
    memset((char*)W5500_S_Connect[2].DestIp,0,4);
    strcpy((char*)W5500_S_Connect[2].DestDnmain,"lnmodern.vicp.net");
    W5500_S_Connect[2].DestPort = 7008;
    W5500_S_Connect[2].LocalPort= 5002;
    //----------���ö˿�3----------
    memset((uint8_t*)&W5500_S_Connect[3],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[3].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[3].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[3].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[3].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[3].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[3].DestPort = 8080;
    W5500_S_Connect[3].LocalPort= 5003;
    //----------���ö˿�4----------
    memset((uint8_t*)&W5500_S_Connect[4],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[4].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[4].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[4].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[4].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[4].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[4].DestPort = 8080;
    W5500_S_Connect[4].LocalPort= 5004;
    //----------���ö˿�5----------
    memset((uint8_t*)&W5500_S_Connect[5],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[5].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[5].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[5].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[5].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[5].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[5].DestPort = 8080;
    W5500_S_Connect[5].LocalPort= 5005;
    //----------���ö˿�6----------
    memset((uint8_t*)&W5500_S_Connect[6],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[6].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[6].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[6].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[6].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[6].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[6].DestPort = 8080;
    W5500_S_Connect[6].LocalPort= 5006;
    //----------���ö˿�7----------
    memset((uint8_t*)&W5500_S_Connect[7],0x00,sizeof(W5500_S_CONNECT));
    W5500_S_Connect[7].order=W5500_E_CONNECT_ORDER_UDP_IP;
    W5500_S_Connect[7].DestIp[0]=p_s_net->IP_Dest1[0];
    W5500_S_Connect[7].DestIp[1]=p_s_net->IP_Dest1[1];
    W5500_S_Connect[7].DestIp[2]=p_s_net->IP_Dest1[2];
    W5500_S_Connect[7].DestIp[3]=p_s_net->IP_Dest1[3];
    W5500_S_Connect[7].DestPort = 8080;
    W5500_S_Connect[7].LocalPort= 5007;
    //�ͷŻ���
    MemManager_Free(p_s_net,&res);
}
//
void W5500APP_100ms(void)
{
    static uint8_t first=1;
    //static INT8U ch0_sendTimer=0;
    //static INT8U ch1_sendTimer=0;
    static uint8_t ch2_sendTimer=1;
    static uint8_t ch3_sendTimer=1;
    static uint8_t ch4_sendTimer=1;
    static uint8_t ch5_sendTimer=1;
    static uint8_t ch6_sendTimer=1;
    static uint8_t ch7_sendTimer=1;
    if(W5500_S_Config.LOCK==0xaa)
    {
        first=1;
    }
    if(first==1)
    {
        first=0;
        //-----
        W5500APP_LocalNetInit();
        W5500APP_DestNetInit();
        W5500_Main_100ms(TRUE);
        //-----
    }
    //-----
    W5500_Main_100ms(FALSE);
    //-----
    //ͨ��0---��ʱ�����ַ���
    /*
    ch0_sendTimer++;
    if(ch0_sendTimer >= 100)
    {
        W5500_S_Connect[0].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(0,"ch0-Heart",strlen("ch0-Heart"));
        ch0_sendTimer=0;
    }
    */
    //ͨ��1---��ʱ�����ַ���
    /*
    ch1_sendTimer++;
    if(ch1_sendTimer >= 200)
    {
        W5500_S_Connect[1].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(1,"ch1-Heart",strlen("ch1-Heart"));
        ch1_sendTimer=0;
    }
    */
    //ͨ��2---��ʱ�����ַ���
    ch2_sendTimer++;
    if(ch2_sendTimer >= 30)
    {
        W5500_S_Connect[2].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(2,"ch2-Heart",strlen("ch2-Heart"));
        ch2_sendTimer=0;
    }
    //ͨ��3---��ʱ�����ַ���
    ch3_sendTimer++;
    if(ch3_sendTimer >= 30)
    {
        W5500_S_Connect[3].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(3,"ch3-Heart",strlen("ch3-Heart"));
        ch3_sendTimer=0;
    }
    //ͨ��4---��ʱ�����ַ���
    ch4_sendTimer++;
    if(ch4_sendTimer >= 30)
    {
        W5500_S_Connect[4].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(4,"ch4-Heart",strlen("ch4-Heart"));
        ch4_sendTimer=0;
    }
    //ͨ��5---��ʱ�����ַ���
    ch5_sendTimer++;
    if(ch5_sendTimer >= 30)
    {
        W5500_S_Connect[5].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(5,"ch5-Heart",strlen("ch5-Heart"));
        ch5_sendTimer=0;
    }
    //ͨ��6---��ʱ�����ַ���
    ch6_sendTimer++;
    if(ch6_sendTimer >= 30)
    {
        W5500_S_Connect[6].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(6,"ch6-Heart",strlen("ch6-Heart"));
        ch6_sendTimer=0;
    }
    //ͨ��7---��ʱ�����ַ���
    ch7_sendTimer++;
    if(ch7_sendTimer >= 30)
    {
        W5500_S_Connect[7].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_TX_OK;
        //ָ��Socket(0~3)�������ݴ���,�˿�0����23�ֽ�����
        W5500_App_Tx(7,"ch7-Heart",strlen("ch7-Heart"));
        ch7_sendTimer=0;
    }
}
//����
void W5500APP_ProtocolAnalysis_Ch0(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    if(port==8240)
    {
        Debug_InterruptRx(pbuf,len);
    }
}
void W5500APP_ProtocolAnalysis_Ch1(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    //�ش�����
    //W5500_App_Tx(1,pbuf,len);
    //INT8U txLen;
    uint8_t *p_i8;
    uint8_t res;
    uint8_t len_8=len;
    uint16_t crc16;
    //���뻺��
    p_i8=MemManager_Get(256,&res);
    //����CRC16
    crc16=Count_CRC16(pbuf,len_8);
    pbuf[len_8++] = crc16;
    pbuf[len_8++] = crc16>>8;
    //CRC16У��
    crc16=Count_CRC16(pbuf,len_8-2);
    if(crc16 != (pbuf[len_8-1] << 8) + pbuf[len_8-2])
    {
        ;
    }
    else
    {
        //���ݽ���
        AES128_decrypt_Project(NULL,&pbuf[1],&pbuf[1],&len_8);
        //
        //ModbusDebug(0,pbuf,&len_8);
        //��������
        //TransferParse(pbuf,&len_8,p_i8,&txLen);
        //
/*
#ifndef STM32
        if(txLen>=(3+MODBUS_LEN_SIZE) && txLen<=250 && (Uart1.TxTimeOutCnt<MAX_ASK_TIME_MS ))
#else
        if(txLen>=(3+MODBUS_LEN_SIZE) && txLen<=250 && (Modbus_NoTx_Overtime_ms<MAX_ASK_TIME_MS ))
#endif
        {
            //���ݱ���
            AES128_encrypt_Project(NULL,&p_i8[1],&p_i8[1],&txLen);
            //����
            W5500_App_Tx(1,p_i8,txLen);
            //
            ModbusDebug(1,p_i8,&txLen);
        }
*/        
    }
    //�ͷŻ���
    MemManager_Free(p_i8,&res);
    //���ݺ��ڴ���
    //TransferWriteParaPro();
}
void W5500APP_ProtocolAnalysis_Ch2(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(2,pbuf,len);
}
void W5500APP_ProtocolAnalysis_Ch3(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(3,pbuf,len);
}
void W5500APP_ProtocolAnalysis_Ch4(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(4,pbuf,len);
}
void W5500APP_ProtocolAnalysis_Ch5(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(5,pbuf,len);
}
void W5500APP_ProtocolAnalysis_Ch6(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(6,pbuf,len);
}
void W5500APP_ProtocolAnalysis_Ch7(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)
{
    W5500_App_Tx(7,pbuf,len);
}
//-------------------------------------------------------------------------------
// ��������: void W5500APP_DEBUG_SET_NET(void)
// ��������: �����������
// ��ڲ���:
// ���ز���:
// �޸�˵����
// �޸�ʱ�䣺
// ע    ��:
//-------------------------------------------------------------------------------
void W5500APP_DebugSetNetPara(void)
{
    uint8_t *p_i8;
    S_MEMORY_NET *p_s_net;
    uint8_t res;
    uint8_t i,d;
    uint16_t i16,len;
    uint8_t buf[4];
    p_i8 = MemManager_Get(256,&res);
    p_s_net = (S_MEMORY_NET*)&p_i8[100];
    Memory_AppFunction(MEMORY_APP_CMD_NET_R,(uint8_t*)p_s_net,0,0);
    //
    sprintf((char*)p_i8,"�����������,[Enter]������ǰֵ,[Esc]�˳�����.\r\n");
    DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
    //��������IP
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"����������IP(��ǰ: %03d.%03d.%03d.%03d)\r\n",p_s_net->Gateway_IP[0],p_s_net->Gateway_IP[1],p_s_net->Gateway_IP[2],p_s_net->Gateway_IP[3]);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        memset((uint8_t*)p_i8,0x00,20);
        //����15���ַ�
        for(i=0; i<15; i++)
        {
            DebugIn((int8_t*)&d,&len);
            //ESC��������
            if(len==1 && d==27)
            {
                goto  goto_setnet;
            }
            else if(len==1&&(d>='0'&&d<='9'))
            {
                p_i8[i]=d;
            }
            else if(len==1&&d=='.')
            {
                p_i8[i]='.';
            }
            else
            {
                p_i8[i]=0;
                break;
            }
        }
        if(i==0&&(d==0x0d||d==0x0a))
        {
            break;
        }
        res = Count_IP_Format(p_i8,buf);
        if(res==OK)
        {
            p_s_net->Gateway_IP[0]=buf[0];
            p_s_net->Gateway_IP[1]=buf[1];
            p_s_net->Gateway_IP[2]=buf[2];
            p_s_net->Gateway_IP[3]=buf[3];
            break;
        }
        else
        {
            continue;
        }
    }
    //������������
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"��������������(��ǰ: %03d.%03d.%03d.%03d)\r\n",p_s_net->Sub_Mask[0],p_s_net->Sub_Mask[1],p_s_net->Sub_Mask[2],p_s_net->Sub_Mask[3]);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        memset((uint8_t*)p_i8,0x00,20);//����15���ַ�
        for(i=0; i<15; i++)
        {
            DebugIn((int8_t*)&d,&len);
            //ESC��������
            if(len==1 && d==27)
            {
                goto  goto_setnet;
            }
            else if(len==1&&(d>='0'&&d<='9'))
            {
                p_i8[i]=d;
            }
            else if(len==1&&d=='.')
            {
                p_i8[i]='.';
            }
            else
            {
                p_i8[i]=0;
                break;
            }
        }
        if(i==0&&(d==0x0d||d==0x0a))
        {
            break;
        }
        res = Count_IP_Format(p_i8,buf);
        if(res==OK)
        {
            p_s_net->Sub_Mask[0]=buf[0];
            p_s_net->Sub_Mask[1]=buf[1];
            p_s_net->Sub_Mask[2]=buf[2];
            p_s_net->Sub_Mask[3]=buf[3];
            break;
        }
        else
        {
            continue;
        }
    }
    //���뱾��IP
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"�����뱾��IP(��ǰ: %03d.%03d.%03d.%03d)\r\n",p_s_net->IP_Local[0],p_s_net->IP_Local[1],p_s_net->IP_Local[2],p_s_net->IP_Local[3]);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        memset((uint8_t*)p_i8,0x00,20);
        //����15���ַ�
        for(i=0; i<15; i++)
        {
            DebugIn((int8_t*)&d,&len);
            //ESC��������
            if(len==1 && d==27)
            {
                goto  goto_setnet;
            }
            else if(len==1&&(d>='0'&&d<='9'))
            {
                p_i8[i]=d;
            }
            else if(len==1&&d=='.')
            {
                p_i8[i]='.';
            }
            else
            {
                p_i8[i]=0;
                break;
            }
        }
        if(i==0&&(d==0x0d||d==0x0a))
        {
            break;
        }
        res = Count_IP_Format(p_i8,buf);
        if(res==OK)
        {
            p_s_net->IP_Local[0]=buf[0];
            p_s_net->IP_Local[1]=buf[1];
            p_s_net->IP_Local[2]=buf[2];
            p_s_net->IP_Local[3]=buf[3];
            break;
        }
        else
        {
            continue;
        }
    }
    //����DNS������IP
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"������DNS������IP(��ǰ: %03d.%03d.%03d.%03d)\r\n",p_s_net->DNS_IP[0],p_s_net->DNS_IP[1],p_s_net->DNS_IP[2],p_s_net->DNS_IP[3]);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        memset((uint8_t*)p_i8,0x00,20);
        //����15���ַ�
        for(i=0; i<15; i++)
        {
            DebugIn((int8_t*)&d,&len);
            //ESC��������
            if(len==1 && d==27)
            {
                goto  goto_setnet;
            }
            else if(len==1&&(d>='0'&&d<='9'))
            {
                p_i8[i]=d;
            }
            else if(len==1&&d=='.')
            {
                p_i8[i]='.';
            }
            else
            {
                p_i8[i]=0;
                break;
            }
        }
        if(i==0&&(d==0x0d||d==0x0a))
        {
            break;
        }
        res = Count_IP_Format(p_i8,buf);
        if(res==OK)
        {
            p_s_net->DNS_IP[0]=buf[0];
            p_s_net->DNS_IP[1]=buf[1];
            p_s_net->DNS_IP[2]=buf[2];
            p_s_net->DNS_IP[3]=buf[3];
            break;
        }
        else
        {
            continue;
        }
    }
    //����Ŀ��IP
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"������Ŀ��1-IP(��ǰ: %03d.%03d.%03d.%03d)\r\n",p_s_net->IP_Dest1[0],p_s_net->IP_Dest1[1],p_s_net->IP_Dest1[2],p_s_net->IP_Dest1[3]);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        memset((uint8_t*)p_i8,0x00,20);
        //����15���ַ�
        for(i=0; i<15; i++)
        {
            DebugIn((int8_t*)&d,&len);
            //ESC��������
            if(len==1 && d==27)
            {
                goto  goto_setnet;
            }
            else if(len==1&&(d>='0'&&d<='9'))
            {
                p_i8[i]=d;
            }
            else if(len==1&&d=='.')
            {
                p_i8[i]='.';
            }
            else
            {
                p_i8[i]=0;
                break;
            }
        }
        if(i==0&&(d==0x0d||d==0x0a))
        {
            break;
        }
        res = Count_IP_Format(p_i8,buf);
        if(res==OK)
        {
            p_s_net->IP_Dest1[0]=buf[0];
            p_s_net->IP_Dest1[1]=buf[1];
            p_s_net->IP_Dest1[2]=buf[2];
            p_s_net->IP_Dest1[3]=buf[3];
            break;
        }
        else
        {
            continue;
        }
    }
    //����Ŀ��PORT
    while(1)
    {
        //��ӡ��ʾ��Ϣ
        sprintf((char*)p_i8,"������Ŀ��1-PORT(��ǰ: %05d)\r\n",p_s_net->PORT_Dest1);
        DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
        i16=p_s_net->PORT_Dest1;
        //
        res=DebugInputNum(&i16,5);
        if(res==OK)
        {
            if(i16!=0)
            {
                p_s_net->PORT_Dest1=i16;
            }
            break;
        }
        else
        {
            continue;
        }
    }
    //����
    p_s_net->Phy_Addr[2]=p_s_net->IP_Local[0];
    p_s_net->Phy_Addr[3]=p_s_net->IP_Local[1];
    p_s_net->Phy_Addr[4]=p_s_net->IP_Local[2];
    p_s_net->Phy_Addr[5]=p_s_net->IP_Local[3];
    Memory_AppFunction(MEMORY_APP_CMD_NET_W,(uint8_t*)p_s_net,0,0);
    sprintf((char*)p_i8,"����������óɹ�!\r\n");
    DebugOut((int8_t*)p_i8,strlen((char*)p_i8));
    //��־��λ
    W5500_S_Config.LOCK=0xAA;
goto_setnet:
    //�ͷŻ���
    MemManager_Free(p_i8,&res);
}
#endif
