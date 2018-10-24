/****************************************Copyright (c)****************************************************
** 文件名称: main.c
** 说    明: STM32-USB-bootloader
*********************************************************************************************************/
//#define USB_ENABLE
#define BOOTLOADER_TF_ENABLE
//----------------------------------------------------------------------------Includes
#include <string.h>
#include <stdio.h>
#include  <stdint.h>
#include "Bsp_CpuFlash.h"
#include "Bsp_ExtFlash.h"
#include "Module_Memory.h"

#if   (defined(STM32F1))
#include "stm32f10x.h"
#elif (defined(STM32F4))
#include "stm32f4xx.h"
#elif	(defined(NRF51)||defined(NRF52))
#include "nrf_bootloader.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_info.h"
#endif

#ifdef USB_ENABLE
#include "mass_mal.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#endif

#ifdef BOOTLOADER_TF_ENABLE
#include "ff.h"
#include "diskio.h"
#endif

#define BOOTLOADER_VER  (5)
//-----------------------------------------------------------------------------
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
//----------------------------------------------------------------------------变量
#ifdef USB_ENABLE
extern       uint8_t  Fat_RootDir[FAT_SIZE + ROOT_DIR_SIZE];  //存储文件分配表的RAM
extern const uint8_t  RootDirEntry[DIR_ENTRY] ;
#endif
//
#ifdef BOOTLOADER_TF_ENABLE
#define  BOOTLOADER_TF_FILE   "/fw.bin"
FATFS    BOOTLOADER_Fs;
FIL      BOOTLOADER_fsrc;
FRESULT  BOOTLOADER_Res;
uint8_t  BOOTLOADER_buf[1024];
uint32_t BOOTLOADER_FileSize;
#endif
typedef  void (*pFunction)(void);
pFunction             Jump_To_Application;
uint32_t              JumpAddress;
//----------------------------------------------------------------------------私有函数
#ifdef USB_ENABLE
void enter_usb_isp(void);
#endif

uint8_t check_isp_entry_pin(void);
void execute_user_code(void);
//-----FLASH
//根据芯片FLASH大小不同分区不同
#define USER_FLASH_ADDR_START_KB    20  //用户程序起始偏移地址
#define USER_FLASH_ADDR_ADDR_KB     265 //用户程序终止偏移地址
#define ISP_FLASH_ADDR_START_KB     266 //ISP程序起始偏移地址
#define ISP_FLASH_ADDR_END_KB       511 //ISP程序终止偏移地址

MODULE_MEMORY_S_PARA S_Para;
/*******************************************************************************
* 函数名称: main
*******************************************************************************/
uint32_t sum1,sum2;
int main(void)
{
#if	(defined(STM32F1)||defined(STM32F4))
    uint32_t i32,j32;
    uint32_t FlashData;
    //--------------------------芯片内部FLASH升级程序---------------------------------
    Bsp_CpuFlash_Init();
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
    //写入Boot版本
    if(S_Para.BootVer!=BOOTLOADER_VER)
    {
        S_Para.BootVer=BOOTLOADER_VER;
        Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
    }
    //判断是否有升级文件
    if(S_Para.Updata == 0x87654321)
    {
        //擦除主程序区
        Bsp_CpuFlash_Erase(BSP_CPU_FLASH_APP_ADDR,BSP_CPU_FLASH_APP_BACKUP_ADDR-1);
        //将升级程序区(ISP_FLASH_ADDR_START_KB-ISP_FLASH_ADDR_END_KB)-->主程序区(20-USER_FLASH_ADDR_ADDR_KB)
        FLASH_Unlock();
        sum1=0;
        for(i32=BSP_CPU_FLASH_APP_ADDR; i32<BSP_CPU_FLASH_APP_BACKUP_ADDR; i32+=4)
        {
            FlashData=*(vu32*)(BSP_CPU_FLASH_APP_BACKUP_ADDR+(i32-BSP_CPU_FLASH_APP_ADDR));
            FLASH_ProgramWord(i32,FlashData);
            sum1+=FlashData;
        }
        FLASH_Lock();
        //校验,利用累加和
        sum2=0;
        for(i32=BSP_CPU_FLASH_APP_ADDR; i32<BSP_CPU_FLASH_APP_BACKUP_ADDR; i32+=4)
        {
            FlashData=*(vu32*)(i32);
            sum2+=FlashData;
        }
        if(sum1==sum2)
        {
            //升级成功
            S_Para.Updata=0x12345678;
            S_Para.BootUpdataState=1;
            //擦除升级标志
            Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
            //printf("程序升级成功! \r\n");
        }
        else
        {
            //升级失败
            //printf("程序升级失败!系统重新启动升级 \r\n");
            while(1);
        }
    }
    else if(S_Para.Updata == 0x77654321)
    {
        Bsp_ExtFlash_Init();
        //验证SPI-Flash的安装情况
        while(BspExtFlash_s_Flash.TotalSize!=0)
        {
            //
            sum1=0;
            for(i32=0; i32<S_Para.UpdataLen; i32+=4)
            {
                Bsp_ExtFlash_Read_nByte(i32,(uint8_t *)&FlashData,4);
                sum1+=(uint8_t)(FlashData>>24);
                sum1+=(uint8_t)(FlashData>>16);
                sum1+=(uint8_t)(FlashData>>8);
                sum1+=(uint8_t)(FlashData);
            }
            if(sum1!=S_Para.UpdataSum)
            {
                break;
            }
            //擦除主程序区
            Bsp_CpuFlash_Erase(BSP_CPU_FLASH_APP_ADDR,BSP_CPU_FLASH_END_ADDR);
            //将升级程序区(ISP_FLASH_ADDR_START_KB-ISP_FLASH_ADDR_END_KB)-->主程序区(20-USER_FLASH_ADDR_ADDR_KB)
            FLASH_Unlock();
            sum1=0;
            for(i32=0; i32<S_Para.UpdataLen; i32+=4)
            {
                Bsp_ExtFlash_Read_nByte(i32,(uint8_t *)&FlashData,4);
                FLASH_ProgramWord(i32+BSP_CPU_FLASH_APP_ADDR,FlashData);
                sum1+=(uint8_t)(FlashData>>24);
                sum1+=(uint8_t)(FlashData>>16);
                sum1+=(uint8_t)(FlashData>>8);
                sum1+=(uint8_t)(FlashData);
            }
            FLASH_Lock();
            //校验,利用累加和
            if(sum1==S_Para.UpdataSum)
            {
                //升级成功
                S_Para.Updata=0x12345678;
                S_Para.BootUpdataState=1;
                //擦除升级标志
                Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
                //printf("程序升级成功! \r\n");
            }
            else
            {
                //升级失败
                //printf("程序升级失败!系统重新启动升级 \r\n");
                while(1);
            }
            break;
        }
    }
    //-----USB升级程序
#if   (defined(USB_ENABLE))
    if( check_isp_entry_pin() )
    {
        //没按,用户程序
        execute_user_code();
        while(1);
    }
    //进入ISP程序
    enter_usb_isp();
#endif
    //-----TF卡升级程序
#if   (defined(BOOTLOADER_TF_ENABLE))
    while(1)
    {
        //探测按键
        if(1 == check_isp_entry_pin())
        {
            break;
        }
        //挂载
        BOOTLOADER_Res = f_mount(&BOOTLOADER_Fs,"0:",0);
        if (BOOTLOADER_Res != FR_OK)
        {
            break;
        }
        //打开文件
        BOOTLOADER_Res = f_open(&BOOTLOADER_fsrc, BOOTLOADER_TF_FILE, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
        if (BOOTLOADER_Res != FR_OK)
        {
            //卸载磁盘
            f_mount(NULL,"0:",0);
            break;
        }
        //拷贝程序到TF卡
        //获取文件大小
        BOOTLOADER_FileSize = f_size(&BOOTLOADER_fsrc);
        if(BOOTLOADER_FileSize>BSP_CPU_FLASH_END_ADDR+1-BSP_CPU_FLASH_APP_ADDR)
        {
            // 关闭文件
            f_close(&BOOTLOADER_fsrc);
            //卸载磁盘
            f_mount(NULL,"0:",0);
            break;
        }
        //计算文件累加和
        //复制数据
        //擦除主程序区
        Bsp_CpuFlash_Erase(BSP_CPU_FLASH_APP_ADDR,BSP_CPU_FLASH_END_ADDR);
        //将升级程序区(ISP_FLASH_ADDR_START_KB-ISP_FLASH_ADDR_END_KB)-->主程序区(20-USER_FLASH_ADDR_ADDR_KB)
        FLASH_Unlock();
        sum1=0;
        for(i32=BSP_CPU_FLASH_APP_ADDR; i32<BSP_CPU_FLASH_END_ADDR; i32+=4)
        {
            memset(BOOTLOADER_buf,0xff,4);
            BOOTLOADER_Res = f_read(&BOOTLOADER_fsrc, BOOTLOADER_buf, 4, &j32);
            if(j32==0)
            {
                break;
            }
            FlashData = Count_4ByteToLong(BOOTLOADER_buf[3],BOOTLOADER_buf[2],BOOTLOADER_buf[1],BOOTLOADER_buf[0]);
            FLASH_ProgramWord(i32,FlashData);
            sum1+=FlashData;
        }
        FLASH_Lock();
        //重新计算累加和
        sum1=sum2=0;
        f_lseek(&BOOTLOADER_fsrc, 0);
        for(i32=BSP_CPU_FLASH_APP_ADDR; i32<BSP_CPU_FLASH_APP_ADDR+BOOTLOADER_FileSize; i32++)
        {
            sum1+=*(vu8*)i32;
        }
        while(1)
        {
            BOOTLOADER_Res = f_read(&BOOTLOADER_fsrc, BOOTLOADER_buf, 1, &j32);
            if(j32==0)
            {
                break;
            }
            else
            {
                sum2+=BOOTLOADER_buf[0];
            }
        }
        while(sum1!=sum2)
        {
            ;
        }
        //
        Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
        S_Para.BootUpdataState=1;
        Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)&S_Para,sizeof(MODULE_MEMORY_S_PARA));
        // 关闭文件
        f_close(&BOOTLOADER_fsrc);
        //卸载磁盘
        f_mount(NULL,"0:",0);
        //
        break;
    }
#endif
    //用户程序
    execute_user_code();
#elif	(defined(NRF51)||defined(NRF52))
	  //ble_stack_init();
		{
		}
		//APP_ERROR_CHECK_BOOL(NRF_UICR->CLENR0 == CODE_REGION_1_START);//14000
    //APP_ERROR_CHECK_BOOL(*((uint32_t *)NRF_UICR_BOOT_START_ADDRESS) == BOOTLOADER_REGION_START);//0
    //APP_ERROR_CHECK_BOOL(NRF_FICR->CODEPAGESIZE == CODE_PAGE_SIZE);//1024
		//nrf_bootloader_app_start(MAIN_APPLICATION_START_ADDR);
		
		nrf_bootloader_init();
		//UICR.BOOTLOADERADDR	=	NRF_UICR_BOOTLOADER_START_ADDRESS;
		//nrf_bootloader_app_start(MAIN_APPLICATION_START_ADDR);
		nrf_bootloader_app_start(0x0001F000);
#endif
}
/*******************************************************************************
* 函数名称: 检测按键
* 返 回 值: 0-按下 1-没按
*******************************************************************************/
#define BOOTLOADER_KEY1_PORT  GPIOD
#define BOOTLOADER_KEY1_PIN   GPIO_Pin_10
#define BOOTLOADER_KEY2_PORT  GPIOC
#define BOOTLOADER_KEY2_PIN   GPIO_Pin_3
#if   (defined(STM32F1))
#define BOOTLOADER_KEY_RCC_ENABLE RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD , ENABLE);
#elif (defined(STM32F4))
#define BOOTLOADER_KEY_RCC_ENABLE RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD , ENABLE);
#endif

uint8_t check_isp_entry_pin(void)
{
#if	(defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟
    BOOTLOADER_KEY_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_2MHz;
#endif
    GPIO_InitStructure.GPIO_Pin     =  BOOTLOADER_KEY1_PIN;
    GPIO_Init(BOOTLOADER_KEY1_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BOOTLOADER_KEY2_PIN;
    GPIO_Init(BOOTLOADER_KEY2_PORT, &GPIO_InitStructure);


    if( GPIO_ReadInputDataBit(BOOTLOADER_KEY1_PORT, BOOTLOADER_KEY1_PIN)\
        ||GPIO_ReadInputDataBit(BOOTLOADER_KEY2_PORT, BOOTLOADER_KEY2_PIN))
    {
        return 1;
    }
    return 0;
#elif	(defined(NRF51)||defined(NRF52))
		return 0;
#endif
}
/*******************************************************************************
* 函数名称: 检测按键
* 返 回 值: 1-没按 0-按下
*******************************************************************************/
uint8_t check_isp_Out_pin(void)
{
    /*
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
    // Wakeup Button -> PA0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) )
    {
    return 1;
    }
    else
    {
    return 0;
    }
    */
    return 1;
}
/*******************************************************************************
* 函数名称: 指向用户代码
*******************************************************************************/
void execute_user_code(void)
{
#if	(defined(STM32F1)||defined(STM32F4))
#if   (defined(STM32F1))
    if (((*(vu32*)BSP_CPU_FLASH_APP_ADDR) & 0x2FFF0000 ) == 0x20000000)
#endif
    {
        //跳转到用户程序
        JumpAddress = *(vu32*) (BSP_CPU_FLASH_APP_ADDR + 4);
        Jump_To_Application = (pFunction) JumpAddress;
        //初始化用户应用的堆栈指针
        __set_MSP(*(vu32*) BSP_CPU_FLASH_APP_ADDR);
        Jump_To_Application();
    }
#elif	(defined(NRF51)||defined(NRF52))
		return;
#endif
}
/*******************************************************************************
* 函数名称: 指向ISP代码
*******************************************************************************/
#ifdef USB_ENABLE
void enter_usb_isp(void)
{
    uint32_t n,m,next_cluster;
    //生成文件分配表去保留Flash空间
    //前两个FAT入口被保留
    Fat_RootDir[0]= 0xF8;
    Fat_RootDir[1]= 0xFF;
    Fat_RootDir[2]= 0xFF;
    //
    /* Start cluster of a file is indicated by the Directory entry = 2 */
    m = 3;
    for ( n = 3; n < NO_OF_CLUSTERS+2; n+=2)
    {
        if( n == (NO_OF_CLUSTERS-1) )
        {
            next_cluster = 0xFFF;
        }
        else
        {
            next_cluster = n + 1;
        }
        Fat_RootDir[m] = (uint8_t)n & 0xFF;
        Fat_RootDir[m+1] = (((uint8_t)next_cluster & 0xF) << 4) | ((uint8_t)(n>>8)&0xF);
        Fat_RootDir[m+2] = (uint8_t)(next_cluster >> 4) & 0xFF;

        m = m+3;
    }
    //复制根目录项
    for (n = 0; n < DIR_ENTRY ; n++)
    {
        //从FLASH到RAM
        Fat_RootDir[(FAT_SIZE+n)] = RootDirEntry[n];
    }
    //firmware.bin的文件大小
    Fat_RootDir[FAT_SIZE+60] = (uint8_t)(USER_FLASH_SIZE & 0xFF);
    Fat_RootDir[FAT_SIZE+61] = (uint8_t)(USER_FLASH_SIZE >> 8);
    Fat_RootDir[FAT_SIZE+62] = (uint8_t)(USER_FLASH_SIZE >> 16);
    Fat_RootDir[FAT_SIZE+63] = (uint8_t)(USER_FLASH_SIZE >> 24);
    //FLASH解锁
    FLASH_Unlock();
    //FLASH擦除用户区
    for( n = BSP_CPU_FLASH_APP_ADDR ; n < USER_FLASH_END; n += FLASH_PAGE_SIZE )
    {
        FLASH_ErasePage( n );
    }
    //系统设置
    Set_System();
    //USB时钟设置
    Set_USBClock();
    //USB终端配置
    USB_Interrupts_Config();
    //USB初始化
    USB_Init();
    //
    while (bDeviceState != CONFIGURED);
    //
    USB_Configured_LED();
    //死循环
    while (1)
    {
        if(check_isp_Out_pin())
        {
            ;
        }
        else
        {
            //用户程序(复位)
            *((u32 *)0xE000ED0C) = 0x05fa0004;
        }
    }
}
#endif
//---------------------------------------------------------------------
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
/*
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
*/
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

