/******************************** 说明 ************************
文件功能：TCI服务器
功    能: 用于TCI对设备的管理
说    明: 1,256k芯片内部FLASH分配:0-15(bootload),16-17(主参数区),18-19(更改参数区),20-137(主程序区),138-255(升级程序区)
          2,TCI通讯协议见PC程序
**************************************************************/
//------------------------------- Includes --------------------
#include <includes.h>
//-------------------------------------------------------------宏定义(配置)
//-----GPRS连接通道
#define TCI_CONNECT_CH       0
#define UPDATA_FLASH_ADDR      0x08000000+138*1024
//-----版本定义
#define HARD_VERSION         "000"
#define SOFT_VERSION         "00"
//-----端口数量信息
#define EQUIPMENT_KIN_NUM        4//开关量输入
#define EQUIPMENT_VIN_NUM        2//电压输入
#define EQUIPMENT_IIN_NUM        2//电流输入
#define EQUIPMENT_FIN_NUM        2//频率输入
#define EQUIPMENT_KOUT_NUM   4//开关量输出
#define EQUIPMENT_VOUT_NUM   2//电压输出
#define EQUIPMENT_IOUT_NUM   2//电流输出
#define EQUIPMENT_FOUT_NUM   2//频率输出
//------------------------------- 用户变量 --------------------
static  OS_STK      App_TaskTciServerStk[APP_TASK_TCISERVER_STK_SIZE];
//标准结构体(用于发送消息队列)
GSM_STRUCT1 Tci_s1;
//TCI专用变量结构体
struct
{
    INT8U  TciNetWatchCount;//TCI网络监控计数器(心跳+1,10次无回应重新连接)
    INT8U  UpdataDownTimer; //保证远程升级收到了数据再要数据(5秒超时)
    INT16U UpdataPacketNow; //升级当前升级包(0无效,1开始)
    INT16U UpdataPacketSum; //升级总包数
    INT8U  TxBuf[100];          //发送缓存
    INT8U  RxBuf[1100];         //接收缓存

    INT8U  KinBuf[EQUIPMENT_KIN_NUM];       //0-断开 1-闭合
    INT16U VinBuf[EQUIPMENT_VIN_NUM];     //mV
    INT16U IinBuf[EQUIPMENT_IIN_NUM];     //uA
    INT32U FinBuf[EQUIPMENT_FIN_NUM];     //Hz
    INT16U KoutBuf[EQUIPMENT_KOUT_NUM];   //0-断开 1-闭合
    INT16U VoutBuf[EQUIPMENT_VOUT_NUM];   //mV
    INT16U IoutBuf[EQUIPMENT_IOUT_NUM];   //uA
    INT32U FoutBuf[EQUIPMENT_FOUT_NUM];   //Hz
} TCI_Variable;
//------------------------------- 外部变量 --------------------
extern INT32U RelayBuf[4];
//------------------------------- 用户函数声明 ----------------
static void uctsk_TciServer (void) ;
static void Tci_Heart(void);
static void Tci_UpdataAck(void);
static void Tci_GetUpdataData(void);
static void Tci_GetPortAck(INT8U *p);
static void Tci_ControlPortAck(INT8U *p);
void TCIServer_Parse(INT8U *buf,INT16U len);
//-------------------------------------------------------------WrServer任务创建
void  App_TciServerTaskCreate (void)
{
    CPU_INT08U  os_err;
    os_err = os_err;
    os_err = OSTaskCreate(//函数体
                 (void (*)(void *)) uctsk_TciServer,
                 //参数
                 (void          * ) 0,
                 //堆栈
                 (OS_STK        * )&App_TaskTciServerStk[APP_TASK_TCISERVER_STK_SIZE - 1],
                 //优先级
                 (INT8U           ) APP_TASK_TCISERVER_PRIO  );

#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_TCISERVER_PRIO, "Task TciServer", &os_err);
#endif
}
//--------------------------------------------------------------TCI平台任务实体
static void uctsk_TciServer (void)
{
    INT8U i;
    //网络连接
    //---TCI服务器
    /**************************************************
      INT8U  state;   //状态: 0-空闲   1-连接中 2-连接上
        INT8U  order;   //命令: 0-不使用 1-启用IP连接(UDP) 2-启用域名连接(UDP) 3-启用IP连接(TCP) 4-启用域名连接(TCP)  5-断线
        INT8U  IpDnmain[30];//IP或域名(ASCII)
        INT8U  Port[6]; //端口号(ASCII)
    **************************************************/

    Connect_s[TCI_CONNECT_CH].state=0;
    Connect_s[TCI_CONNECT_CH].order=2;
    //strcpy((char *)(Connect_s[TCI_CONNECT_CH].IpDnmain),"xslxhn.vicp.cc");
    Connect_s[TCI_CONNECT_CH].pIpDnmain =  "xslxhn.vicp.cc";
    strcpy((char *)(Connect_s[TCI_CONNECT_CH].Port),"00082");

    /*
    Connect_s[TCI_CONNECT_CH].state=0;
    Connect_s[TCI_CONNECT_CH].order=1;
    strcpy((char *)(Connect_s[TCI_CONNECT_CH].IpDnmain),"175.160.188.51");
    strcpy((char *)(Connect_s[TCI_CONNECT_CH].Port),"00082");
    */
    for(;;)
    {
        //延时1S
        OSTimeDlyHMSM(0, 0, 1, 0);
        //同步输入输出端口
        //----------------------------TEST-BEGIN
        //初始化参数
        FlashPara.data.EquipmentID[0]='0';
        FlashPara.data.EquipmentID[1]='0';
        FlashPara.data.EquipmentID[2]='0';
        FlashPara.data.EquipmentID[3]='0';
        FlashPara.data.EquipmentID[4]='1';
        for(i=0; i<EQUIPMENT_KIN_NUM; i++)TCI_Variable.KinBuf[i]=i;
        for(i=0; i<EQUIPMENT_VIN_NUM; i++)TCI_Variable.VinBuf[i]=i;
        for(i=0; i<EQUIPMENT_IIN_NUM; i++)TCI_Variable.IinBuf[i]=i;
        for(i=0; i<EQUIPMENT_FIN_NUM; i++)TCI_Variable.FinBuf[i]=i;
        for(i=0; i<EQUIPMENT_KOUT_NUM; i++)TCI_Variable.KoutBuf[i]=i;
        for(i=0; i<EQUIPMENT_VOUT_NUM; i++)TCI_Variable.VoutBuf[i]=i;
        for(i=0; i<EQUIPMENT_IOUT_NUM; i++)TCI_Variable.IoutBuf[i]=i;
        for(i=0; i<EQUIPMENT_FOUT_NUM; i++)TCI_Variable.FoutBuf[i]=i;
        //----------------------------TEST-END
        //TCI网络通讯监控
        if(TCI_Variable.TciNetWatchCount>5)
        {
            //重新连接
            Connect_s[TCI_CONNECT_CH].state=0;
        }
        //远程升级倒计时计数器
        if(TCI_Variable.UpdataDownTimer!=0)
        {
            TCI_Variable.UpdataDownTimer--;
        }
        //-----GPRS远程升级服务器
        //心跳
        Tci_Heart();
        //升级
        Tci_GetUpdataData();
    }
}
/**************************************************************
函数功能: 心跳
**************************************************************/
static void Tci_Heart(void)
{
    static INT8U UpDateServer_Heart_S=0;   //心跳计时器
    INT8U i;
    INT16U i16;
    if(Connect_s[TCI_CONNECT_CH].state!=2)return;
    if((Tci_s1.type==TYPE_TX_NULL)&&(UpDateServer_Heart_S>=10))
    {
        UpDateServer_Heart_S=0;
        //-----
        i=0;
        TCI_Variable.TxBuf[i++]=0xFF;
        TCI_Variable.TxBuf[i++]=0x55;
        TCI_Variable.TxBuf[i++]=0xAA;
        TCI_Variable.TxBuf[i++]=0x01;
        TCI_Variable.TxBuf[i++]=0x00;
        TCI_Variable.TxBuf[i++]=0x0A;
        memcpy(&TCI_Variable.TxBuf[i],FlashPara.data.EquipmentID,5);
        i+=5;
        memcpy(&TCI_Variable.TxBuf[i],HARD_VERSION,strlen(HARD_VERSION));
        i+=strlen(HARD_VERSION);
        memcpy(&TCI_Variable.TxBuf[i],SOFT_VERSION,strlen(SOFT_VERSION));
        i+=strlen(SOFT_VERSION);
        i16 = CRC16(TCI_Variable.TxBuf,i);
        TCI_Variable.TxBuf[i++]=i16>>8;
        TCI_Variable.TxBuf[i++]=i16;
        Tci_s1.type=TYPE_TX_GPRS;
        Tci_s1.para[0]=TCI_CONNECT_CH+'0';
        Tci_s1.buf=TCI_Variable.TxBuf;
        Tci_s1.len=i;
        OSQPost (GsmAppTxQ,&Tci_s1);
        //监控计数器
        TCI_Variable.TciNetWatchCount++;
    }
    else
    {
        UpDateServer_Heart_S++;
    }
}
/**************************************************************
函数功能: 升级应答
**************************************************************/
static void Tci_UpdataAck(void)
{
    INT8U i;
    INT16U i16;
    if(Connect_s[TCI_CONNECT_CH].state!=2)return;
    while(Tci_s1.type!=TYPE_TX_NULL)OSTimeDlyHMSM(0, 0, 0, 100);
    i=0;
    TCI_Variable.TxBuf[i++]=0xFF;
    TCI_Variable.TxBuf[i++]=0x55;
    TCI_Variable.TxBuf[i++]=0xAA;
    TCI_Variable.TxBuf[i++]=0x02;
    TCI_Variable.TxBuf[i++]=0x00;
    TCI_Variable.TxBuf[i++]=0x0B;
    memcpy(&TCI_Variable.TxBuf[i],FlashPara.data.EquipmentID,5);
    i+=5;
    memcpy(&TCI_Variable.TxBuf[i],HARD_VERSION,strlen(HARD_VERSION));
    i+=strlen(HARD_VERSION);
    memcpy(&TCI_Variable.TxBuf[i],SOFT_VERSION,strlen(SOFT_VERSION));
    i+=strlen(SOFT_VERSION);
    TCI_Variable.TxBuf[i++]=0x01;
    i16 = CRC16(TCI_Variable.TxBuf,i);
    TCI_Variable.TxBuf[i++]=i16>>8;
    TCI_Variable.TxBuf[i++]=i16;
    Tci_s1.type=TYPE_TX_GPRS;
    Tci_s1.para[0]=TCI_CONNECT_CH+'0';
    Tci_s1.buf=TCI_Variable.TxBuf;
    Tci_s1.len=i;
    OSQPost (GsmAppTxQ,&Tci_s1);
}
/**************************************************************
函数功能: 获取升级数据
**************************************************************/
static void Tci_GetUpdataData(void)
{
    INT8U i;
    INT16U i16;
    if(Connect_s[TCI_CONNECT_CH].state!=2)return;
    if(Tci_s1.type!=TYPE_TX_NULL)return;
    if(TCI_Variable.UpdataDownTimer==0 && TCI_Variable.UpdataPacketNow!=0 && TCI_Variable.UpdataPacketSum!=0 && TCI_Variable.UpdataPacketNow<=TCI_Variable.UpdataPacketSum)
    {
        i=0;
        TCI_Variable.TxBuf[i++]=0xFF;
        TCI_Variable.TxBuf[i++]=0x55;
        TCI_Variable.TxBuf[i++]=0xAA;
        TCI_Variable.TxBuf[i++]=0x03;
        TCI_Variable.TxBuf[i++]=0x00;
        TCI_Variable.TxBuf[i++]=0x0C;
        memcpy(&TCI_Variable.TxBuf[i],FlashPara.data.EquipmentID,5);
        i+=5;
        memcpy(&TCI_Variable.TxBuf[i],HARD_VERSION,strlen(HARD_VERSION));
        i+=strlen(HARD_VERSION);
        memcpy(&TCI_Variable.TxBuf[i],SOFT_VERSION,strlen(SOFT_VERSION));
        i+=strlen(SOFT_VERSION);
        TCI_Variable.TxBuf[i++]=TCI_Variable.UpdataPacketNow>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.UpdataPacketNow;
        i16 = CRC16(TCI_Variable.TxBuf,i);
        TCI_Variable.TxBuf[i++]=i16>>8;
        TCI_Variable.TxBuf[i++]=i16;
        Tci_s1.type=TYPE_TX_GPRS;
        Tci_s1.para[0]=TCI_CONNECT_CH+'0';
        Tci_s1.buf=TCI_Variable.TxBuf;
        Tci_s1.len=i;
        OSQPost (GsmAppTxQ,&Tci_s1);
        TCI_Variable.UpdataDownTimer=5;
    }
}
/**************************************************************
函数功能: 获取端口应答
参    数: *p - 帧头
**************************************************************/
static void Tci_GetPortAck(INT8U *p)
{
    INT8U i=0,j=0;
    INT16U i16;
    if(Connect_s[TCI_CONNECT_CH].state!=2)return;
    while(Tci_s1.type!=TYPE_TX_NULL)OSTimeDlyHMSM(0, 0, 0, 100);
    //发送数据
    TCI_Variable.TxBuf[i++]=0xFF;
    TCI_Variable.TxBuf[i++]=0x55;
    TCI_Variable.TxBuf[i++]=0xAA;
    TCI_Variable.TxBuf[i++]=0x04;
    i16=(EQUIPMENT_KIN_NUM*1+2)+ //V+TL
        (EQUIPMENT_VIN_NUM*2+2)+
        (EQUIPMENT_IIN_NUM*2+2)+
        (EQUIPMENT_FIN_NUM*4+2)+
        (EQUIPMENT_KOUT_NUM*2+2)+ //V+TL
        (EQUIPMENT_VOUT_NUM*2+2)+
        (EQUIPMENT_IOUT_NUM*2+2)+
        (EQUIPMENT_FOUT_NUM*4+2)+
        10;
    TCI_Variable.TxBuf[i++]=i16>>8;
    TCI_Variable.TxBuf[i++]=i16;
    memcpy(&TCI_Variable.TxBuf[i],FlashPara.data.EquipmentID,5);
    i+=5;
    memcpy(&TCI_Variable.TxBuf[i],HARD_VERSION,strlen(HARD_VERSION));
    i+=strlen(HARD_VERSION);
    memcpy(&TCI_Variable.TxBuf[i],SOFT_VERSION,strlen(SOFT_VERSION));
    i+=strlen(SOFT_VERSION);
    //-----开关量输入
    TCI_Variable.TxBuf[i++]=0x01;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_KIN_NUM*1;//L
    for(j=0; j<EQUIPMENT_KIN_NUM; j++)
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.KinBuf[j];
    }
    //V
    //-----电压输入
    TCI_Variable.TxBuf[i++]=0x02;               //T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_VIN_NUM*2;//L
    for(j=0; j<EQUIPMENT_VIN_NUM; j++)                      //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.VinBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.VinBuf[j];
    }
    //-----电流输入
    TCI_Variable.TxBuf[i++]=0x03;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_IIN_NUM*2;//L
    for(j=0; j<EQUIPMENT_IIN_NUM; j++)                      //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.IinBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.IinBuf[j];
    }
    //-----频率输入
    TCI_Variable.TxBuf[i++]=0x04;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_FIN_NUM*4;//L
    for(j=0; j<EQUIPMENT_FIN_NUM; j++)                      //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.FinBuf[j]>>24;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FinBuf[j]>>16;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FinBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FinBuf[j];
    }
    //-----开关量输出
    TCI_Variable.TxBuf[i++]=0x81;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_KOUT_NUM*2;//L
    for(j=0; j<EQUIPMENT_KOUT_NUM; j++)
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.KoutBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.KoutBuf[j];
    }
    //V
    //-----电压输出
    TCI_Variable.TxBuf[i++]=0x82;               //T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_VOUT_NUM*2;//L
    for(j=0; j<EQUIPMENT_VOUT_NUM; j++)                     //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.VoutBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.VoutBuf[j];
    }
    //-----电流输出
    TCI_Variable.TxBuf[i++]=0x83;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_IOUT_NUM*2;//L
    for(j=0; j<EQUIPMENT_IOUT_NUM; j++)                     //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.IoutBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.IoutBuf[j];
    }
    //-----频率输出
    TCI_Variable.TxBuf[i++]=0x84;//T
    TCI_Variable.TxBuf[i++]=EQUIPMENT_FOUT_NUM*4;//L
    for(j=0; j<EQUIPMENT_FOUT_NUM; j++)                     //V
    {
        TCI_Variable.TxBuf[i++]=TCI_Variable.FoutBuf[j]>>24;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FoutBuf[j]>>16;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FoutBuf[j]>>8;
        TCI_Variable.TxBuf[i++]=TCI_Variable.FoutBuf[j];
    }
    //CRC
    i16 = CRC16(TCI_Variable.TxBuf,i);
    TCI_Variable.TxBuf[i++]=i16>>8;
    TCI_Variable.TxBuf[i++]=i16;
    Tci_s1.type=TYPE_TX_GPRS;
    Tci_s1.para[0]=TCI_CONNECT_CH+'0';
    Tci_s1.buf=TCI_Variable.TxBuf;
    Tci_s1.len=i;
    OSQPost (GsmAppTxQ,&Tci_s1);
}
/**************************************************************
函数功能: 控制端口应答
参    数: *p - 帧头
**************************************************************/
static void Tci_ControlPortAck(INT8U *p)
{
    INT8U i=0,j=0;
    INT16U i16,j16;
    INT8U err=0;
    INT16U len;
    //解析现有数据(都能操作err=0,否则err=1)
    len = (p[4]<<8) + p[5];
    i16 = 0;
    while(i16<len)
    {
        //T解析
        switch(p[6+i16])
        {
            case 0x81:
                break;
            case 0x82:
                break;
            case 0x83:
                break;
            case 0x84:
                break;
            default:
                err=1;
                break;
        }
        //L解析
    }
    //发送回应
    if(Connect_s[TCI_CONNECT_CH].state!=2)return;
    while(Tci_s1.type!=TYPE_TX_NULL)OSTimeDlyHMSM(0, 0, 0, 100);
    TCI_Variable.TxBuf[i++]=0xFF;
    TCI_Variable.TxBuf[i++]=0x55;
    TCI_Variable.TxBuf[i++]=0xAA;
    TCI_Variable.TxBuf[i++]=0x05;
    TCI_Variable.TxBuf[i++]=0x00;
    TCI_Variable.TxBuf[i++]=0x0B;
    memcpy(&TCI_Variable.TxBuf[i],FlashPara.data.EquipmentID,5);
    i+=5;
    memcpy(&TCI_Variable.TxBuf[i],HARD_VERSION,strlen(HARD_VERSION));
    i+=strlen(HARD_VERSION);
    memcpy(&TCI_Variable.TxBuf[i],SOFT_VERSION,strlen(SOFT_VERSION));
    i+=strlen(SOFT_VERSION);
    if(err==0)
    {
        TCI_Variable.TxBuf[i++]=0x01;
    }
    else
    {
        TCI_Variable.TxBuf[i++]=0x02;
    }
    //CRC
    i16 = CRC16(TCI_Variable.TxBuf,i);
    TCI_Variable.TxBuf[i++]=i16>>8;
    TCI_Variable.TxBuf[i++]=i16;
    Tci_s1.type=TYPE_TX_GPRS;
    Tci_s1.para[0]=TCI_CONNECT_CH+'0';
    Tci_s1.buf=TCI_Variable.TxBuf;
    Tci_s1.len=i;
    OSQPost (GsmAppTxQ,&Tci_s1);
}
//--------------------------------------------------------------解析函数
/*******************************************************************************
* 函数功能: 解析远程升级监控服务器数据
*******************************************************************************/
void TCIServer_Parse(INT8U *buf,INT16U len)
{
    INT16U i16,j16;
    INT16U err;
    //提取数据
    memcpy(TCI_Variable.RxBuf,buf,len);
    //解析
    if(TCI_Variable.RxBuf[0]==0xFF && TCI_Variable.RxBuf[1]==0xAA && TCI_Variable.RxBuf[2]==0x55)
    {
        //帧头验证
        //CRC验证
        i16=CRC16(TCI_Variable.RxBuf,len-2);
        j16=(TCI_Variable.RxBuf[len-2]<<8) + TCI_Variable.RxBuf[len-1];
        if(i16!=j16)return;
        //len验证
        i16=(TCI_Variable.RxBuf[4]<<8) + TCI_Variable.RxBuf[5];
        i16+=8;
        if(i16!=len)return;
        //网络监控计数器清零
        TCI_Variable.TciNetWatchCount=0;
        //解析
        switch(TCI_Variable.RxBuf[3])
        {
            case 0x81://心跳应答
                break;
            case 0x82://升级指令
                TCI_Variable.UpdataPacketNow=1;
                TCI_Variable.UpdataPacketSum=(TCI_Variable.RxBuf[6]<<8) + TCI_Variable.RxBuf[7];
                Tci_UpdataAck();
                break;
            case 0x83://升级数据
                i16=(TCI_Variable.RxBuf[6]<<8) + TCI_Variable.RxBuf[7];
                if(TCI_Variable.UpdataPacketNow==i16)
                {
                    //远程升级数据有效
                    //可以继续要数据
                    TCI_Variable.UpdataDownTimer=0;
                    //擦除分区
                    if(TCI_Variable.UpdataPacketNow%2==1)
                    {
                        //UpdataPacketNow为单数时需要页擦除
                        FLASH_Unlock();
                        FLASH_ErasePage(UPDATA_FLASH_ADDR+(TCI_Variable.UpdataPacketNow-1)*1024);
                        FLASH_Lock();
                    }
                    //写数据
                    FLASH_Unlock();
                    err=ProgramFlash(UPDATA_FLASH_ADDR+(TCI_Variable.UpdataPacketNow-1)*1024,&TCI_Variable.RxBuf[8],1024);
                    FLASH_Lock();
                    //校验
                    if(err==0)
                    {
                        if(TCI_Variable.UpdataPacketNow<TCI_Variable.UpdataPacketSum)
                        {
                            //升级进行中
                            TCI_Variable.UpdataPacketNow++;
                        }
                        else
                        {
                            //升级完成
                            TCI_Variable.UpdataPacketNow=0;
                            TCI_Variable.UpdataPacketSum=0;
                        }
                    }
                }
                break;
            case 0x84://获取端口状态
                Tci_GetPortAck(TCI_Variable.RxBuf);
                break;
            case 0x85://控制端口指令
                Tci_ControlPortAck(TCI_Variable.RxBuf);
                break;
            default:
                break;
        }
    }
}
/*****************************************************************
函数原型：ProgramFlash
功能描述：本函数将数据存储在stm32内部flash中
参数描述：FLASH_ADR 写入首地址 2k/页
参数名称：  输入/输出？ 类型        描述
日期        修改人      修改描述
------      ---------   -------------
*****************************************************************/
INT8U ProgramFlash(INT32U FLASH_ADR,INT8U *buf,INT16U len)
{
    INT32U wWord,rWord ;
    INT16U  i,err=0;
    //得到32位（字）长度
    if(len%4) len=len/4+1;
    else      len=len/4;
    //写数据
    for(i=0; i<len; i++)
    {
        wWord=0;
        wWord += buf[4*i+3];
        wWord<<=8;
        wWord += buf[4*i+2];
        wWord<<=8;
        wWord += buf[4*i+1];
        wWord<<=8;
        wWord += buf[4*i];
        FLASH_ProgramWord(FLASH_ADR+4*i,wWord);//写到flash,每次写入4字节数据
        rWord = *(vu32*)(FLASH_ADR+i*4);      //读写入的数据
        //比较写入与读出的数据，进行交验
        if(wWord!=rWord)
        {
            err++;
        }
    }
    return(err);
}
