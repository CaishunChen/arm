/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2017-08-24
*                    功    能: MPU-6050驱动
***********************************************************************************
*/
//------------------------------- Includes --------------------
#include "Bsp_Mpu6050.h"
#include "Bsp_Twi.h"
#include "MemManager.h"
#include "uctsk_Debug.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"
//------------------------------- 数据结构 --------------------
//------------------------------- 用户变量 --------------------
//测试使能
static uint8_t BspMpu6050_DebugTest_Enable=0;
static uint8_t BspMpu6050_Init_State=0;   // 0-待初始化 1-初始化完毕
uint8_t BspMpu6050_TapCmt=0;
//------------------------------- 用户函数声明 ----------------
static void BspMpu6050_CallBak_Tap(unsigned char direction, unsigned char count);
static void BspMpu6050_CallBak_AndroidOrient(unsigned char orientation);
/*
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)

#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)

#define MOTION          (0)
#define NO_MOTION       (1)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)
*/
/* Starting sampling rate. */
//#define DEFAULT_MPU_HZ  (50)



static signed char gyro_orientation[9] = {-1, 0, 0,
                                          0,-1, 0,
                                          0, 0, 1
                                         };
/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}


static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static void run_self_test(void)
{
    int result;
//    char test_packet[4] = {0};
    //long gyro[3], accel[3];
    char *pbuf;
    long *p_gyro,*p_accel;
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    p_gyro = (long*)&pbuf[100];
    p_accel= (long*)&pbuf[120];
    result = mpu_run_self_test(p_gyro, p_accel);
    if (result == 0x7)
    {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        p_gyro[0] = (long)(p_gyro[0] * sens);
        p_gyro[1] = (long)(p_gyro[1] * sens);
        p_gyro[2] = (long)(p_gyro[2] * sens);
        dmp_set_gyro_bias(p_gyro);
        mpu_get_accel_sens(&accel_sens);
        p_accel[0] *= accel_sens;
        p_accel[1] *= accel_sens;
        p_accel[2] *= accel_sens;
        dmp_set_accel_bias(p_accel);
        sprintf(pbuf,"setting bias succesfully ......\r\n");
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"bias has not been modified ......\r\n");
        DebugOutStr((int8_t*)pbuf);
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);

    /* Report results. */
//    test_packet[0] = 't';
//    test_packet[1] = result;
//    send_packet(PACKET_TYPE_MISC, test_packet);
}

static void BspMpu6050_CallBak_Tap(unsigned char direction, unsigned char count)
{
    uint8_t data[2];
    data[0] = (char)direction;
    data[1] = (char)count;
    //send_packet(PACKET_TYPE_TAP, data);
    DebugLogOutHex(DEBUG_E_LEVEL_INFO,"BspMpu6050_CallBak_Tap:",data,2);
    BspMpu6050_TapCmt++;
}
static void BspMpu6050_CallBak_AndroidOrient(unsigned char orientation)
{
    //send_packet(PACKET_TYPE_ANDROID_ORIENT, &orientation);
    DebugLogOutHex(DEBUG_E_LEVEL_INFO,"BspMpu6050_CallBak_AndroidOrient:",&orientation,1);
}
//-------------------------------------------------------------
uint8_t BspMpu6050_Init(void)
{
    char *pbuf;
    uint16_t i16;
    BspMpu6050_Init_State=0;
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    // 验证芯片身份
    BspTwi_Read_nByte(BSP_MPU6050_ADDR,BSP_MPU6050_REG_WHO_AM_I,(uint8_t *)pbuf,1);
    if(pbuf[0]==0x68)
    {
        sprintf((char*)pbuf,"MPU6050 Confirm!\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
	else if(pbuf[0]==0x71)
	{
		sprintf((char*)pbuf,"MPU9250 Confirm!\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
	}
    else
    {
        sprintf((char*)pbuf,"MPUXXXX Not Confirm!\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 复位芯片
    pbuf[0]=BSP_MPU6050_REG_PWR_MGMT_1;
    pbuf[1]=BSP_MPU6050_REG_PWR_MGMT_1__DEVICE_RESET;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,(uint8_t *)pbuf,2);
    // 硬件初始化
    pbuf[0] = mpu_init(NULL);
    if(pbuf[0]!=OK)
    {
        return ERR;
    }
    sprintf(pbuf,"mpu initialization complete......\r\n ");
    DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    // 设置IIC主线为Bypass模式
    //mpu_set_bypass();
    // 设置传感器
    if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
    {
        sprintf(pbuf,"mpu_set_sensor complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"mpu_set_sensor come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 配置FIFO
    if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))
    {
        sprintf(pbuf,"mpu_configure_fifo complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"mpu_configure_fifo come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 设置采样率
    if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))
    {
        sprintf(pbuf,"mpu_set_sample_rate complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"mpu_set_sample_rate error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    //-----读取部分配置
    if(BspMpu6050_DebugTest_Enable==1)
    {
        // 获取采样率
        pbuf[0]=mpu_get_sample_rate(&i16);
        if(pbuf[0]==OK)
        {
            sprintf((char*)pbuf,"Sample Rate(Hz):%d\r\n",i16);
        }
        else
        {
            sprintf((char*)pbuf,"Sample Rate(Hz):ERR\r\n");
        }
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
        // 获取陀螺仪量程
        pbuf[0]=mpu_get_gyro_fsr(&i16);
        if(pbuf[0]==OK)
        {
            sprintf((char*)pbuf,"Gyro fsr(dps)  :%d\r\n",i16);
        }
        else
        {
            sprintf((char*)pbuf,"Gyro fsr(dps)  :ERR\r\n");
        }
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
        // 获取加速度量程
        pbuf[0]=mpu_get_accel_fsr((uint8_t*)&pbuf[100]);
        if(pbuf[0]==OK)
        {
            sprintf((char*)pbuf,"Accel fsr(g)   :%d\r\n",pbuf[100]);
        }
        else
        {
            sprintf((char*)pbuf,"Accel fsr(g)   :ERR\r\n");
        }
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    //-----初始化DMP-----Begin
    // 1,装载DMP固件
    if(!dmp_load_motion_driver_firmware())
    {
        sprintf(pbuf,"dmp_load_motion_driver_firmware complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"dmp_load_motion_driver_firmware come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 2,设置方向矩阵
    if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))
    {
        sprintf(pbuf,"dmp_set_orientation complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"dmp_set_orientation come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 3,设置事件回调函数
    dmp_register_tap_cb(BspMpu6050_CallBak_Tap);
    dmp_register_android_orient_cb(BspMpu6050_CallBak_AndroidOrient);
    // 4,使能相关功能
    //   DMP_FEATURE_LP_QUAT        :  根据陀螺仪/200Hz/低功耗
    //      Generate a gyro-only quaternion on the DMP at 200Hz.
    //      Integrating the gyro data at higher rates reduces numerical errors
    //      (compared to integration on the MCU at a lower sampling rate).
    //   DMP_FEATURE_6X_LP_QUAT     :  根据陀螺仪+加速度/200Hz/低功耗
    //      Generate a gyro/accel quaternion on the DMP at 200Hz.
    //      Cannot be used in combination with DMP_FEATURE_LP_QUAT.
    //   DMP_FEATURE_TAP            :  单击/双击/轻敲方向
    //      Detect taps along the X, Y, and Z axes.
    //   DMP_FEATURE_ANDROID_ORIENT :  一个状态机,计算显示方向
    //      Google's screen rotation algorithm.
    //      Triggers an event at the four orientations where the screen should rotate.
    //   DMP_FEATURE_GYRO_CAL       :  每次设备静止8秒，校准陀螺仪零偏
    //      Calibrates the gyro data after eight seconds of no motion.
    //   DMP_FEATURE_SEND_RAW_ACCEL :  将加速度计的raw轴(偏航)数据放入FIFO,此数据基于芯片坐标系
    //      Add raw accelerometer data to the FIFO.
    //   DMP_FEATURE_SEND_RAW_GYRO  :  将陀螺仪的raw轴(偏航)数据放入FIFO,此数据基于芯片坐标系
    //      Add raw gyro data to the FIFO.
    //   DMP_FEATURE_SEND_CAL_GYRO  :  将陀螺仪的校准后的数据放入FIFO
    //      Add calibrated gyro data to the FIFO.
    //      Cannot be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
    if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT \
                           | DMP_FEATURE_TAP \
                           | DMP_FEATURE_ANDROID_ORIENT \
                           | DMP_FEATURE_SEND_RAW_ACCEL \
                           | DMP_FEATURE_SEND_CAL_GYRO \
                           | DMP_FEATURE_GYRO_CAL))
    {
        sprintf(pbuf,"dmp_enable_feature complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"dmp_enable_feature come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    // 5,设置FIFO速率
    if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))
    {
        sprintf(pbuf,"dmp_set_fifo_rate complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"dmp_set_fifo_rate come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }

    //
    run_self_test();
    // 6,开启DMP
    if(!mpu_set_dmp_state(1))
    {
        sprintf(pbuf,"mpu_set_dmp_state complete ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    else
    {
        sprintf(pbuf,"mpu_set_dmp_state come across error ......\r\n");
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
    }
    //-----初始化DMP-----End
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    BspMpu6050_Init_State=1;
    return OK;
    /*
    uint8_t buf[2];
    // 复位
    buf[0]=BSP_MPU6050_REG_SIGNAL_PATH_RESET;
    buf[1]=0x04U | 0x02U | 0x01U;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    // 设置 GYRO
    //   设置采样率     -- Sample Rate  = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    //                     Gyroscope Output Rate=8kHz(DLPF Disable)/1kHz(DLPF Enable)
    buf[0]=BSP_MPU6050_REG_SMPLRT_DIV;
    buf[1]=0x00;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   配置           -- EXT_SYNC_SET = 0 (禁用同步晶振输入脚)
    //                     DLPF_CFG     = 0 (数字低通滤波)ACC bandwidth = 260Hz  GYRO bandwidth = 256Hz)
    buf[0]=BSP_MPU6050_REG_CONFIG;
    buf[1]=BSP_MPU6050_REG_CONFIG__EXT_SYNC_SET__INPUT_DISABLED\
           |BSP_MPU6050_REG_CONFIG__DLPF_CFG__A260_G256;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   功耗管理1      -- SLEEP 0  (休眠)
    //                     CYCLE 0  (休眠与唤醒循环切换,需要在寄存器(功耗管理2)配置唤醒频率)
    //                     TEMP_DIS (温度传感器使能 0使能 1禁用)
    //                     CLKSEL 3 (PLL with Z Gyro reference)
    buf[0]=BSP_MPU6050_REG_PWR_MGMT_1;
    buf[1]=BSP_MPU6050_REG_PWR_MGMT_1__CLKSEL__ZG;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   gyro配置       -- XG_ST/YG_ST/ZG_ST (自测触发)
    //                     FS_SEL   (陀螺仪量程选择 0-250 1-500 2-1000 3-2000)
    buf[0]=BSP_MPU6050_REG_GYRO_CONFIG;
    buf[1]=BSP_MPU6050_REG_GYRO_CONFIG__FS_SEL_1000;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   用户配置       -- FIFO_EN        (使能FIFO)
    //                     I2C_MST_EN     (使能I2C主机模式)
    //                     I2C_IF_DIS     (MPU6000:禁用主IIC使能SPI, MPU6050:总写0)
    //                     FIFO_RESET     (FIFO复位,自恢复)
    //                     I2C_MST_RESET  (I2C主机复位)
    //                     SIG_COND_RESET (复位传感器路径与数据)
    buf[0]=BSP_MPU6050_REG_USER_CTRL;
    buf[1]=0x00;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   中断配置       -- 推挽输出，高电平中断，一直输出高电平直到中断清除，任何读取操作都清除中断
    //                     INT_LEVEL         (0 -  中断高电平有效    1  -  中断低电平有效)
    //                     INT_OPEN          (0 -  上拉              1  -  开漏)
    //                     LATCH_INT_EN      (0 -  50us脉冲          1  -  读取自动清除)
    //                     INT_RD_CLEAR      (0 -  仅读中断状态寄存器(58)中断状态清空  1  -  读任何寄存器都会时中断状态清空)
    //                     FSYNC_INT_LEVEL   (0 -  FSYNC引脚高有效   1  -  FSYNC引脚低有效)
    //                     FSYNC_INT_EN      (0 -  禁用FSYNC引脚     1  -  使能FSYNC引脚)
    //                     I2C_BYPASS_EN使能 pass through 功能 直接在6050 读取5883数据
    buf[0]=BSP_MPU6050_REG_INT_PIN_CFG;
    buf[1]=BSP_MPU6050_REG_INT_PIN_CFG__LATCH_INT_EN
           |BSP_MPU6050_REG_INT_PIN_CFG__INT_RD_CLEAR
           |BSP_MPU6050_REG_INT_PIN_CFG__IIC_BYPASS_EN;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   中断使能       -- FIFO_OFLOW_EN     FIFO溢出使能
    //                     I2C_MST_INT_EN    I2C主中断源使能
    //                     DATA_RDY_EN       数据准备中断
    buf[0]=BSP_MPU6050_REG_INT_ENABLE;
    buf[1]=BSP_MPU6050_REG_INT_ENABLE_DATA_RDY_EN;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    //   ACC设置        -- XA_ST             X轴加速度自测
    //                     YA_ST             Y轴加速度自测
    //                     ZA_ST             Z轴加速度自测
    //                     AFS_SEL           量程 (0~+-2g   1~+-4g   2~+-8g   3~+-16g)
    buf[0]=BSP_MPU6050_REG_ACCEL_CONFIG;
    buf[1]=BSP_MPU6050_REG_GYRO_CONFIG__AFS_SEL_2;
    BspTwi_Write_nByte(BSP_MPU6050_ADDR,buf, 2);
    return OK;
    */
}

uint8_t BspMpu6050_ReadTemp(int16_t *pTemp)
{
    uint8_t buf[2];
    int16_t i16;
    BspTwi_Read_nByte(BSP_MPU6050_ADDR,BSP_MPU6050_REG_TEMP_OUT_H,buf,2);
    i16 = Count_2ByteToWord(buf[0],buf[1]);
    i16 = i16/34+365;
    *pTemp=i16;
    return OK;
}
/*
uint8_t BspMpu6050_ReadGyro(int16_t *pGYRO_X,int16_t *pGYRO_Y,int16_t *pGYRO_Z)
{
    uint8_t buf[6];
    BspTwi_Read_nByte(BSP_MPU6050_ADDR,BSP_MPU6050_REG_GYRO_XOUT_H,buf,6);
    *pGYRO_X = Count_2ByteToWord(buf[0],buf[1]);
    if(*pGYRO_X & 0x8000) *pGYRO_X-=65536;

    *pGYRO_Y = Count_2ByteToWord(buf[2],buf[3]);
    if(*pGYRO_Y & 0x8000) *pGYRO_Y-=65536;

    *pGYRO_Z = Count_2ByteToWord(buf[4],buf[5]);
    if(*pGYRO_Z & 0x8000) *pGYRO_Z-=65536;
    return OK;
}
uint8_t BspMpu6050_ReadAcc(int16_t *pACC_X,int16_t *pACC_Y,int16_t *pACC_Z)
{

    uint8_t buf[6];
    BspTwi_Read_nByte(BSP_MPU6050_ADDR,BSP_MPU6050_REG_ACCEL_XOUT_H,buf,6);
    *pACC_X = Count_2ByteToWord(buf[0],buf[1]);
    if(*pACC_X & 0x8000) *pACC_X-=65536;

    *pACC_Y = Count_2ByteToWord(buf[2],buf[3]);
    if(*pACC_Y & 0x8000) *pACC_Y-=65536;

    *pACC_Z = Count_2ByteToWord(buf[4],buf[5]);
    if(*pACC_Z & 0x8000) *pACC_Z-=65536;
    return OK;
}
*/
void BspMpu6050_DebugTest_100ms(void)
{
    uint8_t *pbuf;
    //int16_t i16=0,j16=0,k16=0;
    //static uint8_t s_count=100;
    static uint8_t s_first=1;
    if(s_first==1)
    {
        s_first=0;
        //
        BspMpu6050_Init();
    }
    //申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    memset((char*)pbuf,0,256);
    COUNT_S_DATATYPE *psDataType=(COUNT_S_DATATYPE *)&pbuf[100];
    pbuf[0]  =  140;
    psDataType->p32   =  (int32_t*)&pbuf[pbuf[0]];  //4 // 16B  0-3 四元数(Q30)
    pbuf[0]  += 16;
    psDataType->pu32  =  (uint32_t*)&pbuf[pbuf[0]]; // 4B   0   时间戳
    pbuf[0]  += 4;
    psDataType->pf    =  (float*)&pbuf[pbuf[0]];    // 16B  0-3 四元数(float)
    // 12B  4-6 欧拉角
    pbuf[0]  += (16+12);
    psDataType->p16   =  (int16_t*)&pbuf[pbuf[0]];  // 6B   0-2 陀螺仪
    // 6B   3-5 加速度
    // 2B   6   传感器判断
    // 2B   7   温度
    pbuf[0]  += (6+6+2+2);

    if(BspMpu6050_DebugTest_Enable==1)
    {

        while(1)
        {
            //-----XSL-----Begin
            //mpu_reset_fifo();
            //MODULE_OS_DELAY_MS(10);
            //-----XSL-----End
            // 读取陀螺仪/加速度/四元数/时间/传感器/etc
            pbuf[0]=dmp_read_fifo((short*)&psDataType->p16[0], (short*)&psDataType->p16[3], (long*)psDataType->p32, (unsigned long*)psDataType->pu32, (short*)&psDataType->p16[6], &pbuf[250]);
            if(pbuf[0]!=OK)
            {
                break;
            }
            if (psDataType->p16[6] & INV_WXYZ_QUAT )
            {
                // q30转float
                arm_q30_to_float(psDataType->p32,&psDataType->pf[0],4);
                // 四元数转欧拉角
                Count_Quaternion_To_EulerAngle(&psDataType->pf[0],&psDataType->pf[4]);
                // 打印欧拉角
                sprintf((char*)pbuf,"%f2,%f2,%f2\r\n",psDataType->pf[4],psDataType->pf[5],psDataType->pf[6]);
                DebugOutStr((int8_t*)pbuf);
            }
        }
        /*
        pbuf[0]=BspMpu6050_ReadTemp(&psDataType->p16[7]);
        if(pbuf[0]==OK)
        {
            sprintf((char*)pbuf,"Temp -%d.%d\r\n",psDataType->p16[7]/10,psDataType->p16[7]%10);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            sprintf((char*)pbuf,"Temp-ERR\r\n");
            DebugOutStr((int8_t*)pbuf);
        }
        */

    }
    else
    {
        while(BspMpu6050_Init_State==1)
        {
            pbuf[0]=dmp_read_fifo((short*)&psDataType->p16[0], (short*)&psDataType->p16[3], (long*)psDataType->p32, (unsigned long*)psDataType->pu32, (short*)&psDataType->p16[6], &pbuf[250]);
            if(pbuf[0]!=OK)
            {
                break;
            }
        }
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
void BspMpu6050_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspMpu6050_DebugTest_Enable=1;
    }
    else
    {
        BspMpu6050_DebugTest_Enable=0;
    }
}

