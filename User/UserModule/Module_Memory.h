/**
  ******************************************************************************
  * @file    Bsp_Led.h
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   Module-Memory -->   memory.
  * @note    对内部Flash与外部Flash的统一存储管理
  * @verbatim

 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         STM      :  STM32F1  STM32F4
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
         无
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MODULE_MEMORY_H
#define __MODULE_MEMORY_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
//
#if     (defined(PROJECT_BASE_STM32F1)\
          ||defined(PROJECT_BASE_STM32F4)\
          ||defined(BOOTLOADER)\
          ||defined(PROJECT_SPI_SLAVE)\
          ||defined(BASE_NRF51)\
          ||defined(BASE_NRF52)\
          ||defined(PROJECT_TCI_V30)\
          ||defined(XKAP_ICARE_B_C)\
          ||defined(XKAP_ICARE_A_S)\
          ||defined(XKAP_ICARE_A_C)\
          ||defined(PROJECT_NRF5X_BLE)\
          ||defined(XKAP_ICARE_B_D))
//---->
//硬件接口
//#define  MODULE_MEMORY_CPUFLASH_INIT
//#define  MODULE_MEMORY_EXTFLASH_INIT
//结构体类型
//地址分配
//<----
#elif   (defined(PROJECT_ARMFLY_V5_XSL))
//---->
//硬件接口
#include "Bsp_ExtFlash.h"
#define MODULE_MEMORY_CPUFLASH_INIT            Bsp_CpuFlash_Init()
#define MODULE_MEMORY_EXTFLASH_INIT            Bsp_ExtFlash_Init()
//结构体类型
//地址分配
//<----
#elif   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
//硬件接口
#include "Bsp_ExtFlash.h"
#define MODULE_MEMORY_CPUFLASH_INIT            Bsp_CpuFlash_Init()
#define MODULE_MEMORY_EXTFLASH_INIT            Bsp_ExtFlash_Init()
//结构体类型
typedef struct MODULE_MEMORY_S_DAY_INFO
{
    //-----通讯
    uint8_t Protocol_BeginByte;  //通讯起始字节(固定0x00)
    uint8_t Protocol_Len;        //通讯长度字节(固定0x48)
    //-----存储协议70字节
    uint8_t BeginByte;           //起始字节(固定0xCC)
    uint8_t ProtocolVer;         //协议版本
    uint8_t DeviceType;          //设备类型
    uint8_t BeginTime[7];        //开始时间(0011-22-33-44-55-66(年-月-日-时-分-秒))
    uint8_t EndTime[7];          //结束时间(0011-22-33-44-55-66(年-月-日-时-分-秒))
    //
    uint8_t WakeLevelTime_m[2];  //觉醒时长(0011):单位(分钟)
    uint8_t LightLevelTime_m[2]; //浅睡时长(0011):单位(分钟)
    uint8_t MiddleLevelTime_m[2];//中睡时长(0011):单位(分钟)
    uint8_t DeepLevelTime_m[2];  //深睡时长(0011):单位(分钟)
    uint8_t FallAsleepTime_m[2]; //入睡时长(0011):从开始时间到第一个中睡的时长(分钟)
    //
    uint8_t TempBuf[12];         //温度记录,单位(度),每小时记录一个,未测量为0xFF(开始后15分钟开始记录)
    uint8_t HumiBuf[12];         //湿度记录,单位(% ),每小时记录一个,未测量为0xFF(开始后15分钟开始记录)
    //
    uint8_t BreathAverage;       //平均呼吸数
    uint8_t BreathMax;           //最大呼吸数
    uint8_t BreathMin;           //最小呼吸数
    uint8_t BreathNum;           //呼吸数据有效个数
    //
    uint8_t HeartRateAverage;    //平均心率数
    uint8_t HeartRateMax;        //最大心率数
    uint8_t HeartRateMin;        //最小心率数
    uint8_t HeartRateNum;        //心率数据有效个数
    //
    uint8_t BodyMoveMax;         //最大体动能量
    //
    uint8_t Res1[5];             //保留
    //
    uint8_t BodyMoveCount[2];    //体动次数
    uint8_t BodyMoveNum;         //体动数据有效个数
    uint8_t Scoring;             //评分
    //
    uint8_t EndByte;             //结束字节(固定0xAA)
    //---------------------------//上面是与通讯的相同结构
    //23
    uint32_t GPS_Latitude;
    uint32_t GPS_Longitude;
    //闹钟响前睡眠等级
    uint8_t SleepLevelBeforeAlarm;
    //睡眠结构得分
    uint8_t Mark_SleepStruct;
    //总睡时长系数
    uint16_t Mart_TotalSleep;
    //入睡时长得分
    uint8_t Mart_SleepIntoPeriod;
    //体动次数得分
    uint8_t Mark_BodyMovement;
    //唤醒状态得分
    uint8_t Mark_WakeState;
    //睡眠习惯得分
    uint8_t Mark_BeginSleepTimeChange;
    //睡眠温度得分
    uint8_t Mark_Temp;
    //睡眠湿度得分
    uint8_t Mark_Humi;
    //数据上传成功次数
    uint8_t UpdataToServerCmt;
    //基站数据
    uint8_t Gsm_LacCi[4];
    //状态:
    //---数据是否有效   (0-有效 1-无效)
    uint8_t Flag_DataValid :1;
    //---启动模式       (0-手动 1-自动)
    uint8_t Flag_StartMode :1;
    //---停止模式       (0-手动 1-自动)
    uint8_t Flag_StopMode  :1;
    //---修改标识       (用于ICAM项目,标记此条记录已被重新修改)
    //uint8_t Flag_Revision  :1;
    //---备用
    uint8_t Flag_Res       :5;
    //------------------
    uint8_t Res[128-(2+70+23+1)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_DAY_INFO;
enum MODULE_MEMORY_E_SLEEPLEVEL
{
    MODULE_MEMORY_E_SLEEPLEVEL_NULL    =  0,
    MODULE_MEMORY_E_SLEEPLEVEL_WAKE    =  1,
    MODULE_MEMORY_E_SLEEPLEVEL_LIGHT   =  2,
    MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE  =  3,
    MODULE_MEMORY_E_SLEEPLEVEL_DEEP    =  4,
};
typedef struct MODULE_MEMORY_S_DAY_SLEEPLEVEL
{
    //-----通讯
    uint8_t Protocol_BeginByte;  //通讯起始字节(固定0x01)
    uint8_t Protocol_Len;        //通讯长度字节(固定0xF3)
    //-----存储
    uint8_t DataBuf[241];
    //------------------
    uint8_t Res[256-(2+241)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_DAY_SLEEPLEVEL;
typedef struct MODULE_MEMORY_S_DAY_BREATH
{
    //-----通讯
    uint8_t Protocol_BeginByte;  //通讯起始字节(固定0x02)
    uint8_t Protocol_Len;        //通讯长度字节(固定0xF3)
    //-----存储
    uint8_t DataBuf[241];
    //------------------
    uint8_t Res[256-(2+241)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_DAY_BREATH;

typedef struct MODULE_MEMORY_S_DAY_HEARTRATE
{
    //-----通讯
    uint8_t Protocol_BeginByte;  //通讯起始字节(固定0x03)
    uint8_t Protocol_Len;        //通讯长度字节(固定0xF3)
    //-----存储
    uint8_t DataBuf[241];
    //------------------
    uint8_t Res[256-(2+241)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_DAY_HEARTRATE;

typedef struct MODULE_MEMORY_S_DAY_BODYMOVE
{
    //-----通讯
    uint8_t Protocol_BeginByte;  //通讯起始字节(固定0x04)
    uint8_t Protocol_Len;        //通讯长度字节(固定0xF3)
    //-----存储
    uint8_t DataBuf[241];
    //------------------
    uint8_t Res[256-(2+241)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_DAY_BODYMOVE;

typedef struct MODULE_MEMORY_S_SLEEP_BEGIN_TIME
{
    uint16_t Minute[28];   // 从0点开始计算的分钟数
    uint8_t NextHhMmNum;   // 0-27
    //------------------
    uint8_t Res[128-(28*2+1)-2];
    uint16_t Sum;
} MODULE_MEMORY_S_SLEEP_BEGIN_TIME;

//地址分配
#define MODULE_MEMORY_FORMAT_VER               0x11
#define MODULE_MEMORY_DEVICE_TYPE              0x01
//
#define MODULE_MEMORY_ADDR_DAY_NUM             7
//
#define MODULE_MEMORY_DAY_INFO_SIZE            128
#define MODULE_MEMORY_DAY_INFO_ADDR            (BSP_CPU_FLASH_PARA_ADDR+0x800)
#define MODULE_MEMORY_DAY_INFO_ADDR_END        (MODULE_MEMORY_DAY_INFO_ADDR+(MODULE_MEMORY_ADDR_DAY_NUM*MODULE_MEMORY_DAY_INFO_SIZE)-1)
//
#define MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE      256
#define MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR      (MODULE_MEMORY_DAY_INFO_ADDR_END+1)
#define MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR_END  (MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR+(MODULE_MEMORY_ADDR_DAY_NUM*MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE)-1)
//
#define MODULE_MEMORY_DAY_BREATH_SIZE          256
#define MODULE_MEMORY_DAY_BREATH_ADDR          (MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR_END+1)
#define MODULE_MEMORY_DAY_BREATH_ADDR_END      (MODULE_MEMORY_DAY_BREATH_ADDR+(MODULE_MEMORY_ADDR_DAY_NUM*MODULE_MEMORY_DAY_BREATH_SIZE)-1)
//
#define MODULE_MEMORY_DAY_HEARTRATE_SIZE       256
#define MODULE_MEMORY_DAY_HEARTRATE_ADDR       (MODULE_MEMORY_DAY_BREATH_ADDR_END+1)
#define MODULE_MEMORY_DAY_HEARTRATE_ADDR_END   (MODULE_MEMORY_DAY_HEARTRATE_ADDR+(MODULE_MEMORY_ADDR_DAY_NUM*MODULE_MEMORY_DAY_HEARTRATE_SIZE)-1)
//
#define MODULE_MEMORY_DAY_BODYMOVE_SIZE        256
#define MODULE_MEMORY_DAY_BODYMOVE_ADDR        (MODULE_MEMORY_DAY_HEARTRATE_ADDR_END+1)
#define MODULE_MEMORY_DAY_BODYMOVE_ADDR_END    (MODULE_MEMORY_DAY_BODYMOVE_ADDR+(MODULE_MEMORY_ADDR_DAY_NUM*MODULE_MEMORY_DAY_BODYMOVE_SIZE)-1)
//
#define MODULE_MEMORY_SLEEP_BEGIN_TIME_SIZE     128
#define MODULE_MEMORY_SLEEP_BEGIN_TIME_ADDR     (MODULE_MEMORY_DAY_BODYMOVE_ADDR_END+1)
#define MODULE_MEMORY_SLEEP_BEGIN_TIME_ADDR_END (MODULE_MEMORY_SLEEP_BEGIN_TIME_ADDR+MODULE_MEMORY_SLEEP_BEGIN_TIME_SIZE-1)
//
#define MODULE_MEMORY_EXTFLASH_BEGIN_ADDR         512*1024L
//体动数据(有效状态/年/月/日(4B) + 34560B(6*60*24*4)+6B上传标识位图)
#define MODULE_MEMORY_EXTFLASH_MOVE_NUM         7
#define MODULE_MEMORY_EXTFLASH_MOVE_SIZE        36*1024L
#define MODULE_MEMORY_EXTFLASH_MOVE_ADDR        MODULE_MEMORY_EXTFLASH_BEGIN_ADDR
#define  MODULE_MEMORY_EXTFLASH_MOVE_ADDR_END   (MODULE_MEMORY_EXTFLASH_MOVE_ADDR+(MODULE_MEMORY_EXTFLASH_MOVE_NUM*MODULE_MEMORY_EXTFLASH_MOVE_SIZE)-1)
#define MODULE_MEMORY_EXTFLASH_ADDR_1_BEGIN     MODULE_MEMORY_EXTFLASH_MOVE_ADDR
#define MODULE_MEMORY_EXTFLASH_ADDR_1_END       MODULE_MEMORY_EXTFLASH_MOVE_ADDR_END
#define MODULE_MEMORY_EXTFLASH_ADDR_1_STR       "EF-Move"
//
#define MODULE_MEMORY_INFO_NUM                  (10240L)
#define MODULE_MEMORY_INFO_SIZE                 sizeof(MODULE_MEMORY_S_INFO)
#define MODULE_MEMORY_INFO_ADDR                 (MODULE_MEMORY_EXTFLASH_MOVE_ADDR_END+1)
#define MODULE_MEMORY_INFO_ADDR_END             (MODULE_MEMORY_INFO_ADDR+(MODULE_MEMORY_INFO_NUM*MODULE_MEMORY_INFO_SIZE)-1)
#define MODULE_MEMORY_EXTFLASH_ADDR_2_BEGIN     MODULE_MEMORY_INFO_ADDR
#define MODULE_MEMORY_EXTFLASH_ADDR_2_END       MODULE_MEMORY_INFO_ADDR_END
#define MODULE_MEMORY_EXTFLASH_ADDR_2_STR       "EF-Info"
//<----
#elif   (defined(XKAP_ICARE_A_M))
//---->
//硬件接口
#include "Bsp_ExtFlash.h"
#define MODULE_MEMORY_CPUFLASH_INIT            Bsp_CpuFlash_Init()
#define MODULE_MEMORY_EXTFLASH_INIT            Bsp_ExtFlash_Init()
//结构体类型
//地址分配
//<----
#elif   (defined(XKAP_ICARE_B_M))
//---->
//硬件接口
#include "Bsp_ExtFlash.h"
#define MODULE_MEMORY_CPUFLASH_INIT            Bsp_CpuFlash_Init()
#define MODULE_MEMORY_EXTFLASH_INIT            Bsp_ExtFlash_Init()
//结构体类型
typedef struct MODULE_MEMORY_S_WEIGHT
{
    uint32_t Hx711[10];
    uint16_t AdValue[4][10];
    uint8_t DateTime[6];
    uint8_t Res[2];
} MODULE_MEMORY_S_WEIGHT;
//地址分配
#define MODULE_MEMORY_INFO_NUM                  (10240L)
#define MODULE_MEMORY_INFO_SIZE                 sizeof(MODULE_MEMORY_S_INFO)
#define MODULE_MEMORY_INFO_ADDR                 (400*1024L)
#define MODULE_MEMORY_INFO_ADDR_END             (MODULE_MEMORY_INFO_ADDR+(MODULE_MEMORY_INFO_NUM*MODULE_MEMORY_INFO_SIZE)-1)
#define MODULE_MEMORY_EXTFLASH_ADDR_1_BEGIN     MODULE_MEMORY_INFO_ADDR
#define MODULE_MEMORY_EXTFLASH_ADDR_1_END       MODULE_MEMORY_INFO_ADDR_END
#define MODULE_MEMORY_EXTFLASH_ADDR_1_STR       "EF-Info"

#define MODULE_MEMORY_EXTFLASH_BEGIN_ADDR       512*1024L
#define MODULE_MEMORY_EXTFLASH_WEIGHT_NUM       3600L    //10Hz 3600s 128Byte/s  =   450kB
#define MODULE_MEMORY_EXTFLASH_WEIGHT_SIZE      sizeof(MODULE_MEMORY_S_WEIGHT)
#define MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR      MODULE_MEMORY_EXTFLASH_BEGIN_ADDR
#define MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END  (MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR+(MODULE_MEMORY_EXTFLASH_WEIGHT_NUM*MODULE_MEMORY_EXTFLASH_WEIGHT_SIZE)-1)
#define MODULE_MEMORY_EXTFLASH_ADDR_2_BEGIN     MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR
#define MODULE_MEMORY_EXTFLASH_ADDR_2_END       MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END
#define MODULE_MEMORY_EXTFLASH_ADDR_2_STR       "EF-Weight"
//
extern uint8_t memory_WeightLock;
//<----
#elif   (defined(TEST_NRF52_V1))
//---->
//硬件接口
#include "Bsp_ExtFlash.h"
#define MODULE_MEMORY_CPUFLASH_INIT            Bsp_CpuFlash_Init()
//#define MODULE_MEMORY_EXTFLASH_INIT            Bsp_ExtFlash_Init()
//地址分配
#define MODULE_MEMORY_INFO_NUM                  (10240L)
#define MODULE_MEMORY_INFO_SIZE                 sizeof(MODULE_MEMORY_S_INFO)
#define MODULE_MEMORY_INFO_ADDR                 (400*1024L)
#define MODULE_MEMORY_INFO_ADDR_END             (MODULE_MEMORY_INFO_ADDR+(MODULE_MEMORY_INFO_NUM*MODULE_MEMORY_INFO_SIZE)-1)
#define MODULE_MEMORY_EXTFLASH_ADDR_1_BEGIN     MODULE_MEMORY_INFO_ADDR
#define MODULE_MEMORY_EXTFLASH_ADDR_1_END       MODULE_MEMORY_INFO_ADDR_END
#define MODULE_MEMORY_EXTFLASH_ADDR_1_STR       "EF-Info"
//<----
#else
//---->
#error Please Set Project to Module_Memory.h
//<----
#endif
//-------------------接口宏定义(硬件无关)--------------------数据结构
//----------操作外部存储器的指针结构体
typedef struct MODULE_MEMORY_S_EXTFLASH
{
    uint32_t addr;
    uint32_t len;
} MODULE_MEMORY_S_EXTFLASH;
//----------全局参数(当前放在芯片内部使用)
typedef struct MODULE_MEMORY_S_PARA
{
    //-----半复位参数
    // 1-3
    uint32_t   Updata;              //初始化-0x12345678 需要升级-0x87654321
    uint32_t   UpdataLen;           //升级文件长度
    uint32_t   UpdataSum;           //升级文件校验和
    // 4
    uint16_t   RtcAlarm[2];         //闹钟(高字节表示小时/低字节表示分钟)
    // 5
    uint8_t    Addr;                // 设备地址
    uint8_t    RtcFormat;           // 时钟表达方式(0->24制式,1->12制式)
    uint8_t    RtcAlarmMode[2];     // 闹钟模式(0-无闹钟,1-普通闹钟,2-智能闹钟)
    // 6
    uint8_t    LcdBackLight1;       // 液晶背光(用于通常状态)
    uint8_t    LcdBackLight2;       // 液晶背光(用于特殊状态->睡眠状态)
    uint8_t    LcdBackLight3;       // 液晶背光(用于特殊状态->)
    uint8_t    LcdBackLight4;       // 液晶背光(用于特殊状态->)
    // 7-8
    uint8_t    UartBps[8];          // 0-1200,2-2400,3-4800,4-9600,5-19200,6-38400,7-57600,8-115200
    // 9
    uint16_t   Rfms_SubThreshold;   // 差分阈值
    uint8_t    Gsm_Apn;             // 与GprsNet文件中的Apn意义等同
    uint8_t    MemoryDayNextP;      // 系统最多存储7组数据,0-6表达下一次应存储的位置
    // 10
    uint16_t   RtcAutoBeginTime;    // 自动启动开始时间(高字节表示小时/低字节表示分钟)
    uint16_t   RtcAutoEndTime;      // 自动启动结束时间(高字节表示小时/低字节表示分钟)
    // 11
    uint8_t    RtcAutoOnOff;        // RFMS自动测量使能(OFF-关闭,ON-开始)
    uint8_t    Flag_GprsRegister :1;      // 注册标识
    uint8_t    Flag_GprsUploadDayMove:1;  // GPRS上传整日体动信息
    uint8_t    Flag_GprsKeepOn   :1;      // GPRS保持连接状态(监测时不关闭)
    uint8_t    Flag_Res3         :1;      // 备用
    uint8_t    Flag_NightLight   :1;   // 夜灯功能开启与关闭
    uint8_t    Flag_Res5         :1;   // 备用
    uint8_t    Flag_Res6         :1;   // 备用
    uint8_t    Flag_Res7         :1;   // 备用
    uint8_t    res[2];
    // 12
    uint8_t    RtcWarnOnOff[4];     // 入睡提醒使能(OFF-关闭,ON-开始)
    // 13-14
    uint16_t   RtcWarnTime[4];      // 入睡提醒时间
    //-----全复位参数
    // 15
    uint16_t   PowerOnCount;        // 上电计数
    uint16_t   ResetCount;          // 复位计数(软硬看门狗复位)
    // 16-17
    uint8_t    Language;            // 语言
    uint8_t    HardFault_Count;     // 硬件错误计数
    uint8_t    HardFault_Time[6];   // 出错时间戳
    // 18
    uint8_t    *pHardFaultTaskName; // 出错任务名称
    // 19
    uint8_t    Theme;               // 主题
    uint8_t    BootVer;             // Boot版本
    uint8_t    BootUpdataState;     // 通过Boot升级版本,Boot程序会把此标志置1,再由应用层程序置0
    uint8_t    GprsServer;          // GPRS服务器(0-正式 1-集成 2-熙康)
    // 20
    uint32_t   ThreshsholdValue1;   // 阈值1(iCareB-0压值)
    // 21-26
    uint8_t    FactoryID[24];       // 厂家ID
    // 27
    uint32_t   ThreshsholdValue2;   // 阈值2(iCareB-空床或在床值ad 在床值最高位为1)
    uint16_t   ThreshsholdValue3;   // 阈值3(iCareB-开夜灯照度低阈值)
    uint16_t   ThreshsholdValue4;   // 阈值4(iCareB-关夜灯照度高阈值)
    // 29
    uint8_t    Offset_H[2];         // 上偏移(iCareB: 0: 基于空床判定, 1:基于在床判定)
    uint8_t    Offset_L[2];         // 下偏移(iCareB: 0: 基于空床判定, 1:基于在床判定)
    // 30
    uint16_t   Offset_TimerS;       // 偏移时长nS后响应
    uint16_t   Reponse_TimerS;      // 响应时长(0xFFFF为常响应)
#if   (defined(PROJECT_ARMFLY_V5_XSL))
    // 31   触摸屏
    uint16_t   Touch_usAdcX1;       // 左上角
    uint16_t   Touch_usAdcY1;
    // 32
    uint16_t   Touch_usAdcX2;       // 右下角
    uint16_t   Touch_usAdcY2;
    // 33
    uint16_t   Touch_usAdcX3;       // 左下角
    uint16_t   Touch_usAdcY3;
    // 34
    uint16_t   Touch_usAdcX4;       // 右上角
    uint16_t   Touch_usAdcY4;
    // 35
    uint16_t   Touch_XYChange;      // X, Y 是否交换
    uint16_t   res16;
#endif
} MODULE_MEMORY_S_PARA;
//----------信息记录(当前放在芯片内部使用)
typedef enum
{
    //无错误
    MODULE_E_ERR_NULL   =  0,
    //----------错误
    MODULE_E_ERR_BEGIN  =   1,
    //系统层错误
    MODULE_E_ERR_HARDFAULT,             //  硬件错误        任务优先级
    //驱动层错误
    MODULE_E_ERR_BSP    =  1000,
    MODULE_E_ERR_RTC_VBAT,
    //模块层错误
    MODULE_E_ERR_MODULE =  3000,
    MODULE_E_ERR_MEM,
    MODULE_E_ERR_CRC,
    //应用层错误
    MODULE_E_ERR_APP    =  4000,
    //测量时间过短
    MODULE_E_ERR_SLEEP_MEASURN_SHORT,
    //体动过多
    MODULE_E_ERR_SLEEP_MOVE_TOO_MANY,
    //深睡过多
    MODULE_E_ERR_SLEEP_DEEP_TOO_MANY,
    //无信号
    MODULE_E_ERR_SLEEP_NO_SIGN,
    //蓝牙连接失败
    MODULE_E_ERR_BLUETOOTH_CONNECT,
    //蓝牙数据传输错误
    MODULE_E_ERR_BLUETOOTH_TRANSFER,
    MODULE_E_ERR_END    =   9999,
    //----------警告
    MODULE_E_WARN_BEGIN =   10000,
    MODULE_E_WARN_END   =   19999,
    //----------信息
    MODULE_E_INFO_BEGIN =   20000,
    MODULE_E_INFO_POWER_ON,                     // 开机信息         参数空
    MODULE_E_INFO_ICAREB_MANUAL_CAL_NULL,       // 手动空床校准           校准值>>8
    MODULE_E_INFO_ICAREB_MANUAL_CAL_FULL,       // 手动在床校准           校准值>>8
    MODULE_E_INFO_END   =   29999,
    //----------调试
    MODULE_E_DEBUG_BEGIN=   30000,
    MODULE_E_DEBUG_END  =   39999,
} MODULE_MEMORY_E_INFO_ERR;
typedef struct MODULE_MEMORY_S_INFO_INFO
{
    uint32_t NextAddr;  //  下一个信息要存储的地址
    uint32_t AllNum;    //  当前存储的信息条数
} MODULE_MEMORY_S_INFO_INFO;
typedef struct MODULE_MEMORY_S_INFO
{
    uint32_t UnixTime;
    uint16_t ErrId;
    uint16_t Para;
} MODULE_MEMORY_S_INFO;
typedef struct MODULE_MEMORY_S_INFO_FILTER
{
    //  2
    uint32_t    BeginTime;              // mode-0:起始时间 mode-1/2:跳过条目数
    uint32_t    EndTime;
    //  1
    uint16_t    MaxNumber;              //  最大输出数量
    uint8_t     Mode;                   //  0-按时段 1-首条起始 2-尾条起始
    uint8_t     Flag_PrintToDebug   :1; //  标志  -   打印到Debug接口
    uint8_t     Flag_PrintToMem     :1; //  标志  -   打印到缓存
    uint8_t     Flag_Reverse        :1; //  标志  -   倒序打印
    uint8_t     Flag_Enable_Err     :1; //  标志  -   错误使能
    uint8_t     Flag_Enable_Warn    :1; //  标志  -   警告使能
    uint8_t     Flag_Enable_Info    :1; //  标志  -   信息使能
    uint8_t     Flag_Enable_Debug   :1; //  标志  -   信息使能
    uint8_t     Flag_res            :1;
    //  2
    uint16_t    *pInInclude;            //  指定信息
    MODULE_MEMORY_E_INFO_ERR    *pOutBuf;   //  输出缓存
    //  1
    uint16_t    IncludeNum;             //  指定  信息数量(0-不使能指定信息滤波)
    uint16_t    OutBufNum;              //  实际输出数量
    //
} MODULE_MEMORY_S_INFO_FILTER;
//-------------------接口宏定义(硬件无关)--------------------指令
//----------恢复出厂设置等级
typedef enum MODULE_MEMORY_CMD_FACTORY_MODE
{
    MODULE_MEMORY_CMD_FACTORY_MODE_HALF=0,
    MODULE_MEMORY_CMD_FACTORY_MODE_ALL,
} MODULE_MEMORY_CMD_FACTORY_MODE;
//----------操作指令
enum MODULE_MEMORY_APP_CMD
{
    //正常读写操作
    MODULE_MEMORY_APP_CMD_GLOBAL_R = 0,
    MODULE_MEMORY_APP_CMD_GLOBAL_W,
    MODULE_MEMORY_APP_CMD_FACTORY,
    MODULE_MEMORY_APP_CMD_HARDFAULT,
    //信息记录
    MODULE_MEMORY_APP_CMD_INFO_W,
    MODULE_MEMORY_APP_CMD_INFO_R,
    //外部Flash操作指令
    MODULE_MEMORY_APP_CMD_EXTFLASH_READ,
    MODULE_MEMORY_APP_CMD_EXTFLASH_WRITE,
    MODULE_MEMORY_APP_CMD_EXTFLASH_EARSE,
    MODULE_MEMORY_APP_CMD_EXTFLASH_FORMAT,
    //isleep专用
    MODULE_MEMORY_APP_CMD_DAY_INFO_R,
    MODULE_MEMORY_APP_CMD_DAY_INFO_W,
    MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,
    MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_W,
    MODULE_MEMORY_APP_CMD_DAY_BREATH_R,
    MODULE_MEMORY_APP_CMD_DAY_BREATH_W,
    MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,
    MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_W,
    MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,
    MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_W,
    MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_R,
    MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_W,
    MODULE_MEMORY_APP_CMD_MOVE_DATE_R,
    MODULE_MEMORY_APP_CMD_MOVE_10MIN_R,
    MODULE_MEMORY_APP_CMD_MOVE_1MIN_W,
    MODULE_MEMORY_APP_CMD_MOVE_10MIN_W,
    MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_R,         /*获取上传的720个数据*/
    MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_VALID_R,   /*查看有没有需要上传的数据(可选:按日期)*/
    MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_W,         /*更新位图(按日期和索引置低)*/
    MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_RESET,     /*更新位图(按日期置高)*/
    MODULE_MEMORY_APP_CMD_MOVE_FORMAT,
    //icareB专用
    MODULE_MEMORY_APP_CMD_WEIGHT_W,
    MODULE_MEMORY_APP_CMD_WEIGHT_R,
    MODULE_MEMORY_APP_CMD_WEIGHT_CLR,
};
//-------------------接口变量--------------------------------
extern MODULE_MEMORY_S_PARA *ModuleMemory_psPara;
//-------------------接口函数--------------------------------
extern void       Module_Memory_Init(void);
extern uint8_t    Module_Memory_App(uint16_t Cmd,uint8_t *pBuf,void *pPara);
extern void       Module_Memory_DebugTest_OnOff(uint8_t OnOff);
extern void       Module_Memory_DebugTest_FactoryOnOff(uint8_t OnOff);
extern void       Module_Memory_DebugTest_ReadParaOnOff(uint8_t OnOff);
extern void       Module_Memory_DebugTest_PrintInfo(uint8_t OnOff);
//-----------------------------------------------------------
#endif

