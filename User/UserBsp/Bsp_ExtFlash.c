/**
  ******************************************************************************
  * @file    Bsp_ExtFlash.c 
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
#include "Bsp_ExtFlash.h"
#ifndef BOOTLOADER
#include "uctsk_Debug.h"
#endif
//------------------------------- Typedef ------------------------------------
typedef enum IDTYPE {Manu_ID,Dev_ID,Jedec_ID} idtype;
//命令
#define  CMD_AAI        0xAD  // AAI 连续编程指令(FOR SST25VF016B)
#define  CMD_DISWR      0x04  // 禁止写, 退出AAI状态
#define  CMD_EWRSR      0x50  // 允许写状态寄存器的命令
#define  CMD_WRSR       0x01  // 写状态寄存器命令
#define  CMD_WREN       0x06  // 写使能命令
#define  CMD_READ       0x03  // 读数据区命令
#define  CMD_RDSR       0x05  // 读状态寄存器命令
#define  CMD_RDID       0x9F  // 读器件ID命令
#define  CMD_ERASE_S    0x20  // 擦除扇区命令
#define  CMD_ERASE_32K  0x52  // 擦除命令
#define  CMD_ERASE_64K  0xD8  // 擦除命令
#define  CMD_BE         0xC7  // 批量擦除命令
#define  CMD_DUMMY      0xA5  // 哑命令，可以为任意值，用于读操作
//------------------------------- 公共变量 ------------------------------------
#if   (defined(NRF51)||defined(NRF52))
// SPI instance index.
#define SPI_INSTANCE  0
// SPI instance.
static const nrf_drv_spi_t BspExFlash_Spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
// Flag used to indicate that SPI instance completed the transfer.
static volatile bool spi_xfer_done;
#endif
BSP_EXTFLASH_S_FLASH BspExtFlash_s_Flash;
//------------------------------- 函数声明 ------------------------------------
static uint8_t Bsp_ExtFlash_RW_Byte(uint8_t data);
static uint8_t Bsp_ExtFlash_PageWrite_nByte(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte);
/*******************************************************************************
* 函数功能: 入口函数---初始化GPIO,SPI
*******************************************************************************/
void Bsp_ExtFlash_Init(void)
{
#if   (defined(STM32F1)||defined(STM32F4))
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    static uint8_t first=1;
    if(first==0)
    {
        return;
    }
    first=0;
    //初始化时钟
    BSP_EXTFLASH_SPI_RCC_ENABLE;
    BSP_EXTFLASH_GPIO_RCC_ENABLE;
    //端口重映射
    BSP_EXTFLASH_SPI_MOSI_REMAP;
    BSP_EXTFLASH_SPI_MISO_REMAP;
    BSP_EXTFLASH_SPI_SCLK_REMAP;
    BSP_EXTFLASH_SPI_CS_REMAP;
    //初始化GPIO
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_DOWN;
#endif
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin     =  BSP_EXTFLASH_SPI_MOSI_PIN;
    GPIO_Init(BSP_EXTFLASH_SPI_MOSI_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_EXTFLASH_SPI_MISO_PIN;
    GPIO_Init(BSP_EXTFLASH_SPI_MISO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin     =  BSP_EXTFLASH_SPI_SCLK_PIN;
    GPIO_Init(BSP_EXTFLASH_SPI_SCLK_PORT, &GPIO_InitStructure);

#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Mode    =  GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType   =  GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    =  GPIO_PuPd_NOPULL;
#endif
    GPIO_InitStructure.GPIO_Speed   =  GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin     =  BSP_EXTFLASH_SPI_CS_PIN;
    GPIO_Init(BSP_EXTFLASH_SPI_CS_PORT, &GPIO_InitStructure);
    BSP_EXTFLASH_SPI_CS_HIGH;
#ifdef BSP_EXTFLASH_SPI_WP_PORT
    GPIO_InitStructure.GPIO_Pin     =  BSP_EXTFLASH_SPI_WP_PIN;
    GPIO_Init(BSP_EXTFLASH_SPI_WP_PORT, &GPIO_InitStructure);
#endif
    BSP_EXTFLASH_SPI_WP_LOW;
    //初始化SPI
    SPI_InitStructure.SPI_Direction =  SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode      =  SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize  =  SPI_DataSize_8b;
    //-----
    //之前用SST25中使用
    SPI_InitStructure.SPI_CPOL      =  SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA      =  SPI_CPHA_1Edge;
    //用于W25系列,但是初步试验SST25系列也好使
    /*
    SPI_InitStructure.SPI_CPOL      =  SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA      =  SPI_CPHA_2Edge;
    */
    //-----
    SPI_InitStructure.SPI_NSS       =  SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler  =  SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit  =  SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial      =  7;
    SPI_Init(BSP_EXTFLASH_SPIX, &SPI_InitStructure);
    SPI_Cmd(BSP_EXTFLASH_SPIX, DISABLE);
    SPI_Cmd(BSP_EXTFLASH_SPIX, ENABLE);
#elif (defined(NRF51)||defined(NRF52))
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    static uint8_t first=1;
    if(first==0)
    {
        return;
    }
    first=0;
    nrf_gpio_cfg_output(BSP_EXTFLASH_SPI_CS_PIN);
    BSP_EXTFLASH_SPI_CS_HIGH;
    spi_config.miso_pin = BSP_EXTFLASH_SPI_MISO_PIN;
    spi_config.mosi_pin = BSP_EXTFLASH_SPI_MOSI_PIN;
    spi_config.sck_pin  = BSP_EXTFLASH_SPI_SCLK_PIN;
    APP_ERROR_CHECK(nrf_drv_spi_init(&BspExFlash_Spi, &spi_config, NULL));
#endif
    //-----器件识别
    {
        memset((char*)&BspExtFlash_s_Flash,0,sizeof(BspExtFlash_s_Flash));
        BSP_EXTFLASH_SPI_CS_LOW;
        Bsp_ExtFlash_RW_Byte(CMD_RDID);
        BspExtFlash_s_Flash.ChipID  +=    Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
        BspExtFlash_s_Flash.ChipID  <<=   8;
        BspExtFlash_s_Flash.ChipID  +=    Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
        BspExtFlash_s_Flash.ChipID  <<=   8;
        BspExtFlash_s_Flash.ChipID  +=    Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
        BSP_EXTFLASH_SPI_CS_HIGH;
        switch(BspExtFlash_s_Flash.ChipID)
        {
            case BSP_EXTFLASH_SST25VF016B_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "SST25VF016B");
                BspExtFlash_s_Flash.TotalSize   =  2 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_SST25VF064C_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "SST25VF064C");
                BspExtFlash_s_Flash.TotalSize   =  8 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_MX25L1606E_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "MX25L1606E");
                BspExtFlash_s_Flash.TotalSize   =  2 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_W25Q80DV_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "W25Q80DV");
                BspExtFlash_s_Flash.TotalSize   =  1 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_W25Q16BV_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "W25Q16BV");
                BspExtFlash_s_Flash.TotalSize   =  2 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_W25Q32BV_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "W25Q32BV");
                BspExtFlash_s_Flash.TotalSize   =  4 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            case BSP_EXTFLASH_W25Q64BV_ID:
                strcpy(BspExtFlash_s_Flash.ChipName, "W25Q64BV");
                BspExtFlash_s_Flash.TotalSize   =  8 * 1024 * 1024;
                BspExtFlash_s_Flash.PageSize    =  4 * 1024;
                break;
            default:
                strcpy(BspExtFlash_s_Flash.ChipName, "Unknown Flash");
                BspExtFlash_s_Flash.TotalSize   =  0;
                BspExtFlash_s_Flash.PageSize    =  0;
                break;
        }
    }
}

/*******************************************************************************
* 函数功能: 通过硬件SPI发送/接收一个字节
* 参    数: data: 发送数据
* 返 回 值: SST25VF064B 返回的数据
*******************************************************************************/
static uint8_t Bsp_ExtFlash_RW_Byte(uint8_t data)
{
#if   (defined(STM32F1)||defined(STM32F4))
    //Loop while DR register in not emplty
    while (SPI_I2S_GetFlagStatus(BSP_EXTFLASH_SPIX, SPI_I2S_FLAG_TXE) == RESET);
    //Send byte through the SPI2 peripheral
    SPI_I2S_SendData(BSP_EXTFLASH_SPIX, data);
    //Wait to receive a byte
    while (SPI_I2S_GetFlagStatus(BSP_EXTFLASH_SPIX, SPI_I2S_FLAG_RXNE) == RESET);
    //Return the byte read from the SPI bus
    return SPI_I2S_ReceiveData(BSP_EXTFLASH_SPIX);
#elif (defined(NRF51)||defined(NRF52))
    uint8_t i;
    nrf_drv_spi_transfer(&BspExFlash_Spi,&data,sizeof(data),&i,sizeof(i));
    return i;
#endif
}
/*******************************************************************************
* 函数功能: 读函数
* 参    数: Dst      --- 目标地址0x0 - MAX_ADDR
*           RcvBufPt --- 接收缓存的指针
*           NByte    --- 要读取的数据字节数
* Return  : OK/ERR
*******************************************************************************/
uint8_t Bsp_ExtFlash_Read_nByte(uint32_t Dst, uint8_t* RcvBufPt ,uint32_t NByte)
{
    uint32_t i = 0;
    //检查入口参数
    if ((Dst+NByte > BspExtFlash_s_Flash.TotalSize)||(NByte == 0))
        return (ERR);
    BSP_EXTFLASH_SPI_CS_LOW;
    //-----XSL-----修改
    //发送读命令
    //Flash_ReadWriteByte(0x0B);
    Bsp_ExtFlash_RW_Byte(0x03);
    //-------------
    //发送地址信息:该地址由3个字节组成
    Bsp_ExtFlash_RW_Byte(((Dst & 0xFFFFFF) >> 16));
    Bsp_ExtFlash_RW_Byte(((Dst & 0xFFFF) >> 8));
    Bsp_ExtFlash_RW_Byte(Dst & 0xFF);
    //-----XSL-----删除
    //发送一个哑字节以读取数据
    //Flash_ReadWriteByte(0xFF);
    //-------------
    for (i = 0; i < NByte; i++)
    {
        RcvBufPt[i] = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
    }
    BSP_EXTFLASH_SPI_CS_HIGH;
    return (OK);
}
/*******************************************************************************
* 函数功能: SST25VF064B的读ID函数,可选择读ID和读数据操作
* 参    数: IDType   ---   ID类型。用户可在Jedec_ID,Dev_ID,Manu_ID三者里选择
* 输    出: RcvbufPt ---   存储ID变量的指针
* 返 回 值: 操作成功则返回OK,失败则返回ERROR
*******************************************************************************/
/*
static INT8U SSTF016B_RdID(idtype IDType,INT32U* RcvbufPt)
{
    INT32U temp = 0;

    if (IDType == Jedec_ID)
    {
        SPI_FLASH_CS_LOW();
        //发送读JEDEC ID命令(9Fh)
        Flash_ReadWriteByte(0x9F);
        //接收数据
        temp = (temp | Flash_ReadWriteByte(0x00)) << 8;
        temp = (temp | Flash_ReadWriteByte(0x00)) << 8;
        //在本例中,temp的值应为0xBF2541
        temp = (temp | Flash_ReadWriteByte(0x00));
        SPI_FLASH_CS_HIGH();
        *RcvbufPt = temp;
        return (OK);
    }
    if ((IDType == Manu_ID) || (IDType == Dev_ID) )
    {
        SPI_FLASH_CS_LOW();
        //发送读ID命令 (90h or ABh)
        Flash_ReadWriteByte(0x90);
        //发送地址
        Flash_ReadWriteByte(0x00);
        //发送地址
        Flash_ReadWriteByte(0x00);
        //发送地址 - 不是00H就是01H
        Flash_ReadWriteByte(IDType);
        //接收获取的数据字节
        temp = Flash_ReadWriteByte(0x00);
        SPI_FLASH_CS_HIGH();
        *RcvbufPt = temp;
        return (OK);
    }
    else
    {
        return (ERR);
    }
}
*/

/*******************************************************************************
* 函数功能: SST25VF064B的写函数，可写1个和多个数据到指定地址
* 参    数: Dst      ---   目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）
*           SndbufPt ---   发送缓存区指针
*           NByte    ---   要写的数据字节数
* 返 回 值: 操作成功则返回OK,失败则返回ERR
*******************************************************************************/
static uint8_t Bsp_ExtFlash_PageWrite_nByte(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte)
{
    uint8_t StatRgVal = 0;
    uint32_t i32 = 0;
    if ((Dst+NByte > BspExtFlash_s_Flash.TotalSize)||(NByte == 0))
    {
        return (ERR);
    }
    //写保护关闭
    BSP_EXTFLASH_SPI_WP_HIGH;
    //-----XSL-----删除
    /*
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送读状态寄存器命令
    Bsp_ExtFlash_RW_Byte(0x05);
    //保存读得的状态寄存器值
    temp = Bsp_ExtFlash_RW_Byte(0xFF);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //使状态寄存器可写
    Bsp_ExtFlash_RW_Byte(0x50);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写状态寄存器指令
    Bsp_ExtFlash_RW_Byte(0x01);
    //清0BPx位，使Flash芯片全区可写
    Bsp_ExtFlash_RW_Byte(0);
    BSP_EXTFLASH_SPI_CS_HIGH;
    */
    //-------------
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写使能命令
    Bsp_ExtFlash_RW_Byte(0x06);
    BSP_EXTFLASH_SPI_CS_HIGH;
    //
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送字节数据烧写命令
    Bsp_ExtFlash_RW_Byte(0x02);
    //发送3个字节的地址信息
    Bsp_ExtFlash_RW_Byte((((Dst+i32) & 0xFFFFFF) >> 16));
    Bsp_ExtFlash_RW_Byte((((Dst+i32) & 0xFFFF) >> 8));
    Bsp_ExtFlash_RW_Byte((Dst+i32) & 0xFF);
    for(i32 = 0; i32 < NByte; i32++)
    {
        //发送被烧写的数据
        Bsp_ExtFlash_RW_Byte(SndbufPt[i32]);
    }
    BSP_EXTFLASH_SPI_CS_HIGH;
    do
    {
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送读状态寄存器命令
        Bsp_ExtFlash_RW_Byte(0x05);
        //保存读得的状态寄存器值
        StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
        BSP_EXTFLASH_SPI_CS_HIGH;
    }
    //一直等待，直到芯片空闲
    while (StatRgVal == 0x03 );
    //-----XSL-----删除
    /*
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写使能命令
    Bsp_ExtFlash_RW_Byte(0x06);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //使状态寄存器可写
    Bsp_ExtFlash_RW_Byte(0x50);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写状态寄存器指令
    Bsp_ExtFlash_RW_Byte(0x01);
    //恢复状态寄存器设置信息
    Bsp_ExtFlash_RW_Byte(temp);
    BSP_EXTFLASH_SPI_CS_HIGH;
    */
    //写保护使能
    BSP_EXTFLASH_SPI_WP_LOW;
    //-------------
    return (OK);
}
//
uint8_t Bsp_ExtFlash_Write_nByte(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte)
{
    uint32_t begin_addr;
    uint32_t byte_num=0;
    if ((Dst+NByte > BspExtFlash_s_Flash.TotalSize)||(NByte == 0))
    {
        return (ERR);
    }

    //前
    begin_addr =  Dst;
    byte_num   =  256-(Dst%256);
    if(byte_num>NByte)
    {
        byte_num =  NByte;
    }
    if(byte_num!=0)
    {
        Bsp_ExtFlash_PageWrite_nByte(begin_addr,SndbufPt,byte_num);
        SndbufPt     += byte_num;
        begin_addr   += byte_num;
    }
    //中
    byte_num   =  ((NByte -  byte_num)/256);
    while(1)
    {
        if(byte_num==0)
        {
            break;
        }
        Bsp_ExtFlash_PageWrite_nByte(begin_addr,SndbufPt,256);
        SndbufPt     += 256;
        begin_addr   += 256;
        byte_num--;
    }
    //后
    byte_num   =  NByte-(begin_addr-Dst);
    if(byte_num!=0)
    {
        Bsp_ExtFlash_PageWrite_nByte(begin_addr,SndbufPt,byte_num);
    }
    //
    return (OK);
}
/*******************************************************************************
* 函数功能: 根据指定的扇区号选取最高效的算法擦除
* 参    数: sec1  ---   起始扇区号,范围(0~SEC_MAX)
*           sec2  ---   终止扇区号,范围(0~SEC_MAX)
* 返 回 值: 操作成功则返回OK,失败则返回ERR
*******************************************************************************/
uint8_t Bsp_ExtFlash_Erase(uint32_t addr1, uint32_t addr2)
{
    uint16_t sec1=addr1/BspExtFlash_s_Flash.PageSize,sec2=addr2/BspExtFlash_s_Flash.PageSize;
    uint8_t  temp1 = 0,temp2 = 0,StatRgVal = 0;
    uint32_t SecnHdAddr = 0;
    //要擦除的扇区数目
    uint32_t no_SecsToEr = 0;
    //当前要擦除的扇区号
    uint32_t CurSecToEr = 0;
    //检查入口参数
    if ((sec1 > (BspExtFlash_s_Flash.TotalSize/BspExtFlash_s_Flash.PageSize-1))||(sec2 > (BspExtFlash_s_Flash.TotalSize/BspExtFlash_s_Flash.PageSize-1)))
    {
        return (ERR);
    }
    //
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送读状态寄存器命令
    Bsp_ExtFlash_RW_Byte(0x05);
    //保存读得的状态寄存器值
    temp1 = Bsp_ExtFlash_RW_Byte(0x00);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //使状态寄存器可写
    Bsp_ExtFlash_RW_Byte(0x50);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写状态寄存器指令
    Bsp_ExtFlash_RW_Byte(0x01);
    //清0BPx位，使Flash芯片全区可写
    Bsp_ExtFlash_RW_Byte(0);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写使能命令
    Bsp_ExtFlash_RW_Byte(0x06);
    BSP_EXTFLASH_SPI_CS_HIGH;

    //如果用户输入的起始扇区号大于终止扇区号，则在内部作出调整
    if (sec1 > sec2)
    {
        temp2 = sec1;
        sec1  = sec2;
        sec2  = temp2;
    }
    //-----单扇区擦除解析
    if (sec1 == sec2)
    {
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送写使能命令
        Bsp_ExtFlash_RW_Byte(0x06);
        BSP_EXTFLASH_SPI_CS_HIGH;
        //计算扇区的起始地址
        SecnHdAddr = BspExtFlash_s_Flash.PageSize * sec1;
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送扇区擦除指令
        Bsp_ExtFlash_RW_Byte(CMD_ERASE_S);
        //发送3个字节的地址信息
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Bsp_ExtFlash_RW_Byte(SecnHdAddr & 0xFF);
        BSP_EXTFLASH_SPI_CS_HIGH;
        do
        {
            //延时
#ifndef BOOTLOADER
            MODULE_OS_DELAY_MS(10);
#endif
            //
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送读状态寄存器命令
            Bsp_ExtFlash_RW_Byte(0x05);
            //保存读得的状态寄存器值
            StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
            BSP_EXTFLASH_SPI_CS_HIGH;
        }
        //一直等待，直到芯片空闲
        while (StatRgVal == 0x03);
        return (OK);
    }
    //-----整片扇区擦除使能
    if (sec2 - sec1 == (BspExtFlash_s_Flash.TotalSize/BspExtFlash_s_Flash.PageSize-1))
    {
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送芯片擦除指令(60h or C7h)
        Bsp_ExtFlash_RW_Byte(0x60);
        BSP_EXTFLASH_SPI_CS_HIGH;
        do
        {
            //延时
#ifndef BOOTLOADER
            MODULE_OS_DELAY_MS(10);
#endif
            //
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送读状态寄存器命令
            Bsp_ExtFlash_RW_Byte(0x05);
            //保存读得的状态寄存器值
            StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
            BSP_EXTFLASH_SPI_CS_HIGH;
        }
        //一直等待，直到芯片空闲
        while (StatRgVal == 0x03);
        return (OK);
    }
    //-----任意扇区
    //获取要擦除的扇区数目 (no_SecsToEr)
    no_SecsToEr = sec2 - sec1 +1;
    //从起始扇区开始擦除   (CurSecToEr)
    CurSecToEr  = sec1;
    //特殊处理前部多余扇区
    while (CurSecToEr%8 && no_SecsToEr!=0)
    {
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送写使能命令
        Bsp_ExtFlash_RW_Byte(0x06);
        BSP_EXTFLASH_SPI_CS_HIGH;
        //计算扇区的起始地址
        SecnHdAddr = BspExtFlash_s_Flash.PageSize * CurSecToEr;
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送扇区擦除指令
        Bsp_ExtFlash_RW_Byte(CMD_ERASE_S);
        //发送3个字节的地址信息
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Bsp_ExtFlash_RW_Byte(SecnHdAddr & 0xFF);
        BSP_EXTFLASH_SPI_CS_HIGH;
        do
        {
            //延时
#ifndef BOOTLOADER
            MODULE_OS_DELAY_MS(10);
#endif
            //
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送读状态寄存器命令
            Bsp_ExtFlash_RW_Byte(0x05);
            //保存读得的状态寄存器值
            StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
            BSP_EXTFLASH_SPI_CS_HIGH;
        }
        //一直等待，直到芯片空闲
        while (StatRgVal == 0x03);
        CurSecToEr  += 1;
        no_SecsToEr -=  1;
    }
    //特殊处理中间整块扇区(采取8扇区擦除算法)
    while (no_SecsToEr >= 8)
    {
        if(no_SecsToEr>=16 && (CurSecToEr%16==0))
        {
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送写使能命令
            Bsp_ExtFlash_RW_Byte(0x06);
            BSP_EXTFLASH_SPI_CS_HIGH;
            //计算扇区的起始地址
            SecnHdAddr = BspExtFlash_s_Flash.PageSize * CurSecToEr;
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送64KB擦除指令
            Bsp_ExtFlash_RW_Byte(CMD_ERASE_64K);
            //发送3个字节的地址信息
            Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));
            Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFF) >> 8));
            Bsp_ExtFlash_RW_Byte(SecnHdAddr & 0xFF);
            BSP_EXTFLASH_SPI_CS_HIGH;
            do
            {
                //延时
#ifndef BOOTLOADER
                MODULE_OS_DELAY_MS(10);
#endif
                //
                BSP_EXTFLASH_SPI_CS_LOW;
                //发送读状态寄存器命令
                Bsp_ExtFlash_RW_Byte(0x05);
                //保存读得的状态寄存器值
                StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
                BSP_EXTFLASH_SPI_CS_HIGH;
            }
            //一直等待，直到芯片空闲
            while (StatRgVal == 0x03);
            CurSecToEr  += 16;
            no_SecsToEr -=  16;
        }
        else
        {
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送写使能命令
            Bsp_ExtFlash_RW_Byte(0x06);
            BSP_EXTFLASH_SPI_CS_HIGH;
            //计算扇区的起始地址
            SecnHdAddr = BspExtFlash_s_Flash.PageSize * CurSecToEr;
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送32KB擦除指令
            Bsp_ExtFlash_RW_Byte(CMD_ERASE_32K);
            //发送3个字节的地址信息
            Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));
            Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFF) >> 8));
            Bsp_ExtFlash_RW_Byte(SecnHdAddr & 0xFF);
            BSP_EXTFLASH_SPI_CS_HIGH;
            do
            {
                //延时
#ifndef BOOTLOADER
                MODULE_OS_DELAY_MS(10);
#endif
                //
                BSP_EXTFLASH_SPI_CS_LOW;
                //发送读状态寄存器命令
                Bsp_ExtFlash_RW_Byte(0x05);
                //保存读得的状态寄存器值
                StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
                BSP_EXTFLASH_SPI_CS_HIGH;
            }
            //一直等待，直到芯片空闲
            while (StatRgVal == 0x03);
            CurSecToEr  += 8;
            no_SecsToEr -=  8;
        }
    }
    //特殊处理尾部多余扇区 采用扇区擦除算法擦除剩余的扇区
    while (no_SecsToEr >= 1)
    {
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送写使能命令
        Bsp_ExtFlash_RW_Byte(0x06);
        BSP_EXTFLASH_SPI_CS_HIGH;
        //计算扇区的起始地址
        SecnHdAddr = BspExtFlash_s_Flash.PageSize * CurSecToEr;
        BSP_EXTFLASH_SPI_CS_LOW;
        //发送扇区擦除指令
        Bsp_ExtFlash_RW_Byte(CMD_ERASE_S);
        //发送3个字节的地址信息
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFFFF) >> 16));
        Bsp_ExtFlash_RW_Byte(((SecnHdAddr & 0xFFFF) >> 8));
        Bsp_ExtFlash_RW_Byte(SecnHdAddr & 0xFF);
        BSP_EXTFLASH_SPI_CS_HIGH;
        do
        {
            //延时
#ifndef BOOTLOADER
            MODULE_OS_DELAY_MS(10);
#endif
            //
            BSP_EXTFLASH_SPI_CS_LOW;
            //发送读状态寄存器命令
            Bsp_ExtFlash_RW_Byte(0x05);
            //保存读得的状态寄存器值
            StatRgVal = Bsp_ExtFlash_RW_Byte(CMD_DUMMY);
            BSP_EXTFLASH_SPI_CS_HIGH;
        }
        //一直等待，直到芯片空闲
        while (StatRgVal == 0x03);
        CurSecToEr  += 1;
        no_SecsToEr -=  1;
    }
    //擦除结束,恢复状态寄存器信息
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写使能命令
    Bsp_ExtFlash_RW_Byte(0x06);
    BSP_EXTFLASH_SPI_CS_HIGH;

    BSP_EXTFLASH_SPI_CS_LOW;
    //使状态寄存器可写
    Bsp_ExtFlash_RW_Byte(0x50);
    BSP_EXTFLASH_SPI_CS_HIGH;
    BSP_EXTFLASH_SPI_CS_LOW;
    //发送写状态寄存器指令
    Bsp_ExtFlash_RW_Byte(0x01);
    //恢复状态寄存器设置信息
    Bsp_ExtFlash_RW_Byte(temp1);
    BSP_EXTFLASH_SPI_CS_HIGH;
    return (OK);
}


/*******************************************************************************
* 函数功能: 测试SST25VF016B ID
*******************************************************************************/
/*
static void SPI_FLASH_Test(void)
{
    INT32U  ChipID = 0;
    SSTF016B_RdID(Jedec_ID, &ChipID);
    ChipID &= ~0xff000000;
    if (ChipID != 0xBF2541)
    {
        while(1);
    }
}
*/
//-------------------------------------------------------------------------------输出函数
#ifndef BOOTLOADER
void Bsp_ExtFlash_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf=NULL;
    uint16_t *pbuf1=NULL;
    uint16_t i16=0;
    uint32_t i32=0;
    //初始化
    Bsp_ExtFlash_Init();
    //申请缓存
    pbuf    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if   (MEM_5KB_BASIC_BLK_NBR==0)
    pbuf1   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#else
    pbuf1   =  MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
#endif
    //芯片信息
    sprintf((char*)pbuf,"\r\nFlash-ID:0x%lX\r\n",BspExtFlash_s_Flash.ChipID);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"Flash-Name:%s\r\n",BspExtFlash_s_Flash.ChipName);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"Flash-TotalSize:%ld\r\n",BspExtFlash_s_Flash.TotalSize);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"Flash-PageSize:%d\r\n",BspExtFlash_s_Flash.PageSize);
    DebugOutStr((int8_t*)pbuf);
    MODULE_OS_DELAY_MS(5000);
    if(BspExtFlash_s_Flash.TotalSize==0)
    {
        goto goto_BspExtFlashDebugTestOnOff;
    }
    //整片擦除
    Bsp_ExtFlash_Erase(0,BspExtFlash_s_Flash.TotalSize-1);
    //
    i32=0;
    while(i32<BspExtFlash_s_Flash.TotalSize)
    {
#if   (MEM_5KB_BASIC_BLK_NBR==0)
        //写
        for(i16=0; i16<256/2; i16++)
        {
            pbuf1[i16]  =  i16;
        }
        Bsp_ExtFlash_Write_nByte(i32,(uint8_t *)pbuf1,256);
        //读
        memset((char*)pbuf1,0,256);
        Bsp_ExtFlash_Read_nByte(i32,(uint8_t *)pbuf1,256);
        //校验
        for(i16=0; i16<256/2; i16++)
        {
            if(i16==pbuf1[i16])
                continue;
            else
                break;
        }
        if(i16==256/2)
        {
            //测试成功
            sprintf((char*)pbuf,"\r\nAddr:%ld OK",i32);
            DebugOutStr((int8_t*)pbuf);
            i32+=256;
        }
        else
        {
            //测试失败
            sprintf((char*)pbuf,"\r\nAddr:%ld ERR",i32);
            DebugOutStr((int8_t*)pbuf);
            break;
        }
#else
        //写
        for(i16=0; i16<BspExtFlash_s_Flash.PageSize/2; i16++)
        {
            pbuf1[i16]  =  i16;
        }
        Bsp_ExtFlash_Write_nByte(i32,(uint8_t *)pbuf1,BspExtFlash_s_Flash.PageSize);
        //读
        memset((char*)pbuf1,0,BspExtFlash_s_Flash.PageSize);
        Bsp_ExtFlash_Read_nByte(i32,(uint8_t *)pbuf1,BspExtFlash_s_Flash.PageSize);
        //校验
        for(i16=0; i16<BspExtFlash_s_Flash.PageSize/2; i16++)
        {
            if(i16==pbuf1[i16])
                continue;
            else
                break;
        }
        if(i16==BspExtFlash_s_Flash.PageSize/2)
        {
            //测试成功
            sprintf((char*)pbuf,"\r\nAddr:%ld OK",i32);
            DebugOutStr((int8_t*)pbuf);
            i32+=BspExtFlash_s_Flash.PageSize;
        }
        else
        {
            //测试失败
            sprintf((char*)pbuf,"\r\nAddr:%ld ERR",i32);
            DebugOutStr((int8_t*)pbuf);
            break;
        }
#endif
    }
    if(i32>=BspExtFlash_s_Flash.TotalSize)
    {
        sprintf((char*)pbuf,"\r\nFLASH(SPI) Test OK\r\n");
    }
    else
    {
        sprintf((char*)pbuf,"\r\nFLASH(SPI) Test ERR\r\n");
    }
    DebugOutStr((int8_t*)pbuf);
goto_BspExtFlashDebugTestOnOff:
    //整片擦除
    Bsp_ExtFlash_Erase(0,BspExtFlash_s_Flash.TotalSize-1);
    sprintf((char*)pbuf,"\r\nFLASH(SPI) EARSE!\r\n");
    DebugOutStr((int8_t*)pbuf);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#if   (MEM_5KB_BASIC_BLK_NBR==0)
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf1);
#else
    MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pbuf1);
#endif
}
#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

