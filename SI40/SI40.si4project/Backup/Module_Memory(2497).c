/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "Module_Memory.h"
#include "uctsk_Debug.h"
#include "Bsp_CpuFlash.h"
#include "Bsp_BkpRam.h"
#include "Bsp_Rtc.h"
#include "Language.h"
//------------------------------- 函数声明 -----------------------------------
MODULE_MEMORY_S_PARA *ModuleMemory_psPara;
MODULE_OS_SEM(Module_Memory_Sem);
//
#if     (defined(MODULE_MEMORY_INFO_ADDR))
MODULE_MEMORY_S_INFO_INFO   memory_s_InfoInfo;
#endif
//
#if   (defined(XKAP_ICARE_B_M))
uint8_t memory_WeightLock=0;
static uint32_t memory_WeightNum=0;
#endif
//------------------------------- 函数 ---------------------------------------
void Module_Memory_Init(void)
{
    //创建互斥信号量
    MODULE_OS_SEM_CREATE(Module_Memory_Sem,"Sem_Module_Memory",1);
    //
#if   (defined(MODULE_MEMORY_CPUFLASH_INIT))
    MODULE_MEMORY_CPUFLASH_INIT;
#endif
#if   (defined(MODULE_MEMORY_EXTFLASH_INIT))
    MODULE_MEMORY_EXTFLASH_INIT;
#endif
    //
    ModuleMemory_psPara = (MODULE_MEMORY_S_PARA *)BSP_CPU_FLASH_PARA_ADDR;
    //
    {
        //上电复位处理
        uint8_t i;
        MODULE_MEMORY_S_PARA *psPara;
        //申请缓存
        psPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        // 优先备份恢复
        Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_BACKUP_ADDR,(uint8_t*)psPara,sizeof(MODULE_MEMORY_S_PARA));
        if(psPara->Updata==0x12345678)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)psPara,NULL);
        }
        // 无有效参数则格式化
        else
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)psPara,NULL);
            if(psPara->Updata!=0x12345678)
            {
                i = MODULE_MEMORY_CMD_FACTORY_MODE_ALL;
                Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
            }
        }
        //版本空闲初始化
        if(psPara->Flag_Res7==1)
        {
            psPara->Flag_GprsUploadDayMove=OFF;
            psPara->Flag_GprsKeepOn=OFF;
            psPara->Flag_Res3=OFF;
            psPara->Flag_NightLight=OFF;
            psPara->Flag_Res5=OFF;
            psPara->Flag_Res6=OFF;
            psPara->Flag_Res7=OFF;
            psPara->res[0]=OFF;
            psPara->res[1]=OFF;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)psPara,NULL);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psPara);
    }
#if     (defined(MODULE_MEMORY_INFO_ADDR))
    {
        MODULE_MEMORY_S_INFO *ps_info;
        memory_s_InfoInfo.NextAddr=0xFFFFFFFF;
        memory_s_InfoInfo.AllNum=0;
        //申请缓存
        ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        for(ps_info[1].UnixTime=0; ps_info[1].UnixTime<MODULE_MEMORY_INFO_NUM; ps_info[1].UnixTime++)
        {
            // 读取数据块
            Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_INFO_ADDR+(ps_info[1].UnixTime*MODULE_MEMORY_INFO_SIZE),(uint8_t*)ps_info,MODULE_MEMORY_INFO_SIZE);
            // 判断数据块
            if(ps_info->UnixTime==0xFFFFFFFF)
            {
                if(memory_s_InfoInfo.NextAddr==0xFFFFFFFF)
                {
                    memory_s_InfoInfo.NextAddr  =   MODULE_MEMORY_INFO_ADDR+ps_info[1].UnixTime*MODULE_MEMORY_INFO_SIZE;
                }
            }
            else
            {
                memory_s_InfoInfo.AllNum++;
            }
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
    }
#endif
}
uint8_t  Module_Memory_App(uint16_t Cmd,uint8_t *pBuf,void *pPara)
{
    uint8_t res=OK;
    uint32_t i32;
    MODULE_OS_ERR err;
    //
    i32  =  i32;
    err  =  err;
    //
    MODULE_OS_SEM_PEND(Module_Memory_Sem,0,TRUE,err);
    switch(Cmd)
    {
        case MODULE_MEMORY_APP_CMD_GLOBAL_R:
            res = Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,pBuf,sizeof(MODULE_MEMORY_S_PARA));
            break;
        case MODULE_MEMORY_APP_CMD_GLOBAL_W:
            res = Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,pBuf,sizeof(MODULE_MEMORY_S_PARA));
            break;
        case MODULE_MEMORY_APP_CMD_FACTORY:
        {
            MODULE_MEMORY_S_PARA *psPara;
            struct tm *ptm;
            psPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)psPara,sizeof(MODULE_MEMORY_S_PARA));
            if(*((uint8_t*)pPara)==(uint8_t)MODULE_MEMORY_CMD_FACTORY_MODE_ALL)
            {
#if   (defined(MODULE_MEMORY_EXTFLASH_INIT))
                //清空外部FLASH
                Bsp_ExtFlash_Erase(0,BspExtFlash_s_Flash.TotalSize-1);
#endif
                psPara->PowerOnCount      =  0;
                psPara->ResetCount        =  0;
                psPara->Language          =  LANGUAGE_E_CHINESE;
                psPara->HardFault_Count   =  0;
                memset((char*)(psPara->HardFault_Time),0,6);
                psPara->pHardFaultTaskName=  "NULL";
                psPara->Theme             =  1;
                psPara->GprsServer        =  0;
                psPara->Flag_GprsKeepOn   =  OFF;
                psPara->Flag_GprsRegister =  OFF;
                psPara->Flag_GprsUploadDayMove  =  OFF;
                psPara->Flag_Res3         =  OFF;

                psPara->Flag_Res5         =  OFF;
                psPara->Flag_Res6         =  OFF;
                psPara->Flag_Res7         =  OFF;
                psPara->res[0]            =  OFF;
                psPara->res[1]            =  OFF;
                psPara->Addr              =  1;
#if   (defined(XKAP_ICARE_B_M))
                psPara->Flag_NightLight   =  ON;
                psPara->ThreshsholdValue1 =  0xFFFFFFFF;
                psPara->ThreshsholdValue2 =  1000;
                psPara->ThreshsholdValue3 =  100;
                psPara->ThreshsholdValue4 =  0xFFFF;
                psPara->Offset_H[0]       =  22;
                psPara->Offset_L[0]       =  18;
                psPara->Offset_H[1]       =  45;
                psPara->Offset_L[1]       =  55;
                psPara->Offset_TimerS     =  5;
                psPara->Reponse_TimerS    =  0xFFFF;
#else
                psPara->Flag_NightLight   =  OFF;
                psPara->ThreshsholdValue1 =  0xFFFFFFFF;
                psPara->ThreshsholdValue2 =  0xFFFFFFFF;
                psPara->ThreshsholdValue3 =  0xFFFF;
                psPara->ThreshsholdValue4 =  0xFFFF;
                psPara->Offset_H[0] =   psPara->Offset_H[1] =  0xFF;
                psPara->Offset_L[0] =   psPara->Offset_L[1] =  0xFF;
                psPara->Offset_TimerS     =  0xFFFF;
                psPara->Reponse_TimerS    =  0xFFFF;
#endif
#if   (defined(STM32F1)||defined(STM32F4))
                BSP_BKPRAM_WRITE(BSP_BKPRAM_POWER_CMT, 0x0000);
                BSP_BKPRAM_WRITE(BSP_BKPRAM_WAKE_UP, 0x0000);
                BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_H, 0x0000);
                BSP_BKPRAM_WRITE(BSP_BKPRAM_GPRS_FLOW_L, 0x0000);
                BSP_BKPRAM_WRITE(BSP_BKPRAM_MONTH, 0x0000);
                BSP_BKPRAM_WRITE(BSP_BKPRAM_BELL_TIME_H, 0x0000);
#endif
                //关闭闹钟
                psPara->RtcAlarm[0]=Count_2ByteToWord(6,30);
                psPara->RtcAlarm[1]=Count_2ByteToWord(7,30);
                psPara->RtcAlarmMode[0]=0;
                psPara->RtcAlarmMode[1]=0;
                //时钟格式12
                psPara->RtcFormat=0;
                //背光200
                psPara->LcdBackLight1=200;
                psPara->LcdBackLight2=1;
                psPara->LcdBackLight3=200;
                psPara->LcdBackLight4=200;
                //设置阈值 阀值
                psPara->Rfms_SubThreshold=200;
                //串口波特率
                psPara->UartBps[0]=4;
                psPara->UartBps[1]=4;
                psPara->UartBps[2]=4;
                psPara->UartBps[3]=4;
                psPara->UartBps[4]=4;
                psPara->UartBps[5]=4;
                psPara->UartBps[6]=4;
                psPara->UartBps[7]=4;
                //设置APN为CMNET
                psPara->Gsm_Apn=0;
                // RFMS自动测量开始时间
                psPara->RtcAutoBeginTime =  Count_2ByteToWord(22,30);
                // RFMS自动测量结束时间
                psPara->RtcAutoEndTime   =  Count_2ByteToWord(6,30);
                // RFMS自动测量使能(0-关闭,1-开始)
                psPara->RtcAutoOnOff =  OFF;
                // 入睡提醒使能(0-关闭,1-开始)
                psPara->RtcWarnOnOff[0] =  OFF;
                psPara->RtcWarnOnOff[1] =  OFF;
                psPara->RtcWarnOnOff[2] =  OFF;
                psPara->RtcWarnOnOff[3] =  OFF;
                // 入睡提醒时间
                psPara->RtcWarnTime[0]  =  Count_2ByteToWord(22,30);
                psPara->RtcWarnTime[1]  =  0;
                psPara->RtcWarnTime[2]  =  0;
                psPara->RtcWarnTime[3]  =  0;
            }
            //参数标识
            psPara->Updata=0x12345678;
            //设置存储当前位置
            psPara->MemoryDayNextP=0;
            //擦除
            Bsp_CpuFlash_Erase((uint32_t)BSP_CPU_FLASH_PARA_ADDR,(uint32_t)BSP_CPU_FLASH_PARA_ADDR_END);
            //保存
            Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)psPara,sizeof(MODULE_MEMORY_S_PARA));
            if(*((uint8_t*)pPara)==(uint8_t)MODULE_MEMORY_CMD_FACTORY_MODE_ALL)
            {
                //设置时钟
                ptm=(struct tm *)psPara;
                ptm->tm_year=FACTORY_TIME_YEAR;
                ptm->tm_mon =FACTORY_TIME_MONTH-1;
                ptm->tm_mday=FACTORY_TIME_DAY;
                ptm->tm_hour=FACTORY_TIME_HOUR;
                ptm->tm_min =FACTORY_TIME_MINUTE;
                ptm->tm_sec =FACTORY_TIME_SECONT;
                BspRtc_SetRealTime(NULL,ptm,NULL,NULL);
            }
            //
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,psPara);
        }
        break;
        case MODULE_MEMORY_APP_CMD_HARDFAULT:
        {
            MODULE_MEMORY_S_PARA *pPara;
            pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
            //-----
            pPara->HardFault_Count     += 1;
            BspRtc_ReadRealTime(NULL,NULL,NULL,pPara->HardFault_Time);
            pPara->pHardFaultTaskName  =  (uint8_t*)MODULE_OS_GET_CURRENT_TASK_NAME_STR();
            //-----
            Bsp_CpuFlash_Write(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
        }
        break;
        case MODULE_MEMORY_APP_CMD_INFO_W:
        {
            MODULE_MEMORY_S_INFO *ps_info;
            //申请缓存
            ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            // 直接写
            Bsp_ExtFlash_Write_nByte(memory_s_InfoInfo.NextAddr,(uint8_t*)pBuf,MODULE_MEMORY_INFO_SIZE);
            memory_s_InfoInfo.AllNum++;
            // 删除数据块
            memory_s_InfoInfo.NextAddr+=MODULE_MEMORY_INFO_SIZE;
            if(memory_s_InfoInfo.NextAddr>MODULE_MEMORY_INFO_ADDR_END)
            {
                memory_s_InfoInfo.NextAddr=MODULE_MEMORY_INFO_ADDR;
            }
            if(memory_s_InfoInfo.NextAddr%BspExtFlash_s_Flash.PageSize==0)
            {
                Bsp_ExtFlash_Read_nByte(memory_s_InfoInfo.NextAddr,(uint8_t*)ps_info,MODULE_MEMORY_INFO_SIZE);
                if(ps_info->UnixTime!=0xFFFFFFFF)
                {
                    Bsp_ExtFlash_Erase(memory_s_InfoInfo.NextAddr,(memory_s_InfoInfo.NextAddr+BspExtFlash_s_Flash.PageSize)-1);
                    memory_s_InfoInfo.AllNum-=(BspExtFlash_s_Flash.PageSize/MODULE_MEMORY_INFO_SIZE);
                }
            }
            // 更新全局信息
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
        }
        break;
        case MODULE_MEMORY_APP_CMD_INFO_R:
        {
            MODULE_MEMORY_S_INFO_FILTER *ps_InfoFilter=(MODULE_MEMORY_S_INFO_FILTER *)pPara;
            MODULE_MEMORY_S_INFO *ps_info;
            //申请缓存
            ps_info=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            // 容错---输出最大条目为0
            if(ps_InfoFilter->MaxNumber==0)
            {
                pBuf=NULL;
                break;
            }
            //容错---当前没有有效条目
            if(memory_s_InfoInfo.AllNum==0)
            {
                pBuf=NULL;
                break;
            }
            ps_InfoFilter->OutBufNum=0;
            // 按时段提取
            if(ps_InfoFilter->Mode==0)
            {
            }
            // 首条起始(固定正序)
            else if(ps_InfoFilter->Mode==1)
            {
            }
            // 尾条起始(固定倒叙)
            else if(ps_InfoFilter->Mode==2)
            {
                // 计算尾条地址---i32
                if(memory_s_InfoInfo.NextAddr<=MODULE_MEMORY_INFO_ADDR)
                {
                    i32=MODULE_MEMORY_INFO_ADDR_END+1-MODULE_MEMORY_INFO_SIZE;
                }
                else
                {
                    i32=memory_s_InfoInfo.NextAddr-MODULE_MEMORY_INFO_SIZE;
                }
                //
                ps_info[1].Para=0;
                for(ps_info[1].UnixTime=memory_s_InfoInfo.AllNum; ps_info[1].UnixTime!=0; ps_info[1].UnixTime--)
                {
                    // 提取
                    Bsp_ExtFlash_Read_nByte(i32,(uint8_t*)ps_info,MODULE_MEMORY_INFO_SIZE);
                    // 选取
                    if(i32==0)
                    {
                        i32=MODULE_MEMORY_INFO_ADDR_END+1-MODULE_MEMORY_INFO_SIZE;
                    }
                    else
                    {
                        i32=i32-MODULE_MEMORY_INFO_SIZE;
                    }
                    // 判断
                    if((MODULE_E_ERR_BEGIN<=ps_info->ErrId)&&(ps_info->ErrId<=MODULE_E_ERR_END)&&(ps_InfoFilter->Flag_Enable_Err==0))
                    {
                        continue;
                    }
                    if((MODULE_E_WARN_BEGIN<=ps_info->ErrId)&&(ps_info->ErrId<=MODULE_E_WARN_END)&&(ps_InfoFilter->Flag_Enable_Warn==0))
                    {
                        continue;
                    }
                    if((MODULE_E_INFO_BEGIN<=ps_info->ErrId)&&(ps_info->ErrId<=MODULE_E_INFO_END)&&(ps_InfoFilter->Flag_Enable_Info==0))
                    {
                        continue;
                    }
                    if((MODULE_E_DEBUG_BEGIN<=ps_info->ErrId)&&(ps_info->ErrId<=MODULE_E_DEBUG_END)&&(ps_InfoFilter->Flag_Enable_Debug==0))
                    {
                        continue;
                    }
                    // 指定信息滤波
                    if(ps_InfoFilter->IncludeNum!=0)
                    {
                        for(ps_info[1].ErrId=0; ps_info[1].ErrId<ps_InfoFilter->IncludeNum; ps_info[1].ErrId++)
                        {
                            if(ps_InfoFilter->pInInclude[ps_info[1].ErrId] == ps_info->ErrId)
                            {
                                break;
                            }
                        }
                        if(ps_info[1].ErrId>=ps_InfoFilter->IncludeNum)
                        {
                            continue;
                        }
                    }
                    // 跳过指定数量符合条件的结果
                    ps_info[1].Para++;
                    if((ps_info[1].Para)<=ps_InfoFilter->BeginTime)
                    {
                        continue;
                    }
                    // 打印到debug
                    if(ps_InfoFilter->Flag_PrintToDebug)
                    {
                        sprintf((char*)&ps_info[2],"num-%d,time-%ld,id-%d,Para-%d\r\n",\
                                ps_InfoFilter->OutBufNum+1,ps_info->UnixTime,ps_info->ErrId,ps_info->Para);
                        DebugOutStr((int8_t*)&ps_info[2]);
                    }
                    // 打印到mem
                    if((ps_InfoFilter->Flag_PrintToMem) && pBuf!=NULL)
                    {
                        memcpy((char*)&pBuf[MODULE_MEMORY_INFO_SIZE*ps_InfoFilter->Flag_PrintToMem],\
                               (char*)ps_info,MODULE_MEMORY_INFO_SIZE);
                    }
                    // 超出最大数量,则退出
                    ps_InfoFilter->OutBufNum++;
                    if(ps_InfoFilter->OutBufNum>=ps_InfoFilter->MaxNumber)
                    {
                        break;
                    }
                }
            }
            // 更新全局信息
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_info);
        }
        break;
#if   (defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4)||defined(PROJECT_TCI_V30))
#elif   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        case MODULE_MEMORY_APP_CMD_DAY_INFO_R:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Read(MODULE_MEMORY_DAY_INFO_ADDR+MODULE_MEMORY_DAY_INFO_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_INFO_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_INFO_W:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Write(MODULE_MEMORY_DAY_INFO_ADDR+MODULE_MEMORY_DAY_INFO_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_INFO_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Read(MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR+MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_W:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Write(MODULE_MEMORY_DAY_SLEEPLEVEL_ADDR+MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_SLEEPLEVEL_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_BREATH_R:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Read(MODULE_MEMORY_DAY_BREATH_ADDR+MODULE_MEMORY_DAY_BREATH_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_BREATH_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_BREATH_W:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Write(MODULE_MEMORY_DAY_BREATH_ADDR+MODULE_MEMORY_DAY_BREATH_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_BREATH_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Read(MODULE_MEMORY_DAY_HEARTRATE_ADDR+MODULE_MEMORY_DAY_HEARTRATE_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_HEARTRATE_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_W:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Write(MODULE_MEMORY_DAY_HEARTRATE_ADDR+MODULE_MEMORY_DAY_HEARTRATE_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_HEARTRATE_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Read(MODULE_MEMORY_DAY_BODYMOVE_ADDR+MODULE_MEMORY_DAY_BODYMOVE_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_BODYMOVE_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_W:
            if(pPara==NULL)
            {
                res=ERR;
                break;
            }
            if(*pPara<MODULE_MEMORY_ADDR_DAY_NUM)
            {
                res = Bsp_CpuFlash_Write(MODULE_MEMORY_DAY_BODYMOVE_ADDR+MODULE_MEMORY_DAY_BODYMOVE_SIZE*(*pPara),pBuf,MODULE_MEMORY_DAY_BODYMOVE_SIZE);
            }
            else
            {
                res=ERR;
                break;
            }
            break;
        case MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_R:
            res = Bsp_CpuFlash_Read(MODULE_MEMORY_SLEEP_BEGIN_TIME_ADDR,pBuf,MODULE_MEMORY_SLEEP_BEGIN_TIME_SIZE);
            break;
        case MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_W:
            res = Bsp_CpuFlash_Write(MODULE_MEMORY_SLEEP_BEGIN_TIME_ADDR,pBuf,MODULE_MEMORY_SLEEP_BEGIN_TIME_SIZE);
            break;
            //--------------------
#if     (defined(__BSP_EXT_FLASH_H))
        case MODULE_MEMORY_APP_CMD_EXTFLASH_READ:
        {
            MODULE_MEMORY_S_EXTFLASH *ps;
            Bsp_ExtFlash_Read_nByte(ps->addr,pBuf,ps->len);
            break;
        }
        case MODULE_MEMORY_APP_CMD_EXTFLASH_WRITE:
        {
            MODULE_MEMORY_S_EXTFLASH *ps;
            Bsp_ExtFlash_Write_nByte(ps->addr,pBuf,ps->len);
            break;
        }
        case MODULE_MEMORY_APP_CMD_EXTFLASH_EARSE:
        {
            MODULE_MEMORY_S_EXTFLASH *ps;
            Bsp_ExtFlash_Erase(ps->addr,(ps->addr)+(ps->len)-1);
            break;
        }
        case MODULE_MEMORY_APP_CMD_EXTFLASH_FORMAT:
            Bsp_ExtFlash_Erase(0,BspExtFlash_s_Flash.TotalSize-1);
            break;
#endif
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
        case MODULE_MEMORY_APP_CMD_MOVE_DATE_R:
            //
            pBuf[104]=0;
            for(i32=0; i32<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i32++)
            {
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,&pBuf[100],4);
                pBuf[pBuf[104]++]  =  pBuf[100+0];
                pBuf[pBuf[104]++]  =  pBuf[100+1];
                pBuf[pBuf[104]++]  =  pBuf[100+2];
                pBuf[pBuf[104]++]  =  pBuf[100+3];
            }
            break;
        case MODULE_MEMORY_APP_CMD_MOVE_10MIN_R:
            // 参数: 年 月 日 10分钟索引0起始(0-143)
            for(i32=0; i32<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i32++)
            {
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,pBuf,4);
                if((pPara[0]==pBuf[1])&&(pPara[1]==pBuf[2])&&(pPara[2]==pBuf[3]))
                {
                    break;
                }
            }
            // 无此日期数据
            if((i32>=MODULE_MEMORY_EXTFLASH_MOVE_NUM)||(pPara[3]>143))
            {
                res = ERR;
                break;
            }
            // 有数据
            Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+240*pPara[3]\
                                    ,pBuf,240);
            break;
        case MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_R:
        case MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_VALID_R:
            //
        {
            uint8_t i=0,j=0,k=0;
            uint8_t buf[6],buf1[3];
            uint16_t i16;
            // 找出日期索引i(0~6),编号j(0~47)
            for(i=0; i<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i++)
            {
                // 读取日期
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,buf,4);
                // 日期无效的退出
                if(buf[0]==0xFF||buf[1]==0xFF||buf[2]==0xFF||buf[3]==0xFF)
                {
                    continue;
                }
                if(pPara!=NULL)
                {
                    // 按日期查询看是否有有效数据
                    if(buf[1]!=pPara[0]||buf[2]!=pPara[1]||buf[3]!=pPara[2])
                    {
                        continue;
                    }
                }
                //时间赋值
                buf1[0]=buf[1];
                buf1[1]=buf[2];
                buf1[2]=buf[3];
                // 无待处理数据则退出
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+34564\
                                        ,buf,6);
                if(buf[0]==0&&buf[1]==0&&buf[2]==0&&buf[3]==0&&buf[4]==0&&buf[5]==0)
                {
                    continue;
                }
                Count_Bitmap_Search(buf,&i16,TRUE,48);
                if(i16!=0)
                {
                    j=i16-1;
                }
                // 今日数据处理
                if(BspRtc_CurrentTimeBuf[0]==buf1[0]\
                   &&BspRtc_CurrentTimeBuf[1]==buf1[1]\
                   &&BspRtc_CurrentTimeBuf[2]==buf1[2])
                {
                    k=(BspRtc_CurrentTimeBuf[3]*2+BspRtc_CurrentTimeBuf[4]/30+12*2)%48;
                    if(j>=k)
                    {
                        continue;
                    }
                }
                break;
            }
            if(i>=MODULE_MEMORY_EXTFLASH_MOVE_NUM)
            {
                res = ERR;
                break;
            }
            if(Cmd==MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_VALID_R)
            {
                break;
            }
            //DebugOutHex("bitmap:",buf,6);
            // 日期赋值
            pBuf[0]=pBuf[6]=buf1[0];
            pBuf[1]=pBuf[7]=buf1[1];
            pBuf[2]=pBuf[8]=buf1[2];
            pBuf[3]=(j/2+12)%24;
            pBuf[9]=((j+1)/2+12)%24;
            pBuf[4]=(j%2)*30;
            pBuf[10]=((j+1)%2)*30;
            pBuf[5]=pBuf[11]=0;
            // 数据赋值
            Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+720*j\
                                    ,&pBuf[12],720);
            break;
        }
        case MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_W:
        {
            uint8_t i=0,j=0;
            uint8_t buf[6];
            // 找出日期索引i(0~6),编号j(0~47)
            for(i=0; i<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i++)
            {
                // 读取日期
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,buf,4);
                // 日期不对应则继续
                if((buf[1]!=pBuf[0])||(buf[2]!=pBuf[1])||(buf[3]!=pBuf[2]))
                {
                    continue;
                }
                // 根据时间得出索引
                j=(pBuf[3]*2+pBuf[4]/30+12*2)%48;
                // 读位图
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+34564\
                                        ,buf,6);
                // 更新位图
                Count_Bitmap_Write(buf,j+1,FALSE);
                // 写位图
                Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+34564\
                                         ,buf,6);
            }
            break;
        }
        case MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_RESET:
        {
            uint8_t i=0;
            uint8_t buf[6];
            uint8_t *pbuf1;
            // 找出日期索引i(0~6),编号j(0~47)
            for(i=0; i<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i++)
            {
                // 读取日期
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,buf,4);
                // 日期不对应则继续
                if((buf[1]!=pBuf[0])||(buf[2]!=pBuf[1])||(buf[3]!=pBuf[2]))
                {
                    continue;
                }
                // 读位图
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+34564\
                                        ,buf,6);
                // 满足条件则跳过
                if(buf[0]==0xFF&&buf[1]==0xFF&&buf[2]==0xFF&&buf[3]==0xFF&&buf[4]==0xFF&&buf[5]==0xFF)
                {
                    continue;
                }
                // 读取
                pbuf1 = MemManager_Get(E_MEM_MANAGER_TYPE_5KB_BASIC);
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+32768,pbuf1,4*1024);
                Bsp_ExtFlash_Erase(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+32768,\
                                   MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+32768+4*1024-1);
                memset(&pbuf1[34564-32768],0xFF,6);
                Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+32768,pbuf1,4*1024);
                MemManager_Free(E_MEM_MANAGER_TYPE_5KB_BASIC,pbuf1);
            }
            break;
        }
        case MODULE_MEMORY_APP_CMD_MOVE_1MIN_W:
        case MODULE_MEMORY_APP_CMD_MOVE_10MIN_W:
            // 参数: 年 月 日 10分钟索引0起始(0-143)
        {
            uint8_t *pbuf;
            uint8_t i=0xFF,j=0xFF;
            // 容错
            if(pPara[3]>143 && Cmd == MODULE_MEMORY_APP_CMD_MOVE_10MIN_W)
            {
                res = ERR;
                break;
            }
            pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            // 查看有无此日期
            pbuf[10]=pbuf[11]=pbuf[12]=0xFF;
            for(i32=0; i32<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i32++)
            {
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE\
                                        ,pbuf,4);
                //有此日期
                if((pPara[0]==pbuf[1])&&(pPara[1]==pbuf[2])&&(pPara[2]==pbuf[3]))
                {
                    break;
                }
                //此区域空闲
                else if((pbuf[0]==0xFF)||(pbuf[1]==0xFF)||(pbuf[2]==0xFF)||(pbuf[3]==0xFF))
                {
                    if(i==0xFF)
                    {
                        i=i32;
                    }
                }
                //有其他日期
                else
                {
                    if((pbuf[10]>pbuf[1])\
                       ||((pbuf[10]==pbuf[1])&&(pbuf[11]>pbuf[2]))\
                       ||((pbuf[10]==pbuf[1])&&(pbuf[11]==pbuf[2])&&(pbuf[12]>pbuf[3])))
                    {
                        pbuf[10]=pbuf[1];
                        pbuf[11]=pbuf[2];
                        pbuf[12]=pbuf[3];
                        j=i32;
                    }
                }
            }
            // 如果无此日期
            if(i32>=MODULE_MEMORY_EXTFLASH_MOVE_NUM)
            {
                //查找空闲
                if(i<MODULE_MEMORY_EXTFLASH_MOVE_NUM)
                {
                    ;
                }
                //查找最小
                else if(j<MODULE_MEMORY_EXTFLASH_MOVE_NUM)
                {
                    //擦出
                    Bsp_ExtFlash_Erase(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+j*MODULE_MEMORY_EXTFLASH_MOVE_SIZE,\
                                       MODULE_MEMORY_EXTFLASH_MOVE_ADDR+(j+1)*MODULE_MEMORY_EXTFLASH_MOVE_SIZE-1);
                }
                i32 = i;
                //写入日期
                pbuf[0]   =  0;
                pbuf[1]   =  pPara[0];
                pbuf[2]   =  pPara[1];
                pbuf[3]   =  pPara[2];
                Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE,pbuf,4);
            }
            // 写入索引(i32)
            if(Cmd==MODULE_MEMORY_APP_CMD_MOVE_10MIN_W)
            {
                //查看是否为空
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+240*pPara[3]\
                                        ,pbuf,240);
                for(i=0; i<240; i++)
                {
                    if(pbuf[i]!=0xFF)
                    {
                        break;
                    }
                }
                //如果为空则写入
                if(i>=240)
                {
                    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+240*pPara[3]\
                                             ,pBuf,240);
                }
            }
            else if((Cmd==MODULE_MEMORY_APP_CMD_MOVE_1MIN_W))
            {
                //查看是否为空
                Bsp_ExtFlash_Read_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+24*(pPara[3]*60+pPara[4])\
                                        ,pbuf,24);
                for(i=0; i<24; i++)
                {
                    if(pbuf[i]!=0xFF)
                    {
                        break;
                    }
                }
                //如果为空则写入
                if(i>=24)
                {
                    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+i32*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+24*(pPara[3]*60+pPara[4])\
                                             ,pBuf,24);
                }
            }
            //如果不为空则覆盖(暂时不做)
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
        break;
        case MODULE_MEMORY_APP_CMD_MOVE_FORMAT:
            // 参数: NULL
            Bsp_ExtFlash_Erase(MODULE_MEMORY_EXTFLASH_MOVE_ADDR,MODULE_MEMORY_EXTFLASH_MOVE_ADDR_END);
            break;
#endif
#elif (defined(XKAP_ICARE_B_M))
        case MODULE_MEMORY_APP_CMD_WEIGHT_W:
            // 计算出存储地址
            i32 = MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR;
            i32 += memory_WeightNum*128;
            if(i32>MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END)
            {
                //i32 = i32 - (MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END+1);
                res=ERR;
                break;
            }
            // 越限擦除(必须留空,否则无法初始化起始地址)
            /*
            if((i32==memory_WeightBeginAddr) || ((i32+128)==memory_WeightBeginAddr))
            {
                Bsp_ExtFlash_Erase(memory_WeightBeginAddr,memory_WeightBeginAddr+BspExtFlash_s_Flash.PageSize-1);
                //起始地址更新
                memory_WeightBeginAddr+=(BspExtFlash_s_Flash.PageSize/128);
                if(memory_WeightBeginAddr>MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END)
                {
                    memory_WeightBeginAddr=MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR;
                }
                //数量更新
                memory_WeightNum=memory_WeightNum-(BspExtFlash_s_Flash.PageSize/128);
            }
            */
            // 数量更新
            memory_WeightNum++;
            // 写数据
            Bsp_ExtFlash_Write_nByte(i32,pBuf,128);
            break;
        case MODULE_MEMORY_APP_CMD_WEIGHT_R:
            // 读取索引
            i32=*((uint32_t*)pPara);
            if(i32>=MODULE_MEMORY_EXTFLASH_WEIGHT_SIZE)
            {
                res=ERR;
                break;
            }
            // 索引转换地址(0起始)
            i32 = MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR+(i32*128);
            /*
            if(i32>=MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END)
            {
                i32=i32-(MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END+1);
            }
            */
            //
            Bsp_ExtFlash_Read_nByte(i32,pBuf,128);
            break;
        case MODULE_MEMORY_APP_CMD_WEIGHT_CLR:
            Bsp_ExtFlash_Erase(MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR,MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR_END);
            memory_WeightNum=0;
            break;
#endif
        //
        default:
            res = ERR;
            break;
            //case MODULE_MEMORY_APP_CMD_FORMAT:
            //break;
    }
    MODULE_OS_SEM_POST(Module_Memory_Sem);
    return res;
}
/*******************************************************************************
* 函数功能: 在Debug中打印出信息
*******************************************************************************/
#if   (defined(PROJECT_BASE_STM32F1)||defined(PROJECT_BASE_STM32F4)||defined(PROJECT_TCI_V30))
#else
void Module_Memory_100ms(void)
{
    uint8_t *pbuf;
    uint8_t i=0;
    uint32_t i32;
    i=i;
    i32=i32;
#if  (defined(XKAP_ICARE_B_M))
    if(memory_WeightLock==0)
    {
        return;
    }
    //上锁
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    {
        MODULE_MEMORY_S_WEIGHT *ps;
        //打印出存储的重量信息
        ps  =  (MODULE_MEMORY_S_WEIGHT *)pbuf;

        for(i32=0; i32<MODULE_MEMORY_EXTFLASH_WEIGHT_SIZE; i32++)
        {
            if(memory_WeightLock==0)
            {
                break;
            }
            i=Module_Memory_App(MODULE_MEMORY_APP_CMD_WEIGHT_R,(uint8_t*)ps,(uint8_t *)&i32);
            if(i!=OK)
            {
                //continue;
            }
            //时间
            //sprintf((char*)&pbuf[128],"Count: %05d\r\n",i32);
            //DebugOutStr((int8_t*)&pbuf[128]);
            sprintf((char*)&pbuf[128],"(%04d)DateTime: %04d-%02d-%02d %02d:%02d:%02d\r\n"\
                    ,i32,2000+ps->DateTime[0],ps->DateTime[1],ps->DateTime[2],ps->DateTime[3],ps->DateTime[4],ps->DateTime[5]);
            DebugOutStr((int8_t*)&pbuf[128]);
            //数据
            for(i=0; i<10; i++)
            {
                sprintf((char*)&pbuf[128],"  %05d %05d %05d %05d %ld\r\n"\
                        ,ps->AdValue[0][i],ps->AdValue[1][i],ps->AdValue[2][i],ps->AdValue[3][i],ps->Hx711[i]);
                DebugOutStr((int8_t*)&pbuf[128]);
            }
        }
        memory_WeightLock=0;
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
}

void Module_Memory_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    uint8_t i=0;
    uint32_t i32;
    i=i;
    i32=i32;
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
    // 格式化
    Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_FORMAT,NULL,NULL);
    // 延时
    MODULE_OS_DELAY_MS(1000);
    // 输入模拟数据
    pbuf[0] =  0;
    pbuf[1] =  17;
    pbuf[2] =  2;
    pbuf[3] =  1;
    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+0*MODULE_MEMORY_EXTFLASH_MOVE_SIZE
                             ,pbuf,4);
    pbuf[3] =  2;
    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+1*MODULE_MEMORY_EXTFLASH_MOVE_SIZE
                             ,pbuf,4);
    pbuf[3] =  3;
    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+2*MODULE_MEMORY_EXTFLASH_MOVE_SIZE
                             ,pbuf,4);
    pbuf[3] =  4;
    Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+3*MODULE_MEMORY_EXTFLASH_MOVE_SIZE
                             ,pbuf,4);
    for(i32=0; i32<24*360*4; i32++)
    {
        pbuf[0]   =  i32>>24;
        pbuf[1]   =  i32>>16;
        pbuf[2]   =  i32>>8;
        pbuf[3]   =  i32>>0;
        Bsp_ExtFlash_Write_nByte(MODULE_MEMORY_EXTFLASH_MOVE_ADDR+0*MODULE_MEMORY_EXTFLASH_MOVE_SIZE+4+i32*4
                                 ,pbuf,4);
    }
    // 打印有效日期
    Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_DATE_R,pbuf,NULL);
    DebugOutHex("Move Date:\r\n",pbuf,4*4);
    // 打印数据
    pbuf[300] =  17;
    pbuf[301] =  2;
    pbuf[302] =  1;
    for(i=0; i<144; i++)
    {
        pbuf[303] =  i;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_10MIN_R,pbuf,&pbuf[300]);
        sprintf((char*)&pbuf[400],"Move Data-%d:\r\n",i);
        DebugOutHex((char*)&pbuf[400],pbuf,240);
    }
#elif (defined(XKAP_ICARE_B_M))
    if(OnOff==ON)
    {
        //上锁
        memory_WeightLock=1;
    }
    else
    {
        //解锁
        memory_WeightLock=0;
    }
#endif
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    OnOff=OnOff;
}

void Module_Memory_DebugTestFactoryOnOff(uint8_t OnOff)
{
    /*
#if (defined(XKAP_ICARE_B_M))
     uint8_t *pbuf;
     pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
     //上锁
     memory_WeightLock=1;
     sprintf((char*)pbuf,"Weight Memory Reset:...\r\n");
     DebugOutStr((int8_t*)pbuf);
     Module_Memory_App(MODULE_MEMORY_APP_CMD_WEIGHT_CLR,NULL,NULL);
     sprintf((char*)pbuf,"Weight Memory Reset End\r\n");
     DebugOutStr((int8_t*)pbuf);
     memory_WeightBeginAddr=MODULE_MEMORY_EXTFLASH_WEIGHT_ADDR;
     memory_WeightNum=0;
     //解锁
     memory_WeightLock=0;
     MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
     */
    uint8_t i;
    i = MODULE_MEMORY_CMD_FACTORY_MODE_ALL;
    Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
    MODULE_OS_DELAY_MS(100);
    MCU_SOFT_RESET;
    OnOff=OnOff;
}
void Module_Memory_DebugTest_PrintInfo(uint8_t OnOff)
{
    MODULE_MEMORY_S_INFO_FILTER *ps_Filter;
    //
    ps_Filter   =   MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    // 打印消息头
    sprintf((char*)ps_Filter,"Debug Print Info:\r\n");
    DebugOutStr((int8_t*)ps_Filter);
    // 调试写消息
    /*
    {
        MODULE_MEMORY_S_INFO    *ps_Info;
        ps_Info     =   (MODULE_MEMORY_S_INFO*)&ps_Filter[1];
        for(ps_Filter->Mode=0; ps_Filter->Mode<10; ps_Filter->Mode++)
        {
            ps_Info->UnixTime   =   ps_Filter->Mode+100;
            ps_Info->ErrId      =   MODULE_E_ERR_BEGIN;
            ps_Info->Para       =   0;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_W,(uint8_t*)ps_Info,NULL);
        }
    }
	*/
    // 全消息提取
    ps_Filter->BeginTime    =   0;
    ps_Filter->EndTime      =   0;
    ps_Filter->MaxNumber    =   0xFFFF;
    ps_Filter->Mode         =   2;
    ps_Filter->Flag_PrintToDebug    =   1;
    ps_Filter->Flag_PrintToMem      =   0;
    ps_Filter->Flag_Enable_Err      =   1;
    ps_Filter->Flag_Enable_Warn     =   1;
    ps_Filter->Flag_Enable_Info     =   1;
    ps_Filter->Flag_Enable_Debug    =   1;
    ps_Filter->Flag_res     =   1;
    ps_Filter->pInInclude   =   NULL;
    ps_Filter->pOutBuf      =   NULL;
    ps_Filter->IncludeNum   =   0;
    ps_Filter->OutBufNum    =   0;
    Module_Memory_App(MODULE_MEMORY_APP_CMD_INFO_R,NULL,ps_Filter);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ps_Filter);
    OnOff=OnOff;
}
void Module_Memory_DebugTestReadParaOnOff(uint8_t OnOff)
{
    uint8_t *pbuf;
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    sprintf((char*)pbuf,"Global Para(%d Byte):\r\n",sizeof(MODULE_MEMORY_S_PARA));
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Updata            :0x%lX(%ld)\r\n",ModuleMemory_psPara->Updata,ModuleMemory_psPara->Updata);
    DebugOutStr((int8_t*)pbuf);

    sprintf((char*)pbuf,"-> UpdataLen         :0x%lX(%ld)\r\n",ModuleMemory_psPara->UpdataLen,ModuleMemory_psPara->UpdataLen);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UpdataSum         :0x%lX(%ld)\r\n",ModuleMemory_psPara->UpdataSum,ModuleMemory_psPara->UpdataSum);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAlarm[0]       :%02d:%02d\r\n",ModuleMemory_psPara->RtcAlarm[0]>>8,ModuleMemory_psPara->RtcAlarm[0]&0x00FF);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAlarm[1]       :%02d:%02d\r\n",ModuleMemory_psPara->RtcAlarm[1]>>8,ModuleMemory_psPara->RtcAlarm[1]&0x00FF);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Addr              :0x%02X(%d)\r\n",ModuleMemory_psPara->Addr,ModuleMemory_psPara->Addr);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcFormat         :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcFormat,ModuleMemory_psPara->RtcFormat);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAlarmMode[0]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcAlarmMode[0],ModuleMemory_psPara->RtcAlarmMode[0]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAlarmMode[1]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcAlarmMode[1],ModuleMemory_psPara->RtcAlarmMode[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> LcdBackLight1     :0x%02X(%d)\r\n",ModuleMemory_psPara->LcdBackLight1,ModuleMemory_psPara->LcdBackLight1);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> LcdBackLight2     :0x%02X(%d)\r\n",ModuleMemory_psPara->LcdBackLight1,ModuleMemory_psPara->LcdBackLight1);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> LcdBackLight3     :0x%02X(%d)\r\n",ModuleMemory_psPara->LcdBackLight3,ModuleMemory_psPara->LcdBackLight3);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> LcdBackLight4     :0x%02X(%d)\r\n",ModuleMemory_psPara->LcdBackLight4,ModuleMemory_psPara->LcdBackLight4);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[0]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[0],ModuleMemory_psPara->UartBps[0]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[1]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[1],ModuleMemory_psPara->UartBps[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[2]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[2],ModuleMemory_psPara->UartBps[2]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[3]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[3],ModuleMemory_psPara->UartBps[3]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[4]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[4],ModuleMemory_psPara->UartBps[4]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[5]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[5],ModuleMemory_psPara->UartBps[5]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[6]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[6],ModuleMemory_psPara->UartBps[6]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> UartBps[7]        :0x%02X(%d)\r\n",ModuleMemory_psPara->UartBps[7],ModuleMemory_psPara->UartBps[7]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Rfms_SubThreshold :0x%04X(%d)\r\n",ModuleMemory_psPara->Rfms_SubThreshold,ModuleMemory_psPara->Rfms_SubThreshold);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Gsm_Apn           :0x%02X(%d)\r\n",ModuleMemory_psPara->Gsm_Apn,ModuleMemory_psPara->Gsm_Apn);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> MemoryDayNextP    :0x%02X(%d)\r\n",ModuleMemory_psPara->MemoryDayNextP,ModuleMemory_psPara->MemoryDayNextP);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAutoBeginTime  :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcAutoBeginTime,ModuleMemory_psPara->RtcAutoBeginTime);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAutoEndTime    :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcAutoEndTime,ModuleMemory_psPara->RtcAutoEndTime);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcAutoOnOff      :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcAutoOnOff,ModuleMemory_psPara->RtcAutoOnOff);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Flag_GprsRegister :%db\r\n",ModuleMemory_psPara->Flag_GprsRegister);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Flag_GprsUploadDayMove  :%db\r\n",ModuleMemory_psPara->Flag_GprsUploadDayMove);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Flag_GprsKeepOn   :%db\r\n",ModuleMemory_psPara->Flag_GprsKeepOn);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Flag_NightLight   :%db\r\n",ModuleMemory_psPara->Flag_NightLight);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnOnOff[0]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcWarnOnOff[0],ModuleMemory_psPara->RtcWarnOnOff[0]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnOnOff[1]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcWarnOnOff[1],ModuleMemory_psPara->RtcWarnOnOff[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnOnOff[2]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcWarnOnOff[2],ModuleMemory_psPara->RtcWarnOnOff[2]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnOnOff[3]   :0x%02X(%d)\r\n",ModuleMemory_psPara->RtcWarnOnOff[3],ModuleMemory_psPara->RtcWarnOnOff[3]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnTime[0]    :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcWarnTime[0],ModuleMemory_psPara->RtcWarnTime[0]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnTime[1]    :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcWarnTime[1],ModuleMemory_psPara->RtcWarnTime[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnTime[2]    :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcWarnTime[2],ModuleMemory_psPara->RtcWarnTime[2]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> RtcWarnTime[3]    :0x%04X(%d)\r\n",ModuleMemory_psPara->RtcWarnTime[3],ModuleMemory_psPara->RtcWarnTime[3]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> PowerOnCount      :0x%04X(%d)\r\n",ModuleMemory_psPara->PowerOnCount,ModuleMemory_psPara->PowerOnCount);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> ResetCount        :0x%04X(%d)\r\n",ModuleMemory_psPara->ResetCount,ModuleMemory_psPara->ResetCount);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Language          :0x%02X(%d)\r\n",ModuleMemory_psPara->Language,ModuleMemory_psPara->Language);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> HardFault_Count   :0x%02X(%d)\r\n",ModuleMemory_psPara->HardFault_Count,ModuleMemory_psPara->HardFault_Count);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> HardFault_Time    :%04d-%02d-%02d %02d:%02d:%02d\r\n"\
            ,ModuleMemory_psPara->HardFault_Time[0]+2000\
            ,ModuleMemory_psPara->HardFault_Time[1]\
            ,ModuleMemory_psPara->HardFault_Time[2]\
            ,ModuleMemory_psPara->HardFault_Time[3]\
            ,ModuleMemory_psPara->HardFault_Time[4]\
            ,ModuleMemory_psPara->HardFault_Time[5]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> *pHardFaultTaskName  :%s\r\n",ModuleMemory_psPara->pHardFaultTaskName);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Theme             :0x%02X(%d)\r\n",ModuleMemory_psPara->Theme,ModuleMemory_psPara->Theme);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> BootVer           :0x%02X(%d)\r\n",ModuleMemory_psPara->BootVer,ModuleMemory_psPara->BootVer);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> BootUpdataState   :0x%02X(%d)\r\n",ModuleMemory_psPara->BootUpdataState,ModuleMemory_psPara->BootUpdataState);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> GprsServer        :0x%02X(%d)\r\n",ModuleMemory_psPara->GprsServer,ModuleMemory_psPara->GprsServer);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> ThreshsholdValue1 :0x%08lX(%ld)\r\n",ModuleMemory_psPara->ThreshsholdValue1,ModuleMemory_psPara->ThreshsholdValue1);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> ThreshsholdValue2 :0x%08lX(%ld)\r\n",ModuleMemory_psPara->ThreshsholdValue2,ModuleMemory_psPara->ThreshsholdValue2);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> ThreshsholdValue3 :0x%04X(%d)\r\n",ModuleMemory_psPara->ThreshsholdValue3,ModuleMemory_psPara->ThreshsholdValue3);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> ThreshsholdValue4 :0x%04X(%d)\r\n",ModuleMemory_psPara->ThreshsholdValue4,ModuleMemory_psPara->ThreshsholdValue4);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Offset_H          :0x%02X(%d) 0x%02X(%d)\r\n",ModuleMemory_psPara->Offset_H[0],ModuleMemory_psPara->Offset_H[0],ModuleMemory_psPara->Offset_H[1],ModuleMemory_psPara->Offset_H[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Offset_L          :0x%02X(%d) 0x%02X(%d)\r\n",ModuleMemory_psPara->Offset_L[0],ModuleMemory_psPara->Offset_L[0],ModuleMemory_psPara->Offset_L[1],ModuleMemory_psPara->Offset_L[1]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Offset_TimerS     :0x%04X(%d)\r\n",ModuleMemory_psPara->Offset_TimerS,ModuleMemory_psPara->Offset_TimerS);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Reponse_TimerS    :0x%04X(%d)\r\n",ModuleMemory_psPara->Reponse_TimerS,ModuleMemory_psPara->Reponse_TimerS);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcX1     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcX1,ModuleMemory_psPara->Touch_usAdcX1);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcY1     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcY1,ModuleMemory_psPara->Touch_usAdcY1);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcX2     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcX2,ModuleMemory_psPara->Touch_usAdcX2);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcY2     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcY2,ModuleMemory_psPara->Touch_usAdcY2);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcX3     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcX3,ModuleMemory_psPara->Touch_usAdcX3);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcY3     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcY3,ModuleMemory_psPara->Touch_usAdcY3);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcX4     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcX4,ModuleMemory_psPara->Touch_usAdcX4);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_usAdcY4     :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_usAdcY4,ModuleMemory_psPara->Touch_usAdcY4);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> Touch_XYChange    :0x%04X(%d)\r\n",ModuleMemory_psPara->Touch_XYChange,ModuleMemory_psPara->Touch_XYChange);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"-> FactoryID[]       :0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n"\
            ,ModuleMemory_psPara->FactoryID[0]\
            ,ModuleMemory_psPara->FactoryID[1]\
            ,ModuleMemory_psPara->FactoryID[2]\
            ,ModuleMemory_psPara->FactoryID[3]\
            ,ModuleMemory_psPara->FactoryID[4]\
            ,ModuleMemory_psPara->FactoryID[5]\
            ,ModuleMemory_psPara->FactoryID[6]\
            ,ModuleMemory_psPara->FactoryID[7]\
            ,ModuleMemory_psPara->FactoryID[8]\
            ,ModuleMemory_psPara->FactoryID[9]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"                     :0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n"\
            ,ModuleMemory_psPara->FactoryID[10]\
            ,ModuleMemory_psPara->FactoryID[11]\
            ,ModuleMemory_psPara->FactoryID[12]\
            ,ModuleMemory_psPara->FactoryID[13]\
            ,ModuleMemory_psPara->FactoryID[14]\
            ,ModuleMemory_psPara->FactoryID[15]\
            ,ModuleMemory_psPara->FactoryID[16]\
            ,ModuleMemory_psPara->FactoryID[17]\
            ,ModuleMemory_psPara->FactoryID[18]\
            ,ModuleMemory_psPara->FactoryID[19]);
    DebugOutStr((int8_t*)pbuf);
    sprintf((char*)pbuf,"                     :0x%X 0x%X 0x%X 0x%X\r\n"\
            ,ModuleMemory_psPara->FactoryID[20]\
            ,ModuleMemory_psPara->FactoryID[21]\
            ,ModuleMemory_psPara->FactoryID[22]\
            ,ModuleMemory_psPara->FactoryID[23]);
    DebugOutStr((int8_t*)pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    OnOff=OnOff;
}
#endif
//----------------------------------------------------------------------------
