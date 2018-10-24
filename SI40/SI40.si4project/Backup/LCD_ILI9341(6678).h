/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-29
***********************************************************************************
*/
// 文件描述：驱动:   ILI9341 6804 3510 5310 9325 9328 9320 9331
//                   5408    1505 B505 C505 8989 ...
// 注意事项：此文件的原始文件来自互联网,后经本人修改,使用前需要反复验证.
// 修改记录：
// 修改时间:
//-------------------------------------------------------------------------------
#ifndef __ILI9341_H
#define __ILI9341_H
//----------------------------------------------------------------
#include "includes.h"
//----------------------------------------------------------------数据类型
#define LCD_ILI9341_MODE_FSMC 0
#define LCD_ILI9341_MODE_GPIO 1
#define LCD_ILI9341_MODE_SPI  2
//扫描方向
enum LCD_ILI9341_E_SCAN_DIR
{
    LCD_ILI9341_E_SCAN_DIR_L2R_U2D=0,  //从左到右,从上到下
    LCD_ILI9341_E_SCAN_DIR_L2R_D2U,    //从左到右,从下到上
    LCD_ILI9341_E_SCAN_DIR_R2L_U2D,    //从右到左,从上到下
    LCD_ILI9341_E_SCAN_DIR_R2L_D2U,    //从右到左,从下到上

    LCD_ILI9341_E_SCAN_DIR_U2D_L2R,    //从上到下,从左到右
    LCD_ILI9341_E_SCAN_DIR_U2D_R2L,    //从上到下,从右到左
    LCD_ILI9341_E_SCAN_DIR_D2U_L2R,    //从下到上,从左到右
    LCD_ILI9341_E_SCAN_DIR_D2U_R2L,    //从下到上,从右到左
};
//颜色
enum LCD_ILI9341_E_COLOUR
{
    LCD_ILI9341_E_COLOUR_WHITE       =  0xFFFF,
    LCD_ILI9341_E_COLOUR_BLACK       =  0x0000,
    LCD_ILI9341_E_COLOUR_BLUE        =  0x001F,
    LCD_ILI9341_E_COLOUR_BRED        =  0XF81F,
    LCD_ILI9341_E_COLOUR_GRED        =  0XFFE0,
    LCD_ILI9341_E_COLOUR_GBLUE       =  0X07FF,
    LCD_ILI9341_E_COLOUR_RED         =  0xF800,
    LCD_ILI9341_E_COLOUR_MAGENTA     =  0xF81F,
    LCD_ILI9341_E_COLOUR_GREEN       =  0x07E0,
    LCD_ILI9341_E_COLOUR_CYAN        =  0x7FFF,
    LCD_ILI9341_E_COLOUR_YELLOW      =  0xFFE0,
    LCD_ILI9341_E_COLOUR_BROWN       =  0XBC40, //棕色
    LCD_ILI9341_E_COLOUR_BRRED       =  0XFC07, //棕红色
    LCD_ILI9341_E_COLOUR_GRAY        =  0X8430, //灰色
    LCD_ILI9341_E_COLOUR_DARKBLUE    =  0X01CF, //深蓝色
    LCD_ILI9341_E_COLOUR_LIGHTBLUE   =  0X7D7C, //浅蓝色
    LCD_ILI9341_E_COLOUR_GRAYBLUE    =  0X5458, //灰蓝色
    LCD_ILI9341_E_COLOUR_LIGHTGREEN  =  0X841F, //浅绿色
    LCD_ILI9341_E_COLOUR_LGRAY       =  0XC618, //浅灰色(PANNEL),窗体背景色
    LCD_ILI9341_E_COLOUR_LGRAYBLUE   =  0XA651, //浅灰蓝色(中间层颜色)
    LCD_ILI9341_E_COLOUR_LBBLUE      =  0X2B12, //浅棕蓝色(选择条目的反色)
};
//LCD重要参数集
typedef struct
{
    uint16_t   width;      //LCD 宽度
    uint16_t   height;     //LCD 高度
    uint16_t   id;         //LCD ID
    uint16_t   colorType;  //颜色类型 565 666 888 8888...
    uint8_t    dir;        //横屏还是竖屏控制：0，竖屏；1，横屏。
    uint8_t    wramcmd;    //开始写gram指令
    uint8_t    setxcmd;    //设置x坐标指令
    uint8_t    setycmd;    //设置y坐标指令
    
} LCD_ILI9341_S_DEV;
//-------------------------------------------------------------------------------
//                            硬件设置相关
//-------------------------------------------------------------------------------
//----------------------------------------------------------------宏定义选项
#if   (defined(PROJECT_XKAP_V3))
//---->
//-----总线模式选择
#define LCD_ILI9341_MODE   LCD_ILI9341_MODE_GPIO
//-----默认的扫描方向
#if   (HARDWARE_VER==3)
#define DFT_SCAN_DIR       LCD_ILI9341_E_SCAN_DIR_R2L_D2U
#else
#define DFT_SCAN_DIR       LCD_ILI9341_E_SCAN_DIR_L2R_U2D
#endif
//-----屏尺寸
#define LCD_ILI9341_WIDTH  320
#define LCD_ILI9341_HEIGHT 240
//<----
#elif (defined(XKAP_ICARE_B_D_M))
//---->
//-----总线模式选择
#define LCD_ILI9341_MODE   LCD_ILI9341_MODE_SPI
//-----默认的扫描方向
#define DFT_SCAN_DIR       LCD_ILI9341_E_SCAN_DIR_R2L_U2D
//-----屏尺寸
#define LCD_ILI9341_WIDTH  480/*480*/
#define LCD_ILI9341_HEIGHT 320/*320*/

//<----
#endif
//-------------------------------------------------------------------------------
//                            硬件接口相关
//-------------------------------------------------------------------------------
//----------------------------------------------------------------接口宏定义
#if   (LCD_ILI9341_MODE == LCD_ILI9341_MODE_FSMC)
//-----
typedef struct
{
    uint16_t LCD_ILI9341_REG;
    uint16_t LCD_ILI9341_RAM;
} LCD_ILI9341_S_FSMC_TYPE;
#define LCD_ILI9341_FSMC_BASE ((uint32_t)(0x6C000000 | 0x000007FE))
#define LCD_ILI9341_FSMC      ((LCD_ILI9341_S_FSMC_TYPE *) LCD_ILI9341_FSMC_BASE)
//-----
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
#if   (defined(PROJECT_XKAP_V3))
//---->
#if   (defined(STM32F1))
#define BSP_LCD_ILI9341_RCC_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE , ENABLE);
#elif (defined(STM32F4))
#define BSP_LCD_ILI9341_RCC_ENABLE RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE , ENABLE);
#endif
#define LCD_ILI9341_PORT_IM0  GPIOB
#define LCD_ILI9341_PIN_IM0   GPIO_Pin_15
#define LCD_ILI9341_PORT_WR   GPIOB
#define LCD_ILI9341_PIN_WR    GPIO_Pin_0
#define LCD_ILI9341_PORT_RS   GPIOB
#define LCD_ILI9341_PIN_RS    GPIO_Pin_13
#define LCD_ILI9341_PORT_RST  GPIOC
#define LCD_ILI9341_PIN_RST   GPIO_Pin_4
#define LCD_ILI9341_PORT_RD   GPIOC
#define LCD_ILI9341_PIN_RD    GPIO_Pin_5
#define LCD_ILI9341_PORT_CS   GPIOB
#define LCD_ILI9341_PIN_CS    GPIO_Pin_12
#define LCD_ILI9341_PORT_D0   GPIOE
#define LCD_ILI9341_PIN_D0    GPIO_Pin_8
#define LCD_ILI9341_PORT_D1   GPIOE
#define LCD_ILI9341_PIN_D1    GPIO_Pin_9
#define LCD_ILI9341_PORT_D2   GPIOE
#define LCD_ILI9341_PIN_D2    GPIO_Pin_10
#define LCD_ILI9341_PORT_D3   GPIOE
#define LCD_ILI9341_PIN_D3    GPIO_Pin_11
#define LCD_ILI9341_PORT_D4   GPIOE
#define LCD_ILI9341_PIN_D4    GPIO_Pin_12
#define LCD_ILI9341_PORT_D5   GPIOE
#define LCD_ILI9341_PIN_D5    GPIO_Pin_13
#define LCD_ILI9341_PORT_D6   GPIOE
#define LCD_ILI9341_PIN_D6    GPIO_Pin_14
#define LCD_ILI9341_PORT_D7   GPIOE
#define LCD_ILI9341_PIN_D7    GPIO_Pin_15
//#define LCD_ILI9341_PORT_BACKLIGHT1 GPIOC
//#define LCD_ILI9341_PIN_BACKLIGHT1  GPIO_Pin_7
#define LCD_ILI9341_PORT_BACKLIGHT2 GPIOC
#define LCD_ILI9341_PIN_BACKLIGHT2  GPIO_Pin_6
//如果数据线规律排列,做如下定义
#define LCD_ILI9341_PORT_DATA GPIOE
//<----
#endif
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
//-----
#if  (defined(XKAP_ICARE_B_D_M))
#if   (defined(STM32F1))
#define BSP_LCD_ILI9341_RCC_ENABLE RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
#elif (defined(STM32F4))
#define BSP_LCD_ILI9341_RCC_ENABLE RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
#endif
// SPI
#define LCD_ILI9341_SPI_SPIX              SPI2
#define LCD_ILI9341_SPI_MOSI_PORT         GPIOB
#define LCD_ILI9341_SPI_MOSI_PIN          GPIO_Pin_15
#define LCD_ILI9341_SPI_MOSI_SOURCE       GPIO_PinSource15
#define LCD_ILI9341_SPI_MISO_PORT         GPIOB
#define LCD_ILI9341_SPI_MISO_PIN          GPIO_Pin_14
#define LCD_ILI9341_SPI_MISO_SOURCE       GPIO_PinSource14
#define LCD_ILI9341_SPI_SCLK_PORT         GPIOB
#define LCD_ILI9341_SPI_SCLK_PIN          GPIO_Pin_13
#define LCD_ILI9341_SPI_SCLK_SOURCE       GPIO_PinSource13
#define LCD_ILI9341_SPI_CS_PORT           GPIOB
#define LCD_ILI9341_SPI_CS_PIN            GPIO_Pin_12
#define LCD_ILI9341_SPI_CS_SOURCE         GPIO_PinSource12
#define LCD_ILI9341_SPI_GPIO_RCC_ENABLE   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
#define LCD_ILI9341_SPI_RCC_ENABLE        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#define LCD_ILI9341_SPI_MOSI_REMAP        GPIO_PinAFConfig(LCD_ILI9341_SPI_MOSI_PORT, LCD_ILI9341_SPI_MOSI_SOURCE, GPIO_AF_SPI2);
#define LCD_ILI9341_SPI_MISO_REMAP        GPIO_PinAFConfig(LCD_ILI9341_SPI_MISO_PORT, LCD_ILI9341_SPI_MISO_SOURCE, GPIO_AF_SPI2);
#define LCD_ILI9341_SPI_SCLK_REMAP        GPIO_PinAFConfig(LCD_ILI9341_SPI_SCLK_PORT, LCD_ILI9341_SPI_SCLK_SOURCE, GPIO_AF_SPI2);
#define LCD_ILI9341_SPI_CS_REMAP 
// RST
#define LCD_ILI9341_PORT_RST              GPIOC
#define LCD_ILI9341_PIN_RST               GPIO_Pin_4
// DCX
#define LCD_ILI9341_PORT_DCX              GPIOB
#define LCD_ILI9341_PIN_DCX               GPIO_Pin_11
#endif
//-----
#endif
//-------------------------------------------------------------------------------
//                            硬件无关
//-------------------------------------------------------------------------------
//----------------------------------------------------------------接口宏定义
//直接访问寄存器的宏定义模式要比正常的定义模式快1倍的速度
#if (LCD_ILI9341_MODE == LCD_ILI9341_MODE_GPIO)
#if   (defined(STM32F1))
#define LCD_ILI9341_WR_SET    LCD_ILI9341_PORT_WR->BSRR=LCD_ILI9341_PIN_WR       /*GPIO_SetBits(LCD_ILI9341_PORT_WR    ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_WR_CLR    LCD_ILI9341_PORT_WR->BRR=LCD_ILI9341_PIN_WR        /*GPIO_ResetBits(LCD_ILI9341_PORT_WR  ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_RS_SET    LCD_ILI9341_PORT_RS->BSRR=LCD_ILI9341_PIN_RS       /*GPIO_SetBits(LCD_ILI9341_PORT_RS    ,  LCD_ILI9341_PIN_RS)*/
#define LCD_ILI9341_RS_CLR    LCD_ILI9341_PORT_RS->BRR=LCD_ILI9341_PIN_RS        /*GPIO_ResetBits(LCD_ILI9341_PORT_RS  ,  LCD_ILI9341_PIN_RS)*/
#define LCD_ILI9341_RST_SET   LCD_ILI9341_PORT_RST->BSRR=LCD_ILI9341_PIN_RST     /*GPIO_SetBits(LCD_ILI9341_PORT_RST   ,  LCD_ILI9341_PIN_RST)*/
#define LCD_ILI9341_RST_CLR   LCD_ILI9341_PORT_RST->BRR=LCD_ILI9341_PIN_RST      /*GPIO_ResetBits(LCD_ILI9341_PORT_RST ,  LCD_ILI9341_PIN_RST)*/
#define LCD_ILI9341_RD_SET    LCD_ILI9341_PORT_RD->BSRR=LCD_ILI9341_PIN_RD       /*GPIO_SetBits(LCD_ILI9341_PORT_RD    ,  LCD_ILI9341_PIN_RD)*/
#define LCD_ILI9341_RD_CLR    LCD_ILI9341_PORT_RD->BRR=LCD_ILI9341_PIN_RD        /*GPIO_ResetBits(LCD_ILI9341_PORT_RD  ,  LCD_ILI9341_PIN_RD)*/
#define LCD_ILI9341_CS_SET    LCD_ILI9341_PORT_CS->BSRR=LCD_ILI9341_PIN_CS       /*GPIO_SetBits(LCD_ILI9341_PORT_CS    ,  LCD_ILI9341_PIN_CS)*/
#define LCD_ILI9341_CS_CLR    LCD_ILI9341_PORT_CS->BRR=LCD_ILI9341_PIN_CS        /*GPIO_ResetBits(LCD_ILI9341_PORT_CS  ,  LCD_ILI9341_PIN_CS)*/
#define LCD_ILI9341_D0_SET    LCD_ILI9341_PORT_D0->BSRR=LCD_ILI9341_PIN_D0       /*GPIO_SetBits(LCD_ILI9341_PORT_D0    ,  LCD_ILI9341_PIN_D0)*/
#define LCD_ILI9341_D0_CLR    LCD_ILI9341_PORT_D0->BRR=LCD_ILI9341_PIN_D0        /*GPIO_ResetBits(LCD_ILI9341_PORT_D0  ,  LCD_ILI9341_PIN_D0)*/
#define LCD_ILI9341_D1_SET    LCD_ILI9341_PORT_D1->BSRR=LCD_ILI9341_PIN_D1       /*GPIO_SetBits(LCD_ILI9341_PORT_D1    ,  LCD_ILI9341_PIN_D1)*/
#define LCD_ILI9341_D1_CLR    LCD_ILI9341_PORT_D1->BRR=LCD_ILI9341_PIN_D1        /*GPIO_ResetBits(LCD_ILI9341_PORT_D1  ,  LCD_ILI9341_PIN_D1)*/
#define LCD_ILI9341_D2_SET    LCD_ILI9341_PORT_D2->BSRR=LCD_ILI9341_PIN_D2       /*GPIO_SetBits(LCD_ILI9341_PORT_D2    ,  LCD_ILI9341_PIN_D2)*/
#define LCD_ILI9341_D2_CLR    LCD_ILI9341_PORT_D2->BRR=LCD_ILI9341_PIN_D2        /*GPIO_ResetBits(LCD_ILI9341_PORT_D2  ,  LCD_ILI9341_PIN_D2)*/
#define LCD_ILI9341_D3_SET    LCD_ILI9341_PORT_D3->BSRR=LCD_ILI9341_PIN_D3       /*GPIO_SetBits(LCD_ILI9341_PORT_D3    ,  LCD_ILI9341_PIN_D3)*/
#define LCD_ILI9341_D3_CLR    LCD_ILI9341_PORT_D3->BRR=LCD_ILI9341_PIN_D3        /*GPIO_ResetBits(LCD_ILI9341_PORT_D3  ,  LCD_ILI9341_PIN_D3)*/
#define LCD_ILI9341_D4_SET    LCD_ILI9341_PORT_D4->BSRR=LCD_ILI9341_PIN_D4       /*GPIO_SetBits(LCD_ILI9341_PORT_D4    ,  LCD_ILI9341_PIN_D4)*/
#define LCD_ILI9341_D4_CLR    LCD_ILI9341_PORT_D4->BRR=LCD_ILI9341_PIN_D4        /*GPIO_ResetBits(LCD_ILI9341_PORT_D4  ,  LCD_ILI9341_PIN_D4)*/
#define LCD_ILI9341_D5_SET    LCD_ILI9341_PORT_D5->BSRR=LCD_ILI9341_PIN_D5       /*GPIO_SetBits(LCD_ILI9341_PORT_D5    ,  LCD_ILI9341_PIN_D5)*/
#define LCD_ILI9341_D5_CLR    LCD_ILI9341_PORT_D5->BRR=LCD_ILI9341_PIN_D5        /*GPIO_ResetBits(LCD_ILI9341_PORT_D5  ,  LCD_ILI9341_PIN_D5)*/
#define LCD_ILI9341_D6_SET    LCD_ILI9341_PORT_D6->BSRR=LCD_ILI9341_PIN_D6       /*GPIO_SetBits(LCD_ILI9341_PORT_D6    ,  LCD_ILI9341_PIN_D6)*/
#define LCD_ILI9341_D6_CLR    LCD_ILI9341_PORT_D6->BRR=LCD_ILI9341_PIN_D6        /*GPIO_ResetBits(LCD_ILI9341_PORT_D6  ,  LCD_ILI9341_PIN_D6)*/
#define LCD_ILI9341_D7_SET    LCD_ILI9341_PORT_D7->BSRR=LCD_ILI9341_PIN_D7/*GPIO_SetBits(LCD_ILI9341_PORT_D7    ,  LCD_ILI9341_PIN_D7)*/
#define LCD_ILI9341_D7_CLR    LCD_ILI9341_PORT_D7->BRR=LCD_ILI9341_PIN_D7/*GPIO_ResetBits(LCD_ILI9341_PORT_D7  ,  LCD_ILI9341_PIN_D7)*/
#elif (defined(STM32F4))
#define LCD_ILI9341_IM0_SET   LCD_ILI9341_PORT_IM0->BSRRL=LCD_ILI9341_PIN_IM0       /*GPIO_SetBits(LCD_ILI9341_PORT_WR    ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_IM0_CLR   LCD_ILI9341_PORT_IM0->BSRRH=LCD_ILI9341_PIN_IM0        /*GPIO_ResetBits(LCD_ILI9341_PORT_WR  ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_WR_SET    LCD_ILI9341_PORT_WR->BSRRL=LCD_ILI9341_PIN_WR       /*GPIO_SetBits(LCD_ILI9341_PORT_WR    ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_WR_CLR    LCD_ILI9341_PORT_WR->BSRRH=LCD_ILI9341_PIN_WR        /*GPIO_ResetBits(LCD_ILI9341_PORT_WR  ,  LCD_ILI9341_PIN_WR)*/
#define LCD_ILI9341_RS_SET    LCD_ILI9341_PORT_RS->BSRRL=LCD_ILI9341_PIN_RS       /*GPIO_SetBits(LCD_ILI9341_PORT_RS    ,  LCD_ILI9341_PIN_RS)*/
#define LCD_ILI9341_RS_CLR    LCD_ILI9341_PORT_RS->BSRRH=LCD_ILI9341_PIN_RS        /*GPIO_ResetBits(LCD_ILI9341_PORT_RS  ,  LCD_ILI9341_PIN_RS)*/
#define LCD_ILI9341_RST_SET   LCD_ILI9341_PORT_RST->BSRRL=LCD_ILI9341_PIN_RST     /*GPIO_SetBits(LCD_ILI9341_PORT_RST   ,  LCD_ILI9341_PIN_RST)*/
#define LCD_ILI9341_RST_CLR   LCD_ILI9341_PORT_RST->BSRRH=LCD_ILI9341_PIN_RST      /*GPIO_ResetBits(LCD_ILI9341_PORT_RST ,  LCD_ILI9341_PIN_RST)*/
#define LCD_ILI9341_RD_SET    LCD_ILI9341_PORT_RD->BSRRL=LCD_ILI9341_PIN_RD       /*GPIO_SetBits(LCD_ILI9341_PORT_RD    ,  LCD_ILI9341_PIN_RD)*/
#define LCD_ILI9341_RD_CLR    LCD_ILI9341_PORT_RD->BSRRH=LCD_ILI9341_PIN_RD        /*GPIO_ResetBits(LCD_ILI9341_PORT_RD  ,  LCD_ILI9341_PIN_RD)*/
#define LCD_ILI9341_CS_SET    LCD_ILI9341_PORT_CS->BSRRL=LCD_ILI9341_PIN_CS       /*GPIO_SetBits(LCD_ILI9341_PORT_CS    ,  LCD_ILI9341_PIN_CS)*/
#define LCD_ILI9341_CS_CLR    LCD_ILI9341_PORT_CS->BSRRH=LCD_ILI9341_PIN_CS        /*GPIO_ResetBits(LCD_ILI9341_PORT_CS  ,  LCD_ILI9341_PIN_CS)*/
#define LCD_ILI9341_D0_SET    LCD_ILI9341_PORT_D0->BSRRL=LCD_ILI9341_PIN_D0       /*GPIO_SetBits(LCD_ILI9341_PORT_D0    ,  LCD_ILI9341_PIN_D0)*/
#define LCD_ILI9341_D0_CLR    LCD_ILI9341_PORT_D0->BSRRH=LCD_ILI9341_PIN_D0        /*GPIO_ResetBits(LCD_ILI9341_PORT_D0  ,  LCD_ILI9341_PIN_D0)*/
#define LCD_ILI9341_D1_SET    LCD_ILI9341_PORT_D1->BSRRL=LCD_ILI9341_PIN_D1       /*GPIO_SetBits(LCD_ILI9341_PORT_D1    ,  LCD_ILI9341_PIN_D1)*/
#define LCD_ILI9341_D1_CLR    LCD_ILI9341_PORT_D1->BSRRH=LCD_ILI9341_PIN_D1        /*GPIO_ResetBits(LCD_ILI9341_PORT_D1  ,  LCD_ILI9341_PIN_D1)*/
#define LCD_ILI9341_D2_SET    LCD_ILI9341_PORT_D2->BSRRL=LCD_ILI9341_PIN_D2       /*GPIO_SetBits(LCD_ILI9341_PORT_D2    ,  LCD_ILI9341_PIN_D2)*/
#define LCD_ILI9341_D2_CLR    LCD_ILI9341_PORT_D2->BSRRH=LCD_ILI9341_PIN_D2        /*GPIO_ResetBits(LCD_ILI9341_PORT_D2  ,  LCD_ILI9341_PIN_D2)*/
#define LCD_ILI9341_D3_SET    LCD_ILI9341_PORT_D3->BSRRL=LCD_ILI9341_PIN_D3       /*GPIO_SetBits(LCD_ILI9341_PORT_D3    ,  LCD_ILI9341_PIN_D3)*/
#define LCD_ILI9341_D3_CLR    LCD_ILI9341_PORT_D3->BSRRH=LCD_ILI9341_PIN_D3        /*GPIO_ResetBits(LCD_ILI9341_PORT_D3  ,  LCD_ILI9341_PIN_D3)*/
#define LCD_ILI9341_D4_SET    LCD_ILI9341_PORT_D4->BSRRL=LCD_ILI9341_PIN_D4       /*GPIO_SetBits(LCD_ILI9341_PORT_D4    ,  LCD_ILI9341_PIN_D4)*/
#define LCD_ILI9341_D4_CLR    LCD_ILI9341_PORT_D4->BSRRH=LCD_ILI9341_PIN_D4        /*GPIO_ResetBits(LCD_ILI9341_PORT_D4  ,  LCD_ILI9341_PIN_D4)*/
#define LCD_ILI9341_D5_SET    LCD_ILI9341_PORT_D5->BSRRL=LCD_ILI9341_PIN_D5       /*GPIO_SetBits(LCD_ILI9341_PORT_D5    ,  LCD_ILI9341_PIN_D5)*/
#define LCD_ILI9341_D5_CLR    LCD_ILI9341_PORT_D5->BSRRH=LCD_ILI9341_PIN_D5        /*GPIO_ResetBits(LCD_ILI9341_PORT_D5  ,  LCD_ILI9341_PIN_D5)*/
#define LCD_ILI9341_D6_SET    LCD_ILI9341_PORT_D6->BSRRL=LCD_ILI9341_PIN_D6       /*GPIO_SetBits(LCD_ILI9341_PORT_D6    ,  LCD_ILI9341_PIN_D6)*/
#define LCD_ILI9341_D6_CLR    LCD_ILI9341_PORT_D6->BSRRH=LCD_ILI9341_PIN_D6        /*GPIO_ResetBits(LCD_ILI9341_PORT_D6  ,  LCD_ILI9341_PIN_D6)*/
#define LCD_ILI9341_D7_SET    LCD_ILI9341_PORT_D7->BSRRL=LCD_ILI9341_PIN_D7/*GPIO_SetBits(LCD_ILI9341_PORT_D7    ,  LCD_ILI9341_PIN_D7)*/
#define LCD_ILI9341_D7_CLR    LCD_ILI9341_PORT_D7->BSRRH=LCD_ILI9341_PIN_D7/*GPIO_ResetBits(LCD_ILI9341_PORT_D7  ,  LCD_ILI9341_PIN_D7)*/
#endif
#elif (LCD_ILI9341_MODE == LCD_ILI9341_MODE_SPI)
#define LCD_ILI9341_RST_SET      LCD_ILI9341_PORT_RST->BSRRL=LCD_ILI9341_PIN_RST
#define LCD_ILI9341_RST_CLR      LCD_ILI9341_PORT_RST->BSRRH=LCD_ILI9341_PIN_RST
#define LCD_ILI9341_DCX_SET      LCD_ILI9341_PORT_DCX->BSRRL=LCD_ILI9341_PIN_DCX
#define LCD_ILI9341_DCX_CLR      LCD_ILI9341_PORT_DCX->BSRRH=LCD_ILI9341_PIN_DCX
#define LCD_ILI9341_SPI_CS_LOW   GPIO_ResetBits(LCD_ILI9341_SPI_CS_PORT, LCD_ILI9341_SPI_CS_PIN)
#define LCD_ILI9341_SPI_CS_HIGH  GPIO_SetBits(LCD_ILI9341_SPI_CS_PORT, LCD_ILI9341_SPI_CS_PIN)
#endif
//----------------------------------------------------------------
extern LCD_ILI9341_S_DEV Lcd_Ili9341_s_dev;
extern uint8_t LCD_ILI9341_SleepFlag;

void LCD_ILI9341_SetBackLight(uint8_t _bright);
void LCD_ILI9341_HwInit(void);
uint16_t LCD_ILI9341_ReadID(void);
void LCD_ILI9341_Init(void);
void LCD_ILI9341_Clear(uint32_t color);
void LCD_ILI9341_DisplayOn(void);
void LCD_ILI9341_DisplayOff(void);
void LCD_ILI9341_DrawPoint(uint16_t x,uint16_t y,uint32_t color);
uint16_t LCD_ILI9341_ReadPoint(uint16_t x,uint16_t y);
void LCD_ILI9341_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void LCD_ILI9341_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);
void LCD_ILI9341_DrawCircle(uint16_t x0,uint16_t y0,uint8_t r,uint32_t color);
void LCD_ILI9341_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
//----------------------------------------------------------------
void LCD_ILI9341_SetWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void LCD_ILI9341_QuitWinow(void);
//----------------------------------------------------------------
void LCD_ILI9341_WR_DATA(uint16_t data);
void LCD_ILI9341_WR_REG(uint16_t regval);
uint16_t LCD_ILI9341_RD_DATA(void);
//
void LCD_ILI9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint32_t _usColor);
void LCD_ILI9341_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor);
void LCD_ILI9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint32_t _usColor);
void LCD_ILI9341_DrawRect(uint16_t _usX, uint16_t _usY, uint8_t _usHeight, uint16_t _usWidth, uint32_t _usColor);
void LCD_ILI9341_EnterSleep(void);
void LCD_ILI9341_EnterWork(void);
void LCD_ILI9341_DebugTestOnOff(uint8_t OnOff);
//----------------------------------------------------------------
#endif

