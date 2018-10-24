/**
  ******************************************************************************
  * @file    Bsp_Mpu6050.h 
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  * @brief   bottom-driven -->   MPU-6050驱动.
  * @note    
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
*  1,MPU6050说明
*     9轴运动处理传感器(3轴MEMS陀螺仪,3轴MEMS加速度计,1个数字运动处理器,温度)
*     价格: 5 RMB
*     精度: 16位ADC数值
*     范围: 陀螺仪(±250/500/1000/2000)(°/s)-dps)
*           加速度(±2/4/8/16-g)
*           温度  (-40 ~ +85) 误差±1
*     片上: FIFO-1024B,可编程低通滤波器
*     供电: 2.375-3.46V
*     输出: 以数字输出6轴或9轴的旋转矩阵/四元素/欧拉角格式的融合演算数据
*           旋转矩阵 :  
*           四元数   :  4个数来表征姿态(q30数据格式)
*           欧拉角   :  (由四元数计算)yaw(航向角)/roll(翻滚角)/pitch(俯仰角)
*     DMP使用与输出:
*           使用步骤 :
*              推送DMP映像到MPU内存 :  
*                 dmp_load_motion_driver_firmware()
*              推送陀螺仪和加速度的方向矩阵到DMP
*                 dmp_set_orientation()
*              DMP回调
*                 dmp_register_android_orient_cb(android_orient_cb)
*              使能DMP特性
*                 dmp_enable_feature()
*              数据速率
*                 mpu_set_fifo_rate (input)
*           四元数   : 
*              DMP_FEATURE_LP_QUAT  :  根据陀螺仪/200Hz/低功耗.
*              DMP_FEATURE_6X_LP_QUAT: 根据陀螺仪+加速度/200Hz/低功耗
*           轻敲
*              DMP_FEATURE_TAP      :  单击/双击/轻敲方向
*           显示方向 :
*              DMP_FEATURE_ANDROID_ORIENT:   一个状态机,计算显示方向
*           计步     :
*              DMP_PEDOMETER        :  计步特性,一直使能,可复位数值,查询步行时间及步数,7步延时避免误判
*           自动校准 :
*              DMP_FEATURE_GYRO_CAL :  每次设备静止8秒，校准陀螺仪零偏
*           导入偏航 :
*              DMP_FEATURE_SEND_RAW_ACCEL :  将加速度计的raw轴(偏航)数据放入FIFO,此数据基于芯片坐标系
*              DMP_FEATURE_SEND_RAW_GYRO  :  将陀螺仪的raw轴(偏航)数据放入FIFO,此数据基于芯片坐标系
*           校准     :  不能与"导入偏航"结合使用
*              DMP_FEATURE_SEND_CAL_GYRO  :  将陀螺仪的校准后的数据放入FIFO
*           自测     :
*              int mpu_run_self_test(long* gyro, long* accel)
*     算法: 姿态
*              积分  :  
*              卡尔曼:  
*           稳定
*              PID   :  
*     功耗: 陀螺仪-工作3.6mA,待机5uA
*           加速度-工作500uA,省电模式10uA(1.25Hz)/20uA(5Hz)/60uA(20Hz)/110uA(40Hz)
*           GYRO+ACC+DMP   3.9mA
*           GYRO+ACC       3.8mA
*           GYRO+DMP       3.7mA
*           GYRO           3.6mA
*           ACC            500uA
*     中断: 自由下落 :  达到阈值立即产生,中断时长可设
*           活动     :  达到阈值一定时长后产生,阈值与中断时长可设
*           静止     :  达到阈值一定时长后产生,阈值与中断时长可设
*           FIFO溢出 :
*           数据完成 :
*           IIC主机错误 :
*           IIC从机  :
*     封装承重: 10000g
  @endverbatim      
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL) 
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */

//-------------------------------------------------------------------------------
#ifndef __BSP_MPU6050_H
#define __BSP_MPU6050_H
//-------------------加载库函数------------------------------
#include "includes.h"
//-------------------接口宏定义(硬件相关)--------------------
// 器件地址
//#define BSP_MPU6050_ADDR                     0xD0
#define BSP_MPU6050_ADDR                     0xD2
// 寄存器地址
#define BSP_MPU6050_REG_SELF_TEST_X          0x0D
#define BSP_MPU6050_REG_SELF_TEST_Y          0x0E
#define BSP_MPU6050_REG_SELF_TEST_Z          0x0F
#define BSP_MPU6050_REG_SELF_TEST_A          0x10
#define BSP_MPU6050_REG_SMPLRT_DIV           0x19
#define BSP_MPU6050_REG_CONFIG               0x1A
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__INPUT_DISABLED   (0<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__TEMP_OUT_L       (1<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__GYRO_XOUT_L      (2<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__GYRO_YOUT_L      (3<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__GYRO_ZOUT_L      (4<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__ACCEL_XOUT_L     (5<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__ACCEL_YOUT_L     (6<<3)
#define BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__ACCEL_ZOUT_L     (7<<3)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A260_G256            (0<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A184_G188            (1<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A94_G98              (2<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A44_G42              (3<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A21_G20              (4<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A10_G10              (5<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__A5_G5                (6<<0)
#define BSP_MPU6050_REG_CONFIG__DLPF_CFG__RESERVED             (7<<0)
#define BSP_MPU6050_REG_GYRO_CONFIG          0x1B
#define BSP_MPU6050_REG_GYRO_CONFIG__XG_ST                     (1<<7)
#define BSP_MPU6050_REG_GYRO_CONFIG__YG_ST                     (1<<6)
#define BSP_MPU6050_REG_GYRO_CONFIG__ZG_ST                     (1<<5)
#define BSP_MPU6050_REG_GYRO_CONFIG__FS_SEL_250                (0<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__FS_SEL_500                (1<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__FS_SEL_1000               (2<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__FS_SEL_2000               (3<<3)
#define BSP_MPU6050_REG_ACCEL_CONFIG         0x1C
#define BSP_MPU6050_REG_ACCEL_CONFIG__XA_ST                    (1<<7)
#define BSP_MPU6050_REG_ACCEL_CONFIG__YA_ST                    (1<<6)
#define BSP_MPU6050_REG_ACCEL_CONFIG__ZA_ST                    (1<<5)
#define BSP_MPU6050_REG_GYRO_CONFIG__AFS_SEL_2                 (0<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__AFS_SEL_4                 (1<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__AFS_SEL_8                 (2<<3)
#define BSP_MPU6050_REG_GYRO_CONFIG__AFS_SEL_16                (3<<3)
#define BSP_MPU6050_REG_FIFO_EN              0x23
#define BSP_MPU6050_REG_I2C_MST_CTRL         0x24
#define BSP_MPU6050_REG_I2C_SLV0_ADDR        0x25
#define BSP_MPU6050_REG_I2C_SLV0_REG         0x26
#define BSP_MPU6050_REG_I2C_SLV0_CTRL        0x27
#define BSP_MPU6050_REG_I2C_SLV1_ADDR        0x28
#define BSP_MPU6050_REG_I2C_SLV1_REG         0x29
#define BSP_MPU6050_REG_I2C_SLV1_CTRL        0x2A
#define BSP_MPU6050_REG_I2C_SLV2_ADDR        0x2B
#define BSP_MPU6050_REG_I2C_SLV2_REG         0x2C
#define BSP_MPU6050_REG_I2C_SLV2_CTRL        0x2D
#define BSP_MPU6050_REG_I2C_SLV3_ADDR        0x2E
#define BSP_MPU6050_REG_I2C_SLV3_REG         0x2F
#define BSP_MPU6050_REG_I2C_SLV3_CTRL        0x30
#define BSP_MPU6050_REG_I2C_SLV4_ADDR        0x31
#define BSP_MPU6050_REG_I2C_SLV4_REG         0x32
#define BSP_MPU6050_REG_I2C_SLV4_DO          0x33
#define BSP_MPU6050_REG_I2C_SLV4_CTRL        0x34
#define BSP_MPU6050_REG_I2C_SLV4_DI          0x35
#define BSP_MPU6050_REG_I2C_MST_STATUS       0x36
#define BSP_MPU6050_REG_INT_PIN_CFG          0x37
#define BSP_MPU6050_REG_INT_PIN_CFG__INT_LEVEL                 (1<<7)
#define BSP_MPU6050_REG_INT_PIN_CFG__INT_OPEN                  (1<<6)
#define BSP_MPU6050_REG_INT_PIN_CFG__LATCH_INT_EN              (1<<5)
#define BSP_MPU6050_REG_INT_PIN_CFG__INT_RD_CLEAR              (1<<4)
#define BSP_MPU6050_REG_INT_PIN_CFG__FSYNC_INT_LEVEL           (1<<3)
#define BSP_MPU6050_REG_INT_PIN_CFG__FSYNC_INT_EN              (1<<2)
#define BSP_MPU6050_REG_INT_PIN_CFG__IIC_BYPASS_EN             (1<<1)
#define BSP_MPU6050_REG_INT_ENABLE           0x38
#define BSP_MPU6050_REG_INT_ENABLE__FIFO_OFLOW_EN              (1<<4)
#define BSP_MPU6050_REG_INT_ENABLE__I2C_MST_INT_EN             (1<<3)
#define BSP_MPU6050_REG_INT_ENABLE_DATA_RDY_EN                 (1<<0)
#define BSP_MPU6050_REG_INT_STATUS           0x3A
#define BSP_MPU6050_REG_ACCEL_XOUT_H         0x3B
#define BSP_MPU6050_REG_ACCEL_XOUT_L         0x3C
#define BSP_MPU6050_REG_ACCEL_YOUT_H         0x3D
#define BSP_MPU6050_REG_ACCEL_YOUT_L         0x3E
#define BSP_MPU6050_REG_ACCEL_ZOUT_H         0x3F
#define BSP_MPU6050_REG_ACCEL_ZOUT_L         0x40
#define BSP_MPU6050_REG_TEMP_OUT_H           0x41
#define BSP_MPU6050_REG_TEMP_OUT_L           0x42
#define BSP_MPU6050_REG_GYRO_XOUT_H          0x43
#define BSP_MPU6050_REG_GYRO_XOUT_L          0x44
#define BSP_MPU6050_REG_GYRO_YOUT_H          0x45
#define BSP_MPU6050_REG_GYRO_YOUT_L          0x46
#define BSP_MPU6050_REG_GYRO_ZOUT_H          0x47
#define BSP_MPU6050_REG_GYRO_ZOUT_L          0x48
#define BSP_MPU6050_REG_EXT_SENS_DATA_00     0x49
#define BSP_MPU6050_REG_EXT_SENS_DATA_01     0x4A
#define BSP_MPU6050_REG_EXT_SENS_DATA_02     0x4B
#define BSP_MPU6050_REG_EXT_SENS_DATA_03     0x4C
#define BSP_MPU6050_REG_EXT_SENS_DATA_04     0x4D
#define BSP_MPU6050_REG_EXT_SENS_DATA_05     0x4E
#define BSP_MPU6050_REG_EXT_SENS_DATA_06     0x4F
#define BSP_MPU6050_REG_EXT_SENS_DATA_07     0x50
#define BSP_MPU6050_REG_EXT_SENS_DATA_08     0x51
#define BSP_MPU6050_REG_EXT_SENS_DATA_09     0x52
#define BSP_MPU6050_REG_EXT_SENS_DATA_10     0x53
#define BSP_MPU6050_REG_EXT_SENS_DATA_11     0x54
#define BSP_MPU6050_REG_EXT_SENS_DATA_12     0x55
#define BSP_MPU6050_REG_EXT_SENS_DATA_13     0x56
#define BSP_MPU6050_REG_EXT_SENS_DATA_14     0x57
#define BSP_MPU6050_REG_EXT_SENS_DATA_15     0x58
#define BSP_MPU6050_REG_EXT_SENS_DATA_16     0x59
#define BSP_MPU6050_REG_EXT_SENS_DATA_17     0x5A
#define BSP_MPU6050_REG_EXT_SENS_DATA_18     0x5B
#define BSP_MPU6050_REG_EXT_SENS_DATA_19     0x5C
#define BSP_MPU6050_REG_EXT_SENS_DATA_20     0x5D
#define BSP_MPU6050_REG_EXT_SENS_DATA_21     0x5E
#define BSP_MPU6050_REG_EXT_SENS_DATA_22     0x5F
#define BSP_MPU6050_REG_EXT_SENS_DATA_23     0x60
#define BSP_MPU6050_REG_I2C_SLV0_DO          0x63
#define BSP_MPU6050_REG_I2C_SLV1_DO          0x64
#define BSP_MPU6050_REG_I2C_SLV2_DO          0x65
#define BSP_MPU6050_REG_I2C_SLV3_DO          0x66
#define BSP_MPU6050_REG_I2C_MST_DELAY_CTRL   0x67
#define BSP_MPU6050_REG_SIGNAL_PATH_RESET    0x68
#define BSP_MPU6050_REG_USER_CTRL            0x6A
#define BSP_MPU6050_REG_USER_CTRL__FIFO_EN                     (1<<6)
#define BSP_MPU6050_REG_USER_CTRL__IIC_MST_EN                  (1<<5)
#define BSP_MPU6050_REG_USER_CTRL__IIC_IF_DIS                  (1<<4)
#define BSP_MPU6050_REG_USER_CTRL__FIFO_RESET                  (1<<2)
#define BSP_MPU6050_REG_USER_CTRL__IIC_MST_RESET               (1<<1)
#define BSP_MPU6050_REG_USER_CTRL__SIG_COND_RESET              (1<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1           0x6B  /*默认: 0x40*/
#define BSP_MPU6050_REG_PWR_MGMT_1__DEVICE_RESET               (1<<7)
#define BSP_MPU6050_REG_PWR_MGMT_1__SLEEP                      (1<<6)
#define BSP_MPU6050_REG_PWR_MGMT_1__CYCLE                      (1<<5)
#define BSP_MPU6050_REG_PWR_MGMT_1__TEMP_DIS                   (1<<3)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__INTERNAL_8MHZ      (0<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__XG                 (1<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__YG                 (2<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__ZG                 (3<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__EXTERNAL_32768     (4<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__EXTERNAL_19200K    (5<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__RESERVED           (6<<0)
#define BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__STOP               (7<<0)
#define BSP_MPU6050_REG_PWR_MGMT_2           0x6C
#define BSP_MPU6050_REG_FIFO_COUNTH          0x72
#define BSP_MPU6050_REG_FIFO_COUNTL          0x73
#define BSP_MPU6050_REG_FIFO_R_W             0x74
#define BSP_MPU6050_REG_WHO_AM_I             0x75  /*默认: 0x68*/
// 寄存器地址位

//-------------------接口宏定义(硬件无关)--------------------
#define DEFAULT_MPU_HZ  (100)

extern uint8_t BspMpu6050_TapCmt;
//-------------------接口函数--------------------------------
uint8_t BspMpu6050_Init(void);
uint8_t BspMpu6050_ReadGyro(int16_t *pGYRO_X,int16_t *pGYRO_Y,int16_t *pGYRO_Z);
uint8_t BspMpu6050_ReadAcc(int16_t *pACC_X,int16_t *pACC_Y,int16_t *pACC_Z);
void BspMpu6050_DebugTest_100ms(void);
void BspMpu6050_DebugTestOnOff(uint8_t OnOff);
//-------------------------------------------------------------------------------
#endif


