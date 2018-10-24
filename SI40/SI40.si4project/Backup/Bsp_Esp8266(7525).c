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
/* Private typedef -----------------------------------------------------------*/
enum BSP_ESP8266_E_AT
{
    Idle=0,
    AT,                     //测试启动
    ATE,                    //开关回显
    AT_UART,                //设置UART
    CIPSTATUS,              //查询网络连接状态

    AT_CWMODE,              //设置工作模式
    AT_CWAUTOCONN,          //使能上电自动连接
    AT_CWSTARTSMART,        //使能自动配置网络

    AT_WAITFORCONFIG,       //等待app配置完成

    AT_CIPSTART,            //设置udp连接
//      AT_CIPMODE,         //设置透传模式
    AT_CIPSEND,             //申请发送数据
    AT_SEND_SERVER_DATA,    //向服务器发送数据
    AT_WAIT_FOR_RESPOND,    //等待服务器

//      AT_RST,             //重启
//      AT_GMR,             //查询版本信息

    AT_SEND,                //发送数据
    AT_RECEIVE,             //接收数据
    AT_RST
};
typedef struct BSP_ESP8266_S_TX
{
    uint8_t     *pbuf;
    uint8_t     len;
    uint8_t     AtCmd;
    uint8_t     state;  // 0-初始值  1-装载完毕  2-发送完毕  3-接收成功  4-接收失败
    uint8_t     cmt;
} BSP_ESP8266_S_TX;
typedef struct BSP_ESP8266_S_RX
{
    uint8_t     len;    //  0-结构可用  1-结构不可用
    uint8_t     pbuf[255];
} BSP_ESP8266_S_RX;

/* Private define ------------------------------------------------------------*/
static BSP_ESP8266_S_TX BspEsp8266_s_Tx;
//
#define BSP_ESP8266_RX_BUF_MAX  256
static BSP_ESP8266_S_RX BspEsp8266_s_Rx;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
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
    BSP_ESP8266_RST_H;
    UART_INIT(BSP_ESP8266_UARTX,115200);
    //
    BspEsp8266_s_Tx.AtCmd   =   Idle;
    BspEsp8266_s_Tx.cmt     =   0;
    BspEsp8266_s_Rx.len     =   0;
    memset(BspEsp8266_s_Rx.pbuf,0,sizeof(BspEsp8266_s_Rx.pbuf));
}

/**
 * @brief   中断调用
 * @note    只周转数据,不处理数据
 * @param   None
 * @return  None
 */
void BspEsp8266_InterruptRx(uint8_t *pbuf,uint16_t len)
{
    if(BspEsp8266_s_Rx.len==0 && len<=250)
    {
        BspEsp8266_s_Rx.len=len;
        memcpy(BspEsp8266_s_Rx.pbuf,pbuf,len);
    }
}

/**
 * @brief   给ESP8266模块复位
 * @note    通过控制RST引脚,操作模块复位
 * @param   None
 * @return  None
 */
void BspEsp8266_Reset(void)
{
    BSP_ESP8266_RST_L();
    MODULE_OS_DELAY_MS(20);
    BSP_ESP8266_RST_H();
    MODULE_OS_DELAY_MS(10);
}

/**
 * @brief   向模块发送AT命令。
 * @note    本函数自动在AT字符串口增加<CR>字符
 * @param   *pstr   -   AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
 * @return  None
 */

void BspEsp8266_SendAT(char *pstr)
{
    UART_DMA_Tx(BSP_ESP8266_UARTX, (uint8_t *)pstr, strlen(pstr));
    //UART_DMA_Tx(BSP_ESP8266_UARTX, "\r\n", 2);
}

void BspEsp8266_100ms(void)
{
    //static uint8_t n, Ms100_cnt,Ms100_cnt2 = 0;
    //static char AT_Send_Fifo[256] = {0};
    //----------计时
    if(BspEsp8266_s_Tx.cmt!=0xFF)
    {
        BspEsp8266_s_Tx.cmt++;
    }
    //----------接收数据
    if(BspEsp8266_s_Rx.len!=0)
    {
        //已通过手机设置连接wifi
        if(strstr(BspEsp8266_s_Rx.pbuf,"smartconfig connected wifi"))
        {
        }
        //已连接服务器
        else if(strstr(BspEsp8266_s_Rx.pbuf,"CONNECT"))
        {
        }
        //申请发送数据成功
        else if(strstr(BspEsp8266_s_Rx.pbuf,">")&&BspEsp8266_s_Rx.len==3)
        {}
        //数据发送成功
        else if(strstr(BspEsp8266_s_Rx.pbuf,"SEND OK"))
        {}
        //接收到数据
        else if(strstr(BspEsp8266_s_Rx.pbuf,"+IPD"))
        {}
        //错误
        else if(strstr(BspEsp8266_s_Rx.pbuf,"ERROR"))
        {}
        //正确
        else if(strstr(BspEsp8266_s_Rx.pbuf,"OK"))
        {}
    }
    //----------发送
    if(BspEsp8266_s_Tx.state==1)
    {
        BspEsp8266_SendAT(BspEsp8266_s_Tx.pbuf);
        BspEsp8266_s_Tx.state=2;
    }
    //----------发送处理
    switch(BspEsp8266_s_Tx.AtCmd)
    {
        // 初始等待
        case Idle:
            // 上电等待5s
            if(BspEsp8266_s_Tx.cmt>=50)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT;
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.cmt     =   0;
            }
            break;
        // 测试模块
        case AT:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT\r\n";
            }
            // 处理接收数据
            if(BspEsp8266_s_Rx.len!=0)
            {
                // OK
                if(strstr(BspEsp8266_s_Rx.pbuf,"OK"))
                {
                    BspEsp8266_s_Tx.state   =   0;
                    BspEsp8266_s_Tx.AtCmd   =   ATE;
                }
                // ERR
                else if(strstr(BspEsp8266_s_Rx.pbuf,"ERROR"))
                {
                    BspEsp8266_s_Tx.AtCmd   =   AT;
                    BspEsp8266_s_Tx.state   =   0;
                }
            }
            // 处理超时
            if(BspEsp8266_s_Tx.cmt>=20)
            {
                BspEsp8266_s_Tx.AtCmd   =   AT;
                BspEsp8266_s_Tx.state   =   0;
            }
            break;
        //关闭回显
        case ATE:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "ATE0\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   AT_UART;
            }
            break;
        //设置串口波特率
        case AT_UART:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+UART_DEF=115200,8,1,0,0\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   CIPSTATUS;
            }
            break;
        //查询网络连接状态
        case CIPSTATUS:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CIPSTATUS\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   AT_CWMODE;
            }
            break;
        //配置wifi工作模式
        case AT_CWMODE:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CWMODE_DEF=1\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   AT_CWAUTOCONN;
            }
            break;
        //设置上电自动连接
        case AT_CWAUTOCONN:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+CWSTARTSMART=3\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   AT_CWSTARTSMART;
            }
            break;
        //设置智能连接
        case AT_CWSTARTSMART:
            break;
        //等待app配置
        case AT_WAITFORCONFIG:
            break;
        //连接UDP服务器
        case AT_CIPSTART:
            break;
//      AT_CIPMODE,         //设置透传模式
        //5s查询当前连接状态
        case AT_CIPSEND:
            break;
        //向服务器发送数据
        case AT_SEND_SERVER_DATA:
            break;
        case AT_WAIT_FOR_RESPOND:
            break;
//      AT_GMR,             //查询版本信息

        case AT_SEND:
            break;
        case AT_RECEIVE:
            break;
        //重启
        case AT_RST:
            if(BspEsp8266_s_Tx.state    ==  0)
            {
                BspEsp8266_s_Tx.state   =   1;
                BspEsp8266_s_Tx.cmt     =   0;
                BspEsp8266_s_Tx.pbuf    =   "AT+RST\r\n";
            }
            else if(BspEsp8266_s_Tx.state   ==  2)
            {
                BspEsp8266_s_Tx.state   =   0;
                BspEsp8266_s_Tx.AtCmd   =   AT_CWSTARTSMART;
            }
            break;
        default:
            break;
    }

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

