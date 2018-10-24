/**
  ******************************************************************************
  * @file    Bsp_Esp8266.c
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

/* ESP8266 模块接线图
    ESP8266模块    STM32-V5开发板


        UTXD   ---  PC7/USART6_RX
        GND    ---  GND
        CH_PD  ---  PI0  (接3.3V 或 IO控制模块掉电， 0表示掉电  1表示正常上电工作）
        GPIO2  ---  PG15 (接3.3V 或 IO驱动为高, 不用)
        GPIO16 ---  PB7  (接3.3V 或 IO控制 wifi 硬件复位)
        GPIO0  ---  PG8  (接3.3V 或 IO控制启动模式。0代表进入系统升级，1表示正常引导用户程序（AT指令）)
        VCC    ---  3.3  (供电)
        URXD   ---  PG14/USART6_TX


    模块缺省波特率 9600;  支持的范围：110~460800bps          ---- 本例子会将模块波特率切换为 115200
    在板子上电初始跑boot rom的一段log，需要在 74880 的波特率下正常打印。下面是打印出来的内容.

    ----------- PD = 1 之后 74880bps 打印如下内容 ------

     ets Jan  8 2013,rst cause:1, boot mode:(3,6)

    load 0x40100000, len 25052, room 16
    tail 12
    chksum 0x0b
    ho 0 tail 12 room 4
    load 0x3ffe8000, len 3312, room 12
    tail 4
    chksum 0x53
    load 0x3ffe8cf0, len 6576, room 4
    tail 12
    chksum 0x0d
    csum 0x0d

    ----------- 之后是 9600bps 打印 ---------------

    [Vendor:www.ai-thinker.com Version:0.9.2.4]

    ready


    使用串口超级终端软件时，需要设置 终端 - 仿真 - 模式 页面勾选“新行模式”.


    【修改波特率】
    AT+CIOBAUD=?     ---- 查询命令参数
    +CIOBAUD:(9600-921600)

    OK

    AT+CIOBAUD=115200
    BAUD->115200

    【选择 WIFI 应用模式 】
    AT+CWMODE=1
        1   Station 模式
        2   AP 模式
        3   AP 兼 Station 模式

    【列出当前可用 AP】
    AT+CWLAP=<ssid>,< mac >,<ch>
    AT+CWLAP

    【AT+CWJAP加入 AP】
    AT+CWJAP=<ssid>,< pwd >

*/
/* Includes ------------------------------------------------------------------*/
#include "Bsp_Esp8266.h"
#include "Bsp_Uart.h"
#include "uctsk_Debug.h"
/* Private typedef -----------------------------------------------------------*/
enum BSP_ESP8266_E_AT
{
    INIT=0,
    AT,                     //测试启动
    ATE,                    //开关回显
    AT_GMR,                 //查询版本信息
    //AT_UART,                //设置UART
    CIPSTATUS,              //查询网络连接状态

    AT_CWMODE,              //设置工作模式
    AT_CWJAP,               //连接路由
    AT_CIFSR,               //查询设备IP地址
    AT_CIPMUX,              //使能多链接
    //AT_CWAUTOCONN,          //使能上电自动连接
    //AT_CWSTARTSMART,        //使能自动配置网络

    //AT_WAITFORCONFIG,       //等待app配置完成

    AT_CIPSTART1,           //设置udp连接
    AT_CIPSTART2,
    AT_CIPSTART3,
    AT_CIPSTART4,
    AT_CIPSTART5,
//      AT_CIPMODE,         //设置透传模式
    AT_CIPSEND,             //申请发送数据
    AT_CIPSEND1,
    AT_SEND_SERVER_DATA,    //向服务器发送数据
    AT_WAIT_FOR_RESPOND,    //等待服务器

//      AT_RST,             //重启
//      AT_GMR,             //查询版本信息

    AT_SEND,                //发送数据
    AT_RECEIVE,             //接收数据
    AT_RST,
    IDLE=0xFF,
};
typedef struct BSP_ESP8266_S_TX
{
    uint8_t     len;
    uint8_t     ch;				//	0-4
    uint8_t     AtCmd;
    uint8_t     Flag_state  :2;  // 0-空闲1-待发送
    uint8_t     Flag_res    :6;
    uint8_t     cmt;
    char        buf[25+225];    //  AT指令50      数据200
} BSP_ESP8266_S_TX;
typedef struct BSP_ESP8266_S_RX
{
    uint8_t     len;    //  0-结构可用  1-结构不可用
    uint8_t     buf[250];
} BSP_ESP8266_S_RX;

/* Private define ------------------------------------------------------------*/
static BSP_ESP8266_S_TX BspEsp8266_s_Tx;
//
#define BSP_ESP8266_RX_BUF_MAX  256
static BSP_ESP8266_S_RX BspEsp8266_s_Rx;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSP_ESP8266_S_INFO      BspEsp8266_s_Info;
BSP_ESP8266_S_CONNECT BspEsp8266_s_ConnectBuf[5];
static uint8_t BspEsp8266_DebugTest_Enable      =   OFF;
static uint8_t BspEsp8266_DebugControl_Enable   =   OFF;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void BspEsp8266_Test_100ms(void);
static void BspEsp8266_TestParse(uint8_t *pbuf,uint16_t len);
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   向模块发送AT命令。
 * @note    本函数自动在AT字符串口增加<CR>字符
 * @param   *pstr   -   AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
 * @return  None
 */

static void BspEsp8266_SendAT(char *pstr,uint16_t len)
{
    if(BspEsp8266_DebugControl_Enable==ON)
    {
        return;
    }
    UART_DMA_Tx(BSP_ESP8266_UARTX, (uint8_t *)pstr, len);
    if(BspEsp8266_DebugTest_Enable==ON)
    {
        DebugOut((int8_t*)pstr,len);
    }
}

/**
 * @brief   Esp8266初始化函数
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspEsp8266_Init(void)
{
#if     (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_ESP8266_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Mode    =   GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =   GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =   GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed   =   GPIO_Speed_100MHz;
#ifdef  BSP_ESP8266_RST_PIN
    GPIO_InitStructure.GPIO_Pin     =   BSP_ESP8266_RST_PIN;
    GPIO_Init(BSP_ESP8266_RST_PORT, &GPIO_InitStructure);
#endif
#ifdef  BSP_ESP8266_PWR_PIN
    GPIO_InitStructure.GPIO_Pin     =   BSP_ESP8266_PWR_PIN;
    GPIO_Init(BSP_ESP8266_PWR_PORT, &GPIO_InitStructure);
#endif
#ifdef  BSP_ESP8266_GPIO0_PIN
    GPIO_InitStructure.GPIO_Pin     =   BSP_ESP8266_GPIO0_PIN;
    GPIO_Init(BSP_ESP8266_GPIO0_PORT, &GPIO_InitStructure);
#endif
#ifdef  BSP_ESP8266_GPIO2_PIN
    GPIO_InitStructure.GPIO_Pin     =   BSP_ESP8266_GPIO2_PIN;
    GPIO_Init(BSP_ESP8266_GPIO2_PORT, &GPIO_InitStructure);
#endif

#elif   (defined(NRF51)||defined(NRF52))
#ifdef  BSP_ESP8266_RST_PIN
    nrf_gpio_cfg_output(BSP_ESP8266_RST_PIN);
#endif
#ifdef  BSP_ESP8266_PWR_PIN
    nrf_gpio_cfg_output(BSP_ESP8266_PWR_PIN);
#endif
#ifdef  BSP_ESP8266_GPIO0_PIN
    nrf_gpio_cfg_output(BSP_ESP8266_GPIO0_PIN);
#endif
#ifdef  BSP_ESP8266_GPIO2_PIN
    nrf_gpio_cfg_output(BSP_ESP8266_GPIO2_PIN);
#endif
#endif
    //
    BSP_ESP8266_GPIO2_H;
    BSP_ESP8266_PWR_ON;
    BSP_ESP8266_EXIT_ISP;
    //UART_INIT(BSP_ESP8266_UARTX,115200);
    //UART_INIT(BSP_ESP8266_UARTX,74880);
    //
    memset((char*)&BspEsp8266_s_Tx      ,   0   ,   sizeof(BspEsp8266_s_Tx));
    memset((char*)&BspEsp8266_s_Rx      ,   0   ,   sizeof(BspEsp8266_s_Rx));
    memset((char*)&BspEsp8266_s_ConnectBuf, 0   ,   sizeof(BspEsp8266_s_ConnectBuf));
    memset((char*)&BspEsp8266_s_Info    ,   0   ,   sizeof(BspEsp8266_s_Info));

    //UART_INIT(BSP_ESP8266_UARTX,74880);
    BSP_ESP8266_RST_L;
    MODULE_OS_DELAY_MS(20);
    BSP_ESP8266_RST_H;
    MODULE_OS_DELAY_MS(10);
    MODULE_OS_DELAY_MS(2000);
    UART_INIT(BSP_ESP8266_UARTX,115200);
}

/**
 * @brief   中断调用
 * @note    只周转数据,不处理数据
 * @param   None
 * @return  None
 */
void BspEsp8266_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    if(BspEsp8266_DebugControl_Enable==1)
    {
        DebugOut((int8_t*)pbuf,len);
        return;
    }
    if(BspEsp8266_s_Rx.len==0 && len<=250)
    {
        BspEsp8266_s_Rx.len=len;
        memcpy(BspEsp8266_s_Rx.buf,pbuf,len);
        if(BspEsp8266_DebugTest_Enable==ON)
        {
            DebugOut((int8_t*)pbuf,len);
        }
    }
}

/**
 * @brief   发送数据
 * @note
 * @param   None
 * @return  None
 */

uint8_t BspEsp8266_Tx(BSP_ESP8266_E_CH ch,uint8_t *pbuf,uint16_t len)
{
    // 未连上服务器
    if(BspEsp8266_s_ConnectBuf[ch].state!=BSP_ESP8266_E_CONNECT_STATE_CONNECTED)
    {
        return ERR;
    }
    // 发送未空闲
    if(BspEsp8266_s_Tx.Flag_state!=0)
    {
        return ERR;
    }
    // 长度限制
    if(len>250)
    {
        return ERR;
    }
    BspEsp8266_s_Tx.AtCmd       =   AT_CIPSEND;
    BspEsp8266_s_Tx.Flag_state  =   0;
    BspEsp8266_s_Tx.len         =   len;
    memcpy(&BspEsp8266_s_Tx.buf[0],pbuf,len);
    return OK;
}


void BspEsp8266_100ms(void)
{
    //static uint8_t n, Ms100_cnt,Ms100_cnt2 = 0;
    //static char AT_Send_Fifo[256] = {0};
    uint8_t i=0,j=0;
    uint16_t i16=0;
    char *p;
    uint8_t *p1;
    //
    BspEsp8266_Test_100ms();
    //----------计时
    if(BspEsp8266_s_Tx.cmt!=0xFF)
    {
        BspEsp8266_s_Tx.cmt++;
    }
    //----------接收数据
    if(BspEsp8266_s_Rx.len!=0)
    {
        //已通过手机设置连接wifi
        if(strstr((const char *)BspEsp8266_s_Rx.buf,"smartconfig connected wifi"))
        {
        }
        //已连接服务器
        else if(strstr((const char *)BspEsp8266_s_Rx.buf,"CONNECT"))
        {
        }
        //接收到数据
        else if(strstr((const char *)BspEsp8266_s_Rx.buf,"+IPD,"))
        {
            p=strstr((const char *)BspEsp8266_s_Rx.buf,"+IPD,");
            //提取通道
            i=p[5]-'0';
            //提取数据长度
            i16=0;
            for(j=0; j<4; j++)
            {
                if(('0'<=p[7+j]) &&(p[7+j]<='9') )
                {
                    i16*=10;
                    i16+=p[7+j]-'0';
                }
                else
                {
                    break;
                }
            }
            //提取数据段指针
            p1=(uint8_t*)strstr((const char *)p,":");
            p1++;
            //
            switch (i)
            {
#ifdef  BSP_ESP8266_RX_PARSE_CH0
                case 0:
                    BSP_ESP8266_RX_PARSE_CH0(p1,i16);
                    BspEsp8266_s_Rx.len =   0;
                    break;
#endif
#ifdef  BSP_ESP8266_RX_PARSE_CH1
                case 1:
                    BspEsp8266_s_Rx.len =   0;
                    break;
#endif
#ifdef  BSP_ESP8266_RX_PARSE_CH2
                case 2:
                    BspEsp8266_s_Rx.len =   0;
                    break;
#endif
#ifdef  BSP_ESP8266_RX_PARSE_CH3
                case 3:
                    BspEsp8266_s_Rx.len =   0;
                    break;
#endif
#ifdef  BSP_ESP8266_RX_PARSE_CH4
                case 4:
                    BspEsp8266_s_Rx.len =   0;
                    break;
#endif
                default:
                    break;
            }
        }
    }
    //----------发送
    if(BspEsp8266_s_Tx.Flag_state==1)
    {
        BspEsp8266_SendAT(BspEsp8266_s_Tx.buf,BspEsp8266_s_Tx.len);
        BspEsp8266_s_Tx.Flag_state=2;
    }
    //----------发送处理
    switch(BspEsp8266_s_Tx.AtCmd)
    {
        // 初始化完毕后的空闲
        case IDLE:

            break;
        // 初始等待
        case INIT:
            // 上电等待5s
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT;
                BspEsp8266_s_Tx.Flag_state   =   0;
                BspEsp8266_s_Tx.cmt     =   0;
            }
            break;
        // 测试模块
        case AT:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            // 处理接收数据
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"OK"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   ATE;
                    BspEsp8266_s_Info.Flag_Install=1;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd       =   AT;
                BspEsp8266_s_Tx.Flag_state       =   0;
                BspEsp8266_s_Info.Flag_Install  =   0;
            }
            break;
        //关闭回显
        case ATE:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"ATE0\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"OK"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CWMODE;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   ATE;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        //查询版本信息
        /*
        case AT_GMR:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+GMR\r\n";
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"OK"))
                {
                    BspEsp8266_s_Tx.state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CWMODE;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_GMR;
                BspEsp8266_s_Tx.state   =   0;
            }
            break;
        */
        //查询网络连接状态
        /*
        case CIPSTATUS:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CIPSTATUS\r\n";
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"OK"))
                {
                    BspEsp8266_s_Tx.state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CWMODE;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   CIPSTATUS;
                BspEsp8266_s_Tx.state   =   0;
            }
            break;
        */
        //配置wifi工作模式
        case AT_CWMODE:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT+CWMODE=3\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"OK"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CWJAP;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CWMODE;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        // 连接路由
        case AT_CWJAP:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT+CWJAP=\"xsl\",\"19820824\"\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"WIFI GOT IP"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIFSR;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"FAIL"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=100)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CWJAP;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        // 查询本地IP
        case AT_CIFSR:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT+CIFSR\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                for(j=0; j<=3; j++)
                {
                    switch(j)
                    {
                        case 0:
                            p   =   strstr((const char *)BspEsp8266_s_Rx.buf,"+CIFSR:APIP");
                            p1  =   BspEsp8266_s_Info.LotalIP_AP;
                            break;
                        case 1:
                            p   =   strstr((const char *)BspEsp8266_s_Rx.buf,"+CIFSR:APMAC");
                            p1  =   BspEsp8266_s_Info.MacAddr_AP;
                            break;
                        case 2:
                            p   =   strstr((const char *)BspEsp8266_s_Rx.buf,"+CIFSR:STAIP");
                            p1  =   BspEsp8266_s_Info.LotalIP_STA;
                            break;
                        case 3:
                            p   =   strstr((const char *)BspEsp8266_s_Rx.buf,"+CIFSR:STAMAC");
                            p1  =   BspEsp8266_s_Info.MacAddr_STA;
                            break;
                        default:
                            break;
                    }
                    //提取本地IP
                    if((j==0||j==2)&&(p!=NULL))
                    {
                        i=0;
                        while(i<=3)
                        {
                            p=strstr((const char *)p,".");
                            if(p!=NULL)
                            {
                                if(((*(p-1))<='9') && ((*(p-1))>='0'))
                                {
                                    //个位有效
                                    p1[i]=*(p-1)-'0';
                                    if(((*(p-2))<='9') && ((*(p-2))>='0'))
                                    {
                                        p1[i]+=(*(p-2)-'0')*10;
                                        if(((*(p-3))<='9') && ((*(p-3))>='0'))
                                        {
                                            p1[i]+=(*(p-3)-'0')*100;
                                        }
                                    }
                                }
                                else
                                {
                                    p1[0]=p1[1]=p1[2]=p1[3]=0;
                                    break;
                                }
                            }
                            else
                            {
                                p1[0]=p1[1]=p1[2]=p1[3]=0;
                                break;
                            }
                            *p=0x0D;
                            i++;
                            if(i==3)
                            {
                                if((*(p+1)<='9')&&(*(p+1)>='0'))
                                {
                                    p1[i]=*(p+1)-'0';
                                    if((*(p+2)<='9')&&(*(p+2)>='0'))
                                    {
                                        p1[i]=p1[i]*10;
                                        p1[i]+=*(p+2)-'0';
                                        if((*(p+3)<='9')&&(*(p+3)>='0'))
                                        {
                                            p1[i]=p1[i]*10;
                                            p1[i]+=*(p+3)-'0';
                                        }
                                    }
                                }
                                else
                                {
                                    p1[0]=p1[1]=p1[2]=p1[3]=0;
                                    break;
                                }
                                i++;
                            }
                        }
                    }
                    //提取本地MAC
                    else if((j==1||j==3)&&(p!=NULL))
                    {
                        p=strstr((const char *)p,",\"");
                        if(p==NULL)
                            continue;
                        p++;
                        p++;
                        Count_AsciiToHex((uint8_t*)&p[0],&p1[0],2);
                        Count_AsciiToHex((uint8_t*)&p[3],&p1[1],2);
                        Count_AsciiToHex((uint8_t*)&p[6],&p1[2],2);
                        Count_AsciiToHex((uint8_t*)&p[9],&p1[3],2);
                        Count_AsciiToHex((uint8_t*)&p[12],&p1[4],2);
                        Count_AsciiToHex((uint8_t*)&p[15],&p1[5],2);
                    }
                }
                if(BspEsp8266_s_Info.LotalIP_AP[0]!=0\
                   ||BspEsp8266_s_Info.LotalIP_AP[1]!=0\
                   ||BspEsp8266_s_Info.LotalIP_AP[2]!=0\
                   ||BspEsp8266_s_Info.LotalIP_AP[3]!=0)
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPMUX;
                }

                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CIFSR;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        case AT_CIPMUX:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT+CIPMUX=1\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"OK"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART1;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CIPMUX;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        //连接服务器
        case AT_CIPSTART1:
        case AT_CIPSTART2:
        case AT_CIPSTART3:
        case AT_CIPSTART4:
        case AT_CIPSTART5:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART1)
                {
                    if(BSP_ESP8266_CONNECT_1!=NULL)
                    {
                        strcpy(BspEsp8266_s_Tx.buf,BSP_ESP8266_CONNECT_1);
                        BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
                    }
                    else
                    {
                        BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART2;
                        BspEsp8266_s_Tx.Flag_state   =   0;
                    }
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART2)
                {
#ifdef  BSP_ESP8266_CONNECT_2
                    strcpy(BspEsp8266_s_Tx.buf,BSP_ESP8266_CONNECT_2);
                    BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
#else
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART3;
                    BspEsp8266_s_Tx.Flag_state   =   0;
#endif
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART3)
                {
#ifdef  BSP_ESP8266_CONNECT_3
                    strcpy(BspEsp8266_s_Tx.buf,BSP_ESP8266_CONNECT_3);
                    BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
#else
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART4;
                    BspEsp8266_s_Tx.Flag_state   =   0;
#endif
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART4)
                {
#ifdef  BSP_ESP8266_CONNECT_4

                    strcpy(BspEsp8266_s_Tx.buf,BSP_ESP8266_CONNECT_4);
                    BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
#else
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART5;
                    BspEsp8266_s_Tx.Flag_state   =   0;
#endif
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART5)
                {
#ifdef  BSP_ESP8266_CONNECT_5
                    strcpy(BspEsp8266_s_Tx.buf,BSP_ESP8266_CONNECT_5);
                    BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
#else
                    BspEsp8266_s_Tx.AtCmd   =   IDLE;
                    BspEsp8266_s_Tx.Flag_state   =   0;
#endif
                }
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                p=strstr((const char *)BspEsp8266_s_Rx.buf,",CONNECT");
                if(p)
                {
                    p--;
                    i=*p;
                    i=i-'0';
                    if(i<5)
                    {
                        BspEsp8266_s_ConnectBuf[i].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                //
                p=strstr((const char *)BspEsp8266_s_Rx.buf,"ALREADY CONNECTED");
                if(p)
                {
                    if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART1)
                    {
                        BspEsp8266_s_ConnectBuf[0].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
                    else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART2)
                    {
                        BspEsp8266_s_ConnectBuf[1].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
                    else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART3)
                    {
                        BspEsp8266_s_ConnectBuf[2].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
                    else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART4)
                    {
                        BspEsp8266_s_ConnectBuf[3].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
                    else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART5)
                    {
                        BspEsp8266_s_ConnectBuf[4].state    =   BSP_ESP8266_E_CONNECT_STATE_CONNECTED;
                    }
					BspEsp8266_s_Tx.Flag_state   =   0;
                }
                //
                if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART1)
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART2;
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART2)
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART3;
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART3)
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART4;
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART4)
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART5;
                }
                else if(BspEsp8266_s_Tx.AtCmd==AT_CIPSTART5)
                {
                    BspEsp8266_s_Tx.AtCmd   =   IDLE;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=50)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CIPSTART1;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        //发送数据
        case AT_CIPSEND:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
            	// 不能发送
                BspEsp8266_s_Tx.Flag_state   =   2;
                BspEsp8266_s_Tx.cmt     =   0;
				sprintf((char*)(&BspEsp8266_s_Tx.buf[225]),"AT+CIPSEND=%d,%d\r\n\0",BspEsp8266_s_Tx.ch,BspEsp8266_s_Tx.len);
				BspEsp8266_SendAT(&BspEsp8266_s_Tx.buf[225],strlen(&BspEsp8266_s_Tx.buf[225]));
				break;
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,">"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CIPSEND1;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CIPSEND;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        //发送数据
        case AT_CIPSEND1:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"SEND OK"))
                {
                    BspEsp8266_s_Tx.Flag_state  =   0;
                    BspEsp8266_s_Tx.AtCmd   	=   IDLE;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                	BspEsp8266_s_Tx.Flag_state  =   0;
                    BspEsp8266_s_Tx.AtCmd   	=   AT_CIPSEND;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=50)
            {
                BspEsp8266_s_Tx.AtCmd   		=   AT_CIPSEND1;
                BspEsp8266_s_Tx.Flag_state   	=   0;
            }
            break;
        //设置上电自动连接
        /*
        case AT_CWAUTOCONN:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CWAUTOCONN=1\r\n";
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"OK"))
                {
                    BspEsp8266_s_Tx.state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_CWSTARTSMART;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CWAUTOCONN;
                BspEsp8266_s_Tx.state   =   0;
            }
            break;
        */
        //设置智能连接
        /*
        case AT_CWSTARTSMART:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CWSTARTSMART=3\r\n";
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"OK"))
                {
                    BspEsp8266_s_Tx.state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT_WAITFORCONFIG;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.pbuf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_CWSTARTSMART;
                BspEsp8266_s_Tx.state   =   0;
            }
            break;
        */
        //等待app配置

        //case AT_WAITFORCONFIG:
        //break;
//      AT_CIPMODE,         //设置透传模式
        //5s查询当前连接状态

        //向服务器发送数据
        //case AT_SEND_SERVER_DATA:
        //break;
        //case AT_WAIT_FOR_RESPOND:
        //break;
//      AT_GMR,             //查询版本信息

        //case AT_SEND:
        //  break;
        //case AT_RECEIVE:
        //  break;
        //重启
        case AT_RST:
            if(BspEsp8266_s_Tx.Flag_state    ==  0)
            {
                BspEsp8266_s_Tx.Flag_state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                strcpy(BspEsp8266_s_Tx.buf,"AT+RST\r\n");
                BspEsp8266_s_Tx.len=strlen(BspEsp8266_s_Tx.buf);
            }
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr((const char *)BspEsp8266_s_Rx.buf,"OK"))
                {
                    BspEsp8266_s_Tx.Flag_state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                }
                // ERR
                else if(strstr((const char *)BspEsp8266_s_Rx.buf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.Flag_state   =   0;
                }
                BspEsp8266_s_Rx.len =   0;
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT_RST;
                BspEsp8266_s_Tx.Flag_state   =   0;
            }
            break;
        default:
            break;
    }

}
static void BspEsp8266_Test_100ms(void)
{
    static uint8_t scmt=0xFF;
    if(scmt==0xFF)
    {
        scmt=0;
    }
    scmt++;
    if(scmt>=10*5)
    {
        scmt=0;
        BspEsp8266_Tx(BSP_ESP8266_E_CH_1,"XSL-WIFI-TEST-SEND\r\n",strlen("XSL-WIFI-TEST-SEND\r\n"));
    }
}
static void BspEsp8266_TestParse(uint8_t *pbuf,uint16_t len)
{
    if(BspEsp8266_DebugTest_Enable==1)
    {
        DebugOutStr("\r\nXSL-BspEsp8266-TestParse:");
        DebugOut((int8_t*)pbuf,len);
        DebugOutStr("\r\n");
    }
}
void BspEsp8266_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspEsp8266_DebugTest_Enable=1;
    }
    else
    {
        BspEsp8266_DebugTest_Enable=0;
    }
}
void BspEsp8266_DebugControlOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspEsp8266_DebugControl_Enable=1;
    }
    else
    {
        BspEsp8266_DebugControl_Enable=0;
    }
}
void BspEsp8266_DebugRx(uint8_t *d,uint16_t len)
{
    if(BspEsp8266_DebugControl_Enable==0)
        return;
    UART_DMA_Tx(BSP_ESP8266_UARTX,d,len);
}

#if 0

#define AT_CR       '\r'
#define AT_LF       '\n'



char g_EspBuf[2048];    /* 用于解码 */


/*
*********************************************************************************************************
*   函 数 名: ESP8266_PrintRxData
*   功能说明: 打印STM32从ESP8266收到的数据到COM1串口，主要用于跟踪调试
*   形    参: _ch : 收到的数据
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PrintRxData(uint8_t _ch)
{
#ifdef ESP8266_TO_COM1_EN
    comSendChar(COM1, _ch);     /* 将接收到数据打印到调试串口1 */
#endif
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_PowerOn
*   功能说明: 给ESP8266模块上电
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PowerOn(void)
{
    /* WIFI模块上电时，会以74880波特率打印如下信息:
         ets Jan  8 2013,rst cause:1, boot mode:(3,6)

        load 0x40100000, len 25052, room 16
        tail 12
        chksum 0x0b
        ho 0 tail 12 room 4
        load 0x3ffe8000, len 3312, room 12
        tail 4
        chksum 0x53
        load 0x3ffe8cf0, len 6576, room 4
        tail 12
        chksum 0x0d
        csum 0x0d       <-----  程序识别 csum 后，再自动切换到正常波特率
    */

    ESP_CH_PD_0();

    bsp_InitUart6(74880, 0);    /* 1表示硬件流控CRS RTS有效;  0表示无需硬件流控 */

    ESP_CH_PD_1();

    ESP8266_Reset();

    /* 等待模块完成上电，超时500ms 自动退出 */
    ESP8266_WaitResponse("csum", 1000); /* 先等待 csum */
    ESP8266_WaitResponse("\n", 1000);   /* 再等待回车换行字符结束 */

    bsp_InitUart6(115200, 0);   /* 1表示硬件流控CRS RTS有效;  0表示无需硬件流控 */

    /* 等待模块完成上电，判断是否接收到 ready */
    ESP8266_WaitResponse("ready", 5000);

    // ESP8266_SendAT("AT+RST");
    // ESP8266_WaitResponse("ready", 5000);
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_PowerOff
*   功能说明: 控制ESP8266模块关机
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_PowerOff(void)
{
    ESP_CH_PD_0();
}



/*
*********************************************************************************************************
*   函 数 名: ESP8266_EnterISP
*   功能说明: 控制ESP8266模块进入固件升级模式
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_EnterISP(void)
{
    ESP_CH_PD_0();
    ESP_GPIO0_0()  /* 0 表示进入固件升级模式 */
    ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_ExitISP
*   功能说明: 控制ESP8266模块退出固件升级模式
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_ExitISP(void)
{
    ESP_CH_PD_0();
    ESP_GPIO0_1()  /* 1 表示进入用户程序（AT指令）模式 */
    ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_WaitResponse
*   功能说明: 等待ESP8266返回指定的应答字符串, 可以包含任意字符。只要接收齐全即可返回。
*   形    参: _pAckStr : 应答的字符串， 长度不得超过255
*            _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*   返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP8266_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
    uint8_t ucData;
    uint16_t pos = 0;
    uint32_t len;
    uint8_t ret;

    len = strlen(_pAckStr);
    if (len > 255)
    {
        return 0;
    }

    /* _usTimeOut == 0 表示无限等待 */
    if (_usTimeOut > 0)
    {
        Count_SoftTimer_Start(ESP8266_TMR_ID, _usTimeOut);      /* 使用软件定时器3，作为超时控制 */
    }
    while (1)
    {
        bsp_Idle();             /* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

        if (_usTimeOut > 0)
        {
            if (Count_SoftTimer_Check(ESP8266_TMR_ID))
            {
                ret = 0;    /* 超时 */
                break;
            }
        }

        if (comGetChar(COM_ESP8266, &ucData))
        {
            ESP8266_PrintRxData(ucData);        /* 将接收到数据打印到调试串口1 */

            if (ucData == _pAckStr[pos])
            {
                pos++;

                if (pos == len)
                {
                    ret = 1;    /* 收到指定的应答数据，返回成功 */
                    break;
                }
            }
            else
            {
                pos = 0;
            }
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_ReadLine
*   功能说明: 读取ESP8266返回的一行应答字符串(0x0D 0x0A结束)。该函数根据字符间超时判断结束。 本函数需要紧跟AT命令发送函数。
*   形    参: _pBuf : 存放模块返回的完整字符串
*             _usBufSize : 缓冲区最大长度
*            _usTimeOut : 命令执行超时，0表示一直等待. >0 表示超时时间，单位1ms
*   返 回 值: 0 表示错误（超时）  > 0 表示应答的数据长度
*********************************************************************************************************
*/
uint16_t ESP8266_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
    uint8_t ucData;
    uint16_t pos = 0;
    uint8_t ret;

    /* _usTimeOut == 0 表示无限等待 */
    if (_usTimeOut > 0)
    {
        Count_SoftTimer_Start(ESP8266_TMR_ID, _usTimeOut);      /* 使用软件定时器作为超时控制 */
    }
    while (1)
    {
        bsp_Idle();             /* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

        if (Count_SoftTimer_Check(ESP8266_TMR_ID))
        {
            _pBuf[pos] = 0; /* 结尾加0， 便于函数调用者识别字符串结束 */
            ret = pos;      /* 成功。 返回数据长度 */
            break;
        }

        if (comGetChar(COM_ESP8266, &ucData))
        {
            ESP8266_PrintRxData(ucData);        /* 将接收到数据打印到调试串口1 */

            Count_SoftTimer_Start(ESP8266_TMR_ID, 500);
            _pBuf[pos++] = ucData;      /* 保存接收到的数据 */
            if (ucData == 0x0A)
            {
                _pBuf[pos] = 0;
                ret = pos;      /* 成功。 返回数据长度 */
                break;
            }
        }
    }
    return ret;
}




/*
*********************************************************************************************************
*   函 数 名: ESP8266_SendData
*   功能说明: 发送TCP或UDP数据包
*   形    参: _databuf 数据
*            _len 数据长度
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_SendData(uint8_t *_databuf, uint16_t _len)
{
    char buf[32];

    if (_len > 2048)
    {
        _len = 2048;
    }

    sprintf(buf, "AT+CIPSEND=0,%d\r\n", _len);
    comSendBuf(COM_ESP8266, (uint8_t *)buf, strlen(buf));

    ESP8266_WaitResponse(">", 1000);

    comSendBuf(COM_ESP8266, _databuf, _len);
    ESP8266_WaitResponse("SEND OK", 8000);
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_JoinAP
*   功能说明: 加入AP
*   形    参: _ssid : AP名字字符串
*             _pwd : 密码字符串
*   返 回 值: 无
*********************************************************************************************************
*/
void ESP8266_JoinAP(char *_ssid, char *_pwd)
{
    char buf[128];

    sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"", _ssid, _pwd);
    ESP8266_SendAT(buf);
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_ScanAP
*   功能说明: 扫描AP。结果存放在_pList 结构体数组. 此函数会占用最长5秒时间。直到收到OK或ERROR。
*   形    参: _pList : AP列表数组;
*             _MaxNum : 搜索的最大AP个数。主要是防止缓冲区溢出。
*   返 回 值: -1 表示失败; 0 表示搜索到0个; 1表示1个。
*********************************************************************************************************
*/
int16_t ESP8266_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum)
{
    uint16_t i;
    uint16_t count;
    char buf[128];
    WIFI_AP_T *p;
    char *p1, *p2;

    buf[127] = 0;
    ESP8266_SendAT("AT+CWLAP");

    p = (WIFI_AP_T *)_pList;
    count = 0;
    for (i = 0; i < _MaxNum; i++)
    {
        ESP8266_ReadLine(buf, 128, 3000);
        if (memcmp(buf, "OK", 2) == 0)
        {
            break;
        }
        else if (memcmp(buf, "ERROR", 5) == 0)
        {
            break;
        }
        else if (memcmp(buf, "+CWLAP:", 7) == 0)
        {
            p1 = buf;

            /* +CWLAP:(4,"BaiTu",-87,"9c:21:6a:3c:89:52",1) */
            /* 解析加密方式 */
            p1 = strchr(p1, '(');   /* 搜索到(*/
            p1++;
            p->ecn = str_to_int(p1);

            /* 解析ssid */
            p1 = strchr(p1, '"');   /* 搜索到第1个分号 */
            p1++;
            p2 = strchr(p1, '"');   /* 搜索到第2个分号 */
            memcpy(p->ssid, p1, p2 - p1);
            p->ssid[p2 - p1] = 0;

            /* 解析 rssi */
            p1 = strchr(p2, ',');   /* 搜索到逗号*/
            p1++;
            p->rssi = str_to_int(p1);

            /* 解析mac */
            p1 = strchr(p1, '"');   /* 搜索到分号*/
            p1++;
            p2 = strchr(p1, '"');   /* 搜索到分号*/
            memcpy(p->mac, p1, p2 - p1);
            p->mac[p2 - p1] = 0;

            /* 解析ch */
            p1 = strchr(p2, ',');   /* 搜索到逗号*/
            p1++;
            p->ch = str_to_int(p1);

            /* 有效的AP名字 */
            count++;

            p++;
        }
    }

    return count;
}

/*
*********************************************************************************************************
*   函 数 名: ESP8266_RxNew
*   功能说明: 接收新数据帧
*   形    参: _pRxBuf : 接收到的数据存放在此缓冲区
*   返 回 值: 接收到的数据长度. 0 表示无数据
*********************************************************************************************************
*/
uint16_t ESP8266_RxNew(uint8_t *_pRxBuf)
{
    uint8_t ucData;
    static uint8_t s_buf[512];  /* 约定最长256 */
    static uint16_t s_len = 0;

    if (comGetChar(COM_ESP8266, &ucData))
    {
        ESP8266_PrintRxData(ucData);        /* 将接收到数据打印到调试串口1 */

        if ((ucData == 0x0D) || (ucData == 0x0A))
        {
            /* +IPD,0,7:ledon 1 */
            char *p1;
            uint16_t len;

            if ((s_len > 9) && (memcmp(s_buf, "+IPD,", 5) == 0))
            {
                p1 = (char *)&s_buf[7];
                len = str_to_int(p1);
                p1 = strchr(p1, ':');   /* 搜索到逗号*/
                p1++;
                memcpy(_pRxBuf, p1,  len);
                s_len = 0;
                return len;
            }
            s_len = 0;
        }
        else
        {
            if (ucData == '+')
            {
                s_len = 0;
            }
            if (s_len < sizeof(s_buf))
            {
                s_buf[s_len++] = ucData;        /* 保存接收到的数据 */
            }
        }
    }
    return 0;
}
#endif
/************************ (C) COPYRIGHT XSLXHN *****END OF FILE****************/

