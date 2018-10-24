/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#define DEBUG_GLOBAL
#include "uctsk_Debug.h"
#include "Bsp_Rtc.h"
#if   (DEBUG_RTT_ENABLE==1)
#include "SEGGER_RTT.h"
#endif
#ifdef   DEBUG_UART
#include "Bsp_Uart.h"
#endif
//------------------------------- ��̬���� -----------------------------------
static uint8_t Debug_InitSign=0;
static DEBUG_E_LEVEL Debug_Level=DEBUG_LOG_LEVEL;
static uint8_t Debug_LogOnOff=ON;
//-------------------------------------------------------------------------------
#define DEBUG_MENUTAB_NUM (sizeof(Debug_MenuTab)/sizeof(Debug_S_Menu))
//--------------------------------------------------------------------------------
#if   (DEBUG_RTT_ENABLE==1)
#define DEBUG_RTT_DELAY_MS(x) MODULE_OS_DELAY_MS(x)
#else
#define DEBUG_RTT_DELAY_MS(x)
#endif
//�������
//static INT8S DebugOutBuf[100];
//���뻺��
#define DEBUG_IN_BUF_MAX   5
static int8_t DebugInBuf[DEBUG_IN_BUF_MAX];
static uint16_t DebugInLen;
//�����ڲ�����
struct DebugRx
{
    uint8_t buf[2];
    uint8_t RxCount;
    uint8_t DebugCmd;
    uint8_t DebugRxState;  //0-������,1-��������,2-�������,3-�����˳�
} DebugRx;
/*******************************************************************************
��������: GSM�������񴴽�
*******************************************************************************/
MODULE_OS_TASK_TAB(AppTaskDebugTCB);
MODULE_OS_TASK_STK(AppTaskDebugStk,APP_CFG_TASK_DEBUG_STK_SIZE);
static  void  App_TaskDebug  (void *pvParameters);

void  App_DebugTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("App Task Debug",\
                          App_TaskDebug,\
                          APP_TASK_DEBUG_PRIO,\
                          AppTaskDebugStk,\
                          APP_CFG_TASK_DEBUG_STK_SIZE,\
                          AppTaskDebugTCB,\
                          NULL);
}
static void DebugPro_100ms(void);
static  void  App_TaskDebug(void *pvParameters)
{
    for(;;)
    {
        MODULE_OS_DELAY_MS(100);
#if   (defined(STM32F1) && defined(DEBUG_UART))
#if   (DEBUG_UART==0)
        BspUart_UsbVpcRx();
#endif
#endif        
        //���Գ���
        DebugPro_100ms();
    }
}
//-------------------------------------------------------------------------------
// ��������: void DebugInit(void)
// ��������: ��ں���-��ʼ��
// ��ڲ���: ��
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void DebugInit(void)
{
    if(Debug_InitSign==1)
    {
        return;
    }
#if   (defined(DEBUG_UART))
    UART_INIT(DEBUG_UART,115200);
#endif
    Debug_InitSign=1;
}
//-------------------------------------------------------------------------------
// ��������: void DebugOut(INT8S* str,INT8U Comlength)
// ��������: ��ں���-���Եȴ�����
// ��ڲ���: *str - Ҫ��ӡ������ Comlength - ��ӡ����
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void DebugIn(int8_t* str,uint16_t* Comlength)
{
    DebugInLen=0;
    while(1)
    {
        if(DebugInLen!=0)
        {
            memcpy(str,DebugInBuf,DebugInLen);
            if(Comlength!=NULL)
            {
                *Comlength = DebugInLen;
            }
            DebugInLen = 0;
            break;
        }
        //��ʱ
        MODULE_OS_DELAY_MS(10);
        //
#if   (DEBUG_RTT_ENABLE==1)
        {
            uint8_t len;
            uint8_t *DebugOutBuf;
            DebugOutBuf= MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            len=SEGGER_RTT_Read(0,DebugOutBuf,1);
            if(0!=len)
            {
                Debug_InterruptRx((uint8_t*)DebugOutBuf,len);
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,DebugOutBuf);
        }
#endif
    }
}
void DebugInClear(void)
{
#if   (DEBUG_RTT_ENABLE==1)
    uint8_t *DebugOutBuf;
    DebugOutBuf= MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    SEGGER_RTT_Read(0,DebugOutBuf,250);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,DebugOutBuf);
#endif
}
uint8_t DebugInputNum(uint16_t *Number,uint8_t MaxDight)
{
    uint8_t i,res=OK+10;
    uint16_t len,i16;
    int8_t buf[DEBUG_IN_BUF_MAX];
    for(*Number=i=0; i<MaxDight;)
    {
        if(res==OK||res==ERR)
        {
            break;
        }
        DebugIn((int8_t*)&buf,&len);
        i+=len;
        for(i16=0; i16<len; i16++)
        {

            if(buf[i16]>='0'&&buf[i16]<='9')
            {
                *Number*=10;
                *Number+=buf[i16]-'0';
            }
            else if(buf[i16]==0x0d || buf[i16]==0x0a )
            {
                res=OK;
                break;
            }
            else
            {
                res=ERR;
                break;
            }
        }
        if(i>=MaxDight)
        {
            res=OK;
        }
    }
    return res;
}
//-------------------------------------------------------------------------------
// ��������: void DebugOut(INT8S* str,INT8U Comlength)
// ��������: ��ں���-�������
// ��ڲ���: *str - Ҫ��ӡ������ Comlength - ��ӡ����
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
static uint8_t DebugLogHead(DEBUG_E_LEVEL level)
{
    uint8_t *DebugOutBuf;
    uint8_t i;
    DebugOutBuf=DebugOutBuf;
    if(level>Debug_Level || Debug_LogOnOff==OFF)
    {
        return ERR;
    }
#if 1
    DebugOutBuf= MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    // ʱ���
    {
        i=sprintf((char*)DebugOutBuf,"%04d-%02d-%02d %02d:%02d:%02d ",\
                  BspRtc_CurrentTimeBuf[0]+2000,\
                  BspRtc_CurrentTimeBuf[1],\
                  BspRtc_CurrentTimeBuf[2],\
                  BspRtc_CurrentTimeBuf[3],\
                  BspRtc_CurrentTimeBuf[4],\
                  BspRtc_CurrentTimeBuf[5]);
    }
    // ���
    {
        switch(level)
        {
            case DEBUG_E_LEVEL_ALWAYS:
                sprintf((char*)&DebugOutBuf[i],"ALWAY ");
                break;
            case DEBUG_E_LEVEL_ERROR:
                sprintf((char*)&DebugOutBuf[i],"ERROR ");
                break;
            case DEBUG_E_LEVEL_WARNING:
                sprintf((char*)&DebugOutBuf[i],"WARN  ");
                break;
            case DEBUG_E_LEVEL_INFO:
                sprintf((char*)&DebugOutBuf[i],"INFO  ");
                break;
            case DEBUG_E_LEVEL_DEBUG:
                sprintf((char*)&DebugOutBuf[i],"DEBUG ");
                break;
            default:
                break;
        }
    }
    // �ļ���Ϣ
    if(0)
    {
        sprintf((char*)DebugOutBuf,"file %s line %d : ",(int8_t *)__FILE__,__LINE__);
        DebugOut((int8_t*)DebugOutBuf,strlen((char*)DebugOutBuf));
    }
    DebugOutStr((int8_t*)DebugOutBuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,DebugOutBuf);
#endif
    return OK;
}

void DebugOut(int8_t* str,uint16_t Comlength)
{
    static uint8_t slock=0;
    /*
    if(Debug_InitSign==0)
    {
        return;
    }
    */
    while(slock==1)
    {
        MODULE_OS_DELAY_MS(1);
    }
    slock = 1;
    while(1)
    {
        if(Comlength>256)
        {
#if   (DEBUG_RTT_ENABLE==1)
            SEGGER_RTT_Write(0,(uint8_t*)str,256);
#endif
#if   (defined(DEBUG_UART))
            if(Debug_InitSign==1)
            {
                UART_DMA_Tx(DEBUG_UART,(uint8_t*)str,256);
            }
#endif
            //MODULE_OS_DELAY_MS(26+3);
            Comlength-=256;
        }
        else
        {
#if   (DEBUG_RTT_ENABLE==1)
            SEGGER_RTT_Write(0,(uint8_t*)str,Comlength);
            //MODULE_OS_DELAY_MS(Comlength/16);
#endif
#if   (defined(DEBUG_UART))
            if(Debug_InitSign==1)
            {
                UART_DMA_Tx(DEBUG_UART,(uint8_t*)str,Comlength);
            }
#endif
            break;
        }
    }
    slock=0;
}
void DebugLogOut(DEBUG_E_LEVEL level,int8_t* str,uint16_t Comlength)
{
    if(Debug_InitSign==0)
    {
        return;
    }
    if(ERR==DebugLogHead(level))
        return;
    while(1)
    {
        if(Comlength>256)
        {
#if   (DEBUG_RTT_ENABLE==1)
            SEGGER_RTT_Write(0,(uint8_t*)str,256);
            MODULE_OS_DELAY_MS(150);
#endif
#if   (defined(DEBUG_UART))
            UART_DMA_Tx(DEBUG_UART,(uint8_t*)str,256);
#if (defined(NRF51)||defined(NRF52))
            MODULE_OS_DELAY_MS(26+3);
#endif
#endif
            Comlength-=256;
        }
        else
        {
#if   (DEBUG_RTT_ENABLE==1)
            SEGGER_RTT_Write(0,(uint8_t*)str,Comlength);
#endif
#if   (defined(DEBUG_UART))
            UART_DMA_Tx(DEBUG_UART,(uint8_t*)str,Comlength);
#endif
            break;
        }
    }
}
void DebugOutStr(int8_t* str)
{
    DebugOut(str,strlen((char*)str));
}
void DebugLogOutStr(DEBUG_E_LEVEL level,int8_t* str)
{
    if(ERR==DebugLogHead(level))
        return;
    DebugOut(str,strlen((char*)str));
}
void DebugOutHex(char *pname,uint8_t*pbuf,int16_t len)
{
    uint8_t buf[5];
    uint16_t i16;
    if(pname!=NULL)
    {
        DebugOutStr((int8_t*)pname);
    }
    for(i16=0; i16<len; i16++)
    {
        sprintf((char*)buf,"%02x ",pbuf[i16]);
        DebugOutStr((int8_t*)buf);
    }
    DebugOutStr((int8_t*)"\r\n");
}
void DebugLogOutHex(DEBUG_E_LEVEL level,char *pname,uint8_t*pbuf,int16_t len)
{
    uint8_t buf[5];
    uint16_t i16;
    if(ERR==DebugLogHead(level))
        return;
    if(pname!=NULL)
    {
        DebugOutStr((int8_t*)pname);
    }
    for(i16=0; i16<len; i16++)
    {
        if((i16+1)==len)
        {
            sprintf((char*)buf,"%02x\r\n",pbuf[i16]);
        }
        else
        {
            sprintf((char*)buf,"%02x ",pbuf[i16]);
        }
        DebugOutStr((int8_t*)buf);
    }
}

//-------------------------------------------------------------------------------
// ��������: void Debug_InterruptRx(INT8U d)
// ��������: ���ں���-�����ڴ��ڽ����ж�
// ��ڲ���: d - ��������
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
void Debug_InterruptRx(uint8_t *d,uint16_t len)
{
    int8_t i;
    int8_t buf[1];
    uint16_t i16;
#ifdef   Debug_Rx_ExFun
    Debug_Rx_ExFun(d,len);
#endif
    //
    for (i16 = 0; i16 < len; i16++)
    {
        buf[0]=d[i16];
        DebugOut(buf,1);
        //���ݸ��Ƶ����ջ���
        if(len>DEBUG_IN_BUF_MAX)
        {
            DebugInLen = DEBUG_IN_BUF_MAX;
        }
        else
        {
            DebugInLen = len;
        }
        memcpy(DebugInBuf,d,DebugInLen);
        //������
        if(DebugRx.DebugRxState==0)
        {
            DebugRx.RxCount=0;
        }
        //��������
        else if(DebugRx.DebugRxState==1)
        {
            DebugRx.buf[DebugRx.RxCount++]=d[i16];
            if(   (DebugRx.RxCount==1 && (DebugRx.buf[0]==27 || DebugRx.buf[0]==DEBUG_HELP_CMD))\
                  || (DebugRx.RxCount>1  && (DebugRx.buf[1]==27 || DebugRx.buf[1]==DEBUG_HELP_CMD)))
            {
                DebugRx.DebugCmd=27;
                DebugRx.DebugRxState=2;
                DebugRx.RxCount=0;
            }
            else if(DebugRx.RxCount>=2)
            {
                if(DebugRx.buf[1]==0x0D)
                {
                    DebugRx.DebugCmd=DebugRx.buf[0];
                    DebugRx.DebugRxState=2;
                }
                DebugRx.RxCount=0;
            }
            else if(DebugRx.RxCount==1&&DebugRx.buf[0]==0x0D)
            {
                DebugRx.RxCount=0;
            }
        }
        //�������
        else if(DebugRx.DebugRxState==2)
        {
            if(d[i16]==27 || d[i16]==DEBUG_HELP_CMD)
            {
                DebugRx.DebugRxState=3;
                //
                for(i=0; i<DEBUG_MENUTAB_NUM; i++)
                {
                    if(DebugRx.DebugCmd==Debug_MenuTab[i].CmdCode
                       &&Debug_MenuTab[i].RunCount==0)
                    {
                        Debug_MenuTab[i].TestPro(OFF);
                        break;
                    }
                }
            }
        }
    }
}
//-------------------------------------------------------------------------------
// ��������: void DebugPro(void)
// ��������: ���ں���-�������
// ��ڲ���: *str - Ҫ��ӡ������ Comlength - ��ӡ����
// ���ز���: ��
// �޸�˵����
// �޸�ʱ�䣺
//-------------------------------------------------------------------------------
enum DEBUG_STEP
{
    DEBUG_STEP_INIT=0,
    DEBUG_STEP_MAIN_MENU,
    DEBUG_STEP_IDLE,
};
static void DebugPro_100ms(void)
{
    static uint8_t step=DEBUG_STEP_INIT;
    static uint16_t timer1=0;
    static uint16_t count1=0;
    static uint8_t first=1;
    struct tm *ptm;
    uint8_t i;
    //INT16U i16;
    int8_t *DebugOutBuf;
    //��Ƶ��ִ��
    timer1=10;
    if(timer1>=10)
    {
        timer1=0;
    }
    else
    {
        if(DebugRx.DebugRxState==2&&DebugRx.DebugCmd==27);
        else
        {
            timer1++;
            return;
        }
    }
    //���뻺��
    DebugOutBuf= MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if   (DEBUG_RTT_ENABLE==1)
    {
        uint8_t len;
        len=SEGGER_RTT_Read(0,DebugOutBuf,250);
        if(0!=len)
        {
            Debug_InterruptRx((uint8_t*)DebugOutBuf,len);
        }
    }
#endif
    //
    switch(step)
    {
        case DEBUG_STEP_IDLE:
            if(DebugRx.DebugRxState==2)
            {
                if(DebugRx.DebugCmd==27)
                {
                    step=DEBUG_STEP_INIT;
                    DebugRx.DebugRxState=1;
                }
                else
                {
                    //��������
                    DebugRx.DebugRxState=1;
                    break;
                }
            }
            else
            {
                break;
            }
        case DEBUG_STEP_INIT:
            DebugInit();
            count1=0;
            step=DEBUG_STEP_MAIN_MENU;
        case DEBUG_STEP_MAIN_MENU:
            //��ʼ��ҳ��
            if(count1==0)
            {
                count1++;
                //�ر�LOG
                Debug_LogOnOff=OFF;
                //����
                //��ȡ��ǰʱ��
                ptm = (struct tm *)&DebugOutBuf[128];
                BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
                //�ϸ�ע��:��֪ԭ������ǰ����\r\n,USB��ĳЩ�������ͺ�ԭ�����
                DebugOutStr((int8_t*)"\r\n**************************************************\r\n");
                sprintf((char*)DebugOutBuf,"*Welcome to use XSL-Shell(V2.2)\r\n");
                DebugOutStr(DebugOutBuf);
                sprintf((char*)DebugOutBuf,"*Author : XuSongliang. (QQ:5387603)\r\n");
                DebugOutStr(DebugOutBuf);
                sprintf((char*)DebugOutBuf,"*Product: %s(HW:%02d.%02d SW:%02d.%02d)(%04d-%02d-%02d)\r\n",\
                        PRODUCT_NAME,SOFTWARE_VER,SOFTWARE_SUB_VER,\
                        HARDWARE_VER,HARDWARE_SUB_VER,\
                        FACTORY_TIME_YEAR,FACTORY_TIME_MONTH,FACTORY_TIME_DAY);
                DebugOutStr(DebugOutBuf);
                sprintf((char*)DebugOutBuf,"*RTC    : %04d-%02d-%02d %02d:%02d:%02d(%d)\r\n",\
                        ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,ptm->tm_wday);
                DebugOutStr(DebugOutBuf);
                //-----test-----Begin
                //-----Test-----End
                DEBUG_RTT_DELAY_MS(10);
                DebugOutStr((int8_t*)"**************************************************\r\n");
                if(first==1)
                {
                    count1=DEBUG_QUIT_OVERTIME_S*10;
                }
                else
                {
                    for(i=0; i<DEBUG_MENUTAB_NUM; i++)
                    {
                        DebugOutStr(Debug_MenuTab[i].pNameStr);
                        DEBUG_RTT_DELAY_MS(10);
                    }
                    //
                    sprintf((char*)DebugOutBuf,"Please Press Code,Press (Enter) to end(<10S):\r\n");
                    DebugOutStr(DebugOutBuf);
                }
                //��������
                DebugRx.DebugRxState = 1;
            }
            //�ȴ�����
            else if(count1<DEBUG_QUIT_OVERTIME_S*10)
            {
                count1++;
                //����
                if(DebugRx.DebugRxState==2)
                {
                    for(i=0; i<DEBUG_MENUTAB_NUM; i++)
                    {
                        if(Debug_MenuTab[i].CmdCode==DebugRx.DebugCmd)
                        {
                            Debug_MenuTab[i].TestPro(ON);
                            if(Debug_MenuTab[i].RunCount==0)
                            {
                                count1=1;
                            }
                            else
                            {
                                DebugRx.DebugRxState=0;
                                count1=0;
                            }
                            break;
                        }
                    }
                    if(i==DEBUG_MENUTAB_NUM)
                    {
                        DebugRx.DebugRxState=0;
                        count1=0;
                    }
                }
                else if(DebugRx.DebugRxState==3)
                {
                    DebugRx.DebugRxState=0;
                    DebugRx.buf[0]=DebugRx.buf[1]=0;
                    count1=0;
                }
            }
            //��ʱ�˳�
            else
            {
                if(first==1)
                {
                    first=0;
                }
                else
                {
                    sprintf((char*)DebugOutBuf,"Overtime,Auto exit Shell.Press [ESC] or [$] Start!\r\n");
                    DebugOut(DebugOutBuf,strlen((char*)DebugOutBuf));
                }
                //��������
                DebugRx.DebugRxState = 1;
                DebugRx.buf[0]=DebugRx.buf[1]=0;
                step=DEBUG_STEP_IDLE;
                //����LOG
                Debug_LogOnOff=ON;
            }
            break;
        default:
            break;
    }
    //�ͷŻ���
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,DebugOutBuf);
}

/*
*********************************************************************************************************
*   �� �� ��: assert_failed
*   ��    �Σ�file : Դ�����ļ����ơ��ؼ���__FILE__��ʾԴ�����ļ�����
*             line �������кš��ؼ��� __LINE__ ��ʾԴ�����к�
*   �� �� ֵ: ��
*  ˵    ��:
    ST�⺯��ʹ����C�������Ķ��Թ��ܣ����������USE_FULL_ASSERT����ô���е�ST�⺯������麯���β�
    �Ƿ���ȷ���������ȷ������ assert_failed() ���������������һ����ѭ���������û������롣

    �ؼ��� __LINE__ ��ʾԴ�����кš�
    �ؼ���__FILE__��ʾԴ�����ļ�����

    ���Թ���ʹ�ܺ���������С���Ƽ��û����ڵ���ʱʹ�ܣ�����ʽ��������ǽ�ֹ��

    �û�����ѡ���Ƿ�ʹ��ST�̼���Ķ��Թ��ܡ�ʹ�ܶ��Եķ��������֣�
    (1) ��C��������Ԥ�����ѡ���ж���USE_FULL_ASSERT��
    (2) �ڱ��ļ�ȡ��"#define USE_FULL_ASSERT    1"�е�ע�͡�

*********************************************************************************************************
*/
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    /*
        �û���������Լ��Ĵ��뱨��Դ�����ļ����ʹ����кţ����罫�����ļ����кŴ�ӡ������
        printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    */
    /* ����һ����ѭ��������ʧ��ʱ������ڴ˴��������Ա����û���� */
    while (1)
    {
    }
}
#endif
//-------------------------------------------------------------------------------

