/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes --------------------
#include "includes.h"
#include "Module_SdFat.h"
#include "Bsp_Rtc.h"
#include "uctsk_Debug.h"
//------------------------------- 用户变量 --------------------
FIL      ModuleSdFat_fsrc;
FRESULT  ModuleSdFat_Res;
FATFS    ModuleSdFat_Fs;
static uint32_t  br;
static DIR     DirInf;
//------------------------------- 用户函数声明 ----------------
static void WriteFileTest(void);
static void ViewRootDir(void);
#if     (defined(NRF52))
/**
 * @brief  SDC block device definition
 * */
NRF_BLOCK_DEV_SDC_DEFINE(
    m_block_dev_sdc,
    NRF_BLOCK_DEV_SDC_CONFIG(
        SDC_SECTOR_SIZE,
        APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
    ),
    NFR_BLOCK_DEV_INFO_CONFIG("Nordic", "SDC", "1.00")
);
/**
 * @brief Function for demonstrating FAFTS usage.
 */
static void fatfs_example()
{
    static FATFS fs;
    static DIR dir;
    static FILINFO fno;
    static FIL file;

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    //申请缓存
    uint8_t *pbuf;
    pbuf = (uint8_t*)MemManager_Get(E_MEM_MANAGER_TYPE_256B);


    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
        DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    DebugOutStr("Initializing disk 0 (SDC)...\r\n");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        DebugOutStr("Disk initialization failed.\r\n");
        goto fatfs_example_goto;
    }

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    sprintf(pbuf,"Capacity: %d MB\r\n", capacity);
    DebugOutStr(pbuf);

    DebugOutStr("Mounting volume...\r\n");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        DebugOutStr("Mount failed.\r\n");
        goto fatfs_example_goto;
    }

    DebugOutStr("\r\n Listing directory: /\r\n");
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        DebugOutStr("Directory listing failed!\r\n");
        goto fatfs_example_goto;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            DebugOutStr("Directory read failed.");
            goto fatfs_example_goto;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                NRF_LOG_RAW_INFO("   <DIR>   %s\r\n",(uint32_t)fno.fname);
            }
            else
            {
                NRF_LOG_RAW_INFO("%9lu  %s\r\n", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("\r\n");

    DebugOutStr("Writing to file " FILE_NAME "...\r\n");
    ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        DebugOutStr("Unable to open or create file: " FILE_NAME ".\r\n");
        goto fatfs_example_goto;
    }

    ff_result = f_write(&file, TEST_STRING, sizeof(TEST_STRING) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        DebugOutStr("Write failed\r\n.");
    }
    else
    {
        sprintf(pbuf,"%d bytes written.\r\n", bytes_written);
        DebugOutStr(pbuf);
    }

    (void) f_close(&file);
fatfs_example_goto:
    MemManager_Free(E_MEM_MANAGER_TYPE_256B, pbuf);
    return;
}
#endif
//-------------------------------------------------------------------------------
//                     出口函数
//-------------------------------------------------------------------------------
/*******************************************************************************
函数功能: 调用于1mS中断
参    数: 无
返 回 值: 无
*******************************************************************************/
static uint32_t SdFat_msCount;
void SdFat_1msPro(void)
{
    if(SdFat_msCount!=0xFFFFFFFF)  //1mS计数器
    {
        SdFat_msCount++;
    }
}
/*******************************************************************************
函数功能: 主函数
参    数: 无
返 回 值: 无
*******************************************************************************/
uint8_t SdFat_App(MODULE_SDFAT_E_APP_CMD cmd,char *pPathFileName,void *pbuf,uint32_t *pPos,uint16_t *plen,uint32_t *pbw)
{
    uint8_t res=OK,res1=OK;
    cmd   =  cmd;
    pPathFileName  =  pPathFileName;
    pbuf  =  pbuf;
    pPos  =  pPos;
    plen  =  plen;
    //挂载磁盘
    res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
    if( res != FR_OK )
    {
        return ERR;
    }
    //解析指令
    switch(cmd)
    {
        case MODULE_SDFAT_E_APP_CMD_FILE_R:
            res = f_open(&ModuleSdFat_fsrc, (char*)pPathFileName, FA_OPEN_EXISTING | FA_READ);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_lseek(&ModuleSdFat_fsrc, *pPos);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_read(&ModuleSdFat_fsrc, pbuf, *plen, pbw);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_close(&ModuleSdFat_fsrc);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            break;
        case MODULE_SDFAT_E_APP_CMD_FILE_W:
            res = f_open(&ModuleSdFat_fsrc, (char*)pPathFileName, FA_CREATE_ALWAYS | FA_WRITE);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_write(&ModuleSdFat_fsrc, pbuf, *plen, pbw);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_close(&ModuleSdFat_fsrc);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            break;
        case MODULE_SDFAT_E_APP_CMD_FILE_ADD:
            res = f_open(&ModuleSdFat_fsrc, (char*)pPathFileName, FA_OPEN_EXISTING | FA_WRITE);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_lseek(&ModuleSdFat_fsrc, f_size(&ModuleSdFat_fsrc));
            //
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_write(&ModuleSdFat_fsrc, pbuf, *plen, pbw);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            //
            res = f_close(&ModuleSdFat_fsrc);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            break;
        case MODULE_SDFAT_E_APP_CMD_FILE_GETSIZE:
            res = f_open(&ModuleSdFat_fsrc, (char*)pPathFileName, FA_OPEN_EXISTING | FA_READ);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            *pPos = f_size(&ModuleSdFat_fsrc);
            res = f_close(&ModuleSdFat_fsrc);
            if (res !=  FR_OK)
            {
                res1  =  ERR;
                break;
            }
            break;
        case MODULE_SDFAT_E_APP_CMD_FORMAT:
            break;
        default:
            break;
    }
    //卸载磁盘
    res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
    \

    if(res1 != 0)
    {
        return ERR;
    }
    if((res != FR_OK))
    {
        res1 = ERR;
    }
    else
    {
        res1 = OK;
    }
    return res1;
}
/*******************************************************************************
* Description    : 文件空间占用情况
* Input          : None
* Output         : None
* Return         : 返回1成功 返回0失败
* Attention      : None
*******************************************************************************/
FRESULT res;
void SdFat_Test(uint8_t OnOff)
{
    //--------------------初始化SD卡
    //--------------------截取数据
    /*
    res = f_open( &ModuleSdFat_fsrc , "0:/WRITE.TXT" , FA_READ | FA_WRITE);
    if(res == FR_OK)
      {
          res = f_lseek(&ModuleSdFat_fsrc, 0x14);
            res = f_truncate(&ModuleSdFat_fsrc);
            //关闭文件
      f_close(&ModuleSdFat_fsrc);
            Display_Str("截取数据成功! \r\n");
        }
    else
      {
          Display_Str("截取数据成功! \r\n");
        }
    */
    //--------------------读音频文件
    /*
    TIM3_PWM_WAV_INIT();
    res = f_open( &ModuleSdFat_fsrc , "0:/1.WAV" , FA_OPEN_EXISTING | FA_READ);
                if(res == FR_OK)
            {
                  while(1)
                    {
                res = f_read(&ModuleSdFat_fsrc, textFileBuffer, 1, &br);
                                if (res || br == 0) break;   // error or eof
                                i = (textFileBuffer[0]*1000)/256;
                                PWM_Pulse_f(i);
                                Display_Char(textFileBuffer[0]);
              }
                  //关闭文件
            f_close(&ModuleSdFat_fsrc);
              }
          else
            {
                Display_Str("Can't open WRITE.TXT \r\n");
              }
    */
    //--------------------
    char const *TEST_FILE_NAME="/Test.TXT";

    FATFS *pfs;
    DWORD fre_clust;
    //申请缓存
    uint8_t *pbuf;
#if     (defined(NRF52))
    fatfs_example();
#endif
    pbuf = (uint8_t*)MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
    //
    DebugOutStr("-----TF Test Begin!\r\n");
    //挂载
    res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
    if( res == FR_OK )
    {
        DebugOutStr("TF mount OK!\r\n");
    }
    else
    {
        DebugOutStr("TF mount ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //容量统计
    res = f_getfree("", &fre_clust, &pfs);  /* 必须是根目录，选择磁盘0 */
    if ( res==FR_OK )
    {
        DebugOutStr("TF size measure OK!\r\n");
        // Print free space in unit of MB (assuming 512 bytes/sector)
        sprintf((char*)pbuf,"TF All Size:%d MB.\r\nTF Free Size:%d MB available.\r\n",
                ( (pfs->n_fatent - 2) * pfs->csize ) / 2 /1024 , (fre_clust * pfs->csize) / 2 /1024 );
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        DebugOutStr("TF size measure ERR!\r\n");
    }
    DebugOutStr("-----TF Test New|open file!\r\n");
    //创建文件
    res = f_open( &ModuleSdFat_fsrc , (char*)TEST_FILE_NAME , FA_CREATE_ALWAYS | FA_WRITE);
    if( res == FR_OK )
    {
        DebugOutStr("TF file Create OK!\r\n");
    }
    else if ( res == FR_EXIST )
    {
        DebugOutStr("TF file open OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file open ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //向文件写数据
    BspRtc_ReadRealTime(NULL,NULL,NULL,&pbuf[100]);
    sprintf((char *)pbuf,"%04d-%02d-%02d %02d:%02d:%02d 欢迎进入TF卡测试系统!\r\n",\
            pbuf[100]+2000,pbuf[101],pbuf[102],pbuf[103],pbuf[104],pbuf[105]);
    res = f_write(&ModuleSdFat_fsrc, pbuf, strlen((const char*)pbuf), &br);
    if( res == FR_OK )
    {
        DebugOutStr("TF file write OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file write ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //关闭文件
    res = f_close(&ModuleSdFat_fsrc);
    if( res == FR_OK )
    {
        DebugOutStr("TF file close OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file close ERR!\r\n");
        goto SdFat_Test_goto;
    }
    DebugOutStr("-----TF Test add data!\r\n");
    res = f_open(&ModuleSdFat_fsrc, (char *)TEST_FILE_NAME, FA_OPEN_ALWAYS | FA_WRITE);
    //追加数据
    if(res == FR_OK)
    {
        DebugOutStr("TF file create OK!\r\n");
    }
    else if ( res == FR_EXIST )
    {
        DebugOutStr("TF file open OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file open ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //指针后移3000字节
    //res = f_lseek(&ModuleSdFat_fsrc, f_tell(&fsrc) + 3000);
    //指针前移3000字节
    //res = f_lseek(&ModuleSdFat_fsrc, f_tell(&fsrc) - 2000);
    //指针移动到文件结尾
    res = f_lseek(&ModuleSdFat_fsrc, f_size(&ModuleSdFat_fsrc));
    //向文件写数据
    BspRtc_ReadRealTime(NULL,NULL,NULL,&pbuf[100]);
    sprintf((char *)pbuf,"%04d-%02d-%02d %02d:%02d:%02d 追加数据!\r\n",\
            pbuf[100]+2000,pbuf[101],pbuf[102],pbuf[103],pbuf[104],pbuf[105]);
    res = f_write(&ModuleSdFat_fsrc, pbuf, strlen((char*)pbuf), &br);
    if(res == FR_OK)
    {
        DebugOutStr("TF file add data OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file add data ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //关闭文件
    res = f_close(&ModuleSdFat_fsrc);
    if( res == FR_OK )
    {
        DebugOutStr("TF file close OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file close ERR!\r\n");
        goto SdFat_Test_goto;
    }
    DebugOutStr("-----TF Test read data!\r\n");
    //创建文件
    res = f_open( &ModuleSdFat_fsrc , (char*)TEST_FILE_NAME , FA_OPEN_ALWAYS | FA_READ);
    if( res == FR_OK )
    {
        DebugOutStr("TF file open OK!\r\n");
    }
    else if ( res == FR_EXIST )
    {
        DebugOutStr("TF file open OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file open ERR!\r\n");
        goto SdFat_Test_goto;
    }
    memset(pbuf,0,256);
    res = f_read(&ModuleSdFat_fsrc, pbuf, 256, &br);
    pbuf[255]=0;
    if(res == FR_OK)
    {
        DebugOutStr("TF file Read OK!\r\n");
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        DebugOutStr("TF file Read ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //关闭文件
    res = f_close(&ModuleSdFat_fsrc);
    if( res == FR_OK )
    {
        DebugOutStr("TF file close OK!\r\n");
    }
    else
    {
        DebugOutStr("TF file close ERR!\r\n");
        goto SdFat_Test_goto;
    }
    //读写速度测试
    WriteFileTest();
    //删除文件测试
    DebugOutStr("-----TF Test Delete file!\r\n");
    //-----容量统计
    res = f_getfree("", &fre_clust, &pfs);  /* 必须是根目录，选择磁盘0 */
    if ( res==FR_OK )
    {
        // Print free space in unit of MB (assuming 512 bytes/sector)
        sprintf((char*)pbuf,"TF All Size:%d MB.\r\nTF Free Size:%d MB available.\r\n",
                ( (pfs->n_fatent - 2) * pfs->csize ) / 2 /1024 , (fre_clust * pfs->csize) / 2 /1024 );
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        DebugOutStr("TF size measure ERR!\r\n");
    }
    //-----扫描根目录
    ViewRootDir();
    //-----删除文件
    res = f_unlink ("/Speed.txt");
    if ( res==FR_OK )
    {
        sprintf((char*)pbuf,"/Speed.txt Delete OK.\r\n");
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        sprintf((char*)pbuf,"/Speed.txt Delete ERR.\r\n");
        DebugOutStr((int8_t*)pbuf);
    }
    //-----容量统计
    res = f_getfree("", &fre_clust, &pfs);  /* 必须是根目录，选择磁盘0 */
    if ( res==FR_OK )
    {
        // Print free space in unit of MB (assuming 512 bytes/sector)
        sprintf((char*)pbuf,"TF All Size:%d MB.\r\nTF Free Size:%d MB available.\r\n",
                ( (pfs->n_fatent - 2) * pfs->csize ) / 2 /1024 , (fre_clust * pfs->csize) / 2 /1024 );
        DebugOutStr((int8_t*)pbuf);
    }
    else
    {
        DebugOutStr("TF size measure ERR!\r\n");
    }
    //-----扫描根目录
    ViewRootDir();
    //-----
    //扫描所有文件
    //scan_files(path);
SdFat_Test_goto:
    DebugOutStr("\r\n");
    //卸载磁盘
    f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
    //
    DebugOutStr("-----TF Test End!\r\n");
    OnOff = OnOff;
}
//------------------------------------------------------------------------------测试函数
/*******************************************************************************
* Function Name  : scan_files
* Description    : 搜索文件目录下所有文件
* Input          : - path: 根目录
* Output         : None
* Return         : FRESULT
* Attention      : 不支持长文件名
*******************************************************************************/
#if 0
#define _DF1S   0x81
static FRESULT scan_files_1 (char* path)
{
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;
    FRESULT res;
    //
#if _USE_LFN
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        i = strlen(path);
        for (;;)
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            //It is a dir.
            if (fno.fattrib & AM_DIR)
            {
                sprintf(&path[i], "/%s", fn);
                //res = scan_files(path);
                path[i] = 0;
            }
            //It is a file.
            else
            {
                printf("%s/%s\n", path, fn);
            }
        }
        //f_closedir(&dir);
    }
    return res;
}

static FRESULT scan_files (char* path)
{
    FRESULT res;
    printf("-----TF scan file dir\r\n");
    path[0]=0;
    while(1)
    {
        res = scan_files_1(path);
        if (res != FR_OK)
        {
            break;
        }
    }
    printf("-----\r\n");
    return FR_OK;
}
#endif
//------------------------------------------------------------------------------
/*
********************************************************************************
*   函 数 名: ViewRootDir
*   功能说明: 显示SD卡根目录下的文件名
*   形    参：无
*   返 回 值: 无
********************************************************************************
*/
static void ViewRootDir(void)
{
    /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
    FRESULT result;
    FILINFO FileInf;
    uint32_t cnt = 0;
    //char lfname[256];
    DebugOutStr("-----扫描根目录\r\n");
    /* 挂载文件系统 */
    /*
    result = f_mount(&fs,MODULE_SDFAT_SD_PATH,0);
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%d)\r\n", result);
    }
    */
    /* 打开根文件夹 */
    result = f_opendir(&DirInf, "/"); /* 如果不带参数，则从当前目录开始 */
    if (result != FR_OK)
    {
        printf("打开根目录失败 (%d)\r\n", result);
        return;
    }

    /* 读取当前文件夹下的文件和目录 */
    //FileInf.lfname = lfname;
    //FileInf.lfsize = 256;

    DebugOutStr("属性        |  文件大小 | 短文件名 \r\n");
    for (cnt = 0; ; cnt++)
    {
        result = f_readdir(&DirInf,&FileInf);       /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {
            continue;
        }

        /* 判断是文件还是子目录 */
        if (FileInf.fattrib & AM_DIR)
        {
            printf("(0x%02d)目录  ", FileInf.fattrib);
        }
        else
        {
            printf("(0x%02d)文件  ", FileInf.fattrib);
        }

        /* 打印文件大小, 最大4G */
        printf(" %10d", FileInf.fsize);

        printf("  %s\r\n", FileInf.fname);  /* 短文件名 */

        //printf("  %s\r\n", (char *)FileInf.lfname);   /* 长文件名 */
    }

    /* 卸载文件系统 */
    //f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
}
/*
*********************************************************************************************************
*   函 数 名: WriteFileTest
*   功能说明: 测试文件读写速度
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
/* 用于测试读写速度 */
#define TEST_FILE_LEN           (2*1024*1024L)    /* 用于测试的文件长度 */
#define BUF_SIZE                512/*(4*1024)*/    /*用内存管理不能大于1K*/ /* 每次读写SD卡的最大数据长度(XSL目前STM32F1XX程序的SD卡驱动写多个数据块有问题,so大于512不好使) */
//uint8_t g_TestBuf[BUF_SIZE];
static void WriteFileTest(void)
{
    /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
    uint32_t bw;
    uint32_t i32,j32;
    uint32_t runtime1,runtime2,timelen;
    uint8_t err = 0;
    FRESULT res;
    //申请内存
    uint8_t *textFileBuffer;
    uint32_t *p32;
    textFileBuffer = (uint8_t*)MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    p32 = MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
    //
    printf("-----测试文件读写速度\r\n");

    SdFat_msCount=0;
    /* 挂载文件系统 */
    /*
    res = f_mount(&fs,MODULE_SDFAT_SD_PATH,0);
    if (res != FR_OK)
    {
        printf("挂载文件系统失败 (%d)\r\n", res);
    }
    */
    /* 打开根文件夹 */
    res = f_opendir(&DirInf, "/");
    if (res != FR_OK)
    {
        printf("打开根目录失败 (%d)\r\n", res);
        goto goto_WriteFileTestEnd;
    }
    sprintf((char *)textFileBuffer, "Speed.txt");
    /* 打开文件 */
    res = f_open(&ModuleSdFat_fsrc, (char *)textFileBuffer, FA_CREATE_ALWAYS | FA_WRITE);

    /* 写一串数据 */
    printf("开始写文件%s %dKB ...\r\n", (char *)textFileBuffer, TEST_FILE_LEN / 1024);
    runtime1 = SdFat_msCount;   /* 读取系统运行时间 */
    for (i32 = 0; i32 < TEST_FILE_LEN / BUF_SIZE; i32++)
    {
        //
        for(j32=0; j32<(BUF_SIZE/4); j32++)
        {
            p32[j32]=i32*BUF_SIZE+j32;
        }
        //
        res = f_write(&ModuleSdFat_fsrc, p32, BUF_SIZE, &bw);
        if (res == FR_OK)
        {
            if (((i32 + 1) % (128*(1024/BUF_SIZE))) == 0)
            {
                printf("%d(KB)\r\n",(i32+1)/(1024/BUF_SIZE));
            }
        }
        else
        {
            err = 1;
            printf("%s文件写失败\r\n", textFileBuffer);
            break;
        }
    }
    runtime2 = SdFat_msCount;   /* 读取系统运行时间 */

    if (err == 0)
    {
        timelen = (runtime2 - runtime1);
        printf("\r\n  写耗时 : %dms   平均写速度 : %dKB/S\r\n",\
               timelen,\
               ((TEST_FILE_LEN / 1024) * 1000) / timelen);
    }
    //关闭文件
    f_close(&ModuleSdFat_fsrc);

    /* 开始读文件测试 */
    res = f_open(&ModuleSdFat_fsrc, (char*)textFileBuffer, FA_OPEN_EXISTING | FA_READ);
    if (res !=  FR_OK)
    {
        printf("没有找到文件: %s\r\n", textFileBuffer);
        goto goto_WriteFileTestEnd;
    }
    printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);
    runtime1 = SdFat_msCount;   /* 读取系统运行时间 */
    for (i32 = 0; i32 < TEST_FILE_LEN / BUF_SIZE; i32++)
    {
        res = f_read(&ModuleSdFat_fsrc, p32, BUF_SIZE, &bw);
        if (res == FR_OK)
        {
            if (((i32 + 1) % (128*(1024/BUF_SIZE))) == 0)
            {
                printf("%d(KB)\r\n",(i32+1)/(1024/BUF_SIZE));
            }
            /* 比较写入的数据是否正确，此语句会导致读卡速度结果降低到 3.5MBytes/S */
            for (j32 = 0; j32 < (BUF_SIZE/4); j32++)
            {
                if (p32[j32] != (i32*BUF_SIZE+j32))
                {
                    err = 1;
                    printf("%s文件读成功，但是数据出错\r\n", textFileBuffer);
                    break;
                }
            }
            if (err == 1)
            {
                break;
            }
        }
        else
        {
            err = 1;
            printf("%s文件读失败(%02d)\r\n", textFileBuffer,res);
            break;
        }
    }
    runtime2 = SdFat_msCount;   /* 读取系统运行时间 */

    if (err == 0)
    {
        timelen = (runtime2 - runtime1);
        printf("\r\n  读耗时 : %dms   平均读速度 : %dKB/S\r\n", timelen,
               ((TEST_FILE_LEN / 1024) * 1000) / timelen);
    }

    // 关闭文件
    f_close(&ModuleSdFat_fsrc);
    // 卸载文件系统
    //f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
goto_WriteFileTestEnd:
    //释放内存
    MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,p32);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,textFileBuffer);
}
//------------------------------------------------------------------------------
