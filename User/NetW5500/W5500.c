//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 5.10                    //
//                    编 译 器: RealView MDK-ARM Version 5.10                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: W5500.c                                          //
//                    作    者: 徐松亮                                           //
//-------------------------------------------------------------------------------//
/**********************************************************************************
 * 一,包括5个部分：
 *    1. W5500初始化
 *    2. W5500的Socket初始化
 *    3. Socket连接
 *         如果Socket设置为TCP服务器模式，则调用Socket_Listen()函数,W5500处于侦听状态，直到远程客户端与它连接。
 *         如果Socket设置为TCP客户端模式，则调用Socket_Connect()函数，
 *                                        每调用一次Socket_Connect(s)函数，产生一次连接，
 *                                        如果连接不成功，则产生超时中断，然后可以再调用该函数进行连接。
 *         如果Socket设置为UDP模式,则调用Socket_UDP函数
 *    4. Socket数据接收和发送
 *    5. W5500中断处理
 * 二,3种模式的流程
 *    置W5500为服务器模式的调用过程：
 *       W5500_Init()-->Socket_Init(s)-->Socket_Listen(s)，设置过程即完成，等待客户端的连接。
 *    置W5500为客户端模式的调用过程：
 *       W5500_Init()-->Socket_Init(s)-->Socket_Connect(s)，设置过程即完成，并与远程服务器连接。
 *    置W5500为UDP模式的调用过程：
 *       W5500_Init()-->Socket_Init(s)-->Socket_UDP(s)，设置过程即完成，可以与远程主机UDP通信。
 *    W5500产生的连接成功、终止连接、接收数据、发送数据、超时等事件，都可以从中断状态中获得。
**********************************************************************************/
#include <includes.h>
#ifndef STM32
    #include "user.h"
#else
    #include "stm32f10x.h"
    #include "stm32f10x_spi.h"
    #include "W5500.h"
#endif

#ifdef __W5500_H
#ifdef __W5500APP_H
//-------------------------------------------------宏定义
//最大通道数
#define W5500_CH_MAX 8
//基地址
#define COMMON_BASE           0x0000
//基本寄存器
#define W5500_MODE            COMMON_BASE          //Mode register,R/W,Default=0x00(模式寄存器,可读可写,缺省值0x00,该寄存器用于软件复位、Ping关闭模式、PPPoE模式以及间接总线接口)
#define W5500_MODE_RST        0x80  //软件复位,置1有效,复位后自动清0,芯片内部寄存器将被初始化为缺省值
#define W5500_MODE_WOL        0x20  //网络唤醒使能
#define W5500_MODE_PB         0x10  //Ping block使能(Ping阻止模式,0关闭Ping阻止,1启动Ping阻止)
#define W5500_MODE_PPPOE      0x08  //PPPOE 使能(0:关闭 1:打开如果不经过路由器直接连接到ADSL,该位必须置“1”以便与ADSL服务器连接。欲知详情,请参照“How to connect ADSL”应用笔记)
#define W5500_MODE_FARP       0x02  //强迫ARP模式
#define W5500_GAR             COMMON_BASE+0x01     //网关IP  ,    R/W, 缺省值0x00, 例:192.168.1.1
#define W5500_SUBR            COMMON_BASE+0x05     //子网掩码,    R/W, 缺省值0x00, 例:255.255.255.0
#define W5500_SHAR            COMMON_BASE+0x09     //源MAC地址,   R/W, 缺省值0x00, 例:00.08.DC.01.02.03->0x00,0x08,0xdc,0x01,0x02,0x03
#define W5500_SIPR            COMMON_BASE+0x0f     //源IP    ,    R/W, 缺省值0x00, 例:192.168.0.3->0xc0,0xa8,0x00,0x03
#define W5500_INTLEVEL        COMMON_BASE+0x13     //低电平中断定时器寄存器
#define W5500_IR              COMMON_BASE+0x15     //中断寄存器,R/W, 缺省值0x00, 
#define W5500_IR_CONFLICT     0x80  //IP地址冲突:当一个与本机IP地址相同IP地址作ARP请求时,该位被置“1”,对该位写”1”可清0)
#define W5500_IR_UNREACH      0x40  //目标不可抵达:(在UDP数据包发送过程中,如果目的IP地址不存在时,W5500将会收到一ICMP(目的无法到达)数据包,(参照5.2.2.UDP ),在该状态下,IP地址及端口号将被存到UIPR和UPORT 寄存器,同时该位置“1”,对该位写”1”可清0 )
#define W5500_IR_PPPOE        0x20  //PPPOE关闭 :(在PPPoE模式,如果PPPoE连接被关闭,该位置“1”,对该位写”1”可清0)
#define W5500_IR_MP           0x10  //收到网络唤醒包(UDP)
#define W5500_IMR             COMMON_BASE+0x16     //中断屏蔽,    R/W, 缺省值0x00
#define W5500_IMR_CONFLICT    0x80  //IP地址冲突
#define W5500_IMR_UNREACH     0x40  //目标不可抵达
#define W5500_IMR_PPPOE       0x20  //PPPOE关闭
#define W5500_IMR_MP          0x10  //网络唤醒包(UDP)
#define W5500_SIR             COMMON_BASE+0x17     //Socket中断寄存器
#define W5500_SIR_S7          (1<<7)
#define W5500_SIR_S6          (1<<6)
#define W5500_SIR_S5          (1<<5)
#define W5500_SIR_S4          (1<<4)
#define W5500_SIR_S3          (1<<3)
#define W5500_SIR_S2          (1<<2)
#define W5500_SIR_S1          (1<<1)
#define W5500_SIR_S0          (1<<0)
#define W5500_SIMR            COMMON_BASE+0x18     //Socket中断屏蔽寄存器
#define W5500_SIMR_S7         (1<<7)
#define W5500_SIMR_S6         (1<<6)
#define W5500_SIMR_S5         (1<<5)
#define W5500_SIMR_S4         (1<<4)
#define W5500_SIMR_S3         (1<<3)
#define W5500_SIMR_S2         (1<<2)
#define W5500_SIMR_S1         (1<<1)
#define W5500_SIMR_S0         (1<<0)
#define W5500_RTR             COMMON_BASE+0X19     //重试时间寄存器
#define W5500_RCR             COMMON_BASE+0X1b     //重试次数寄存器
#define W5500_PTIMER          COMMON_BASE+0X1c     //PPP连接控制协议请求定时寄存器
#define W5500_PMAGIC          COMMON_BASE+0X1d     //PPP连接控制协议幻数寄存器
#define W5500_PHAR            COMMON_BASE+0x1E     //PPPoE模式下目标MAC寄存器
#define W5500_PSID            COMMON_BASE+0x24     //PPPoE模式下会话ID寄存器
#define W5500_PMRU            COMMON_BASE+0x26     //PPPoE模式下最大接收单元
#define W5500_UIPR            COMMON_BASE+0x28     //无法抵达IP地址寄存器
#define W5500_UPORTR          COMMON_BASE+0x2C     //无法抵达PORT寄存器
#define W5500_PHYCFGR         COMMON_BASE+0x2E     //W5500 PHY 配置寄存器
#define W5500_PHYCFGR_RST_PHY (1<<7)   //重启内部PHY
#define W5500_PHYCFGR_OPMODE  (1<<6)   //配置PHY工作模式
#define W5500_PHYCFGR_DPX     (1<<2)   //双工工作状态 (只读)
#define W5500_PHYCFGR_SPD     (1<<1)   //速度状态     (只读)
#define W5500_PHYCFGR_LNK     (1<<0)   //连接状态     (只读)
#define W5500_VER             COMMON_BASE+0x39     //W5500 芯片版本寄存器
//SOCKET寄存器
#define W5500_Sn_MR           COMMON_BASE+0x0000   //Socket 模式寄存器
#define W5500_Sn_MR_MULTI_MFEN   (1<<7)
#define W5500_Sn_MR_BCASTB       (1<<6)
#define W5500_Sn_MR_ND_MC_MMB    (1<<5)
#define W5500_Sn_MR_UCASTB_MIP6B (1<<4)
#define W5500_Sn_MR_CLOSE     0x00
#define W5500_Sn_MR_TCP       0x01
#define W5500_Sn_MR_UDP       0x02
#define W5500_Sn_MR_MACRAW    0x04
#define W5500_Sn_CR           COMMON_BASE+0x0001   //Socket 配置寄存器
#define W5500_Sn_CR_OPEN      0x01
#define W5500_Sn_CR_LISTEN    0x02
#define W5500_Sn_CR_CONNECT   0x04
#define W5500_Sn_CR_DISCON    0x08
#define W5500_Sn_CR_CLOSE     0x10
#define W5500_Sn_CR_SEND      0x20
#define W5500_Sn_CR_SEND_MAC  0x21
#define W5500_Sn_CR_SEND_KEEP 0x22
#define W5500_Sn_CR_RECV      0x40
#define W5500_Sn_IR           COMMON_BASE+0x0002   //Socket 中断寄存器(只读)
#define W5500_Sn_IR_SENDOK    0x10
#define W5500_Sn_IR_TIMEOUT   0x08
#define W5500_Sn_IR_RECV      0x04
#define W5500_Sn_IR_DISCON    0x02
#define W5500_Sn_IR_CON       0x01
#define W5500_Sn_SR           COMMON_BASE+0x0003   //Socket 状态寄存器(只读)
#define W5500_Sn_SR_CLOSED    0x00
#define W5500_Sn_SR_INIT      0x13
#define W5500_Sn_SR_LISTEN    0x14
#define W5500_Sn_SR_ESTABLISHED  0x17
#define W5500_Sn_SR_CLOSE_WAIT   0x1c
#define W5500_Sn_SR_UDP       0x22
#define W5500_Sn_SR_MACRAW    0x02
#define W5500_Sn_SR_SYNSEND   0x15
#define W5500_Sn_SR_SYNRECV   0x16
#define W5500_Sn_SR_FIN_WAI   0x18
#define W5500_Sn_SR_CLOSING   0x1a
#define W5500_Sn_SR_TIME_WAIT 0x1b
#define W5500_Sn_SR_LAST_ACK  0x1d
#define W5500_Sn_PORT         COMMON_BASE+0x0004   //Socket 源端口寄存器
#define W5500_Sn_DHAR         COMMON_BASE+0x0006   //目的MAC
#define W5500_Sn_DIPR         COMMON_BASE+0x000c   //目的IP
#define W5500_Sn_DPORTR       COMMON_BASE+0x0010   //目的PORT
#define W5500_Sn_MSSR         COMMON_BASE+0x0012   //Socket n-th 最大分段寄存器
#define W5500_Sn_TOS          COMMON_BASE+0x0015   //Socket IP 服务类型寄存器
#define W5500_Sn_TTL          COMMON_BASE+0x0016   //Socket IP 生存时间寄存器
#define W5500_Sn_RXBUF_SIZE   COMMON_BASE+0x001e   //接收缓存大小寄存器
#define W5500_Sn_TXBUF_SIZE   COMMON_BASE+0x001f   //发送缓存大小寄存器
#define W5500_Sn_TX_FSR       COMMON_BASE+0x0020   //空闲发送缓存寄存器
#define W5500_Sn_TX_RD        COMMON_BASE+0x0022   //发送读指针寄存器
#define W5500_Sn_TX_WR        COMMON_BASE+0x0024   //发送写指针寄存器
#define W5500_Sn_RX_RSR       COMMON_BASE+0x0026   //空闲接收缓存寄存器
#define W5500_Sn_RX_RD        COMMON_BASE+0x0028   //接收读指针寄存器
#define W5500_Sn_RX_WR        COMMON_BASE+0x002a   //接收写指针寄存器
#define W5500_Sn_IMR          COMMON_BASE+0x002c   //中断屏蔽寄存器
#define W5500_Sn_IMR_SENDOK   0x10
#define W5500_Sn_IMR_TIMEOUT  0x08
#define W5500_Sn_IMR_RECV     0x04
#define W5500_Sn_IMR_DISCON   0x02
#define W5500_Sn_IMR_CON      0x01
#define W5500_Sn_FRAG         COMMON_BASE+0x002d   //Socket n 分段寄存器
#define W5500_Sn_KPALVTR      COMMON_BASE+0x002f   //Socket 在线时间寄存器
//操作模式位
#define W5500_VDM       0x00
#define W5500_FDM1  0x01
#define W5500_FDM2  0x02
#define W5500_FDM4  0x03
//读写控制位
#define W5500_RWB_READ  0x00
#define W5500_RWB_WRITE 0x04
//块选择位
#define W5500_COMMON_R  0x00
//
#define W5500_S_RX_SIZE 2048    //定义Socket接收缓冲区的大小，可以根据W5500_RMSR的设置修改
#define W5500_S_TX_SIZE 2048    //定义Socket发送缓冲区的大小，可以根据W5500_TMSR的设置修改
//-------------------------------------------------变量
static uint8_t W5500_InterruptSign;
W5500_S_CONFIG W5500_S_Config;
W5500_S_CONNECT W5500_S_Connect[W5500_CH_MAX];
//-------------------------------------------------函数指针
void (*W5500APP_ProtocolAnalysis_ChX[8])(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)= {W5500_ProtocolAnalysis_Ch0,\
                                                                                            W5500_ProtocolAnalysis_Ch1,\
                                                                                            W5500_ProtocolAnalysis_Ch2,\
                                                                                            W5500_ProtocolAnalysis_Ch3,\
                                                                                            W5500_ProtocolAnalysis_Ch4,\
                                                                                            W5500_ProtocolAnalysis_Ch5,\
                                                                                            W5500_ProtocolAnalysis_Ch6,\
                                                                                            W5500_ProtocolAnalysis_Ch7,\
                                                                                           };
//-------------------------------------------------函数
//硬件相关函数-->按需修改
static void W5500_GPIO_Configuration(void);
static uint8_t W5500_SPI_SendByte(uint8_t dt);
//软件相关函数-->按需修改-->网络配置
static uint8_t W5500_LoadVar(void);
static void W5500_ProtocolAnalysis(uint8_t ch,uint8_t *pbuf,uint16_t len);
//硬件无关函数-->无需修改-->中间函数
static uint8_t W5500_ReadReg_1Byte(uint16_t addr);
static void W5500_WriteReg_1Byte(uint16_t addr,uint8_t dat);
#ifdef STM32
static void W5500_Initialization(void);
#else
void W5500_Initialization(void);
#endif

static uint8_t W5500_TCPClient_SocketConnect(uint8_t ch);
static uint8_t W5500_TCPServer_SocketListen(uint8_t ch);
static uint8_t W5500_UDP_Socket(uint8_t ch);
static uint16_t W5500_App_Rx(uint8_t ch,uint8_t *pbuf,uint16_t maxlen);
static void W5500_Interrupt_Process(void);
static uint8_t W5500_Socket_Monitor(void);
//对外接口函数-->无需修改
void W5500_Main_100ms(uint8_t sign_init);
void W5500_IRQHandler(void);
/*******************************************************************************
* 描述    : 延时函数(ms)
* 输入    : d:延时系数，单位为毫秒
* 输出    : 无
* 返回    : 无
*******************************************************************************/
#ifndef STM32
    void W5500_DelayMs(uint16_t d)
    {
        Delay_xmS(d);
    }
#else
    void W5500_DelayMs(uint16_t d)
    {
        OSTimeDlyHMSM(0, 0, d/1000, d%1000);
    }
#endif

/*******************************************************************************
* 描述    : W5500统初始化函
* 输入    : 无
* 输出    : 无
* 返回    : 无
*******************************************************************************/
static void W5500_GPIO_Configuration(void)
{
#ifndef STM32                     //PIC 初始化
    W5500_SCS = 0;                //片选使能
    Nop();Nop();Nop();
    W5500_SCS = 1;                //取消片选
    Nop();Nop();Nop();

    SPI2CON1  = 0;                //清零
    SPI2CON2  = 0;
    SPI2STAT  = 0;

    SPI2CON1bits.DISSDO= 0;       //使用 SDO
    SPI2CON1bits.DISSCK= 0;       //使用 SCK
    SPI2CON1bits.SSEN  = 0;       //禁止 SS

    //辅助分频(多选一)
    //SPI2CON1bits.SPRE  = 7;     //1:1分频   SCK = 16MHZ
    SPI2CON1bits.SPRE  = 6;       //2:1分频   SCK = 8MHZ
    //SPI2CON1bits.SPRE  = 5;     //3:1分频
    //SPI2CON1bits.SPRE  = 4;     //4:1分频
    //SPI2CON1bits.SPRE  = 3;     //5:1分频
    //SPI2CON1bits.SPRE  = 2;     //6:1分频
    //SPI2CON1bits.SPRE  = 1;     //7:1分频
    //SPI2CON1bits.SPRE  = 0;     //8:1分频

    //主分频(多选一)
    //SPI2CON1bits.PPRE  = 3;     //1:1分频
    SPI2CON1bits.PPRE  = 2;       //4:1分频
    //SPI2CON1bits.PPRE  = 1;     //16:1分频
    //SPI2CON1bits.PPRE  = 0;     //64:1分频

    SPI2CON1bits.MSTEN = 1;       //主控模式
    SPI2CON1bits.SMP   = 0;       //0 = 中间采样; 1 = 末端采样
    SPI2CON1bits.CKE   = 1;       //上升沿发送数据
    SPI2CON1bits.CKP   = 0;       //模式0 SCK时序 空闲低电平
    SPI2CON1bits.MODE16= 0;       //8位字节宽

    SPI2STATbits.SPIEN = 1;       //SPI使能
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 0;       //SPI使能
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 1;       //SPI使能
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 0;       //SPI使能
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 1;       //SPI使能
    Nop();Nop();Nop();Nop();

    //-----------W5500 电平中断---------------
    CNENCbits.CNIEC9 = 1;        // RC9中断
    _CNIP = 5;                   // 中断优先级
    IFS1bits.CNIF    = 0;        // 清除标志位
    IEC1bits.CNIE    = 1;        // 电平变化中断

#else
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //----------SPI初始化配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //初始化SCK、MISO、MOSI引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
    //初始化配置STM32 SPI3
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;    //SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master;                         //设置为主SPI
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;                     //SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;                            //时钟悬空低
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;                          //数据捕获于第1个时钟沿
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                             //NSS由外部管脚管理
    SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;    //波特率预分频值为8
    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;                    //数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial=7;                              //CRC多项式为7
    SPI_Init(SPI3, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPI1寄存器
    SPI_Cmd(SPI3, ENABLE);
    //----------GPIO初始化配置
    //
    GPIO_InitStructure.GPIO_Pin  = W5500_RST;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
    W5500_PIN_RST_H;
    //
    GPIO_InitStructure.GPIO_Pin = W5500_INT;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);

    //
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //
    GPIO_InitStructure.GPIO_Pin = W5500_SCS;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
    W5500_PIN_SCS_H;
    //----------中断优先级设置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}
/*******************************************************************************
* 描述    : STM32 SPI1发送一个字节数据，并返回接收到的数据
* 输入    : dt:待发送的数据
* 输出    : 无
* 返回值  : STM32 SPI1接收到的数据
* 说明    : 移植需要修改
*******************************************************************************/
static uint8_t W5500_SPI_SendByte(uint8_t dt)
{
#ifndef STM32                     //PIC发送 1 Byte

    SPI2BUF = dt;                 //发送数据
    while(!IFS2bits.SPI2IF)       //等待发送
    {
        Nop();
        ClrWdt();
    }
    IFS2bits.SPI2IF = 0;          //清除标志
    return (SPI2BUF);             //读取数据

#else
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, dt);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI3);
#endif

}
static void W5500_SPI_SendShort(uint16_t dt)
{
    W5500_SPI_SendByte(dt/256);
    W5500_SPI_SendByte(dt);
}
/*******************************************************************************
* 描述    : 向W5500指定地址寄存器读写数据
* 输入    : addr:(地址) dat:(待写入数据)
* 输出    : 无
* 返回值  : 无
* 说明    : 移植不需修改
*******************************************************************************/
static void W5500_WriteReg_1Byte(uint16_t addr, uint8_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_WRITE|W5500_COMMON_R);
    W5500_SPI_SendByte(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteReg_2Byte(uint16_t addr, uint16_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_WRITE|W5500_COMMON_R);
    W5500_SPI_SendShort(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteReg_nByte(uint16_t addr, uint8_t* pBuf ,uint16_t len)
{
    uint16_t i16;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|W5500_COMMON_R);
    for(i16=0; i16<len; i16++)
    {
        W5500_SPI_SendByte(pBuf[i16]);
    }
    W5500_PIN_SCS_H;
}
static uint8_t W5500_ReadReg_1Byte(uint16_t addr)
{
    uint8_t i;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_READ|W5500_COMMON_R);
    i=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i;
}
/*******************************************************************************
* 描述    : 向W5500指定地址SOCKET写数据
* 输入    : addr:(地址) dat:(待写入数据)
* 输出    : 无
* 返回值  : 无
* 说明    : 移植不需修改
*******************************************************************************/
static void W5500_WriteSocket_1Byte(uint8_t num,uint16_t addr, uint8_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //1个字节数据长度,写数据,选择通用寄存器
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_WRITE|(num*0x20+0x08));
    //写入数据
    W5500_SPI_SendByte(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteSocket_2Byte(uint8_t num,uint16_t addr, uint16_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //2个字节数据长度,写数据,选择通用寄存器
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_WRITE|(num*0x20+0x08));
    W5500_SPI_SendShort(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteSocket_4Byte(uint8_t num,uint16_t addr, uint8_t* pBuf)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //2个字节数据长度,写数据,选择通用寄存器
    W5500_SPI_SendByte(W5500_FDM4|W5500_RWB_WRITE|(num*0x20+0x08));
    W5500_SPI_SendByte(pBuf[0]);
    W5500_SPI_SendByte(pBuf[1]);
    W5500_SPI_SendByte(pBuf[2]);
    W5500_SPI_SendByte(pBuf[3]);
    W5500_PIN_SCS_H;
}
static uint8_t W5500_ReadSocket_1Byte(uint8_t num,uint16_t addr)
{
    uint8_t i;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_READ|(num*0x20+0x08));
    i=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i;
}
static uint16_t W5500_ReadSocket_2Byte(uint8_t num,uint16_t addr)
{
    uint16_t i16;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_READ|(num*0x20+0x08));
    i16=W5500_SPI_SendByte(0);
    i16*=256;
    i16+=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i16;
}
/*******************************************************************************
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
static uint8_t W5500_LoadVar(void)
{
    if(W5500_S_Config.LOCK==0x55)
    {
        W5500_S_Config.Sign_IpConflict=0;
        return W5500_TRUE;
    }
    else
    {
        return W5500_FALSE;
    }
}
/*******************************************************************************
* 描述    : W5500初始货配置
* 输入    : 无
* 输出    : 无
* 返回值  : OK ---   初始化成功     ERR   ---   初始化失败
* 说明    : 无
*******************************************************************************/

#ifdef STM32
static void W5500_Initialization(void)
#else
void W5500_Initialization(void)
#endif
{
    uint8_t i;
    uint8_t res;
    res=res;
    //----------硬复位
    W5500_PIN_RST_L;
    W5500_DelayMs(100);
    W5500_PIN_RST_H;
    W5500_DelayMs(100);
    //等待以太网连接完成
    while((W5500_ReadReg_1Byte(W5500_PHYCFGR)&W5500_PHYCFGR_LNK)==0)
    {
        W5500_DelayMs(100);
    }
    //----------初始化W5500寄存器函数
    //软件复位
    W5500_WriteReg_1Byte(W5500_MODE,W5500_MODE_RST);
    W5500_DelayMs(1000);
    W5500_DelayMs(1000);
    //读取芯片版本
    i=W5500_ReadReg_1Byte(W5500_VER);
    //设置网关
    W5500_WriteReg_nByte(W5500_GAR, W5500_S_Config.Gateway_IP, 4);
    //设置子网掩码
    W5500_WriteReg_nByte(W5500_SUBR, W5500_S_Config.Sub_Mask, 4);
    //设置物理地址
    //该地址值需要到IEEE申请，按照OUI的规定，前3个字节为厂商代码，后三个字节为产品序号
    //如果自己定义物理地址，注意第一个字节必须为偶数
    W5500_WriteReg_nByte(W5500_SHAR, W5500_S_Config.Phy_Addr, 6);
    //设置本机IP
    W5500_WriteReg_nByte(W5500_SIPR, W5500_S_Config.IP_Addr, 4);
    //设置发送缓冲区和接收缓冲区的大小，参考W5500数据手册
    //分配2K的存储空间
    for(i=0; i<W5500_CH_MAX; i++)
    {
        W5500_WriteSocket_1Byte(i,W5500_Sn_RXBUF_SIZE,0x02);
        W5500_WriteSocket_1Byte(i,W5500_Sn_TXBUF_SIZE,0x02);
    }
    //设置重试时间，默认为2000(200ms)
    //每一单位数值为100微秒,初始化时值设为2000(0x07D0),等于200毫秒
    W5500_WriteReg_2Byte(W5500_RTR,0x07d0);
    //设置重试次数，默认为8次
    //如果重发的次数超过设定值,则产生超时中断(相关的端口中断寄存器中的Sn_IR 超时位(TIMEOUT)置“1”)
    W5500_WriteReg_1Byte(W5500_RCR,8);
    //中断启用->Socket事件:      IP冲突中断  |  UDP目的地址不能抵达中断
    W5500_WriteReg_1Byte(W5500_IMR,(W5500_IMR_CONFLICT|W5500_IMR_UNREACH));
    //中断启用->通道总开关:      8个通道的中断全部使能
    W5500_WriteReg_1Byte(W5500_SIMR,W5500_SIMR_S0|W5500_SIMR_S1|W5500_SIMR_S2|W5500_SIMR_S3|W5500_SIMR_S4|W5500_SIMR_S5|W5500_SIMR_S6|W5500_SIMR_S7);
    //中断启用->通道功能开关:    发送完成    |  超时  |  接收完成 |  断开连接 |  完成连接
    W5500_WriteSocket_1Byte(0, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(1, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(2, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(3, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(4, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(5, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(6, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(7, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    //----------检查网关服务器
    /*
    //检查网关及获取网关的物理地址
    {
        INT8U buf[4];
        buf[0]=W5500_S_Config.IP_Addr[0];
        buf[1]=W5500_S_Config.IP_Addr[1];
        buf[2]=W5500_S_Config.IP_Addr[2];
        buf[3]=W5500_S_Config.IP_Addr[3]+1;
        W5500_WriteSocket_4Byte(0,W5500_Sn_DIPR,buf);
    }
    //设置socket0为TCP模式
    W5500_WriteSocket_1Byte(0,W5500_Sn_MR,W5500_Sn_MR_TCP);
    //打开socket0
    W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_OPEN);
    //延时5ms
    W5500_DelayMs(5);
    //如果socket0打开失败
    if(W5500_ReadSocket_1Byte(0,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //打开不成功,关闭Socket
        W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        res = FALSE;
        goto W5500_Initialization_Goto;
    }

    //打开socket0的TCP连接
    W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CONNECT);
    do
    {
        INT8U j=0;
        //读取Socket0中断标志寄存器
        j=W5500_ReadSocket_1Byte(0,W5500_Sn_IR);
        if(j!=0)
            W5500_WriteSocket_1Byte(0,W5500_Sn_IR,j);
        W5500_DelayMs(5);//延时5ms
        if((j&W5500_Sn_IR_TIMEOUT) == W5500_Sn_IR_TIMEOUT)
        {
            res = FALSE;
            goto W5500_Initialization_Goto;
        }
        else if(W5500_ReadSocket_1Byte(0,W5500_Sn_DHAR) != 0xff)
        {
            //关闭Socket
            W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
            break;
        }
    }
    while(1);
    */
    //----------指定Socket(0~3)初始化,初始化端口0
    for(i=0; i<W5500_CH_MAX; i++)
    {
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        //设置分片长度，参考W5500数据手册，该值可以不修改
        //最大分片字节数=1460(0x5b4)
        //W5500_WriteReg_1Byte((W5500_S0_MSS+i*0x100),0x05);
        //W5500_WriteReg_1Byte((W5500_S0_MSS+i*0x100+1),0xb4);
        //设置本地端口端口号
        W5500_WriteSocket_2Byte(i,W5500_Sn_PORT,W5500_S_Connect[i].LocalPort);
        //设置目的IP和端口
        switch(W5500_S_Connect[i].order)
        {
            case W5500_E_CONNECT_ORDER_TCP_SERVER:
                //目前未实现
                break;
            case W5500_E_CONNECT_ORDER_TCP_IP_CLIENT:
                //设置目的端口号
                W5500_WriteSocket_2Byte(i,W5500_Sn_DPORTR,W5500_S_Connect[i].DestPort);
                //设置目的IP地址
                W5500_WriteSocket_4Byte(i,W5500_Sn_DIPR,W5500_S_Connect[i].DestIp);
                break;
            case W5500_E_CONNECT_ORDER_UDP_IP:
                //此处不用配置,发送时配置即可
                break;
            case W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT:
            case W5500_E_CONNECT_ORDER_UDP_DOMAIN:
                //先要连接DNS服务器获取IP
                //设置DNS服务器IP
                //W5500_WriteSocket_2Byte(i,W5500_Sn_DIPR,W5500_S_Config.DNS_IP);
                //设置DNS服务器PORT
                //W5500_WriteSocket_4Byte(i,W5500_Sn_DPORTR,W5500_S_Config.DNS_Port);
                break;
        }
    }
    //----------
//W5500_Initialization_Goto:
    return;
}
/*******************************************************************************
* 描述    : 设置指定Socket(0~3)为客户端与远程服务器连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 当本机Socket工作在客户端模式时,引用该程序,与远程服务器建立连接
*           如果启动连接后出现超时中断，则与服务器连接失败,需要重新调用该程序连接
*           该程序每调用一次,就与服务器产生一次连接
*******************************************************************************/
static uint8_t W5500_TCPClient_SocketConnect(uint8_t ch)
{
    //设置socket为TCP模式
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_TCP);
    //打开Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //如果socket打开失败
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //打开不成功,关闭Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    //设置Socket为Connect模式
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CONNECT);
    return TRUE;

    //至此完成了Socket的打开连接工作,至于它是否与远程服务器建立连接,则需要等待Socket中断，
    //以判断Socket的连接是否成功。参考W5500数据手册的Socket中断状态
}

/*******************************************************************************
* 描述    : 设置指定Socket(0~3)作为服务器等待远程主机的连接
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 当本机Socket工作在服务器模式时,引用该程序,等等远程主机的连接
*           该程序只调用一次,就使W5500设置为服务器模式
*******************************************************************************/
static uint8_t W5500_TCPServer_SocketListen(uint8_t ch)
{
    //设置socket为TCP模式
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_TCP);
    //打开Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //如果socket打开失败
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //打开不成功,关闭Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    //设置Socket为侦听模式
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_LISTEN);
    //
    W5500_DelayMs(5);
    //如果socket设置失败
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_LISTEN)
    {
        //设置不成功,关闭Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    return TRUE;

    //至此完成了Socket的打开和设置侦听工作,至于远程客户端是否与它建立连接,则需要等待Socket中断，
    //以判断Socket的连接是否成功。参考W5500数据手册的Socket中断状态
    //在服务器侦听模式不需要设置目的IP和目的端口号
}

/*******************************************************************************
* 描述    : 设置指定Socket(0~3)为UDP模式
* 输入    : s:待设定的端口
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 如果Socket工作在UDP模式,引用该程序,在UDP模式下,Socket通信不需要建立连接
*           该程序只调用一次，就使W5500设置为UDP模式
*******************************************************************************/
static uint8_t W5500_UDP_Socket(uint8_t ch)
{
    //设置Socket为UDP模式
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_UDP);
    //打开Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //如果Socket打开失败
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_UDP)
    {
        //打开不成功,关闭Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    return TRUE;

    //至此完成了Socket的打开和UDP模式设置,在这种模式下它不需要与远程主机建立连接
    //因为Socket不需要建立连接,所以在发送数据前都可以设置目的主机IP和目的Socket的端口号
    //如果目的主机IP和目的Socket的端口号是固定的,在运行过程中没有改变,那么也可以在这里设置
}

/*******************************************************************************
* 描述    : 指定Socket(0~3)接收数据处理
* 输入    : s:端口
* 输出    : 无
* 返回值  : 返回接收到数据的长度
* 说明    : 如果Socket产生接收数据的中断,则引用该程序进行处理
*           该程序将Socket的接收到的数据缓存到Rx_buffer数组中,并返回接收的数据字节数
*******************************************************************************/
static uint16_t W5500_App_Rx(uint8_t ch,uint8_t *pbuf,uint16_t maxlen)
{
    uint8_t j;
    uint16_t i16;
    uint16_t rx_size,rx_offset,rx_offset1;
    //
    if(0==(W5500_S_Connect[ch].SignBitmap & S_W5500_SOCKET_SIGNBITMAP_RX_OK))
    {
        return 0;
    }
    W5500_S_Connect[ch].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_RX_OK;
    //读取接收数据的字节数
    rx_size=W5500_ReadSocket_2Byte(ch,W5500_Sn_RX_RSR);
    if(rx_size==0)
    {
        return 0;
    }
    else if(rx_size>1460)
    {
        rx_size=1460;
    }
    //防止内存溢出
    if(rx_size>maxlen)
    {
        rx_size=maxlen;
    }
    //读取接收缓冲区的偏移量
    rx_offset=W5500_ReadSocket_2Byte(ch,W5500_Sn_RX_RD);
    rx_offset1=rx_offset;
    rx_offset&=(W5500_S_RX_SIZE-1);
    //
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(rx_offset);
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_READ|(ch*0x20+0x18));
    //发送一个哑数据并读取数据
    //j=W5500_SPI_SendByte(0);
    //如果最大地址未超过W5500接收缓冲区寄存器的最大地址
    if((rx_offset+rx_size)<W5500_S_RX_SIZE)
    {
        //循环读取rx_size个字节数据
        for(i16=0; i16<rx_size; i16++)
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
    }
    //如果最大地址超过W5500接收缓冲区寄存器的最大地址
    else
    {
        rx_offset=W5500_S_RX_SIZE-rx_offset;
        for(i16=0; i16<rx_offset; i16++) //循环读取出前offset个字节数据
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
        W5500_PIN_SCS_H;
        W5500_PIN_SCS_L;
        //写16位地址
        W5500_SPI_SendShort(0x00);
        //写控制字节,N个字节数据长度,读数据,选择端口s的寄存器
        W5500_SPI_SendByte(W5500_VDM|W5500_RWB_READ|(ch*0x20+0x18));
        j=W5500_SPI_SendByte(0);
        //循环读取后rx_size-offset个字节数据
        for(; i16<rx_size; i16++)
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
    }
    W5500_PIN_SCS_H;
    //更新实际物理地址,即下次读取接收到的数据的起始地址
    rx_offset1+=rx_size;
    W5500_WriteSocket_2Byte(ch, W5500_Sn_RX_RD, rx_offset1);
    //发送启动接收命令
    W5500_WriteSocket_1Byte(ch, W5500_Sn_CR, W5500_Sn_CR_RECV);
    //返回接收到数据的长度
    return rx_size;
}

/*******************************************************************************
* 描述    : 指定Socket(0~3)发送数据处理
* 输入    : s:端口,size(发送数据的长度)
* 输出    : 无
* 返回值  : 成功返回TRUE(0xFF),失败返回FALSE(0x00)
* 说明    : 要发送的数据缓存在Tx_buffer中
*******************************************************************************/
uint8_t W5500_App_Tx(uint8_t ch,uint8_t* pbuf ,uint16_t size)
{
    uint16_t i16;
    uint16_t tx_offset,tx_offset1;
    //如果没有连接上,则发送数据
    if(W5500_S_Connect[ch].state!=W5500_E_CONNECT_STATE_IP_CONNECTED
       && W5500_S_Connect[ch].state!=W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
    {
        return W5500_FALSE;
    }
    //如果是UDP模式,可以在此设置目的主机的IP和端口号
    //if((W5500_ReadSocket_1Byte(ch,W5500_Sn_MR)&0x0f)!=W5500_Sn_SR_UDP)
    if((W5500_ReadSocket_1Byte(ch,W5500_Sn_MR))!=W5500_Sn_SR_UDP)
    {
        if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            //设置目的主机IP
            W5500_WriteSocket_4Byte(ch,W5500_Sn_DIPR,W5500_S_Connect[ch].DestIp);
            //设置目的主机端口号
            W5500_WriteSocket_2Byte(ch,W5500_Sn_DPORTR,W5500_S_Connect[ch].DestPort);
        }
        else if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
        {
            //设置DNS服务器IP
            W5500_WriteSocket_4Byte(ch,W5500_Sn_DIPR,W5500_S_Config.DNS_IP);
            //设置DNS服务器PORT
            W5500_WriteSocket_2Byte(ch,W5500_Sn_DPORTR,W5500_S_Config.DNS_Port);
        }
    }
    //计算实际的物理地址
    tx_offset=W5500_ReadSocket_2Byte(ch,W5500_Sn_TX_WR);
    tx_offset1=tx_offset;
    tx_offset&=(W5500_S_TX_SIZE-1);
    W5500_PIN_SCS_L;
    //写16位地址
    W5500_SPI_SendShort(tx_offset);
    //写控制字节,N个字节数据长度,写数据,选择端口s的寄存器
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|(ch*0x20+0x10));
    //如果最大地址未超过W5500发送缓冲区寄存器的最大地址
    if((tx_offset+size)<W5500_S_TX_SIZE)
    {
        //循环写入size个字节数据
        for(i16=0; i16<size; i16++)
        {
            //写入一个字节的数据
            W5500_SPI_SendByte(*pbuf++);
        }
    }
    //如果最大地址超过W5500发送缓冲区寄存器的最大地址
    else
    {
        tx_offset=W5500_S_TX_SIZE-tx_offset;
        //循环写入前offset个字节数据
        for(i16=0; i16<tx_offset; i16++)
        {
            //写入一个字节的数据
            W5500_SPI_SendByte(*pbuf++);
        }
        W5500_PIN_SCS_H;
        W5500_PIN_SCS_L;
        //写16位地址
        W5500_SPI_SendShort(0x00);
        //写控制字节,N个字节数据长度,写数据,选择端口s的寄存器
        W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|(ch*0x20+0x10));
        //循环写入size-offset个字节数据
        for(; i16<size; i16++)
        {
            //写入一个字节的数据
            W5500_SPI_SendByte(*pbuf++);
        }
    }
    //置W5500的SCS为高电平
    W5500_PIN_SCS_H;
    //更新实际物理地址,即下次写待发送数据到发送数据缓冲区的起始地址
    tx_offset1+=size;
    W5500_WriteSocket_2Byte(ch, W5500_Sn_TX_WR, tx_offset1);
    //发送启动发送命令
    W5500_WriteSocket_1Byte(ch, W5500_Sn_CR, W5500_Sn_CR_SEND);
    //返回成功
    return W5500_TRUE;
}
/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*           从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
static uint8_t W5500_Socket_Monitor(void)
{
    uint8_t i,res,len;
    uint8_t *p_i8;
    static uint8_t timer[W5500_CH_MAX]= {0};
    static uint16_t timer_all=0;
    //总通道监控
    for(i=0; i<W5500_CH_MAX; i++)
    {
        //无需连接
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            timer_all=0;
        }
    }
    timer_all++;
    if(timer_all>=300)
    {
        timer_all=0;
        return TRUE;
    }
    //单个通道监控
    for(i=0; i<W5500_CH_MAX; i++)
    {
        //无需连接
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        //初始连接
        if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_NULL)
        {
            //TCP服务器模式
            if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_SERVER)
            {
                if(W5500_TCPServer_SocketListen(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-IP客户端模式
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_IP_CLIENT)
            {
                if(W5500_TCPClient_SocketConnect(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //UDP-IP模式
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_IP)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-域名客户端模式 || UDP-域名模式
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                {
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED;
                    //发送查询DNS数据帧
                    //---申请缓存
                    p_i8=MemManager_Get(256,&res);
                    //---域名转换为标准格式帧
                    DNSXsl_MakeQuery(W5500_S_Connect[i].DestDnmain,p_i8,&len);
                    //---发送
                    W5500_App_Tx(i,p_i8,len);
                    //---释放缓存
                    MemManager_Free(p_i8,&res);
                }
                else
                {
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
                }
            }
        }
        //如果已经获取到了IP,则断开网络,并更新标志
        else if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_DOMAIN_GETIP_OK)
        {
            W5500_WriteSocket_1Byte(i,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
            W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_DOMAIN_GETIP_CLOSED;
        }
        //如果已经关闭了DNS服务器链接,则开始连接目标
        else if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_DOMAIN_GETIP_CLOSED)
        {
            //设置目标IP和PORT
            W5500_WriteSocket_2Byte(i,W5500_Sn_DPORTR,W5500_S_Connect[i].DestPort);
            W5500_WriteSocket_4Byte(i,W5500_Sn_DIPR,W5500_S_Connect[i].DestIp);
            //TCP服务器模式
            if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_SERVER)
            {
                if(W5500_TCPServer_SocketListen(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-IP客户端模式
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_IP_CLIENT
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT)
            {
                if(W5500_TCPClient_SocketConnect(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //UDP-IP模式
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_IP
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
        }
        //监控连接时间
        if(W5500_S_Connect[i].state!=W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            timer[i]++;
            if(timer[i]>=100)
            {
                timer[i]=0;
                W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
        }
    }
    return FALSE;
}
/*******************************************************************************
* 描述    : W5500中断处理程序框架
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
static void W5500_Interrupt_Process(void)
{
    uint8_t i,j,ch;
    uint8_t num=3;
    while(num--)
    {
        //读取中断标志寄存器
        i=W5500_ReadReg_1Byte(W5500_IR);
        //回写清除中断标志
        W5500_WriteReg_1Byte(W5500_IR, (i&0xf0));
        //IP地址冲突异常处理
        if((i & W5500_IR_CONFLICT) == W5500_IR_CONFLICT)
        {
            //-----自己添加代码
            W5500_S_Config.Sign_IpConflict=1;
            //-----
        }
        //UDP模式下地址无法到达异常处理
        if((i & W5500_IR_UNREACH) == W5500_IR_UNREACH)
        {
            //-----自己添加代码
            //-----
        }
        //读取端口中断标志寄存器
        i=W5500_ReadReg_1Byte(W5500_SIR);
        if(i==0)
        {
            break;
        }
        for(ch=0; ch<W5500_CH_MAX; ch++)
        {
            //Socket0事件处理
            if((i & (W5500_SIR_S0<<ch)) != 0)
            {
                //读取Socket中断标志寄存器
                j=W5500_ReadSocket_1Byte(ch,W5500_Sn_IR);
                //回写清中断标志
                W5500_WriteSocket_1Byte(ch,W5500_Sn_IR, j);
                //在TCP模式下,Socket成功连接
                if(j&W5500_Sn_IR_CON)
                {
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                }
                //在TCP模式下Socket断开连接处理
                if(j&W5500_Sn_IR_DISCON)
                {
                    // 关闭端口,等待重新打开连接
                    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_NULL;
                }
                //Socket数据发送完成
                if(j&W5500_Sn_IR_SENDOK)
                {
                    W5500_S_Connect[ch].SignBitmap|=S_W5500_SOCKET_SIGNBITMAP_TX_OK;
                }
                //Socket接收到数据
                if(j&W5500_Sn_IR_RECV)
                {
                    W5500_S_Connect[ch].SignBitmap|=S_W5500_SOCKET_SIGNBITMAP_RX_OK;
                }
                //Socket连接或数据传输超时处理
                if(j&W5500_Sn_IR_TIMEOUT)
                {
                    //关闭端口,等待重新打开连接
                    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
                    //网络连接状态0x00,端口连接失败
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_NULL;
                }
            }
        }
    }
}

/*******************************************************************************
* 描述    : 中断服务函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
*******************************************************************************/
void W5500_IRQHandler(void)
{
    W5500_InterruptSign=1;
}
/*******************************************************************************
* 描述    : 对外接口函数,放在100mS定时
* 输入    : 无
* 输出    : 无
* 返回值  : 无
*******************************************************************************/
void W5500_Main_100ms(uint8_t sign_init)
{
    static uint8_t first=1;
    uint8_t ch=0;
    uint16_t i16;
    uint8_t res;
    uint8_t *pbuf;
    //
    if(sign_init==TRUE)
    {
        first=1;
    }
    //INT8U i;
    if(first!=0)
    {
        if(first==1)
        {
            //初始化硬件配置
            W5500_GPIO_Configuration();
            first=2;
        }
        if(first==2)
        {
            //装载网络参数
            if(W5500_TRUE==W5500_LoadVar())
            {
                first=3;
            }
            else
            {
                return;
            }
        }
        if(first==3)
        {
            //W5500初始货配置
            W5500_Initialization();
            first=0;
        }
    }
    //W5500端口连接监控
    if(TRUE==W5500_Socket_Monitor())
    {
        first=1;
    }
    //处理W5500中断
    if(W5500_InterruptSign)
    {
        W5500_InterruptSign=0;
        W5500_Interrupt_Process();
    }
    //Socket0接收数据解析
    //申请缓存
    pbuf=Mem1Manager_Get(1024,&res);
    for(ch=0; ch<W5500_CH_MAX; ch++)
    {
        i16=W5500_App_Rx(ch,pbuf,1024);
        if(i16!=0)
        {
            W5500_ProtocolAnalysis(ch,pbuf,i16);
        }
    }
    //释放缓存
    Mem1Manager_Free(pbuf,&res);
}
/*******************************************************************************
* 描述    : 通道0-7的解析函数
* 输入    : *pbuf ---   缓存地址
*           len   ---   缓存有效长度
* 输出    : 无
* 返回值  : 无
*******************************************************************************/
static void W5500_ProtocolAnalysis(uint8_t ch,uint8_t *pbuf,uint16_t len)
{
    if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
    {
        if(W5500_S_Connect[ch].order==W5500_E_CONNECT_ORDER_UDP_IP
           ||W5500_S_Connect[ch].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
        {
            (*W5500APP_ProtocolAnalysis_ChX[ch])(&pbuf[8],len-8,&pbuf[0],pbuf[4]*256+pbuf[5]);
        }
        else
        {
            (*W5500APP_ProtocolAnalysis_ChX[ch])(pbuf,len,W5500_S_Connect[ch].DestIp,W5500_S_Connect[ch].DestPort);
        }
    }
    else if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
    {
        //解析DNS服务器数据帧
        if(TRUE==DNSXsl_Response(&pbuf[8],len-8,W5500_S_Connect[ch].DestIp))
        {
            //更新状态
            W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_DOMAIN_GETIP_OK;
        }
    }
}
#endif
#endif
