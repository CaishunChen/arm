/****************************************Copyright (c)****************************************************
** 文件名称: main.c
** 说    明: STM32-USB-bootloader
*********************************************************************************************************/
//#define USB_ENABLE
//----------------------------------------------------------------------------Includes
#include <string.h>
#include <stdio.h>
#include "Bsp_Led.h"

#if   (defined(STM32F1))
#include "stm32f10x.h"
#elif (defined(STM32F4))
#include "stm32f4xx.h"
#endif

#ifdef USB_ENABLE
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#endif
//-----------------------------------------------------------------------------
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
//----------------------------------------------------------------------------变量

//----------------------------------------------------------------------------私有函数

/*******************************************************************************
* 函数名称: main
*******************************************************************************/
int main(void)
{
	  uint32_t i32;
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x800C000);
	  //初始化GPIO
	  BspLed_Init();
	  //
	  while(1)
		{
			BSP_LED_LED1_TOGGLE;
			i32=2000000;
			while(i32)i32--;
		}
}

//---------------------------------------------------------------------
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    //GPIO_SetBits(GPIOA,GPIO_Pin_8);
    // Place your implementation of fputc here
    // e.g. write a character to the USART
    USART_SendData(USART1, (uint8_t) ch);

    //Loop until the end of transmission
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
    //GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    return ch;
}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

