/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2014-05-29
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#define SPI_MASTER_GLOBAL
#include "uctsk_SpiMaster.h"
#include "uctsk_SpiSlave.h"
//----------------------------------------------------------------------------宏定义
//获取数组成员数量
#define Spi_MasterArraySize(a) (sizeof(a) / sizeof((a)[0]))
//----------------------------------------------------------------------------静态全局变量
#ifdef OS_LINUX
static const char *Spi_MasterDevice = "/dev/spidev2.0";
static uint8_t Spi_MasterMode;
static uint8_t Spi_MasterBits = 8;
static uint32_t Spi_MasterSpeed = 10000000;
static uint16_t Spi_MasterDelay = 10;
static int Spi_Master_fd;
pthread_mutex_t Mutex_SpiMasterWriteRead;
#else
//系统互斥信号量定义
MODULE_OS_SEM(Mutex_SpiMasterWriteRead);
#endif
//
static uint8_t Spi_MasterIntSign=0;
static SPI_SLAVE_S_STATE s_SpiMaster_SlaveState;
//----------------------------------------------------------------------------函数
/*******************************************************************************
函数功能: SPI主机出错输出并终止程序
参    数: 输出的字符串
返 回 值:
*******************************************************************************/
#ifdef OS_LINUX
static void Spi_MasterAbortDebugOut(const char *s)
{
    perror(s);
    abort();
}
#endif
static void *Spi_MasterMemGet(uint16_t len,uint8_t *res)
{
    len=len;
    res=res;
#ifdef OS_LINUX
    return malloc(len);
#else
    return MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#endif
}
static void Spi_MasterMemFree(void *pBuf,uint8_t *res)
{
    pBuf=pBuf;
    res=res;
#ifdef OS_LINUX
    free(pBuf);
#else
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pBuf);
#endif
}
static void Spi_MasterPrintf(uint8_t *pBuf)
{
#ifdef SPI_MASTER_DEBUG_ENABLE
#ifdef OS_LINUX
    printf(pBuf);
#else
    DebugOut((int8_t*)pBuf,strlen((char*)pBuf));
#endif
#endif
}
static void Spi_MasterDisplay(uint8_t ch,uint8_t *buf,uint16_t len)
{
#ifdef SPI_MASTER_DEBUG_ENABLE
#ifdef OS_LINUX
    uint16_t i16;
    for (i16 = 0; i16 < len; i16++)
    {
        if (!(i16 % 20))         puts("");
        printf("%.2X ", buf[i16]);
    }
    puts("");
#else
    uint16_t i16;
    uint8_t buf1[5];
    if(ch==0)
    {
        for (i16 = 0; i16 < len; i16++)
        {
            if (!(i16 % 20))
            {
                DebugOut("\r\n",2);
            }
            sprintf((char*)buf1,"%.2X ",buf[i16]);
            DebugOut((int8_t*)buf1,strlen((char*)buf1));
        }
    }
    else
    {
        UART_DMA_Tx(ch,buf,len);
    }
#endif
#endif
    ch=ch;
    buf=buf;
    len=len;
}
/*******************************************************************************
函数功能: SPI(1)主机初始化
参    数:
返 回 值:
*******************************************************************************/
static void Spi_MasterInit(void)
{
#ifdef OS_LINUX

#else
    //GPIO
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	  memset((char*)&SPI_InitStructure,0,sizeof(SPI_InitTypeDef));
	  memset((char*)&GPIO_InitStructure,0,sizeof(GPIO_InitTypeDef));
    //EXTI_InitTypeDef EXTI_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
#if (SPI_MASTER_SPI_CH==2)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    //CLK MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //INT(中断)
    /*
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    //中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
#elif (SPI_MASTER_SPI_CH==3)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    //禁用JTAG多余引脚(注意: 单独放在主函数不好使,不知原因)
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //CS
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    SPI_MASTER_CS_HIGH();
    /*
    //INT(中断)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    //中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    */
#endif
    //SPI1 Config
    SPI_Cmd(SPI_MASTER_SPIx, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    //时钟极性(SPI_CPOL_Low低电平空闲  SPI_CPOL_High高电平空闲)
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    //时钟采样(SPI_CPHA_1Edge第一个跳变沿采样,SPI_CPHA_2Edge第二个跳变沿采样)
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	 //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    //首位(SPI_FirstBit_MSB高位在前,SPI_FirstBit_LSB低位在前)
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_CalculateCRC( SPI_MASTER_SPIx,  DISABLE);
    SPI_Init(SPI_MASTER_SPIx, &SPI_InitStructure);
    //Enable SPI1
    SPI_Cmd(SPI_MASTER_SPIx, ENABLE);
#endif
}
/*******************************************************************************
* 函数功能: SPI主机通过硬件SPI发送/接收一个字节
* 参    数: 发送数据
* 返 回 值: 读取的数据
*******************************************************************************/
#ifdef OS_LINUX
#else
static uint8_t Spi_MasterReadWriteByte(uint8_t data)
{
    //Loop while DR register in not emplty
    while (SPI_I2S_GetFlagStatus(SPI_MASTER_SPIx, SPI_I2S_FLAG_TXE) == RESET);
    //Send byte through the SPI1 peripheral
    SPI_I2S_SendData(SPI_MASTER_SPIx, data);
    //Wait to receive a byte
    while (SPI_I2S_GetFlagStatus(SPI_MASTER_SPIx, SPI_I2S_FLAG_RXNE) == RESET);
    //Return the byte read from the SPI bus
    return SPI_I2S_ReceiveData(SPI_MASTER_SPIx);
}
#endif
/*******************************************************************************
* 函数功能: SPI主机写函数
* 参    数: Dst      ---   目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）
*           SndbufPt ---   发送缓存区指针
*           NByte    ---   要写的数据字节数
* 返 回 值: 操作成功则返回OK,失败则返回ERR
*******************************************************************************/
static uint8_t Spi_MasterReadWriteBuf(uint8_t* pWriteBuf,uint8_t* pReadBuf,uint32_t NByte)
{
    uint32_t i = 0;
    SPI_MASTER_CS_LOW();
    //发送字节数据烧写命令
#ifdef OS_LINUX
    int ret;
    struct spi_ioc_transfer tr =
    {
        .tx_buf = (unsigned long)pWriteBuf,
        .rx_buf = (unsigned long)pReadBuf,
        .len = NByte,
        .delay_usecs = Spi_MasterDelay,
        .speed_hz = Spi_MasterSpeed,
        .bits_per_word = Spi_MasterBits,
    };
    //检查入参
    if (NByte == 0)
    {
        return (ERR);
    }
    ret = ioctl(Spi_Master_fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        Spi_MasterAbortDebugOut("can't send spi message");
    }
    usleep(1000*10);
#else
    //检查入参
    if (NByte == 0)
    {
        return (ERR);
    }
    if(pReadBuf==NULL)
    {
        for(i = 0; i < NByte; i++)
        {
            //发送被烧写的数据
            Spi_MasterReadWriteByte(pWriteBuf[i]);
        }
    }
    else
    {
        for(i = 0; i < NByte; i++)
        {
            //发送被烧写的数据
            pReadBuf[i]=Spi_MasterReadWriteByte(pWriteBuf[i]);
        }
    }
    MODULE_OS_DELAY_MS(10);
#endif
    SPI_MASTER_CS_HIGH();
    //
    return (OK);
}
/*******************************************************************************
* 函数功能: SPI从机片选(外中断)调用函数
* 参    数:
* Return  :
*******************************************************************************/
void Spi_MasterIntExtIRQHandler(void)
{
    Spi_MasterIntSign=1;
}
/*******************************************************************************
* 函数功能: 应用函数
* 参    数:
* Return  :
*******************************************************************************/
uint8_t Spi_MasterApp(uint8_t SlaveAddr,uint8_t cmd,uint16_t regAddr,void *pBuf,uint16_t *pregByteLen)
{
    uint8_t *pwbuf;
    uint8_t *prbuf;
    uint16_t len;
    uint16_t reg_Addr,reg_WordLen;
    uint8_t reg_ByteLen;
    uint8_t res;
    uint8_t ret=OK;
#ifdef OS_LINUX
    pthread_mutex_lock(&Mutex_SpiMasterWriteRead);
#else
    MODULE_OS_ERR err;
    err=err;
    MODULE_OS_SEM_PEND(Mutex_SpiMasterWriteRead,1,FALSE,err);
#endif
    //申请缓存
    pwbuf=Spi_MasterMemGet(256,&res);
    prbuf=Spi_MasterMemGet(256,&res);
    //
    switch(cmd)
    {
        case COUNT_MODBUS_FC_READ_COILS:
            break;
        case COUNT_MODBUS_FC_READ_DISCRETE_INPUTS:
            break;
        case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
            switch(regAddr)
            {
                case COUNT_MODBUS_REGISTERS_ADDR_STATE:
                case COUNT_MODBUS_REGISTERS_ADDR_PARA:
                case COUNT_MODBUS_REGISTERS_ADDR_RTC:
                case COUNT_MODBUS_REGISTERS_ADDR_AD:
                case COUNT_MODBUS_REGISTERS_ADDR_KIN:
                case COUNT_MODBUS_REGISTERS_ADDR_KOUT:
                    reg_WordLen =  (*pregByteLen)/2+(*pregByteLen)%2;
                    Count_Modbus_Array(pwbuf,&len,SlaveAddr,cmd,&regAddr,&reg_WordLen,NULL,NULL,NULL);
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,len);
                    //读取
                    pwbuf[0]=0;
                    len=5+(*pregByteLen);
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,1+len);
                    //验证数据
                    ret=Count_Modbus_Check(&prbuf[1],len,0xFF);
                    if(OK==ret)
                    {
                        //复制数据
                        memcpy((char*)pBuf,&prbuf[4],*pregByteLen);
                    }
                    break;
                case COUNT_MODBUS_REGISTERS_ADDR_UART1:
                case COUNT_MODBUS_REGISTERS_ADDR_UART2:
                case COUNT_MODBUS_REGISTERS_ADDR_UART3:
                case COUNT_MODBUS_REGISTERS_ADDR_UART4:
                case COUNT_MODBUS_REGISTERS_ADDR_UART5:
                    //获取长度
                    reg_Addr=COUNT_MODBUS_REGISTERS_ADDR_STATE;
                    reg_WordLen =  COUNT_MODBUS_REGISTERS_LEN_STATE/2+COUNT_MODBUS_REGISTERS_LEN_STATE%2;
                    Count_Modbus_Array(pwbuf,&len,SlaveAddr,cmd,&reg_Addr,&reg_WordLen,NULL,NULL,NULL);
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,len);
                    //读取
                    pwbuf[0]=0;
                    len=5+COUNT_MODBUS_REGISTERS_LEN_STATE;
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,1+len);
                    //验证数据
                    ret=Count_Modbus_Check(&prbuf[1],len,0xFF);
                    if(OK!=ret)
                    {
                        *pregByteLen=0;
                        break;
                    }
                    memcpy(&s_SpiMaster_SlaveState,&prbuf[4],COUNT_MODBUS_REGISTERS_LEN_STATE);
                    *pregByteLen=s_SpiMaster_SlaveState.UartRxSize[(regAddr-COUNT_MODBUS_REGISTERS_ADDR_UART1)/COUNT_MODBUS_REGISTERS_SIZE_UART];
                    if((*pregByteLen)!=0)
                    {
                        *pregByteLen+=2;
                        reg_WordLen =  (*pregByteLen)/2+(*pregByteLen)%2;
                        Count_Modbus_Array(pwbuf,&len,SlaveAddr,cmd,&regAddr,&reg_WordLen,NULL,NULL,NULL);
                        Spi_MasterReadWriteBuf(pwbuf,prbuf,len);
                        //读取
                        pwbuf[0]=0;
                        len=5+(*pregByteLen);
                        Spi_MasterReadWriteBuf(pwbuf,prbuf,1+len);
                        //验证数据
                        ret=Count_Modbus_Check(&prbuf[1],len,0xFF);
                        if(OK==ret)
                        {
                            //复制数据
                            memcpy((char*)pBuf,&prbuf[4],*pregByteLen);
                        }
                        else
                        {
                            *pregByteLen=0;
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case COUNT_MODBUS_FC_READ_INPUT_REGISTERS:
            break;
        case COUNT_MODBUS_FC_WRITE_SINGLE_COIL:
            break;
        case COUNT_MODBUS_FC_WRITE_SINGLE_REGISTER:
            break;
        case COUNT_MODBUS_FC_READ_EXCEPTION_STATUS:
            break;
        case COUNT_MODBUS_FC_WRITE_MULTIPLE_COILS:
            break;
        case COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            switch(regAddr)
            {
                case COUNT_MODBUS_REGISTERS_ADDR_PARA:
                case COUNT_MODBUS_REGISTERS_ADDR_CONTROL:
                case COUNT_MODBUS_REGISTERS_ADDR_RTC:
                case COUNT_MODBUS_REGISTERS_ADDR_KOUT:
                case COUNT_MODBUS_REGISTERS_ADDR_UART1:
                case COUNT_MODBUS_REGISTERS_ADDR_UART2:
                case COUNT_MODBUS_REGISTERS_ADDR_UART3:
                case COUNT_MODBUS_REGISTERS_ADDR_UART4:
                case COUNT_MODBUS_REGISTERS_ADDR_UART5:
                    //写操作
                    reg_WordLen =  (*pregByteLen)/2+(*pregByteLen)%2;
                    reg_ByteLen =  *pregByteLen;
                    Count_Modbus_Array(pwbuf,&len,SlaveAddr,cmd,&regAddr,&reg_WordLen,&reg_ByteLen,pBuf,*pregByteLen);
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,len);
                    //读响应
                    pwbuf[0]=0;
                    len=8;
                    Spi_MasterReadWriteBuf(pwbuf,prbuf,1+len);
                    //验证数据
                    ret=Count_Modbus_Check(&prbuf[1],len,0xFF);
                    if(OK!=ret)
                    {
                        //复制数据
                        ret=ERR;
                    }
                    break;
                default:
                    break;
            }
            break;
        case COUNT_MODBUS_FC_REPORT_SLAVE_ID:
            break;
        case COUNT_MODBUS_FC_MASK_WRITE_REGISTER:
            break;
        case COUNT_MODBUS_FC_WRITE_AND_READ_REGISTERS:
            break;
        default:
            break;
    }
    //释放缓存
    Spi_MasterMemFree(prbuf,&res);
    Spi_MasterMemFree(pwbuf,&res);
#ifdef OS_LINUX
    pthread_mutex_unlock(&Mutex_SpiMasterWriteRead);
#else
    MODULE_OS_SEM_POST(Mutex_SpiMasterWriteRead);
#endif
    return ret;
}
/*******************************************************************************
* 函数功能: SPI主从通讯测试
* 参    数:
* Return  :
*******************************************************************************/
void SpiMaster_DebugTestOnOff(uint8_t OnOff)
{
    static uint8_t first=1;
    uint8_t res=0;
    uint8_t *pprintfbuf=NULL;
    uint16_t len=0;
    static uint8_t timer=0;
    static uint8_t setTime=1;
    MODULE_OS_ERR err;
    //
    err=err;
    //
    if(first==1)
    {
        first=0;
        Spi_MasterInit();
        //
#ifdef  OS_LINUX
        pthread_mutex_init(&Mutex_SpiMasterWriteRead,NULL);
#endif
#ifdef  OS_UCOSIII
        MODULE_OS_SEM_CREATE(Mutex_SpiMasterWriteRead,"Mutex_SpiMasterWriteRead",1);
#endif
        //
        return;
    }
    if(OnOff==ON)
    {
        //Spi_MasterPrintf("SpiMS-Test-Begin\r\n");
    }
    else
    {
        Spi_MasterPrintf("SpiMS-Test-End\r\n");
        return;
    }
    //申请缓存
    pprintfbuf=Spi_MasterMemGet(256,&res);
    //测试串口接收(另测试读取状态)
    if(0)
    {
        uint8_t i,j;
//#ifdef OS_LINUX
        Spi_MasterIntSign=1;
//#endif
        if(Spi_MasterIntSign==1)
        {
            Spi_MasterIntSign=0;
            //
            for(i=0; i<SPI_SLAVE_MAX_UART; i++)
            {
                Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART1+(i*COUNT_MODBUS_REGISTERS_SIZE_UART),(uint8_t*)pprintfbuf,&len);
                //回发操作
                if(len>2)
                {
                    Spi_MasterPrintf("\r\nRead Uart:");
                    j=i+1;
                    Spi_MasterDisplay(0,&j,1);
                    Spi_MasterDisplay(0,&pprintfbuf[0],len);
                }
            }
        }
    }
    //
    timer++;
    if(timer<10)
    {
        goto goto_SpiTest;
    }
    else
    {
        timer=0;
    }
    //测试时钟写
    if(0 && setTime)
    {
        struct tm* ptm;
        //
        setTime=0;
        //主机读取时钟
        //2015-01-02 03:04:05
        ptm=Spi_MasterMemGet(256,&res);
        //
        ptm->tm_year  =  2015;
        ptm->tm_mon   =  7-1;
        ptm->tm_mday  =  20;
        ptm->tm_hour  =  3;
        ptm->tm_min   =  4;
        ptm->tm_sec   =  5;
        len=COUNT_MODBUS_REGISTERS_LEN_RTC;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_RTC,ptm,&len);
        Spi_MasterMemFree(ptm,&res);
    }
    //测试重启动
    if(0)
    {
        SPI_SLAVE_S_CONTROL *p_S_spiSlaveControl;
        p_S_spiSlaveControl=Spi_MasterMemGet(256,&res);
        p_S_spiSlaveControl->SysReset=0x0001;
        len=COUNT_MODBUS_REGISTERS_LEN_CONTROL;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_CONTROL,p_S_spiSlaveControl,&len);
        Spi_MasterMemFree(p_S_spiSlaveControl,&res);
    }
    //测试配置参数写(将串口5改为4800)
    if(0)
    {
        U_IAP_PARA* p_uIapPara;
        p_uIapPara=(U_IAP_PARA*)pprintfbuf;
        len=COUNT_MODBUS_REGISTERS_LEN_PARA;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_PARA,(uint8_t*)pprintfbuf,&len);
        if(p_uIapPara->data.UartSetBuf[4].Bsp!=6)
        {
            p_uIapPara->data.UartSetBuf[4].Bsp=6;
            Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_PARA,(uint8_t*)pprintfbuf,&len);
            Spi_MasterPrintf("\r\nCom5->9600");
            //Spi_MasterApp(SPI_E_SLAVE_STEP_CMD_W_RESET,NULL);
        }
    }
    //测试配置参数读
    if(0)
    {
        Spi_MasterPrintf("\r\nRead Para:");
        len=COUNT_MODBUS_REGISTERS_LEN_PARA;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_PARA,(uint8_t*)pprintfbuf,&len);
        //回发操作
        if(pprintfbuf[0]!=0)
        {
            Spi_MasterDisplay(0,&pprintfbuf[0],sizeof(U_IAP_PARA));
        }
    }
#ifdef SPI_SLAVE_EXTEND_S_RTC_EN
    //测试时钟读
    if(1)
    {
        struct tm* ptm;
        //
        ptm=(struct tm*)&pprintfbuf[128];
			  memset((char*)ptm,0,sizeof(struct tm));
        len=COUNT_MODBUS_REGISTERS_LEN_RTC;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_RTC,ptm,&len);
        sprintf((char*)pprintfbuf,"\r\nRead RTC: %04d-%02d-%02d %02d:%02d:%02d(%d)",
                ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,ptm->tm_wday);
        Spi_MasterPrintf(pprintfbuf);
    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_KIN_EN
    //测试开关量输入
    if(1)
    {
        uint16_t i16;
        len=COUNT_MODBUS_REGISTERS_LEN_KIN;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_KIN,(uint8_t*)&i16,&len);
        sprintf((char*)pprintfbuf,"\r\nRead Kin: 0x%04x",i16);
        Spi_MasterPrintf(pprintfbuf);
    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_KOUT_EN
    //测试开关量输出
    if(1)
    {
        static uint8_t timer=0;
        uint16_t i16;
        timer++;
        if(timer==1)
        {
            i16=0x0001;
            len=COUNT_MODBUS_REGISTERS_LEN_KOUT;
            Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_KOUT,&i16,&len);
        }
        else if(timer>=2)
        {
            timer=0;
            i16=0x0000;
            len=COUNT_MODBUS_REGISTERS_LEN_KOUT;
            Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_KOUT,&i16,&len);
        }
    }
    //测试读开关量输出
    if(1)
    {
        uint16_t i16;
        len=COUNT_MODBUS_REGISTERS_LEN_KOUT;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_KOUT,(uint8_t*)&i16,&len);
        sprintf((char*)pprintfbuf,"\r\nRead Ko : 0x%04x",i16);
        Spi_MasterPrintf(pprintfbuf);
    }
#endif
#ifdef SPI_SLAVE_EXTEND_S_AD_EN
    //测试AD
    if(1)
    {
        len=COUNT_MODBUS_REGISTERS_LEN_AD;
        Spi_MasterApp(1,COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_AD,&pprintfbuf[128],&len);
        sprintf((char*)pprintfbuf,"\r\nRead AD : %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d",
                pprintfbuf[128+0]*256+pprintfbuf[128+1],
                pprintfbuf[128+2]*256+pprintfbuf[128+3],
                pprintfbuf[128+4]*256+pprintfbuf[128+5],
                pprintfbuf[128+6]*256+pprintfbuf[128+7],
                pprintfbuf[128+8]*256+pprintfbuf[128+9],
                pprintfbuf[128+10]*256+pprintfbuf[128+11],
                pprintfbuf[128+12]*256+pprintfbuf[128+13],
                pprintfbuf[128+14]*256+pprintfbuf[128+15],
                pprintfbuf[128+16]*256+pprintfbuf[128+17],
                pprintfbuf[128+18]*256+pprintfbuf[128+19],
                pprintfbuf[128+20]*256+pprintfbuf[128+21],
                pprintfbuf[128+22]*256+pprintfbuf[128+23]);
        Spi_MasterPrintf(pprintfbuf);
    }
#endif
    //测试串口发送
    if(1)
    {
        uint16_t *pi16;
        pi16=(uint16_t*)&pprintfbuf[0];
#ifdef SPI_SLAVE_EXTEND_S_UART1_EN
        //串口1
        *pi16=strlen("SPI-Uart1-Tx\r\n");
        memcpy(&pprintfbuf[2],"SPI-Uart1-Tx\r\n",pprintfbuf[0]);
        len=*pi16+2;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART1,pprintfbuf,&len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART2_EN
        //串口2
        *pi16=strlen("SPI-Uart2-Tx\r\n");
        memcpy(&pprintfbuf[2],"SPI-Uart2-Tx\r\n",pprintfbuf[0]);
        len=*pi16+2;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART2,pprintfbuf,&len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART3_EN
        //串口3
        *pi16=strlen("SPI-Uart3-Tx\r\n");
        memcpy(&pprintfbuf[2],"SPI-Uart3-Tx\r\n",pprintfbuf[0]);
        len=*pi16+2;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART3,pprintfbuf,&len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART4_EN
        //串口4
        *pi16=strlen("SPI-Uart4-Tx\r\n");
        memcpy(&pprintfbuf[2],"SPI-Uart4-Tx\r\n",pprintfbuf[0]);
        len=*pi16+2;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART4,pprintfbuf,&len);
#endif
#ifdef SPI_SLAVE_EXTEND_S_UART5_EN
        //串口5
        *pi16=strlen("SPI-Uart5-Tx\r\n");
        memcpy(&pprintfbuf[2],"SPI-Uart5-Tx\r\n",pprintfbuf[0]);
        len=*pi16+2;
        Spi_MasterApp(1,COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS,COUNT_MODBUS_REGISTERS_ADDR_UART5,pprintfbuf,&len);
#endif
    }
    res=res;
goto_SpiTest:
    //释放缓存
    Spi_MasterMemFree(pprintfbuf,&res);
}
/*******************************************************************************
* 函数功能: linux主函数
* 参    数:
* Return  :
*******************************************************************************/
#ifdef OS_LINUX
static void print_usage(const char *prog)
{
    printf("Usage: %s [-DsbdlHOLC3]\n", prog);
    puts("  -D --device   device to use (default /dev/spidev1.1)\n"
         "  -s --speed    max speed (Hz)\n"
         "  -d --delay    delay (usec)\n"
         "  -b --bpw      bits per word \n"
         "  -l --loop     loopback\n"
         "  -H --cpha     clock phase\n"
         "  -O --cpol     clock polarity\n"
         "  -L --lsb      least significant bit first\n"
         "  -C --cs-high  chip select active high\n"
         "  -3 --3wire    SI/SO signals shared\n");
    exit(1);
}
static void parse_opts(int argc, char *argv[])
{
    while (1)
    {
        /*************************
        结构名称:    option{const char *name;int has_arg;int *flag;nt val;}
        结构说明:    选项结构体
        成    员:    1-->名称
                     2-->以下三种值
                        0-->不跟参数
                        1-->一定要有参数
                        2-->可以有也可以没有参数
                        3-->用来决定，getopt_long()的返回值到底是什么。如果flag是null，则函数会返回与该项option匹配的val值
                        4-->和flag联合决定返回值
        *************************/
        static const struct option lopts[] =
        {
            { "device",  1, 0, 'D' },
            { "speed",   1, 0, 's' },
            { "delay",   1, 0, 'd' },
            { "bpw",     1, 0, 'b' },
            { "loop",    0, 0, 'l' },
            { "cpha",    0, 0, 'H' },
            { "cpol",    0, 0, 'O' },
            { "lsb",     0, 0, 'L' },
            { "cs-high", 0, 0, 'C' },
            { "3wire",   0, 0, '3' },
            { "no-cs",   0, 0, 'N' },
            { "ready",   0, 0, 'R' },
            { NULL, 0, 0, 0 },
        };
        int c;
        /*************************
        函数名称:    getopt_long
        函数说明:    解析字符串为单个字符
        参    数:    1-->通常主函数传递
                     2-->通常主函数传递
                     3-->选项参数组成的字符串
                        说明1-->单个字符表示选项
                        说明2-->单个字符后跟冒号表示必须跟一个参数,参数紧跟选项后或以空格隔开.该参数指针赋值给optarg
                        说明3-->单个字符后跟冒号表示必须跟一个参数,参数紧跟选项后或不能以空格隔开.该参数指针赋值给optarg
                     4-->
                     5-->
        *************************/
        c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);
        if (c == -1)         break;
        switch (c)
        {
            case 'D':
                Spi_MasterDevice = optarg;
                break;
            case 's':
                Spi_MasterSpeed = atoi(optarg);
                break;
            case 'd':
                Spi_MasterDelay = atoi(optarg);
                break;
            case 'b':
                Spi_MasterBits = atoi(optarg);
                break;
            case 'l':
                Spi_MasterMode |= SPI_LOOP;
                break;
            case 'H':
                Spi_MasterMode |= SPI_CPHA;
                break;
            case 'O':
                Spi_MasterMode |= SPI_CPOL;
                break;
            case 'L':
                Spi_MasterMode |= SPI_LSB_FIRST;
                break;
            case 'C':
                Spi_MasterMode |= SPI_CS_HIGH;
                break;
            case '3':
                Spi_MasterMode |= SPI_3WIRE;
                break;
            case 'N':
                Spi_MasterMode |= SPI_NO_CS;
                break;
            case 'R':
                Spi_MasterMode |= SPI_READY;
                break;
            default:
                print_usage(argv[0]);
                break;
        }
    }
}
int main(int argc, char *argv[])
{
    int ret = 0;
    printf("spi_master Begin:\n");
    parse_opts(argc, argv);
    Spi_Master_fd = open(Spi_MasterDevice, O_RDWR);
    if (Spi_Master_fd < 0)
        Spi_MasterAbortDebugOut("can't open device");
    //spi mode
    ret = ioctl(Spi_Master_fd, SPI_IOC_WR_MODE, &Spi_MasterMode);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't set spi mode");
    ret = ioctl(Spi_Master_fd, SPI_IOC_RD_MODE, &Spi_MasterMode);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't get spi mode");
    //bits per word
    ret = ioctl(Spi_Master_fd, SPI_IOC_WR_BITS_PER_WORD, &Spi_MasterBits);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't set bits per word");
    ret = ioctl(Spi_Master_fd, SPI_IOC_RD_BITS_PER_WORD, &Spi_MasterBits);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't get bits per word");
    //max speed hz
    ret = ioctl(Spi_Master_fd, SPI_IOC_WR_MAX_SPEED_HZ, &Spi_MasterSpeed);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't set max speed hz");
    ret = ioctl(Spi_Master_fd, SPI_IOC_RD_MAX_SPEED_HZ, &Spi_MasterSpeed);
    if (ret == -1)
        Spi_MasterAbortDebugOut("can't get max speed hz");
    printf("spi mode: %d\n", Spi_MasterMode);
    printf("bits per word: %d\n", Spi_MasterBits);
    printf("max speed: %d Hz (%d KHz)\n", Spi_MasterSpeed, Spi_MasterSpeed/1000);
    while(1)
    {
        usleep(1000*100);
        SpiMaster_DebugTestOnOff(ON);
    }
    close(Spi_Master_fd);
}
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
