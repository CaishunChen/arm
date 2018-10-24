/**
  ******************************************************************************
  * @file    Bsp_Hx711.c
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
//------------------------------- Includes -----------------------------------
#include "Bsp_Hx711.h"
#include "uctsk_Debug.h"
#if   (defined(XKAP_ICARE_B_M))
#include "uctsk_GprsAppXkap.h"
#include "Bsp_Rtc.h"
#include "Module_Memory.h"
static uint8_t BspHx711_StartCmt=0;
#endif
//----------------------------------------------------------------------------
//MODULE_OS_SEM(BspHx711_Sem);
static uint32_t BspHx711_ZeroValue=0;
static uint8_t BspHx711_DebugTest_Enable=0;
static uint8_t BspHx711_Read(uint32_t *value);
/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/
void BspHx711_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_HX711_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_HX711_SCLK_PIN;
    GPIO_Init(BSP_HX711_SCLK_PORT, &GPIO_InitStructure);
    //
#if   (defined(STM32F1))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_KEY_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
#elif (defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    BSP_KEY_RCC_ENABLE;
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BSP_HX711_DOUT_PIN;
    GPIO_Init(BSP_HX711_DOUT_PORT, &GPIO_InitStructure);
#elif (defined(NRF51)||defined(NRF52))
    nrf_gpio_cfg_output(BSP_HX711_SCLK_PIN);
    nrf_gpio_cfg_input(BSP_HX711_DOUT_PIN, NRF_GPIO_PIN_NOPULL);
#endif
    BSP_HX711_SCLK_H;
    //
    //创建互斥信号量
    //MODULE_OS_SEM_CREATE(BspHx711_Sem,"Sem_BspHx711",1);
    //
    BspHx711_ZeroValue  =  ModuleMemory_psPara->ThreshsholdValue1;
    if((BspHx711_ZeroValue==0x00000000)||(BspHx711_ZeroValue==0xFFFFFFFF))
    {
        BspHx711_Calibration();
    }

}
static uint8_t BspHx711_Read(uint32_t *value)
{
    uint32_t Count=0;
    uint8_t i=0;
    BSP_HX711_SCLK_L;
    Count=0;
    while(BSP_HX711_DOUT_R)
    {
        MODULE_OS_DELAY_MS(10);
        i++;
        if(i>=200)
        {
            break;
        }
    }
    if(i>=200)
    {
        return ERR;
    }
    //
    for (i=0; i<24; i++)
    {
        BSP_HX711_SCLK_H;
        Count=Count<<1;
        BSP_HX711_SCLK_L;
        if(BSP_HX711_DOUT_R)
        {
            Count++;
        }
    }
    BSP_HX711_SCLK_H;
    Count=Count^0x800000;//第25个脉冲下降沿来时,转换数据
    BSP_HX711_SCLK_L;
    *value = Count;
    return OK;
}

uint8_t BspHx711_Calibration(void)
{
    uint8_t i=0;
    uint32_t i32;
    BspHx711_ZeroValue=0;
    for(i=0; i<64; i++)
    {
        BspHx711_Read(&i32);
        BspHx711_ZeroValue += i32;
    }
    // 计算均值
    BspHx711_ZeroValue = BspHx711_ZeroValue/64;
    // 保存为参数
    {
        MODULE_MEMORY_S_PARA *psPara;
        //申请缓存
        psPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)psPara,NULL);
        psPara->ThreshsholdValue1=BspHx711_ZeroValue;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)psPara,NULL);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psPara);
    }
    //
    return OK;
}
uint8_t BspHx711_Get_g(uint32_t *pvalue_g,uint32_t *pvalue_ad)
{
    uint32_t i32;
    //MODULE_OS_ERR err;
    //MODULE_OS_SEM_PEND(BspHx711_Sem,0,TRUE,err);
    if((BspHx711_ZeroValue==0x00000000)||(BspHx711_ZeroValue==0xFFFFFFFF))
    {
        BspHx711_Calibration();
    }
    // 采集
    if(OK!=BspHx711_Read(&i32))
    {
        //MODULE_OS_SEM_POST(BspHx711_Sem);
        return ERR;
    }
    if(pvalue_ad!=NULL)
    {
        *pvalue_ad=i32;
    }
    // 采集值与零值比对限值
    if(i32<=BspHx711_ZeroValue)
    {
        i32 = BspHx711_ZeroValue;
    }
    // 转换数据为克值
    i32 = (unsigned int)((float)((i32-BspHx711_ZeroValue)/100)/BSP_HX711_REF_VALUE+0.05);
    // 限值
    if(i32 > 50000)
    {
        i32 = 50000;
    }
    if(pvalue_g!=NULL)
    {
        *pvalue_g = i32;
    }
    //MODULE_OS_SEM_POST(BspHx711_Sem);
    return OK;
}

void BspHx711_100ms(void)
{
    //static uint8_t si=0;
    static uint16_t si16=0;
    uint8_t *pbuf;
    uint8_t res;
    uint32_t i32;
    if(BspHx711_StartCmt==0)
    {
        return;
    }
    BspHx711_StartCmt--;

    // 打印输出
    if(BspHx711_DebugTest_Enable==1)
    {
        // 采集
        res=BspHx711_Get_g(&i32,NULL);
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(res==OK)
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: Hx711(%05d)-%ld\r\n",si16++,i32);
            DebugOutStr((int8_t*)pbuf);
        }
        else
        {
            //打印输出
            sprintf((char*)pbuf,"DebugOut: Hx711(%05d)-ERR\r\n",si16++);
            DebugOutStr((int8_t*)pbuf);
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
void BspHx711_Precise1000ms(void)
{
    if(BspHx711_StartCmt!=0xFF)
    {
        BspHx711_StartCmt++;
    }
}
void BspHx711_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspHx711_DebugTest_Enable=1;
    }
    else
    {
        BspHx711_DebugTest_Enable=0;
    }
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
