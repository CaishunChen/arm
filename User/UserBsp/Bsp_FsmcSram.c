/**
  ******************************************************************************
  * @file    Bsp_FsmcSram.c 
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

/* Includes ------------------------------------------------------------------*/
#include "Bsp_FsmcSram.h"
#include "uctsk_Debug.h"
#include "Bsp_Tim.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t BspFsmcSram_State=ERR;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief   SRAM 初始化函数
 * @note    配置连接外部SRAM的GPIO和FSMC
 * @param   None
 * @return  None
 */
void BspFsmcSram_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
	             RCC_AHB1Periph_GPIOG, ENABLE);

	/* 使能 FSMC 时钟 */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/* SRAM 的 GPIO ：
		PD0/FSMC_D2
		PD1/FSMC_D3
		PD4/FSMC_NOE
		PD5/FSMC_NWE
		PD8/FSMC_D13
		PD9/FSMC_D14
		PD10/FSMC_D15
		PD11/FSMC_A16
		PD12/FSMC_A17
		PD13/FSMC_A18
		PD14/FSMC_D0
		PD15/FSMC_D1

		PE0/FSMC_NBL0
		PE1/FSMC_NBL1
		PE3/FSMC_A19
		PE4/FSMC_A20	-- 参与片选的译码
		PE5/FSMC_A21	-- 参与片选的译码
		PE7/FSMC_D4
		PE8/FSMC_D5
		PE9/FSMC_D6
		PE10/FSMC_D7
		PE11/FSMC_D8
		PE12/FSMC_D9
		PE13/FSMC_D10
		PE14/FSMC_D11
		PE15/FSMC_D12

		PF0/FSMC_A0
		PF1/FSMC_A1
		PF2/FSMC_A2
		PF3/FSMC_A3
		PF4/FSMC_A4
		PF5/FSMC_A5
		PF12/FSMC_A6
		PF13/FSMC_A7
		PF14/FSMC_A8
		PF15/FSMC_A9

		PG0/FSMC_A10
		PG1/FSMC_A11
		PG2/FSMC_A12
		PG3/FSMC_A13
		PG4/FSMC_A14
		PG5/FSMC_A15
		PG10/FSMC_NE3	--- 片选主信号
	*/

	/* GPIOD configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_4  | GPIO_Pin_5  |
	                    GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
	                    GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* GPIOE configuration */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3 |
	                    GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_7 |
	                    GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11|
	                    GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init(GPIOE, &GPIO_InitStructure);


	/* GPIOF configuration */
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3  |
	                    GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_12 | GPIO_Pin_13 |
	                    GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init(GPIOF, &GPIO_InitStructure);


	/* GPIOG configuration */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10 , GPIO_AF_FSMC);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
	                    GPIO_Pin_4  | GPIO_Pin_5  |GPIO_Pin_10;

	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/*-- FSMC Configuration ------------------------------------------------------*/
	p.FSMC_AddressSetupTime = 3;		/* 设置为2会出错; 3正常 */
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 2;			/* 设置为1出错，2正常 */
	p.FSMC_BusTurnAroundDuration = 1;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;	// FSMC_MemoryType_PSRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/*!< Enable FSMC Bank1_SRAM3 Bank */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);
   //
   if(0==BspFsmcSram_Test())
   {
      BspFsmcSram_State=OK;
   }
   else
   {
      BspFsmcSram_State=ERR;
   }
   BspFsmcSram_State=BspFsmcSram_State;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_TestExtSRAM
*	功能说明: 扫描测试外部SRAM
*	形    参: 无
*	返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint8_t BspFsmcSram_Test(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* 写SRAM */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* 读SRAM */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

	/* 对SRAM 的数据求反并写入 */
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* 再次比较SRAM的数据 */
	err = 0;
	pSRAM = (uint32_t *)EXT_SRAM_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)EXT_SRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}

/**
 * @brief   Debug接口函数
 * @note    开启Debug相关指令后,打印出SRAM基本信息以及测试读写速度
 * @param   OnOff-判断指令进入与退出
 * @return  None
 */
void BspFsmcSram_DebugTestOnOff(uint8_t OnOff)
{
   uint8_t *pbuf;
   uint32_t i32,j32;
   pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
   //
   sprintf((char*)pbuf,"*SRAM Chip Test:\r\n");
   DebugOutStr((int8_t*)pbuf);
   if(BspFsmcSram_State==OK)
   {
      sprintf((char*)pbuf,"  Model:IS61WV102416BLL-10TL 2MByte 16Bit 10ns\r\n");
      DebugOutStr((int8_t*)pbuf);
      sprintf((char*)pbuf,"  Addr :0x%08lX\r\n",EXT_SRAM_ADDR);
      DebugOutStr((int8_t*)pbuf);
      sprintf((char*)pbuf,"  Size :%d MB\r\n",EXT_SRAM_SIZE>>20);
      DebugOutStr((int8_t*)pbuf);
   }
   else
   {
      sprintf((char*)pbuf,"ERR!\r\n");
      DebugOutStr((int8_t*)pbuf);
   }
   //
   sprintf((char*)pbuf,"*SRAM Write Test:\r\n");
   DebugOutStr((int8_t*)pbuf);
   {
      uint8_t i,j;
	   uint16_t *pBuf;
	   uint16_t usTemp;
      for(i=0;i<2;i++)
      {
	      pBuf = (uint16_t *)EXT_SRAM_ADDR;
         if(i==0)
         {
	         usTemp = 0x5555;
         }
         else
         {
            usTemp = 0xAAAA;
         }
         Count_GetTimeMs(&j32);
	      for (j = 0; j < 10; j++)
	      {
            pBuf = (uint16_t *)EXT_SRAM_ADDR;
		      for (i32 = 0; i32 < EXT_SRAM_SIZE / 2; i32++)
		      {
			      *pBuf++ = usTemp;
		      }
	      }
	      Count_GetTimeMs(&i32);
         j32 = i32-j32;	
	      sprintf((char*)pbuf,"  Write %dMB(0x%04x): %dms, Spead: %dKB/s\r\n"\
                 ,(EXT_SRAM_SIZE>>20)*10\
                 ,usTemp\
                 ,j32\
                 ,(10 * (EXT_SRAM_SIZE>>10)  * 1000) / j32);
         DebugOutStr((int8_t*)pbuf);
      }
   }
   //
   sprintf((char*)pbuf,"*SRAM Read Test:\r\n");
   DebugOutStr((int8_t*)pbuf);
   {
	   uint16_t *pBuf;
      uint16_t usTemp;
	   Count_GetTimeMs(&j32);
	   for (pbuf[250] = 0; pbuf[250] < 10; pbuf[250]++)
	   {
         pBuf = (uint16_t *)(EXT_SRAM_ADDR);
	      for (i32 = 0; i32 < EXT_SRAM_SIZE / 2; i32++)
	      {
		      usTemp = *pBuf++;
	      }
	   }
      //
      Count_GetTimeMs(&i32);
      usTemp=usTemp;
      j32 = i32-j32;
      //
	   sprintf((char*)pbuf,"  Read %ldMB: %dms, Spead: %dB/s\r\n"\
                  ,(EXT_SRAM_SIZE>>20)*10\
                  , j32\
                  , (10 * (EXT_SRAM_SIZE>>10)*1000) / j32);
      DebugOutStr((int8_t*)pbuf);
   }
   //
   MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
   OnOff=OnOff;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

