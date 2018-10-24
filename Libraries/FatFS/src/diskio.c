/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"         /* FatFs lower layer API */
#include "Bsp_Rtc.h"
#if   (defined(STM32F1))
//---->
#include "Bsp_SdioSd_F1.h"
//<----
#elif (defined(STM32F4))
//---->
#include "Bsp_SdioSd_F4.h"
//<----
#elif (defined(NRF51822)||defined(NRF52832))
//---->
/*
#include "Bsp_SpiSD.h"
*/
#include "diskio_blkdev.h"
/**
 * @brief Registered drives array.
 * */
static diskio_blkdev_t * m_drives;

/**
 * @brief Number of registered drives.
 * */
static BYTE m_drives_count;
/**
 * @brief Block device handler.
 *
 * @ref nrf_block_dev_ev_handler
 * */
static void block_dev_handler(struct nrf_block_dev_s const * p_blk_dev,
                              nrf_block_dev_event_t const *  p_event)
{
    uint8_t drv = (uint8_t)(uint32_t) p_event->p_context;
    ASSERT(drv < m_drives_count);

    switch (p_event->ev_type)
    {
        case NRF_BLOCK_DEV_EVT_INIT:
        case NRF_BLOCK_DEV_EVT_UNINIT:
        case NRF_BLOCK_DEV_EVT_BLK_WRITE_DONE:
        case NRF_BLOCK_DEV_EVT_BLK_READ_DONE:
            m_drives[drv].last_result = p_event->result;
            m_drives[drv].busy = false;
            break;
        default:
            break;
    }
}
/**
 * @brief Default IO operation wait function.
 * */
static void default_wait_func(void)
{
    __WFE();
}
//<----
#endif
//-----------------------------------------------------------------------
#define SECTOR_SIZE     512
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber (0..) */
)
{
    DSTATUS stat = STA_NOINIT;
    switch (pdrv)
    {
        case FS_SD :    /* SD卡 */
#if		(defined(STM32F1)||defined(STM32F4))
            if (SD_Init() == SD_OK)
            {
                stat = RES_OK;
            }
            else
            {
                stat = STA_NODISK;
            }
            break;
#elif	(defined(NRF51)||defined(NRF52))
						/*
						stat = SD_Initialize();//SD_Initialize() 
						if(stat)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
						{
								SD_SPI_SpeedLow();
								SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
								SD_SPI_SpeedHigh();
						}
						*/
#endif
        case FS_NAND :  /* NAND Flash */
            break;
        case FS_USB :   /* STM32 USB Host 口外接U盘 */
            break;
        case FS_SPI :   /* SPI Flash */
            break;
        case FS_NOR :   /* NOR Flash */
            break;
        default :
            break;
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber (0..) */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (pdrv)
    {
        case FS_SD :    /* SD卡 */
            stat = 0;
            break;
        case FS_NAND :  /* NAND Flash */
            stat = 0;
            break;
        case FS_USB :   /* STM32 USB Host 口外接U盘 */
            stat = 0;
            break;
        case FS_SPI :   /* SPI Flash */
            stat = 0;
            break;
        case FS_NOR :   /* NOR Flash */
            stat = 0;
            break;
        default:
            stat = 0;
            break;
    }
    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address (LBA) */
    BYTE count      /* Number of sectors to read (1..128) */
)
{
    DRESULT res;
		//count不能等于0，否则返回参数错误	
		if (!count)return RES_PARERR;
		//
    switch (pdrv)
    {
        case FS_SD :    /* SD卡 */
#if	(defined(STM32F1)||defined(STM32F4))
        {
            SD_Error Status = SD_OK;

            if (count == 1)
            {
                Status = SD_ReadBlock(buff, sector << 9 , SECTOR_SIZE);
            }
            else
            {
                Status = SD_ReadMultiBlocks(buff, sector << 9 , SECTOR_SIZE, count);
            }
            if (Status != SD_OK)
            {
                res = RES_ERROR;
                break;
            }
#if   (defined(STM32F4))
#ifdef SD_DMA_MODE
            /* SDIO工作在DMA模式，需要检查操作DMA传输是否完成 */
            Status = SD_WaitReadOperation();
            if (Status != SD_OK)
            {
                res = RES_ERROR;
                break;
            }

            while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
#endif						
            res = RES_OK;
            break;
        }
#elif		(defined(NRF51)||defined(NRF52))
				/*
				{
						uint8_t i=0;
						i=SD_ReadDisk(buff,sector,count);	 
						if(i)//STM32 SPI的bug,在sd卡操作失败的时候如果不执行下面的语句,可能导致SPI读写异常
						{
								SD_SPI_SpeedLow();
								SD_SPI_ReadWriteByte(0xff);//提供额外的8个时钟
								SD_SPI_SpeedHigh();
						}
						if(i==0)
						{
								res	=	RES_OK;
						}
						else
						{
								res	=	RES_ERROR;
						}
				}
				*/
#endif
        case FS_NAND :  /* NAND Flash */
            break;
        case FS_USB :   /* STM32 USB Host 口外接U盘 */
            break;
        case FS_SPI :   /* SPI Flash */
            break;
        case FS_NOR :   /* NOR Flash */
            break;
        default:
						res = RES_PARERR;
            break;
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber (0..) */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address (LBA) */
    BYTE count          /* Number of sectors to write (1..128) */
)
{
    DRESULT res;
		//count不能等于0，否则返回参数错误	
		if (!count)return RES_PARERR;
    switch (pdrv)
    {
        case FS_SD :    /* SD卡 */
#if	(defined(STM32F1)||defined(STM32F4))
        {
            SD_Error Status = SD_OK;
            if (count == 1)
            {
                Status = SD_WriteBlock((uint8_t *)buff, sector << 9 ,SECTOR_SIZE);
            }
            else
            {
                Status = SD_WriteMultiBlocks((uint8_t *)buff, sector << 9 ,SECTOR_SIZE, count);
            }
            if (Status != SD_OK)
            {
                res =  RES_ERROR;
            }
#if   (defined(STM32F4))
#ifdef SD_DMA_MODE
            /* SDIO工作在DMA模式，需要检查操作DMA传输是否完成 */
            Status = SD_WaitReadOperation();
            if (Status != SD_OK)
            {
                res =  RES_ERROR;
            }

            while(SD_GetStatus() != SD_TRANSFER_OK);
#endif
#endif
            while(SD_GetStatus() != SD_TRANSFER_OK);

            res =  RES_OK;
            break;
        }
#elif		(defined(NRF51)||defined(NRF52))
				/*
				{
						uint8_t i;
						i=SD_WriteDisk((uint8_t*)buff,sector,count);
						if(i==0)
						{
								res	=	RES_OK;
						}
						else
						{
								res	=	RES_ERROR;
						}
				}
				*/
#endif
        case FS_NAND :     /* NAND Flash */
            break;
        case FS_USB :      /* STM32 USB Host 口外接U盘 */
            break;
        case FS_SPI :      /* SPI Flash */
            break;
        case FS_NOR :      /* NOR Flash */
            break;
        default:
            res = RES_PARERR;
            break;
    }
    return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;

    switch (pdrv)
    {
        case FS_SD :
#if			(defined(STM32F1)||defined(STM32F4))					
            /* SD卡磁盘容量： SDCardInfo.CardCapacity */
            res = RES_OK;
#elif		(defined(NRF51)||defined(NRF52))
						/*
						switch(cmd)
						{
								case CTRL_SYNC:
											SD_CS_0;
											if(SD_WaitReady()==0)res = RES_OK; 
											else res = RES_ERROR;	  
											SD_CS_1;
											break;	 
								case GET_SECTOR_SIZE:
											*(WORD*)buff = 512;
											res = RES_OK;
											break;	 
								case GET_BLOCK_SIZE:
											*(WORD*)buff = 8;
											res = RES_OK;
											break;	 
								case GET_SECTOR_COUNT:
											*(DWORD*)buff = SD_GetSectorCount();
											res = RES_OK;
											break;
								default:
											res = RES_PARERR;
											break;
						}
						*/
#endif
        case FS_NAND :
            break;

        case FS_USB :

            break;

        case FS_SPI :       /* SPI Flash */
						
            break;

        case FS_NOR :       /* NOR Flash */

            break;
				default:
						res	=	RES_PARERR;
						break;
    }
    return res;
}
#endif

/*
*********************************************************************************************************
*   函 数 名: get_fattime
*   功能说明: 获得系统时间，用于改写文件的创建和修改时间。
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
DWORD get_fattime (void)
{
#if 0
    /* 如果有全局时钟，可按下面的格式进行时钟转换. 这个例子是2013-01-01 00:00:00 */

    return    ((DWORD)(2013 - 1980) << 25)  /* Year = 2013 */
              | ((DWORD)1 << 21)              /* Month = 1 */
              | ((DWORD)1 << 16)              /* Day_m = 1*/
              | ((DWORD)0 << 11)              /* Hour = 0 */
              | ((DWORD)0 << 5)               /* Min = 0 */
              | ((DWORD)0 >> 1);              /* Sec = 0 */
#else
    uint8_t i;
    DWORD t=0;
    uint8_t time6buf[6];
#ifndef  BOOTLOADER
    BspRtc_ReadRealTime(NULL,NULL,NULL,time6buf);
#else
    time6buf[0]   =  16;
    time6buf[1]   =  2;
    time6buf[2]   =  5;
    time6buf[3]   =  15;
    time6buf[4]   =  30;
    time6buf[5]   =  00;
#endif
    //提取年信息
    i = time6buf[0] + (2000 - 1980);
    t += ((DWORD)i)<<25;
    //提取月信息
    i = time6buf[1];
    t += ((DWORD)i)<<21;
    //提取日信息
    i = time6buf[2];
    t += ((DWORD)i)<<16;
    //提取时信息
    i = time6buf[3];
    t += ((DWORD)i)<<11;
    //提取分信息
    i = time6buf[4];
    t += ((DWORD)i)<<5;
    //提取秒信息
    i = time6buf[5]/2;
    t += ((DWORD)i)<<0;

    return t;
#endif
}

