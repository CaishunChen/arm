/**
  ******************************************************************************
  * @file    Bsp_CpuFlash.c 
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
#include "includes.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_Rtc.h"
#include "Language.h"
#include "Bsp_BkpRam.h"
#include "uctsk_Debug.h"
#include "Bsp_CpuId.h"
//------------------------------- 用户变量 --------------------
/*
*********************************************************************************************************
*   函 数 名: bsp_GetSector
*   功能说明: 根据地址计算扇区首地址
*   形    参: 无
*   返 回 值: 扇区首地址
*********************************************************************************************************
*/
#if   (defined(STM32F4))
static uint32_t Bsp_CpuFlash_GetSector(uint32_t Address)
{
    uint32_t sector = 0;

    if((Address < BSP_CPU_FLASH_ADDR_SECTOR_1) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_0))
    {
        sector = FLASH_Sector_0;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_2) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_1))
    {
        sector = FLASH_Sector_1;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_3) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_2))
    {
        sector = FLASH_Sector_2;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_4) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_3))
    {
        sector = FLASH_Sector_3;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_5) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_4))
    {
        sector = FLASH_Sector_4;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_6) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_5))
    {
        sector = FLASH_Sector_5;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_7) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_6))
    {
        sector = FLASH_Sector_6;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_8) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_7))
    {
        sector = FLASH_Sector_7;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_9) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_8))
    {
        sector = FLASH_Sector_8;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_10) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_9))
    {
        sector = FLASH_Sector_9;
    }
    else if((Address < BSP_CPU_FLASH_ADDR_SECTOR_11) && (Address >= BSP_CPU_FLASH_ADDR_SECTOR_10))
    {
        sector = FLASH_Sector_10;
    }
    else    /*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
    {
        sector = FLASH_Sector_11;
    }

    return sector;
}
#elif (defined(NRF51)||defined(NRF52))
static void FLASH_ErasePage(uint32_t page_address)
{
    // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
void FLASH_Unlock(void)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
void FLASH_Lock(void)
{
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}
void FLASH_ProgramWord(uint32_t address, uint32_t value)
{
    *(__IO uint32_t*)address = value;
}
#endif
/*
*********************************************************************************************************
*   函 数 名: Bsp_CpuFlash_Init
*   功能说明: 如果备份区有数据,则把备份区的数据复制到参数区,防止操作过程中导致的数据丢失
*********************************************************************************************************
*/
void Bsp_CpuFlash_Init(void)
{
#ifdef BOOTLOADER
    return;
#else
    // ----------设置读保护
#if   (READ_PROTECT_ONOFF==ON)
#if   (defined(STM32F1))
    if (FLASH_GetReadOutProtectionStatus() == RESET)
    {
        FLASH_Unlock();
        FLASH_ReadOutProtection(ENABLE);
    }
#elif (defined(STM32F4))
    if(SET != FLASH_OB_GetRDP())
    {
        FLASH_Unlock();
        FLASH_OB_Unlock();
        //FLASH_OB_WRPConfig();
        FLASH_OB_RDPConfig(OB_RDP_Level_1);
        //FLASH_OB_UserConfig();
        //FLASH_OB_BORConfig();
        FLASH_OB_Launch();
        FLASH_OB_Lock();
        FLASH_Lock();
    }
#elif   (defined(NRF51)||defined(NRF52))
#endif
#endif
    // ----------备份区恢复
#if   (defined(STM32F1))
#elif (defined(STM32F4))
    //关中断
    __set_PRIMASK(1);
    // FLASH 解锁
    FLASH_Unlock();
    // Clear pending flags (if any)
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    // 需要擦除
    if (*(uint32_t *)BSP_CPU_FLASH_PARA_BACKUP_ADDR == 0x12345678 || *(uint32_t *)BSP_CPU_FLASH_PARA_BACKUP_ADDR == 0x87654321)
    {
        uint32_t i32;
        // 擦除参数区
        FLASH_EraseSector(Bsp_CpuFlash_GetSector(BSP_CPU_FLASH_PARA_ADDR), VoltageRange_1);
        // 复制备份区数据到参数区
        for (i32 = 0; i32 < (BSP_CPU_FLASH_PARA_BACKUP_ADDR-BSP_CPU_FLASH_PARA_ADDR); i32++)
        {
            FLASH_ProgramByte(BSP_CPU_FLASH_PARA_ADDR+i32, *(uint8_t *)(BSP_CPU_FLASH_PARA_BACKUP_ADDR+i32));
        }
        // 擦除备份区
        FLASH_EraseSector(Bsp_CpuFlash_GetSector(BSP_CPU_FLASH_PARA_BACKUP_ADDR), VoltageRange_1);
    }
    // Flash 加锁，禁止写Flash控制寄存器
    FLASH_Lock();
    // 开中断
    __set_PRIMASK(0);
#endif
#endif
}
/*
*********************************************************************************************************
*   函 数 名: bsp_CmpCpuFlash
*   功能说明: 比较Flash指定地址的数据.
*   形    参: _ulFlashAddr : Flash地址
*            _ucpBuf : 数据缓冲区
*            _ulSize : 数据大小（单位是字节）
*   返 回 值:
*           FLASH_IS_EQU        0   Flash内容和待写入的数据相等，不需要擦除和写操作
*           FLASH_REQ_WRITE     1   Flash不需要擦除，直接写
*           FLASH_REQ_ERASE     2   Flash需要先擦除,再写
*           FLASH_PARAM_ERR     3   函数参数错误
*********************************************************************************************************
*/
uint8_t Bsp_CpuFlash_Cmp(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
    uint32_t i;
    uint8_t ucIsEqu;
    uint8_t ucByte;

    // 如果偏移地址超过芯片容量，则不改写输出缓冲区
    if (_ulFlashAddr + _ulSize > BSP_CPU_FLASH_BASE_ADDR + BSP_CPU_FLASH_SIZE)
    {
        return BSP_CPU_FLASH_PARAM_ERR;
    }

    // 长度为0时返回正确
    if (_ulSize == 0)
    {
        return BSP_CPU_FLASH_IS_EQU;
    }

    ucIsEqu = 1;
    for (i = 0; i < _ulSize; i++)
    {
        ucByte = *(uint8_t *)_ulFlashAddr;

        if (ucByte != *_ucpBuf)
        {
            if (ucByte != 0xFF)
            {
                // 需要擦除后再写(可优化:可以判断是否可写)
                return BSP_CPU_FLASH_REQ_ERASE;
            }
            else
            {
                ucIsEqu = 0;
            }
        }

        _ulFlashAddr++;
        _ucpBuf++;
    }

    if (ucIsEqu == 1)
    {
        return BSP_CPU_FLASH_IS_EQU;
    }
    else
    {
        return BSP_CPU_FLASH_REQ_WRITE;
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_ReadCpuFlash
*   功能说明: 读取CPU Flash的内容
*   形    参:  _ucpDst : 目标缓冲区
*            _ulFlashAddr : 起始地址
*            _ulSize : 数据大小（单位是字节）
*   返 回 值: 0=成功，1=失败
*********************************************************************************************************
*/
uint8_t Bsp_CpuFlash_Read(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
    uint32_t i32;

    // 如果偏移地址超过芯片容量，则不改写输出缓冲区
    if (_ulFlashAddr + _ulSize > BSP_CPU_FLASH_BASE_ADDR + BSP_CPU_FLASH_SIZE)
    {
        return ERR;
    }

    // 长度为0时不继续操作,否则起始地址为奇地址会出错
    if (_ulSize == 0)
    {
        return ERR;
    }
    // 读取数据
    for (i32 = 0; i32 < _ulSize; i32++)
    {
        *_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
    }
    return OK;
}
/*
*********************************************************************************************************
*   功能说明: 擦除(页或扇区)
*   形    参: _ulFlashAddr : Flash地址
*            _ucpSrc : 数据缓冲区
*            _ulSize : 数据大小（单位是字节）
*   返 回 值: OK/ERR
*********************************************************************************************************
*/
void Bsp_CpuFlash_Erase(uint32_t FlashBeginAddr,uint32_t FlashEndAddr)
{
#if   (defined(STM32F1))
    FLASH_Unlock();
    if(FlashBeginAddr==FlashEndAddr)
    {
        FLASH_ErasePage(FlashBeginAddr);
    }
    else
    {
        while(FlashBeginAddr<FlashEndAddr)
        {
            FLASH_ErasePage(FlashBeginAddr);
            FlashBeginAddr+=(2*1024);
        }
    }
    FLASH_Lock();
#elif (defined(STM32F4))
    uint32_t sectorNum;
    //关中断
    __set_PRIMASK(1);
    // FLASH 解锁
    FLASH_Unlock();
    // Clear pending flags (if any)
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    if(FlashBeginAddr==FlashEndAddr)
    {
        FLASH_EraseSector(Bsp_CpuFlash_GetSector(FlashBeginAddr), VoltageRange_1);
    }
    else
    {
        while(FlashBeginAddr<FlashEndAddr)
        {
            sectorNum = Bsp_CpuFlash_GetSector(FlashBeginAddr);
            FLASH_EraseSector(sectorNum, VoltageRange_1);
            if(sectorNum==FLASH_Sector_0
               || sectorNum==FLASH_Sector_1
               || sectorNum==FLASH_Sector_2
               || sectorNum==FLASH_Sector_3 )
            {
                FlashBeginAddr+=(16*1024);
            }
            else if(sectorNum==FLASH_Sector_4)
            {
                FlashBeginAddr+=(64*1024);
            }
            else
            {
                FlashBeginAddr+=(128*1024);
            }
        }
    }
    // Flash 加锁，禁止写Flash控制寄存器
    FLASH_Lock();
    // 开中断
    __set_PRIMASK(0);
#elif (defined(NRF51)||defined(NRF52))
    if(FlashBeginAddr==FlashEndAddr)
    {
        FLASH_ErasePage(FlashBeginAddr);
    }
    else
    {
        while(FlashBeginAddr<FlashEndAddr)
        {
            FLASH_ErasePage(FlashBeginAddr);
            FlashBeginAddr+=NRF_FICR->CODEPAGESIZE;
        }
    }
#endif
}
/*
*********************************************************************************************************
*   功能说明: 写数据到CPU 内部Flash。
*   形    参: _ulFlashAddr : Flash地址
*            _ucpSrc : 数据缓冲区
*            _ulSize : 数据大小（单位是字节）
*   返 回 值: OK/ERR
*********************************************************************************************************
*/
uint8_t Bsp_CpuFlash_Write(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
    uint32_t i;
    uint8_t ucRet;

    // 如果偏移地址超过芯片容量，则不改写输出缓冲区
    if (_ulFlashAddr + _ulSize > BSP_CPU_FLASH_BASE_ADDR + BSP_CPU_FLASH_SIZE)
    {
        return ERR;
    }
    // 长度为0时不继续操作
    if (_ulSize == 0)
    {
        return OK;
    }
    ucRet = Bsp_CpuFlash_Cmp(_ulFlashAddr, _ucpSrc, _ulSize);
    if (ucRet == BSP_CPU_FLASH_IS_EQU)
    {
        return OK;
    }
#if   (defined(STM32F1)||defined(NRF51)||defined(NRF52))
    //F1芯片只要写入就要擦除
    ucRet = BSP_CPU_FLASH_REQ_ERASE;
    {
        uint32_t wWord,rWord ;
        uint16_t err=0;
        // 需要擦除
        if (ucRet == BSP_CPU_FLASH_REQ_ERASE)
        {
            Bsp_CpuFlash_Erase(_ulFlashAddr,_ulFlashAddr);
        }
        //得到32位（字）长度
        if(_ulSize%4)   _ulSize=_ulSize/4+1;
        else            _ulSize=_ulSize/4;
        //写数据
        FLASH_Unlock();
        for(i=0; i<_ulSize; i++)
        {
            wWord=0;
            wWord += _ucpSrc[4*i+3];
            wWord<<=8;
            wWord += _ucpSrc[4*i+2];
            wWord<<=8;
            wWord += _ucpSrc[4*i+1];
            wWord<<=8;
            wWord += _ucpSrc[4*i];
            //写到flash,每次写入4字节数据
            FLASH_ProgramWord(_ulFlashAddr+4*i,wWord);
            //读写入的数据
            rWord = *(__IO uint32_t*)(_ulFlashAddr+i*4);
            //比较写入与读出的数据，进行交验
            if(wWord!=rWord)
            {
                err++;
            }
        }
        FLASH_Lock();
        if(err!=0)
        {
            return(ERR);
        }
    }
#elif (defined(STM32F4))
    //关中断
    __set_PRIMASK(1);
    // FLASH 解锁
    FLASH_Unlock();
    // Clear pending flags (if any)
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    // 需要擦除
    if (ucRet == BSP_CPU_FLASH_REQ_ERASE)
    {
        //操作参数区
        if(_ulFlashAddr>=BSP_CPU_FLASH_PARA_ADDR && _ulFlashAddr<BSP_CPU_FLASH_PARA_BACKUP_ADDR)
        {
            // 擦除参数备份
            FLASH_EraseSector(Bsp_CpuFlash_GetSector(BSP_CPU_FLASH_PARA_BACKUP_ADDR), VoltageRange_1);
            // 复制数据到备份区
            for (i = 0; i < (BSP_CPU_FLASH_PARA_BACKUP_ADDR-BSP_CPU_FLASH_PARA_ADDR); i++)
            {
                FLASH_ProgramByte(BSP_CPU_FLASH_PARA_BACKUP_ADDR+i, *(uint8_t *)(BSP_CPU_FLASH_PARA_ADDR+i));
            }
            // 擦除参数区
            FLASH_EraseSector(Bsp_CpuFlash_GetSector(BSP_CPU_FLASH_PARA_ADDR), VoltageRange_1);
        }
        //操作非参数区
        else
        {
            FLASH_EraseSector(Bsp_CpuFlash_GetSector(_ulFlashAddr), VoltageRange_1);
        }
    }
    //操作参数区
    if(_ulFlashAddr>=BSP_CPU_FLASH_PARA_ADDR && _ulFlashAddr<BSP_CPU_FLASH_PARA_BACKUP_ADDR)
    {
        // 重新写入参数区
        // 按字节模式编程（为提高效率，可以按字编程，一次写入4字节）
        for (i = 0; i < (BSP_CPU_FLASH_PARA_BACKUP_ADDR-BSP_CPU_FLASH_PARA_ADDR); i++)
        {
            if((BSP_CPU_FLASH_PARA_ADDR+i)>=_ulFlashAddr
               && (BSP_CPU_FLASH_PARA_ADDR+i)<(_ulFlashAddr+_ulSize))
            {
                //如果是要修改的数据,按缓存修改
                FLASH_ProgramByte(BSP_CPU_FLASH_PARA_ADDR+i, _ucpSrc[BSP_CPU_FLASH_PARA_ADDR+i-_ulFlashAddr]);
            }
            else
            {
                //如果不是要修改的数据,则从备份区调入
                FLASH_ProgramByte(BSP_CPU_FLASH_PARA_ADDR+i, *(uint8_t *)(BSP_CPU_FLASH_PARA_BACKUP_ADDR+i));
            }
        }
    }
    //操作非参数区
    else
    {
        // 按字节模式编程（为提高效率，可以按字编程，一次写入4字节）
        for (i = 0; i < _ulSize; i++)
        {
            FLASH_ProgramByte(_ulFlashAddr++, *_ucpSrc++);
        }
    }
    // 擦除参数备份
    FLASH_EraseSector(Bsp_CpuFlash_GetSector(BSP_CPU_FLASH_PARA_BACKUP_ADDR), VoltageRange_1);
    // Flash 加锁，禁止写Flash控制寄存器
    FLASH_Lock();
    // 开中断
    __set_PRIMASK(0);
#elif (defined(NRF51)||defined(NRF52))
    i=i;
#endif
    return OK;
}

void Bsp_CpuFlash_DebugTestOnOff(uint8_t OnOff)
{
#ifdef BOOTLOADER
	  OnOff=OnOff;
#else
    uint8_t *pbuf=NULL;
    uint16_t i16=0;
    OnOff=OnOff;
    //初始化
    Bsp_CpuFlash_Init();
    //申请缓存
    pbuf    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    BspCpuId_Get(pbuf,&i16);
#if   (defined(NRF51))
    //芯片信息
    DebugOutHex("CpuID:",pbuf,12);
    sprintf((char*)pbuf,"Cpu FlashSize:%d kB\r\n",i16);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"Cpu FlashPageNum :%d\r\n",NRF_FICR->CODESIZE);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"Cpu FlashPageSize:%d\r\n",NRF_FICR->CODEPAGESIZE);
    DebugOutStr((int8_t*)pbuf);
    //擦除
    Bsp_CpuFlash_Erase(BSP_CPU_FLASH_PARA_ADDR,BSP_CPU_FLASH_PARA_ADDR);
    //写
    for(i16=0; i16<=255; i16++)
    {
        pbuf[i16]=i16;
    }
    Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,pbuf,256);
    //清空缓存
    memset(pbuf,0,256);
    //读取内存
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,pbuf,256);
    //验证
    for(i16=0; i16<=255; i16++)
    {
        if(pbuf[i16]!=i16)
        {
            break;
        }
    }
    //判定
    if(i16>255)
    {
        DebugOutStr((int8_t*)"Test OK");
    }
    else
    {
        DebugOutStr((int8_t*)"Test ERR");
    }
#endif
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
}

