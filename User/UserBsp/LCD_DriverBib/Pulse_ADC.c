#include "includes.h"
#include "Pulse_ADC.h"
#include "MemManager.h"
#include "uctsk_Debug.h"

static uint8_t PulseADC_DebugTest_Enable=0;

void MyADC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
	
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//ģ������ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//ADCͨ��PA0,PA1
	
	ADC_DeInit(ADC1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADCʱ������ΪPCLK2/6=12MHz
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //����ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE; //�ر�ɨ�裬ʹ�õ�ͨ��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������ת��
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1; //˳����й���ת����ADCͨ����ĿΪ1
  ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC1
	ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1
  ADC_ResetCalibration(ADC1); //��λУ׼
  while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
  ADC_StartCalibration(ADC1); //����ADУ׼
  while(ADC_GetCalibrationStatus(ADC1)); //�ȴ�У׼����
}

u16 GetADC(void)
{
	ADC_RegularChannelConfig(ADC1, 9, 1, ADC_SampleTime_239Cycles5);//ADC1��ͨ��9(PB1)���������˳��ֵΪ1��ת������Ϊ239.5������
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //�������ת��
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//�ȴ�ת�����
  return ADC_GetConversionValue(ADC1);//����ADת��ֵ
	//ADC_ClearITPendingBit(ADC1, ADC_FLAG_EOC);
}

u16 GetADC_PULSE(void) //�����ʵ�ADCֵ���˿�PA0
{
	ADC_RegularChannelConfig(ADC1, 0, 1, ADC_SampleTime_239Cycles5);//ADC1��ͨ��0(PA0)���������˳��ֵΪ1��ת������Ϊ239.5������
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //�������ת��
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//�ȴ�ת�����
  return ADC_GetConversionValue(ADC1);//����ADת��ֵ
	//ADC_ClearITPendingBit(ADC1, ADC_FLAG_EOC);
}
u16 GetADC_BAT(void)  //�ɵ�ص�ADCֵ���˿�PA1
{
	ADC_RegularChannelConfig(ADC1, 1, 1, ADC_SampleTime_239Cycles5);//ADC1��ͨ��1(PA1)���������˳��ֵΪ1��ת������Ϊ239.5������
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //�������ת��
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )==0);//�ȴ�ת�����
  return ADC_GetConversionValue(ADC1);//����ADת��ֵ
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
		//���뻺��
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
		i16 = GetADC_PULSE();
		j16 = GetADC_BAT();
		sprintf(pbuf,"DebugOut: AD0-%d,AD1=%d\r\n",i16,j16);
    DebugOutStr((int8_t*)pbuf);
		//�ͷŻ���
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
