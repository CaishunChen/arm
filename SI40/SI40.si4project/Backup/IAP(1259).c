/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//-------------------加载库函数------------------------------
#include "IAP.h"
#include "uctsk_Debug.h"
#include "Module_Memory.h"
#include "Bsp_CpuFlash.h"

#ifdef IAP_FLASH_SPI
#include "Bsp_ExtFlash.h"
#endif
//-------------------宏定义----------------------------------
#define IAP_MAX_PACKET_NUM    (IAP_MAX_BYTE/IAP_PACKET_SIZE)
static uint16_t Iap_MaxPacketNum=0;   //最大包数量(1起始)
static uint32_t Iap_Sum=0;
static uint8_t *Iap_pStateBitmapbuf=NULL;
static uint8_t Iap_Ver=0;
static uint32_t Iap_Len=0; 
//
uint16_t Iap_AllPackage=0,Iap_CurrentPackage=0;
/*******************************************************************************
函数功能:   IAP应用函数(输入函数)
参    数:
返 回 值:
*******************************************************************************/
enum IAP_ENTERPRO_CMD
{
    IAP_ENTERPRO_CMD_W=0,     //写数据
    IAP_ENTERPRO_CMD_F,       //格式化数据区
    IAP_ENTERPRO_CMD_S,       //标记升级标志
    IAP_ENTERPRO_CMD_RESET,   //软复位
};
static uint8_t IAP_Enter_pro(uint8_t cmd,uint8_t *buf,uint16_t CurrentPacket)
{
    uint8_t res=OK;
    uint32_t i32=0;
    i32 = i32;
    switch(cmd)
    {
        //---------------------写
        case IAP_ENTERPRO_CMD_W:
#ifdef   IAP_FLASH_CPU
            FLASH_Unlock();
            res=Bsp_CpuFlash_Write(BSP_CPU_FLASH_APP_BACKUP_ADDR+(CurrentPacket-1)*IAP_PACKET_SIZE,buf,IAP_PACKET_SIZE);
            FLASH_Lock();
#endif
#ifdef   IAP_FLASH_SPI
            Bsp_ExtFlash_Write_nByte((CurrentPacket-1)*IAP_PACKET_SIZE,buf,IAP_PACKET_SIZE);
#endif
            break;
        //---------------------格式化
        case IAP_ENTERPRO_CMD_F:
#ifdef   IAP_FLASH_CPU         
#ifdef STM32F1
            FLASH_Unlock();
            for(i32=BSP_CPU_FLASH_APP_BACKUP_ADDR; i32<(BSP_CPU_FLASH_APP_BACKUP_ADDR+BSP_CPU_FLASH_SIZE); i32+=2048)
                FLASH_ErasePage(i32);
            FLASH_Lock();
#endif
#endif
#ifdef   IAP_FLASH_SPI
            Bsp_ExtFlash_Erase(0,BspExtFlash_s_Flash.TotalSize-1);
#endif
            break;
        //---------------------标记
        case IAP_ENTERPRO_CMD_S:
        {
            MODULE_MEMORY_S_PARA *pPara;
            pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
#ifdef   IAP_FLASH_CPU               
            pPara->Updata=0x87654321;
#endif
#ifdef   IAP_FLASH_SPI
            pPara->Updata     =  0x77654321;
            pPara->UpdataLen  =  Iap_Len;
            pPara->UpdataSum  =  Iap_Sum;
#endif
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
        }
        break;
        //---------------------复位
        case IAP_ENTERPRO_CMD_RESET:
            MCU_SOFT_RESET;
            break;
        default:
            break;
    }
    return res;
}
/*******************************************************************************
函数功能:   IAP应用函数(输出函数)
参    数:
返 回 值:
*******************************************************************************/
enum IAP_APP_STEP
{
    IAP_APP_STEP_PRE=0,
    IAP_APP_STEP_START,
    IAP_APP_STEP_DOING,
    IAP_APP_STEP_END,
    IAP_APP_STEP_UPGRADE,
};
uint8_t IAP_APP(uint8_t cmd,uint8_t *buf,uint16_t len,uint16_t MaxPacket,uint16_t CurrentPacket,uint32_t Sum,uint8_t Ver,uint16_t* NextPacket)
{
    static uint8_t step=IAP_APP_STEP_PRE;
    uint8_t res=OK;
    uint16_t i,j;
    uint16_t i16;
    uint8_t *pi8;
    //申请内存
    pi8=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
    //复制数据
    memcpy(pi8,buf,IAP_PACKET_SIZE);
    //
    switch(cmd)
    {
        case IAP_APP_CMD_DATA:
            //长度校验
            if(len!=IAP_PACKET_SIZE && CurrentPacket!=MaxPacket)
            {
                res=ERR;
                break;
            }
            //状态机
            switch(step)
            {
                case IAP_APP_STEP_PRE:
                case IAP_APP_STEP_START:
                    //判断版本号 最大包数
                    if(Ver==0||MaxPacket>IAP_MAX_PACKET_NUM)
                    {
                        res=ERR;
                        break;
                    }
                    //更新变量
                    Iap_MaxPacketNum   =  MaxPacket;
                    Iap_Sum            =  0;
                    Iap_Ver            =  Ver;
                    //申请缓存
                    Iap_pStateBitmapbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
                    memset(Iap_pStateBitmapbuf,0,((IAP_MAX_PACKET_NUM-1)/8)+1);
                    //格式化存储器IAP空间
                    IAP_Enter_pro(IAP_ENTERPRO_CMD_F,NULL,NULL);
                    //
                    step = IAP_APP_STEP_DOING;
                case IAP_APP_STEP_DOING:
                    //验证基本信息
                    if(Iap_MaxPacketNum!=MaxPacket
                       ||Iap_Ver         !=Ver
                       ||CurrentPacket > Iap_MaxPacketNum)
                    {
                        res=ERR;
                        break;
                    }
                    //查看位图此包数据是否存在
                    i=Count_Bitmap_Read(Iap_pStateBitmapbuf,CurrentPacket);
                    //如果此包不存在,则更新数据和位图
                    if(i==FALSE)
                    {
                        //写入数据
                        IAP_Enter_pro(IAP_ENTERPRO_CMD_W,pi8,CurrentPacket);
                        //更新位图
                        Count_Bitmap_Write(Iap_pStateBitmapbuf,CurrentPacket,TRUE);
                        //更新校验和
                        for(i16=0; i16<len; i16++)
                        {
                            Iap_Sum+=pi8[i16];
                            Iap_Len++;
                        }
                    }
                    //根据位图返回下一包号
                    Count_Bitmap_Search(Iap_pStateBitmapbuf,NextPacket,FALSE,MaxPacket);
                    //总包
                    Iap_AllPackage = MaxPacket;
                    //收到有效包
                    i=(MaxPacket-1)>>3;
                    for(i16=j=0; j<(((IAP_MAX_PACKET_NUM-1)/8)+1); j++)
                    {
                        if(Iap_pStateBitmapbuf[j]&0x01)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x02)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x04)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x08)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x10)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x20)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x40)i16++;
                        if(Iap_pStateBitmapbuf[j]&0x80)i16++;
                    }
                    Iap_CurrentPackage = i16;
                    //
                    if(*NextPacket==0)
                    {
                        step=IAP_APP_STEP_END;
                    }
                    else
                    {
                        break;
                    }
                case IAP_APP_STEP_END:
                    //判定校验和
                    if(Iap_Sum != Sum)
                    {
                        *NextPacket=0xFFFE;
                        sprintf((char*)pi8,"IAP SUM ERR:获取(%ld)计数(%ld)",Sum,Iap_Sum);
                        DebugOut((int8_t*)pi8,strlen((char*)pi8));
                        step=IAP_APP_STEP_PRE;
                        break;
                    }
                    else
                    {
                        *NextPacket=0xFFFF;
                        step=IAP_APP_STEP_UPGRADE;
                    }
                case IAP_APP_STEP_UPGRADE:
                    //释放缓存
                    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,Iap_pStateBitmapbuf);
                    //标记升级标志
                    IAP_Enter_pro(IAP_ENTERPRO_CMD_S,pi8,NULL);
                    //延时2秒软复位
					BspRtc_SoftResetTime_s=2;
                    break;
                default:
                    step=IAP_APP_STEP_PRE;
                    break;
            }
            break;
        case IAP_APP_CMD_INIT:
            step = IAP_APP_STEP_PRE;
            break;
        default:
            break;
    }
    //释放内存
    MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pi8);
    //
    return res;
}
//-------------------------------------------------------------------------------
