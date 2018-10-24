/**
  ******************************************************************************
  * @file    Bsp_NrfBle.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   基于Nrf5x的BLE驱动.
  * @note    
  * @verbatim
    
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
   1,    适用芯片
         Nordic   :  Nrf51    Nrf52
   2,    移植步骤
   3,    验证方法
   4,    使用方法
   5,    其他说明
SDK官方开发板对应:
   PCA10000 -  nRF51822 USB Dongle
   PCA10001 -  part of nRF51822 Evaluation Kit
   PCA10002 -  nRF51422 USB dongle
   PCA10003 -  part of nRF51422 Evaluation Kit, BLE + ANT
   PCA10028 -  part of nRF51422 Evaluation Kit, Arduino form factor
   PCA10031 -  nRF51422 USB dongle
   PCA10036 -  part of nRF52832 Preview Development Kit
   PCA10040 -  part of nRF52832 Development Kit
   PCA10056 -  part of nRF52840 Preview Development Kit
   PCA20006 -  nRF51822 Beacon board
   D52DK1   -  Dynastream D52 Starter Kit
   WT51822  -  Wavetek shield
   Custom board   -    definition in custom_board.h
通用概念    :
   GAP      :  通用访问协议
      广播  :  广播/扫描/启动
   GAAT     :  通用属性协议
Mac Addr:(输入与显示相反,比如输入0x0123456789ab,则显示0xab8967452301),以下按输入说明
   格式     ：
      LAP   :  3B 24位地址低端部分     由厂家内部分配
      UAP   :  1B 8位地址高端部分      由蓝牙权威部门分配
      NAP   :  2B 16位无意义地址部分   由蓝牙权威部门分配   (***0xXXXX|0x00C0***)
   修改     :  通过程序修改后,只是本次上电修改.
报文格式    :
   前导     :  1Byte    01010101/10101010
   接入地址 :  4Byte    有广播与数据两种类型
      广播  :  固定接入地址是0x8E89BED6
      数据  :  有一定规则的随机值(满足一定白化特性)
   报头     :  1Byte
      广播报头:
         广播报文类型      4  bit
            广播设备
               ADV_IND           通用广播指示   (可被扫描   可被连接)
               ADV_DIRECT_IND    定向连接指示   (不可被扫描 可被指定设备连接)
               ADV_NONCONN_IND   不可连接指示   (不可被扫描 不可被连接)
               ADV_SCAN_IND      可扫描指示     (可被扫描   不可被连接)
            扫描设备
               SCAN_REQ          主动扫描请求   (扫描后,请求广播设备传送更多信息)
               SCAN_RSP          主动扫描响应   (扫描后,把更多数据传送给广播设备)
            启动(准备连接)
               CONNECT_REQ       连接请求
         保留位            2  bit
         发送数据地址类型  1  bit
         接收数据地址类型  1  bit
      数据包头:
         链路层标识符      2  bit
         下一报文序号      1  bit
         序号              1  bit
         更多数据          1  bit
         保留              3  bit
   长度     :  1Byte    广播数据长度(广播地址AdvA+数据AdvData)
      广播报文:            6-37
      数据报文:            0-31
   数据     :  nByte       6-37Byte /  0-31Byte
      广播报文    :
         AdvA     :        6Byte 广播地址
         AdvData  :        nByte 广播数据
            Len   :        1Byte (Type+Value)
            Type  :        1Byte
               BLE_GAP_AD_TYPE_FLAGS                               0x01  可发现标志
               BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE   0x02  16位服务UUIDs部分列表
               BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE         0x03  16位服务UUIDs完整列表
               BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE   0x04  32位服务UUIDs部分列表
               BLE_GAP_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE         0x05  32位服务UUIDs完整列表
               BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE  0x06  128位服务UUIDs部分列表
               BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE        0x07  128位服务UUIDs完整列表
               BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME                    0x08  本地设备短名称
               BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME                 0x09  本地设备完整名称
               BLE_GAP_AD_TYPE_TX_POWER_LEVEL                      0x0A  传输功率
               BLE_GAP_AD_TYPE_CLASS_OF_DEVICE                     0x0D  设备类
               BLE_GAP_AD_TYPE_SIMPLE_PAIRING_HASH_C               0x0E  Simple Pairing Hash C
               BLE_GAP_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R         0x0F  Simple Pairing Randomizer R
               BLE_GAP_AD_TYPE_SECURITY_MANAGER_TK_VALUE           0x10  Security Manager TK Value
               BLE_GAP_AD_TYPE_SECURITY_MANAGER_OOB_FLAGS          0x11  Security Manager Out Of Band Flags
               BLE_GAP_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE     0x12  Slave Connection Interval Range
               BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_16BIT       0x14  List of 16-bit Service Solicitation UUIDs
               BLE_GAP_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT      0x15  List of 128-bit Service Solicitation UUIDs
               BLE_GAP_AD_TYPE_SERVICE_DATA                        0x16  Service Data - 16-bit UUID
               BLE_GAP_AD_TYPE_PUBLIC_TARGET_ADDRESS               0x17  公共目标地址
               BLE_GAP_AD_TYPE_RANDOM_TARGET_ADDRESS               0x18  随机目标地址
               BLE_GAP_AD_TYPE_APPEARANCE                          0x19  外观
               BLE_GAP_AD_TYPE_ADVERTISING_INTERVAL                0x1A  广播时间间隔
               BLE_GAP_AD_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS         0x1B  LE Bluetooth Device Address
               BLE_GAP_AD_TYPE_LE_ROLE                             0x1C  LE Role
               BLE_GAP_AD_TYPE_SIMPLE_PAIRING_HASH_C256            0x1D  Simple Pairing Hash C-256
               BLE_GAP_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R256      0x1E  Simple Pairing Randomizer R-256
               BLE_GAP_AD_TYPE_SERVICE_DATA_32BIT_UUID             0x20  Service Data - 32-bit UUID
               BLE_GAP_AD_TYPE_SERVICE_DATA_128BIT_UUID            0x21  Service Data - 128-bit UUID
               BLE_GAP_AD_TYPE_URI                                 0x24  URI
               BLE_GAP_AD_TYPE_3D_INFORMATION_DATA                 0x3D  3D Information Data
               BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA          0xFF  厂商自定义数据
            Value :        nByte
            ...
      数据报文
   校验     :  3Byte    CRC
Beacon格式  :
   四种信息 :
      UUID  :  128bit标识符
      Major :  自行设定(商场信息/产品型号)
      Minor :  自行设定(店铺信息/错误码)
      MeasuredPower: 模块与接收器之间相距1m时的参考接收信号强度(RSSI)
                     1m内基本成线性,1m外受反射波影响会上下波动
   Access Address:   接入地址(4Byte),广播报文固定接入地址是0x8E89BED6
   AdvA     :  6Byte
   AdvData  :  30Byte   (0~31)
      Len1                       :  1B    02
      Type1                      :  1B    01
      Value1                     :  1B    0x04/06
      len2
         AD Field Length         :  1B    0x1A(26)
      Type2                      :  1B    0xFF
      Value2
         APP_COMPANY_IDENTIFIER  :  2B    0x0059(NRF)
         APP_DEVICE_TYPE         :  1B    0x02(ProximityBeacon)
         APP_ADV_DATA_LENGTH     :  1B    0x15(21)
         APP_BEACON_UUID         :  16B
         APP_MAJOR_VALUE         :  2B
         APP_MINOR_VALUE         :  2B
         APP_MEASURED_RSSI       :  1B
   CRC      :  3Byte
XKAP自定义广播数据格式
   通用
      流水  1B
      信息  LTV
         事件
            按键(iCareB-C) 03    81    XX(按键编号)XX(按压时长(100ms单位))
            iCareB-M信息   02    82    XX编号
                                       1-反床告知
                                       2-离床告知
                                       3-床重校零
                                       4-遥控器启动报警
                                       5-遥控器停止报警
                                       6-遥控器启动报警功能
                                       7-遥控器关闭报警功能
            MI Band 2            A0    不明确
            XSL-TAG        0D    A1    VCC      XX(电压值 (mv/10-120)的值 为了用一个字节表达1200mv~3750mv范围)
                                       key      XX(k1次数)XX(按压时长(100ms单位))XX(k2次数)XX(按压时长(100ms单位))
                                       BMP180   XXXX(温度0.1)  XXXXXX(气压pa) XX(海拔m)
                                       AP3216   XXXX(lx照度)   XXXX(接近)
                                       MPU6050  XX(敲击次数)
            Beacon               A2
      CRC16 2B
小米手环2的广播数据格式
   类型一   : ADV_DISCOVER_IND
      Access Address:   接入地址(4Byte),广播报文固定接入地址是0x8E89BED6
      AdvA     :  6Byte    (设备的MAC:0xCFA7009EC502)
      AdvData  :  31Byte   (0~31)   02 01 04 1B FF 57 01 00 B0 90 8B 8B 91 B8 2F 88 AB 87 A1 77 C2 A5 45 6B 02 CF A7 00 9E C5 02
                                    02 01 04 1B FF 57 01 00 E8 1A 9F 34 DE BA 54 AD 97 5A D7 1A C3 E8 ED 5A 01 CF A7 00 9E C5 02
         Len1                       :  1B    02
         Type1                      :  1B    01
         Value1                     :  1B    0x04
         len2
            AD Field Length         :  1B    0x1B(27)
         Type2                      :  1B    0xFF
         Value2                     :  57 01
            APP_COMPANY_IDENTIFIER  :  2B    0x0157(华米)
            00 B0 90 8B 8B 91 B8 2F 88 AB 87 A1 77 C2 A5 45 6B 02
            MAC                     :  CF A7 00 9E C5 02
   类型二   :  ADV_SCAN_REQ   (扫描询问手机的MAC: 7F F3 23 B5 38 AE)
      Access Address:   接入地址(4Byte),广播报文固定接入地址是0x8E89BED6
      AdvA     :  6Byte    (设备的MAC:0xCFA7009EC502)
      AdvData  :  23Byte   (0~31)   0A 09 4D 49 20 42 61 6E 64 20 32 03 02 E0 FE 07 16 E0 FE 2E 15 00 00
         Len1                       :  1B    0A
         Type1                      :  1B    09 (本地设备完整名称)
         Value1                     :  1B    4D 49 20 42 61 6E 64 20 32 (MI Band 2)
         Len2                       :  1B    03
         Type2                      :  1B    02 (16位服务UUIDs部分列表)
         Value2                     :  1B    E0 FE
         Len3                       :  1B    07
         Type3                      :  1B    16
         Value3                     :  1B    E0 FE 2E 15 00 00
            UUID  :  E0 FE
            步数  :  2E 15 (0x152E=5422步)
            未知  :  00 00
-----------------------------------------------------------------------------------
mesh网络
   mesh重要概念
      mesh是建立在BLE基础之上的协议，与BR/EDR无关
      mesh使用发布/订阅(publish/subscribe)消息系统
      节点数量：最多32767个节点
      中继：耗电相对较高，消息中继为1跳，网络最多127跳
      管理型网络泛洪(Managed Flooding):   非单一性消息传送
   协议栈分层
      models                  模型        定义模型以及其功能性
      foundation models       基础模型    定义设置与管理mesh网络所需的状态、消息与模型
      access layer            接入层      定义应用数据的格式以及更高层应用如何利用上层传输层的方式
                                          对应用层数据的格式、定义并控制加密解密过程，验证数据是否适用于正确的网络应用
      upper transport layer   上层传输层  对应用数据进行加密、解密、认证；传输控制消息，包括friendship相关的心跳和消息
      lower transport layer   底层传输层  PDU的分段与重组
      network layer           网络层      定义传输层消息如何被寻址至一个或多个元素
                                          定义各种消息地址类型和网络消息格式。中继和代理行为实施
      bearer layer            承载层      定义网络层消息如何被传输(广播承载层、GATT承载层)
      Bluetooth Low Energy    BLE   
   网络节点特性(元素，一个节点可以有多个元素)
      低功耗节点     Low-Power Node(LPN)
         参数-ReceiveDelay    询问Friend节点后,等待这些时间给Friend做准备,之后再启动接收
         参数-ReceiveWindow   接收时间
         参数-PollTimeout     两次发送给Friend的时间间隔，超时友谊的小船就翻了。
      Friend节点     与n个LPN配合成为friendship，为LPN存储消息和安全更新
      中继(Relay)    接收和转发
      代理(Proxy)    负责与手机通讯，两个GATT特性，mesh代理数据输入/输出
      ----------
      灯泡实例
         节点              =  灯泡     (可以包含1~n个元素)
            一个元素       =  主元素   (可以包含1~n个模型)  每个元素唯一的地址(单播地址)
               模型           每个模型唯一标识符(蓝牙技术联盟采用16bit，供应商采用32bit(16bit+16bit))
                     ------------------模型----------------------
                     |节点功能：     -开/关      -亮度            |
                     |元素条件/状态  -"开"/"关"  -0~10(亮度等级)  |
                     --------------------------------------------
   网络拓扑
                           friendship  
      (低功耗节点LPN)   <----------------->  (好友friends)  <---> 中继(Relay) <--->|
       手机(代理客户端)(启动配置设备)<---->  (代理节点Proxy)<---> 中继(Relay) <--->|
      ----------
   启动配置 (Provision)
      概念
         1，说明：将设备接入mesh成为节点
         2，启动配置设备(Provisioner)：运行启动配置应用程序的设备称为"启动配置设备"
         3，启动协议：Provisioner可通过PB-ADV或PB-GATT承载层使用配置协议
         4，提供密钥：Provisioner给要入网的设备提供NetKey。
         5，必要删除：待移除的设备含有网络密钥，为了防止收到网络攻击，建议执行删除步骤。
      加入流程：
         1，广播Beacon     ： 设备通过广播来声明自己是未经启动配置的设备，可被启动配置。
         2，邀请Invitation :  Provisioner向设备发出邀请(包含元素数量，相关算法等)，设备回应自身信息。
         3，交换公共密钥(Public Key):  所有节点都用到FIPS P-256椭圆曲线算法，必须拥有公共密钥。
         4，认证(Authentication):      Provisioner对新设备功能了解，向其发送消息。然后交换密码散列。
         5，启动配置数据的分配： 通过两台设备的私有密钥(PrivateKey)和交换的对等公共密钥生成的对话密钥(SessionKey).
      删除流程：
         1，使用Provisioner将待删除的设备加入"黑名单"。目的是不让待移除设备在下面步骤收到新密钥。
         2，执行"密钥刷新程序(Key Refresh Procedure)"流程，这个流程包含"第二阶段"过渡期，这时期新旧密钥均可用。
   友谊     (Friendship)
      建立
         1，LPN发布一个"好友请求"消息(Friend Request)，该消息不被中继，含有参数ReceiveDelay/ReceiveWindow/PollTimeout
         2，符合条件的Friend准备一个"Friend Offer"消息，并回发至LPN。
         3，LPN接收到"Friend Offer"消息时，通过用户算法选择合适的Friend节点，
            算法用户定,比如根据RSSI或ReceiveWindow或RAM容量等。
         4，LPN向Friend发送一个"Friend Poll"轮询消息
         5，Friend回复一个"Friend Update"更新消息，完成friendship建立流程并提供安全参数。到此建立成功
      消息传送
         1，好友节点缓存LPN的消息，将其存储在"好友"队列。
         2，LPN会周期性的启用其收发器，并向Friend发送"Friend Poll"消息
         3，Friend将被存储的消息发回至LPN作为响应
         4，LPN继续发送"Friend Poll"消息，直到收到一条"MD(MD更多数据)"字段设置为0的"Friend Update"消息为止
      终止
         1，PollTimeout超时，Friend未收到"Friend Poll"或"Friend Subscription List"增删。
         2，LPN发送"Friend Clear"。
      安全性
         主安全资料     ： 由NetKey派生，可被同一网络的其他节点使用，可由同一网络的任意节点解码。
                           适用消息:   "Friend Clear"、"Friend Clear Confirm"
         好友安全资料   ： NetKey+LPN+Friend联合派生，只能用于该LPN和Friend解码。
                           适用消息:   "Friend Poll"、"Friend Update"、"Friend Subscription List"
   地址  (Address 16bit)
      未分配地址     0b 0000 0000 0000 0000  不会用于消息传送
      单播地址       0b 0xxx xxxx xxxx xxxx  (除了"未分配地址")，每个设备唯一地址，在"启动配置"期间，由启动配置设备分配
      虚拟地址       0b 10xx xxxx xxxx xxxx  虚拟地址是与特定的UUID标签相关联的一组元素
      群组地址       0b 11xx xxxx xxxx xxxx  群组地址是一种多播地址
   消息  (message)
      消息分为两类
         控制消息(Control Message)  与Mess网络相关的消息,如心跳和friend的请求消息
         接入消息(Access Message)   应用消息
            经确认的    (acknowledged)
            未经确认的  (unacknowledged)
      消息交换 (发布/订阅)
   安全
      加密与认证     强制安全使用，每个数据包都加密。
         AES-CMAC    监测有意的、未经授权的数据修改，以及意外修改
         AES-CCM     通用的、认证的加密算法，使用时需要加密块加密。
      安全分级考量   
         应用密钥(AppKey)
         网络密钥(NetKey)  派生出网络加密密钥(EncryptionKey)和隐私密钥(PrivacyKey)
         设备密钥(DevKey)  用于Provisioning和配置流程
      区域隔离       可分子网，每个子网密钥不同各自独立
      密钥刷新
      消息模糊化
      中继攻击防护   基于序列号和IV索引的两个网络PDU字段
      垃圾桶攻击防护 设备移除机制
      启动配置       设备加入
   mesh相关资源
      蓝牙Mesh规范   Bluetooth Specification Mesh Profile   
         https://www.bluetooth.com/specifications/mesh-specifications
      蓝牙Mesh术语表 mesh-glossary
         https://www.bluetooth.com/bluetooth-technology/topology-options/le-mesh/mesh-glossary
      蓝牙Mesh概述   Bluetooth Mesh Overview
         白皮书，介绍技术细节
         https://www.bluetooth.com/bluetooth-technology/topology-options/le-mesh
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

#ifndef __BSP_NRF_BLE_H
#define __BSP_NRF_BLE_H
//-------------------加载库函数------------------------------
#include "includes.h"

#include <stdbool.h>
#include <stdint.h>
//-------------------数据结构--------------------------------
typedef enum BSP_NRF_BLE_MODE
{
    BSP_NRF_BLE_MODE_BEACON_S=0,
    BSP_NRF_BLE_MODE_NUM,
} BSP_NRF_BLE_MODE;
// 自定义协议
typedef enum BSP_NRF_BLE_FACTORY_PROTOCOL
{
    BSP_NRF_BLE_FACTORY_PROTOCOL_LEN1  =0,
    BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE1,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE1,
    BSP_NRF_BLE_FACTORY_PROTOCOL_LEN2,
    BSP_NRF_BLE_FACTORY_PROTOCOL_TYPE2,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_L,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_SN,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_LEN,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_TYPE,
    BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE,
} BSP_NRF_BLE_FACTORY_PROTOCOL;
//
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_NULL_TYPE       0x00
//
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_LEN             30 // LTV1(3B) L2(1B)T2(1B)V2(2B+23B)
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_COMPANY_L       0x59
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_COMPANY_H       0x00
// iCareB-C
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_NAME   "iCareB-C"
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE   0x81
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_LEN    0x03
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_VALUE_KEYNUM    0
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_VALUE_KEYTIME   1
// iCareB-M
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_NAME   "iCareB-M"
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_TYPE   0x82
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_LEN    0x02
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT     0
// MI Band 2
#define  BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_NAME      "MI Band 2"
#define  BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_TYPE      0xA0
#define  BSP_NRF_BLE_HUAMI_PROTOCOL_LEN               31
#define  BSP_NRF_BLE_HUAMI_PROTOCOL_COMPANY_L         0x57
#define  BSP_NRF_BLE_HUAMI_PROTOCOL_COMPANY_H         0x01
// NRF51822-TAG
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_NAME    "XSL-TAG"
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE    0xA1
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_LEN     0x10
#define  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_VALUE_EVENT     0
// BEACON
#define  BSP_NRF_BLE_BEACON_PROTOCOL_NAME             "iBeacon"
#define  BSP_NRF_BLE_BEACON_PROTOCOL_TYPE             0xA2
#define  BSP_NRF_BLE_BEACON_PROTOCOL_LEN              30
#define  BSP_NRF_BLE_BEACON_PROTOCOL_COMPANY_L        0x4C
#define  BSP_NRF_BLE_BEACON_PROTOCOL_COMPANY_H        0x00
//-------------------接口宏定义(硬件有关)--------------------
/*
BSP_NRFBLE_ENABLE_ADVERTISE   蓝牙广播
BSP_NRFBLE_ENABLE_BEACON      蓝牙Beacon广播
BSP_NRFBLE_ENABLE_SCAN        蓝牙扫描(解析广播)
BSP_NRFBLE_ENABLE_TIMER_10MS  10ms定时器
*/
#if   (defined(PROJECT_NRF5X_BLE))
#include "Bsp_Bmp180.h"
#include "Bsp_Ap3216c.h"
#include "Bsp_Mpu6050.h"
#include "Bsp_Key.h"
#include "Bsp_Led.h"
#define  BSP_NRFBLE_ENABLE_ADVERTISE
//#define  BSP_NRFBLE_ENABLE_SCAN
#define  BSP_NRFBLE_SCAN_RX_LED           BSP_LED_LED2_TOGGLE
#define  NON_CONNECTABLE_ADV_INTERVAL     MSEC_TO_UNITS(500, UNIT_0_625_MS)
// -40(NRF52), -30(NRF51), -20, -16, -12, -8, -4, 0, and 4 dBm
#define  BSP_NRFBLE_TXPOWER               0
//#define  BSP_NRFBLE_ENABLE_TIMER_10MS
//#define  BSP_NRFBLE_TIMER_10MS_CH1        BspKey_Main_10ms();
#elif (defined(XKAP_ICARE_B_C))
#include "Bsp_Key.h"
#define  BSP_NRFBLE_ENABLE_ADVERTISE
#define  BSP_NRFBLE_ENABLE_SCAN
#define  BSP_NRFBLE_SCAN_RX_LED
#define  NON_CONNECTABLE_ADV_INTERVAL     MSEC_TO_UNITS(1000, UNIT_0_625_MS)
#define  BSP_NRFBLE_TXPOWER               4
#define  BSP_NRFBLE_ENABLE_TIMER_10MS
#define  BSP_NRFBLE_TIMER_10MS_CH1        BspKey_Main_10ms();
#elif (defined(XKAP_ICARE_B_D))
#include "Bsp_Led.h"
#include "Bsp_Uart.h"
#define  BSP_NRFBLE_ENABLE_ADVERTISE
#define  BSP_NRFBLE_ENABLE_SCAN
#define  BSP_NRFBLE_SCAN_RX_LED           BSP_LED_LED2_TOGGLE
#define  NON_CONNECTABLE_ADV_INTERVAL     MSEC_TO_UNITS(1000, UNIT_0_625_MS)
#define  BSP_NRFBLE_TXPOWER               4
#define  BSP_NRFBLE_UART_X                1
#define  BSP_NRFBLE_UART_BSP              115200
#define  BSP_NRFBLE_UART_ADDR             1
#define  BSP_NRFBLE_ENABLE_TIMER_1MS
#define  BSP_NRFBLE_TIMER_1MS_CH1         UART_1ms_IRQ();
#elif (defined(XKAP_ICARE_B_M))
#include "uctsk_HCI.h"
#include "Bsp_Key.h"
#include "Bsp_BuzzerMusic.h"
#define  BSP_NRFBLE_ENABLE_ADVERTISE
#define  BSP_NRFBLE_ENABLE_SCAN
#define  BSP_NRFBLE_SCAN_RX_LED
#define  NON_CONNECTABLE_ADV_INTERVAL     MSEC_TO_UNITS(1000, UNIT_0_625_MS)
#define  BSP_NRFBLE_TXPOWER               4
#define  BSP_NRFBLE_ENABLE_TIMER_10MS
#define  BSP_NRFBLE_TIMER_10MS_CH1        BspKey_Main_10ms();
#define  BSP_NRFBLE_TIMER_10MS_CH2        Bsp_BuzzerMusic_PlayMusic_10ms()
#elif (defined(TEST_NRF52_V1))
#include "uctsk_HCI.h"
#include "Bsp_Key.h"
#include "Bsp_BuzzerMusic.h"
#define  BSP_NRFBLE_DISABLE
//#define  BSP_NRFBLE_ENABLE_ADVERTISE
//#define  BSP_NRFBLE_ENABLE_SCAN
//#define  BSP_NRFBLE_SCAN_RX_LED
//#define  NON_CONNECTABLE_ADV_INTERVAL     MSEC_TO_UNITS(1000, UNIT_0_625_MS)
//#define  BSP_NRFBLE_TXPOWER               4
#define  BSP_NRFBLE_ENABLE_TIMER_1MS
#define  BSP_NRFBLE_TIMER_1MS_CH1         UART_1ms_IRQ();
#define  BSP_NRFBLE_ENABLE_TIMER_10MS
#define  BSP_NRFBLE_TIMER_10MS_CH1        BspKey_Main_10ms();
#define  BSP_NRFBLE_TIMER_10MS_CH2        Bsp_BuzzerMusic_PlayMusic_10ms()
#elif (defined(BASE_NRF51)||defined(BASE_NRF52)||defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#define  BSP_NRFBLE_DISABLE
#else
#error Please Set Project to Bsp_NrfBle.h
#endif
//-------------------接口宏定义(硬件无关)--------------------
#define DEAD_BEEF                       0xDEADBEEF                        /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         2                                 /**< Size of timer operation queues. */
// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/
// -----Advertise-----
#if   (defined(BSP_NRFBLE_ENABLE_ADVERTISE))
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#ifndef  NON_CONNECTABLE_ADV_INTERVAL
#define  NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(2000, UNIT_0_625_MS) /**< 发送时间间隔(100~10240) The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#endif
#define APP_ADV_INFO_LENGTH             0x17
#define APP_COMPANY_IDENTIFIER          0x0059                            /**< Beacon厂商信息 Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
typedef struct BSP_NRFBLE_S_MANUF_DATA
{
    uint8_t state;  // 0-未使用 1-正在使用
    uint8_t SerialNum;
    uint8_t buf[APP_ADV_INFO_LENGTH-3];
    uint16_t crc16;
} BSP_NRFBLE_S_MANUF_DATA;
extern BSP_NRFBLE_S_MANUF_DATA BspNrfBle_s_Tx;
#endif
// -----Beacon-----
#if   (defined(BSP_NRFBLE_ENABLE_BEACON))
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
//#define IS_SRVC_CHANGED_CHARACT_PRESENT 0                                 /**< Include or not the service_changed characteristic. if not enabled, the server's database cannot be changed for the lifetime of the device*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#ifndef  NON_CONNECTABLE_ADV_INTERVAL
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(200, UNIT_0_625_MS) /**< 发送时间间隔(100~10240) The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#endif
#define APP_ADV_INFO_LENGTH             0x17                              /**< Beacon信息总长度 Total length of information advertised by the Beacon. */
#define APP_ADV_DATA_LENGTH             0x15                              /**< Beacon信息数据段长度 Length of manufacturer specific data in the advertisement. */
#define APP_DEVICE_TYPE                 0x02                              /**< Beacon数据类型 0x02 refers to Beacon. */
#define APP_MEASURED_RSSI               0xC3                              /**< Beacon基准RSSI The Beacon's measured RSSI at 1 meter distance in dBm. */
#define APP_COMPANY_IDENTIFIER          0x0059                            /**< Beacon厂商信息 Company identifier for Nordic Semiconductor ASA. as per www.bluetooth.org. */
#define APP_MAJOR_VALUE                 0x01, 0x02                        /**< Beacon主键值   Major value used to identify Beacons. */
#define APP_MINOR_VALUE                 0x03, 0x04                        /**< Beacon副键值   Minor value used to identify Beacons. */
#define APP_BEACON_UUID                 0x01, 0x12, 0x23, 0x34, \
                                        0x45, 0x56, 0x67, 0x78, \
                                        0x89, 0x9a, 0xab, 0xbc, \
                                        0xcd, 0xde, 0xef, 0xf0            /**< Beacon-UUID    Proprietary UUID for Beacon. */

#if defined(USE_UICR_FOR_MAJ_MIN_VALUES)
#define MAJ_VAL_OFFSET_IN_BEACON_INFO   18                                /**< Position of the MSB of the Major Value in m_beacon_info array. */
#define UICR_ADDRESS                    0x10001080                        /**< Address of the UICR register used by this example. The major and minor versions to be encoded into the advertising data will be picked up from this location. */
#endif
#endif
// -----Scan-----
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
#include "ble_db_discovery.h"
#include "ble_nus_c.h"
#if (NRF_SD_BLE_API_VERSION == 3)
#define NRF_BLE_MAX_MTU_SIZE    GATT_MTU_SIZE_DEFAULT           /**< MTU size used in the softdevice enabling and to reply to a BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST event. */
#endif

#define NUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN      /**< UUID type for the Nordic UART Service (vendor specific). */

#define SCAN_INTERVAL           0x00A0                          /**< 单周期时长(扫描窗口+休息时间)Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW             (SCAN_INTERVAL*0.8)/*0x0050*/   /**< 扫描窗口   Determines scan window in units of 0.625 millisecond. */
#define SCAN_ACTIVE             1                               /**< 扫描请求   If 1, performe active scanning (scan requests). */
#define SCAN_SELECTIVE          0                               /**< 启动白名单 If 1, ignore unknown devices (non whitelisted). */
#define SCAN_TIMEOUT            0x0000                          /**< 扫描超时   Timout when scanning. 0x0000 disables timeout. */

#define MIN_CONNECTION_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) /**< Determines minimum connection interval in millisecond. */
#define MAX_CONNECTION_INTERVAL MSEC_TO_UNITS(75, UNIT_1_25_MS) /**< Determines maximum connection interval in millisecond. */
#define SLAVE_LATENCY           0                               /**< Determines slave latency in counts of connection events. */
#define SUPERVISION_TIMEOUT     MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Determines supervision time-out in units of 10 millisecond. */

#define UUID16_SIZE             2                               /**< Size of 16 bit UUID */
#define UUID32_SIZE             4                               /**< Size of 32 bit UUID */
#define UUID128_SIZE            16                              /**< Size of 128 bit UUID */
#endif
//-------------------接口函数--------------------------------
void BspNrfBle_Init(void);
void BspNrfBle_Timer(uint16_t ms);
void BspNrfBle_ProtocolParse(uint8_t *pbuf,uint16_t len);
void BspNrfBle_DebugTestOnOff(uint8_t OnOff);
//-----------------------------------------------------------
#endif
