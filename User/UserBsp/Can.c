/**
  ******************************************************************************
  * @file    Can.c 
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
//-------------------------------------------------------------------------------Private include
#include <includes.h>
//-------------------------------------------------------------------------------Private define
#define CANx CAN1
//-------------------------------------------------------------------------------Private typedef
//-------------------------------------------------------------------------------Private variables
static  OS_STK  App_TaskCanStk[APP_TASK_CAN_STK_SIZE];
//消息队列-->Can口数据
#define CAN_SUBSECTION_QBUF_MAX  10
OS_EVENT *CanSubsectionQ;
void *CanSubsectionQBuf[CAN_SUBSECTION_QBUF_MAX];
//-----第一层: 接收中断数据提取缓存区
CanRxMsg  Can_RxMsg_RBuf0;
//-----第二层: 环缓存
#define CAN_RxMsg_BUF1_MAX  CAN_SUBSECTION_QBUF_MAX
CanRxMsg  Can_RxMsg_RBuf1[CAN_RxMsg_BUF1_MAX];
uint16_t CanRxMsgRBuf1Addr=0;            //环儿存储新地址
//-----第三层: 应用层数据提取缓存区
CanRxMsg  Can_RxMsg_RBuf2;
//-------------------------------------------------------------------------------Private function prototypes
static void CAN_Config(void);
static void uctsk_Can(void);
//-------------------------------------------------------------------------------CAN配置
/********************************************************************************
* 函数功能: Can总线初始化配置
* 入口参数: 无
* 返回参数: 无
********************************************************************************/
static void CAN_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    CAN_InitTypeDef  CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    //----------中断配置
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //----------时钟配置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    //----------重映射
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
    //----------GPIO配置
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
    //----------CAN寄存器初始化
    CAN_DeInit(CANx);
    CAN_StructInit(&CAN_InitStructure);
    //----------CAN单元初始化
    CAN_InitStructure.CAN_TTCM = DISABLE;//时间触发禁止, 时间触发：CAN硬件的内部定时器被激活，并且被用于产生时间戳
    CAN_InitStructure.CAN_ABOM = ENABLE;//自动离线禁止，自动离线：一旦硬件监控到128次11个隐性位，就自动退出离线状态。在这里要软件设定后才能退出
    CAN_InitStructure.CAN_AWUM = DISABLE;//自动唤醒禁止，有报文来的时候自动退出休眠
    CAN_InitStructure.CAN_NART = ENABLE ;//报文重传, 如果错误一直传到成功止，否则只传一次
    CAN_InitStructure.CAN_RFLM = DISABLE;//接收FIFO锁定, 1--锁定后接收到新的报文摘不要，0--接收到新的报文则覆盖前一报文
    CAN_InitStructure.CAN_TXFP = ENABLE; //发送优先级  0---由标识符决定  1---由发送请求顺序决定
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    //CAN波特率
    /* 波特率计算方法 */
    /* CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  此处计算为  CANbps=36000000/(45*(4+3+1))=100kHz */                                                             //此处Tseg1+1 = CAN_BS1_8tp
    /* 配置大方向: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW */
    /* CAN Baudrate = 1MBps*/
    /*
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳宽，只有can硬件处于初始化模式时才能访问这个寄存器
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;//时间段1
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;//时间段2
    CAN_InitStructure.CAN_Prescaler = 4;    //波特率预分频数
    */
    /* CAN Baudrate = 100kbps*/
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;//重新同步跳宽，只有can硬件处于初始化模式时才能访问这个寄存器
    CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;//时间段1
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;//时间段2
    CAN_InitStructure.CAN_Prescaler = 45;    //波特率预分频数
    if (CAN_Init(CANx,&CAN_InitStructure) == CANINITFAILED)
    {
        /* 初始化时先设置CAN_MCR的初始化位 */
        /* 然后查看硬件是否真的设置了CAN_MSR的初始化位来确认是否进入了初始化模式  */
    }
    /* 配置CAN过滤器 */
    /* 32位对应的id */
    /* stdid[10:0]，extid[17:0],ide,rtr   */
    /* 16位对应的id */
    /* stdid[10:0],ide,rtr,extid[17:15] */
    /* 一般使用屏蔽模式   */
    /* 要注意的是fifo接收存满了中断，还有就是fifo的概念，即取的一直是最早那一个数据， 要释放才能取下一个数据 */
    /* 常使用的中断有 */
    /* 1,有信息中断，即fifo挂号中断 */
    /* 2,fifo满中断   */
    /* 3,fifo满之后又有信息来则中断，即fifo溢出中断   */
    CAN_FilterInitStructure.CAN_FilterNumber=0;     /* 过滤器0 */
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  /* 屏敝模式 */
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; /* 32位 */
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;  /* 以下四个都为0, 表明不过滤任何id */
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;  /* 能够通过该过滤器的报文存到fifo0中 */
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
    CAN_ITConfig(CANx,CAN_IT_FMP0, ENABLE);   /* 挂号中断, 进入中断后读fifo的报文函数释放报文清中断标志 */
    CAN_FilterInit(&CAN_FilterInitStructure);
}
/********************************************************************************
* 函数功能: Can总线发送
* 入口参数: 无
* 返回参数: 无
********************************************************************************/
void CanWriteData(uint16_t ID,uint8_t *Txbuf)
{
    //INT8U res;
    static CanTxMsg TxMessage;
    //设置标准id(11bit)(0~0x7FF)(注意:标准id的最高7位不能全是隐性(1))
    TxMessage.StdId = ID;
    //设置扩展id(18bit)(0~0x1FFFFFFF)
    //TxMessage.ExtId = 0;
    //远程传输请求(数据帧/控制帧)
    TxMessage.RTR = CAN_RTR_DATA;
    //TxMessage.RTR = CAN_RTR_REMOTE;
    //使用标准id(标准帧/扩展帧)
    TxMessage.IDE = CAN_ID_STD;
    //TxMessage.IDE = CAN_ID_EXT;
    //数据区长度(0-8)
    TxMessage.DLC = 8;
    //数据区
    TxMessage.Data[0] = Txbuf[0];
    TxMessage.Data[1] = Txbuf[1];
    TxMessage.Data[2] = Txbuf[2];
    TxMessage.Data[3] = Txbuf[3];
    TxMessage.Data[4] = Txbuf[4];
    TxMessage.Data[5] = Txbuf[5];
    TxMessage.Data[6] = Txbuf[6];
    TxMessage.Data[7] = Txbuf[7];
    //发送消息
    CAN_Transmit(CANx,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */
    /*
    while(1)
    {
    res = CAN_Transmit(CANx,&TxMessage);
    if(res!=CAN_TxStatus_NoMailBox)
    {
        break;
    }
    else
    {
        OSTimeDlyHMSM(0, 0, 0, 1);
    }
    }
    */
}
//翻转显示LED
static void CanRxLed(void)
{
    static uint8_t state=ON;
    if(state==ON)
    {
        state=OFF;
    }
    else
    {
        state=ON;
    }
#ifdef __HT1632_H    
    //HT1632_APP(0,HT1632_LED_ALLOCATION_CAN_RX,state,NULL,state);
#endif
#ifdef __NXP74HC595_H
    //NXP74HC595_APP(NXP74HC595_LED_ALLOCATION_CAN_RX,state);
#endif
}
static void CanTxLed(void)
{
    static uint8_t state=ON;
    if(state==ON)
    {
        state=OFF;
    }
    else
    {
        state=ON;
    }
#ifdef __HT1632_H     
    //HT1632_APP(0,HT1632_LED_ALLOCATION_CAN_TX,state,NULL,state);
#endif
#ifdef __NXP74HC595_H
    //NXP74HC595_APP(HT1632_LED_ALLOCATION_CAN_TX,state);
#endif
}
/********************************************************************************
* 函数功能: Can总线接收中断
* 入口参数: 无
* 返回参数: 无
********************************************************************************/
void CAN_RX_IRQHandler(void)
{
    uint8_t i;
    //
    Can_RxMsg_RBuf0.StdId=0x00;
    Can_RxMsg_RBuf0.IDE=CAN_ID_STD;
    Can_RxMsg_RBuf0.DLC=0;
    for(i=0; i<8; i++)
    {
        Can_RxMsg_RBuf0.Data[i]=0x00;
    }
    //此函数包含释放提出报文了的,在非必要时,不需要自己释放
    CAN_Receive(CANx, CAN_FIFO0, &Can_RxMsg_RBuf0);
    //复制到缓存环
    memcpy((char*)&Can_RxMsg_RBuf1[CanRxMsgRBuf1Addr],(char*)&Can_RxMsg_RBuf0,sizeof(CanRxMsg));
    //发送消息队列
    OSQPost (CanSubsectionQ,&Can_RxMsg_RBuf1[CanRxMsgRBuf1Addr]);
    //更新地址
    CanRxMsgRBuf1Addr++;
    if(CanRxMsgRBuf1Addr>=CAN_RxMsg_BUF1_MAX)
    {
        CanRxMsgRBuf1Addr=0;
    }
    //
    //错误警告标志位
    if(CAN_GetFlagStatus(CANx,CAN_FLAG_EWG))
        CAN_ClearFlag(CANx,CAN_FLAG_EWG);
    //错误被动标志位
    if(CAN_GetFlagStatus(CANx,CAN_FLAG_EPV))
        CAN_ClearFlag(CANx,CAN_FLAG_EPV);
    //离线标志位
    if(CAN_GetFlagStatus(CANx,CAN_FLAG_BOF))
        CAN_ClearFlag(CANx,CAN_FLAG_BOF);
    //清除挂起中断
    CAN_ClearITPendingBit(CANx,CAN_IT_FMP0);
}
//-------------------------------------------------------------Can任务创建
void  App_CanTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Can",\
                              uctsk_Can,\
                              APP_TASK_CAN_PRIO,\
                              App_TaskCanStk,\
                              APP_TASK_CAN_STK_SIZE,\
                              AppTaskCanTCB,\
                              NULL);
}
//--------------------------------------------------------------Modbus任务实体
static void uctsk_Can (void)
{
    CanRxMsg *prx;
    uint8_t err;
    uint8_t buf[8]= {'C','a','n','T','x',' ','O','K'};
    //创建消息队列
    CanSubsectionQ = OSQCreate (&CanSubsectionQBuf[0], CAN_SUBSECTION_QBUF_MAX);
    //配置
    CAN_Config();
    //
    for(;;)
    {
        //等待消息
        prx = OSQPend (CanSubsectionQ,100,&err);
        if(err == OS_ERR_NONE)
        {
            //复制数据到应用层
            memcpy((char*)&Can_RxMsg_RBuf2,(char*)prx,sizeof(CanRxMsg));
            if(  Can_RxMsg_RBuf2.Data[0]==buf[0]
                 &&Can_RxMsg_RBuf2.Data[1]==buf[1]
                 &&Can_RxMsg_RBuf2.Data[2]==buf[2]
                 &&Can_RxMsg_RBuf2.Data[3]==buf[3]
                 &&Can_RxMsg_RBuf2.Data[4]==buf[4]
                 &&Can_RxMsg_RBuf2.Data[5]==buf[5]
                 &&Can_RxMsg_RBuf2.Data[6]==buf[6]
                 &&Can_RxMsg_RBuf2.Data[7]==buf[7]
              )
            {
                //
                CanRxLed();
            }
        }
        else
        {
            //测试->循环发送
            CanWriteData(0xA5A5,buf);
            CanTxLed();
        }
    }
}
//-------------------------------------------------------------------------------
