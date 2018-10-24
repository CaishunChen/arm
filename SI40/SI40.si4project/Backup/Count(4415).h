/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-07-06
***********************************************************************************
*/
//--------------------------------
#ifndef __COUNT_H
#define __COUNT_H
//-------------------加载库函数------------------------------
#include "includes.h"

#ifndef BOOTLOADER
#include "arm_math.h"
#include "math_helper.h"
#endif
//-------------------------------------------------------------------------------宏定义(编译开关)
#if		(defined(PROJECT_BIB_TEST1))
//---->
//<----
#elif (defined(STM32F1) || defined(STM32F4))
//---->
#define COUNT_CRC16_ENABLE
//#define COUNT_CRC32_ENABLE
#define COUNT_MODBUS_ENABLE
#define COUNT_FFT_ENABLE
//#define COUNT_AES128_ENABLE
#define COUNT_NON_LINEAR_EQUATION
#define COUNT_SPLINE3_ENABLE
#define COUNT_GA_ENABLE
//#define COUNT_ACO_ENABLE
//<----
#elif (defined(NRF51) || defined(NRF52))
//---->
#define COUNT_CRC16_ENABLE
//#define COUNT_CRC32_ENABLE
#define COUNT_MODBUS_ENABLE
//#define COUNT_FFT_ENABLE
//#define COUNT_AES128_ENABLE
#define COUNT_NON_LINEAR_EQUATION
//#define COUNT_SPLINE3_ENABLE
//#define COUNT_GA_ENABLE
//#define COUNT_ACO_ENABLE
//<----
#else
//---->
//<----
#endif
//-------------------------------------------------------------------------------数据类型
typedef union Count_U_WordToByte
{
  	uint16_t words;
	struct
  	{
     	uint8_t _0;
     	uint8_t _1;
  	}bytes;
}Count_U_WordToByte;
typedef union Count_U_LongToByte
{
   uint32_t dWords;
   struct
   {
      uint8_t _0;
      uint8_t _1;
      uint8_t _2;
      uint8_t _3;
   }bytes;
}Count_U_LongToByte;
typedef struct COUNT_S_DATATYPE
{
   uint8_t  *pu8;
   int8_t   *p8;
   uint16_t *pu16;
   int16_t  *p16;
   uint32_t *pu32;
   int32_t  *p32;
   float    *pf;
   //备用
   char     *pres[3];
}COUNT_S_DATATYPE;
//-------------------------------------------------------------------------------常数定义
#define COUNT_Q7        (128.0f)
#define COUNT_Q15       (32768.0f)
#define COUNT_Q30       (1073741824.0f)
#define COUNT_Q31       (2147483648.0f)
//-------------------------------------------------------------------------------宏定义(运算)
#define COUNT_MAX(x,y)                (((x) > (y))? (x) : (y))
#define COUNT_MIN(x,y)                (((x) < (y))? (x) : (y))
#define COUNT_ABS(x)                  ((x)>0 ? (x) : -(x))
#define COUNT_CHECKVAL(val, min,max)  ((val < min || val > max) ? 0 : 1)
#define COUNT_EXCHANGE(x,y)           ((*x)^=(*y)^=(*x)^=(*y))
//These operations extract a byte from an u32 data:
#define Count_ByteL(x)  (x & 0xff)         
#define Count_ByteML(x) ((x >> 8) & 0xff)
#define Count_ByteMH(x) ((x >> 16) & 0xff)
#define Count_ByteH(x)  (x >> 24)
//
#define Count_2ByteToWord(b0, b1)         ((uint16_t)(b0) << 8 | (b1))
#define Count_4ByteToLong(b0, b1, b2, b3) ((uint32_t)(b0) << 24 | (uint32_t)(b1) << 16 | (uint32_t)(b2) << 8 | (b3))
//矩阵位置索引
#define COUNT_ARR_INDEX(row, col, dim)  ((row) * (dim) + (col))
//-------------------------------------------------------------------------------函数
extern void Count_Init(void);
extern void Count_DebugTestOnOff(uint8_t OnOff);
//------------------------------------------------------------延时
extern void Count_DelayUs(uint32_t x);
extern void Count_SysTickDelayUs(uint32_t n);
extern void Count_GetTimeMs(uint32_t *pms);
//------------------------------------------------------------随机数
#define  COUNT_RAND_MAX 0x7fff
extern int Count_Rand(void);
//------------------------------------------------------------不快指数
extern uint8_t Count_THI(int8_t temp,uint8_t humi,uint8_t *ThiValue);
//------------------------------------------------------------大小端转换
extern uint16_t Count_Int16ByteReversed(uint16_t val);
extern uint32_t Count_Int32ByteReversed(uint32_t val);
//------------------------------------------------------------循环加(减)
uint8_t Count_AddCyc(uint8_t degindata,uint8_t num,uint8_t maxdata);
uint8_t Count_SubCyc(uint8_t degindata,uint8_t num,uint8_t maxdata);
//------------------------------------------------------------求和
extern uint16_t Count_Sum(uint16_t BeginSum,uint8_t *Buf,uint16_t len);
extern uint32_t Count_SumMax(uint32_t BeginSum,uint8_t *pBuf,uint32_t len);
extern uint32_t Count_Sum16(uint32_t BeginSum,uint16_t *pBuf,uint16_t len);
//------------------------------------------------------------求最大/最小/平均值
extern uint16_t Count_MaxMinAve_Byte(uint8_t* pbuf,uint16_t len,uint8_t filterMin,uint8_t filterMax,uint8_t* pmax,uint8_t* pmin,uint8_t* pave);
//------------------------------------------------------------求m^n
extern uint32_t Count_Pow(uint8_t m,uint8_t n);
//------------------------------------------------------------求异或
extern uint8_t Count_Xor(uint8_t BeginXor,uint8_t *p,uint32_t len);
//------------------------------------------------------------求平均
extern uint16_t Count_Average8(uint8_t *p,uint16_t len);
extern uint16_t Count_Average16(uint16_t *p,uint16_t len);
extern uint32_t Count_Average32(uint32_t *p,uint16_t len);
//------------------------------------------------------------颜色转换
extern void Count_ColorConver(uint8_t cmd,uint32_t *p32,uint8_t *p8);
//------------------------------------------------------------求CRC
#ifdef COUNT_CRC16_ENABLE
extern uint16_t Count_CRC16(uint8_t *pchMsg, uint16_t DataLen);
#endif
#ifdef COUNT_CRC32_ENABLE
extern uint32_t Count_CRC32(uint8_t *databuf,uint32_t len);
#endif
//------------------------------------------------------------AES128
#ifdef COUNT_AES128_ENABLE
//Used to select the AES algorithm version to use.
//  - COUNT_AES128_ALGORITHM=1, version with 522 bytes of look-up tables, slower than version 2.
//  - COUNT_AES128_ALGORITHM=2, version with 2048 bytes of look-up tables, faster than version 1.
#define COUNT_AES128_ALGORITHM	1
//Number of 32 bit words to store an AES128 block.
#define COUNT_AES128_BLOCK_SIZE  4  
//Number of 32 bit words to store an AES128 key.
#define COUNT_AES128_KEY_SIZE    4  
//Number of 32bits words to store in an AES128 expanded key.
#define COUNT_AES128_EXPKEY_SIZE 44 
//According to key computes the expanded key exp for AES128 encryption.
extern void Count_AES128_KeyscheduleEnc(uint32_t* key, uint32_t* exp);
//According to key computes the expanded key exp for AES128 decryption.
extern void Count_AES128_KeyscheduleDec(uint32_t* key, uint32_t* exp);
//Encrypts, according to the expanded key expkey, one block of 16 bytes 
//at address 'input_pointer' into the block at address 'output_pointer'.
//They can be the same.
extern void Count_AES128_encrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey);
//Decrypts, according to the expanded key expkey, one block of 16 bytes 
//at address 'input_pointer' into the block at address 'output_pointer'.
//They can be the same.
extern void Count_AES128_decrypt(uint32_t* input_pointer, uint32_t* output_pointer, uint32_t* expkey);
//
extern void Count_AES128_Test(void);
extern void Count_AES128_1msPro(void);
//
//void AES128_encrypt_Project(uint32_t* pkey,uint8_t *psbuf,uint8_t *pdbuf,uint8_t *plen);
//void AES128_decrypt_Project(uint32_t* pkey,uint8_t *psbuf,uint8_t *pdbuf,uint8_t *plen);
#endif
//------------------------------------------------------------16单字节与BCD单字节
extern uint8_t Count_BcdToInt8(uint8_t val);
extern uint8_t Count_Int8ToBcd(uint8_t val);
//------------------------------------------------------------BCD字符串与uint32_t
extern uint32_t Count_StringBcdToInt(uint8_t *str, uint16_t len);
extern void Count_IntToStringBcd(uint8_t *str, uint16_t size, uint32_t val);
//------------------------------------------------------------ASC与16
extern int32_t Count_AsciiToHex(uint8_t *O_data,uint8_t *r_data,uint32_t len);
extern uint32_t Count_HexToAscii(uint8_t *data, uint8_t *buffer, uint32_t len);
//------------------------------------------------------------字符串
extern uint8_t Count_StrToLong(int8_t *str,uint8_t len,long *num);
extern uint8_t Count_Capital(uint8_t chr);
//------------------------------------------------------------IP PORT格式化
extern uint8_t Count_IP_Format(uint8_t * ip_asc,uint8_t *ip_buf);
//------------------------------------------------------------状态滤波
typedef struct
{
    int8_t state;
    int8_t judgeCount;
} COUNT_S_STATUS;
extern int8_t Count_StatusFilter(COUNT_S_STATUS *pState, int8_t newState, int8_t judgeCount);
//------------------------------------------------------------缓存环求和
extern void Count_BufLoopSum(uint16_t *pbuf,uint16_t bufMaxNum,uint16_t value,uint32_t *psum);
//------------------------------------------------------------数据分段
extern uint16_t Count_Classification(uint32_t input_data,uint32_t *ptable,uint16_t table_len);
//------------------------------------------------------------时间
//BCD 时间结构
typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
} BCDTIME_t;
extern struct tm Count_Time_ConvUnixToCalendar(time_t t);
extern time_t Count_Time_ConvCalendarToUnix(struct tm t);
extern uint32_t Count_BCDTimeToSec(BCDTIME_t *time);
extern void Count_SecToBCDTime(BCDTIME_t *tim, uint32_t sec);
extern int32_t Count_TimeCompare(struct tm *tm1,struct tm *tm2);
extern uint8_t Count_YYMMDDToWeek(uint8_t yy,uint8_t mm,uint8_t dd);
extern int32_t Count_ZellerWeek(int32_t year,int32_t month,int32_t day);
extern uint8_t Count_TimeTypeConvert(uint8_t mode,struct tm* tm_time,uint8_t* time);
extern uint8_t Count_TimeOffset(uint8_t *pInTime6,uint8_t *pOutTime6,int32_t offset_second);
//------------------------------------------------------------位图
extern uint8_t Count_Bitmap_Read(uint8_t *bitmap,uint16_t bnum);
extern uint8_t Count_Bitmap_Write(uint8_t *bitmap,uint16_t bnum,uint8_t state);
extern void Count_Bitmap_Search(uint8_t *bitmap,uint16_t *bnum,uint8_t state,uint16_t maxbnum);
//------------------------------------------------------------线模块阻值计算
extern void Count_LineMode_R(void);
//------------------------------------------------------------傅立叶实验
extern void Count_DoubleSin(long nfill, long Fs, long Freq1, long Freq2, long Ampli);
extern void Count_FFT_Test(uint32_t freqinc1,uint32_t freqinc2);
//------------------------------------------------------------缓存环(应用程序定义空间)
typedef struct COUNT_BUFFERLOOP_S_NODE
{
    uint8_t   *buf;             //数据:
    uint16_t  len;              //长度:
    uint16_t  TimeoutCount_ms;  //超时计时器
} COUNT_BUFFERLOOP_S_NODE;
//端点
typedef struct COUNT_BUFFERLOOP_S_LIMIT
{
    uint16_t  beginAddr;        //缓存环起始地址
    uint16_t  endAddr;          //缓存环结束地址
    uint8_t   beginNode;        //缓存环起始结点
    uint8_t   endNode;          //缓存环结束结点
} COUNT_BUFFERLOOP_S_LIMIT;
typedef struct COUNT_BUFFERLOOP_S
{
    uint8_t *pBuf;
    uint16_t len;
    COUNT_BUFFERLOOP_S_NODE *psNodeBuf;
    uint16_t NodeNum;
    COUNT_BUFFERLOOP_S_LIMIT *pslimit;
}COUNT_BUFFERLOOP_S;
typedef enum COUNT_BUFFERLOOP_E_POPMODE
{
   COUNT_BUFFERLOOP_E_POPMODE_DELETE=0,
   COUNT_BUFFERLOOP_E_POPMODE_KEEP,   
}COUNT_BUFFERLOOP_E_POPMODE;
extern void Count_BufferLoopInit(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t len,COUNT_BUFFERLOOP_S_NODE *psNodeBuf,uint16_t snodeBufNum,COUNT_BUFFERLOOP_S_LIMIT *pslimit);
extern uint8_t Count_BufferLoopPush(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t len);
extern uint8_t Count_BufferLoopPop(COUNT_BUFFERLOOP_S *ps,uint8_t *pBuf,uint16_t* len,COUNT_BUFFERLOOP_E_POPMODE mode);
//------------------------------------------------------------modbus
//最大通讯长度
#define COUNT_MODBUS_MAX_MESSAGE_LENGTH            256
//广播地址
#define COUNT_MODBUS_BROADCAST_ADDR                0xFF
//最大扩展串口数量
#define COUNT_MODBUS_MAX_EX_UART_MAX               4
//数据传输先传高字节字节(使能后:crc16先传低字节;字和长整型先传高字节,数组先传低字节)
//#define COUNT_MODBUS_TRANSFER_FIRST_HIGH
//标准功能码
#define COUNT_MODBUS_FC_READ_COILS                 0x01
#define COUNT_MODBUS_FC_READ_DISCRETE_INPUTS       0x02
#define COUNT_MODBUS_FC_READ_HOLDING_REGISTERS     0x03
#define COUNT_MODBUS_FC_READ_INPUT_REGISTERS       0x04
#define COUNT_MODBUS_FC_WRITE_SINGLE_COIL          0x05
#define COUNT_MODBUS_FC_WRITE_SINGLE_REGISTER      0x06
#define COUNT_MODBUS_FC_READ_EXCEPTION_STATUS      0x07
#define COUNT_MODBUS_FC_WRITE_MULTIPLE_COILS       0x0F
#define COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS   0x10
#define COUNT_MODBUS_FC_REPORT_SLAVE_ID            0x11
#define COUNT_MODBUS_FC_MASK_WRITE_REGISTER        0x16
#define COUNT_MODBUS_FC_WRITE_AND_READ_REGISTERS   0x17
//自定义功能码
#define COUNT_MODBUS_FC_USER_UART_WRITE_BLOCK_READ 0x08  //写串口,之后阻塞读()
#define COUNT_MODBUS_FC_WRITE_IAP                  0x60
#define COUNT_MODBUS_FC_LOADFILE                   0x61
#define COUNT_MODBUS_FC_UPLOAD                     0x62
//寄存器地址
//串口参数配置信息
typedef struct S_COUNT_MODBUS_UARTPARA
{
    uint8_t BaudRate;   //波特率:   1-300 2-600 3-1200 4-2400 5-4800 6-9600 7-19200 8-38400 9-57600 10-115200
    uint8_t DataBit;    //数据位:   5/6/7/8
    uint8_t StopBit;    //停止位:   0/1
    uint8_t Parity;     //奇偶校验: 0/1/2
    uint8_t ReSendNum;  //重发次数: 1/2/3/4
    uint8_t res[1];     //备用
} S_COUNT_MODBUS_UARTPARA;
#define COUNT_MODBUS_REGISTERS_ADDR_UART_PARA      0x0000
#define COUNT_MODBUS_REGISTERS_LEN_UART_PARA       (COUNT_MODBUS_MAX_EX_UART_MAX*sizeof(S_COUNT_MODBUS_UARTPARA))
#define COUNT_MODBUS_REGISTERS_SIZE_UART_PARA      ((((COUNT_MODBUS_REGISTERS_LEN_UART_PARA-1)/32)+1)*32)
//串口读取寄存器
#define COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_R   0x6000
#define COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_R   0x7000
#define COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_R   0x8000
#define COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_R   0x9000
#define COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_R   0xA000
#define COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_R   0x1000 
//串口循环发送配置寄存器
typedef struct S_COUNT_MODBUS_UART_AUTO_W
{
   uint8_t Interval_ms; //帧间隔
   uint8_t res[3];      //因为操作内部flash为32位操作,所以要匹配为4的倍数.
   uint8_t Txbuf[8];    //本设备不用知道数据内容,但一般为modbus(地址1B+功能码1B+起始地址2B+数据字长度2B+CRC162B)
}S_COUNT_MODBUS_UART_AUTO_W;
#define COUNT_MODBUS_REGISTERS_ADDR_UART1_AUTO_W   0xB000
#define COUNT_MODBUS_REGISTERS_ADDR_UART2_AUTO_W   0xC000
#define COUNT_MODBUS_REGISTERS_ADDR_UART3_AUTO_W   0xD000
#define COUNT_MODBUS_REGISTERS_ADDR_UART4_AUTO_W   0xE000
#define COUNT_MODBUS_REGISTERS_ADDR_UART5_AUTO_W   0xF000
#define COUNT_MODBUS_REGISTERS_SIZE_UARTX_AUTO_W   0x1000 
//
#define COUNT_MODBUS_REGISTERS_ADDR_BEGIN_XSL      0x2000
#define COUNT_MODBUS_REGISTERS_ADDR_RTC            COUNT_MODBUS_REGISTERS_ADDR_BEGIN_XSL  //44字节,预算64字节
#define COUNT_MODBUS_REGISTERS_LEN_RTC             sizeof(struct tm)                      //44字节
#define COUNT_MODBUS_REGISTERS_SIZE_RTC            64                                     //
#define COUNT_MODBUS_REGISTERS_ADDR_AD             (COUNT_MODBUS_REGISTERS_ADDR_RTC+COUNT_MODBUS_REGISTERS_SIZE_RTC)
#define COUNT_MODBUS_REGISTERS_LEN_AD              24
#define COUNT_MODBUS_REGISTERS_SIZE_AD             32
#define COUNT_MODBUS_REGISTERS_ADDR_KIN            (COUNT_MODBUS_REGISTERS_ADDR_AD+COUNT_MODBUS_REGISTERS_SIZE_AD)
#define COUNT_MODBUS_REGISTERS_LEN_KIN             2
#define COUNT_MODBUS_REGISTERS_SIZE_KIN            32
#define COUNT_MODBUS_REGISTERS_ADDR_KOUT           (COUNT_MODBUS_REGISTERS_ADDR_KIN+COUNT_MODBUS_REGISTERS_SIZE_KIN)
#define COUNT_MODBUS_REGISTERS_LEN_KOUT            2
#define COUNT_MODBUS_REGISTERS_SIZE_KOUT           32
#define COUNT_MODBUS_REGISTERS_ADDR_STATE          (COUNT_MODBUS_REGISTERS_ADDR_KOUT+COUNT_MODBUS_REGISTERS_SIZE_KOUT)
#define COUNT_MODBUS_REGISTERS_LEN_STATE           16
#define COUNT_MODBUS_REGISTERS_SIZE_STATE          32
#define COUNT_MODBUS_REGISTERS_ADDR_PARA           (COUNT_MODBUS_REGISTERS_ADDR_STATE+COUNT_MODBUS_REGISTERS_SIZE_STATE)
#define COUNT_MODBUS_REGISTERS_LEN_PARA            64
#define COUNT_MODBUS_REGISTERS_SIZE_PARA           64
#define COUNT_MODBUS_REGISTERS_ADDR_CONTROL        (COUNT_MODBUS_REGISTERS_ADDR_PARA+COUNT_MODBUS_REGISTERS_SIZE_PARA)
#define COUNT_MODBUS_REGISTERS_LEN_CONTROL         2
#define COUNT_MODBUS_REGISTERS_SIZE_CONTROL        2
//串口(非阻塞操作)
#define COUNT_MODBUS_REGISTERS_LEN_UART            256
#define COUNT_MODBUS_REGISTERS_SIZE_UART           256
#define COUNT_MODBUS_REGISTERS_ADDR_UART1          (COUNT_MODBUS_REGISTERS_ADDR_CONTROL+COUNT_MODBUS_REGISTERS_SIZE_CONTROL)
#define COUNT_MODBUS_REGISTERS_ADDR_UART2          (COUNT_MODBUS_REGISTERS_ADDR_UART1+COUNT_MODBUS_REGISTERS_SIZE_UART)
#define COUNT_MODBUS_REGISTERS_ADDR_UART3          (COUNT_MODBUS_REGISTERS_ADDR_UART2+COUNT_MODBUS_REGISTERS_SIZE_UART)
#define COUNT_MODBUS_REGISTERS_ADDR_UART4          (COUNT_MODBUS_REGISTERS_ADDR_UART3+COUNT_MODBUS_REGISTERS_SIZE_UART)
#define COUNT_MODBUS_REGISTERS_ADDR_UART5          (COUNT_MODBUS_REGISTERS_ADDR_UART4+COUNT_MODBUS_REGISTERS_SIZE_UART)
//
//变量
extern uint8_t Count_ModbusSlaveAddr;
//函数
extern void Count_Modbus_Array(uint8_t *pOutBuf,uint16_t *pOutLen,uint8_t slaveAddr,uint8_t functionCode,uint16_t *pDataBeginAddr,uint16_t *pDataWordLen,uint8_t *pDataByteLen,uint8_t *pDataBuf,uint16_t DataLen);
extern uint8_t Count_Modbus_Check(uint8_t *pBuf,uint16_t len,uint8_t addr);
//------------------------------------------------------------缓存环(有待与前面的程序整合)

#define DEBUG_FULL_ASSERT       0
//
#if     DEBUG_FULL_ASSERT
#define ASSERT_PARAM(a)          ((a) ? (void)0 : ASSERT_FAILED((uint8_t *)__FILE__, __LINE__))
extern void ASSERT_FAILED(uint8_t* file, uint32_t line);
#else
#define ASSERT_PARAM(a)
#endif
//
typedef volatile struct
{
    volatile uint32_t   bufSize;        //缓冲区总大小
    volatile uint8_t    *pStart;        //起始地址
    volatile uint8_t    *pEnd;          //结束地址
    volatile uint8_t    *pBuf;          //缓冲区首地址
} QUEUE8_t;

typedef volatile struct
{
    volatile uint32_t   elemSize;       //结构体单元大小
    volatile uint32_t   sumCount;       //结构体单元的最大个数
    volatile uint32_t   start;          //结束结构体地址
    volatile uint32_t   end;            //缓冲区首地址
    volatile void       *pBuf;          //起始结构体地址
} QUEUE_STRUCT_t;

extern uint32_t QUEUE_PacketCreate(QUEUE8_t *pQ8, uint8_t *pBuf, uint32_t bufSize);
extern uint32_t QUEUE_PacketIn(QUEUE8_t *pQ8, uint8_t *pData, uint32_t len);
extern uint32_t QUEUE_PacketOut(QUEUE8_t *pQ8, uint8_t *pData, uint32_t dataLen);
extern uint32_t QUEUE_PacketLengthGet(QUEUE8_t *pQ8);

extern uint32_t QUEUE_PacketCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
extern uint32_t QUEUE_PacketStartEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
extern uint32_t QUEUE_PacketStartEndDifferentCharSplit(QUEUE8_t *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen);
extern uint32_t QUEUE_PacketDoubleEndCharSplit(QUEUE8_t *pQ8, uint8_t splitChar1, uint8_t splitChar2, uint8_t *pData, uint32_t dataLen);

extern uint32_t QUEUE_StructCreate(QUEUE_STRUCT_t *pQueue, void *pBuf, uint32_t bufSize, uint16_t blkSize);
extern uint32_t QUEUE_StructIn(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount);
extern uint32_t QUEUE_StructOut(QUEUE_STRUCT_t *pQueue, void *pData, uint32_t blkCount);
extern uint32_t QUEUE_StructCountGet(QUEUE_STRUCT_t *pQueue);

//------------------------------------------------------------插值算法之三次样条
//定义样条数据样本最多个数
#define  COUNT_SPLINE3_MAXNUM  (1000/4)
//定义样条结构体，用于存储一条样条的所有信息
typedef struct COUNT_S_SPLINE3
{
    //---输入
    float *pInX;        // 样本x
    float *pInY;        // 样本y
    uint16_t InNum;     // 样本数量
    uint8_t BoundType;  // 边界条件(1-第一类,2-第二类)
    uint8_t res;        // 补齐备用
    float BoundBegin;   // 首个样本边界条件(导数)
    float BoundEnd;     // 末尾样本边界条件(导数)
    float *pOutX;       // 输出x
    uint32_t OutNum;    // 输出点有效个数
    //---输出
    float *pOutY;       // 输出y
} COUNT_S_SPLINE3;
uint8_t Count_Spline3(COUNT_S_SPLINE3* ps);
//------------------------------------------------------------非线性方程解析
//二分逼近法
#define COUNT_DICHOTOMY_EQUATION_PRECISION   0.000000001
typedef double (*pCount_DichotomyEquationFunction)(double);
extern double Count_DichotomyEquation(double a, double b, pCount_DichotomyEquationFunction f,uint16_t *pNum);
//牛顿迭代法
#define COUNT_NEWTON_RAPHSON_PRECISION       0.000000001
typedef double (*pCount_NewtonRaphsonFunction)(double);
double Count_NewtonRaphson(pCount_NewtonRaphsonFunction f, double x0,uint16_t *pNum);
//------------------------------------------------------------遗传算法
//------------------------------------------------------------3D
uint8_t Count_Quaternion_To_EulerAngle(float *p_quat,float *p_ea);
//------------------------------------------------------------STM32-DSP库函数
//数据结构定义
#define  q30_t       int32_t
#define  float32_t   float
//基本
//---绝对值
//void arm_abs_f32     (float32_t *pSrc , float32_t *pDst , uint32_t blockSize);
//void arm_abs_q31     (q31_t     *pSrc , q31_t     *pDst , uint32_t blockSize);
//void arm_abs_q15     (q15_t     *pSrc , q15_t     *pDst , uint32_t blockSize);
//void arm_abs_q7      (q7_t      *pSrc , q7_t      *pDst , uint32_t blockSize);
//---求和
//void arm_add_f32     (float32_t *pSrcA , float32_t *pSrcB , float32_t *pDst , uint32_t blockSize);
//void arm_add_q31     (q31_t     *pSrcA , q31_t     *pSrcB , q31_t     *pDst , uint32_t blockSize);
//void arm_add_q15     (q15_t     *pSrcA , q15_t     *pSrcB , q15_t     *pDst , uint32_t blockSize);
//void arm_add_q7      (q7_t      *pSrcA , q7_t      *pSrcB , q7_t      *pDst , uint32_t blockSize);
//---点乘
//void arm_dot_prod_f32(float32_t *pSrcA , float32_t *pSrcB , uint32_t blockSize , float32_t *result);
//void arm_dot_prod_q31(q31_t     *pSrcA , q31_t     *pSrcB , uint32_t blockSize , q63_t     *result);
//void arm_dot_prod_q15(q15_t     *pSrcA , q15_t     *pSrcB , uint32_t blockSize , q63_t     *result);
//void arm_dot_prod_q7 (q7_t      *pSrcA , q7_t      *pSrcB , uint32_t blockSize , q63_t     *result);
//---乘法
//void arm_mult_f32    (float32_t *pSrcA , float32_t *pSrcB , float32_t *pDst , uint32_t blockSize);
//void arm_mult_q31    (q31_t     *pSrcA , q31_t     *pSrcB , q31_t     *pDst , uint32_t blockSize);
//void arm_mult_q15    (q15_t     *pSrcA , q15_t     *pSrcB , q15_t     *pDst , uint32_t blockSize);
//void arm_mult_q7     (q7_t      *pSrcA , q7_t      *pSrcB , q7_t      *pDst , uint32_t blockSize);
//---相反数
//void arm_negate_f32  (float32_t *pSrc , float32_t *pDst , uint32_t blockSize);
//void arm_negate_q31  (q31_t     *pSrc , q31_t     *pDst , uint32_t blockSize);
//void arm_negate_q15  (q15_t     *pSrc , q15_t     *pDst , uint32_t blockSize);
//void arm_negate_q7   (q7_t      *pSrc , q7_t      *pDst , uint32_t blockSize);
//---偏移
//void arm_offset_f32  (float32_t *pSrc , float32_t offset , float32_t *pDst , uint32_t blockSize);
//void arm_offset_q31  (q31_t     *pSrc , q31_t     offset , q31_t     *pDst , uint32_t blockSize);
//void arm_offset_q15  (q15_t     *pSrc , q15_t     offset , q15_t     *pDst , uint32_t blockSize);
//void arm_offset_q7   (q7_t      *pSrc , q7_t      offset , q7_t      *pDst , uint32_t blockSize);
//---位移
//void arm_shift_q31   (q31_t *pSrc , int8_t shiftBits , q31_t * pDst , uint32_t blockSize);
//void arm_shift_q15   (q15_t *pSrc , int8_t shiftBits , q15_t * pDst , uint32_t blockSize);
//void arm_shift_q7    (q7_t  *pSrc , int8_t shiftBits , q7_t * pDst  , uint32_t blockSize);
//---减法
//void arm_sub_f32     (float32_t *pSrcA , float32_t *pSrcB , float32_t *pDst , uint32_t blockSize);
//void arm_sub_q31     (q31_t     *pSrcA , q31_t     *pSrcB , q31_t     *pDst , uint32_t blockSize);
//void arm_sub_q15     (q15_t     *pSrcA , q15_t     *pSrcB , q15_t     *pDst , uint32_t blockSize);
//void arm_sub_q7      (q7_t      *pSrcA , q7_t      *pSrcB , q7_t      *pDst , uint32_t blockSize);
//---比例因子
//void arm_scale_f32   (float32_t *pSrc , float32_t  scale , float32_t *pDst , uint32_t blockSize);
//void arm_scale_q31   (q31_t     *pSrc , q31_t scaleFract , int8_t    shift , q31_t   *pDst , uint32_t blockSize);
//void arm_scale_q15   (q15_t     *pSrc , q15_t scaleFract , int8_t    shift , q15_t   *pDst , uint32_t blockSize);
//void arm_scale_q7    (q7_t      *pSrc , q7_t  scaleFract , int8_t    shift , q7_t    *pDst , uint32_t blockSize);
//---三角函数
//float32_t   arm_cos_f32(float32_t   x);
//q31_t       arm_cos_q31(q31_t       x);
//q15_t       arm_cos_q15(q15_t       x);
//float32_t   arm_sine_f32(float32_t  x);
//q31_t       arm_sine_q31(q31_t      x);
//q15_t       arm_sine_q15(q15_t      x);
//void arm_sin_cos_f32(float32_t   theta, float32_t  *pSinVal, float32_t  *pCosVal)
//void arm_sin_cos_q31(q31_t       theta, q31_t      *pSinVal, q31_t      *pCosVal)
//---平方根
//arm_status  arm_sqrt_f32(float32_t  in, float32_t  *pOut)
//arm_status  arm_sqrt_q31(q31_t      in, q31_t      *pOut)
//arm_status  arm_sqrt_q15(q15_t      in, q15_t      *pOut);
//---最大值
//void arm_max_f32  (float32_t  *pSrc, uint32_t blockSize, float32_t   *pResult, uint32_t *pIndex)
//void arm_max_q31  (q31_t      *pSrc, uint32_t blockSize, q31_t       *pResult, uint32_t *pIndex)
//void arm_max_q15  (q15_t      *pSrc, uint32_t blockSize, q15_t       *pResult, uint32_t *pIndex)
//void arm_max_q7   (q7_t       *pSrc, uint32_t blockSize, q7_t        *pResult, uint32_t *pIndex)
//---最小值
//void arm_min_f32  (float32_t  *pSrc, uint32_t blockSize, float32_t   *pResult, uint32_t *pIndex)
//void arm_min_q31  (q31_t      *pSrc, uint32_t blockSize, q31_t       *pResult, uint32_t *pIndex)
//void arm_min_q15  (q15_t      *pSrc, uint32_t blockSize, q15_t       *pResult, uint32_t *pIndex)
//void arm_min_q7   (q7_t       *pSrc, uint32_t blockSize, q7_t        *pResult, uint32_t *pIndex)
//---平均值
//void arm_mean_f32 (float32_t  *pSrc, uint32_t blockSize, float32_t   *pResult)
//void arm_mean_q31 (q31_t      *pSrc, uint32_t blockSize, q31_t       *pResult)
//void arm_mean_q15 (q15_t      *pSrc, uint32_t blockSize, q15_t       *pResult)
//void arm_mean_q7  (q7_t       *pSrc, uint32_t blockSize, q7_t        *pResult)
//---功率
//void arm_power_f32(float32_t  *pSrc, uint32_t blockSize, float32_t   *pResult)
//void arm_power_q31(q31_t      *pSrc, uint32_t blockSize, q63_t       *pResult)
//void arm_power_q15(q15_t      *pSrc, uint32_t blockSize, q63_t       *pResult)
//void arm_power_q7 (q7_t       *pSrc, uint32_t blockSize, q31_t       *pResult)
//---标准偏差
//void arm_std_f32  (float32_t  *pSrc, uint32_t blockSize, float32_t   *pResult)
//void arm_std_q31  (q31_t      *pSrc, uint32_t blockSize, q31_t       *pResult)
//void arm_std_q15  (q15_t      *pSrc, uint32_t blockSize, q15_t       *pResult)
//---均方根RMS
//void arm_rms_f32  (float32_t  *pSrc, uint32_t blockSize, float32_t   * pResult)
//void arm_rms_q31  (q31_t      *pSrc, uint32_t blockSize, q31_t       * pResult)
//void arm_rms_q15  (q15_t      *pSrc, uint32_t blockSize, q15_t       * pResult)
//---方差
//void arm_var_f32  (float32_t  *pSrc, uint32_t blockSize, float32_t   * pResult)
//void arm_var_q31  (q31_t      *pSrc, uint32_t blockSize, q63_t       * pResult)
//void arm_var_q15  (q15_t      *pSrc, uint32_t blockSize, q31_t       * pResult)
//---数据拷贝
//void arm_copy_f32 (float32_t  *pSrc, float32_t  *pDst, uint32_t blockSize)
//void arm_copy_q31 (q31_t      *pSrc, q31_t      *pDst, uint32_t blockSize)
//void arm_copy_q15 (q15_t      *pSrc, q15_t      *pDst, uint32_t blockSize)
//void arm_copy_q7  (q7_t       *pSrc, q7_t       *pDst, uint32_t blockSize)
//---数据填充
//void arm_fill_f32 (float32_t  value, float32_t  *pDst, uint32_t blockSize)
//void arm_fill_q31 (q31_t      value, q31_t      *pDst, uint32_t blockSize)
//void arm_fill_q15 (q15_t      value, q15_t      *pDst, uint32_t blockSize)
//void arm_fill_q7  (q7_t       value, q7_t       *pDst, uint32_t blockSize)
//---浮点定点转换
//void arm_float_to_q31   (float32_t  *pSrc, q31_t      *pDst,   uint32_t blockSize)
//void arm_float_to_q15   (float32_t  *pSrc, q15_t      *pDst,   uint32_t blockSize)
//void arm_float_to_q7    (float32_t  *pSrc, q7_t       *pDst,   uint32_t blockSize)
//void arm_q7_to_float    (q7_t       *pSrc, float32_t  *pDst,   uint32_t blockSize)
//void arm_q7_to_q31      (q7_t       *pSrc, q31_t      *pDst,   uint32_t blockSize)
//void arm_q7_to_q15      (q7_t       *pSrc, q15_t      *pDst,   uint32_t blockSize)
//void arm_q15_to_float   (q15_t      *pSrc, float32_t  *pDst,   uint32_t blockSize)
//void arm_q15_to_q31     (q15_t      *pSrc, q31_t      *pDst,   uint32_t blockSize)
//void arm_q15_to_q7      (q15_t      *pSrc, q7_t       *pDst,   uint32_t blockSize)
void arm_q30_to_float   (q30_t      *pSrc, float32_t  *pDst,   uint32_t blockSize);
//void arm_q31_to_float   (q31_t      *pSrc, float32_t  *pDst,   uint32_t blockSize)
//void arm_q31_to_q15     (q31_t      *pSrc, q15_t      *pDst,   uint32_t blockSize)
//void arm_q31_to_q7      (q31_t      *pSrc, q7_t       *pDst,   uint32_t blockSize)
//==================================================
//---PID
//float32_t   arm_pid_f32       (arm_pid_instance_f32 * S, float32_t in)
//void        arm_pid_init_f32  (arm_pid_instance_f32 * S, int32_t resetStateFlag)
//void        arm_pid_reset_f32 (arm_pid_instance_f32 * S)
//q31_t       arm_pid_q31       (arm_pid_instance_q31 * S, q31_t in)
//void        arm_pid_init_q31  (arm_pid_instance_q31 * S, int32_t resetStateFlag)
//void        arm_pid_reset_q31 (arm_pid_instance_q31 * S)
//q15_t       arm_pid_q15       (arm_pid_instance_q31 * S, q31_t in)
//void        arm_pid_init_q15  (arm_pid_instance_q15 * S, int32_t resetStateFlag)
//void        arm_pid_reset_q15 (arm_pid_instance_q15 * S)
//==================================================
//复数
//---复数共轭运算
//void arm_cmplx_conj_f32(float32_t   *pSrc, float32_t  *pDst, uint32_t numSamples)
//void arm_cmplx_conj_q31(q31_t       *pSrc, q31_t      *pDst, uint32_t numSamples)
//void arm_cmplx_conj_q15(q15_t       *pSrc, q15_t      *pDst, uint32_t numSamples)
//---复数点乘
//void arm_cmplx_dot_prod_f32(float32_t * pSrcA, float32_t * pSrcB, uint32_t numSamples,float32_t * realResult, float32_t * imagResult)
//void arm_cmplx_dot_prod_q31(q31_t * pSrcA, q31_t * pSrcB, uint32_t numSamples,q63_t * realResult, q63_t * imagResult)
//void arm_cmplx_dot_prod_q15(q15_t * pSrcA, q15_t * pSrcB, uint32_t numSamples,q31_t * realResult, q31_t * imagResult)
//---复数求模
//void arm_cmplx_mag_f32(float32_t * pSrc, float32_t * pDst, uint32_t numSamples)
//void arm_cmplx_mag_q31(q31_t * pSrc, q31_t * pDst, uint32_t numSamples)
//void arm_cmplx_mag_q31(q31_t * pSrc, q31_t * pDst, uint32_t numSamples)
//---复数模平方
//void arm_cmplx_mag_squared_f32(float32_t * pSrc, float32_t * pDst, uint32_t numSamples)
//void arm_cmplx_mag_squared_q31(q31_t * pSrc, q31_t * pDst, uint32_t numSamples)
//void arm_cmplx_mag_squared_q15(q15_t * pSrc, q15_t * pDst, uint32_t numSamples)
//---复数乘法
//void arm_cmplx_mult_cmplx_f32(float32_t * pSrcA,float32_t * pSrcB,float32_t * pDst,uint32_t numSamples)
//void arm_cmplx_mult_cmplx_q31(q31_t * pSrcA,q31_t * pSrcB,q31_t * pDst,uint32_t numSamples)
//void arm_cmplx_mult_cmplx_q15(q15_t * pSrcA,q15_t * pSrcB,q15_t * pDst,uint32_t numSamples)
//---复数乘实数
//void arm_cmplx_mult_real_f32(float32_t * pSrcCmplx,float32_t * pSrcReal,float32_t * pCmplxDst,uint32_t numSamples)
//void arm_cmplx_mult_real_q31(q31_t * pSrcCmplx,q31_t * pSrcReal,q31_t * pCmplxDst,uint32_t numSamples)
//void arm_cmplx_mult_real_q15(q15_t * pSrcCmplx,q15_t * pSrcReal,q15_t * pCmplxDst,uint32_t numSamples)
//==================================================
//矩阵
//---矩阵初始化
//void arm_mat_init_f32(arm_matrix_instance_f32 * S,uint16_t nRows,uint16_t nColumns,float32_t * pData)
//void arm_mat_init_q31(arm_matrix_instance_q31 * S,uint16_t nRows,uint16_t nColumns,q31_t * pData)
//void arm_mat_init_q15(arm_matrix_instance_q15 * S,uint16_t nRows,uint16_t nColumns,q15_t * pData)
//---矩阵加法
//arm_status arm_mat_add_f32(const arm_matrix_instance_f32 * pSrcA,const arm_matrix_instance_f32 * pSrcB,arm_matrix_instance_f32 * pDst)
//arm_status arm_mat_add_q31(const arm_matrix_instance_q31 * pSrcA,const arm_matrix_instance_q31 * pSrcB,arm_matrix_instance_q31 * pDst)
//arm_status arm_mat_add_q15(const arm_matrix_instance_q15 * pSrcA,const arm_matrix_instance_q15 * pSrcB,arm_matrix_instance_q15 * pDst)
//---逆矩阵
//arm_status arm_mat_inverse_f32(const arm_matrix_instance_f32 * pSrc,arm_matrix_instance_f32 * pDst)
//---矩阵减法
//arm_status arm_mat_sub_f32(const arm_matrix_instance_f32 * pSrcA,const arm_matrix_instance_f32 * pSrcB,arm_matrix_instance_f32 * pDst)
//arm_status arm_mat_sub_q31(const arm_matrix_instance_q31 * pSrcA,const arm_matrix_instance_q31 * pSrcB,arm_matrix_instance_q31 * pDst);
//arm_status arm_mat_sub_q15(const arm_matrix_instance_q15 * pSrcA,const arm_matrix_instance_q15 * pSrcB,arm_matrix_instance_q15 * pDst);
//---矩阵缩放
//arm_status arm_mat_scale_f32(const arm_matrix_instance_f32 * pSrc,float32_t scale,arm_matrix_instance_f32 * pDst)
//arm_status arm_mat_scale_q31(const arm_matrix_instance_q31 * pSrc,q31_t scaleFract,int32_t shift,arm_matrix_instance_q31 * pDst)
//arm_status arm_mat_scale_q15(const arm_matrix_instance_q15 * pSrc,q15_t scaleFract,int32_t shift,arm_matrix_instance_q15 * pDst)
//---矩阵乘法
//arm_status arm_mat_mult_f32(const arm_matrix_instance_f32 * pSrcA,const arm_matrix_instance_f32 * pSrcB,arm_matrix_instance_f32 * pDst)
//arm_status arm_mat_mult_q31(const arm_matrix_instance_q31 * pSrcA,const arm_matrix_instance_q31 * pSrcB,arm_matrix_instance_q31 * pDst)
//arm_status arm_mat_mult_q15(const arm_matrix_instance_q15 * pSrcA,const arm_matrix_instance_q15 * pSrcB,arm_matrix_instance_q15 * pDst,q15_t * pState CMSIS_UNUSED)
//arm_status arm_mat_mult_fast_q31(const arm_matrix_instance_q31 * pSrcA,const arm_matrix_instance_q31 * pSrcB,arm_matrix_instance_q31 * pDst)
//arm_status arm_mat_mult_fast_q15(const arm_matrix_instance_q15 * pSrcA,const arm_matrix_instance_q15 * pSrcB,arm_matrix_instance_q15 * pDst,q15_t * pState)
//---矩阵转置
//arm_status arm_mat_trans_f32(const arm_matrix_instance_f32 * pSrc,arm_matrix_instance_f32 * pDst)
//arm_status arm_mat_trans_q31(const arm_matrix_instance_q31 * pSrc,arm_matrix_instance_q31 * pDst)
//arm_status arm_mat_trans_q15(const arm_matrix_instance_q15 * pSrc,arm_matrix_instance_q15 * pDst)
//==================================================
//插值
//---线性插值
//float32_t arm_linear_interp_f32(arm_linear_interp_instance_f32 * S , float32_t x)
//q31_t     arm_linear_interp_q31(q31_t *pYData , q31_t x , uint32_t nValues)
//q15_t     arm_linear_interp_q15(q15_t *pYData , q31_t x , uint32_t nValues)
//q7_t      arm_linear_interp_q7 (q7_t  *pYData , q31_t x , uint32_t nValues)
//---双线性插值
//float32_t arm_bilinear_interp_f32(const arm_bilinear_interp_instance_f32 *S , float32_t X , float32_t Y)
//q31_t     arm_bilinear_interp_q31(arm_bilinear_interp_instance_q31       *S , q31_t     X , q31_t     Y)
//q15_t     arm_bilinear_interp_q15(arm_bilinear_interp_instance_q15       *S , q31_t     X , q31_t     Y)
//q7_t      arm_bilinear_interp_q7 (arm_bilinear_interp_instance_q7        *S , q31_t     X , q31_t     Y)
//==================================================
//傅立叶变换
//---汇编FFT
//------64点FFT
//void cr4_fft_64_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
//------256点FFT
//void cr4_fft_256_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
//------1024点FFT
//void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);
//==================================================
//复数傅立叶
//---浮点
//void arm_cfft_f32(const arm_cfft_instance_f32 * S,float32_t * p1,uint8_t ifftFlag,uint8_t bitReverseFlag);
//---复数FFT-基2算法
//void arm_cfft_radix2_q15(const arm_cfft_radix2_instance_q15 * S,q15_t * pSrc);
//void arm_cfft_radix2_q31(const arm_cfft_radix2_instance_q31 * S,q31_t * pSrc);
//---复数FFT-基4算法
//void arm_cfft_radix4_q15(const arm_cfft_radix4_instance_q15 * S,q15_t * pSrc);
//void arm_cfft_radix4_q31(const arm_cfft_radix4_instance_q31 * S,q31_t * pSrc);
//==================================================
//实数FFT逆变换
//void arm_rfft_fast_f32(arm_rfft_fast_instance_f32 * S,float32_t * p, float32_t * pOut,uint8_t ifftFlag);
//==================================================
//滤波器
//---汇编IIR
//void iirarma_stm32(void *y, void *x, uint16_t *h2, uint16_t *h1, uint32_t ny );
//---汇编FIR
//void fir_16by16_stm32(void *y, void *x, COEFS *c, uint32_t N);
//---C语言IIR
//void iir_biquad_stm32(uint16_t *y, uint16_t *x, int16_t *IIRCoeff, uint16_t ny);
//==================================================
//PID
//---汇编PID
//uint16_t PID_stm32(uint16_t Error, uint16_t *Coeff);
//---C语言PID
//uint16_t DoPID(uint16_t Error, uint16_t *Coeff);
//uint16_t DoFullPID(uint16_t In, uint16_t Ref, uint16_t *Coeff);
//==================================================
//math_help
//---信噪比
//float arm_snr_f32(float *pRef, float *pTest, uint32_t buffSize)
//---浮点数转换为Q12.20格式
//void arm_float_to_q12_20(float *pIn, q31_t * pOut, uint32_t numSamples)
//---给q15_t类型数据提供保护位,防止溢出
//void arm_provide_guard_bits_q15 (q15_t *input_buf, uint32_t blockSize , uint32_t guard_bits)
//---给q15_t类型数据提供保护位,防止溢出
//void arm_provide_guard_bits_q31(q31_t *input_buf, uint32_t blockSize, uint32_t guard_bits);
//---浮点数转换为Q14格式
//void arm_float_to_q14(float *pIn, q15_t *pOut, uint32_t numSamples);
//---浮点数转换为Q29格式
//void arm_float_to_q29(float *pIn, q31_t *pOut, uint32_t numSamples);
//---浮点数转换为Q28格式
//void arm_float_to_q28(float *pIn, q31_t *pOut, uint32_t numSamples);
//---浮点数转换为Q30格式
//void arm_float_to_q30(float *pIn, q31_t *pOut, uint32_t numSamples);
//---浮点数据转为-1~+1范围
//void arm_clip_f32(float *pIn, uint32_t numSamples);
//---根据加数的个数来计算最终结果需要的保护位格式
//uint32_t arm_calc_guard_bits(uint32_t num_adds);
//---未知
//void arm_apply_guard_bits (float32_t * pIn, uint32_t numSamples, uint32_t guard_bits);
//---对比matlab与arm的实际输出,并返回最大差值
//uint32_t arm_compare_fixed_q15(q15_t *pIn, q15_t * pOut, uint32_t numSamples);
//uint32_t arm_compare_fixed_q31(q31_t *pIn, q31_t *pOut, uint32_t numSamples);
//---求解2的n次方
//uint32_t arm_calc_2pow(uint32_t guard_bits);
//------------------------------------------------------------
//----------
#endif
