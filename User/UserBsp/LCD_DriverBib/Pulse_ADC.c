#include "includes.h"
#include "Pulse_ADC.h"
#include "MemManager.h"
#include "uctsk_Debug.h"

static uint8_t PulseADC_DebugTest_Enable=0;

void MyADC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
	
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//模拟输入模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//ADC通道PA0,PA1
	
	ADC_DeInit(ADC1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC时钟设置为PCLK2/6=12MHz
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; //关闭扫描，使用单通道模式
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //单次转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发转换
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1; //顺序进行规则转换的ADC通道数目为1
  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC1
	ADC_Cmd(ADC1, ENABLE); //使能ADC1
  ADC_ResetCalibration(ADC1); //复位校准
  while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
  ADC_StartCalibration(ADC1); //开启AD校准
  while(ADC_GetCalibrationStatus(ADC1)); //等待校准结束
}

u16 GetADC(void)
{
	ADC_RegularChannelConfig(ADC1, 9, 1, ADC_SampleTime_239Cycles5);//ADC1，通道9(PB1)，规则采样顺序值为1，转换周期为239.5个周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //软件启动转换
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//等待转换完成
  return ADC_GetConversionValue(ADC1);//返回AD转换值
	//ADC_ClearITPendingBit(ADC1, ADC_FLAG_EOC);
}

u16 GetADC_PULSE(void) //采心率的ADC值，端口PA0
{
	ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_239Cycles5);//ADC1，通道0(PA0)，规则采样顺序值为1，转换周期为239.5个周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //软件启动转换
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//等待转换完成
  return ADC_GetConversionValue(ADC1);//返回AD转换值
	//ADC_ClearITPendingBit(ADC1, ADC_FLAG_EOC);
}
u16 GetADC_BAT(void)  //采电池的ADC值，端口PA1
{
	ADC_RegularChannelConfig(ADC1, 1, 1, ADC_SampleTime_239Cycles5);//ADC1，通道1(PA1)，规则采样顺序值为1，转换周期为239.5个周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //软件启动转换
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//等待转换完成
  return ADC_GetConversionValue(ADC1);//返回AD转换值
	//ADC_ClearITPendingBit(ADC1, ADC_FLAG_EOC);
}
void PulseADC_100ms(void)
{
	u16 i16,j16;
	static uint8_t si=0;
  si++;
  if(si!=10)
  {
     return;
  }
  si=0;
	if(PulseADC_DebugTest_Enable==1)
	{
		char *pbuf;
		//申请缓存
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
		i16 = GetADC_PULSE();
		j16 = GetADC_BAT();
		sprintf(pbuf,"DebugOut: AD0-%d,AD1=%d\r\n",i16,j16);
    DebugOutStr((int8_t*)pbuf);
		//释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
	}
}
void PulseADC_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        PulseADC_DebugTest_Enable=1;
    }
    else
    {
        PulseADC_DebugTest_Enable=0;
    }
}
