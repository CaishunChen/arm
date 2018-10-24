//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 4.14                    //
//                    编 译 器: RealView MDK-ARM Version 4.14                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: Memory.h                                         //
//                    作    者: 徐松亮                                           //
//                    时    间: 2014-01-17    版本:  0.1                         //
//-------------------------------------------------------------------------------//
// 文件描述：读写参数.此文件为Flash.c和EEPROM.c的上层文件,应与硬件无关类型
// 注意事项：PIC为16位单片机,注意在结构体中定义要以字为单位连续定义,否者空间浪费且整个结构体长度增加
// 当前状态: 只是搭建了程序框架,还没有填充具体内容
//-------------------------------------------------------------------------------
#ifndef __Memory_H
#define __Memory_H
//-------------------------------------------------------------------------------宏定义
#define MEMORY_PRODUCT_HEAD         "HH_FRAME"
#define MEMORY_PRODUCTION_DATE      0x0D0C

#define MEMORY_HARDWARE_VER         1
#define MEMORY_SOFTWARE_VER         59
#define MEMORY_MAIN_VER             1
#define MEMORY_SUB_VER              14
#define MEMORY_PRODUCT_SERIAL_NUMBER 12345
#define MEMORY_PRODUCT_CODE         12345678
#define MEMORY_PRODUCT_TYPE         2
//-------------------------------------------------------------------------------EEPROM地址空间分配
#define MEMORY_EEPROM_BEGIN_ADDR    256L
//EEPROM头
#define MEMORY_EEPROM_HEAD_SIZE     32
#define MEMORY_EEPROM_HEAD_ADDR     MEMORY_EEPROM_BEGIN_ADDR
#define MEMORY_EEPROM_HEAD_END      (MEMORY_EEPROM_HEAD_ADDR+MEMORY_EEPROM_HEAD_SIZE-1)
//版本
#define MEMORY_VER_SIZE             32
#define MEMORY_VER_LEN              20
#define MEMORY_VER_ADDR             (MEMORY_EEPROM_HEAD_END+1)
#define MEMORY_VER_ADDR_END         (MEMORY_VER_ADDR+MEMORY_VER_SIZE-1)
//校验
#define MEMORY_CHECK_SIZE           512
#define MEMORY_CHECK_LEN            (4+32*8)
#define MEMORY_CHECK_ADDR           (MEMORY_VER_ADDR_END+1)
#define MEMORY_CHECK_ADDR_END       (MEMORY_CHECK_ADDR+MEMORY_CHECK_SIZE-1)
//EEPROM
#define MEMORY_KEEPPARA_SIZE        32
#define MEMORY_KEEPPARA_LEN         22
#define MEMORY_KEEPPARA_ADDR        (MEMORY_CHECK_ADDR_END+1)
#define MEMORY_KEEPPARA_ADDR_END    (MEMORY_KEEPPARA_ADDR+MEMORY_KEEPPARA_SIZE-1)
//控制
#define MEMORY_CONTROL_SIZE         32
#define MEMORY_CONTROL_LEN          6
#define MEMORY_CONTROL_ADDR         (MEMORY_KEEPPARA_ADDR_END+1)
#define MEMORY_CONTROL_ADDR_END     (MEMORY_CONTROL_ADDR+MEMORY_CONTROL_SIZE-1)
//门
#define MEMORY_DOOR_MAX_NUM         4
#define MEMORY_DOOR_SIZE            16
#define MEMORY_DOOR_LEN             9
#define MEMORY_DOOR_ADDR            (MEMORY_CONTROL_ADDR_END+1)
#define MEMORY_DOOR_ADDR_END        (MEMORY_DOOR_ADDR+(MEMORY_DOOR_MAX_NUM*MEMORY_DOOR_SIZE)-1)
//门组位图
#define MEMORY_DOORGBITMAP_MAX_NUM  4
#define MEMORY_DOORGBITMAP_SIZE     256
#define MEMORY_DOORGBITMAP_LEN      129
#define MEMORY_DOORGBITMAP_ADDR     (MEMORY_DOOR_ADDR_END+1)
#define MEMORY_DOORGBITMAP_ADDR_END (MEMORY_DOORGBITMAP_ADDR+(MEMORY_DOORGBITMAP_MAX_NUM*MEMORY_DOORGBITMAP_SIZE)-1)
//卡组位图
#define MEMORY_CARDGBITMAP_MAX_NUM  4
#define MEMORY_CARDGBITMAP_SIZE     256
#define MEMORY_CARDGBITMAP_LEN      129
#define MEMORY_CARDGBITMAP_ADDR     (MEMORY_DOORGBITMAP_ADDR_END+1)
#define MEMORY_CARDGBITMAP_ADDR_END (MEMORY_CARDGBITMAP_ADDR+(MEMORY_CARDGBITMAP_MAX_NUM*MEMORY_CARDGBITMAP_SIZE)-1)
//读卡器
#define MEMORY_READER_MAX_NUM       8
#define MEMORY_READER_SIZE          32
#define MEMORY_READER_LEN           23
#define MEMORY_READER_ADDR          (MEMORY_CARDGBITMAP_ADDR_END+1)
#define MEMORY_READER_ADDR_END      (MEMORY_READER_ADDR+(MEMORY_READER_MAX_NUM*MEMORY_READER_SIZE)-1)
//继电器
#define MEMORY_RELAY_MAX_NUM        8
#define MEMORY_RELAY_SIZE           16
#define MEMORY_RELAY_LEN            6
#define MEMORY_RELAY_ADDR           (MEMORY_READER_ADDR_END+1)
#define MEMORY_RELAY_ADDR_END       (MEMORY_RELAY_ADDR+(MEMORY_RELAY_MAX_NUM*MEMORY_RELAY_SIZE)-1)
//输入
#define MEMORY_INPUT_MAX_NUM        8
#define MEMORY_INPUT_SIZE           16
#define MEMORY_INPUT_LEN            8
#define MEMORY_INPUT_ADDR           (MEMORY_RELAY_ADDR_END+1)
#define MEMORY_INPUT_ADDR_END       (MEMORY_INPUT_ADDR+(MEMORY_INPUT_MAX_NUM*MEMORY_INPUT_SIZE)-1)
//报警
#define MEMORY_ALARM_SIZE           16
#define MEMORY_ALARM_LEN            13
#define MEMORY_ALARM_ADDR           (MEMORY_INPUT_ADDR_END+1)
#define MEMORY_ALARM_ADDR_END       (MEMORY_ALARM_ADDR+MEMORY_ALARM_SIZE-1)
//掉电恢复区
typedef struct S_MEMORY_EEPROM_FLASH
{
    uint8_t Head[8];
    uint32_t FalshAppAddr;  //应用地址
    uint8_t SaveBlockNum;   //保存块编号(0起始,用于平衡擦除次数)
    //----------
    uint8_t Reserve[1];     //保留
    uint16_t Sum;
} S_MEMORY_EEPROM_FLASH;
#define MEMORY_EEPROM_FLASH_HEAD_STR         "FLASH"
#define MEMORY_EEPROM_FLASH_ADDR             (MEMORY_ALARM_ADDR_END+1)
#define MEMORY_EEPROM_FLASH_ADDR_END         (MEMORY_EEPROM_FLASH_ADDR+16-1)
//逻辑模块-->输入输出与
#define MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM      16
#define MEMORY_LOGICBLOCK_INOUTAND_SIZE         32
#define MEMORY_LOGICBLOCK_INOUTAND_LEN          23
#define MEMORY_LOGICBLOCK_INOUTAND_ADDR         (MEMORY_EEPROM_FLASH_ADDR_END+1)
#define MEMORY_LOGICBLOCK_INOUTAND_ADDR_END     (MEMORY_LOGICBLOCK_INOUTAND_ADDR+(MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM*MEMORY_LOGICBLOCK_INOUTAND_SIZE)-1)
/*
//逻辑模块-->输入模块配置
#define MEMORY_LOGICBLOCK_INPUT_MAX_NUM      64
#define MEMORY_LOGICBLOCK_INPUT_SIZE         8
#define MEMORY_LOGICBLOCK_INPUT_LEN          5
#define MEMORY_LOGICBLOCK_INPUT_ADDR         (MEMORY_EEPROM_FLASH_ADDR_END+1)
#define MEMORY_LOGICBLOCK_INPUT_ADDR_END     (MEMORY_LOGICBLOCK_INPUT_ADDR+(MEMORY_LOGICBLOCK_INPUT_MAX_NUM*MEMORY_LOGICBLOCK_INPUT_SIZE)-1)
//逻辑模块-->逻辑模块配置
#define MEMORY_LOGICBLOCK_LOGIC_MAX_NUM      128
#define MEMORY_LOGICBLOCK_LOGIC_SIZE         16
#define MEMORY_LOGICBLOCK_LOGIC_LEN          7
#define MEMORY_LOGICBLOCK_LOGIC_ADDR         (MEMORY_LOGICBLOCK_INPUT_ADDR_END+1)
#define MEMORY_LOGICBLOCK_LOGIC_ADDR_END     (MEMORY_LOGICBLOCK_LOGIC_ADDR+(MEMORY_LOGICBLOCK_LOGIC_MAX_NUM*MEMORY_LOGICBLOCK_LOGIC_SIZE)-1)
//逻辑模块-->输出模块配置
#define MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM      64
#define MEMORY_LOGICBLOCK_OUTPUT_SIZE         16
#define MEMORY_LOGICBLOCK_OUTPUT_LEN          9
#define MEMORY_LOGICBLOCK_OUTPUT_ADDR         (MEMORY_LOGICBLOCK_LOGIC_ADDR_END+1)
#define MEMORY_LOGICBLOCK_OUTPUT_ADDR_END     (MEMORY_LOGICBLOCK_OUTPUT_ADDR+(MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM*MEMORY_LOGICBLOCK_OUTPUT_SIZE)-1)
*/
//反传区
#define MEMORY_REVERSE_MAX_NUM               (256L)
#define MEMORY_REVERSE_SIZE                  (4L)
#define MEMORY_REVERSEG_NUM                  (4L)
#define MEMORY_REVERSE_ADDR                  (MEMORY_LOGICBLOCK_INOUTAND_ADDR_END+1)
#define MEMORY_REVERSE_ADDR_END              (MEMORY_REVERSE_ADDR+(MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE*MEMORY_REVERSEG_NUM)-1)
//结束地址
#define MEMORY_EERPOM_END_ADDR               (MEMORY_REVERSE_ADDR_END+1)
//最后2K作为永不改变的数据空间
#define MEMORY_EERPOM_NO_CHANGE_BEGIN_ADDR   (MEMORY_EERPOM_END_ADDR/256+1)*256
//NET
#define MEMORY_NET_SIZE                      64
#define MEMORY_NET_LEN                       28
#define MEMORY_NET_ADDR                      MEMORY_EERPOM_NO_CHANGE_BEGIN_ADDR
#define MEMORY_NET_ADDR_END                  (MEMORY_NET_ADDR+MEMORY_NET_SIZE-1)
//芯片最大地址
#define MEMORY_EEPROM_MAX_ADDR               (32*1024L)
//-------------------------------------------------------------------------------FLASH地址空间分配
//最小擦除单位
#define MEMORY_FLASH_MIN_ERASE_SIZE          (4*1024L)
//最大写入单位
#define MEMORY_FLASH_MAX_WRITE_SIZE          (256L)
//应用层起始地址
#define MEMORY_FLASH_BEGIN_ADDR              0L
//头地址(4K)
#define MEMORY_FLASH_HEAD_ADDR               MEMORY_FLASH_BEGIN_ADDR
#define MEMORY_FLASH_HEAD_ADDR_END           (MEMORY_FLASH_HEAD_ADDR+MEMORY_FLASH_MIN_ERASE_SIZE-1)
//权限表地址(320K)(先存普通用户,后存访客)
#define MEMORY_FLASH_USER_PERMISSION_MAX_NUM (2048L)
#define MEMORY_FLASH_USER_PERMISSION_SIZE    (32L)
#define MEMORY_FLASH_USER_PERMISSION_LEN     (26)
#define MEMORY_FLASH_USER_PERMISSION_GROUP_NUM 17
#define MEMORY_FLASH_MAX_USER                ((MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
#define MEMORY_FLASH_USER_PERMISSION_ADDR    (MEMORY_FLASH_HEAD_ADDR_END+1)
#define MEMORY_FLASH_USER_PERMISSION_ADDR_END  (MEMORY_FLASH_USER_PERMISSION_ADDR+(MEMORY_FLASH_USER_PERMISSION_SIZE*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_GROUP_NUM)-1)
//刷卡记录地址(160K)
#define MEMORY_FLASH_MAX_CARD_RECORD         (10240L)
#define MEMORY_FLASH_CARD_RECORD_SIZE        (32L)
#define MAX_CARD_RECORD_STRUCT_BYTE_LEN      (19)
#define MEMORY_FLASH_CARD_RECORD_ADDR        (MEMORY_FLASH_USER_PERMISSION_ADDR_END+1)
#define MEMORY_FLASH_CARD_RECORD_ADDR_END    (MEMORY_FLASH_CARD_RECORD_ADDR+(MEMORY_FLASH_MAX_CARD_RECORD*MEMORY_FLASH_CARD_RECORD_SIZE)-1)
//报警记录地址(160K)
#define MEMORY_FLASH_MAX_ALARM_RECORD        (10240L)
#define MEMORY_FLASH_ALARM_RECORD_SIZE       (32L)
#define MAX_ALARM_RECORD_STRUCT_BYTE_LEN     (19)
#define MEMORY_FLASH_ALARM_RECORD_ADDR       (MEMORY_FLASH_CARD_RECORD_ADDR_END+1)
#define MEMORY_FLASH_ALARM_RECORD_ADDR_END   (MEMORY_FLASH_ALARM_RECORD_ADDR+(MEMORY_FLASH_MAX_ALARM_RECORD*MEMORY_FLASH_ALARM_RECORD_SIZE)-1)
//时间域组(16K)
#define MEMORY_FLASH_MAX_TIMEGROUP           (256L)
#define MEMORY_FLASH_TIMEGROUP_SIZE          (128L)
#define MEMORY_FLASH_TIMEGROUP_LEN           (96)
#define MEMORY_FLASH_TIMEGROUP_NUMBER_LEN    (2) /*只用于删除指令的长度*/
#define MEMORY_FLASH_TIMEGROUP_ADDR          (MEMORY_FLASH_ALARM_RECORD_ADDR_END+1)
#define MEMORY_FLASH_TIMEGROUP_ADDR_END      (MEMORY_FLASH_TIMEGROUP_ADDR+MEMORY_FLASH_TIMEGROUP_SIZE*MEMORY_FLASH_MAX_TIMEGROUP-1)
//节假日组(16K)
#define MEMORY_FLASH_MAX_HOLIDAYGROUP        (256L)
#define MEMORY_FLASH_HOLIDAYGROUP_SIZE       (64L)
#define MEMORY_FLASH_HOLIDAYGROUP_LEN        (49)
#define MEMORY_FLASH_HOLIDAYGROUP_NUMBER_LEN (2) /*只用于删除指令的长度*/
#define MEMORY_FLASH_HOLIDAYGROUP_ADDR       (MEMORY_FLASH_TIMEGROUP_ADDR_END+1)
#define MEMORY_FLASH_HOLIDAYGROUP_ADDR_END   (MEMORY_FLASH_HOLIDAYGROUP_ADDR+MEMORY_FLASH_HOLIDAYGROUP_SIZE*MEMORY_FLASH_MAX_HOLIDAYGROUP-1)
//门组
#define MEMORY_DOORGROUP_MAX_NUM             (1024L)
#define MEMORY_DOORGROUP_SIZE                (8L)
#define MEMORY_DOORGROUP_LEN                 (3)
#define MEMORY_DOORGROUP_NUMBER_LEN          (2) /*只用于删除指令的长度*/
#define MEMORY_DOORGROUP_ADDR                (MEMORY_FLASH_HOLIDAYGROUP_ADDR_END+1)
#define MEMORY_DOORGROUP_ADDR_END            (MEMORY_DOORGROUP_ADDR+MEMORY_DOORGROUP_MAX_NUM*MEMORY_DOORGROUP_SIZE-1)
//卡组权限
#define MEMORY_CARDGROUPLIMIT_MAX_NUM        (1024L)
#define MEMORY_CARDGROUPLIMIT_SIZE           (16L)
#define MEMORY_CARDGROUPLIMIT_LEN            (10)
#define MEMORY_CARDGROUPLIMIT_NUMBER_LEN     (2) /*只用于删除指令的长度*/
#define MEMORY_CARDGROUPLIMIT_ADDR           (MEMORY_DOORGROUP_ADDR_END+1)
#define MEMORY_CARDGROUPLIMIT_ADDR_END       (MEMORY_CARDGROUPLIMIT_ADDR+(MEMORY_CARDGROUPLIMIT_MAX_NUM*MEMORY_CARDGROUPLIMIT_SIZE)-1)
//区域
#define MEMORY_AREA_MAX_NUM                  (1024L)
#define MEMORY_AREA_SIZE                     (16L)
#define MEMORY_AREA_LEN                      (12)
#define MEMORY_AREA_NUMBER_LEN               (2) /*只用于删除指令的长度*/
#define MEMORY_AREA_ADDR                     (MEMORY_CARDGROUPLIMIT_ADDR_END+1)
#define MEMORY_AREA_ADDR_END                 (MEMORY_AREA_ADDR+(MEMORY_AREA_MAX_NUM*MEMORY_AREA_SIZE)-1)
//反传区
/*
#define MEMORY_REVERSE_MAX_NUM               (1024L)
#define MEMORY_REVERSE_SIZE                  (4L)
#define MEMORY_REVERSEG_NUM                  (4L)
#define MEMORY_REVERSE_ADDR                  (MEMORY_AREA_ADDR_END+1)
#define MEMORY_REVERSE_ADDR_END              (MEMORY_REVERSE_ADDR+(MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE*MEMORY_REVERSEG_NUM)-1)
*/
//修改备用地址(400K)
#define MEMORY_FLASH_MAX_MODIFICATION        100
#define MEMORY_FLASH_MODIFICATION_SIZE       MEMORY_FLASH_MIN_ERASE_SIZE
#define MEMORY_FLASH_MODIFICATION_ADDR       (MEMORY_AREA_ADDR_END+1)
#define MEMORY_FLASH_MODIFICATION_ADDR_END   (MEMORY_FLASH_MODIFICATION_ADDR+MEMORY_FLASH_MODIFICATION_SIZE*MEMORY_FLASH_MAX_MODIFICATION-1)
//结束地址
#define MEMORY_FLASH_END_ADDR                (MEMORY_FLASH_MODIFICATION_ADDR_END+1)
//IAP地址
#define MEMORY_FLASH_IAP_ADDR_STATE          (6*1024*1024L)
#define MEMORY_FLASH_IAP_ADDR_BEGIN          (6*1024*1024L+4*1024L)
#define MEMORY_FLASH_IAP_ADDR_END            (7*1024*1024L-1)
//芯片最大
#define MEMORY_FLASH_MAX_ADDR                (8*1024*1024L)
//-------------------------------------------------------------------------------命令枚举定义
enum MEMORY_APP_CMD
{
    //正常读写操作
    MEMORY_APP_CMD_VER_R=0,            //读版本
    MEMORY_APP_CMD_VER_W,              //写版本(仅内部使用)
    MEMORY_APP_CMD_CHECK_R,            //读检测
    MEMORY_APP_CMD_CHECK_W,            //写检查
    MEMORY_APP_CMD_KEEPPARA_R,         //读要保存的参数(仅内部使用)
    MEMORY_APP_CMD_KEEPPARA_W,         //写要保存的参数(仅内部使用)
    MEMORY_APP_CMD_NET_R,              //读网络参数
    MEMORY_APP_CMD_NET_W,              //写网络参数
    MEMORY_APP_CMD_CONTROL_R,          //读控制信息
    MEMORY_APP_CMD_CONTROL_W,          //写控制信息
    MEMORY_APP_CMD_DOOR_R,             //读门
    MEMORY_APP_CMD_DOOR_W,             //写门
    //MEMORY_APP_CMD_DOORGBITMAP_R,      //读门组位图
    //MEMORY_APP_CMD_DOORGBITMAP_W,      //写门组位图
    MEMORY_APP_CMD_CARDGBITMAP_R,      //读卡组位图
    MEMORY_APP_CMD_CARDGBITMAP_W,      //写卡组位图
    MEMORY_APP_CMD_READER_R,           //读读卡器
    MEMORY_APP_CMD_READER_W,           //写读卡器
    MEMORY_APP_CMD_RELAY_R,            //读继电器
    MEMORY_APP_CMD_RELAY_W,            //写继电器
    MEMORY_APP_CMD_INPUT_R,            //读输入
    MEMORY_APP_CMD_INPUT_W,            //写输入
    MEMORY_APP_CMD_ALARM_R,            //读警报
    MEMORY_APP_CMD_ALARM_W,            //写警报
    MEMORY_APP_CMD_DOORGROUP_R,        //读门组
    MEMORY_APP_CMD_DOORGROUP_W,        //写门组
    MEMORY_APP_CMD_CARDGROUP_LIMIT_R,  //读卡组权限
    MEMORY_APP_CMD_CARDGROUP_LIMIT_W,  //写卡组权限
    MEMORY_APP_CMD_AREA_R,             //读区域
    MEMORY_APP_CMD_AREA_W,             //写区域
    MEMORY_APP_CMD_HOLIDAYGROUP_R,     //读节假日组
    MEMORY_APP_CMD_HOLIDAYGROUP_W,     //写节假日组
    MEMORY_APP_CMD_TIMEGROUP_R,        //读时间域组
    MEMORY_APP_CMD_TIMEGROUP_W,        //写时间域组
    MEMORY_APP_CMD_USER_PERMISSION_R,  //读用户
    MEMORY_APP_CMD_NEW_USER_W,         //新建普通用户
    MEMORY_APP_CMD_MODY_USER_W,        //修改普通用户
    MEMORY_APP_CMD_NEW_VISITOR_W,      //新建访客用户
    MEMORY_APP_CMD_MODY_VISITOR_W,     //修改访客用户
    MEMORY_APP_CMD_CARD_RECORD_R,      //读事件记录
    MEMORY_APP_CMD_CARD_RECORD_W,      //写事件记录
    MEMORY_APP_CMD_ALARM_RECORD_R,     //读报警记录
    MEMORY_APP_CMD_ALARM_RECORD_W,     //写报警记录
    //
    MEMORY_APP_CMD_REVERSE_R,          //读反传
    MEMORY_APP_CMD_REVERSE_W,          //写反传
    MEMORY_APP_CMD_REVERSE_C,          //清反传
    //
    MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_R, //读输入输出与
    MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_W, //写输入输出与
    /*
    MEMORY_APP_CMD_LOGICBLOCK_INPUT_R, //读逻辑模块之输入模块
    MEMORY_APP_CMD_LOGICBLOCK_INPUT_W, //写逻辑模块之输入模块
    MEMORY_APP_CMD_LOGICBLOCK_LOGIC_R, //读逻辑模块之逻辑模块
    MEMORY_APP_CMD_LOGICBLOCK_LOGIC_W, //写逻辑模块之逻辑模块
    MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_R,//读逻辑模块之输出模块
    MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_W,//写逻辑模块之输出模块
    */
    //特殊指令
    MEMORY_APP_CMD_FORMAT,             //格式化EEPROM和FLASH,并写入基本头信息
    MEMORY_APP_CMD_INIT,               //初始化IIC和SPI及相关DMA
    //EEPROM初始化测试
    MEMORY_APP_CMD_EEPROM_INIT_TEST,   //铁电初始化测试
    //FLASH初始化测试
    MEMORY_APP_CMD_FLASH_INIT_TEST,    //FLASH初始化测试
    //FLASH擦除
    MEMORY_APP_CMD_FLASH_EARSE,
    //FLASH写入
    MEMORY_APP_CMD_FLASH_WRITE,
    //FLASH读取
    MEMORY_APP_CMD_FLASH_READ,
};
enum MEMORY_APP_ERR
{
    MEMORY_APP_ERR_NO=OK,               //无错误
    MEMORY_APP_ERR_OPERATION=ERR,       //读写函数错误
    MEMORY_APP_ERR_SUM,                 //读写正确,但是结构体校验错误
    MEMORY_APP_ERR_PARA,                //参数错误
    MEMORY_APP_ERR_VOID_RETURN,         //无有效返回
    MEMORY_APP_ERR_FIRST,               //执行了首次初始化
};
//---------------------------------------------------
//enum MEMORY_TEST_CMD
//{
#define Memory_TEST_READ_BASEINFO       0x00000001  //只读-门控器基本信息
#define Memory_TEST_READ_CHECK          0x00000002  //只读-校验信息
#define Memory_TEST_READ_DOORG          0x00000004  //只读-校验信息
#define Memory_TEST_READ_CARDG          0x00000008  //只读-校验信息
#define Memory_TEST_READ_AREA           0x00000010  //只读-区域信息
#define Memory_TEST_READ_HOLIDAYGROUP   0x00000020  //只读-节假日组信息
#define Memory_TEST_READ_TIMEGROUP      0x00000040  //只读-时间组信息
#define Memory_TEST_READ_USER           0x00000080  //只读-卡表信息
#define Memory_TEST_WR_TYPE             0x00000100  //写读-版本读写测试
#define Memory_TEST_WR_TIMEGROUP        0x00000200  //写读-时间组读写测试
#define Memory_TEST_WR_HOLIDAYGROUP     0x00000400  //写读-节假日组读写测试
#define Memory_TEST_WR_CARDRECORD       0x00000800  //写读-刷卡记录信息极限读写测试
#define Memory_TEST_WR_ALARMRECORD      0x00001000  //写读-报警记录信息极限读写测试
#define Memory_TEST_WR_USER_NEW         0x00002000  //写读-用户信息极限读写测试(耗时操作---测试OK)
#define Memory_TEST_WR_VISITOR_NEW      0x00004000  //写读-访客极限读写测试(耗时操作)
#define Memory_TEST_READ_INOUTAND       0x00008000  //只读-输入输出与
#define Memory_TEST_WR_REVERSE          0x00100000  //写读-反传测试(程序保留,但是功能已作废)
#define Memory_TEST_READ_EVENT          0x00200000  //只读-事件记录
#define Memory_TEST_READ_ALARM          0x00400000  //只读-报警记录
#define Memory_TEST_READ_DOOR           0x00800000  //只读-取门(基本信息+门组位图+卡组位图+读卡器+继电器+输入)
#define Memory_TEST_WR_EEPROM           0x10000000  //写读-EEPROM整个芯片测试
#define Memory_TEST_WR_FLASH            0x20000000  //写读-FLASH整个芯片测试(耗时操作)
#define Memory_TEST_PRE_CONF            0x40000000  //预设数据
#define Memory_TEST_FLASH_FORMAT        0x80000000  //写  -FLASH分区格式化测试
//};
//---------------------------------------------------
//enum MEMORY_FORMAT_CODE
//{
#define MEMORY_FORMAT_ALARM_RECORD     0x00000001
#define MEMORY_FORMAT_CARD_RECORD      0x00000002
#define MEMORY_FORMAT_VISITOR          0x00000004
#define MEMORY_FORMAT_USER1            0x00000008
#define MEMORY_FORMAT_USER2            0x00000010
#define MEMORY_FORMAT_USER3            0x00000020
#define MEMORY_FORMAT_USER4            0x00000040
#define MEMORY_FORMAT_USER5            0x00000080
#define MEMORY_FORMAT_USER6            0x00000100
#define MEMORY_FORMAT_USER7            0x00000200
#define MEMORY_FORMAT_USER8            0x00000400
#define MEMORY_FORMAT_USER9            0x00000800
#define MEMORY_FORMAT_USER10           0x00001000
#define MEMORY_FORMAT_USER11           0x00002000
#define MEMORY_FORMAT_USER12           0x00004000
#define MEMORY_FORMAT_USER13           0x00008000
#define MEMORY_FORMAT_USER14           0x00010000
#define MEMORY_FORMAT_USER15           0x00020000
#define MEMORY_FORMAT_USER16           0x00040000
#define MEMORY_FORMAT_TIMEG            0x00080000
#define MEMORY_FORMAT_HOLIDAYG         0x00100000
#define MEMORY_FORMAT_DOOR             0x00200000
#define MEMORY_FORMAT_DOORG            0x00400000
#define MEMORY_FORMAT_CARDG            0x00800000
#define MEMORY_FORMAT_AREA             0x01000000
//#define MEMORY_FORMAT_DOORGB           0x02000000
#define MEMORY_FORMAT_INOUTAND         0x02000000
#define MEMORY_FORMAT_CARDGB           0x04000000
#define MEMORY_FORMAT_READER           0x08000000
#define MEMORY_FORMAT_RELAY            0x10000000
#define MEMORY_FORMAT_INPUT            0x20000000
//#define MEMORY_FORMAT_INOUTAND         0x40000000
#define MEMORY_FORMAT_ALL              0xFFFFFFFF
//};
//---------------------------------------------------
//-------------------------------------------------------------------------------数据类型定义
//版本
typedef struct S_MEMORY_VER
{
    uint16_t ProductionDate;       //出厂年月
    uint16_t SerialNumber;         //序号
    uint32_t ProductCode;          //产品代码(名称)
    uint16_t HardwareVer;          //硬件版本号
    uint16_t SoftwareVer;          //软件版本号
    uint16_t MemoryMainVer;        //存储结构主版本号
    uint16_t MemorySubVer;         //存储结构子版本号
    uint8_t Type;                  //门控器类型(1,2,4)
    uint8_t addr;                  //地址(1-8)
    uint16_t BaudRate;             //
    //----------
    uint8_t Reserve[MEMORY_VER_SIZE-MEMORY_VER_LEN-2]; //保留
    uint16_t Sum;                  //校验和
} S_MEMORY_VER;
//校验结构体(只用于校验)
typedef struct S_CHECK
{
    uint32_t SerialNumber;
    uint16_t Number;
    uint16_t CheckSum;
} S_CHECK;
//校验
enum MEMORY_APP_CHECK_ENUM
{
    MEMORY_APP_CHECK_ALL=0,
    MEMORY_APP_CHECK_CARD_RECORD,
    MEMORY_APP_CHECK_ALARM_RECORD,
    MEMORY_APP_CHECK_DOOR,
    MEMORY_APP_CHECK_DOORGROUP,
    MEMORY_APP_CHECK_CARDGROUP,
    MEMORY_APP_CHECK_ALARM,
    MEMORY_APP_CHECK_AREA,
    MEMORY_APP_CHECK_HOLIDAYGROUP,
    MEMORY_APP_CHECK_TIMEGROUP,
    MEMORY_APP_CHECK_CARD_VISITOR,
    MEMORY_APP_CHECK_CARD_USER1,
    MEMORY_APP_CHECK_CARD_USER2,
    MEMORY_APP_CHECK_CARD_USER3,
    MEMORY_APP_CHECK_CARD_USER4,
    MEMORY_APP_CHECK_CARD_USER5,
    MEMORY_APP_CHECK_CARD_USER6,
    MEMORY_APP_CHECK_CARD_USER7,
    MEMORY_APP_CHECK_CARD_USER8,
    MEMORY_APP_CHECK_CARD_USER9,
    MEMORY_APP_CHECK_CARD_USER10,
    MEMORY_APP_CHECK_CARD_USER11,
    MEMORY_APP_CHECK_CARD_USER12,
    MEMORY_APP_CHECK_CARD_USER13,
    MEMORY_APP_CHECK_CARD_USER14,
    MEMORY_APP_CHECK_CARD_USER15,
    MEMORY_APP_CHECK_CARD_USER16,
    MEMORY_APP_CHECK_READER,
    MEMORY_APP_CHECK_RELAY,
    MEMORY_APP_CHECK_INPUT,
    //MEMORY_APP_CHECK_DOORG_BITMAP,
    MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,
    MEMORY_APP_CHECK_CARDG_BITMAP,
    //
    /*
    MEMORY_APP_CHECK_LOGICBLOCK_INPUT,
    MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,
    MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,
    */
};
typedef struct S_MEMORY_CHECK
{
    uint16_t CardRecordNumber;           //刷卡记录数量
    uint16_t AlarmRecordNumber;          //报警记录数量
    S_CHECK Door;
    S_CHECK DoorGroup;
    S_CHECK CardGroup;
    S_CHECK Alarm;
    S_CHECK Area;
    S_CHECK HolidayGroup;
    S_CHECK TimeGroup;
    S_CHECK Card_Visitor;              //卡0(临时)
    S_CHECK Card_User[16];             //卡1-卡16
    S_CHECK Reader;
    S_CHECK Relay;
    S_CHECK Input;
    S_CHECK DoorGBitmap;
    S_CHECK CardGBitmap;
    S_CHECK LogicBlockInput;
    S_CHECK LogicBlockLogic;
    S_CHECK LogicBlockOutput;
    //----------
    uint8_t Reserve[MEMORY_CHECK_SIZE-MEMORY_CHECK_LEN-2];
    uint16_t Sum;
} S_MEMORY_CHECK;
//铁电信息
typedef struct S_MEMORY_KEEPPARA
{
    uint16_t TransferAlarmTxNum;         //发送计数
    uint16_t TransferAlarmTxLastNum;     //上一次发送计数
    uint16_t TransferEventTxNum;         //发送计数
    uint16_t TransferEventTxLastNum;     //上一次发送计数
    uint8_t MonitorSign[4];
    uint32_t DoorFirstTime[2];           //首卡刷卡Unix时间(0xFFFFFFFF无效)
    uint8_t ReaderLockBitmap;            //读卡器锁定位图(0-未锁定,1-锁定)
    uint8_t PowerDownCount;
    //----------
    uint8_t Reserve[MEMORY_KEEPPARA_SIZE-MEMORY_KEEPPARA_LEN-2];
    uint16_t Sum;
} S_MEMORY_KEEPPARA;
//网络
typedef struct S_MEMORY_NET
{
    uint8_t Gateway_IP[4];//网关IP地址
    uint8_t Sub_Mask[4];  //子网掩码
    uint8_t IP_Local[4];  //本机IP地址
    uint8_t Phy_Addr[6];  //物理地址(MAC)
    uint8_t DNS_IP[4];    //DNS服务器
    uint8_t IP_Dest1[4];  //目标IP地址
    uint16_t PORT_Dest1;  //目标PORT
    //----------
    uint8_t Reserve[MEMORY_NET_SIZE-MEMORY_NET_LEN-2];
    uint16_t Sum;
} S_MEMORY_NET;
//控制信息
typedef struct S_MEMORY_CONTROL
{
    uint8_t DeviceEnable;                 //设备使能
    uint8_t ForceCountMode;               //胁迫模式
    uint8_t ReaderEnableBitmap;           //韦根信号使能位图
    uint8_t ReaderPasswordEnableBitmap;   //读卡器超级密码使能位图
    uint8_t ReaderTimeGroupEnableBitmap;  //读卡器准进时段使能位图
    uint8_t ReaderManyCardOpenDoorEnableBitmap; //读卡器多卡开门使能位图
    //----------
    uint8_t Reserve[MEMORY_CONTROL_SIZE-MEMORY_CONTROL_LEN-2];
    uint16_t Sum;
} S_MEMORY_CONTROL;
//门
enum E_MEMORY_DOOR_FUNCTION
{
    E_MEMORY_DOOR_FUNCTION_DIR=0,       //方向(0单向,1双向)
    E_MEMORY_DOOR_FUNCTION_ENABLE,      //门使能
    E_MEMORY_DOOR_FUNCTION_SELFLOCK,    //自锁使能
    E_MEMORY_DOOR_FUNCTION_LONG_OPENDOOR_ALARM, //长时间不关门报警使能
    E_MEMORY_DOOR_FUNCTION_NO_CONTINUOUS,       //连续操作读卡器使能
    E_MEMORY_DOOR_FUNCTION_MA,          //门磁使能
    E_MEMORY_DOOR_FUNCTION_OPENDOOR_BUTTON,      //开门按钮使能
};
typedef struct S_MEMORY_DOOR
{
    //-----基本设置-----
    uint8_t Number;                   //编号
    uint8_t InDoorNumber[2];          //防反传门内区域编号
    uint8_t OutDoorNumber[2];         //防反传门外区域编号
    uint8_t FunctionSelect;           //功能选项
    uint8_t DoorGroup[2];             //门所属门组(2字节表示一个门组)
    uint8_t LongNoCloseDoorTimeS;     //长时间不关门报警时间
    //------------------
    uint8_t Reserve[MEMORY_DOOR_SIZE-MEMORY_DOOR_LEN-2];              //备用
    uint16_t Sum;                     //校验和
} S_MEMORY_DOOR;
//门组位图
/*
typedef struct S_MEMORY_DOORGBITMAP
{
    INT8U Number;                   //编号
    INT8U Bitmap[128];              //位图
    //------------------
    INT8U Reserve[MEMORY_DOORGBITMAP_SIZE-MEMORY_DOORGBITMAP_LEN-2];       //备用
    INT16U Sum;                     //校验和
} S_MEMORY_DOORGBITMAP;
*/
//卡组位图
typedef struct S_MEMORY_CARDG_BITMAP
{
    uint8_t Number;        //编号
    uint8_t Bitmap[128];   //位图
    //------------------
    uint8_t Reserve[MEMORY_CARDGBITMAP_SIZE-MEMORY_CARDGBITMAP_LEN-2];              //备用
    uint16_t Sum;                     //校验和
} S_MEMORY_CARDG_BITMAP;
//读卡器
typedef struct S_MEMORY_READER
{
    uint8_t Number;                    //编号
    uint8_t InterfaceType;             //0-韦根26;1-韦根34
    uint8_t AuthenticationType;        //0-卡,1-卡+密,2-卡/密,3-密,4-多卡,5-多卡+密
    uint8_t PasswordRetryCount;        //密码重试次数
    uint8_t PasswordMaxInputTime_s;    //密码最大输入时间(秒)
    uint8_t Password[4];               //密码
    uint8_t PasswordErrLockTime_s;     //密码错误锁定时间(秒)
    uint8_t EffectTimeGroupNumber;     //有效时间域
    uint8_t functionOption;            //功能选项
    uint8_t FirstCardEffectTime_m[2];  //首卡有效时间(分钟)
    uint8_t CardG1Num[2];              //卡组1编号
    uint8_t CardG1Sum;                 //卡组1人数
    uint8_t CardG2Num[2];              //卡组1编号
    uint8_t CardG2Sum;                 //卡组1人数
    uint8_t CardG3Num[2];              //卡组1编号
    uint8_t CardG3Sum;                 //卡组1人数
    //------------------
    uint8_t Reserve[MEMORY_READER_SIZE-MEMORY_READER_LEN-2];          //备用
    uint16_t Sum;                      //校验和
} S_MEMORY_READER;
//继电器
typedef struct S_MEMORY_RELAY
{
    uint8_t Number;                    //编号
    //-----继电器设置-----
    uint8_t Relay_Mode;               //模式:0常开/1常闭
    uint8_t Relay_OpenDoorKeepTime;   //开门保持时间
    uint8_t Relay_ExtendOpenDoorTime; //延长开门时间
    //INT8U Relay_EffectTimeGroupNumber;           //继电器有效时间域组编号
    uint8_t Relay_LinkageRelayGroupNumber_Global;  //关联继电器组_全局
    uint8_t Relay_LinkageRelayGroupNumber_Local;   //关联继电器组-本地
    //------------------
    uint8_t Reserve[MEMORY_RELAY_SIZE-MEMORY_RELAY_LEN-2];              //备用
    uint16_t Sum;                     //校验和
} S_MEMORY_RELAY;
//输入
typedef struct S_MEMORY_INPUT
{
    uint8_t Number;                    //编号
    //-----输入设置-----
    uint8_t Input_LineModeType;                    //线模块类型
    uint8_t Input_FunctionSelect;                  //b0有效状态+b1使能
    uint8_t Input_DelayTime_s;                     //输入延迟时间
    uint8_t Input_KeepTime_s;                      //输入保持时间
    uint8_t Input_DisableTimeGroupNumber;          //输入禁用时间域
    uint8_t Input_LinkageRelayGroupNumber_Global;  //关联继电器组_全局
    uint8_t Input_LinkageRelayGroupNumber_Local;   //关联继电器组-本地
    //
    uint8_t Reserve[MEMORY_INPUT_SIZE-MEMORY_INPUT_LEN-2];              //备用
    uint16_t Sum;                     //校验和
} S_MEMORY_INPUT;
//报警
typedef struct S_MEMORY_ALARM
{
    uint8_t Common_RelayNumber;                     //常规报警-继电器序号
    uint8_t Common_LinkageRelayGroupNumber_Global;  //常规报警-联动全局继电器组编号
    uint8_t Common_LinkageRelayGroupNumber_Local;   //常规报警-联动本地继电器组编号
    uint8_t Force_RelayNumber;                      //胁迫报警-继电器序号
    uint8_t Force_LinkageRelayGroupNumber_Global;   //胁迫报警-联动全局继电器组编号
    uint8_t Force_LinkageRelayGroupNumber_Local;    //胁迫报警-联动本地继电器组编号
    uint8_t Invade_RelayNumber;                     //入侵报警-继电器序号
    uint8_t Invade_LinkageRelayGroupNumber_Global;  //入侵报警-联动全局继电器组编号
    uint8_t Invade_LinkageRelayGroupNumber_Local;   //入侵报警-联动本地继电器组编号
    uint8_t Fault_RelayNumber;                      //故障报警-继电器序号
    uint8_t Fault_LinkageRelayGroupNumber_Global;   //故障报警-联动全局继电器组编号
    uint8_t Fault_LinkageRelayGroupNumber_Local;    //故障报警-联动本地继电器组编号
    uint8_t Reverse_FunctionSelect;                 //b0反传-事件报告
    //b1反传-报警
    //b2反传报警-联动本地继电器组编号
    //----------
    uint8_t Reserve[MEMORY_ALARM_SIZE-MEMORY_ALARM_LEN-2];
    uint16_t Sum;
} S_MEMORY_ALARM;
//门组
typedef struct S_MEMORY_DOOR_GROUP
{
    uint8_t Number[2];                //编号
    uint8_t FunctionSelect;        //b0:门组内互锁,b1-b7:保留
    //----------
    uint8_t Reserve[MEMORY_DOORGROUP_SIZE-MEMORY_DOORGROUP_LEN-2];
    uint16_t Sum;                  //校验和
} S_MEMORY_DOOR_GROUP;
//卡组权限
typedef struct S_MEMORY_CARDGROUP_LIMIT
{
    uint8_t Number[2];                   //编号
    uint8_t AreaGroupNumber[4];       //区域组编号
    uint8_t WayId[2];                 //线路ID
    uint8_t RedioSAB;                 //防潜回功能单选
    uint8_t TimeGroupNumber;          //时间域组编号
    //----------
    uint8_t Reserve[MEMORY_CARDGROUPLIMIT_SIZE-MEMORY_CARDGROUPLIMIT_LEN-2];          //备用
    uint16_t Sum;                     //校验和
} S_MEMORY_CARDGROUP_LIMIT;
//区域
typedef struct S_MEMORY_AREA
{
    uint8_t Number[2];                 //编号
    uint8_t AreaGroupNumber;        //区域组编号
    uint8_t TimeGroupNumber;        //时间域组ID
    uint8_t SafetyLevel;            //安全等级
    uint8_t PeopleMax[2];             //人数上限
    uint8_t PeopleMin[2];             //人数下限
    uint8_t FunctionSelect;         //功能选项:
    //b0-区域内门间互锁使能
    //b1-区域组内互锁使能
    //----------
    uint8_t InterlockArea[2];       //互锁区域
    uint8_t Reserve[MEMORY_AREA_SIZE-MEMORY_AREA_LEN-2];
    uint16_t Sum;                   //校验和
} S_MEMORY_AREA;
//节假日组
typedef struct S_MEMORY_HOLIDAYGROUP
{
    uint8_t Number;                //编号
    uint8_t DayBitMap[48];         //日期位图
    //----------
    uint8_t Reserve[MEMORY_FLASH_HOLIDAYGROUP_SIZE-MEMORY_FLASH_HOLIDAYGROUP_LEN-2];
    uint16_t Sum;
} S_MEMORY_HOLIDAYGROUP;
//时间域组
typedef struct S_MEMORY_TIMEGROUP
{
    uint8_t Number;                //编号
    uint8_t HolidayGroupNumber;    //节假日组编号
    uint8_t EffectBeginTime[5];    //时标
    uint8_t EffectEndTime[5];      //时标
    uint8_t TimeBitmap[7][12];     //周n工作时间位图(96bit,15min/bit)
    //----------
    uint8_t Reserve[MEMORY_FLASH_TIMEGROUP_SIZE-MEMORY_FLASH_TIMEGROUP_LEN-2];
    uint16_t Sum;                  //校验和
} S_MEMORY_TIMEGROUP;

//用户存储
typedef struct S_MEMORY_USER_PERMISSION
{
    uint8_t CardNumber[4];        //卡号
    uint8_t CardGroupNumber[2];    //卡组编号
    uint8_t EffectBeginTime[5]; //时标
    uint8_t EffectEndTime[5];   //时标
    uint8_t Password[4];        //
    uint8_t Status;             //状态(无/0-有效/1-丢失/2-损坏/3-禁用/4-待用/5-黑名单 0xFF-已被删除)
    uint8_t Type;               //卡类型
    uint8_t Privilege[2];       //特权
    uint8_t PassLevel;          //通行等级
    uint8_t ForceLevel;         //胁迫等级
    //----------
    uint8_t Reserve[MEMORY_FLASH_USER_PERMISSION_SIZE-MEMORY_FLASH_USER_PERMISSION_LEN-2];   //保留
    uint16_t Sum;
} S_MEMORY_USER_PERMISSION;
//刷卡记录存储
typedef struct S_MEMORY_CARD_RECORD
{
    //INT8U Number[2];             //编号
    uint8_t AlarmArea[2];          //报警区域
    uint8_t ReaderOrWireOrRelayNum;//读卡器/线模块/继电器编号
    uint8_t CardNumber[4];         //卡号
    uint8_t TargetArea[2];         //目标区域
    uint8_t CurrentArea[2];        //当前区域(门控器固定赋值为0)
    uint8_t Event[2];              //事件(包含卡片状态)
    uint8_t TimeGErr;              //违反的时间域组
    uint8_t RecordTime[5];         //时间
    //----------
    uint8_t Reserve[32-MAX_CARD_RECORD_STRUCT_BYTE_LEN];
    uint16_t Sum;
} S_MEMORY_CARD_RECORD;
//报警记录存储
typedef struct S_MEMORY_ALARM_RECORD
{
    //INT8U Number[2];             //编号
    uint8_t AlarmArea[2];          //报警区域
    uint8_t ReaderOrWireOrRelayNum;//读卡器/线模块/继电器编号
    uint8_t CardNumber[4];         //卡号
    uint8_t TargetArea[2];         //目标区域
    uint8_t CurrentArea[2];        //当前区域(门控器固定赋值为0)
    uint8_t Event[2];              //事件(包含卡片状态)
    uint8_t TimeGErr;              //违反的时间域组
    uint8_t RecordTime[5];         //时间
    //----------
    uint8_t Reserve[32-MAX_ALARM_RECORD_STRUCT_BYTE_LEN];
    uint16_t Sum;
} S_MEMORY_ALARM_RECORD;
//逻辑模块-->输入输出与
typedef struct S_MEMORY_LOGICBLOCK_INOUTAND
{
    uint8_t Number;                 //编号
    uint8_t Input1Type;             //输入1的触发状态(0-始终有效,1-低电平,2-高电平,3-下降沿,4-上升沿)
    uint8_t Input1Time_100ms;       //输入1的保持/去抖时间(100mS)
    uint8_t Input2Type;             //输入2的触发状态(0-始终有效,1-低电平,2-高电平,3-下降沿,4-上升沿)
    uint8_t Input2Time_100ms;       //输入2的保持/去抖时间(100mS)
    uint8_t Input3Type;             //输入3的触发状态(0-始终有效,1-低电平,2-高电平,3-下降沿,4-上升沿)
    uint8_t Input3Time_100ms;       //输入3的保持/去抖时间(100mS)
    uint8_t Input4Type;             //输入4的触发状态(0-始终有效,1-低电平,2-高电平,3-下降沿,4-上升沿)
    uint8_t Input4Time_100ms;       //输入4的保持/去抖时间(100mS)
    uint8_t Output1OnTimeS[2];      //输出1的开启时长(秒)
    uint8_t Output1_TimeG;          //输出1的有效时间域组
    uint8_t Output2OnTimeS[2];      //输出2的开启时长(秒)
    uint8_t Output2_TimeG;          //输出2的有效时间域组
    uint8_t Output3OnTimeS[2];      //输出3的开启时长(秒)
    uint8_t Output3_TimeG;          //输出3的有效时间域组
    uint8_t Output4OnTimeS[2];      //输出4的开启时长(秒)
    uint8_t Output4_TimeG;          //输出4的有效时间域组
    uint8_t AlarmID[2];             //报警ID
    //----------
    uint8_t Reserve[MEMORY_LOGICBLOCK_INOUTAND_SIZE-MEMORY_LOGICBLOCK_INOUTAND_LEN-2];
    uint16_t Sum;
} S_MEMORY_LOGICBLOCK_INOUTAND;
//逻辑模块-->输入模块配置
/*
typedef struct S_MEMORY_LOGICBLOCK_INPUT
{
   INT8U Number[2];     //编号
   INT8U InBitmap;      //输入位图(bit7~2-保留,bit1-高电平/低电平有效,bit0-使能/禁用)
   INT8U Timer;         //输入保持nS时间后,再触发输出.
   INT8U TimeG;         //有效时间域
   //----------
   INT8U Reserve[MEMORY_LOGICBLOCK_INPUT_SIZE-MEMORY_LOGICBLOCK_INPUT_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_INPUT;
//逻辑模块-->逻辑模块配置
typedef struct S_MEMORY_LOGICBLOCK_LOGIC
{
   INT8U Number[2];     //编号
   INT8U ID_Input1[2];    //ID-输入1
   INT8U ID_Input2[2];    //ID-输入2
   INT8U InBitmap;      //输入位图(bit7~4-保留,bit3~1-与/或/非/同或/异或,bit0-使能/禁用)
   //
   INT8U Reserve[MEMORY_LOGICBLOCK_LOGIC_SIZE-MEMORY_LOGICBLOCK_LOGIC_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_LOGIC;
//逻辑模块-->输出模块配置
typedef struct S_MEMORY_LOGICBLOCK_OUTPUT
{
   INT8U Number[2];     //编号
   INT8U ID_Input[2];     //ID-输入
   INT8U RelayBitmap[2];  //继电器位图
   INT8U AlarmID[2];      //报警ID
   INT8U InBitmap;      //输入位图(bit7~1-保留,bit0-使能/禁用)
   //
   INT8U Reserve[MEMORY_LOGICBLOCK_OUTPUT_SIZE-MEMORY_LOGICBLOCK_OUTPUT_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_OUTPUT;
*/
//防反传存储
typedef struct S_MEMORY_REVERSE
{
    uint8_t CardNumber[4];    //卡号
    uint8_t State;            //0-未知,1-存在,2-不存在,3-已满
} S_MEMORY_REVERSE;
//
extern S_MEMORY_VER *p_Ver;
extern S_CHECK *p_Check;
extern S_MEMORY_CHECK *p_MemoryCheck;
extern S_MEMORY_CONTROL *p_Control;
extern S_MEMORY_ALARM *p_Alarm;
extern S_MEMORY_DOOR *p_Door;
//extern S_MEMORY_DOORGBITMAP *p_DoorGBitmap;
extern S_MEMORY_CARDG_BITMAP *p_CardGBitmap;
extern S_MEMORY_READER *p_Reader;
extern S_MEMORY_RELAY *p_Relay;
extern S_MEMORY_INPUT *p_Input;
extern S_MEMORY_DOOR_GROUP *p_DoorG;
extern S_MEMORY_CARDGROUP_LIMIT *p_CardG;
extern S_MEMORY_AREA *p_Area;
extern S_MEMORY_HOLIDAYGROUP *p_HolidayG;
extern S_MEMORY_TIMEGROUP *p_TimeG;
extern S_MEMORY_USER_PERMISSION *p_User;
extern S_MEMORY_ALARM_RECORD *p_AlarmRecord;
extern S_MEMORY_CARD_RECORD *p_CardRecord;
extern S_MEMORY_REVERSE *p_Reverse;
extern S_MEMORY_LOGICBLOCK_INOUTAND *p_LogicBlockInOutAnd;
extern uint8_t Memory_RecordNum_ms;
//-------------------------------------------------------------------------------函数声明
extern void Memory_1msPro(void);
extern uint8_t Memory_AppFunction(uint16_t Cmd,uint8_t *Buf,uint32_t Number,uint32_t SerialNumber);
extern void Memory_TEST(uint32_t cmd);
#endif
//-------------------------------------------------------------------------------

