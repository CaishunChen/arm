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
#include "uctsk_BluetoothDTU.h"
#include "uctsk_Debug.h"
#include "Bsp_Uart.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_CpuId.h"
#include "Bsp_Rtc.h"
#include "IAP.h"
//================================
//------------------------------- 数据类型 ---------------------------------------
//------------------------------- 静态变量 ---------------------------------------
//环
#define BLUETOOTH_BUFFERLOOP_BUF_MAX   256
#define BLUETOOTH_BUFFERLOOP_NODE_MAX  10
static uint8_t Bluetooth_BufferLoop_Buf[BLUETOOTH_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE Bluetooth_BufferLoop_Node[BLUETOOTH_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT Bluetooth_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S Bluetooth_BufferLoop;
//任务
MODULE_OS_TASK_TAB(App_TaskBluetoothTCB);
MODULE_OS_TASK_STK(App_TaskBluetoothStk,APP_TASK_BLUETOOTH_STK_SIZE);
//信号量
MODULE_OS_SEM(Bluetooth_Sem_Rx);
//数据提取缓存区(申请缓存)
//数据发送缓存区(申请缓存)
//测试
uint16_t Bluetooth_ChipPowerCount=0;
uint16_t Bluetooth_ChipOkCount=0;
uint16_t Bluetooth_ChipErrCount=0;
uint16_t Bluetooth_ChipNCCount=0;
//连接时间
uint8_t Bluetooth_PowerOnHold_Enable   =  0;
//连接使能
uint8_t Bluetooth_ConnectOnOff         =  OFF;
//连接状态
uint8_t Bluetooth_ConnectSign          =  0;
//用于HCI显示的传输成功/失败
uint16_t Bluetooth_HciTxRxOkErr        =  MODULE_E_ERR_NULL;
//
BLUETOOTH_S_INFO Bluetooth_s_info;
//
BLUETOOTH_S_SLAVE BluetoothDtu_s_SlaveBuf[BLUETOOTH_DTU_S_SLAVEBUF_NUM];
//
static uint8_t Bluetooth_PowerOnHold_Enable_Bak =  0;
//测试使能
static uint8_t Bluetooth_DebugTest_Enable =  0;
//设备地址
static uint8_t Bluetooth_Addr          =  0;
//蓝牙与APP通信变量
static uint8_t Bluetooth_Step          =  0;
static BLUETOOTH_E_CMD Bluetooth_Cmd   =  BLUETOOTH_CMD_NULL;
//------------------------------- 静态函数 ---------------------------------------
static void uctsk_Bluetooth(void *pvParameters);
static void Bluetooth_Power(uint8_t OnOff);
static void Bluetooth_VisableOnOff(uint8_t OnOff);
static uint8_t Bluetooth_ProtocolAnalysis(uint8_t* pBuf,uint16_t len);
static uint8_t Bluetooth_Handle_100ms(uint8_t *pRxBuf,uint16_t len);
//------------------------------- 引用变量 ---------------------------------------
/*******************************************************************************
函数功能: 蓝牙供电控制
*******************************************************************************/
static void Bluetooth_Power(uint8_t OnOff)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(OnOff==ON)
    {
        Bluetooth_ChipPowerCount++;
        //初始化时钟
        BLUETOOTH_RCC_ENABLE;
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
        //
        GPIO_InitStructure.GPIO_Pin     =  BLUETOOTH_PWR_PIN;
        GPIO_Init(BLUETOOTH_PWR_PORT, &GPIO_InitStructure);
        BLUETOOTH_PWR_ON;
        //
        GPIO_InitStructure.GPIO_Pin     =  BLUETOOTH_RADLATE_PIN;
        GPIO_Init(BLUETOOTH_RADLATE_PORT, &GPIO_InitStructure);
        BLUETOOTH_RADLATE_L;
        //初始化输入(连接)
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IPU;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
        GPIO_InitStructure.GPIO_Pin     =  BLUETOOTH_CONNECT_PIN;
        GPIO_Init(BLUETOOTH_CONNECT_PORT, &GPIO_InitStructure);
        //串口初始化
        BLUETOOTH_PWR_ON;
        //初始化变量
        UART_INIT(BLUETOOTH_UARTX,BLUETOOTH_UART_BPS);
        //
    }
    else if(OnOff==OFF)
    {
        //关闭串口
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
        //关闭串口
        GPIO_InitStructure.GPIO_Pin   = BLUETOOTH_TX_PIN;
        GPIO_Init(BLUETOOTH_TX_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin  = BLUETOOTH_RX_PIN;
        GPIO_Init(BLUETOOTH_RX_PORT, &GPIO_InitStructure);
        GPIO_ResetBits(BLUETOOTH_TX_PORT , BLUETOOTH_TX_PIN);
        GPIO_ResetBits(BLUETOOTH_RX_PORT , BLUETOOTH_RX_PIN);
        //关闭GPIO
        BLUETOOTH_RADLATE_L;
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN_FLOATING;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
        GPIO_InitStructure.GPIO_Pin     =  BLUETOOTH_CONNECT_PIN;
        GPIO_Init(BLUETOOTH_CONNECT_PORT, &GPIO_InitStructure);
        //关闭时钟
        //关闭电源
        BLUETOOTH_PWR_OFF;
    }
}
/*******************************************************************************
函数功能: 蓝牙可见控制
*******************************************************************************/
static void Bluetooth_VisableOnOff(uint8_t OnOff)
{
    BLUETOOTH_RADLATE_H;
    if(OnOff==ON)
    {
        MODULE_OS_DELAY_MS(1000);
    }
    else
    {
        MODULE_OS_DELAY_MS(4000);
    }
    BLUETOOTH_RADLATE_L;
    Bluetooth_ConnectOnOff=OnOff;
}
/*******************************************************************************
函数功能:   数据复制到缓存环并发送消息(只被调用于中断)
注    意:   目前是发送消息给本任务处理，未来优化应该直接发送消息给3G任务，避免不必
            要的数据复制。
*******************************************************************************/
void Bluetooth_RxIrqBufToRBuf0(uint8_t *pRxIqBuf,uint16_t len)
{
    if(Bluetooth_s_info.Mode==0)
    {
        //Debug输出
        if(Bluetooth_DebugTest_Enable==ON)
        {
            DebugOutHex("Rx:",pRxIqBuf,len);
            //DebugOutStr("Rx\r\n");
        }
        //数据压栈
        Count_BufferLoopPush(&Bluetooth_BufferLoop,pRxIqBuf,len);
        //发送消息
        MODULE_OS_SEM_POST(Bluetooth_Sem_Rx);
    }
    else
    {
        //
        if(!BLUETOOTH_CONNECT_R || Bluetooth_ConnectOnOff==OFF)
        {
            return;
        }
        //Debug输出
        if(Bluetooth_DebugTest_Enable==ON)
        {
            DebugOutHex("Rx:",pRxIqBuf,len);
            //DebugOutStr("Rx\r\n");
        }
        //数据压栈
        Count_BufferLoopPush(&Bluetooth_BufferLoop,pRxIqBuf,len);
        //发送消息
        MODULE_OS_SEM_POST(Bluetooth_Sem_Rx);
    }
}
/*******************************************************************************
函数功能: Bluetooth发送
*******************************************************************************/
uint8_t Bluetooth_Tx(uint8_t *pBuf,uint16_t len)
{
    if(BLUETOOTH_CONNECT_R && Bluetooth_ConnectOnOff==ON)
    {
        UART_DMA_Tx(BLUETOOTH_UARTX,pBuf,len);
        if(Bluetooth_DebugTest_Enable==ON)
        {
            DebugOutHex("Tx:",pBuf,len);
            //DebugOutStr("Tx\r\n");
        }
        return OK;
    }
    else
    {
        return ERR;
    }
}
/*******************************************************************************
函数功能: Bluetooth任务创建
*******************************************************************************/
void  App_BluetoothTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Bluetooth",\
                          uctsk_Bluetooth,\
                          APP_TASK_BLUETOOTH_PRIO,\
                          App_TaskBluetoothStk,\
                          APP_TASK_BLUETOOTH_STK_SIZE,\
                          App_TaskBluetoothTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: Sensor任务实体
*******************************************************************************/
static void uctsk_Bluetooth (void *pvParameters)
{
    uint8_t *pbuf;
    uint8_t i=0,j=0;
    uint16_t len;
    //
    MODULE_OS_ERR err;
    //初始化信号量
    MODULE_OS_SEM_CREATE(Bluetooth_Sem_Rx,"Bluetooth_Sem_Rx",0);
    //初始化数据结构
    Count_BufferLoopInit(&Bluetooth_BufferLoop,\
                         Bluetooth_BufferLoop_Buf,\
                         BLUETOOTH_BUFFERLOOP_BUF_MAX,\
                         Bluetooth_BufferLoop_Node,\
                         BLUETOOTH_BUFFERLOOP_NODE_MAX,\
                         &Bluetooth_BufferLoop_Limit);
    memset((char*)&Bluetooth_s_info,0,sizeof(Bluetooth_s_info));
    //读取参数
    {
        MODULE_MEMORY_S_PARA *pPara;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        Bluetooth_Addr=pPara->Addr;
        if(Bluetooth_Addr==COUNT_MODBUS_BROADCAST_ADDR)
        {
            Bluetooth_Addr=1;
            pPara->Addr=Bluetooth_Addr;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
        }
        Count_ModbusSlaveAddr = Bluetooth_Addr;
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
    // 提取蓝牙MAC地址,并借此查看蓝牙是否启用
    while(1)
    {
        Bluetooth_Power(ON);
        MODULE_OS_SEM_PEND(Bluetooth_Sem_Rx,10000,TRUE,err);
        if(err==MODULE_OS_ERR_NONE)
        {
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            while(1)
            {
                Count_BufferLoopPop(&Bluetooth_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
                if(len==0)
                {
                    break;
                }
                if(len==6)
                {
                    if(j==0)
                    {
                        memcpy(Bluetooth_s_info.MacBuf,pbuf,len);
                        j=1;
                    }
                    else if(j==1&&Bluetooth_s_info.MacBuf[0]==pbuf[0]\
                            &&Bluetooth_s_info.MacBuf[1]==pbuf[1]\
                            &&Bluetooth_s_info.MacBuf[2]==pbuf[2]\
                            &&Bluetooth_s_info.MacBuf[3]==pbuf[3]\
                            &&Bluetooth_s_info.MacBuf[4]==pbuf[4]\
                            &&Bluetooth_s_info.MacBuf[5]==pbuf[5])
                    {
                        j=2;
                    }
                    break;
                }
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            if(len==6 && j>=2)
            {
                break;
            }
        }
        else if(err==MODULE_OS_ERR_TIMEOUT)
        {
            Bluetooth_Power(OFF);
            MODULE_OS_DELAY_MS(5000);
            i++;
        }
        while(i>=3)
        {
            MODULE_OS_DELAY_MS(60*1000);
        }
    }
    //
    Bluetooth_s_info.Mode=1;
    //
    for(;;)
    {
        MODULE_OS_SEM_PEND(Bluetooth_Sem_Rx,100,TRUE,err);
        //-----------------------
        if(err==MODULE_OS_ERR_NONE)
        {
            //申请缓存
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            //提取数据
            Count_BufferLoopPop(&Bluetooth_BufferLoop,pbuf,&len,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
            //解析数据
            Bluetooth_ProtocolAnalysis(pbuf,len);
            //释放缓存
            //MemManager_Free(pbuf,&res);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
        else if(err==MODULE_OS_ERR_TIMEOUT)
        {
            //
            Bluetooth_Handle_100ms(NULL,NULL);
            //保证一直处在广播状态
            if(BLUETOOTH_CONNECT_R)
            {
                //连接标识
                if(Bluetooth_ConnectSign==0)
                {
                    //处理
                    if(Bluetooth_DebugTest_Enable==ON)
                    {
                        DebugOutStr((int8_t*)"Bluetooth-Connect\r\n");
                    }
                }
                Bluetooth_ConnectSign=1;
                //调试输出
                if(Bluetooth_DebugTest_Enable==ON)
                {
                    //已连接
                    //Bluetooth_Tx("01234567890123456789",20);
                    //Bluetooth_Tx("abcdefghijklmnopqrst",20);
                }
            }
            //当前未连接状态
            else
            {
                //连接标识
                if(Bluetooth_ConnectSign==1)
                {
                    //处理
                    if(Bluetooth_DebugTest_Enable==ON)
                    {
                        DebugOutStr((int8_t*)"Bluetooth-Break\r\n");
                    }
                }
                Bluetooth_ConnectSign=0;
            }
            //
            if(Bluetooth_PowerOnHold_Enable_Bak != Bluetooth_PowerOnHold_Enable)
            {
                Bluetooth_PowerOnHold_Enable_Bak = Bluetooth_PowerOnHold_Enable;
                if(Bluetooth_PowerOnHold_Enable==OFF)
                {
                    Bluetooth_VisableOnOff(OFF);
                    MODULE_OS_DELAY_MS(1000);
                    Bluetooth_Power(OFF);
                }
                else
                {
                    Bluetooth_Power(OFF);
                    MODULE_OS_DELAY_MS(100);
                    Bluetooth_Power(ON);
                    MODULE_OS_DELAY_MS(100);
                    Bluetooth_VisableOnOff(ON);
                }
            }
        }
        else
        {
            //等待信号出错
            ;
        }

    }
}
/*******************************************************************************
函数功能: 协议解析(Modbus协议)
*******************************************************************************/
static uint8_t Bluetooth_ProtocolAnalysis(uint8_t* pBuf,uint16_t len)
{
    uint8_t i,res;
    uint16_t i16,j16;
    uint16_t tx_i16=0;
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pspara;
    //----------MODBUS----------
    //进行地址,长度,CRC16的校验
    res=Count_Modbus_Check(pBuf,len,Bluetooth_Addr);
    if(res!=OK)
    {
        // 解析
        Bluetooth_Handle_100ms(pBuf,len);
        return ERR;
    }
    //数据解密
    //数据处理
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    switch(pBuf[1])
    {
        case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
            //起始地址
            i16      =  Count_2ByteToWord(pBuf[3],pBuf[2]);
            //读取寄存器数量
            i   =  Count_2ByteToWord(pBuf[5],pBuf[4]);
            //
            if((i16+i)<=0x0013)
            {
                pspara = (MODULE_MEMORY_S_PARA*)&pbuf[128];
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                res=10;
                //硬件版本号
                pBuf[res++] =  HARDWARE_SUB_VER;
                pBuf[res++] =  HARDWARE_VER;
                //软件版本号
                pBuf[res++] =  SOFTWARE_SUB_VER;
                pBuf[res++] =  SOFTWARE_VER;
                //CPU唯一码
                BspCpuId_Get(&pBuf[res],NULL);
                res+=12;
                //闹钟1模式
                pBuf[res++] =  pspara->RtcAlarmMode[0];
                pBuf[res++] =  0x00;
                //闹钟1时间
                pBuf[res++] =  (uint8_t)pspara->RtcAlarm[0];
                pBuf[res++] =  (uint8_t)(pspara->RtcAlarm[0]>>8);
                //闹钟2模式
                pBuf[res++] =  pspara->RtcAlarmMode[1];
                pBuf[res++] =  0x00;
                //闹钟2时间
                pBuf[res++] =  (uint8_t)pspara->RtcAlarm[1];
                pBuf[res++] =  (uint8_t)(pspara->RtcAlarm[1]>>8);
                //时钟格式
                pBuf[res++] =  pspara->RtcFormat;
                pBuf[res++] =  0x00;
                //背光(常态)
                pBuf[res++] =  pspara->LcdBackLight1;
                pBuf[res++] =  0x00;
                //背光(测量态)
                pBuf[res++] =  pspara->LcdBackLight2;
                pBuf[res++] =  0x00;
                //备用
                pBuf[res++] =  0x00;
                pBuf[res++] =  0x00;
                //时间-年月
                BspRtc_ReadRealTime(NULL,NULL,NULL,&pBuf[200]);
                pBuf[res++] =  pBuf[200];
                pBuf[res++] =  pBuf[201];
                //时间-日时
                pBuf[res++] =  pBuf[202];
                pBuf[res++] =  pBuf[203];
                //时间-分秒
                pBuf[res++] =  pBuf[204];
                pBuf[res++] =  pBuf[205];
                // 字节数
                res   =  i*2;
                Count_Modbus_Array(pbuf,&tx_i16,Bluetooth_Addr,pBuf[1],NULL,NULL,&res,&pBuf[10+i16*2],res);
            }
            break;
        case COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            //起始地址
            i16      =  Count_2ByteToWord(pBuf[3],pBuf[2]);
            //读取寄存器数量
            j16      =  Count_2ByteToWord(pBuf[5],pBuf[4]);
            //字节数量

            //
            if((i16>0x0000) && ((i16+j16)<=0x000F))
            {
                /*
                //时钟同步处理
                // 字节数
                res   =  i*2;
                Count_Modbus_Array(pbuf,&tx_i16,Bluetooth_Addr,pBuf[1],NULL,NULL,&res,&pBuf[10+i16*2],res);
                */
            }
            //同步时钟
            else if(i16==0x0010  && j16==3)
            {
                BspRtc_SetRealTime(NULL,NULL,NULL,&pBuf[7]);
                Count_Modbus_Array(pbuf,&tx_i16,Bluetooth_Addr,pBuf[1],&i16,&j16,NULL,NULL,NULL);
            }
            break;
        case COUNT_MODBUS_FC_WRITE_IAP:
            //
            res=IAP_APP(IAP_APP_CMD_DATA,
                        &pBuf[10],
                        128,
                        Count_2ByteToWord(pBuf[3],pBuf[2]),
                        Count_2ByteToWord(pBuf[5],pBuf[4]),
                        Count_4ByteToLong(pBuf[9],pBuf[8],pBuf[7],pBuf[6]),
                        1,
                        &i16);
            Count_Modbus_Array(pbuf,&tx_i16,Bluetooth_Addr,pBuf[1],&i16,NULL,NULL,NULL,0);
            break;
        default:
            break;
    }
    //数据加密
    //应答发送
    if(pBuf[0]==Bluetooth_Addr)
    {
        //Module_OS_DelayMs(100);
        Bluetooth_Tx(pbuf,tx_i16);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    return OK;
}

/*******************************************************************************
函数功能: 应用接口
*******************************************************************************/
void Bluetooth_App(BLUETOOTH_E_CMD cmd,uint8_t *pbuf)
{
    pbuf=pbuf;
    switch(cmd)
    {
        case BLUETOOTH_CMD_NULL:
            break;
        case BLUETOOTH_CMD_UPDATA_1DAY:
        case BLUETOOTH_CMD_UPDATA_7DAY:
        case BLUETOOTH_CMD_IAP:
            if(Bluetooth_Cmd==BLUETOOTH_CMD_NULL)
            {
                Bluetooth_Cmd          =  cmd;
                Bluetooth_Step         =  0;
                Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_NULL;
            }
            break;
        case BLUETOOTH_CMD_READ_CMD:
            *pbuf = Bluetooth_Cmd;
            break;
        case BLUETOOTH_CMD_KEEP_CONNECT_ONOFF:
            if(*pbuf==ON || *pbuf==OFF)
            {
                Bluetooth_PowerOnHold_Enable  =  *pbuf;
            }
        default:
            break;
    }
}
/*******************************************************************************
函数功能: 处理流程
*******************************************************************************/
enum BLUETOOTH_E_UPLOAD_STEP
{
    BLUETOOTH_E_UPLOAD_STEP_POWER_ON  =  0,
    BLUETOOTH_E_UPLOAD_STEP_CONNECT,
    BLUETOOTH_E_UPLOAD_STEP_CONNECT_DELAY,
    BLUETOOTH_E_UPLOAD_STEP_HAND,
    BLUETOOTH_E_UPLOAD_STEP_HAND_WAIT_ACK,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK1,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK1_DELAY,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK2,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK2_DELAY,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK3,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK3_DELAY,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK4,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK4_DELAY,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK5,
    BLUETOOTH_E_UPLOAD_STEP_TX_PACK5_WAIT_ACK,
    BLUETOOTH_E_UPLOAD_STEP_TX_OK,
    BLUETOOTH_E_UPLOAD_STEP_TX_ERR,
    BLUETOOTH_E_UPLOAD_STEP_POWER_OFF,
};
enum BLUETOOTH_E_IAP_STEP
{
    BLUETOOTH_E_IAP_STEP_POWER_ON  =  0,
    BLUETOOTH_E_IAP_STEP_CONNECT,
    BLUETOOTH_E_IAP_STEP_CONNECT_DELAY,
    BLUETOOTH_E_IAP_STEP_MONITOR,
    BLUETOOTH_E_IAP_STEP_TX_OK,
    BLUETOOTH_E_IAP_STEP_TX_ERR,
    BLUETOOTH_E_IAP_STEP_POWER_OFF,
};
static uint8_t Bluetooth_Handle_100ms(uint8_t *pRxBuf,uint16_t len)
{
    uint8_t *pbuf;
    static uint16_t stimer=0;
    static uint8_t snum=0,ssub=0;
    if(stimer!=0xFFFF)
    {
        stimer++;
    }
    switch(Bluetooth_Cmd)
    {
        case BLUETOOTH_CMD_NULL:
            break;
        case BLUETOOTH_CMD_IAP:
            switch(Bluetooth_Step)
            {
                //蓝牙上电
                case BLUETOOTH_E_IAP_STEP_POWER_ON:
                    if(!BLUETOOTH_CONNECT_R)
                    {
                        Bluetooth_Power(OFF);
                        MODULE_OS_DELAY_MS(100);
                        Bluetooth_Power(ON);
                        MODULE_OS_DELAY_MS(100);
                        Bluetooth_VisableOnOff(ON);
                        //
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_CONNECT;
                        stimer=0;
                    }
                    else
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_CONNECT;
                        stimer=0;
                    }
                    break;
                //等待蓝牙连接
                case BLUETOOTH_E_IAP_STEP_CONNECT:
                    if(BLUETOOTH_CONNECT_R)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_CONNECT_DELAY;
                        stimer         =  0;
                    }
                    //最长等待60s
                    else if(stimer>=600)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_TX_ERR;
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_BLUETOOTH_CONNECT;
                    }
                    break;
                //连接延时
                case BLUETOOTH_E_IAP_STEP_CONNECT_DELAY:
                    //
                    if(stimer>=20)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_MONITOR;
                        stimer         =  0;
                    }
                    break;
                //判断是否有数据流
                case BLUETOOTH_E_IAP_STEP_MONITOR:
                    if(stimer>=100)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_POWER_ON;
                    }
                    ssub=Iap_CurrentPackage;
                    if(Iap_CurrentPackage!=Iap_AllPackage && snum!=ssub)
                    {
                        stimer=  0;
                        snum  =  ssub;
                    }
                    else if(Iap_CurrentPackage==Iap_AllPackage)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_TX_OK;
                    }
                    break;
                //成功
                case BLUETOOTH_E_IAP_STEP_TX_OK:
                    Bluetooth_ChipOkCount++;
                    Bluetooth_Step =  BLUETOOTH_E_IAP_STEP_POWER_OFF;
                    Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_NULL;
                    break;
                //失败
                case BLUETOOTH_E_IAP_STEP_TX_ERR:
                    Bluetooth_ChipErrCount++;
                    if(Bluetooth_HciTxRxOkErr==MODULE_E_ERR_NULL)
                    {
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_BLUETOOTH_TRANSFER;
                    }
                    Bluetooth_Step   =  BLUETOOTH_E_IAP_STEP_POWER_OFF;
                    break;
                //蓝牙掉电
                case BLUETOOTH_E_IAP_STEP_POWER_OFF:
                    if(Bluetooth_PowerOnHold_Enable==OFF)
                    {
                        Bluetooth_VisableOnOff(OFF);
                        MODULE_OS_DELAY_MS(1000);
                        Bluetooth_Power(OFF);
                    }
                    Bluetooth_Cmd   =  BLUETOOTH_CMD_NULL;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_POWER_ON;
                    break;
                default:
                    break;
            }
            break;
        case BLUETOOTH_CMD_UPDATA_1DAY:
            ssub = 1;
        case BLUETOOTH_CMD_UPDATA_7DAY:
            switch(Bluetooth_Step)
            {
                //蓝牙上电
                case BLUETOOTH_E_UPLOAD_STEP_POWER_ON:
                    if(!BLUETOOTH_CONNECT_R)
                    {
                        Bluetooth_Power(OFF);
                        MODULE_OS_DELAY_MS(100);
                        Bluetooth_Power(ON);
                        MODULE_OS_DELAY_MS(100);
                        Bluetooth_VisableOnOff(ON);
                        //
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_CONNECT;
                        stimer=0;
                    }
                    else
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_CONNECT;
                        stimer=0;
                    }
                    break;
                //等待蓝牙连接
                case BLUETOOTH_E_UPLOAD_STEP_CONNECT:
                    if(BLUETOOTH_CONNECT_R)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_CONNECT_DELAY;
                        stimer         =  0;
                    }
                    else if(stimer>=600)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_ERR;
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_BLUETOOTH_CONNECT;
                    }
                    break;
                //连接延时
                case BLUETOOTH_E_UPLOAD_STEP_CONNECT_DELAY:
                    if(stimer>=10)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_HAND;
                    }
                    break;
                //发送握手包
                case BLUETOOTH_E_UPLOAD_STEP_HAND:
                    pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    memset(pbuf,0,20);
                    pbuf[0] = 0x5A;
                    pbuf[1] = 0xA5;
                    pbuf[2] = 20;
                    Bluetooth_Tx(pbuf, 2);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_HAND_WAIT_ACK;
                    stimer=0;
                    break;
                //等待握手应答
                case BLUETOOTH_E_UPLOAD_STEP_HAND_WAIT_ACK:
                    if(len==2)
                    {
                        if(pRxBuf[0]==0x5A&&pRxBuf[1]==0xA5)
                        {
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK1;
                        }
                    }
                    else if(stimer>=30)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_ERR;
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_BLUETOOTH_TRANSFER;
                    }
                    break;
                //发送第一包(非睡眠数据)
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK1:
                    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    {
                        MODULE_MEMORY_S_PARA *pspara;
                        pspara=(MODULE_MEMORY_S_PARA*)pbuf;
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                        snum = pspara->MemoryDayNextP;
                        snum = Count_SubCyc(snum,ssub,MODULE_MEMORY_ADDR_DAY_NUM-1);
                    }
                    {
                        MODULE_MEMORY_S_DAY_INFO *psDayInfo=(MODULE_MEMORY_S_DAY_INFO *)pbuf;
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,pbuf,&snum);
                        if(psDayInfo->BeginByte==0xCC)
                        {
                            Bluetooth_Tx(pbuf, 72);
                            stimer=0;
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK1_DELAY;
                        }
                        else
                        {
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_TX_OK;
                        }
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    break;
                //延时
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK1_DELAY:
                    if(stimer>=8)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK2;
                    }
                    break;
                //发送第二包(睡眠数据)
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK2:
                    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    {
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,pbuf,&snum);
                        Bluetooth_Tx(pbuf, 243);
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    stimer=0;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK2_DELAY;
                    break;
                //延时
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK2_DELAY:
                    if(stimer>=8)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK3;
                    }
                    break;
                //发送第三包(呼吸数据)
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK3:
                    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    {
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_R,pbuf,&snum);
                        Bluetooth_Tx(pbuf, 243);
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    stimer=0;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK3_DELAY;
                    break;
                //延时
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK3_DELAY:
                    if(stimer>=8)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK4;
                    }
                    break;
                //发送第四包(心率数据)
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK4:
                    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    {
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,pbuf,&snum);
                        Bluetooth_Tx(pbuf, 243);
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    stimer=0;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK4_DELAY;
                    break;
                //延时
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK4_DELAY:
                    if(stimer>=8)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK5;
                    }
                    break;
                //发送第五包(体动数据)
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK5:
                    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    {
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,pbuf,&snum);
                        Bluetooth_Tx(pbuf, 243);
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    stimer=0;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_TX_PACK5_WAIT_ACK;
                    break;
                //等待应答
                case BLUETOOTH_E_UPLOAD_STEP_TX_PACK5_WAIT_ACK:
                    if(len==3)
                    {
                        if(pRxBuf[0]==0x5A&&pRxBuf[1]==0x01&&pRxBuf[2]==0xA5)
                        {
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_TX_OK;
                        }
                        else
                        {
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_TX_ERR;
                        }
                    }
                    if(stimer>=50)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_TX_ERR;
                    }
                    break;
                //成功
                case BLUETOOTH_E_UPLOAD_STEP_TX_OK:
                    Bluetooth_ChipOkCount++;
                    if(Bluetooth_Cmd==BLUETOOTH_CMD_UPDATA_1DAY)
                    {
                        Bluetooth_Step =  BLUETOOTH_E_UPLOAD_STEP_POWER_OFF;
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_NULL;
                    }
                    else if(Bluetooth_Cmd==BLUETOOTH_CMD_UPDATA_7DAY)
                    {
                        if(ssub>=7)
                        {
                            ssub  =  0;
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_POWER_OFF;
                            Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_NULL;
                        }
                        else
                        {
                            ssub++;
                            Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_HAND;
                        }
                    }
                    break;
                //失败
                case BLUETOOTH_E_UPLOAD_STEP_TX_ERR:
                    Bluetooth_ChipErrCount++;
                    if(Bluetooth_HciTxRxOkErr==MODULE_E_ERR_NULL)
                    {
                        Bluetooth_HciTxRxOkErr =  MODULE_E_ERR_BLUETOOTH_TRANSFER;
                    }
                    Bluetooth_Step   =  BLUETOOTH_E_UPLOAD_STEP_POWER_OFF;
                    break;
                //蓝牙掉电
                case BLUETOOTH_E_UPLOAD_STEP_POWER_OFF:
                    if(Bluetooth_PowerOnHold_Enable==OFF)
                    {
                        Bluetooth_VisableOnOff(OFF);
                        MODULE_OS_DELAY_MS(1000);
                        Bluetooth_Power(OFF);
                    }
                    Bluetooth_Cmd   =  BLUETOOTH_CMD_NULL;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_POWER_ON;
                    break;
                //其他
                default:
                    Bluetooth_Cmd   =  BLUETOOTH_CMD_NULL;
                    Bluetooth_Step  =  BLUETOOTH_E_UPLOAD_STEP_POWER_ON;
                    break;
            }
            break;
        default:
            break;
    }
    return OK;
}
//------------------------------------------------------------------------------非任务模式
void BluetoothDtu_Init(void)
{
    //硬件初始化
    UART_INIT(BLUETOOTH_UARTX,BLUETOOTH_UART_BPS);
    //变量初始化
    Bluetooth_DebugTest_Enable   =  0;
    Bluetooth_s_info.Type        =  0;
    Bluetooth_s_info.UartTxNum   =  0;
    Bluetooth_s_info.UartRxNum   =  0;
    Count_ModbusSlaveAddr  = BLUETOOTH_DTU_SLAVE_ADDR;
    memset((char*)BluetoothDtu_s_SlaveBuf,0,sizeof(BluetoothDtu_s_SlaveBuf));
}
void BluetoothDtu_100ms(void)
{
    static uint8_t s_count=0;
    static uint8_t step=0;
    uint8_t i;
    uint8_t *pbuf;
    uint16_t *pi16;
    s_count++;
    if(s_count<10)
    {
        return;
    }
    s_count=0;
    // 每秒发送
    switch(step)
    {
        case 0:
            if(Bluetooth_s_info.Type!=0)
            {
                step=1;
                break;
            }
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pi16 = (uint16_t*)&pbuf[200];
            pi16[1]=0x0000;
            pi16[2]=0x0005;
            //
            Count_Modbus_Array(pbuf,&pi16[0],BLUETOOTH_DTU_SLAVE_ADDR,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,&pi16[1],&pi16[2],NULL,NULL,0);
            //
            Bluetooth_s_info.UartTxNum++;
            UART_DMA_Tx(BLUETOOTH_UARTX,pbuf,pi16[0]);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            break;
        case 1:
            //长时间没有数据则发送心跳
            break;
        default:
            step = 0;
            break;
    }
    // 更新秒数
    for(i=0; i<BLUETOOTH_DTU_S_SLAVEBUF_NUM; i++)
    {
        if(BluetoothDtu_s_SlaveBuf[i].state!=0 && BluetoothDtu_s_SlaveBuf[i].timeover_s!=0xFFFF)
        {
            BluetoothDtu_s_SlaveBuf[i].timeover_s++;
        }
    }
}
void BluetoothDtu_RxIrq(uint8_t *pRx,uint16_t len)
{
    uint8_t i,j,k;
    // 规则判定
    i = Count_Modbus_Check(pRx,len,BLUETOOTH_DTU_SLAVE_ADDR);
    if(i==ERR)
    {
        return;
    }
    // 统计
    Bluetooth_s_info.UartRxNum++;
    // 解析功能码
    switch(pRx[1])
    {
        case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
            if(pRx[2]!=10)
            {
                break;
            }
            Bluetooth_s_info.HardwareVer_s   =  pRx[3];
            Bluetooth_s_info.HardwareVer_m   =  pRx[4];
            Bluetooth_s_info.SoftwareVer_s   =  pRx[5];
            Bluetooth_s_info.SoftwareVer_m   =  pRx[6];
            Bluetooth_s_info.MacBuf[0]       =  pRx[7];
            Bluetooth_s_info.MacBuf[1]       =  pRx[8];
            Bluetooth_s_info.MacBuf[2]       =  pRx[9];
            Bluetooth_s_info.MacBuf[3]       =  pRx[10];
            Bluetooth_s_info.MacBuf[4]       =  pRx[11];
            Bluetooth_s_info.MacBuf[5]       =  pRx[12];
            Bluetooth_s_info.Type            =  1;
            break;
        case COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            break;
        case COUNT_MODBUS_FC_UPLOAD:
            // 查看有无
            for(i=0,j=k=0xFF; i<BLUETOOTH_DTU_S_SLAVEBUF_NUM; i++)
            {
                // 搜索位置
                if(pRx[3]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[0]\
                   &&pRx[4]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[1]\
                   &&pRx[5]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[2]\
                   &&pRx[6]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[3]\
                   &&pRx[7]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[4]\
                   &&pRx[8]==BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[5])
                {
                    break;
                }
                // 搜索空位
                if(j==0xFF && BluetoothDtu_s_SlaveBuf[i].state==0)
                {
                    j=i;
                }
                // 记录最无意义项
                if(BluetoothDtu_s_SlaveBuf[i].state==1 && BluetoothDtu_s_SlaveBuf[i].Type==0)
                {
                    if(k==0xFF)
                    {
                        k=i;
                    }
                    else
                    {
                        if(BluetoothDtu_s_SlaveBuf[k].timeover_s < BluetoothDtu_s_SlaveBuf[i].timeover_s)
                        {
                            k=i;
                        }
                    }
                }
            }
            // i比j优先(匹配优先)
            if(i<BLUETOOTH_DTU_S_SLAVEBUF_NUM)
            {
                j=i;
            }
            // 空位优先，其次是删除未识别的最长断开项
            else if(j==0xFF && k!=0xFF)
            {
                j=k;
                memset((char*)&BluetoothDtu_s_SlaveBuf[k],0,sizeof(BLUETOOTH_S_SLAVE));
            }
            // 容错
            if(j>=BLUETOOTH_DTU_S_SLAVEBUF_NUM)
            {
                break;
            }
            // 赋值新数据
            BluetoothDtu_s_SlaveBuf[j].state=1;
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[0]=pRx[3];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[1]=pRx[4];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[2]=pRx[5];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[3]=pRx[6];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[4]=pRx[7];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.peer_addr.addr[5]=pRx[8];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.rssi             =pRx[9];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.scan_rsp         =pRx[10];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.type             =pRx[11];
            BluetoothDtu_s_SlaveBuf[j].AdvReport.dlen             =pRx[12];
            memcpy(BluetoothDtu_s_SlaveBuf[j].AdvReport.data,&pRx[13],31);
            BluetoothDtu_s_SlaveBuf[j].state=2;
            BluetoothDtu_s_SlaveBuf[j].timeover_s=0;
            if(BluetoothDtu_s_SlaveBuf[j].AdvReport.scan_rsp==0\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.type==BLE_GAP_ADV_TYPE_ADV_NONCONN_IND\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.dlen==BSP_NRF_BLE_BEACON_PROTOCOL_LEN\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_LEN1]==0x02\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE1]==0x01\
               &&(BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE1]==0x04||BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE1]==0x06)\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_LEN2]==0x1A\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE2]==0xFF\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_L]==BSP_NRF_BLE_BEACON_PROTOCOL_COMPANY_L\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H]==BSP_NRF_BLE_BEACON_PROTOCOL_COMPANY_H\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_SN]==0x02\
               &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_LEN]==0x15\
              )
            {
                BluetoothDtu_s_SlaveBuf[j].Type=BSP_NRF_BLE_BEACON_PROTOCOL_TYPE;
            }
            else if(BluetoothDtu_s_SlaveBuf[j].AdvReport.scan_rsp==0\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.type==BLE_GAP_ADV_TYPE_ADV_NONCONN_IND\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.dlen==BSP_NRF_BLE_FACTORY_PROTOCOL_LEN\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_LEN1]==0x02\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE1]==0x01\
                    &&(BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE1]==0x04||BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE1]==0x06)\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_LEN2]==0x1A\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE2]==0xFF\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_L]==BSP_NRF_BLE_FACTORY_PROTOCOL_COMPANY_L\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H]==BSP_NRF_BLE_FACTORY_PROTOCOL_COMPANY_H\
                   )
            {
                if(BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_LEN]==BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_LEN\
                   &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_TYPE]==BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_TYPE)
                {
                    BluetoothDtu_s_SlaveBuf[j].Type=BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_TYPE;
                }
                else if(BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_LEN]==BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_LEN\
                        &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_TYPE]==BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE)
                {
                    BluetoothDtu_s_SlaveBuf[j].Type=BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE;
                }
                else if(BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_LEN]==BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_LEN\
                        &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_TYPE]==BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE)
                {
                    BluetoothDtu_s_SlaveBuf[j].Type=BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE;
                }
                else
                {
                    BluetoothDtu_s_SlaveBuf[j].Type=0;
                }
            }
            else if(BluetoothDtu_s_SlaveBuf[j].AdvReport.dlen==BSP_NRF_BLE_HUAMI_PROTOCOL_LEN\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_L]==BSP_NRF_BLE_HUAMI_PROTOCOL_COMPANY_L\
                    &&BluetoothDtu_s_SlaveBuf[j].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H]==BSP_NRF_BLE_HUAMI_PROTOCOL_COMPANY_H)
            {
                BluetoothDtu_s_SlaveBuf[j].Type=BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_TYPE;
            }

            else
            {
                //不能归0
                //BluetoothDtu_s_SlaveBuf[j].Type=0;
            }
            break;
        default:
            break;
    }
}
/*******************************************************************************
函数功能: Debug应用测试函数
*******************************************************************************/
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
//------------------------------------------------------------------------------
