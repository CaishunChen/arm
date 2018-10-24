/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*
关于AD采样速率的计算
1，ADC时钟: RCC_ADCCLKConfig(RCC_PCLK2_Div8)表示AD时钟设为72/8=9MHz
2，ADC总转换时间=采样时间+12.5个ADC时钟周期(信号量转换时间)，而采样时间由寄存器设定，最低1.5ADC周期，最大239.5ADC周期，也就是你程序中设置的55.5个采样周期。所以ADC一次采样的总采样时间是55.5+12.5=68个ADC周期，也就是68/9 us
3,所以，总采样时间最快就是1.5+12.5个ADC周期，最慢就是239.5+12.5个ADC周期。至于采样速率，是跟你的程序是有关的，比如ADC采样时间设定为最快，但你可以设定1s采一次，那采样速率仍是1；再比如你开启扫描模式，跟单次转换又不一样
*/
//================================
#include "uctsk_AD.h"
#include "uctsk_Debug.h"
#if (defined(NRF51)||defined(NRF52))
#include "nrf_drv_adc.h"
#include "nrf_delay.h"
#endif
//================================
//------------------------------- 用户宏定义 ------------------------
//------------------------------- 用户变量 --------------------------
MODULE_OS_TASK_TAB(App_TaskADTCB);
MODULE_OS_TASK_STK(App_TaskADStk,APP_TASK_AD_STK_SIZE);
//--------
#if   (ADC_CHANNEL_NUM!=0)
//转换缓存(DMA)
uint16_t  ADC_ConvertedValue[ADC_NEX][ADC_CHANNEL_NUM]= {0};
//计算平均值
static uint16_t  ADC_Average[ADC_CHANNEL_NUM]= {0};
//采集电压
uint16_t  ADC_mV[ADC_CHANNEL_NUM];
#endif
//温度
uint16_t ADC_Temperature=0;
//参考电压
uint16_t ADC_Vref_mV=0;
//VBAT
uint16_t ADC_Vbat_mV=0;
//
uint8_t AD_DebugTest_Enable=0;
//------------------------------- 函数声明 --------------------------
static void uctsk_AD(void *pvParameters);
#ifdef AD_DMA_ENABLE
static void AD_DebugTest_100ms(void);
#endif
//-------------------------
#if (defined(NRF51)||defined(NRF52))
//
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {
        //uint8_t i;
        //char *buf;
        /*
        buf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        for (i = 0; i < p_event->data.done.size; i++)
        {
            //NRF_LOG_INFO("Current sample value: %d\r\n", p_event->data.done.p_buffer[i]);
            sprintf((char*)buf,"Current sample value: %d\r\n", p_event->data.done.p_buffer[i]);
            DebugOutStr((int8_t*)buf);
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,buf);
        */
        //DebugOutHex("Current sample value: ", (uint8_t *)(p_event->data.done.p_buffer), 2*p_event->data.done.size);
    }
}
#endif
/*******************************************************************************
函数功能: AD配置函数
参    数: 无
返 回 值: 无
*******************************************************************************/
void ADC_Configuration(void)
{
#if (defined(STM32F1)||defined(STM32F4))
    ADC_InitTypeDef ADC_InitStructure;
#ifdef AD_DMA_ENABLE
    uint8_t i;
    DMA_InitTypeDef DMA_InitStructure;
#endif
#if   (ADC_USER_CHANNEL_NUM   != 0)
    GPIO_InitTypeDef GPIO_InitStructure;
#endif
    static uint8_t first=1;
    if(first!=1)
    {
        return;
    }
    first=0;
    memset((char*)&ADC_InitStructure,0,sizeof(ADC_InitTypeDef));
    //时钟
#ifdef AD_DMA_ENABLE
    AD_DMA_RX_RCC_ENABLE;
#endif
    AD_GPIO_RCC_ENABLE;
    AD_RCC_ENABLE;
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //ADC时钟配置
#if   (defined(STM32F1))
    //只能选择2/4/6/8
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
#elif (defined(STM32F4))
    {
        ADC_CommonInitTypeDef ADC_CommonInitStructure;
        // 独立模式
        ADC_CommonInitStructure.ADC_Mode              =  ADC_Mode_Independent;
        // 预分频=8分频(ADCCLK=PCLK2/8=84/8=10.5MHz)
        ADC_CommonInitStructure.ADC_Prescaler         =  ADC_Prescaler_Div8;
        // DMA失能
        ADC_CommonInitStructure.ADC_DMAAccessMode     =  ADC_DMAAccessMode_Disabled;
        // 两个采样阶段之间延迟5个时钟周期
        ADC_CommonInitStructure.ADC_TwoSamplingDelay  =  ADC_TwoSamplingDelay_5Cycles;
        ADC_CommonInit(&ADC_CommonInitStructure);
    }
#endif
    //GPIO
    {
#if   (ADC_USER_CHANNEL_NUM   != 0)
#if   (defined(STM32F1))
        GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AIN;
#elif (defined(STM32F4))
        GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AN;
        GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL ;
#endif
#endif
        //-----配置
#ifdef   ADC_ADC1_CHANNEL_0_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_1_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_2_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_3_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_4_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_5_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_6_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_7_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_8_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_9_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_10_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_11_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_12_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_13_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_14_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
#ifdef   ADC_ADC1_CHANNEL_15_ENABLE
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
    }
    //-----
#ifdef AD_DMA_ENABLE
    {
        //DMA channel1 configuration
        DMA_DeInit(AD_DMA_RX_STREAM);
#if   (defined(STM32F1))
        DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(AD_ADCX->DR));
        DMA_InitStructure.DMA_MemoryBaseAddr     =  (uint32_t)&ADC_ConvertedValue;   //定义DMA内存基地址
        DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralSRC;           //外设作为数据传输的来源
        DMA_InitStructure.DMA_BufferSize         =  ADC_CHANNEL_NUM*ADC_NEX;         //定义指定DMA通道的DMA缓存的大小
        DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;       //外设地址寄存器不变
        DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;            //设置DMA的内存递增模式
        DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_HalfWord; //数据宽度为16位
        DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_HalfWord;     //数据宽度为16位
        DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;               //工作在循环缓存模式
        DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;               //DMA通道1拥有高优先级
        DMA_InitStructure.DMA_M2M                =  DMA_M2M_Disable;                 //DMA通道1没有设置为内存到内存传输
#elif (defined(STM32F4))
        DMA_InitStructure.DMA_Channel            =  AD_DMA_RX_CHANNEL;
        DMA_InitStructure.DMA_PeripheralBaseAddr =  (uint32_t) (&(AD_ADCX->DR));
        DMA_InitStructure.DMA_Memory0BaseAddr    =  (uint32_t)&ADC_ConvertedValue;
        DMA_InitStructure.DMA_DIR                =  DMA_DIR_PeripheralToMemory;
        DMA_InitStructure.DMA_BufferSize         =  ADC_CHANNEL_NUM*ADC_NEX;
        DMA_InitStructure.DMA_PeripheralInc      =  DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc          =  DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize =  DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize     =  DMA_MemoryDataSize_HalfWord;
        DMA_InitStructure.DMA_Mode               =  DMA_Mode_Circular;
        DMA_InitStructure.DMA_Priority           =  DMA_Priority_High;
        DMA_InitStructure.DMA_FIFOMode           =  DMA_FIFOMode_Disable;
        DMA_InitStructure.DMA_FIFOThreshold      =  DMA_FIFOThreshold_HalfFull;
        DMA_InitStructure.DMA_MemoryBurst        =  DMA_MemoryBurst_Single;
        DMA_InitStructure.DMA_PeripheralBurst    =  DMA_PeripheralBurst_Single;
#endif
        DMA_Init(AD_DMA_RX_STREAM, &DMA_InitStructure);
        //Enable DMA1 channel1
        DMA_Cmd(AD_DMA_RX_STREAM, ENABLE);
    }
#endif
    //ADC配置
    {
#if   (defined(STM32F1))
        ADC_DeInit(AD_ADCX);
        ADC_InitStructure.ADC_Mode                  = ADC_Mode_Independent;      //独立工作模式
        ADC_InitStructure.ADC_ScanConvMode          = ENABLE;                    //扫描模式
        ADC_InitStructure.ADC_ContinuousConvMode    = ENABLE;                    //连续转换模式
        ADC_InitStructure.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_None; //软件控制转换
        ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;       //右对齐
        ADC_InitStructure.ADC_NbrOfChannel          = ADC_CHANNEL_NUM;
        ADC_Init(AD_ADCX, &ADC_InitStructure);
#elif (defined(STM32F4))
        ADC_DeInit();
        // 12位AD
        ADC_InitStructure.ADC_Resolution            = ADC_Resolution_12b;
#ifdef AD_DMA_ENABLE
        // 扫描方式
        ADC_InitStructure.ADC_ScanConvMode          = ENABLE;
        // 连续转换
        ADC_InitStructure.ADC_ContinuousConvMode    = ENABLE;
        // 转换序列个数
        ADC_InitStructure.ADC_NbrOfConversion       = ADC_CHANNEL_NUM;
#else
        // 非扫描方式
        ADC_InitStructure.ADC_ScanConvMode          = DISABLE;
        // 非连续转换
        ADC_InitStructure.ADC_ContinuousConvMode    = DISABLE;
        // 转换序列个数
        ADC_InitStructure.ADC_NbrOfConversion       = 1;
#endif
        // 禁止触发检测,使用软件触发
        ADC_InitStructure.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_None;
        //ADC_InitStructure.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_T1_CC1;
        // 右对齐
        ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;
        ADC_Init(AD_ADCX, &ADC_InitStructure);
#endif
    }
#ifdef AD_DMA_ENABLE
    {
        //-----配置
        i=1;
#if   (defined(STM32F1))
        //ADC1通道配置(1.5/7.5/13.5/28.5/41.5/55.5/71.5/239.5)
#ifdef   ADC_ADC1_CHANNEL_0_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_1_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_1, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_2_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_3_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_4_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_4, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_5_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_5, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_6_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_6, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_7_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_7, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_8_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_8, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_9_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_9, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_10_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_10, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_11_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_11, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_12_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_12, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_13_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_13, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_14_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_14, i++,  ADC_SampleTime_239Cycles5);
#endif
#ifdef   ADC_ADC1_CHANNEL_15_ENABLE
        ADC_RegularChannelConfig(ADC1, ADC_Channel_15, i++,  ADC_SampleTime_239Cycles5);
#endif
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_TempSensor,  i++, ADC_SampleTime_239Cycles5);
        //ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vrefint,     3, ADC_SampleTime_239Cycles5);
#elif (defined(STM32F4))
#ifdef   ADC_ADC1_CHANNEL_0_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_0,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_1_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_1,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_2_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_2,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_3_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_3,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_4_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_4,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_5_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_5,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_6_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_6,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_7_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_7,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_8_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_8,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_9_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_9,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_10_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_10,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_11_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_11,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_12_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_12,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_13_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_13,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_14_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_14,   i++, ADC_SampleTime_480Cycles);
#endif
#ifdef   ADC_ADC1_CHANNEL_15_ENABLE
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_15,   i++, ADC_SampleTime_480Cycles);
#endif
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_TempSensor,  i++, ADC_SampleTime_480Cycles);
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vrefint,     i++, ADC_SampleTime_480Cycles);
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vbat,        i++, ADC_SampleTime_480Cycles);
#endif
        //-----
#if   (defined(STM32F1))
        //使能温度转换
        ADC_TempSensorVrefintCmd(ENABLE);
        //使能 ADC1 DMA
        ADC_DMACmd(AD_ADCX, ENABLE);
        //使能 ADC1
        ADC_Cmd(AD_ADCX, ENABLE);
        //复位 ADC1 校准寄存器
        ADC_ResetCalibration(AD_ADCX);
        while(ADC_GetResetCalibrationStatus(AD_ADCX));
        //启动 ADC1 校准
        ADC_StartCalibration(AD_ADCX);
        while(ADC_GetCalibrationStatus(AD_ADCX));
        //使能 ADC1 软件转换
        ADC_SoftwareStartConvCmd(AD_ADCX, ENABLE);
#elif (defined(STM32F4))
        //使能温度与参考电压转换
        ADC_TempSensorVrefintCmd(ENABLE);
        //使能Vbat
        //ADC_VBATCmd(ENABLE);
        //
        ADC_DMARequestAfterLastTransferCmd(AD_ADCX, ENABLE);
        //使能 ADC1 DMA
        ADC_DMACmd(AD_ADCX, ENABLE);
        //使能 ADC1
        ADC_Cmd(AD_ADCX, ENABLE);
        //---
        //使能 ADC1 软件转换
        ADC_SoftwareStartConv(AD_ADCX);
#endif
    }
#else
    {
        uint16_t i16;
        //使能温度与参考电压转换
        ADC_TempSensorVrefintCmd(ENABLE);
        //使能 ADC1
        ADC_Cmd(AD_ADCX, ENABLE);
        //
        // 读取参考电压
        //while(ADC_Vref_mV<2000)
        {
            ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vrefint,1, ADC_SampleTime_480Cycles);
            ADC_SoftwareStartConv(AD_ADCX);
            while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
            i16 =  ADC_GetConversionValue(ADC1);
            ADC_Vref_mV=(4096*1210L)/i16;
        }
        // 读取电池电压
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_0,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        ADC_mV[AD_E_CHANNEL_BAT] =(i16*ADC_Vref_mV)/AD_RESOLUTION;
#endif
        // 读取电源电压
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_1,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        ADC_mV[AD_E_CHANNEL_AC] =(i16*ADC_Vref_mV)/AD_RESOLUTION;
#endif
        // 读取RTC电池电压
        ADC_VBATCmd(ENABLE);
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vbat,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
        ADC_VBATCmd(DISABLE);
        ADC_Vbat_mV=i16*2*ADC_Vref_mV/0xFFF;
        //
        ADC_Cmd(AD_ADCX, DISABLE);
        ADC_TempSensorVrefintCmd(DISABLE);
        AD_RCC_DISABLE;
    }
#endif
#elif (defined(XKAP_ICARE_A_S))
    static nrf_drv_adc_channel_t  m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_5);
    ret_code_t ret_code;
    //配置优先级
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
    //配置中断函数
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);
    //通道使能
    m_channel_config.config.config.input=ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;
    //m_channel_config.config.config.input=ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling;
    //m_channel_config.config.config.reference  = ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling;
    m_channel_config.config.config.reference  = ADC_CONFIG_REFSEL_VBG;
    nrf_drv_adc_channel_enable(&m_channel_config);
#elif (defined(XKAP_ICARE_B_M))
#if   (HARDWARE_SUB_VER==1)
    static nrf_drv_adc_channel_t  m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_7);
#elif (HARDWARE_SUB_VER==2)
    static nrf_drv_adc_channel_t  m_channel_config[5];
#endif
    ret_code_t ret_code;
    uint8_t i;
    //配置优先级
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;

    for(i=0; i<5; i++)
    {
        //通道使能
        m_channel_config[i].config.config.resolution    =   NRF_ADC_CONFIG_RES_10BIT;
        m_channel_config[i].config.config.input     =   ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling;
        m_channel_config[i].config.config.reference     =   ADC_CONFIG_REFSEL_VBG;
        if(i==0)
        {
            m_channel_config[i].config.config.ain   =   NRF_ADC_CONFIG_INPUT_2;
        }
        else if(i==1)
        {
            m_channel_config[i].config.config.ain   =   NRF_ADC_CONFIG_INPUT_3;
        }
        else if(i==2)
        {
            m_channel_config[i].config.config.ain   =   NRF_ADC_CONFIG_INPUT_4;
        }
        else if(i==3)
        {
            m_channel_config[i].config.config.ain   =   NRF_ADC_CONFIG_INPUT_5;
        }
        else if(i==4)
        {
            m_channel_config[i].config.config.ain   =   NRF_ADC_CONFIG_INPUT_6;
        }
        //通道使能
        nrf_drv_adc_channel_enable(&m_channel_config[i]);
    }
    //配置中断函数
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);
#endif
    AD_DebugTest_Enable=AD_DebugTest_Enable;
}
/*******************************************************************************
函数功能: AD任务创建
*******************************************************************************/
void  App_ADTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-AD",\
                          uctsk_AD,\
                          APP_TASK_AD_PRIO,\
                          App_TaskADStk,\
                          APP_TASK_AD_STK_SIZE,\
                          App_TaskADTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: AD任务实体
*******************************************************************************/
static void uctsk_AD (void *pvParameters)
{
#if (defined(STM32F1)||defined(STM32F4))
#ifdef AD_DMA_ENABLE
    uint8_t channel;
    //INT8U state;
    uint8_t num;
    uint32_t sum;
    uint16_t min,max;
    //MODULE_OS_ERR err;
    //INT16U theoryMvBuf[10];
    //INT16U realMv;
    //AD初始化
    ADC_Configuration();
    //变量初始化
    for(;;)
    {
        MODULE_OS_DELAY_MS(100);
        //-----------------------
        //获取每个通道平均值
        for(channel=0; channel<ADC_CHANNEL_NUM; channel++)
        {
            //求和(去最大值,最小值)
            sum=0;
            min=AD_RESOLUTION;
            max=0;
            for(num=0; num<ADC_NEX; num++)
            {
                sum+=ADC_ConvertedValue[num][channel];
                if(min>ADC_ConvertedValue[num][channel])
                {
                    min=ADC_ConvertedValue[num][channel];
                }
                if(max<ADC_ConvertedValue[num][channel])
                {
                    max=ADC_ConvertedValue[num][channel];
                }
            }
            sum=sum-min-max;
            //求平均采集
            ADC_Average[channel]=sum/(ADC_NEX-2);
            //求平均电压
            ADC_mV[channel]=(ADC_Average[channel]*AD_REFERENCE_VOLTAGE_MV)/AD_RESOLUTION;
        }
        //计算温度
#if   (defined(STM32F1))
        //换算温度
        ADC_Temperature= (1.42 - ADC_Average[ADC_CHANNEL_NUM-1]*3.3/4096)*1000/4.35 + 25;
        //换算参考电压
        ADC_Vref_mV=0;
#elif (defined(STM32F4))
        //换算温度
        {
            float f_temp;
            f_temp= ((ADC_Average[ADC_CHANNEL_NUM-3]*((float)ADC_Vref_mV)/4095000)-0.76f)/0.0025f + 25;
            ADC_Temperature=f_temp;
        }
        //换算参考电压
        ADC_Vref_mV=(4096*1210)/ADC_Average[ADC_CHANNEL_NUM-2];
        //换算电池电压
        ADC_Vbat_mV=ADC_Average[ADC_CHANNEL_NUM-1]*2*ADC_Vref_mV/0xFFF;
#endif
        //波形输出
        //AD_DebugTest_WaveOut();
        //调试输出
        AD_DebugTest_100ms();
    }
#else
    uint16_t i16;
    uint16_t buf[3]= {0};
    ADC_Average[0]=ADC_Average[0];
    AD_DebugTest_Enable=AD_DebugTest_Enable;
    ADC_Configuration();
    for(;;)
    {
        AD_RCC_ENABLE;
        ADC_TempSensorVrefintCmd(ENABLE);
        ADC_Cmd(AD_ADCX, ENABLE);
        // 读取参考电压
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_Vrefint,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
        //ADC_Vref_mV=(4096*1210)/i16;
        i16 = (4096*1210)/i16;
        if(abs(i16-buf[0])<100)
        {
            ADC_Vref_mV  =  i16;
        }
        buf[0]=i16;
#if (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        // 读取电池电压
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_0,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
        //ADC_mV[AD_E_CHANNEL_BAT] =(i16*ADC_Vref_mV)/AD_RESOLUTION;
        i16 = (i16*ADC_Vref_mV)/AD_RESOLUTION;;
        if(abs(i16-buf[1])<100)
        {
            ADC_mV[AD_E_CHANNEL_BAT]  =  i16;
        }
        buf[1]=i16;
        // 读取电源电压
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_1,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
        //ADC_mV[AD_E_CHANNEL_AC] =(i16*ADC_Vref_mV)/AD_RESOLUTION;
        i16 = (i16*ADC_Vref_mV)/AD_RESOLUTION;
        if(abs(i16-buf[2])<100)
        {
            ADC_mV[AD_E_CHANNEL_AC]  =  i16;
        }
        buf[2]=i16;
#endif
        // 读取温度
        ADC_RegularChannelConfig(AD_ADCX, ADC_Channel_TempSensor,1, ADC_SampleTime_480Cycles);
        ADC_SoftwareStartConv(AD_ADCX);
        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
        i16 =  ADC_GetConversionValue(ADC1);
        {
            float f_temp;
            f_temp= ((i16*((float)ADC_Vref_mV)/4095000)-0.76f)/0.0025f + 25;
            ADC_Temperature=f_temp;
        }
        //
        ADC_Cmd(AD_ADCX, DISABLE);
        ADC_TempSensorVrefintCmd(DISABLE);
        AD_RCC_DISABLE;
        //
        MODULE_OS_DELAY_MS(1000);
    }
#endif
#elif (defined(NRF51) || defined(NRF52))
    ADC_Configuration();
    for(;;)
    {
        MODULE_OS_DELAY_MS(10000);
    }
#endif
}

uint8_t AD_ReadValue(uint8_t OnOff,uint8_t ch,uint16_t *pValue)
{
#if (defined(XKAP_ICARE_A_S))
    static uint8_t si=1;
    nrf_adc_value_t adc_buffer;
    ch=ch;
    pValue=pValue;
    si++;
    if(si<10)
    {
        return OK;
    }
    si=0;
    Bsp_NrfRfEsb_AD(adc_buffer);
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(&adc_buffer,1));
    //for(i=0; i<ADC_BUFFER_SIZE; i++)
    {
        nrf_drv_adc_sample();
    }
    return OK;
#elif (defined(XKAP_ICARE_B_M))
    static nrf_adc_value_t i16_buf[5]= {0};
    uint8_t i=0;
    if(pValue!=NULL)
    {
        if(ch==0)
        {
			memcpy((char*)pValue,(uint16_t*)i16_buf,10);
        }
        else
        {
            *pValue =   i16_buf[ch-1];
        }
    }
    if(OnOff==ON)
    {
        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(i16_buf,5));
        for(i=0; i<5; i++)
        {
            nrf_drv_adc_sample();
        }
    }
    //-----
    return OK;
#else
    ch=ch;
    pValue=pValue;
    return OK;
#endif
}
/*******************************************************************************
函数功能: 100ms
*******************************************************************************/
#ifdef AD_DMA_ENABLE
static void AD_DebugTest_100ms(void)
{
    int8_t *pbuf;
#if   (ADC_USER_CHANNEL_NUM   != 0)
    uint8_t i;
    uint16_t i16;
#endif
    static uint8_t s_count=0;
    if(AD_DebugTest_Enable==1)
    {
        s_count++;
        if(s_count<10)
        {
            return;
        }
        s_count=0;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if   (ADC_USER_CHANNEL_NUM   != 0)
        //打印采集电压
        for(i=0; i<ADC_USER_CHANNEL_NUM; i++)
        {
            i16 = (ADC_Average[i]*AD_REFERENCE_VOLTAGE_MV)/AD_RESOLUTION;
            sprintf((char *)pbuf,"AD-%02d: %05d %05dmV\r\n",i+1,ADC_Average[i],i16);
            DebugOutStr(pbuf);
        }
#endif
        //打印芯片温度
        sprintf((char *)pbuf,"CpuTemp: %d\r\n",ADC_Temperature);
        DebugOutStr(pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
#endif
/*******************************************************************************
* 函数功能: 读取RTC电池电压
* 说    明: 由于怕影响电池的使用寿命,故采用瞬时采集模式
************
函数功能:   电压采集
参    考:   根据nordic手册,针对CR2032纽扣电池,信息如下:
            Section 1: 3.00V - 2.90V = 100%  -  42%   (58% drop on 100 mV)
            Section 2: 2.90V - 2.74V = 42%   -  18%   (24% drop on 160 mV)
            Section 3: 2.74V - 2.44V = 18%   -  6%    (12% drop on 300 mV)
            Section 4: 2.44V - 2.10V = 6%    -  0%    (6% drop on 340 mV)
            mV-->%的函数
            static __INLINE uint8_t battery_level_in_percent(const uint16_t mvolts)
*******************************************************************************/
uint16_t AD_VBAT_Read_mv(void)
{
#if   (defined(AD_DMA_ENABLE))
#if   (defined(STM32F1))
    ADC_Vbat_mV   =  0;
#elif (defined(STM32F4))
    ADC_VBATCmd(ENABLE);
    MODULE_OS_DELAY_MS(100);
    ADC_VBATCmd(DISABLE);
#endif
    return ADC_Vbat_mV;
#elif (defined(PROJECT_NRF5X_BLE))
    // 初始化配置
    static nrf_drv_adc_channel_t  m_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(ADC_CONFIG_PSEL_Disabled);
    ret_code_t ret_code;
    nrf_adc_value_t i16;
    // ---配置优先级
    static nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
    // ---配置中断函数
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);
    // ---通道使能
    m_channel_config.config.config.input     =  ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling;
    m_channel_config.config.config.reference =  ADC_CONFIG_REFSEL_VBG;
    nrf_drv_adc_channel_enable(&m_channel_config);
    // 读取值
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(&i16,1));
    nrf_drv_adc_sample();
    MODULE_OS_DELAY_MS(1);
    // 数值转换
    ADC_Vbat_mV = (i16*3600)/1024;
    // 解除配置
    nrf_drv_adc_channel_disable(&m_channel_config);
    nrf_drv_adc_uninit();
    return ADC_Vbat_mV;
#else
    return ADC_Vbat_mV;
#endif
}
/*******************************************************************************
函数功能: AD-Debug开关测试接口函数
*******************************************************************************/
void AD_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        AD_DebugTest_Enable=1;
    }
    else
    {
        AD_DebugTest_Enable=0;
    }
}
//---------------------END-------------------------------------------------------

