/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
*  ����printf/sprintf��ת���ַ�:
*     %Ns      ���ָ������(N)���ַ��� ,  ����ʱ���ض�   ,����ʱ�Ҷ���
*     %-Ns     ���ָ������(N)���ַ��� ,  ����ʱ���ض�   ,����ʱ�����
*     %N.Ms    ���ָ������(N)���ַ��� ,  ����(M)ʱ�ض�  ,����ʱ�Ҷ���
*     %-N.Ms   ���ָ������(N)���ַ��� ,  ����(M)ʱ�ض�  ,����ʱ�����
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "includes.h"
#include "Bsp_Pwr.h"
#include "Module_Memory.h"
//------------------------------- Ԥ���� -------------------------------------
#ifdef DEBUG_GLOBAL
#define DEBUG_EXT
#undef DEBUG_GLOBAL
//------------------------------- ���ݽṹ -----------------------------------
typedef enum DEBUG_E_LEVEL
{
   DEBUG_E_LEVEL_ALWAYS=0,
   DEBUG_E_LEVEL_ERROR,
   DEBUG_E_LEVEL_WARNING,
   DEBUG_E_LEVEL_INFO,
   DEBUG_E_LEVEL_DEBUG,
}DEBUG_E_LEVEL;
//------------------------------- ��Ŀ���� -----------------------------------
extern void BspRtc_DebugTestOnOff(uint8_t OnOff);
//
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
#define DEBUG_UART            0
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "uctsk_RFMS.h"
#include "Bsp_ExtFlash.h"
#include "Module_SdFat.h"
#include "uctsk_GprsNet.h"
#include "Bsp_Key.h"
#include "LCD_ILI9341.h"
#include "bsp_tft_lcd.h"
#include "uctsk_GasModule.h"
#include "uctsk_AD.h"
#include "Bsp_IllumSensor.h"
#include "Bsp_TempHumidSensor.h"
#include "uctsk_BluetoothDTU.h"
#include "Bsp_PwmOut.h"
#include "Bsp_Uart.h"
//<----
#elif (defined(PROJECT_SPI_SLAVE))
//---->
#define DEBUG_UART            0
#define DEBUG_RTT_ENABLE      0
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "uctsk_SpiMaster.h"
//<----
#elif (defined(PROJECT_ARMFLY_V5_XSL))
//---->
#define DEBUG_UART            1
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_Key.h"
#include "Bsp_ExtFlash.h"
#include "Bsp_WM8978.h"
#include "Bsp_Ds18b20.h"
#include "Module_DList.h"
#include "Module_RBTree.h"
#include "Module_SdFat.h"
#include "Module_GameBucket.h"
#include "Bsp_FsmcSram.h"
#include "Bsp_NorFlash.h"
//<----
#elif (defined(XKAP_ICARE_A_M))
//---->
#define DEBUG_UART            0
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_NrfRfEsb.h"
#include "uctsk_GprsNet.h"
#include "Bsp_CpuId.h"
#include "Bsp_ExtFlash.h"
#include "Bsp_Key.h"
#define  Debug_Rx_ExFun       GprsNet_DebugRx
//<----
#elif (defined(XKAP_ICARE_A_S))
//---->
#define DEBUG_UART            1
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_NrfRfEsb.h"
//<----
#elif (defined(XKAP_ICARE_B_M))
//---->
#define DEBUG_UART            0
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_DEBUG
#include "uctsk_GprsNet.h"
#include "Bsp_CpuId.h"
#include "Bsp_ExtFlash.h"
#include "Bsp_Key.h"
#include "Bsp_Hx711.h"
#include "Bsp_BuzzerMusic.h"
#include "Bsp_CpuFlash.h"
#include "Module_Memory.h"
#include "Bsp_Led.h"
#include "Bsp_Ap3216c.h"
#if   (defined(HAIER))
#include "Bsp_NrfBle.h"
#else
#include "Bsp_NrfRfEsb.h"
#endif
#define  Debug_Rx_ExFun       GprsNet_DebugRx
//<----
#elif (defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
//---->
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_Key.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Twi.h"
#include "Bsp_Bmp180.h"
#include "Bsp_Ap3216c.h"
#include "Bsp_Mpu6050.h"
#include "Bsp_Led.h"
#include "Bsp_NrfBle.h"
//<----
#elif (defined(XKAP_ICARE_B_D))
//---->
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_CpuFlash.h"
#include "Bsp_Led.h"
#include "Bsp_NrfBle.h"
//<----
#elif (defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4)||defined(BASE_NRF51)||defined(BASE_NRF52))
//---->
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
//<----
#elif (defined(PROJECT_BIB_TEST1))
//---->
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
#include "Bsp_Key.h"
#include "Pulse_ADC.h"
//<----
#elif (defined(PROJECT_TCI_V30))
//---->
#define DEBUG_RTT_ENABLE      1
#define DEBUG_LOG_LEVEL       DEBUG_E_LEVEL_INFO
//#include "Bsp_Key.h"
//#include "Pulse_ADC.h"
#include "Bsp_Relay.h"
#include "Bsp_NXP74HC165.h"
//<----

#else
//---->
#error Please Set Project to uctsk_Debug.h
//<----
#endif

typedef struct
{
    int8_t CmdCode;                    //ָ����
    int8_t *pNameStr;                  //�����ַ���
    void (*TestPro)(uint8_t OnOrOff);  //�����ص�����(����һ������,ͨ����������)
    int8_t RunCount;                   //0-��������   1��������
} Debug_S_Menu;
Debug_S_Menu const Debug_MenuTab[]=
{
    {
        '0',
        "0,Test-OS-Info\r\n",
        (*Module_OS_Info_DebugTestOnOff),
        0
    },
    //ϵͳʱ��
    {
        '1',
        "1,Set-RealTime\r\n",
        (*BspRtc_DebugTestOnOff),
        1
    },
    //�㷨����
    {
        '2',
        "2,Test-Count\r\n",
        (*Count_DebugTestOnOff),
        1
    },
    //����
    {
        '3',
        "3,Test-Restart\r\n",
        (*Bsp_Pwr_Reset_DebugTestOnOff),
        1
    },
    //����
    {
        '4',
        "4,Test-Read Global Para\r\n",
        (*Module_Memory_DebugTestReadParaOnOff),
        1
    },
    {
        '5',
        "5,Test-Factory\n",
        (*Module_Memory_DebugTestFactoryOnOff),
        1
    },
#if   (defined(PROJECT_XKAP_V3))
    {
        'a',
        "a,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Ad\r\n",
        (*AD_DebugTestOnOff),
        0
    },
    {
        'c',
        "c,Test TF-File Sys\r\n",
        (*SdFat_Test),
        1
    },
    {
        'e',
        "e,Test-IllumSensor\r\n",
        (*BspIllumSensor_DebugTestOnOff),
        0
    },
    {
        'f',
        "f,Test-TempHumidSensor\r\n",
        (*BspTempHumidSensor_DebugTestOnOff),
        0
    },
    {
        'g',
        "g,Test-Bluetooth\r\n",
        (*Bluetooth_DebugTestOnOff),
        0
    },
    //��ȡҺ���Ĵ���
    {
        'h',
        "h,Read-LcdReg\r\n",
        (*LCD_ILI9341_DebugTestOnOff),
        1
    },
    //ϵͳʱ��
    {
        'i',
        "i,Set-RealTime\r\n",
        (*BspRtc_DebugTestOnOff),
        1
    },
    //PWM
    {
        'j',
        "j,Test-PwmOut\r\n",
        (*BspPwmOut_DebugTestOnOff),
        0
    },
    //���Դ���
    {
        'k',
        "k,Test-Uart3\r\n",
        (*BspUart_DebugTestOnOff),
        0
    },
    //���RFMS����
    {
        'l',
        "l,Test-Monitor RFMS Data Stream\r\n",
        (*uctsk_Rfms_DataStream_DebugTestOnOff),
        0
    },
    //���RFMS����
    {
        'm',
        "m,Test-Monitor RFMS Wave Stream\r\n",
        (*uctsk_Rfms_WaveStream_DebugTestOnOff),
        0
    },
    //�����������
    {
        'n',
        "n,Test-Monitor RFMS Final Data\r\n",
        (*uctsk_Rfms_FinalData_DebugTestOnOff),
        1
    },
    //ExtFlash
    {
        'o',
        "o,Test-ExtFlash\r\n",
        (*Bsp_ExtFlash_DebugTestOnOff),
        1
    },
    //��ȡҺ���Ĵ���
    {
        'p',
        "p,LcdErrReset\r\n",
        (*LCD_ErrReset_DebugTestOnOff),
        0
    },
    //��ȡҺ���Ĵ���
    /*
    {
        'q',
        "q,GasModule Read\r\n",
        (*GasModule_DebugTestOnOff),
        0
    },
    */
    //RFMS�����ļ�����
    {
        'r',
        "r,Rfms-Run File Data\r\n",
        (*uctsk_Rfms_RunFileData_DebugTestOnOff),
        1
    },
    //GprsNet�����ļ�����
    {
        's',
        "s,Test-GprsNet\r\n",
        (*GprsNet_DebugTestOnOff),
        0
    },
    //
    {
        't',
        "t,Test-Memory\r\n",
        (*Module_Memory_DebugTestOnOff),
        1
    },
#elif  (defined(XKAP_ICARE_B_D_M))  
    {
        'a',
        "a,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Ad\r\n",
        (*AD_DebugTestOnOff),
        0
    },
    {
        'c',
        "c,Test TF-File Sys\r\n",
        (*SdFat_Test),
        1
    },
    {
        'f',
        "f,Test-TempHumidSensor\r\n",
        (*BspTempHumidSensor_DebugTestOnOff),
        0
    },
    {
        'g',
        "g,Test-Bluetooth\r\n",
        (*Bluetooth_DebugTestOnOff),
        0
    },
    //ϵͳʱ��
    {
        'i',
        "i,Set-RealTime\r\n",
        (*BspRtc_DebugTestOnOff),
        1
    },
    //PWM
    {
        'j',
        "j,Test-PwmOut\r\n",
        (*BspPwmOut_DebugTestOnOff),
        0
    },
    //���Դ���
    {
        'k',
        "k,Test-Uart3\r\n",
        (*BspUart_DebugTestOnOff),
        0
    },
    //ExtFlash
    {
        'o',
        "o,Test-ExtFlash\r\n",
        (*Bsp_ExtFlash_DebugTestOnOff),
        1
    },
    //
    {
        't',
        "t,Test-Memory\r\n",
        (*Module_Memory_DebugTestOnOff),
        1
    },
#elif  (defined(PROJECT_SPI_SLAVE))
    {
        'a',
        "a,Test-SpiMS\r\n",
        (*SpiMaster_DebugTestOnOff),
        0
    },
#elif  (defined(PROJECT_BIB_TEST1))
    {
        'a',
        "a,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Pulse_ADC\r\n",
        (*PulseADC_DebugTestOnOff),
        0
    },
#elif  (defined(PROJECT_ARMFLY_V5_XSL))
    {
        'a',
        "a,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-WM8978 Record\r\n",
        (*BspWM8978_Record_DebugTestOnOff),
        0
    },
    {
        'c',
        "c,Test-WM8978 TfPlayer\r\n",
        (*BspWM8978_TfPlayer_DebugTestOnOff),
        1
    },
    {
        'd',
        "d,Test TF-File Sys\r\n",
        (*SdFat_Test),
        1
    },
    {
        'e',
        "e,Test-ModuleSList\r\n",
        (*ModuleSList_DebugTestOnOff),
        1
    },
    {
        'f',
        "f,Test-ModuleRBTree\r\n",
        (*ModuleRBTree_DebugTestOnOff),
        1
    },
    //ExtFlash
    {
        'g',
        "g,Test-ExtFlash\r\n",
        (*Bsp_ExtFlash_DebugTestOnOff),
        1
    },
    //����DS18B20
    {
        'h',
        "h,Test-DS18B20\r\n",
        (*BspDs18b20_DebugTestOnOff),
        0
    },
    //����SRAM
    {
        'i',
        "i,Test-SRAM\r\n",
        (*BspFsmcSram_DebugTestOnOff),
        1
    },
    //����NorFlash
    {
        'j',
        "j,Test-Nor Flash\r\n",
        (*BspNorFlash_DebugTestOnOff),
        1
    },
    //��Ϸ---ˮͰ
    {
        't',
        "t,Game-Bucket\r\n",
        (*ModuleGameBucket_DebugTestOnOff),
        1
    },
#elif (defined(PROJECT_TCI_V30))
    //Relay
    {
        'a',
        "a,Test-Relay\r\n",
        (*BspRelay_DebugTestOnOff),
        0
    },
    //NXP74HC165
    {
        'b',
        "b,Test-NXP74HC165\r\n",
        (*BspNXP74HC165_DebugTestOnOff),
        0
    },
#elif (defined(XKAP_ICARE_A_M))
    {
        'a',
        "a,Test-Gsm Print\r\n",
        (*GprsNet_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Gsm Control\r\n",
        (*GprsNet_DebugControlOnOff),
        0
    },
    {
        'c',
        "c,Cpu ID\r\n",
        (*Bsp_CpuId_DebugTestOnOff),
        1
    },
    {
        'd',
        "d,Test-ExtFlash\r\n",
        (*Bsp_ExtFlash_DebugTestOnOff),
        1
    },
    {
        'e',
        "e,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
#elif (defined(XKAP_ICARE_A_S))
    {
        'c',
        "c,Test-RfmsWave\r\n",
        (*Bsp_NrfRfEsb_DebugTestWaveOnOff),
        0
    },
#elif (defined(XKAP_ICARE_B_M))
    {
        'a',
        "a,Test-Gsm Print\r\n",
        (*GprsNet_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Gsm Control\r\n",
        (*GprsNet_DebugControlOnOff),
        0
    },
    {
        'c',
        "c,Cpu ID\r\n",
        (*Bsp_CpuId_DebugTestOnOff),
        1
    },
    {
        'd',
        "d,Test-ExtFlash\r\n",
        (*Bsp_ExtFlash_DebugTestOnOff),
        1
    },
    {
        'e',
        "e,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'f',
        "f,Test-Hx711\r\n",
        (*BspHx711_DebugTestOnOff),
        0
    },
    {
        'g',
        "g,Test-BuzzerMusic\r\n",
        (*BspBuzzerMusic_DebugTestOnOff),
        1
    },
#if   (defined(HAIER))
    {
        'h',
        "h,Test-NrfBle\r\n",
        (*BspNrfBle_DebugTestOnOff),
        0
    },
#else
    {
        'h',
        "h,Test-Rfr\n",
        (*Bsp_NrfRfEsb_DebugTestOnOff),
        0
    },
#endif
    {
        'i',
        "i,Test-CpuFlash\n",
        (*Bsp_CpuFlash_DebugTestOnOff),
        1
    },
    {
        'j',
        "j,Test-Memory\n",
        (*Module_Memory_DebugTestOnOff),
        0
    },
    {
        'k',
        "k,Test-Ap3216c\r\n",
        (*BspAp3216c_DebugTestOnOff),
        0
    },
    {
        'l',
        "l,Test-Led\r\n",
        (*BspLed_DebugTestOnOff),
        0
    },
    {
        'm',
        "m,Test-Cpu Temp\r\n",
        (*Bsp_CpuId_ReadTemp_DebugTestOnOff),
        0
    },  
#elif (defined(XKAP_ICARE_B_C)||defined(PROJECT_NRF5X_BLE))
    {
        'a',
        "a,Test-Key\r\n",
        (*BspKey_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-Led\r\n",
        (*BspLed_DebugTestOnOff),
        0
    },
    {
        'c',
        "c,Test-CpuFlash\n",
        (*Bsp_CpuFlash_DebugTestOnOff),
        1
    },
    {
        'd',
        "d,Test-Twi Device Search\r\n",
        (*BspTwi_DebugTestOnOff),
        1
    },
    {
        'e',
        "e,Test-Bmp180\r\n",
        (*BspBmp180_DebugTestOnOff),
        0
    },
    {
        'f',
        "f,Test-Ap3216c\r\n",
        (*BspAp3216c_DebugTestOnOff),
        0
    },
    {
        'g',
        "g,Test-Mpu6050\r\n",
        (*BspMpu6050_DebugTestOnOff),
        0
    },
    {
        'h',
        "h,Test-NrfBle\r\n",
        (*BspNrfBle_DebugTestOnOff),
        0
    },
#elif (defined(XKAP_ICARE_B_D))
		{
        'a',
        "a,Test-Led\r\n",
        (*BspLed_DebugTestOnOff),
        0
    },
    {
        'b',
        "b,Test-CpuFlash\n",
        (*Bsp_CpuFlash_DebugTestOnOff),
        1
    },
		{
        'c',
        "c,Test-NrfBle\r\n",
        (*BspNrfBle_DebugTestOnOff),
        0
    },
#endif
};
#else
#ifndef DEBUG_EXT
#define DEBUG_EXT extern
typedef enum DEBUG_E_LEVEL
{
   DEBUG_E_LEVEL_ALWAYS=0,
   DEBUG_E_LEVEL_ERROR,
   DEBUG_E_LEVEL_WARNING,
   DEBUG_E_LEVEL_INFO,
   DEBUG_E_LEVEL_DEBUG,
}DEBUG_E_LEVEL;
#endif
#endif
//-------------------�ӿں궨��(Ӳ���޹�)--------------------
// ����ָ��
#define  DEBUG_HELP_CMD       '$'
// �˳���ʱʱ��궨��
#define DEBUG_QUIT_OVERTIME_S 10
//-------------------�ӿں���--------------------------------
// ��ֲ---���񴴽�
DEBUG_EXT void  App_DebugTaskCreate (void);
// ��ֲ---�жϽӿ�
DEBUG_EXT void Debug_InterruptRx(uint8_t *d,uint16_t len);
// Ӧ��---Debug����
DEBUG_EXT void DebugIn(int8_t* str,uint16_t* Comlength);
DEBUG_EXT void DebugInClear(void);
DEBUG_EXT uint8_t DebugInputNum(uint16_t *Number,uint8_t MaxDight);
// Ӧ��---Debug���(����LEVEL����,������LOGȡ��)
DEBUG_EXT void DebugOut(int8_t* str,uint16_t Comlength);
DEBUG_EXT void DebugOutStr(int8_t* str);
DEBUG_EXT void DebugOutHex(char *pname,uint8_t*pbuf,int16_t len);
// Ӧ��---LOG���(��LEVEL����)
DEBUG_EXT void DebugLogOut(DEBUG_E_LEVEL level,int8_t* str,uint16_t Comlength);
DEBUG_EXT void DebugLogOutStr(DEBUG_E_LEVEL level,int8_t* str);
DEBUG_EXT void DebugLogOutHex(DEBUG_E_LEVEL level,char *pname,uint8_t*pbuf,int16_t len);
//-----------------------------------------------------------

