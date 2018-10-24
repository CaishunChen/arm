/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*********************************************************************************
说明: 蓝牙任务(用近似Modbus协议的模式来做)
一,接收理念:
接收第一层: Uart_Rx2Buf    ---   串口接收数据(DMA接收中断)(uart.c/stm32f10x_it.c)
            操作: 解析校验地址(首字节)-->复制数据至第二层-->发送消息队列
接收第二层: Modbus_RBuf0   ---   缓存环
            操作: 提取数据到第三层
接收第三层: Modbus_RBuf1   ---   待处理缓存
            操作: CRC校验-->解密-->解析Modbus
二,发送理念:
发送第一层: MODBUS_TBuf0   ---   直接DMA发送
            准备发送数据-->计算CRC-->加密-->DMA发送
**********************************************************************************/
//================================
#include "includes.h"
#include "uctsk_BluetoothBeaconM.h"
#include "Debug.h"
#include "Bsp_Uart.h"
//================================
//------------------------------- 数据类型 ---------------------------------------
//------------------------------- 静态变量 ---------------------------------------
#if   (defined(OS_UCOSII))
static  OS_STK    App_TaskBluetoothStk[APP_TASK_BLUETOOTH_STK_SIZE];
#elif (defined(OS_UCOSIII))
static  OS_TCB    App_TaskBluetoothTCB;
static  CPU_STK   App_TaskBluetoothStk[APP_TASK_BLUETOOTH_STK_SIZE];
#endif
//环
#define BLUETOOTH_BUFFERLOOP_BUF_MAX   256
#define BLUETOOTH_BUFFERLOOP_NODE_MAX  10
static uint8_t Bluetooth_BufferLoop_Buf[BLUETOOTH_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP1_S_NODE Bluetooth_BufferLoop_Node[BLUETOOTH_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP1_S_LIMIT Bluetooth_BufferLoop_Limit;
static COUNT_BUFFERLOOP1_S Bluetooth_BufferLoop;
//缓存
#define BLUETOOTH_BUFFER_TXBUF_MAX   100
static uint8_t Bluetooth_TxBuf[BLUETOOTH_BUFFER_TXBUF_MAX]= {0};
static uint8_t Bluetooth_TxBufNum=0;
//数据提取缓存区(申请缓存)
//数据发送缓存区(申请缓存)
//测试
uint16_t Bluetooth_ChipPowerCount=0;
uint16_t Bluetooth_ChipOkCount=0;
uint16_t Bluetooth_ChipErrCount=0;
uint16_t Bluetooth_ChipNCCount=0;
//测试使能
static uint8_t Bluetooth_DebugTest_Enable=0;
//模式
uint8_t Bluetooth_PowerOnHold_Enable=0;
//连接使能
uint8_t Bluetooth_ConnectSign=0; 
//------------------------------- 静态函数 ---------------------------------------
static void uctsk_Bluetooth(void);
static void Bluetooth_Init(void);
static void Bluetooth_Power(uint8_t OnOff);
//------------------------------- 引用变量 ---------------------------------------
//--------------------------------------------------------------------------------
/*******************************************************************************
函数功能: 硬件初始化
*******************************************************************************/
static void Bluetooth_Init(void)
{
    //GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    BLUETOOTH_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_OD;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    //
    GPIO_InitStructure.GPIO_Pin     =  BLUETOOTH_PWR_PIN;
    GPIO_Init(BLUETOOTH_PWR_PORT, &GPIO_InitStructure);
    //
    Bluetooth_Power(OFF);
    //串口初始化
    UART_INIT(3,115200);
    //初始化变量
    Bluetooth_TxBufNum=0;
}
static void Bluetooth_Power(uint8_t OnOff)
{
    static uint8_t state=0xFF;
    if(state==OnOff)
        return;
    state = OnOff;
    if(OnOff==ON)
    {
        UART_INIT(3,115200);
        GPIO_ResetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN);
        UART_DMA_Tx(3,"12345678",8);
    }
    else if(OnOff==OFF)
    {
        GPIO_InitTypeDef GPIO_InitStructure;
#if   (defined(STM32F1))
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
        }
#elif (defined(STM32F4))
        {
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
            GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    
        }
#endif
        USART_Cmd(USART3, DISABLE);
        GPIO_InitStructure.GPIO_Pin   = BLUETOOTH_TX_PIN;
        GPIO_Init(BLUETOOTH_TX_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin  = BLUETOOTH_RX_PIN;
        GPIO_Init(BLUETOOTH_RX_PORT, &GPIO_InitStructure);
        GPIO_ResetBits(BLUETOOTH_TX_PORT , BLUETOOTH_TX_PIN);
        GPIO_ResetBits(BLUETOOTH_RX_PORT , BLUETOOTH_RX_PIN);
        GPIO_SetBits(BLUETOOTH_PWR_PORT , BLUETOOTH_PWR_PIN);
    }
}
/*******************************************************************************
函数功能:   数据复制到缓存环并发送消息(只被调用于中断)
注    意:   目前是发送消息给本任务处理，未来优化应该直接发送消息给3G任务，避免不必
            要的数据复制。
*******************************************************************************/
void Bluetooth_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len)
{
    //
    if(Bluetooth_DebugTest_Enable==ON)
    {
        DebugOutHex("Rx:",pRxIqBuf,len);
    }
    //过少的数据丢弃
    /*
    if(len<10)
    {
        Bluetooth_ChipErrCount++;
        return;
    }
    */
    if(BLUETOOTH_BUFFER_TXBUF_MAX>=(len+Bluetooth_TxBufNum))
    {
        memcpy(&Bluetooth_TxBuf[Bluetooth_TxBufNum],pRxIqBuf,len);
        Bluetooth_TxBufNum+=len;
    }
}
uint8_t Bluetooth_BufferLoopPop(uint8_t *pBuf,uint16_t *plen)
{
    return Count_BufferLoop1Pop(&Bluetooth_BufferLoop,pBuf,plen,COUNT_BUFFERLOOP1_E_POPMODE_DELETE);
}
/*******************************************************************************
函数功能: Bluetooth任务创建
*******************************************************************************/
void  App_BluetoothTaskCreate (void)
{
#if   (defined(OS_UCOSII))
    CPU_INT08U  os_err;
    os_err = os_err;
    os_err = OSTaskCreateExt(
                 //函数体
                 (void (*)(void *))uctsk_Bluetooth,
                 //参数
                 (void *)0,
                 //堆栈
                 (OS_STK *)&App_TaskBluetoothStk[APP_TASK_BLUETOOTH_STK_SIZE-1],
                 //优先级
                 APP_TASK_BLUETOOTH_PRIO,
                 APP_TASK_BLUETOOTH_PRIO,
                 (OS_STK *)&App_TaskBluetoothStk[0],
                 APP_TASK_BLUETOOTH_STK_SIZE,
                 (void *)0,
                 OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
             );
#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_BLUETOOTH_PRIO, "Task Bluetooth", &os_err);
#endif
#elif (defined(OS_UCOSIII))
    OS_ERR      err;
    OSTaskCreate((OS_TCB       *)&App_TaskBluetoothTCB,
                 (CPU_CHAR     *)"App Task Bluetooth",
                 (OS_TASK_PTR   )uctsk_Bluetooth,
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_BLUETOOTH_PRIO,
                 (CPU_STK      *)&App_TaskBluetoothStk[0],
                 (CPU_STK_SIZE  )APP_TASK_BLUETOOTH_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_BLUETOOTH_STK_SIZE,
                 (OS_MSG_QTY    )1,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
#endif
}
/*******************************************************************************
函数功能: Sensor任务实体
*******************************************************************************/
static void uctsk_Bluetooth (void)
{
    uint8_t err;
    uint16_t i16;
    uint16_t power_timer=0;
    //初始化硬件
    Bluetooth_Init();
    //初始化数据结构
    Count_BufferLoop1Init(&Bluetooth_BufferLoop,\
                          Bluetooth_BufferLoop_Buf,\
                          BLUETOOTH_BUFFERLOOP_BUF_MAX,\
                          Bluetooth_BufferLoop_Node,\
                          BLUETOOTH_BUFFERLOOP_NODE_MAX,\
                          &Bluetooth_BufferLoop_Limit);
    //
    for(;;)
    {
        //OSTimeDlyHMSM(0, 0, 1, 0);
        //DebugOutStr("Task Bluetooth Test\r\n");
        //-----------------------定期开关
        //-----------------------
        Module_OS_DelayMs(100);
        //供电10秒,断电10秒
        power_timer++;
        if(power_timer==1)
        {
            Bluetooth_Power(ON);
            Bluetooth_ChipPowerCount++;
        }
        else if(power_timer==100)
        {
            if(Bluetooth_PowerOnHold_Enable==0)
            {
                Bluetooth_Power(OFF);
            }
            //处理数据
            if(Bluetooth_TxBufNum==0)
            {
                //没有Beacon数据
                Bluetooth_ChipNCCount++;
            }
            else if(Bluetooth_TxBufNum%10==0)
            {
                //数据为10的倍数,则处理
                for(err=0; err<Bluetooth_TxBufNum; err+=10)
                {
                    //校验
                    i16=Count_Sum(0,&Bluetooth_TxBuf[err],9);
                    i16=i16&0x00FF;
                    if(i16==Bluetooth_TxBuf[err+9])
                    {
                        //校验正确
                        //数据复制到缓存环
                        Count_BufferLoop1Push(&Bluetooth_BufferLoop,&Bluetooth_TxBuf[err],10);
                    }
                    else
                    {
                        //校验错误
                    }
                }
                //
                Bluetooth_ChipOkCount++;
            }
            else
            {
                //数据数量不对
                Bluetooth_ChipErrCount++;
            }
            //清空缓存
            Bluetooth_TxBufNum=0;
        }
        else if(power_timer>200)
        {
            power_timer=0;
        }
    }
}
void Bluetooth_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        Bluetooth_DebugTest_Enable=1;
    }
    else
    {
        Bluetooth_DebugTest_Enable=0;
    }
}
