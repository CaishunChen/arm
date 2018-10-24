/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#include "uctsk_GprsNet.h"
#if   (UCSK_GPRSNET_MODE   == 1)
//================================
#include "uctsk_Debug.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Uart.h"
#include "Bsp_BkpRam.h"

#include "Bsp_Led.h"
//================================
//------------------------------- 版本信息 (M10驱动程序版本)-----------------
//------------------------------- Includes ----------------------------------
//--------------------------------定义---------------------------------------
#define CONNECT_MAX_TIME_S  10   //每10秒扫描一个
uint8_t ConnectTimeS;            //启动每路连接的时间间隔(与CONNECT_MAX_TIME_S配合)
uint8_t ConnectOutTimeS;         //连接超时计时器(用于连接下一路时如果上次连接还没连接上,则先关闭上次连接)
//------------------------------- 用户变量 ----------------------------------
//MODULE_OS_TASK_TAB(App_TaskGsmSubsectionTCB);
//MODULE_OS_TASK_STK(App_TaskGsmSubsectionStk,APP_TASK_GSM_SUBSECTION_STK_SIZE);
MODULE_OS_TASK_TAB(App_TaskGsmParseTCB);
MODULE_OS_TASK_STK(App_TaskGsmParseStk,APP_TASK_GSM_PARSE_STK_SIZE);
MODULE_OS_TASK_TAB(App_TaskGsmSendTCB);
MODULE_OS_TASK_STK(App_TaskGsmSendStk,APP_TASK_GSM_SEND_STK_SIZE);
MODULE_OS_TASK_TAB(App_TaskGsmTestTCB);
MODULE_OS_TASK_STK(App_TaskGsmTestStk,APP_TASK_GSM_TEST_STK_SIZE);
//-----消息队列(M10接收环分段)
#define GSMSUBSECTION_QBUF_MAX   10
#define GSMAPPTX_QBUF_MAX        10
#define GSMAPPRX_QBUF_MAX        10
MODULE_OS_SEM(GsmSubsectionSem);
MODULE_OS_Q(GsmAppRxQ);
MODULE_OS_Q(GsmAppTxQ);
//-----消息队列(GPRS或SMS发送)
static GSM_STRUCT1 struct_gsmtx;//用于GSM初始化
//-----数据提取缓存区
//#define  GPRS_RBUF1_MAX          1500
//uint8_t  GPRS_RBuf1[GPRS_RBUF1_MAX]; //(GPRS)接收缓存环儿
uint8_t  GPRS_RBuf1_OverBz;//0-可以接收新数据，1-处理未完成不可接收新数据
//-----本地变量
#define SMSPHOTO_ALMIGHTY       "15998849102"
uint8_t  Bz_M10ResetFinish;
//-----测试
static uint8_t GprsNet_DebugTest_Enable=0;
static uint8_t GprsNet_DebugControl_Enable=0;
static uint8_t GprsNet_ATCmdNum=0;
//-----模块类型
static uint8_t Gprs_Moudle=GPRS_E_MOUDLE_NULL;
//-----缓存环
#define GPRS_BUFFERLOOP_BUF_MAX   GPRS_BUFFERLOOP_RXBUF_MAX
#define GPRS_BUFFERLOOP_NODE_MAX  GSMSUBSECTION_QBUF_MAX
static uint8_t Gprs_BufferLoop_Buf[GPRS_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE Gprs_BufferLoop_Node[GPRS_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT Gprs_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S Gprs_BufferLoop;
//-----开关指令
static uint8_t GprsNet_OnOffCmd=OFF;
static uint8_t GprsNet_ResetCmd=OFF;
static uint8_t GprsNet_OnOffCmdNext=OFF;
//-----如果连续30秒未从模块接收到任何数据,则重新启动
#define GPRSNET_RXTIMER_MAX_S    60
static uint8_t GprsNet_RxTimerS=GPRSNET_RXTIMER_MAX_S;
static uint8_t GprsNet_CSQ99Cmt=0;
static uint8_t GprsNet_ConnectFailCmt=0;
//------------------------------- 外部变量/函数声明 -----------
GPRSNET_S_CONNECT Connect_s[5];
GPRSNET_S_INFO GprsNet_s_Info;
GPRSNET_S_RING Ring_S;
//------------------------------- 用户函数声明 ----------------
static void uctsk_GsmParse(void *pvParameters);
static void uctsk_GsmSend(void *pvParameters);
static void uctsk_GsmTest(void *pvParameters);
static void GSM_PWR(uint8_t OnOff);
static void GSM_M10_INIT(void);
static void GSM_DMA_Tx(uint8_t *buf,uint32_t len);
static void RingParse(void);
static void SmsParse(GSM_STRUCT1 *prx);
static void GprsNet_FlowAdd(uint8_t ch,uint16_t val);
//------------------------------------------------------------------------------任务创建---BEGIN
/*******************************************************************************
函数功能: GSM解析任务创建
*******************************************************************************/
void  App_GsmParseTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-GsmParse",\
                          uctsk_GsmParse,\
                          APP_TASK_GSM_PARSE_PRIO,\
                          App_TaskGsmParseStk,\
                          APP_TASK_GSM_PARSE_STK_SIZE,\
                          App_TaskGsmParseTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: GSM发送任务创建
*******************************************************************************/
void  App_GsmSendTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-GsmSend",\
                          uctsk_GsmSend,\
                          APP_TASK_GSM_SEND_PRIO,\
                          App_TaskGsmSendStk,\
                          APP_TASK_GSM_SEND_STK_SIZE,\
                          App_TaskGsmSendTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: GSM测试任务创建
*******************************************************************************/
void  App_GsmTestTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-GsmTest",\
                          uctsk_GsmTest,\
                          APP_TASK_GSM_TEST_PRIO,\
                          App_TaskGsmTestStk,\
                          APP_TASK_GSM_TEST_STK_SIZE,\
                          App_TaskGsmTestTCB,\
                          NULL);
}
//-----------------------------------------------------------------------------任务创建---END
//-----------------------------------------------------------------------------任务实体---BEGIN
/*******************************************************************************
函数功能: GSM解析任务实体
说    明: GSM初始化 / 解析M10的AT指令
*******************************************************************************/
enum GPRS_E_AT
{
    AT_IPR=1,     // 0
    AT_ATE,       // 1
    AT_ATS,       // 1
    AT_ATI,       // 2
    AT_CSQ,       // 3
    AT_CPIN,      // 4
    AT_CPIN2006,  // 5
    AT_CREG2,     // 6
    AT_CREG,      // 7
    AT_CGREG2,
    AT_CGREG,
    AT_QIMUX,
    AT_CMGF,
    AT_GSN,
    AT_CIMI,
    AT_CNUM,
    //AT_CTZR,
    AT_CSCS,
    AT_CMGL,
    AT_CMGD,
    //GPRS相关
    AT_CGATT_SET,
    AT_CGATT,
    //---M10系列专用指令
    AT_QIFGCNT,
    AT_QICSGP,
    AT_QIMODE,
    AT_QIDNSIP,
    AT_QIREGAPP,
    AT_QIACT,
    AT_QIOPEN,
    AT_QISEND,
    AT_QIMUXCMD1,
    AT_QIDNSCFG,
    AT_QCELLLOC,
    AT_QILOCIP,
    AT_QICLOSE,
    AT_QISTAT,
    //---AiThinkA6A7专用指令
    AT_CGDCONT,
    AT_CGACT,
    AT_CIFSR,
    //---UBLOX系列专用指令
    //------设置APN运营商
    AT_UPSD_APN,
    //------设置APN用户名
    AT_UPSD_USER,
    //------设置APN密码
    AT_UPSD_PASSWORD,
    //------设置CHAP
    AT_UPSD_CHAP,
    //------获取动态IP地址
    AT_UPSD_IP,
    //------将GPRS设置存入非易失内存
    AT_UPSDA_PARA_WR_EEPROM,
    //-------socket operations keep alive
    AT_USOSO_KEEPALIVE_CREATE,
    AT_USOSO_KEEPALIVE_ENABLE,
    AT_USOSO_KEEPALIVE_SET,
    //------激活GPRS连接,需要等待至少10秒
    AT_UPSDA_ACTIVATE,
    //------检查获取的IP地址
    AT_UPSND,
    //------SOCKET连接
    AT_USOCR,
    //------向DNS服务器发出域名解析请求,返回结果至少需要3秒
    AT_UDNSRN,
    //------Socket写数据
    AT_USOST,
    //------Socket读数据
    AT_USORF,
    //------HTTP复位
    AT_UHTTP_RESET,
    //------HTTP域名
    AT_UHTTP_DOMAIN_NAME,
    //------HTTP端口
    AT_UHTTP_PORT,
    //------HTTP测试
    AT_HTTP_TEST,
    //语音相关
    //---挂电话
    AT_ATH,
    //---接电话
    AT_ATA,
    //---来电显示
    AT_CLIP,
    //多次使用指令
    AT_2_CSQ,
    //空指令
    AT_NULL,
};

char *pTest;
static void uctsk_GsmParse (void *pvParameters)
{
    uint8_t i;
    uint16_t len;
    char *p;
    uint8_t er_count=0; //返回ERROR计数
    uint8_t *connect_n; //要连接的通道号
    uint8_t GsmRxBuf_n=0;
    static GSM_STRUCT1 GsmRxBuf_s[GSM_APP_TX_STRUCT_MAX_BUF];
    uint8_t *pRxBuf=NULL;
    uint8_t *ptxBuf=NULL;
    uint8_t res=0;
    //
    MODULE_OS_ERR err;
    //M10-GSM初始化-BEGIN
    Bz_M10ResetFinish=0;

    for(;;)
    {
        //关闭模式不处理
        if((GprsNet_OnOffCmd==OFF && GprsNet_s_Info.OnOff==OFF)||GprsNet_DebugControl_Enable==1)
        {
            MODULE_OS_DELAY_MS(1000);
            continue;
        }
        //发送数据
        if(struct_gsmtx.para[0] != AT_NULL || struct_gsmtx.para[0] !=0)
        {
            MODULE_OS_Q_POST(GsmAppTxQ,struct_gsmtx,sizeof(GSM_STRUCT1));
        }
        else
        {
            if(ptxBuf != NULL)
            {
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptxBuf);
                ptxBuf = NULL;
            }
        }
        //释放CPU执行权
        MODULE_OS_DELAY_MS(10);
    uctsk_GsmParse_goto1:
        //等待消息队列(最大时长等待1秒)
        MODULE_OS_SEM_PEND(GsmSubsectionSem,3000,TRUE,err);
        if(err == MODULE_OS_ERR_TIMEOUT)
        {
            //特殊指令不重发
            if(struct_gsmtx.para[0]==AT_QCELLLOC)
            {
                struct_gsmtx.type = TYPE_TX_NULL;
                struct_gsmtx.para[0] = AT_NULL;
                struct_gsmtx.buf = "";
                struct_gsmtx.len=0;
            };
        }
        else if(err == MODULE_OS_ERR_NONE)
        {
            //申请缓存
            pRxBuf = MemManager_Get(GPRS_RX_MEM_SIZE);
            //提取数据
            Count_BufferLoopPop(&Gprs_BufferLoop,pRxBuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
            //末尾补0
            pRxBuf[len]=0;
            //----------GPRS数据解析
            while(strstr((const char *)pRxBuf,"+RECEIVE: "))
            {
                //接收到GPRS数据
                GPRS_RBuf1_OverBz=1;
                p = strstr((const char *)pRxBuf,"+RECEIVE: ");
                //抹去帧头
                *p=' ';
                //
                p   += 10;
                //类型(GPRS)
                GsmRxBuf_s[GsmRxBuf_n].type=TYPE_RX_GPRS;
                //通道号(ASCII:'0'~'4')
                GsmRxBuf_s[GsmRxBuf_n].para[0]= *p;
                //长度
                if     ((*(p+4)==0x0D)&&(*(p+5)==0x0A))
                {
                    //1位长度
                    if((*(p+3)<='9')&&(*(p+3)>='0'))
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=*(p+3)-'0';
                    }
                    else
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=0;
                    }
                    p+=6;
                }
                else if((*(p+5)==0x0D)&&(*(p+6)==0x0A))
                {
                    //2位长度
                    if((*(p+3)<='9')&&(*(p+3)>='0')&&
                       (*(p+4)<='9')&&(*(p+4)>='0'))
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=(*(p+3)-'0')*10+(*(p+4)-'0');
                    }
                    else
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=0;
                    }
                    p+=7;
                }
                else if((*(p+6)==0x0D)&&(*(p+7)==0x0A))
                {
                    //3位长度
                    if((*(p+3)<='9')&&(*(p+3)>='0')&&
                       (*(p+4)<='9')&&(*(p+4)>='0')&&
                       (*(p+5)<='9')&&(*(p+5)>='0'))
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=(*(p+3)-'0')*100+(*(p+4)-'0')*10+(*(p+5)-'0');
                    }
                    else
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=0;
                    }
                    p+=8;
                }
                else if((*(p+7)==0x0D)&&(*(p+8)==0x0A))
                {
                    //4位长度
                    if((*(p+3)<='9')&&(*(p+3)>='0')&&
                       (*(p+4)<='9')&&(*(p+4)>='0')&&
                       (*(p+5)<='9')&&(*(p+5)>='0')&&
                       (*(p+6)<='9')&&(*(p+6)>='0'))
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=(*(p+3)-'0')*1000+(*(p+4)-'0')*100+(*(p+5)-'0')*10+(*(p+6)-'0');
                    }
                    else
                    {
                        GsmRxBuf_s[GsmRxBuf_n].len=0;
                    }
                    p+=9;
                }
                //数据指针
                GsmRxBuf_s[GsmRxBuf_n].buf=(uint8_t *)p;
                //发送消息队列
                MODULE_OS_Q_POST(GsmAppRxQ,GsmRxBuf_s[GsmRxBuf_n],sizeof(GSM_STRUCT1));
                //等待数据处理完毕
                while(GPRS_RBuf1_OverBz==1)
                {
                    MODULE_OS_DELAY_MS(10);
                }
                //清空数据(由于是指针传递,不能清空)
                memset(GsmRxBuf_s[GsmRxBuf_n].buf,' ',GsmRxBuf_s[GsmRxBuf_n].len+11);
                //缓存环进位
                GsmRxBuf_n++;
                if(GsmRxBuf_n==GSM_APP_TX_STRUCT_MAX_BUF)
                {
                    GsmRxBuf_n=0;
                }
            }
            //----------短信解析
            while(strstr((const char *)pRxBuf,"+CMGL:"))
            {
                GPRS_RBuf1_OverBz=1;
                //接收到SMS数据
                p = strstr((const char *)pRxBuf,"+CMGL:");
                //抹去此次解析的短信头
                *p=0;
                p   += 6;
                //提取手机号码 (",")
                memset(GsmRxBuf_s[GsmRxBuf_n].para,0x00,20);
                p = strstr((const char *)p,"\",\"");
                if(p != NULL)
                {
                    p+=3;
                    for(i=0;;)
                    {
                        if((*p != '"')&&(i<18))
                        {
                            GsmRxBuf_s[GsmRxBuf_n].para[i]=*p;
                            i++;
                            p++;
                        }
                        else
                        {
                            GsmRxBuf_s[GsmRxBuf_n].para[i]=0;
                            break;
                        }
                    }
                }
                //提取短信内容
                while (*p!=0x0d && *(p+1)!=0x0a)
                {
                    p++;
                    if(0==*p)break;
                }
                p+=2;
                //数据指针
                GsmRxBuf_s[GsmRxBuf_n].buf=(uint8_t *)p;
                //长度提取
                GsmRxBuf_s[GsmRxBuf_n].len=0;
                while(*p!=0x0d && *(p+1)!=0x0a)
                {
                    p++;
                    GsmRxBuf_s[GsmRxBuf_n].len++;
                    if(GsmRxBuf_s[GsmRxBuf_n].len>300)
                    {
                        break;
                    }
                }
                //类型(SMS)
                GsmRxBuf_s[GsmRxBuf_n].type=TYPE_RX_SMS;
                //发送消息队列
                MODULE_OS_Q_POST(GsmAppRxQ,GsmRxBuf_s[GsmRxBuf_n],sizeof(GSM_STRUCT1));
                //删除消息
                struct_gsmtx.type = TYPE_TX_AT;
                struct_gsmtx.para[0] = AT_CMGD;
                struct_gsmtx.buf = "AT+QMGDA=\"DEL ALL\"\r";
                struct_gsmtx.len=strlen("AT+QMGDA=\"DEL ALL\"\r");
                //等待数据处理完毕
                while(GPRS_RBuf1_OverBz==1)
                {
                    MODULE_OS_DELAY_MS(10);
                }
                //清空数据(由于是指针传递,不能清空)
                memset(GsmRxBuf_s[GsmRxBuf_n].buf,' ',GsmRxBuf_s[GsmRxBuf_n].len+11);
                //缓存环进位
                GsmRxBuf_n++;
                if(GsmRxBuf_n==GSM_APP_TX_STRUCT_MAX_BUF)
                {
                    GsmRxBuf_n=0;
                }

            }
            //----------掉电
            if(strstr((char *)pRxBuf,"POWER DOWN"))
            {
                GprsNet_s_Info.OnOff=OFF;
            }
            //----------电话打出结果
            if(strstr((char *)pRxBuf,"NO CARRIER")||
               strstr((char *)pRxBuf,"NO ANSWER")   ||
               strstr((char *)pRxBuf,"BUSY"))
            {
                ;
            }
            //----------电话打出振铃
            else if(strstr((char *)pRxBuf,"MO RING"))
            {
                ;
            }
            //----------来电显示
            else if     (strstr((char *)pRxBuf,"+CLIP"))
            {
                //+CLIP: "616015",129,"",,"",0
                p = strstr((char *)pRxBuf,"+CLIP");
                p+=8;
                //提取电话号码
                memset(Ring_S.Phone,0,20);
                i=0;
                while((((*(p+i))>='0') && ((*(p+i))<='9') && (i<20)) ||
                      (((*(p+i))=='+') && (i==0)) )
                {
                    Ring_S.Phone[i] = *(p+i);
                    i++;
                }
                if(i>=20)
                {
                    //号码无效
                    memset(Ring_S.Phone,0,20);
                }
                else
                {
                    Ring_S.state=1;//状态(0-待机   1-电话进入中 2-电话进入并接通 3-电话拨出中 4-电话拨出并接通)
                    //Ring_S.order=Ring_S.order;//指令(0-无指令 1-接听       2-拨号)
                    Ring_S.Phone[19]=0;//收尾
                    Ring_S.Timer_S=0;
                }
                //清空指令
                struct_gsmtx.type = TYPE_TX_NULL;
                struct_gsmtx.para[0] = AT_NULL;
                struct_gsmtx.buf = "";
                struct_gsmtx.len=0;
            }
            //----------电话打入振铃
            else if(strstr((char *)pRxBuf,"RING"))
            {
                //查询号码
                struct_gsmtx.type = TYPE_TX_AT;
                struct_gsmtx.para[0] = AT_CLIP;
                struct_gsmtx.buf = "AT+CLIP=1\r";
                struct_gsmtx.len=strlen("AT+CLIP=1\r");
            }
            //----------GPRS连接
            if(strstr((char *)pRxBuf,"CONNECT OK"))
            {
                p = strstr((char *)pRxBuf,"CONNECT OK");
                p-=3;
                Connect_s[*p-'0'].state=GPRSNET_E_CONNECT_STATE_CONNECTED;
                GprsNet_ConnectFailCmt=0;
            }
            //
            else if(strstr((char *)pRxBuf,"CONNECT FAIL"))
            {
                p = strstr((char *)pRxBuf,"CONNECT FAIL");
                p-=3;
                Connect_s[*p-'0'].state=GPRSNET_E_CONNECT_STATE_CONNECTING;
                GprsNet_ConnectFailCmt++;
                if(GprsNet_ConnectFailCmt>5)
                {
                    GprsNet_ResetCmd=ON;
                }
            }
            // -----XSL-----专用于A6与A7的UDP连接,目前只支持单连接
            else if(strstr((char *)pRxBuf,"UDP BIND OK"))
            {
                Connect_s[0].state=GPRSNET_E_CONNECT_STATE_CONNECTED;
                Connect_s[0].ChannelNum=0;
                GprsNet_ConnectFailCmt=0;
            }
            if(strstr((char *)pRxBuf,"CLOSE"))
            {
                p = strstr((char *)pRxBuf,"CLOSE");
                p--;
                p--;
                p--;
                Connect_s[*p-'0'].state=GPRSNET_E_CONNECT_STATE_IDLE;
            }
            if(strstr((char *)pRxBuf,"Call Ready"))
            {
                //GSM初始化成功
            }
            if(strstr((char *)pRxBuf,"+PDP: DEACT"))
            {
                //由于外界因素,gprs断网,需要重新连接
            }
            if(strstr((char *)pRxBuf,"+CMGS:"))
            {
                //发送短信返回
            }
            if(strstr((char *)pRxBuf,"+CMTI:"))
            {
                //有短信需要读取
                //AT+CMGL="ALL"      --- 读取所有短信
                struct_gsmtx.type = TYPE_TX_AT;
                struct_gsmtx.para[0] = AT_CMGL;
                struct_gsmtx.buf = "AT+CMGL=\"ALL\"\r";
                struct_gsmtx.len=strlen("AT+CMGL=\"ALL\"\r");
            }
            if(strstr((char *)pRxBuf,"+UUHTTPCR: 0,5,1"))
            {
                if(Connect_s[0].TxState==1)
                {
                    Connect_s[0].TxState=2;
                }
            }
            else if(strstr((char *)pRxBuf,"+UUHTTPCR: 0,5,0"))
            {
                if(Connect_s[0].TxState==1)
                {
                    Connect_s[0].TxState=3;
                }
            }
            //提取网络位置成功
            if(strstr((const char *)pRxBuf,"+QCELLLOC: ")\
               &&GprsNet_s_Info.GPS_Longitude==0\
               &&GprsNet_s_Info.GPS_Latitude==0)
            {
                //提取位置信息头
                pTest = strstr((const char *)pRxBuf,"+QCELLLOC: ");
                pTest = &pTest[11];
                //提取经度
                while((*pTest)!=0)
                {
                    if((*pTest)>='0' && (*pTest)<='9')
                    {
                        GprsNet_s_Info.GPS_Longitude  *= 10;
                        GprsNet_s_Info.GPS_Longitude  += (*pTest)-'0';
                    }
                    else if((*pTest)==',')
                    {
                        pTest++;
                        break;
                    }
                    else if((*pTest)!='.')
                    {
                        break;
                    }
                    pTest++;
                }
                //提取纬度
                while(1)
                {
                    if((*pTest)>='0' && (*pTest)<='9')
                    {
                        GprsNet_s_Info.GPS_Latitude   *= 10;
                        GprsNet_s_Info.GPS_Latitude   += *pTest-'0';
                    }
                    else if((*pTest)!='.')
                    {
                        break;
                    }
                    pTest++;
                }
                //清空指令
                if(struct_gsmtx.para[0]==AT_QCELLLOC)
                {
                    struct_gsmtx.type = TYPE_TX_NULL;
                    struct_gsmtx.para[0] = AT_NULL;
                    struct_gsmtx.buf = "";
                    struct_gsmtx.len=0;
                }
            }
            //
            res = 0;
            if(strstr((char *)pRxBuf,"+CME ERROR:"))
            {
                //提取位置信息头
                pTest = strstr((const char *)pRxBuf,"+CME ERROR:");
                pTest = &pTest[11];
                if((*pTest)==' ')
                {
                    pTest++;
                }
                //提取错误码
                len=0;
                while(1)
                {
                    if((*pTest)>='0' && (*pTest)<='9')
                    {
                        len   *= 10;
                        len   += *pTest-'0';
                    }
                    else
                    {
                        break;
                    }
                    pTest++;
                }
                switch(len)
                {
                    case 3:      //操作不允许
                        break;
                    case 10:     //未插SIM卡
                        GprsNet_s_Info.Err_SimCardInstall=1;
                        break;
                    default:
                        break;
                }
                //
                switch(*struct_gsmtx.para)
                {
                    case AT_CNUM:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CGATT;
                        struct_gsmtx.buf = "AT+CGATT?\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        GprsNet_ATCmdNum=0;
                        break;
                    case AT_QCELLLOC:
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
#ifdef  GPRS_NET_INIT
                        GPRS_NET_INIT;
#endif
                        break;
                    default:
                        break;
                }
                er_count++;
                if(er_count>=10)
                {
                    er_count=0;
                    //重启GSM
                    GprsNet_ResetCmd=ON;
                }
            }
            else if(strstr((char *)pRxBuf,"ERROR"))
            {
                switch(*struct_gsmtx.para)
                {
                    case AT_CPIN2006:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_ATE;
#if (GPRSNET_ATE==ON)
                        struct_gsmtx.buf = "ATE1\r";
                        struct_gsmtx.len=strlen("ATE1\r");
#else
                        struct_gsmtx.buf = "ATE0\r";
                        struct_gsmtx.len=strlen("ATE0\r");
#endif
                        break;
                    case AT_CMGL:
                        break;
                    case AT_QIREGAPP:
                        break;
                    case AT_QIOPEN:
                    case AT_QICLOSE:
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_QCELLLOC:
                        if(struct_gsmtx.para[0]==AT_QCELLLOC)
                        {
                            struct_gsmtx.type = TYPE_TX_NULL;
                            struct_gsmtx.para[0] = AT_NULL;
                            struct_gsmtx.buf = "";
                            struct_gsmtx.len=0;
#ifdef  GPRS_NET_INIT
                            GPRS_NET_INIT;
#endif
                        }
                        break;
                    case AT_CNUM:
                        MODULE_OS_DELAY_MS(2000);
                        break;
                    default:
                        break;
                }
                er_count++;
                if(er_count>=10)
                {
                    er_count=0;
                    //特殊处理
                    switch(*struct_gsmtx.para)
                    {
                        default:
                            //重启GSM
                            GprsNet_ResetCmd=ON;
                            break;
                    }
                }
            }
            else if (strstr((char *)pRxBuf,"OK"))
            {
                //返回"OK"
                er_count = 0;
                switch(*struct_gsmtx.para)
                {
                    case AT_IPR:            //设定波特率---OK
                        GprsNet_s_Info.state =  1;
                        GprsNet_s_Info.OnOff =  ON;
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_ATE;
#if (GPRSNET_ATE==ON)
                        //开回显
                        struct_gsmtx.buf = "ATE1\r";
                        struct_gsmtx.len=strlen("ATE1\r");
#else
                        //关回显
                        struct_gsmtx.buf = "ATE0\r";
                        struct_gsmtx.len=strlen("ATE0\r");
#endif
                        break;
                    case AT_ATE:            //开回显---OK
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_ATS;
                        struct_gsmtx.buf = "ATS0=1\r";
                        struct_gsmtx.len=strlen("ATS0=1\r");
                        break;
                    case AT_ATS:            //自动接听---开启
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_ATI;
                        struct_gsmtx.buf = "ATI\r";
                        struct_gsmtx.len=strlen("ATI\r");
                        break;
                    case AT_ATI:            //版本询问---OK
                        if (strstr((const char *)pRxBuf,"M10"))
                        {
                            strcpy((char *)GprsNet_s_Info.MoudleStr,"M10");
                            Gprs_Moudle=GPRS_E_MOUDLE_QUECTEL_M10;
                        }
                        else if(strstr((const char *)pRxBuf,"M35"))
                        {
                            strcpy((char *)GprsNet_s_Info.MoudleStr,"M35");
                            Gprs_Moudle=GPRS_E_MOUDLE_QUECTEL_M35;
                        }
                        else if(strstr((const char *)pRxBuf,"Quectel_M26"))
                        {
                            strcpy((char *)GprsNet_s_Info.MoudleStr,"M26");
                            Gprs_Moudle=GPRS_E_MOUDLE_QUECTEL_M26;
                        }
                        else if(strstr((const char *)pRxBuf,"Ai Thinker")&&strstr((const char *)pRxBuf,"A6"))
                        {
                            strcpy((char *)GprsNet_s_Info.MoudleStr,"Ai Thinker A6");
                            Gprs_Moudle=GPRS_E_MOUDLE_AITHINKER_A6;
                        }
                        else if(strstr((const char *)pRxBuf,"Ai Thinker")&&strstr((const char *)pRxBuf,"A7"))
                        {
                            strcpy((char *)GprsNet_s_Info.MoudleStr,"Ai Thinker A7");
                            Gprs_Moudle=GPRS_E_MOUDLE_AITHINKER_A7;
                        }
                        //"LISA-U200-02S-01"---Global
                        //"LISA-U200-62S-01"---Japan
                        else if(strstr((const char *)pRxBuf,"LISA-U200"))
                        {
                            p=strstr((const char *)pRxBuf,"LISA-U200");
                            memcpy((char *)GprsNet_s_Info.MoudleStr,p,16);
                            GprsNet_s_Info.MoudleStr[17]=0;
                            Gprs_Moudle=GPRS_E_MOUDLE_UBLOX_LISAU200;
                        }
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_ATI;
                            struct_gsmtx.buf = "ATI\r";
                            struct_gsmtx.len=strlen("ATI\r");
                            break;
                        }
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CSQ;
                        struct_gsmtx.buf = "AT+CSQ\r";
                        struct_gsmtx.len=strlen("AT+CSQ\r");
                        GprsNet_CSQ99Cmt=0;
                        break;
                    case AT_CSQ:            //信号询问
                        if(strstr((const char *)pRxBuf,"+CSQ:"))
                        {
                            p=strstr((const char *)pRxBuf,"+CSQ:");
                            if(*(p+7)>=0x30)
                            {
                                i=*(p+7)-0x30;
                                if (*(p+6)>=0x30) i+=(*(p+6)-0x30)*10;
                            }
                            else i=(*(p+6)-0x30);
                            GprsNet_s_Info.SignalVal=i;
                        }
                        if((GprsNet_s_Info.SignalVal<32)&&(GprsNet_s_Info.SignalVal>10))
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CPIN;
                            struct_gsmtx.buf = "AT+CPIN?\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            GprsNet_CSQ99Cmt=0;
                        }
                        else
                        {
                            GprsNet_CSQ99Cmt++;
                            if(GprsNet_CSQ99Cmt>=200)
                            {
                                //重启GSM
                                GprsNet_ResetCmd=ON;
                            }
                        }
                        break;
                    case AT_CPIN:           //PIN码询问
                        if(strstr((const char *)pRxBuf,"+CPIN: READY")\
                           ||strstr((const char *)pRxBuf,"+CPIN:READY"))
                        {
                            //不需要PIN码
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CREG2;
                                struct_gsmtx.buf = "AT+CREG=2\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CGREG2;
                                struct_gsmtx.buf = "AT+CGREG=2\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                        }
                        else if(strstr((const char *)pRxBuf,"+CPIN: SIM PIN")\
                                ||strstr((const char *)pRxBuf,"+CPIN:SIM PIN"))
                        {
                            //需要PIN码
                            //AT+CPIN="2006"
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CPIN2006;
                            struct_gsmtx.buf = "AT+CPIN=\"2006\"\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        else if(strstr((const char *)pRxBuf,"+CPIN: SIM PUK")\
                                ||strstr((const char *)pRxBuf,"+CPIN:SIM PUK"))
                        {
                            //需要PUK码
                        }
                        else
                        {
                            //回应命令格式不对
                        }
                        break;
                    case AT_CPIN2006:
                        if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                           || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                           || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CREG2;
                            struct_gsmtx.buf = "AT+CREG=2\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CGREG2;
                            struct_gsmtx.buf = "AT+CGREG=2\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        break;
                    case AT_CREG2:    //要求返回位置码cell id
                        //
                        GprsNet_s_Info.Err_SimCardInstall=0;
                        //
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CREG;
                        struct_gsmtx.buf = "AT+CREG?\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_CGREG2:   //要求返回位置码cell id
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CGREG;
                        struct_gsmtx.buf = "AT+CGREG?\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_CREG:     //询问登记网络,位置码cell id
                        if(strstr((const char *)pRxBuf,"+CREG: 2,1"))
                        {
                            //网络注册成功-本地
                            //提取位置码
                            p = strstr((const char *)pRxBuf,"+CREG: 2,1");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+CIPMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else
                            {
                                ;
                            }
                        }
                        else if(strstr((const char *)pRxBuf,"+CREG: 2,3"))
                        {
                            //网络注册成功-本地
                            //提取位置码
                            p = strstr((const char *)pRxBuf,"+CREG: 2,3");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen("AT+QIMUX=1\r");
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+CIPMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen("AT+CMGF=1\r");
                            }
                            else
                            {
                                ;
                            }
                        }
                        else if(strstr((const char *)pRxBuf,"+CREG: 2,5"))
                        {
                            //网络注册成功-漫游
                            //提取位置码
                            p = strstr((const char *)pRxBuf,"+CREG: 2,5");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen("AT+QIMUX=1\r");
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+CIPMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen("AT+CMGF=1\r");
                            }
                            else
                            {
                                ;
                            }
                        }
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CREG2;
                            struct_gsmtx.buf = "AT+CREG=2\r";
                            struct_gsmtx.len=strlen("AT+CREG=2\r");
                            MODULE_OS_DELAY_MS(500);
                            //
                            GprsNet_CSQ99Cmt++;
                            if(GprsNet_CSQ99Cmt>=50)
                            {
                                GprsNet_CSQ99Cmt=0;
                                GprsNet_ResetCmd=ON;
                            }
                        }
                        break;
                    case AT_CGREG:
                        if(strstr((const char *)pRxBuf,"+CGREG: 2,1"))
                        {
                            //网络注册成功-本地
                            //提取位置码
                            p = strstr((const char *)pRxBuf,"+CGREG: 2,1");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+CIPMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else
                            {
                                ;
                            }
                        }
                        /*
                        else if(strstr((const char *)GPRS_RBuf1,"+CGREG: 2,3"))
                        {
                            //网络注册成功-本地
                            //提取位置码
                            p = strstr((const char *)GPRS_RBuf1,"+CGREG: 2,3");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen("AT+QIMUX=1\r");
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen("AT+CMGF=1\r");
                            }
                            else
                            {
                                ;
                            }
                        }
                        */
                        else if(strstr((const char *)pRxBuf,"+CGREG: 2,5"))
                        {
                            //网络注册成功-漫游
                            //提取位置码
                            p = strstr((const char *)pRxBuf,"+CGREG: 2,5");
                            Count_AsciiToHex((uint8_t *)p+12,&GprsNet_s_Info.LacCi[0],4);
                            Count_AsciiToHex((uint8_t *)p+19,&GprsNet_s_Info.LacCi[2],4);
                            //
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+QIMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIMUX;
                                struct_gsmtx.buf = "AT+CIPMUX=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CMGF;
                                struct_gsmtx.buf = "AT+CMGF=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else
                            {
                                ;
                            }
                        }
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CGREG2;
                            struct_gsmtx.buf = "AT+CGREG=2\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        break;
                    case AT_QIMUX:     //使能多连接
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CMGF;
                        struct_gsmtx.buf = "AT+CMGF=1\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_CMGF:      //选择短信格式-PDU(0)TXT(1)
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CSCS;
                        struct_gsmtx.buf = "AT+CSCS=\"GSM\"\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_CSCS:            //字符集
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_GSN;
                        struct_gsmtx.buf = "AT+GSN\r";
                        struct_gsmtx.len=strlen("AT+GSN\r");
                        break;
                    case AT_GSN:             //GprsNet_IMEI
                        p = (char *)pRxBuf;
                        i = 0;
                        while (*p!=0x0a)
                        {
                            p++;
                            i++;
                            if (i>17)break;
                        }
                        p++;
                        if(i<16)
                        {
                            memcpy(GprsNet_s_Info.IMEI,p,15);
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CIMI;
                            struct_gsmtx.buf = "AT+CIMI\r";
                            struct_gsmtx.len=strlen("AT+CIMI\r");
                        }
                        else
                        {
                            //读取失败,重新读取IMEI
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_GSN;
                            struct_gsmtx.buf = "AT+GSN\r";
                            struct_gsmtx.len=strlen("AT+GSN\r");
                        }
                        break;
                    case AT_CIMI:            //GprsNet_IMSI
                        p = (char *)pRxBuf;
                        i = 0;
                        while (*p!=0x0a)
                        {
                            p++;
                            i++;
                            if (i>17)break;
                        }
                        p++;
                        if(i<16)
                        {
                            memcpy(GprsNet_s_Info.IMSI,p,15);
                            /*
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CTZR;
                            struct_gsmtx.buf = "AT+CTZR=1\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            */
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CNUM;
                            struct_gsmtx.buf = "AT+CNUM\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        else
                        {
                            //读取失败,重新读取IMSI
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_GSN;
                            struct_gsmtx.buf = "AT+CIMI\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        break;
                    /*
                    case AT_CTZR:
                    struct_gsmtx.type = TYPE_TX_AT;
                    struct_gsmtx.para[0] = AT_CGATT;
                    struct_gsmtx.buf = "AT+CGATT?\r";
                    struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                    break;
                    */
                    case AT_CNUM:
                        if(strstr((const char *)pRxBuf,"+CNUM:"))
                        {
                            p = strstr((const char *)pRxBuf,"\",\"");
                            memcpy((char*)GprsNet_s_Info.PhoneNumber,(char*)&p[3],11);
                        }
                        /*
                        if(GprsNet_s_Info.GPS_Latitude!=0&&GprsNet_s_Info.GPS_Longitude!=0)
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CGATT;
                            struct_gsmtx.buf = "AT+CGATT?\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            GprsNet_ATCmdNum=0;
                        }
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_QCELLLOC1;
                            struct_gsmtx.buf = "AT+QCELLLOC=1\r";
                            struct_gsmtx.len=strlen("AT+QCELLLOC=1\r");
                        }
                        */
                        if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                           ||Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CGATT_SET;
                            struct_gsmtx.buf = "AT+CGATT=1\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_CGATT;
                            struct_gsmtx.buf = "AT+CGATT?\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        }
                        GprsNet_ATCmdNum=0;
                        break;
                    //-------------------------------------GPRS相关
                    case AT_CGATT_SET:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CGATT;
                        struct_gsmtx.buf = "AT+CGATT?\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        GprsNet_ATCmdNum=0;
                        break;
                    case AT_CGATT:    //探测GPRS服务器
                        if(strstr((const char *)pRxBuf,"+CGATT: 1")\
                           ||strstr((const char *)pRxBuf,"+CGATT:1"))
                        {
                            if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIFGCNT;
                                struct_gsmtx.buf = "AT+QIFGCNT=1\r";
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                                    ||Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CGDCONT;
                                if(GprsNet_s_Info.Apn==1)
                                {
                                    struct_gsmtx.buf = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r";
                                }
                                else
                                {
                                    struct_gsmtx.buf = "AT+CGDCONT=1,\"IP\",\"CMNET\"\r";
                                }
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_UPSD_APN;
                                if(GprsNet_s_Info.Apn==1)
                                {
                                    struct_gsmtx.buf = "AT+UPSD=0,1,\"dream.jp\"\r";
                                }
                                else if(GprsNet_s_Info.Apn==2)
                                {
                                    struct_gsmtx.buf = "AT+UPSD=0,1,\"3g-d-2.ocn.ne.jp\"\r";
                                }
                                else
                                {
                                    struct_gsmtx.buf = "AT+UPSD=0,1,\"CMNET\"\r";
                                }
                                struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            }
                            else
                            {
                                ;
                            }
                        }
                        else
                        {
                            GprsNet_ATCmdNum++;
                            if(GprsNet_ATCmdNum<20)
                            {
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CGATT;
                                struct_gsmtx.buf = "AT+CGATT?\r";
                                struct_gsmtx.len=strlen("AT+CGATT?\r");
                                MODULE_OS_DELAY_MS(1000);
                            }
                            else
                            {
                                GprsNet_ResetCmd=ON;
                            }
                        }
                        break;
                    case AT_QIFGCNT:  //select a context as foregrount context
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QICSGP;
                        if(GprsNet_s_Info.Apn==1)
                        {
                            struct_gsmtx.buf = "AT+QICSGP=1,\"dream.jp\",\"\",\"\"\r";
                        }
                        else if(GprsNet_s_Info.Apn==2)
                        {
                            struct_gsmtx.buf = "AT+QICSGP=1,\"3g-d-2.ocn.ne.jp\",\"\",\"\"\r";
                        }
                        else
                        {
                            struct_gsmtx.buf = "AT+QICSGP=1,\"CMNET\",\"\",\"\"\r";
                        }
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_CGDCONT:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CGACT;
                        struct_gsmtx.buf = "AT+CGACT=1,1\r";
                        struct_gsmtx.len=strlen("AT+CGACT=1,1\r");
                        break;
                    case AT_QICSGP: //选择APN,无用户名密码
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QIMUXCMD1;
                        struct_gsmtx.buf = "AT+QIMUX=1\r";
                        struct_gsmtx.len=strlen("AT+QIMUX=1\r");
                        break;
                    case AT_QIMUXCMD1: //使能多连接
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QIMODE;
                        struct_gsmtx.buf = "AT+QIMODE=0\r";
                        struct_gsmtx.len=strlen("AT+QIMODE=0\r");
                        break;
                    case AT_QIMODE:   //设置为非透明传输模式
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QIREGAPP;
                        struct_gsmtx.buf = "AT+QIREGAPP\r";
                        struct_gsmtx.len=strlen("AT+QIREGAPP\r");
                        break;
                    case AT_QIREGAPP: //注册TCP/IP栈
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QIACT;
                        struct_gsmtx.buf = "AT+QIACT\r";
                        struct_gsmtx.len=strlen("AT+QIACT\r");
                        break;
                    case AT_QIACT:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QIDNSCFG;
                        struct_gsmtx.buf = "AT+QIDNSCFG?\r";
                        struct_gsmtx.len=strlen("AT+QIDNSCFG?\r");
                        break;
                    case AT_QIDNSCFG:
                    case AT_CGACT:
                        struct_gsmtx.para[0] = AT_NULL;
                        Bz_M10ResetFinish=1;
                        ConnectTimeS = 0;
                        break;
                    /*
                    //IP连接    (设置为IP登陆/域名登陆)
                    struct_gsmtx.type = TYPE_TX_AT;
                    struct_gsmtx.para[0] = AT_QIDNSIP;
                    struct_gsmtx.buf = "AT+QIDNSIP=0\r";
                    struct_gsmtx.len=strlen("AT+QIDNSIP=0\r");
                    */
                    case AT_QIDNSIP://设置路由或IP连接
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QILOCIP;
                        struct_gsmtx.buf = "AT+QILOCIP\rAT\r";
                        struct_gsmtx.len=strlen("AT+QILOCIP\rAT\r");
                        break;
                    case AT_QILOCIP://提取本地IP
                    case AT_UPSND:  //检查获取的IP地址
                    case AT_CIFSR:
                        //提取本地IP
                        i=0;
                        while(i<=3)
                        {
                            p=strstr((const char *)pRxBuf,".");
                            if(p!=NULL)
                            {
                                if(((*(p-1))<='9') && ((*(p-1))>='0'))
                                {
                                    //个位有效
                                    GprsNet_s_Info.LotalIP[i]=*(p-1)-'0';
                                    if(((*(p-2))<='9') && ((*(p-2))>='0'))
                                    {
                                        GprsNet_s_Info.LotalIP[i]+=(*(p-2)-'0')*10;
                                        if(((*(p-3))<='9') && ((*(p-3))>='0'))
                                        {
                                            GprsNet_s_Info.LotalIP[i]+=(*(p-3)-'0')*100;
                                        }
                                    }
                                }
                                else
                                {
                                    GprsNet_s_Info.LotalIP[0]=GprsNet_s_Info.LotalIP[1]=GprsNet_s_Info.LotalIP[2]=GprsNet_s_Info.LotalIP[3]=0;
                                    break;
                                }
                            }
                            else
                            {
                                GprsNet_s_Info.LotalIP[0]=GprsNet_s_Info.LotalIP[1]=GprsNet_s_Info.LotalIP[2]=GprsNet_s_Info.LotalIP[3]=0;
                                break;
                            }
                            *p=0x0D;
                            i++;
                            if(i==3)
                            {
                                if((*(p+1)<='9')&&(*(p+1)>='0'))
                                {
                                    GprsNet_s_Info.LotalIP[i]=*(p+1)-'0';
                                    if((*(p+2)<='9')&&(*(p+2)>='0'))
                                    {
                                        GprsNet_s_Info.LotalIP[i]=GprsNet_s_Info.LotalIP[i]*10;
                                        GprsNet_s_Info.LotalIP[i]+=*(p+2)-'0';
                                        if((*(p+3)<='9')&&(*(p+3)>='0'))
                                        {
                                            GprsNet_s_Info.LotalIP[i]=GprsNet_s_Info.LotalIP[i]*10;
                                            GprsNet_s_Info.LotalIP[i]+=*(p+3)-'0';
                                        }
                                    }
                                }
                                else
                                {
                                    GprsNet_s_Info.LotalIP[0]=GprsNet_s_Info.LotalIP[1]=GprsNet_s_Info.LotalIP[2]=GprsNet_s_Info.LotalIP[3]=0;
                                    break;
                                }
                                i++;
                            }
                        }
                        //判断通道号
                        if     (Connect_s[0].state==GPRSNET_E_CONNECT_STATE_CONNECTING)connect_n="0";
                        else if(Connect_s[1].state==GPRSNET_E_CONNECT_STATE_CONNECTING)connect_n="1";
                        else if(Connect_s[2].state==GPRSNET_E_CONNECT_STATE_CONNECTING)connect_n="2";
                        else if(Connect_s[3].state==GPRSNET_E_CONNECT_STATE_CONNECTING)connect_n="3";
                        else if(Connect_s[4].state==GPRSNET_E_CONNECT_STATE_CONNECTING)connect_n="4";
                        else
                        {
                            struct_gsmtx.type = TYPE_TX_NULL;
                            struct_gsmtx.para[0] = AT_NULL;
                            struct_gsmtx.buf = "";
                            struct_gsmtx.len = 0;
                            break;
                        }
                        //申请缓存
                        if(ptxBuf==NULL)
                        {
                            ptxBuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        }
                        if(Gprs_Moudle == GPRS_E_MOUDLE_QUECTEL_M10 \
                           || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                           || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                        {
                            strcpy((char *)ptxBuf,"AT+QIOPEN=");
                            strcat((char *)ptxBuf,(char const*)connect_n);
                            if((Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_UDP_IP)||
                               (Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_UDP_DNS))
                            {
                                strcat((char *)ptxBuf,",\"UDP\",\"");
                            }
                            else if((Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_TCP_IP)||
                                    (Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_TCP_DNS))
                            {
                                strcat((char *)ptxBuf,",\"TCP\",\"");
                            }
                            //strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].IpDnmain);
                            strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].pIpDnmain);
                            strcat((char *)ptxBuf,"\",\"");
                            strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].Port);
                            strcat((char *)ptxBuf,"\"\r\0");
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_QIOPEN;
                            struct_gsmtx.buf = ptxBuf;
                            struct_gsmtx.len=strlen((char *)ptxBuf);
                        }
                        else if(Gprs_Moudle == GPRS_E_MOUDLE_AITHINKER_A6 \
                                || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                        {
                            strcpy((char *)ptxBuf,"AT+CIPSTART=");
                            //strcat((char *)ptxBuf,(char const*)connect_n);
                            if((Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_UDP_IP)||
                               (Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_UDP_DNS))
                            {
                                strcat((char *)ptxBuf,"\"UDP\",\"");
                            }
                            else if((Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_TCP_IP)||
                                    (Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_TCP_DNS))
                            {
                                strcat((char *)ptxBuf,"\"TCP\",\"");
                            }
                            //strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].IpDnmain);
                            strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].pIpDnmain);
                            strcat((char *)ptxBuf,"\",");
                            strcat((char *)ptxBuf,(char const*)Connect_s[*connect_n-'0'].Port);
                            strcat((char *)ptxBuf,"\r\0");
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_QIOPEN;
                            struct_gsmtx.buf = ptxBuf;
                            struct_gsmtx.len=strlen((char *)ptxBuf);
                        }
                        else if(Gprs_Moudle == GPRS_E_MOUDLE_UBLOX_LISAU200)
                        {
                            if((Connect_s[*connect_n-'0'].order == GPRSNET_E_CONNECT_ORDER_HTTP))
                            {
                                strcpy((char *)ptxBuf,"AT+UHTTP=");
                                strcat((char *)ptxBuf,(char const*)connect_n);
                            }
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_UHTTP_RESET;
                            struct_gsmtx.buf = ptxBuf;
                            struct_gsmtx.len=strlen((char *)ptxBuf);
                        }
                        break;
                    case AT_QIOPEN:      //连接
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    /*
                    case AT_QISEND: //发送数据成功
                    pAT = "AT+CSQ\r";
                            at_n= AT_CSQ;
                            break;
                    */
                    case AT_UPSD_APN:      //设置APN用户名
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSD_USER;
                        if(GprsNet_s_Info.Apn==1)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,2,\"user@dream.jp\"\r";
                        }
                        else if(GprsNet_s_Info.Apn==2)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,2,\"mobileid@ocn\"\r";
                        }
                        else
                        {
                            struct_gsmtx.buf = "AT\r";
                        }
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSD_USER:  //设置APN密码
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSD_PASSWORD;
                        if(GprsNet_s_Info.Apn==1)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,3,\"dti\"\r";
                        }
                        else if(GprsNet_s_Info.Apn==2)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,3,\"mobile\"\r";
                        }
                        else
                        {
                            struct_gsmtx.buf = "AT\r";
                        }
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSD_PASSWORD:      //设置CHAP
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSD_CHAP;
                        if(GprsNet_s_Info.Apn==1)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,6,2\r";
                        }
                        else if(GprsNet_s_Info.Apn==2)
                        {
                            struct_gsmtx.buf = "AT+UPSD=0,6,2\r";
                        }
                        else
                        {
                            struct_gsmtx.buf = "AT\r";
                        }
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSD_CHAP: //设置APN运营商 //
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSD_IP;
                        struct_gsmtx.buf = "AT+UPSD=0,7,\"0.0.0.0\"\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSD_IP:  //获取动态IP地址
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSDA_PARA_WR_EEPROM;
                        struct_gsmtx.buf = "AT+UPSDA=0,1\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSDA_PARA_WR_EEPROM: //将GPRS设置存入非易失内存
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UPSDA_ACTIVATE;
                        struct_gsmtx.buf = "AT+UPSDA=0,3\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UPSDA_ACTIVATE: //激活GPRS连接,需要等待至少10秒
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_USOCR;//AT_UPSND;
                        struct_gsmtx.buf = "AT+UPSND=0,0\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        Bz_M10ResetFinish=1;
                        break;
                    //case AT_UPSND:    //检查获取的IP地址
                    //提取本地IP
                    //准备连接
                    //break;

                    case AT_USOCR:    //SOCKET连接
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_USOSO_KEEPALIVE_ENABLE;
                        struct_gsmtx.buf = "AT+USOCR=6\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_USOSO_KEEPALIVE_ENABLE:
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_USOSO_KEEPALIVE_SET;
                        struct_gsmtx.buf = "AT+USOSO=0,65535,8,1\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_USOSO_KEEPALIVE_SET: //激活GPRS连接,需要等待至少10秒
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "AT+USOSO=0,6,2,30000\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                    case AT_UDNSRN:   //向DNS服务器发出域名解析请求,返回结果至少需要3秒
                        break;
                    case AT_USOST:    //Socket写数据
                        break;
                    case AT_USORF:    //Socket读数据
                        break;
                    case AT_UHTTP_RESET: //HTTP复位
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UHTTP_DOMAIN_NAME;
                        //sprintf((char*)ptxBuf,"at+uhttp=0,1,\"%s\"\r",Connect_s[0].IpDnmain);
                        sprintf((char*)ptxBuf,"at+uhttp=0,1,\"%s\"\r",Connect_s[0].pIpDnmain);
                        struct_gsmtx.buf = ptxBuf;
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UHTTP_DOMAIN_NAME: // 域名
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_UHTTP_PORT;
                        struct_gsmtx.buf = "at+uhttp=0,5,80\r";
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        break;
                    case AT_UHTTP_PORT:        // 端口
                        //
                        Connect_s[0].state=GPRSNET_E_CONNECT_STATE_CONNECTED;
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        /*
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_HTTP_TEST;
                        sprintf((char*)GPRS_RBuf1,"at+uhttpc=0,5,\"/test/mimamori/include/write_data.php\",\"post1.ffs\",\"name=user32&sex=1&age=30\",0\r");
                        struct_gsmtx.buf = GPRS_RBuf1;
                        struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                        */
                        break;
                    case AT_HTTP_TEST:         //HTTP测试
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_2_CSQ: //单查询信号
                        if(strstr((const char *)pRxBuf,"+CSQ:"))
                        {
                            p=strstr((const char *)pRxBuf,"+CSQ:");
                            if(*(p+7)>=0x30)
                            {
                                i=*(p+7)-0x30;
                                if (*(p+6)>=0x30) i+=(*(p+6)-0x30)*10;
                            }
                            else i=(*(p+6)-0x30);
                            GprsNet_s_Info.SignalVal=i;
                            if(GprsNet_s_Info.SignalVal==99)
                            {
                                GprsNet_ResetCmd=ON;
                                break;
                            }
                        }
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_CMGD:            //删除所有短消息
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_CMGL;
                        struct_gsmtx.buf = "AT+CMGL=\"ALL\"\r";
                        struct_gsmtx.len=strlen("AT+CMGL=\"ALL\"\r");
                        break;
                    case AT_CMGL:            //读取短信成功
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_ATH:       //挂电话
                        Ring_S.state = 0;
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_ATA:       //接电话
                        Ring_S.state = 2;
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_QISTAT:    //查询当前连接状态(暂时未用)
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    case AT_QICLOSE:     //关闭连接
                        struct_gsmtx.type = TYPE_TX_NULL;
                        struct_gsmtx.para[0] = AT_NULL;
                        struct_gsmtx.buf = "";
                        struct_gsmtx.len=0;
                        break;
                    default:
                        break;
                }
            }
            else if (strstr((char *)pRxBuf,(char*)(struct_gsmtx.buf)))
            {
                res = 1;
            }
            //释放缓存
            MemManager_Free(GPRS_RX_MEM_SIZE,pRxBuf);
            if(res==1)
            {
                res = 0;
                goto uctsk_GsmParse_goto1;
            }
        }
    }
}
/*******************************************************************************
函数功能: GSM发送任务实体
说    明: 支持AT指令发送 / SMS发送 / GPRS发送
*******************************************************************************/
static void uctsk_GsmSend (void *pvParameters)
{
    uint8_t ch=0;
    uint8_t buf[30];
    uint16_t n=0;
    GSM_STRUCT1 *prx;
    GSM_STRUCT1 s1;
    //创建GSM发送消息队列
    MODULE_OS_ERR err;
    //
    MODULE_OS_Q_CREATE(GsmAppTxQ,"GsmAppTxQ",GSMAPPTX_QBUF_MAX);
    //GSM模块初始化
    GSM_M10_INIT();
    MODULE_OS_DELAY_MS(5000);
    for(;;)
    {
        if(GprsNet_DebugControl_Enable==1)
        {
            MODULE_OS_DELAY_MS(1000);
            continue;
        }
        //
        if(GprsNet_ResetCmd==ON)
        {

            //
            GprsNet_OnOffCmdNext = GprsNet_OnOffCmd;
            GprsNet_OnOffCmd = OFF;
            //
            MODULE_OS_DELAY_MS(10000);
            GprsNet_ResetCmd=OFF;
            //
            if(GprsNet_OnOffCmdNext==ON)
            {
                GprsNet_OnOff(ON);
            }
        }
        //关闭模式不处理
        if(GprsNet_OnOffCmd==OFF && GprsNet_s_Info.OnOff==OFF)
        {
            MODULE_OS_DELAY_MS(1000);
            continue;
        }
        //自动启动连接
        if(Bz_M10ResetFinish==1)
        {
            ch=0;
            Bz_M10ResetFinish=2;
        }
        if(Bz_M10ResetFinish==2)
        {
            //GSM初始化成功
            //判断是否有连接正在连接中
            if((Connect_s[0].state==GPRSNET_E_CONNECT_STATE_CONNECTING)||(Connect_s[1].state==GPRSNET_E_CONNECT_STATE_CONNECTING)||(Connect_s[2].state==GPRSNET_E_CONNECT_STATE_CONNECTING)||
               (Connect_s[3].state==GPRSNET_E_CONNECT_STATE_CONNECTING)||(Connect_s[4].state==GPRSNET_E_CONNECT_STATE_CONNECTING))
            {
                //有正在连接中的操作
                if(ConnectOutTimeS==0)
                {
                    if(struct_gsmtx.para[0] == AT_NULL)
                    {
                        if(Connect_s[0].state==GPRSNET_E_CONNECT_STATE_CONNECTING)
                        {
                            Connect_s[0].state=GPRSNET_E_CONNECT_STATE_IDLE;
                            struct_gsmtx.buf = "AT+QICLOSE=0\r";
                        }
                        else if(Connect_s[1].state==GPRSNET_E_CONNECT_STATE_CONNECTING)
                        {
                            Connect_s[1].state=GPRSNET_E_CONNECT_STATE_IDLE;
                            struct_gsmtx.buf = "AT+QICLOSE=1\r";
                        }
                        else if(Connect_s[2].state==GPRSNET_E_CONNECT_STATE_CONNECTING)
                        {
                            Connect_s[2].state=GPRSNET_E_CONNECT_STATE_IDLE;
                            struct_gsmtx.buf = "AT+QICLOSE=2\r";
                        }
                        else if(Connect_s[3].state==GPRSNET_E_CONNECT_STATE_CONNECTING)
                        {
                            Connect_s[3].state=GPRSNET_E_CONNECT_STATE_IDLE;
                            struct_gsmtx.buf = "AT+QICLOSE=3\r";
                        }
                        else if(Connect_s[4].state==GPRSNET_E_CONNECT_STATE_CONNECTING)
                        {
                            Connect_s[4].state=GPRSNET_E_CONNECT_STATE_IDLE;
                            struct_gsmtx.buf = "AT+QICLOSE=4\r";
                        }
                        struct_gsmtx.type = TYPE_TX_AT;
                        struct_gsmtx.para[0] = AT_QICLOSE;
                        struct_gsmtx.len=strlen("AT+QICLOSE=0\r");
                    }
                }
            }
            else if(ConnectTimeS==0)
            {
                //没有正在连接中的操作
                if     ((Connect_s[ch].order>GPRSNET_E_CONNECT_ORDER_IDLE)&&(Connect_s[ch].order<=GPRSNET_E_CONNECT_ORDER_HTTP)&&(Connect_s[ch].state!=GPRSNET_E_CONNECT_STATE_CONNECTED))
                {
                    //(启用IP连接 || 域名连接)&& 没有连接上
                    //启动连接
                    if     ((Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_UDP_IP)||
                            (Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_TCP_IP))
                    {
                        //IP连接
                        if(struct_gsmtx.para[0] == AT_NULL)
                        {
                            if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6 \
                               || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                            {
                                Connect_s[ch].state=GPRSNET_E_CONNECT_STATE_CONNECTING;
                                ConnectOutTimeS   = CONNECT_MAX_TIME_S-3;
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_CIFSR;
                                struct_gsmtx.buf = "AT+CIFSR\r";
                                struct_gsmtx.len=strlen("AT+CIFSR\r");
                            }
                            else if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                                    || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                            {
                                //置位"正在连接中连接"
                                Connect_s[ch].state=GPRSNET_E_CONNECT_STATE_CONNECTING;
                                ConnectOutTimeS   = CONNECT_MAX_TIME_S-3;
                                struct_gsmtx.type = TYPE_TX_AT;
                                struct_gsmtx.para[0] = AT_QIDNSIP;
                                struct_gsmtx.buf = "AT+QIDNSIP=0\r";
                                struct_gsmtx.len=strlen("AT+QIDNSIP=0\r");
                            }
                            ConnectTimeS=CONNECT_MAX_TIME_S;
                            if(ch>=4)
                            {
                                ch=0;
                            }
                            else
                            {
                                ch++;
                            }
                        }
                    }
                    else if((Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_UDP_DNS)||
                            (Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_TCP_DNS))
                    {
                        //域名连接
                        if(struct_gsmtx.para[0] == AT_NULL)
                        {
                            //置位"正在连接中连接"
                            Connect_s[ch].state=GPRSNET_E_CONNECT_STATE_CONNECTING;
                            ConnectOutTimeS   = CONNECT_MAX_TIME_S-3;
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_QIDNSIP;
                            struct_gsmtx.buf = "AT+QIDNSIP=1\r";
                            struct_gsmtx.len=strlen("AT+QIDNSIP=1\r");
                            ConnectTimeS=CONNECT_MAX_TIME_S;
                            if(ch>=4)
                            {
                                ch=0;
                            }
                            else
                            {
                                ch++;
                            }
                        }
                    }
                    else if(Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_HTTP)
                    {
                        //域名连接
                        if((Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)&&(struct_gsmtx.para[0] == AT_NULL))
                        {
                            //置位"正在连接中连接"
                            Connect_s[ch].state=GPRSNET_E_CONNECT_STATE_CONNECTING;
                            ConnectOutTimeS   = CONNECT_MAX_TIME_S-3;
                            struct_gsmtx.type = TYPE_TX_AT;
                            struct_gsmtx.para[0] = AT_UHTTP_RESET;
                            struct_gsmtx.buf = "AT+UHTTP=0\r";
                            struct_gsmtx.len=strlen((char*)(struct_gsmtx.buf));
                            ConnectTimeS=CONNECT_MAX_TIME_S;
                            if(ch>=4)
                            {
                                ch=0;
                            }
                            else
                            {
                                ch++;
                            }
                        }
                    }
                }
                else
                {
                    if(ch>=4)
                    {
                        ch=0;
                    }
                    else
                    {
                        ch++;
                    }
                }
            }
        }
        //等待消息队列
        MODULE_OS_Q_PEND(prx,GsmAppTxQ,300,TRUE,err);
        if(err == MODULE_OS_ERR_NONE)
        {
            //
            uint8_t *pbuf;
            //申请缓存
            pbuf = MemManager_Get(GPRS_TX_MEM_SIZE);
            //判断AT 或 GPRS 或 SMS
            if     ((*prx).type == TYPE_TX_AT)
            {
                //AT指令
                GSM_DMA_Tx((*prx).buf,(*prx).len);
            }
            else if((*prx).type == TYPE_TX_GPRS)
            {
                //提取数据
                memcpy(pbuf,(*prx).buf,(*prx).len);
                s1.para[0] = (*prx).para[0];
                s1.len = (*prx).len;
                s1.buf = pbuf;
                (*prx).type = TYPE_TX_NULL;
                if(Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M10 \
                   || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M26 \
                   || Gprs_Moudle==GPRS_E_MOUDLE_QUECTEL_M35)
                {
                    //AT+QISEND=(ch),(len),(0x0D 0x00)
                    strcpy((char *)buf,"AT+QISEND=");
                    n=10;
                    buf[n++]=s1.para[0];
                    buf[n++]=',';
                    buf[n++]=   (s1.len)%10000/1000+'0';
                    buf[n++]=   (s1.len)%1000/100+'0';
                    buf[n++]=   (s1.len)%100/10+'0';
                    buf[n++]=   (s1.len)%10+'0';
                    buf[n++]=0x0D;
                    buf[n++]=0;
                    GSM_DMA_Tx(buf,n);
                    //等待'>'
                    MODULE_OS_DELAY_MS(50);
                    //data发送
                    GSM_DMA_Tx(s1.buf,s1.len);
                    //流量统计
                    GprsNet_FlowAdd(s1.para[0]-'0',prx->len);
                }
                else if(Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A6\
                        || Gprs_Moudle==GPRS_E_MOUDLE_AITHINKER_A7)
                {
                    strcpy((char *)buf,"AT+CIPSEND=");
                    n=11;
                    buf[n++]=s1.para[0];
                    buf[n++]=',';
                    buf[n++]=   (s1.len)%10000/1000+'0';
                    buf[n++]=   (s1.len)%1000/100+'0';
                    buf[n++]=   (s1.len)%100/10+'0';
                    buf[n++]=   (s1.len)%10+'0';
                    buf[n++]=0x0D;
                    buf[n++]=0;
                    GSM_DMA_Tx(buf,n);
                    //等待'>'
                    MODULE_OS_DELAY_MS(200);
                    //data发送
                    GSM_DMA_Tx(s1.buf,s1.len);
                    //流量统计
                    GprsNet_FlowAdd(s1.para[0]-'0',prx->len);
                }
                else if(Gprs_Moudle==GPRS_E_MOUDLE_UBLOX_LISAU200)
                {
                    if(Connect_s[s1.para[0]-'0'].order==GPRSNET_E_CONNECT_ORDER_HTTP)
                    {
                        Connect_s[(*prx).para[0]-'0'].TxState=1;
                        s1.len=sprintf((char*)pbuf,"AT+UHTTPC=0,5,\"%s\",\"post1.ffs\",\"%s\",0\r",Connect_s[s1.para[0]-'0'].pPathWrite,pbuf);
                        GSM_DMA_Tx(s1.buf,s1.len);
                    }
                }
                //等待数据发送完毕
                MODULE_OS_DELAY_MS(100);
            }
            else if((*prx).type == TYPE_TX_SMS)
            {
                //SMS
            }
            else
            {
                //无效
                ;
            }
            //释放缓存
            MemManager_Free(GPRS_TX_MEM_SIZE,pbuf);
        }
    }
}
/*******************************************************************************
函数功能: GSM测试任务实体
说    明: 发送给家里花生壳域名及路由器端口映射,实现远程升级控制
*******************************************************************************/
static void uctsk_GsmTest(void *pvParameters)
{
    GSM_STRUCT1 *prx;
    uint8_t csq_timer_s=0;
    uint8_t gps_timer_s=30;
    uint8_t timer=0;
    //创建GSM发送消息队列
    MODULE_OS_ERR err;
    MODULE_OS_Q_CREATE(GsmAppRxQ,"GsmAppRxQ",GSMAPPRX_QBUF_MAX);
    //
#if   (GPRS_INIT_ON_OFF==ON)
    GprsNet_OnOff(ON);
#endif
    //
    for(;;)
    {
        if(GprsNet_DebugControl_Enable==1)
        {
            MODULE_OS_DELAY_MS(1000);
            continue;
        }
        MODULE_OS_DELAY_MS(10);
        timer++;
        // 1S一次
        if(timer>=100)
        {
            timer=0;
            //----------关闭模式不处理
            //
            if(GprsNet_OnOffCmd==OFF)
            {
                if(GprsNet_s_Info.OnOff==ON)
                {
                    Bz_M10ResetFinish=0;
                    Connect_s[0].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[1].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[2].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[3].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[4].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    GSM_PWR(OFF);
                }
                MODULE_OS_DELAY_MS(1000);
                continue;
            }
            //模块一分钟未成功初始化,重上电
            else if(GprsNet_OnOffCmd==ON)
            {
                // 1分钟串口未接到任何数据-重启
                if(GprsNet_s_Info.OnOff==OFF || GprsNet_RxTimerS==0)
                {
                    while(1)
                    {
                        GSM_PWR(ON);
                        // 清空缓存环
                        Count_BufferLoopInit(&Gprs_BufferLoop,\
                                             Gprs_BufferLoop_Buf,\
                                             GPRS_BUFFERLOOP_BUF_MAX,\
                                             Gprs_BufferLoop_Node,\
                                             GPRS_BUFFERLOOP_NODE_MAX,\
                                             &Gprs_BufferLoop_Limit);
                        GprsNet_RxTimerS = 0;
                        GSM_DMA_Tx((uint8_t*)"AT\r",strlen("AT\r"));
                        MODULE_OS_DELAY_MS(500);
                        if(GprsNet_RxTimerS!=0)
                        {
                            GprsNet_s_Info.OnOff=ON;
                            break;
                        }
                        else
                        {
                            GprsNet_s_Info.OnOff=OFF;
                        }
                    }
                    //重启GSM
                    struct_gsmtx.type = TYPE_TX_AT;
                    struct_gsmtx.para[0] = AT_IPR;
                    struct_gsmtx.buf = "AT+IPR=115200\r";
                    struct_gsmtx.len = strlen("AT+IPR=115200\r");
                    Bz_M10ResetFinish=0;
                    Connect_s[0].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[1].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[2].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[3].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    Connect_s[4].state=GPRSNET_E_CONNECT_STATE_IDLE;
                    //
                    GprsNet_RxTimerS = GPRSNET_RXTIMER_MAX_S;
                }
            }
            if(GprsNet_RxTimerS!=0)
            {
                GprsNet_RxTimerS--;
            }

            //GPS数据
            if(gps_timer_s<30)
            {
                gps_timer_s++;
            }
            else if(Connect_s[0].TxState==0\
                    &&Connect_s[1].TxState==0\
                    &&Connect_s[2].TxState==0\
                    &&Connect_s[3].TxState==0\
                    &&Connect_s[4].TxState==0\
                    &&GprsNet_s_Info.GPS_Latitude==0\
                    &&GprsNet_s_Info.GPS_Longitude==0)
            {
                if(struct_gsmtx.para[0] == AT_NULL)
                {
                    gps_timer_s=0;
                    struct_gsmtx.type = TYPE_TX_AT;
                    struct_gsmtx.para[0] = AT_QCELLLOC;
                    struct_gsmtx.buf = "AT+QCELLLOC=1\r";
                    struct_gsmtx.len=strlen("AT+QCELLLOC=1\r");
                }
            }
            //信号强度
            if(csq_timer_s<10)
            {
                csq_timer_s++;
            }
            else if(Connect_s[0].TxState==0\
                    &&Connect_s[1].TxState==0\
                    &&Connect_s[2].TxState==0\
                    &&Connect_s[3].TxState==0\
                    &&Connect_s[4].TxState==0)
            {
                if(struct_gsmtx.para[0] == AT_NULL)
                {
                    csq_timer_s=0;
                    struct_gsmtx.type = TYPE_TX_AT;
                    struct_gsmtx.para[0] = AT_2_CSQ;
                    struct_gsmtx.buf = "AT+CSQ\r";
                    struct_gsmtx.len=strlen("AT+CSQ\r");
                }
            }
            //连接计时器
            if(ConnectTimeS!=0)ConnectTimeS--;
            //连接超时计时
            if(ConnectOutTimeS!=0)ConnectOutTimeS--;
        }
        //电话鉴权
        RingParse();
        //GSM接收总解析
        MODULE_OS_Q_PEND(prx,GsmAppRxQ,1,FALSE,err);
        if(err==MODULE_OS_ERR_NONE)
        {
            //接收到一个消息队列
            if     ((*prx).type==TYPE_RX_SMS)
            {
                //SMS数据处理
                SmsParse(prx);
            }
            else if((*prx).type==TYPE_RX_GPRS)
            {
                //GPRS数据处理
                if     ((*prx).para[0]=='0')
                {
                    //用户函数---连接0解析(远程升级监控服务器)
#ifdef GPRS_RX_PARSE_CH0
                    GPRS_RX_PARSE_CH0(prx->buf,prx->len);
                    GprsNet_FlowAdd(0,prx->len);
#endif
                }
                else if((*prx).para[0]=='1')
                {
#ifdef GPRS_RX_PARSE_CH1
                    GPRS_RX_PARSE_CH1((*prx).buf,(*prx).len);
                    GprsNet_FlowAdd(1,prx->len);
#endif
                }
                else if((*prx).para[0]=='2')
                {
#ifdef GPRS_RX_PARSE_CH2
                    GPRS_RX_PARSE_CH2((*prx).buf,(*prx).len);
                    GprsNet_FlowAdd(2,prx->len);
#endif
                }
                else if((*prx).para[0]=='3')
                {
#ifdef GPRS_RX_PARSE_CH3
                    GPRS_RX_PARSE_CH3((*prx).buf,(*prx).len);
                    GprsNet_FlowAdd(3,prx->len);
#endif
                }
                else if((*prx).para[0]=='4')
                {
#ifdef GPRS_RX_PARSE_CH4
                    GPRS_RX_PARSE_CH4((*prx).buf,(*prx).len);
                    GprsNet_FlowAdd(4,prx->len);
#endif
                }
                else
                {
                    //无效不处理
                    ;
                }
                //
                (*prx).type = TYPE_RX_NULL;
                GPRS_RBuf1_OverBz=0;
            }
        }
    }
}
//-----------------------------------------------------------------------------电话鉴权
void RingParse(void)
{
    //-----电话接听
    if((Ring_S.order==1)&&(Ring_S.state!=2))
    {
        if(struct_gsmtx.para[0] == AT_NULL)
        {
            //接电话
            struct_gsmtx.type = TYPE_TX_AT;
            struct_gsmtx.para[0] = AT_ATA;
            struct_gsmtx.buf = "ATA\r";
            struct_gsmtx.len=strlen("ATA\r");
        }
    }
    //-----电话挂断
    else if((Ring_S.order==1)&&(Ring_S.state!=0))
    {
        if(struct_gsmtx.para[0] == AT_NULL)
        {
            //接电话
            struct_gsmtx.type = TYPE_TX_AT;
            struct_gsmtx.para[0] = AT_ATH;
            struct_gsmtx.buf = "ATH\r";
            struct_gsmtx.len=strlen("ATH\r");
        }
    }
}
//-----------------------------------------------------------------------------短信鉴权与解析
void SmsParse(GSM_STRUCT1 *prx)
{
    //无权限操作
    //万能号码权限操作
    if(strstr((char const*)(*prx).para,(char const*)SMSPHOTO_ALMIGHTY))
    {
        //短信解析
        /*
        if(strstr((const char *)(*prx).buf,"0031"))
        {
            RelayBuf[0]=10;
        }
        if(strstr((const char *)(*prx).buf,"0032"))
        {
            RelayBuf[1]=10;
        }
        if(strstr((const char *)(*prx).buf,"0033"))
        {
            RelayBuf[2]=10;
        }
        if(strstr((const char *)(*prx).buf,"0034"))
        {
            RelayBuf[3]=10;
        }
        */
        /*
        struct_gsmtx.type = TYPE_TX_AT;
        struct_gsmtx.para[0] = AT_CMGD;
        struct_gsmtx.buf = "AT+QMGDA=\"DEL ALL\"\r";
        struct_gsmtx.len=strlen("AT+QMGDA=\"DEL ALL\"\r");
        */
    }
    //特定号码权限操作
    if((strstr((char const*)(*prx).para,(char const*)SMSPHOTO_ALMIGHTY))||
       (strstr((char const*)(*prx).para,(char const*)SMSPHOTO_ALMIGHTY)))
    {
    }
    (*prx).type = TYPE_RX_NULL;
    GPRS_RBuf1_OverBz=0;
}
void UctskGprsNet_HttpGet(uint8_t *pDns,char *pBuf,uint16_t *plen)
{
}
void UctskGprsNet_HttpPost(uint8_t *pDns,char *pBuf,uint16_t *plen)
{}
//-----------------------------------------------------------------------------任务实体---END
/*******************************************************************************
                              GPIO初始化
*******************************************************************************/
static void GSM_PWR(uint8_t OnOff)
{
    uint16_t i=0;
    i=i;
    if(OnOff==ON)
    {
        GPRS_PWR_H;
        GPRS_VCC_EN_H;
        GPRS_RESET_H;
        //模块初始化
#ifdef   GPRS_PORT_VCCEN
        GPRS_VCC_EN_L;
        MODULE_OS_DELAY_MS(1000);
        GPRS_VCC_EN_H;
        MODULE_OS_DELAY_MS(1000);
#endif
        GPRS_PWR_L;
        MODULE_OS_DELAY_MS(3000);
        GPRS_PWR_H;
        MODULE_OS_DELAY_MS(3000);
    }
    else
    {
#ifdef   GPRS_PORT_VCCEN
        GPRS_VCC_EN_L;
#else
#if		(defined(NRF51)||defined(NRF52))
        GPRS_PWR_L;
		MODULE_OS_DELAY_MS(3000);	
#else
        GSM_DMA_Tx((uint8_t*)"AT+QPOWD=1\r",strlen("AT+QPOWD=1\r"));
        while(GprsNet_s_Info.OnOff==ON)
        {
            MODULE_OS_DELAY_MS(10);
            i++;
            if(i>=1000)
            {
                break;
            }
        }
#endif
        //
        GPRS_PWR_H;
        //
        //MODULE_OS_DELAY_MS(1000);
#endif
    }
}
static void GSM_M10_INIT(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    MODULE_MEMORY_S_PARA *pPara;
    //时钟初始化
    GPRS_GPIO_RCC_ENABLE;
    //GPIO初始化
    //-----GPRS_PWR | GPRS_VCC_EN | GPRS_RESET
#ifdef GPRS_PORT_VCCEN
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin     =  GPRS_PIN_VCCEN;
    GPIO_Init(GPRS_PORT_VCCEN, &GPIO_InitStructure);
#endif
    //
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
#ifdef GPRS_PORT_PWR
    GPIO_InitStructure.GPIO_Pin     =  GPRS_PIN_PWR;
    GPIO_Init(GPRS_PORT_PWR, &GPIO_InitStructure);
    GPRS_PWR_H;
#endif
#ifdef GPRS_PORT_RESET
    GPIO_InitStructure.GPIO_Pin     =  GPRS_PIN_RESET;
    GPIO_Init(GPRS_PORT_RESET, &GPIO_InitStructure);
#endif
#elif (defined(NRF51)||defined(NRF52))
#ifdef GPRS_PIN_VCCEN
    nrf_gpio_cfg_output(GPRS_PIN_VCCEN);
    nrf_gpio_pin_write(GPRS_PIN_VCCEN,0);
#endif
#ifdef GPRS_PIN_PWR
    nrf_gpio_cfg_output(GPRS_PIN_PWR);
    nrf_gpio_pin_write(GPRS_PIN_PWR,0);
#endif
#ifdef GPRS_PIN_NETLIGHT
    nrf_gpio_cfg_input(GPRS_PIN_NETLIGHT, NRF_GPIO_PIN_NOPULL);
#endif
#endif
    //-----初始化消息队列
    MODULE_OS_SEM_CREATE(GsmSubsectionSem,"GsmSubsectionSem",0);
    //-----初始化缓存环
    Count_BufferLoopInit(&Gprs_BufferLoop,\
                         Gprs_BufferLoop_Buf,\
                         GPRS_BUFFERLOOP_BUF_MAX,\
                         Gprs_BufferLoop_Node,\
                         GPRS_BUFFERLOOP_NODE_MAX,\
                         &Gprs_BufferLoop_Limit);
    //-----初始化信息结构体
    memset((char*)&GprsNet_s_Info,0,sizeof(GprsNet_s_Info));
#if   (defined(STM32F1)||defined(STM32F4))
    //-----参数加载
    GprsNet_s_Info.Flow_B  =  BSP_BKPRAM_READ(BSP_BKPRAM_GPRS_FLOW_H);
    GprsNet_s_Info.Flow_B  <<=   16;
    GprsNet_s_Info.Flow_B  += BSP_BKPRAM_READ(BSP_BKPRAM_GPRS_FLOW_L);
    //
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
    //设置APN为CMNET
    if(pPara->Gsm_Apn<=2)
    {
        GprsNet_s_Info.Apn=pPara->Gsm_Apn;
    }
    else
    {
        GprsNet_s_Info.Apn=0;
        pPara->Gsm_Apn=0;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    //-----
#elif (defined(NRF51)||defined(NRF52))
    GprsNet_s_Info.Flow_B=0;
    GprsNet_s_Info.Apn=0;
	// 没有这个延时51822下面的串口初始化因电平压差起不来
    MODULE_OS_DELAY_MS(50);
		//MODULE_OS_DELAY_MS(10000);
#endif
	//串口初始化
    UART_INIT(GPRS_UARTX,115200);
}
void GSM_DMA_Tx(uint8_t *pbuf,uint32_t len)
{
    UART_DMA_Tx(GPRS_UARTX,pbuf,len);

    if(GprsNet_DebugTest_Enable==ON)
    {
        DebugOut((int8_t*)pbuf,len);
    }
}
void GprsNet_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    if(GprsNet_DebugControl_Enable==1)
    {
        DebugOut((int8_t*)pbuf,len);
        return;
    }
    //数据复制到缓存环
    Count_BufferLoopPush(&Gprs_BufferLoop,pbuf,len);
    //发送消息队列
    MODULE_OS_SEM_POST(GsmSubsectionSem);
    if(GprsNet_DebugTest_Enable==ON)
    {
        DebugOut((int8_t*)pbuf,len);
    }
    GprsNet_RxTimerS=GPRSNET_RXTIMER_MAX_S;
}
//-----测试
void GprsNet_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        GprsNet_DebugTest_Enable=1;
    }
    else
    {
        GprsNet_DebugTest_Enable=0;
    }
}
void GprsNet_DebugControlOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        GprsNet_DebugControl_Enable=1;
    }
    else
    {
        GprsNet_DebugControl_Enable=0;
    }
}
void GprsNet_DebugRx(uint8_t *d,uint16_t len)
{
    if(GprsNet_DebugControl_Enable==0)
        return;
    UART_DMA_Tx(GPRS_UARTX,d,len);
}
//-----控制
void GprsNet_OnOff(uint8_t OnOff)
{
    //正在执行复位指令
    if(GprsNet_ResetCmd==ON)
    {
        GprsNet_OnOffCmdNext  =  OnOff;
    }
    else
    {
        GprsNet_OnOffCmd      =  OnOff;
        GprsNet_OnOffCmdNext  =  OnOff;
    }
}
//-----流量更新
static uint8_t GprsNet_Flow_Lock=0;
static void GprsNet_FlowAdd(uint8_t ch,uint16_t val)
{
#if (defined(STM32F1)||defined(STM32F4))
    uint32_t i32;
    while(GprsNet_Flow_Lock==1)
    {
        MODULE_OS_DELAY_MS(10);
    }
    GprsNet_Flow_Lock=1;
    //读取
    i32  =     BSP_BKPRAM_READ(BSP_BKPRAM_GPRS_FLOW_H);
    i32  <<=   16;
    i32  +=    BSP_BKPRAM_READ(BSP_BKPRAM_GPRS_FLOW_L);
    //更新
    if(Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_UDP_IP\
       ||Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_UDP_DNS)
    {
        i32+=28;
        i32+=val;
    }
    else if(Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_TCP_IP\
            ||Connect_s[ch].order==GPRSNET_E_CONNECT_ORDER_TCP_DNS)
    {
        ;
    }
    //存入
    BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_H,(uint16_t)(i32>>16));
    BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_L,(uint16_t)i32);
    GprsNet_s_Info.Flow_B  =  i32;
    GprsNet_Flow_Lock=0;
#else
    GprsNet_Flow_Lock=GprsNet_Flow_Lock;
    ch=ch;
    val=val;
#endif
}
void GprsNet_FlowWrite(uint32_t val)
{
#if (defined(STM32F1)||defined(STM32F4))
    while(GprsNet_Flow_Lock==1)
    {
        MODULE_OS_DELAY_MS(10);
    }
    GprsNet_Flow_Lock=1;
    BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_H,(uint16_t)(val>>16));
    BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_L,(uint16_t)val);
    GprsNet_s_Info.Flow_B  =  val;
    GprsNet_Flow_Lock=0;
#else
    GprsNet_Flow_Lock=GprsNet_Flow_Lock;
    val=val;
#endif
}
#elif (UCSK_GPRSNET_MODE   == 2)
/*******************************************************************************
函数功能: GSM测试任务创建
*******************************************************************************/
#include "Bsp_Led.h"
#include "Bsp_Uart.h"
MODULE_OS_TASK_TAB(App_TaskGsmTestTCB);
MODULE_OS_TASK_STK(App_TaskGsmTestStk,APP_TASK_GSM_TEST_STK_SIZE);
static void uctsk_GsmTest(void *pvParameters);
void  App_GsmTestTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-GsmTest",\
                          uctsk_GsmTest,\
                          APP_TASK_GSM_TEST_PRIO,\
                          App_TaskGsmTestStk,\
                          APP_TASK_GSM_TEST_STK_SIZE,\
                          App_TaskGsmTestTCB,\
                          NULL);
}

#include "app_uart.h"
#include "nrf_delay.h"
#include "SEGGER_RTT.h"
GSM_STRUCT1 struct_gsmtx,struct_gsmtx1;
MODULE_OS_Q(GsmAppTxQ);
//static uint8_t TestTxBuf[30];
static uint8_t TestRxBuf[200];
#define GSMAPPTX_QBUF_MAX        10
static void uctsk_GsmTest(void *pvParameters)
{
    //uint8_t i;
    uint16_t i16;

    /*
    MODULE_OS_ERR err;
#if   (defined(NRF51))
    //MODULE_OS_Q_CREATE(GsmAppTxQ,"GsmAppTxQ",GSMAPPTX_QBUF_MAX);
    GsmAppTxQ = xQueueCreate(2, sizeof(GSM_STRUCT1));
    for(;;)
    {
        // 发送数据
        if(GsmAppTxQ!=0)
        {
            //MODULE_OS_Q_POST(GsmAppTxQ,struct_gsmtx,sizeof(GSM_STRUCT1));
            xQueueSend( GsmAppTxQ, ( void * ) &struct_gsmtx, ( portTickType )10);
        }

        // 接收
        if(GsmAppTxQ!=0)
        {
            MODULE_OS_Q_PEND(&struct_gsmtx1,GsmAppTxQ,300,TRUE,err);
        }

        if(err == MODULE_OS_ERR_NONE)
        {
            struct_gsmtx.len++;
            BSP_LED_LED3_TOGGLE;
        }
        //释放CPU执行权
        MODULE_OS_DELAY_MS(100);
        BSP_LED_LED4_TOGGLE;
    }
#endif
    */
    UART_INIT(1,115200);
    //
    nrf_gpio_cfg_output(GPRS_PIN_VCCEN);
    GPRS_VCC_EN_L;
    MODULE_OS_DELAY_MS(2000);
    GPRS_VCC_EN_H;
    MODULE_OS_DELAY_MS(3000);

    nrf_gpio_cfg_output(GPRS_PIN_PWR);
    GPRS_PWR_H;
    MODULE_OS_DELAY_MS(2500);
    GPRS_PWR_L;
    for(;;)
    {
        memset(TestRxBuf,0,sizeof(TestRxBuf));
        UART_DMA_Tx(1,"ATE0\r",strlen("ATE0\r"));
        MODULE_OS_DELAY_MS(1000);

        memset(TestRxBuf,0,sizeof(TestRxBuf));
        UART_DMA_Tx(1,"ATI\r",strlen("ATI\r"));
        MODULE_OS_DELAY_MS(1000);
    }
}
void GprsNet_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    if(len<=sizeof(TestRxBuf))
    {
        memcpy(TestRxBuf,pbuf,len);
    }
}
#else
#error Please Set UCSK_GPRSNET_MODE to uctsk_GprsNet.c
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
