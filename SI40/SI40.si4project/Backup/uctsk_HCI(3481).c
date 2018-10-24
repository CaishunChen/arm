/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
/*********************************************************************************
说明: 人机界面任务(HCI-->human-computer interface)
**********************************************************************************/
//-------------------加载库函数------------------------------
#include "uctsk_HCI.h"
#if   (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
//---->
#include "uctsk_Sensor.h"
#include "Bsp_Key.h"
#include "Bsp_Rtc.h"
#include "Language.h"
#include "uctsk_AD.h"
#include "Bsp_CpuId.h"
#include "uctsk_GprsNet.h"
#include "Bsp_CpuFlash.h"
#include "Module_Memory.h"
#include "Module_Err.h"
#include "Language.h"
#include "bsp_tft_lcd.h"
#include "uctsk_RFMS.h"
#include "Bsp_BuzzerMusic.h"
#include "uctsk_BluetoothDTU.h"
#include "IAP.h"
#include "uctsk_GprsAppXkap.h"
#include "Module_SdFat.h"
#include "Bsp_ExtFlash.h"
#include "Bsp_Pwr.h"
#include "Bsp_BkpRam.h"
//#include "uctsk_GasModule.h"
//
#include "LCD_ILI9341.h"

#include "uctsk_Debug.h"
//------------------------------- 引用变量 ---------------------
extern unsigned char Bmp_Bell[];
extern MODULE_MEMORY_S_DAY_INFO        uctsk_Rfms_s_DayInfo;
extern MODULE_MEMORY_S_DAY_SLEEPLEVEL  uctsk_Rfms_s_DaySleepLevel;
extern MODULE_MEMORY_S_DAY_BREATH      uctsk_Rfms_s_DayBreath;
extern MODULE_MEMORY_S_DAY_HEARTRATE   uctsk_Rfms_s_HeartRate;
extern MODULE_MEMORY_S_DAY_BODYMOVE    uctsk_Rfms_s_BodyMove;
//------------------------------- 静态函数 -------------------
static void uctsk_Hci(void *pvParameters);
static uint8_t Hci_Menu_Ask(uint16_t MaxTimeS,uint8_t KeyEnable,char *ptitle,char *ptext);
static uint8_t Hci_Menu_HintAsk(uint16_t MaxTimeS,uint8_t KeyEnable,char *ptitle,char *ptext);
static void Hci_Menu_BootScreen(void);
static void Hci_Menu_SlefCheck(void);
static void Hci_Menu_SOS(void);
static void Hci_Menu_I(void);
static void Hci_Menu_II_Info(void);
static void Hci_Menu_II_Set(void);
static void Hci_Menu_II_Test(void);
static void Hci_Menu_II_DataSaveAndLoad(void);
static void Hci_Menu_InfoIAP(void);
static void Hci_Menu_IapToUser(void);
static void Hci_Menu_SetTime(void);
static void Hci_Menu_SetBackLight(void);
static void Hci_Menu_SetMobileModule(void);
static void Hci_Menu_SetLanguage(void);
static void Hci_Menu_SetFunctionOnOff(void);
//static void Hci_Menu_SetTheme(void);
static void Hci_Menu_TestLcd(void);
static void Hci_Menu_InfoChip(void);
static void Hci_Menu_InfoVer(void);
static void Hci_Menu_InfoOS(void);
static void Hci_Menu_InfoMobileModule(void);
static void Hci_Menu_InfoRfms(void);
static void Hci_Menu_InfoSleepBeginTime(void);
static void Hci_Menu_InfoBluetooth(void);
static void Hci_Menu_InfoBluetooth_Slave(void);
static void Hci_Menu_InfoQrCode(void);
static void Hci_Menu_InfoSalesQrCode(void);
static void Hci_Menu_InfoFactoryIDQrCode(void);
static void Hci_Menu_InfoIMEIQrCode(void);
//static void Hci_Menu_InfoGasModule(void);
static void Hci_Menu_TestBuzzer(void);
static void Hci_Menu_TestGsm(void);
//
static void Hci_Menu_SetBluetoothIAP(void);
static void Hci_Menu_SetMenu(void);
static void Hci_Menu_SleepCurve(void);
static void Hci_Menu_BreathHeartCurve(void);
static void Hci_Draw_Breath(MODULE_MEMORY_S_DAY_INFO *psDayInfo,MODULE_MEMORY_S_DAY_BREATH *psDayBreath,MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove);
static void Hci_Draw_Heart(MODULE_MEMORY_S_DAY_INFO *psDayInfo,MODULE_MEMORY_S_DAY_HEARTRATE *psDayHeartRate,MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove);
static void Hci_Draw_HBCoordinate(uint16_t X_Coordinate, uint16_t Y_Coordinate, uint8_t Begintime, uint8_t Endtime);
static void Hci_Menu_DayMoveCurve(void);
static uint8_t Hci_SleepDataValid(uint8_t *pbuf, uint16_t len);
static void Hci_Draw_SleepCoordinate(uint16_t X_Coordinate, uint16_t Y_Coordinate, uint8_t Begintime, uint8_t Endtime);
#ifdef HCI_EMWIN_ENABLE
static void Hci_Menu_Default1(void);
static void Hci_Menu_SleepMark(void);
static void Hci_CbBkWindow(WM_MESSAGE* pMsg);
#endif
//-------------------静态变量--------------------------------
MODULE_OS_TASK_TAB(App_TaskHciTCB);
MODULE_OS_TASK_STK(App_TaskHciStk,APP_TASK_HCI_STK_SIZE);
//应用层使用
static void (*pHci_MenuFun)(void);
static void (*pHci_DefaultMenuFun)(void);
//---用于刷新计时
static  uint16_t  Hci_ReShowTimer      =  0;
//---用于返回计时
static  uint16_t  Hci_ReturnTimer      =  0;
//---用于参数修改标记
static  uint8_t   Hci_ParaChangeSign   =  0;
//---用于光标位置
static  uint8_t   Hci_cursor           =  0;
//---用于光标闪烁计时
static  uint8_t   Hci_CursorFlick      =  0;
//---用于参数加载
//---用于表示第一次进入函数(表达指针转换)
static  uint8_t   Hci_NoFirst          =  0;
//---通用指针
static  uint8_t   *Hci_pbuf            =  NULL;
//---供电源
BSP_PWR_E_POWER   Hci_Power            =  BSP_PWR_E_POWER_NULL;
static  uint8_t   Hci_PowerStopTimerS  =  0;
static  uint8_t   Hci_PowerLowCmt      =  0;
//---IAP提示
static  uint8_t   Hci_IapHint          =  1;
//---被哪个入口调用(0-默认界面 1-info 2-set 3-test)
static  uint8_t   Hci_CalledFun        =  0;
//---
INT16_S_BIT Hci_s_Bit_hint;
//
static uint8_t curve_cnt = 1;
static uint8_t Hci_FlashDataSign=0;
//---显示整日体动页面索引号(0-6)
static uint8_t Hci_DayMoveCnt=0;
static uint8_t Hci_DayMoveDate[3]= {0};
//------------------------------- 外部变量 -------------------
#ifdef HCI_EMWIN_ENABLE
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_DQHT_18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_DQHT_24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_HNLP_18;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_HNLP_24;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_HNLP_66;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_HNLP_72;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_ST_16;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontFont_ST_24;
#endif
/*******************************************************************************
函数功能: Hci任务创建
*******************************************************************************/
void  App_HciTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Hci",\
                          uctsk_Hci,\
                          APP_TASK_HCI_PRIO,\
                          App_TaskHciStk,\
                          APP_TASK_HCI_STK_SIZE,\
                          App_TaskHciTCB,\
                          NULL);
}
/*******************************************************************************
函数功能: LCD初始化到开机画面
*******************************************************************************/
void  App_Hci_Init (void)
{
#ifdef   HCI_EMWIN_ENABLE
    LCD_InitHard();
    GUI_Init();
    //使能中文显示
    GUI_UC_SetEncodeUTF8();
    //光标显示
    //GUI_CURSOR_Show();
#ifdef XKAP_ICARE_B_D_M
#else
    if (BSP_BKPRAM_READ(BSP_BKPRAM_WAKE_UP) == 0x0000)
#endif
    {
        //挂载磁盘
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        if(FR_OK!=f_open(&ModuleSdFat_fsrc, "BS.bmp", FA_OPEN_EXISTING | FA_READ))
        {
#if   ((HARDWARE_VER==1) || (HARDWARE_VER==7) || (HARDWARE_VER==8) || (HARDWARE_VER==9))
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres36;
                GUI_DrawBitmap(&bmres36,0,0);
            }
#elif (HARDWARE_VER==2)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres36;
                GUI_DrawBitmap(&bmres36,0,0);
            }
#elif (HARDWARE_VER==4)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres21;
                GUI_DrawBitmap(&bmres21,0,0);
            }
#elif (HARDWARE_VER==5)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres28;
                GUI_DrawBitmap(&bmres28,0,0);
            }
#elif (HARDWARE_VER==6)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres31;
                GUI_DrawBitmap(&bmres31,0,0);
            }
#endif
        }
        //卸载磁盘
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
    }
#else
    LCD_InitHard();
#endif
    //等待AD 判断供电方式
    Hci_Power = Bsp_Pwr_Monitor();
    if(Hci_Power==BSP_PWR_E_POWER_AC)
    {
        GprsNet_OnOff(ON);
    }
    //参数装载
    {
        MODULE_MEMORY_S_PARA *pPara;
        uint8_t res;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        //背光
        if(pPara->LcdBackLight1<10)
        {
            pPara->LcdBackLight1=10;
        }
        res=pPara->LcdBackLight1;
        if(Hci_Power != BSP_PWR_E_POWER_AC && res>BSP_PWR_LIGHT_LEVEL)
        {
            res=BSP_PWR_LIGHT_LEVEL;
        }
        LCD_SetBackLight(res);
        //主题
        switch(pPara->Theme)
        {
            case 0:
                pHci_DefaultMenuFun=Hci_Menu_Default1;
                break;
            case 1:
#ifdef HCI_EMWIN_ENABLE
                pHci_DefaultMenuFun=Hci_Menu_Default1;
#else
                pHci_DefaultMenuFun=Hci_Menu_Default1;
#endif
                break;
            default:
#ifdef HCI_EMWIN_ENABLE
                pPara->Theme=1;
                pHci_DefaultMenuFun=Hci_Menu_Default1;
#else
                pPara->Theme=0;
                pHci_DefaultMenuFun=Hci_Menu_Default1;
#endif
                break;
        }
        //
        if(BSP_BKPRAM_READ(BSP_BKPRAM_POWER_CMT)<=3)
        {
            Hci_NoFirst=1;
        }
        else
        {
            Hci_NoFirst=0;
        }
        //
        if(pPara->BootUpdataState==1)
        {
            pPara->BootUpdataState=0;
            LCD_ClrScr(CL_BLACK);
            Hci_Menu_InfoVer();
            MODULE_OS_DELAY_MS(10000);
        }
        //
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    }
}
/*******************************************************************************
函数功能: HCI任务实体
*******************************************************************************/
static void uctsk_Hci (void *pvParameters)
{
    uint16_t i16=0;
    i16=i16;
    //防止编译警告
    Hci_ReturnTimer=Hci_ReturnTimer;
    //
    //App_Hci_Init();
    //变量初始化
    pHci_MenuFun=Hci_Menu_BootScreen;
    memset((char*)&Hci_s_Bit_hint,0,sizeof(Hci_s_Bit_hint));
    //
    for(;;)
    {
        MODULE_OS_DELAY_MS(10);
#if (HARDWARE_VER == 8)
        //
        if(BspKey_NewSign==1)
        {
            //软关机
            if((BspKey_KeepTimer_ms>=2000) && (BspKey_Value == HCI_KEY_RIGHT))
            {
                Bsp_Pwr_EnterStop();
            }
        }
#endif
        //-----------------------
        (*pHci_MenuFun)();
#if (HARDWARE_VER==0xF0)
        // 周期性重启GPRS
        if(i16==100*40)
        {
            GprsNet_OnOff(OFF);
            i16++;
        }
        else if(i16==100*60)
        {
            GprsNet_OnOff(ON);
            i16=0;
        }
        else
        {
            i16++;
        }
#else
        // 2s周期做LCD监控
        if(i16>100*2)
        {
            i16=0;
            if(ERR==LCD_ErrReset())
            {
                Hci_ReShowTimer =  0;
                Hci_ParaChangeSign=0;
                Hci_CursorFlick =  0;
                Hci_ReturnTimer =  0;
                BspKey_NewSign  =  0;
                Hci_NoFirst     =  0;
                if(Hci_pbuf!=NULL)
                {
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                }
                MODULE_OS_DELAY_MS(100);
            }
        }
        else
        {
            i16++;
        }
#endif
    }
}
/*******************************************************************************
* 函数功能: 询问窗口
*******************************************************************************/
static uint8_t Hci_Menu_Ask(uint16_t MaxTimeS,uint8_t KeyEnable,char *ptitle,char *ptext)
{
    WIN_T *pWinT;
    uint16_t time_10ms=0;
    uint8_t first=1;
    FONT_T sfont;
    sfont.FontCode   =  FC_ST_16;
    sfont.FrontColor =  CL_YELLOW;
    sfont.BackColor  =  CL_BLACK;
    sfont.Space=0;

    //清屏
    LCD_ClrScr(CL_BLACK);
    //
    while(1)
    {
        //
        MODULE_OS_DELAY_MS(100);
        //
        if(time_10ms>10*MaxTimeS)
        {
            Hci_ReShowTimer   =  0;
            //清屏
            sfont.BackColor   =  CL_BLACK;
            LCD_ClrScr(sfont.BackColor);
            first = BspKey_Value;
            BspKey_Value   =  0;
            BspKey_NewSign  =  0;
            Hci_ReShowTimer =  0;
            return first;
        }
        else
        {
            time_10ms++;
        }
        if(first==1)
        {
            first=0;
            BspKey_Value = 0;
            pWinT=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pWinT->id    =  ID_WIN;
            pWinT->Height=  g_LcdHeight*3/4;
            pWinT->Width =  g_LcdWidth*3/4;
            pWinT->Left  =  g_LcdWidth/8;
            pWinT->Top   =  g_LcdHeight/8;
            sfont.FontCode=0;
            sfont.FrontColor  =  CL_BLACK;
            sfont.BackColor   =  WIN_TITLE_COLOR;
            sfont.Space=0;
            pWinT->Font       =  &sfont;
            pWinT->pCaption   =  ptitle;
            pWinT->pText      =  ptext;
            LCD_DispControl(pWinT);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pWinT);
#ifdef HCI_EMWIN_ENABLE
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres10;
                GUI_DrawBitmap(&bmres10,(320-72)/2,(240-72)/2);
            }
#endif
        }
        //按键处理
        if(BspKey_NewSign==1 && KeyEnable==1)
        {
            BspKey_NewSign  =  0;
            Hci_ReShowTimer =  0;
            switch(BspKey_Value)
            {
                case HCI_KEY_ENTER:
                case HCI_KEY_ESC:
                case HCI_KEY_LEFT:
                case HCI_KEY_RIGHT:
                case HCI_KEY_TOP:
                    time_10ms=0xFFFF;
                    break;
                default:
                    break;
            }
        }
    }
}
/*******************************************************************************
* 函数功能: 提示询问窗口
*******************************************************************************/
//标题背景色
#define HCI_MENU_HintAsk_COLOR_1    Count_4ByteToLong(0,0xB7,0x69,0)
//正文背景
#define HCI_MENU_HintAsk_COLOR_2    Count_4ByteToLong(0,0xEF,0xFB,0xFF)
//标题字体
#define HCI_MENU_HintAsk_COLOR_3    Count_4ByteToLong(0,0x70,0xF0,0xFF)
//正文
#define HCI_MENU_HintAsk_COLOR_4    Count_4ByteToLong(0,0x53,0x106,0x0)
//
#define HCI_MENU_HintAsk_WIN_X1     (100-6)
#define HCI_MENU_HintAsk_WIN_Y1     (40-2)
#define HCI_MENU_HintAsk_WIN_W      (200+8+4)
#define HCI_MENU_HintAsk_WIN_H1     (30+2)
#define HCI_MENU_HintAsk_WIN_H2     (140-8+2+2)

static uint8_t Hci_Menu_HintAsk(uint16_t MaxTimeS,uint8_t KeyEnable,char *ptitle,char *ptext)
{
    uint16_t time_10ms=0;
    uint8_t first=1;
    uint8_t i=0,j=0;
    uint16_t xpos,ypos;
    char *pbuf;
    //清屏
    LCD_ClrScr(CL_BLACK);
#if (HARDWARE_VER==7)
    BspKey_NewSign  =  0;
    Hci_ReShowTimer =  0;
    return HCI_KEY_ESC;
#endif
    //画小护士
    {
        extern GUI_CONST_STORAGE GUI_BITMAP bmres25;
        GUI_DrawBitmap(&bmres25,20-6,60);
    }
    /*
    {
        extern GUI_CONST_STORAGE GUI_BITMAP bmres27;
        GUI_DrawBitmap(&bmres27,20-6,60);
    }
    */
    //
    while(1)
    {
        //
        MODULE_OS_DELAY_MS(100);
        //
        if(time_10ms>10*MaxTimeS)
        {
            Hci_ReShowTimer   =  0;
            //清屏
            LCD_ClrScr(CL_BLACK);
            first = BspKey_Value;
            BspKey_Value   =  0;
            BspKey_NewSign  =  0;
            Hci_ReShowTimer =  0;
            return first;
        }
        else
        {
            time_10ms++;
        }
        if(first==1)
        {
            first=0;
            BspKey_Value = 0;
            //提示窗口
            GUI_SetColor(HCI_MENU_HintAsk_COLOR_1);
            GUI_FillRect(HCI_MENU_HintAsk_WIN_X1,HCI_MENU_HintAsk_WIN_Y1,\
                         HCI_MENU_HintAsk_WIN_X1+HCI_MENU_HintAsk_WIN_W,\
                         HCI_MENU_HintAsk_WIN_Y1+HCI_MENU_HintAsk_WIN_H1);
            GUI_SetColor(HCI_MENU_HintAsk_COLOR_2);
            GUI_FillRect(HCI_MENU_HintAsk_WIN_X1,HCI_MENU_HintAsk_WIN_Y1+HCI_MENU_HintAsk_WIN_H1,\
                         HCI_MENU_HintAsk_WIN_X1+HCI_MENU_HintAsk_WIN_W,\
                         HCI_MENU_HintAsk_WIN_Y1+HCI_MENU_HintAsk_WIN_H1+HCI_MENU_HintAsk_WIN_H2);
            //写标题
            GUI_SetColor(HCI_MENU_HintAsk_COLOR_3);
            GUI_SetTextMode(GUI_TM_TRANS);
            GUI_SetTextStyle(GUI_TS_NORMAL);
            GUI_UC_SetEncodeUTF8();
            GUI_SetFont(&GUI_FontFont_ST_24);
            GUI_DispStringHCenterAt((char*)ptitle,\
                                    HCI_MENU_HintAsk_WIN_X1+HCI_MENU_HintAsk_WIN_W/2,\
                                    HCI_MENU_HintAsk_WIN_Y1+3+2);
            //写内容
            GUI_SetColor(HCI_MENU_HintAsk_COLOR_4);
            GUI_SetFont(&GUI_FontFont_ST_16);
            if(ptext==NULL)
            {
                continue;
            }
            xpos  =  HCI_MENU_HintAsk_WIN_X1+2;
            ypos  =  HCI_MENU_HintAsk_WIN_Y1+HCI_MENU_HintAsk_WIN_H1+4+2;
            //识别行数
            i  =  0;
            j  =  1;
            while(1)
            {
                if(ptext[i]==0)
                {
                    break;
                }
                else if(ptext[i]=='\r' || ptext[i]=='\n')
                {
                    j++;
                }
                i++;
            }
            //
            if(j<=4)
            {
                ypos += 20;
            }
            //居中
            else if(j<=5)
            {
                ypos += (6-j)*10;
            }
            else
            {
                ypos += 0;
            }
            pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pbuf[200]   =  1;
            i  =  j  =  0;
            while(1)
            {
                //字符串结束
                if(ptext[i]==0)
                {
                    pbuf[j++]   =  0;
                    if(pbuf[200]==1)
                    {
                        GUI_DrawCircle(xpos+6+4,ypos+8,4);
                    }
                    GUI_DispStringAt((char*)pbuf,xpos+12+8,ypos);
                    break;
                }
                //小换行
                else if(ptext[i]=='\r')
                {
                    pbuf[j++]   =  0;
                    if(pbuf[200]==1)
                    {
                        GUI_DrawCircle(xpos+6+4,ypos+8,4);
                    }
                    GUI_DispStringAt((char*)pbuf,xpos+12+8,ypos);
                    j         =  0;
                    ypos      += 16+4;
                    pbuf[200] =  0;
                }
                //大换行
                else if(ptext[i]=='\n')
                {
                    pbuf[j++]   =  0;
                    if(pbuf[200]==1)
                    {
                        GUI_DrawCircle(xpos+6+4,ypos+8,4);
                    }
                    GUI_DispStringAt((char*)pbuf,xpos+12+8,ypos);
                    j         =  0;
                    ypos      += 16+8;
                    pbuf[200] =  1;
                }
                //
                else
                {
                    pbuf[j++]   =  ptext[i];
                }
                i++;
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            /*
            pWinT=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pWinT->id    =  ID_WIN;
            pWinT->Height=  g_LcdHeight*3/4;
            pWinT->Width =  g_LcdWidth*3/4;
            pWinT->Left  =  g_LcdWidth/8;
            pWinT->Top   =  g_LcdHeight/8;
            sfont.FontCode=0;
            sfont.FrontColor  =  CL_BLACK;
            sfont.BackColor   =  WIN_TITLE_COLOR;
            sfont.Space=0;
            pWinT->Font       =  &sfont;
            pWinT->pCaption   =  ptitle;
            pWinT->pText      =  ptext;
            LCD_DispControl(pWinT);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pWinT);
            #ifdef HCI_EMWIN_ENABLE
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres10;
                GUI_DrawBitmap(&bmres10,(320-72)/2,(240-72)/2);
            }
            #endif
            */
        }
        //按键处理
        if(BspKey_NewSign==1 && KeyEnable==1)
        {
            BspKey_NewSign  =  0;
            Hci_ReShowTimer =  0;
            switch(BspKey_Value)
            {
                case HCI_KEY_ENTER:
                case HCI_KEY_ESC:
                case HCI_KEY_LEFT:
                case HCI_KEY_RIGHT:
                case HCI_KEY_TOP:
                    time_10ms=0xFFFF;
                    break;
                default:
                    break;
            }
        }
    }
}
/*******************************************************************************
* 函数功能: 开机页面
*******************************************************************************/
static void Hci_Menu_BootScreen(void)
{
#ifdef   HCI_EMWIN_ENABLE
    if (BSP_BKPRAM_READ(BSP_BKPRAM_WAKE_UP) == 0x0000)
    {
        //挂载磁盘
        ModuleSdFat_Res = f_mount(&ModuleSdFat_Fs,MODULE_SDFAT_SD_PATH,0);
        if( ModuleSdFat_Res != FR_OK )
        {
            return;
        }
        if(OK!=EmWinXslApp_ShowBMP("BS.bmp",0,20))
        {
#if   ((HARDWARE_VER==1) || (HARDWARE_VER==7) || (HARDWARE_VER==8) || (HARDWARE_VER==9))
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres36;
                GUI_DrawBitmap(&bmres36,0,0);
            }
#elif (HARDWARE_VER==2)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres36;
                GUI_DrawBitmap(&bmres36,0,0);
            }
#elif (HARDWARE_VER==4)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres21;
                GUI_DrawBitmap(&bmres21,0,0);
            }
#elif (HARDWARE_VER==5)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres28;
                GUI_DrawBitmap(&bmres28,0,0);
            }
#elif (HARDWARE_VER==6)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres31;
                GUI_DrawBitmap(&bmres31,0,0);
            }
#endif
        }
        //卸载磁盘
        ModuleSdFat_Res = f_mount(NULL,MODULE_SDFAT_SD_PATH,0);
        //
        MODULE_OS_DELAY_MS(6000);
        pHci_MenuFun = Hci_Menu_SlefCheck;
    }
    else
    {
        BSP_BKPRAM_WRITE(BSP_BKPRAM_WAKE_UP, 0x0000);
        pHci_MenuFun = pHci_DefaultMenuFun;
    }
    //清屏
    Hci_ReShowTimer =  0;
    Hci_ParaChangeSign=0;
    Hci_CursorFlick =  0;
    Hci_ReturnTimer =  0;
    BspKey_NewSign  =  0;
    Hci_NoFirst      =  0;
    LCD_ClrScr(CL_BLACK);
#else
    //仅仅显示，无任何按键支持
    uint8_t *pbuf;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;

    Hci_ReturnTimer++;
    if(Hci_ReturnTimer>100*3)
    {
        pHci_MenuFun = Hci_Menu_SlefCheck;
        //清屏
        Hci_ReShowTimer =  0;
        Hci_ParaChangeSign=0;
        Hci_CursorFlick =  0;
        Hci_ReturnTimer =  0;
        Hci_NoFirst     =  0;
        LCD_ClrScr(sfont.BackColor);
        return;
    }
    if(Hci_ReShowTimer==0)
    {
        Hci_ReShowTimer=0xFFFF;
        //显示XKAP
        sfont.FontCode=FC_ST_64;
        pbuf = "XKAP";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*32)/2, g_LcdHeight/4, (char*)pbuf, &sfont);
        //显示硬件版本
        sfont.FontCode=FC_ST_16;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"Hareware Ver: %02d.%02d",HARDWARE_VER,HARDWARE_SUB_VER);
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, g_LcdHeight/2+32, (char*)pbuf, &sfont);
        //显示软件版本
        sprintf((char*)pbuf,"Software Ver: %02d.%02d",SOFTWARE_VER,SOFTWARE_SUB_VER);
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, g_LcdHeight/2+32+20, (char*)pbuf , &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#endif
}
/*******************************************************************************
* 函数功能: 自检页面
*******************************************************************************/
static void Hci_Menu_SlefCheck(void)
{
    //仅仅显示，无任何按键支持
    uint8_t *pbuf;
    uint16_t i16;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    //自检RTC电压
#if   (defined(STM32F1))
    pbuf =  pbuf;
    i16  =  i16;
#elif (defined(STM32F4))
    //采集RTC电池电压
    i16 = AD_VBAT_Read_mv();
    if(2000>i16)
    {
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"%s\r%d mV\n%s",(char*)pSTR_UTF8_HINT_RTC_LOW[Main_Language],\
                i16,\
                pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
        Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#endif
    // 上电电池供电提示语
    if(Bsp_Pwr_ResetFlag == BSP_PWR_E_RESET_FLAG_PORRST && BSP_PWR_E_POWER_AC!=Bsp_Pwr_Monitor())
    {
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"%s\n%s\n%s",(char*)pSTR_UTF8_HINT_BELL[Main_Language],\
                pSTR_UTF8_HINT_BELL_KEY[Main_Language],pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
        Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //首次开机显示二维码
    if(Hci_NoFirst==1)
    {
        //pHci_MenuFun = Hci_Menu_InfoQrCode;
        pHci_MenuFun = pHci_DefaultMenuFun;
    }
    //非首次则进入正常页面
    else
    {
        pHci_MenuFun = pHci_DefaultMenuFun;
    }
    //清屏
    Hci_ReShowTimer  =  0;
    Hci_ParaChangeSign= 0;
    Hci_CursorFlick  =  0;
    Hci_ReturnTimer  =  0;
    Hci_NoFirst      =  0;
    LCD_ClrScr(sfont.BackColor);
}

/*******************************************************************************
* 函数功能: 默认页面1
* 说    明: 美工设计的界面
*******************************************************************************/
#ifdef HCI_EMWIN_ENABLE
//第一背景色
#define HCI_MENU_DEFAULT1_COLOR_1   GUI_BLACK
//第二背景色
#define HCI_MENU_DEFAULT1_COLOR_2   Count_4ByteToLong(0,0,0,0)
//色条5-1
#define HCI_MENU_DEFAULT1_COLOR_3   Count_4ByteToLong(0,230,73,82)
//色条5-2
//#define HCI_MENU_DEFAULT1_COLOR_4   Count_4ByteToLong(0,230,215,33)
#define HCI_MENU_DEFAULT1_COLOR_4   Count_4ByteToLong(0,255,255,47)
//色条5-3
//#define HCI_MENU_DEFAULT1_COLOR_5   Count_4ByteToLong(0,99,231,74)
#define HCI_MENU_DEFAULT1_COLOR_5   Count_4ByteToLong(0,23,238,66)
//色条5-4
//#define HCI_MENU_DEFAULT1_COLOR_6   Count_4ByteToLong(0,41,231,222)
#define HCI_MENU_DEFAULT1_COLOR_6   Count_4ByteToLong(0,0,214,251)
//色条5-5
#define HCI_MENU_DEFAULT1_COLOR_7   Count_4ByteToLong(0,41,73,230)
//左上文字色
#define HCI_MENU_DEFAULT1_COLOR_8   Count_4ByteToLong(0,235,225,125)
//
#define HCI_MENU_DEFAULT1_THI_10    50
#define HCI_MENU_DEFAULT1_THI_11    56
#define HCI_MENU_DEFAULT1_THI_20    57
#define HCI_MENU_DEFAULT1_THI_21    61
#define HCI_MENU_DEFAULT1_THI_30    62
#define HCI_MENU_DEFAULT1_THI_31    70
#define HCI_MENU_DEFAULT1_THI_40    71
#define HCI_MENU_DEFAULT1_THI_41    74
#define HCI_MENU_DEFAULT1_THI_50    75
#define HCI_MENU_DEFAULT1_THI_51    79
//
#define HCI_MENU_DEFAULT1_DATATIME_OFFSET /*50*/0
//
static void Hci_Menu_Default1(void)
{
    uint8_t *pbuf;
    struct tm *ptm;
    uint8_t res;
    int8_t temp;
    uint8_t humi;
    uint8_t thi;
    uint16_t i16;
    static uint8_t si1=0;
    static uint8_t si2=0;
    static uint8_t si3=0;
    //
    if(Hci_NoFirst==0 || Hci_ParaChangeSign==1)
    {
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            // 申请缓存
            Hci_pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            /*
            Hci_pbuf[1]=ptm->tm_mon;
            Hci_pbuf[2]=ptm->tm_mday;
            Hci_pbuf[3]=ptm->tm_wday;
            Hci_pbuf[4]=ptm->tm_hour;
            Hci_pbuf[5]=ptm->tm_min;
            Hci_pbuf[6]=ptm->tm_sec;
            Hci_pbuf[7]=temp;
            Hci_pbuf[8]=humi;
            Hci_pbuf[9]=thi;
            */
            memset((char*)Hci_pbuf,0,10);
            //手动结束测量标志
            Hci_pbuf[14]=0;
            //10秒显示睡眠结构分析
            Hci_pbuf[15]=0;
            //180秒等待网络连接
            Hci_pbuf[16]=0;
            //快速显示 0-初始 1-开始 2-监测中  3-结束 4-检测结束
            Hci_pbuf[17]=0;
            //
            Hci_PowerStopTimerS= 0;
            Hci_PowerLowCmt    = 0;
            //
            Hci_ParaChangeSign = 1;
        }
        if(Hci_ParaChangeSign==1)
        {
            //刷新供电图标
            Hci_pbuf[10]=Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state+10;
            Hci_pbuf[11]=Bluetooth_ConnectSign+10;
            Hci_pbuf[12]=Bluetooth_ConnectOnOff+10;
            Hci_pbuf[13]=0;
            Hci_ReShowTimer=0;
            //
        }
        //
        GUI_UC_SetEncodeUTF8();
        Hci_ParaChangeSign=0;
        GUI_SetBkColor(HCI_MENU_DEFAULT1_COLOR_1);
        // 画背景
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
        GUI_FillRect(0,0,LCD_GetXSize()-1,LCD_GetYSize()-1);
        // 画3个区域方块
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
        GUI_FillRect(8,29,311,157);
        GUI_FillRect(8,168,132,231);
        GUI_FillRect(143,168,311,231);
        //---画白横线
        GUI_SetColor(GUI_WHITE);
        GUI_SetPenSize(1);
        //GUI_DrawHLine(145+1+10,10,310-1);
        //---画颜色条
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_3);
        GUI_FillRect(10,152+10-7,10+10*(HCI_MENU_DEFAULT1_THI_11-HCI_MENU_DEFAULT1_THI_10+1)-1,152+7+10+1);
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_4);
        GUI_FillRect(10+10*(HCI_MENU_DEFAULT1_THI_11-HCI_MENU_DEFAULT1_THI_10+1),152+10-7,10+10*(HCI_MENU_DEFAULT1_THI_21-HCI_MENU_DEFAULT1_THI_10+1)-1,152+7+10+1);      //57-61=5
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_5);
        GUI_FillRect(10+10*(HCI_MENU_DEFAULT1_THI_21-HCI_MENU_DEFAULT1_THI_10+1),152+10-7,10+10*(HCI_MENU_DEFAULT1_THI_31-HCI_MENU_DEFAULT1_THI_10+1)-1,152+7+10+1);     //62-70=9
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_6);
        GUI_FillRect(10+10*(HCI_MENU_DEFAULT1_THI_31-HCI_MENU_DEFAULT1_THI_10+1),152+10-7,10+10*(HCI_MENU_DEFAULT1_THI_41-HCI_MENU_DEFAULT1_THI_10+1)-1,152+7+10+1);     //71-73=3
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_7);
        GUI_FillRect(10+10*(HCI_MENU_DEFAULT1_THI_41-HCI_MENU_DEFAULT1_THI_10+1),152+10-7,10+10*(HCI_MENU_DEFAULT1_THI_51-HCI_MENU_DEFAULT1_THI_10+1)-1,152+7+10+1);     //74-79=6
        //---环境舒适度
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&GUI_FontFont_DQHT_18);
        //GUI_DispStringAt((char*)pSTR_UTF8_THI[Main_Language],12,120+10+3);
        GUI_SetTextMode(GUI_TM_XOR);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_THI[0][Main_Language]\
                                ,((10+10*(HCI_MENU_DEFAULT1_THI_11-HCI_MENU_DEFAULT1_THI_10+1)-1)+10)/2+1\
                                ,120+10+3+22);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_THI[1][Main_Language]\
                                ,((10+10*(HCI_MENU_DEFAULT1_THI_21-HCI_MENU_DEFAULT1_THI_10+1)-1)+(10+10*(HCI_MENU_DEFAULT1_THI_11-HCI_MENU_DEFAULT1_THI_10+1)))/2+1\
                                ,120+10+3+22);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_THI[2][Main_Language]\
                                ,((10+10*(HCI_MENU_DEFAULT1_THI_31-HCI_MENU_DEFAULT1_THI_10+1)-1)+(10+10*(HCI_MENU_DEFAULT1_THI_21-HCI_MENU_DEFAULT1_THI_10+1)))/2+1\
                                ,120+10+3+22);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_THI[3][Main_Language]\
                                ,((10+10*(HCI_MENU_DEFAULT1_THI_41-HCI_MENU_DEFAULT1_THI_10+1)-1)+(10+10*(HCI_MENU_DEFAULT1_THI_31-HCI_MENU_DEFAULT1_THI_10+1)))/2+1\
                                ,120+10+3+22);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_THI[4][Main_Language]\
                                ,((10+10*(HCI_MENU_DEFAULT1_THI_51-HCI_MENU_DEFAULT1_THI_10+1)-1)+(10+10*(HCI_MENU_DEFAULT1_THI_41-HCI_MENU_DEFAULT1_THI_10+1)))/2+1\
                                ,120+10+3+22);
        //
        GUI_SetTextMode(GUI_TM_NORMAL);
        //显示闹钟图标
        LCD_DrawImage(154-4+4,186,32,32,Bmp_Bell);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示闹钟1设置信息
        BspRtc_ReadAlarm(1,&i16,&res);
        if(res<3)
        {
            sprintf((char*)pbuf,(char*)pSTR_UTF8_ALARM_MODE_BUF[res][Main_Language]);
        }
        else
        {
            pbuf[0]=0;
        }
        GUI_SetTextMode(GUI_TM_NORMAL);
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_DispStringAt((char*)pbuf,8+120+8+24+40-12+4,180);
        GUI_SetFont(&GUI_FontFont_HNLP_24);
        if(i16==0xFFFF || res==0)
        {
            GUI_DispStringAt((char*)":",270-12+4,180);
            GUI_DispStringAt((char*)"--",270-24-12+4,180);
            GUI_DispStringAt((char*)"--",270+12-12+4,180);
        }
        else
        {
            GUI_DispStringAt((char*)":",270-12+4,180);
            sprintf((char*)pbuf,"%02d",i16>>8);
            GUI_DispStringAt((char*)pbuf,270-24-12+4,180);
            sprintf((char*)pbuf,"%02d",i16&0x00FF);
            GUI_DispStringAt((char*)pbuf,270+12-12+4,180);
        }
        //显示闹钟2设置信息
        BspRtc_ReadAlarm(2,&i16,&res);
        if(res<3)
        {
            sprintf((char*)pbuf,(char*)pSTR_UTF8_ALARM_MODE_BUF[res][Main_Language]);
        }
        else
        {
            pbuf[0]=0;
        }
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_DispStringAt((char*)pbuf,8+120+8+24+40-12+4,205);
        GUI_SetFont(&GUI_FontFont_HNLP_24);
        if(i16==0xFFFF || res==0)
        {
            GUI_DispStringAt((char*)":",270-12+4,205);
            GUI_DispStringAt((char*)"--",270-24-12+4,205);
            GUI_DispStringAt((char*)"--",270+12-12+4,205);
        }
        else
        {
            GUI_DispStringAt((char*)":",270-12+4,205);
            sprintf((char*)pbuf,"%02d",i16>>8);
            GUI_DispStringAt((char*)pbuf,270-24-12+4,205);
            sprintf((char*)pbuf,"%02d",i16&0x00FF);
            GUI_DispStringAt((char*)pbuf,270+12-12+4,205);
        }
        //显示温度
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_DispStringAt((char*)pSTR_UTF8_TEMP[Main_Language],25-4,180);
        GUI_DispStringAt((char*)pSTR_UTF8_SIGN_TEMP[Main_Language],25+24*3+12-4,180);
        //显示湿度
        GUI_DispStringAt((char*)pSTR_UTF8_HUMI[Main_Language],25-4,205);
        GUI_DispStringAt((char*)pSTR_UTF8_SIGN_PERCENT[Main_Language],25+24*3+12-4,205);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //定时刷新判断
    //刷新屏幕
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        //SIM卡安装提醒
        if(GprsNet_s_Info.Err_SimCardInstall==1 && GprsNet_s_Info.state==1 && si3==0)
        {
            si3=1;
            sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_INSTALL_SIMCARD[Main_Language],\
                    pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
            Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            Hci_ParaChangeSign = 1;
        }
        //供电转换判定
        Hci_Power = Bsp_Pwr_Monitor();
        //电池使用时间统计
        {
            static uint16_t s_bat_timer=0;
            uint16_t i16;
            if(Hci_Power !=  BSP_PWR_E_POWER_AC)
            {
                s_bat_timer++;
                if(s_bat_timer>=3600)
                {
                    s_bat_timer=0;
                    //
                    i16 = BSP_BKPRAM_READ(BSP_BKPRAM_BELL_TIME_H);
                    if(i16>60000)
                    {
                        i16=1;
                        BSP_BKPRAM_WRITE(BSP_BKPRAM_BELL_TIME_H,i16);
                    }
                    else if(i16<60000)
                    {
                        i16++;
                        BSP_BKPRAM_WRITE(BSP_BKPRAM_BELL_TIME_H,i16);
                    }
                }
            }
            else
            {
                s_bat_timer=0;
            }
        }
        //不进入低功耗模式的条件
        if(Hci_Power ==  BSP_PWR_E_POWER_AC \
           ||UCTSK_RFMS_E_RUNSTATE_IDLE!=uctsk_Rfms_GetRunState()\
           ||NULL!=Bsp_BuzzerMusic_GetState()\
           /*
           ||GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL\
           ||GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_SCAN*/)
        {
            Hci_PowerStopTimerS  =  0;
            Hci_PowerLowCmt      =  0;
        }
        if(Hci_PowerStopTimerS<HCI_ENTER_POWER_STOP_TIME_S)
        {
            Hci_PowerStopTimerS++;
        }
        if(Hci_PowerLowCmt>=HCI_ENTER_BAT_LOW_TIME_S||Hci_PowerStopTimerS>=HCI_ENTER_POWER_STOP_TIME_S)
        {
            sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_PWR_STOP[Main_Language],\
                    pSTR_UTF8_HINT_PWR_STOP_KEY[Main_Language]);
            Hci_Menu_HintAsk(5,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            //进入待机画面
            Bsp_Pwr_EnterStop();
        }
        //关闭检测
        if(Hci_pbuf[14]!=0)
        {
            if(Hci_pbuf[14]==2)
            {
                if(Hci_Power==BSP_PWR_E_POWER_AC)
                {
                    GprsNet_OnOff(ON);
                }
                uctsk_Rfms_SetOnOff(OFF,0);
                Hci_pbuf[14]=0;
                //
                Hci_cursor|=(1<<3);
            }
            else
            {
                Hci_pbuf[14]++;
            }
        }
        //显示提示弹窗
        if(Hci_s_Bit_hint.bit0==1)
        {
            Hci_s_Bit_hint.bit0=0;
            sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_SLEEP[Main_Language],\
                    pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
            if(Hci_Power==BSP_PWR_E_POWER_AC)
            {
                Hci_Menu_HintAsk(300,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            }
            else
            {
                Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            }
            Hci_ParaChangeSign = 1;
            //goto goto_HciMenuDefault1;
        }
        if(Hci_s_Bit_hint.bit1==1)
        {
            Hci_s_Bit_hint.bit1=0;
            Hci_ParaChangeSign = 1;
        }
        if(Hci_s_Bit_hint.bit2==1)
        {
            Hci_s_Bit_hint.bit2=0;
            Hci_ParaChangeSign = 1;
        }
        if(GprsAppXkap_UpdataState==1)
        {
            //Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"Gprs Data uploading...!");
        }
        else if(GprsAppXkap_UpdataState==2)
        {
            GprsAppXkap_UpdataState=0;
            /*
            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],(char*)pSTR_GPRS_DATA_UPLOADED_OK[Main_Language]);
            */
            Hci_ParaChangeSign = 1;
            /*
            if(Hci_cursor&(1<<1))
            {
                Hci_cursor|=(1<<3);
            }
            */
        }
        else if(GprsAppXkap_UpdataState==3)
        {
            GprsAppXkap_UpdataState=0;
            /*
            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],(char*)pSTR_GPRS_DATA_UPLOADED_ERR[Main_Language]);
            */
            Hci_ParaChangeSign = 1;
            /*
            if(Hci_cursor&(1<<1))
            {
                Hci_cursor|=(1<<3);
            }
            */
        }
        else if(GprsAppXkap_UpdataState==4)
        {
            GprsAppXkap_UpdataState=0;
            sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_NOT_VALID_DATA[Main_Language],\
                    pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
            Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            Hci_ParaChangeSign = 1;
            //goto goto_HciMenuDefault1;
            /*
            if(Hci_cursor&(1<<1))
            {
                Hci_cursor|=(1<<3);
            }
            */
        }
        //检测中...
        res=uctsk_Rfms_GetRunState();
        Bluetooth_App(BLUETOOTH_CMD_READ_CMD,&pbuf[200]);
        if(Bluetooth_HciTxRxOkErr==MODULE_E_ERR_NONE)
        {
            Bluetooth_HciTxRxOkErr = MODULE_E_ERR_NULL;
            Hci_CursorFlick=5;
        }
        else if(Bluetooth_HciTxRxOkErr==MODULE_E_ERR_BLUETOOTH_CONNECT)
        {
            Bluetooth_HciTxRxOkErr = MODULE_E_ERR_NULL;
            Hci_CursorFlick=10;
        }
        else if(Bluetooth_HciTxRxOkErr==MODULE_E_ERR_BLUETOOTH_TRANSFER)
        {
            Bluetooth_HciTxRxOkErr = MODULE_E_ERR_NULL;
            Hci_CursorFlick=15;
        }
        else if(Hci_CursorFlick!=0)
        {
            Hci_CursorFlick--;
        }
        //
        if(si1!=si2)
        {
            si1=si2;
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            GUI_FillRect(25,8,160,8+18);
        }
        //判断闹铃 显示"起床"
        GUI_SetBkColor(HCI_MENU_DEFAULT1_COLOR_1);
        GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_8);
        GUI_SetTextMode(GUI_TM_NORMAL);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_SetFont(&GUI_FontFont_DQHT_18);
        // 起床提醒
        if(BSP_BUZZER_MUSIC_LIB_MORNING == Bsp_BuzzerMusic_GetState())
        {
            GUI_DispStringAt((char*)pSTR_UTF8_GETUP[Main_Language],25,8);
            //打开背光
            Hci_ReturnTimer = 0;
            //
            si2=1;
        }
        else if(si2==1&&NULL==Bsp_BuzzerMusic_GetState())
        {
            si2=2;
        }
        // 监测中
        else if(Hci_pbuf[17]==1)
        {
            Hci_pbuf[17]=2;

            //蜂鸣器
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BB,0);
            //发送指令
            if(ModuleMemory_psPara->Flag_GprsKeepOn==OFF)
            {
                GprsNet_OnOff(OFF);
            }
            uctsk_Rfms_SetOnOff(ON,0);
            //
            Hci_cursor|=(1<<1);
            //
#if   (HARDWARE_VER==1)
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres36;
                GUI_DrawBitmap(&bmres36,0,0);
                MODULE_OS_DELAY_MS(5000);
                Hci_ParaChangeSign = 1;
            }
#endif
            GUI_DispStringAt((char*)pSTR_UTF8_MONITORING[Main_Language],25,8);
        }
        else if(Hci_pbuf[17]==3)
        {
            Hci_pbuf[17]=4;
            //判断自动监测
            BspRtc_ReadAuto(&res,NULL,NULL);
            //当前版本关闭自动监测提示窗口
            res = OFF;
            //启动自动监测
            if(ON==res)
            {
                //如果自启动开启中,则要关闭出现询问框
                sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_ASK_STOP_MEASURE[Main_Language],\
                        pSTR_UTF8_ASK_STOP_KEY[Main_Language]);
                res = Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_ASK[Main_Language],(char*)pbuf);
                if(res==HCI_KEY_TOP)
                {
                    GUI_DispStringAt((char*)pSTR_UTF8_MONITOR_OVER[Main_Language],25,8);
                    //蜂鸣器
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BLB,0);
                    //
                    Hci_pbuf[14]=1;
                }
                Hci_ParaChangeSign = 1;
                //goto goto_HciMenuDefault1;
            }
            //未启动自动监测
            else
            {
                GUI_DispStringAt((char*)pSTR_UTF8_MONITOR_OVER[Main_Language],25,8);
                //蜂鸣器
                Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BLB,0);
                //
                Hci_pbuf[14]=1;
            }
            Hci_pbuf[17]=0;
        }
        else if(res>=UCTSK_RFMS_E_RUNSTATE_START && res <=UCTSK_RFMS_E_RUNSTATE_GET && Hci_pbuf[14]==0)
        {
            GUI_DispStringAt((char*)pSTR_UTF8_MONITORING[Main_Language],25,8);
            //
            si2=3;
            //
            if(Uctsk_Rfms_RunFileData_DebugTest_Enable==ON)
            {
                sprintf((char*)pbuf,"%ld/%ld",UctskRfms_TestfileDataNum,UctskRfms_TestfileDataAllByteNum);
                GUI_DispStringAt((char*)pbuf,25+9*8,8);
            }
            //用于测量后页面跳转
            Hci_cursor|=(1<<1);
            //
            Hci_pbuf[15]=0;
            Hci_pbuf[16]=0;
        }
        else if((res==UCTSK_RFMS_E_RUNSTATE_IDLE || res ==UCTSK_RFMS_E_RUNSTATE_STOP) /*&& GprsNet_s_Info.state==1*/)
        {
            if(Hci_cursor&(1<<1))
            {
                if(uctsk_Rfms_MonitorRxNum<=3600*100)
                {
                    //申请缓存
                    sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_INVALID_MEASURE[Main_Language],\
                            pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
                    res = Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
                    Hci_ParaChangeSign = 1;
                    Hci_cursor =  0;
                    //goto goto_HciMenuDefault1;
                }
                else
                {
                    GUI_DispStringAt((char*)pSTR_UTF8_MONITOR_OVER[Main_Language],25,8);
                    Hci_cursor|=(1<<3);
                }
            }
            else if(GprsAppXkap_RfmsCmd==GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL)
            {
                if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)
                {
                    GUI_DispStringAt((char*)pSTR_UTF8_GPRS_WAITING[Main_Language],25,8);
                    si2=9;
                }
                else
                {
                    sprintf((char*)pbuf,"%s(%d/%d)",(char*)pSTR_UTF8_GPRS_UPLOADING[Main_Language],GprsAppXkap_DataPacketIndex,MODULE_MEMORY_ADDR_DAY_NUM);
                    GUI_DispStringAt((char*)pbuf,25,8);
                    si2=10;
                }
            }
        }
        //------------------------------不会执行下面条件,暂时这么做,后续完善
        // BLE 数据上传OK
        else if(Hci_CursorFlick>=1 && Hci_CursorFlick<=5)
        {
            if(Bluetooth_s_info.Mode==1)
            {
                GUI_DispStringAt((char*)pSTR_UTF8_BLE_UPLOAD_OK[Main_Language],25,8);
                //
                si2=11;
            }
            if(Hci_CursorFlick==1)
            {
                Hci_CursorFlick=0;
                if(Hci_cursor&(1<<1))
                {
                    Hci_cursor|=(1<<2);
                }
            }
        }
        // BLE 连接失败
        else if((Hci_CursorFlick>=6 && Hci_CursorFlick<=10))
        {
            if(Bluetooth_s_info.Mode==1)
            {
                GUI_DispStringAt((char*)pSTR_UTF8_BLE_CONNECT_ERR[Main_Language],25,8);
                //
                si2=12;
            }
            if(Hci_CursorFlick==6)
            {
                Hci_CursorFlick=0;
                if(Hci_cursor&(1<<1))
                {
                    Hci_cursor|=(1<<2);
                }
            }
        }
        // BLE 上传数据失败
        else if((Hci_CursorFlick>=11 && Hci_CursorFlick<=15))
        {
            if(Bluetooth_s_info.Mode==1)
            {
                GUI_DispStringAt((char*)pSTR_UTF8_BLE_UPLOAD_ERR[Main_Language],25,8);
                //
                si2=13;
            }
            if(Hci_CursorFlick==11)
            {
                Hci_CursorFlick=0;
                if(Hci_cursor&(1<<1))
                {
                    Hci_cursor|=(1<<2);
                }
            }
        }
        // BLE 数据上传中
        else if((Bluetooth_ConnectOnOff==ON)&&(Bluetooth_ConnectSign==1)&&(pbuf[200]!=BLUETOOTH_CMD_NULL))
        {
            if(Bluetooth_s_info.Mode==1)
            {
                GUI_DispStringAt((char*)pSTR_UTF8_BLE_UPLOADING[Main_Language],25,8);
                //
                si2=14;
            }
        }
        // 开启 BLE
        else if((Bluetooth_ConnectOnOff==ON)&&(Bluetooth_ConnectSign==0))
        {
            if(Bluetooth_s_info.Mode==1)
            {
                GUI_DispStringAt((char*)pSTR_UTF8_OPENBLE[Main_Language],25,8);
                //
                si2=15;
            }
        }
        /*
        else if(res==UCTSK_RFMS_E_RUNSTATE_IDLE || res ==UCTSK_RFMS_E_RUNSTATE_STOP)
        {
            if(Hci_cursor&(1<<1))
            {
                if(uctsk_Rfms_MonitorRxNum<=3600*100)
                {
                    //sprintf((char*)pbuf,"%s(Err-%05d)",(char*)pSTR_HINT[Main_Language],MODULE_E_ERR_SLEEP_MEASURN_SHORT);
                    sprintf((char*)pbuf,"%s",(char*)pSTR_HINT[Main_Language]);
                    Hci_Menu_Ask(5,1,(char*)pbuf,(char*)pSTR_HINT_INVALID_MEASURE[Main_Language]);
                    Hci_ParaChangeSign = 1;
                    Hci_cursor =  0;
                }
            }
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            GUI_FillRect(25,8,160,8+18);
        }
        */
        if(Hci_ParaChangeSign==1)
        {
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            return;
        }
        GUI_SetTextMode(GUI_TM_TRANS);
        //
        ptm = (struct tm *)&pbuf[128];
        BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
        temp   =  (int8_t)(Sensor_Temp/10);
        humi    =    Sensor_Humi/10;
        if(temp==0&&humi==0)
        {
            thi=50;
        }
        else
        {
            if(temp<0)
            {
                temp  =  0;
            }
            else if(temp>35)
            {
                temp  =  35;
            }
            if(humi>=100)
            {
                humi  =  99;
            }
            Count_THI(Sensor_Temp/10,Sensor_Humi/10,&thi);
            if(thi<50)
            {
                thi=50;
            }
            else if(thi>79)
            {
                thi=79;
            }
        }
        //请开启手机蓝牙
        //数据上传中
        //数据上传成功
        //画AC图标
        //电源电压大于4000mv,则显示电源图标
        if(Hci_Power==BSP_PWR_E_POWER_AC)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres16;
            if((Hci_pbuf[13]&(1<<0))==0)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
                GUI_FillRect(235+(16+8)*1,8,235+(16+8)*1+20,8+20);
                GUI_DrawBitmap(&bmres16,235+(16+8)*1,8);
                Hci_pbuf[13]=(1<<0);
            }
            Hci_PowerLowCmt   =  0;
        }
        //电源电压小于4000mv,电池电压大于3500mv,则显示电池满电图标
        else if(Hci_Power==BSP_PWR_E_POWER_BAT_FULL)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres17;
            if((Hci_pbuf[13]&(1<<1))==0)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
                GUI_FillRect(235+(16+8)*1,8,235+(16+8)*1+20,8+20);
                GUI_DrawBitmap(&bmres17,235+(16+8)*1,8);
                Hci_pbuf[13]=(1<<1);
            }
            Hci_PowerLowCmt   =  0;
        }
        //电源电压小于4000mv,电池电压大于3500mv,则显示电池满电图标
        else if(Hci_Power==BSP_PWR_E_POWER_BAT_MID)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres23;
            if((Hci_pbuf[13]&(1<<2))==0)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
                GUI_FillRect(235+(16+8)*1,8,235+(16+8)*1+20,8+20);
                GUI_DrawBitmap(&bmres23,235+(16+8)*1,8);
                Hci_pbuf[13]=(1<<2);
            }
            Hci_PowerLowCmt   =  0;
        }
        //电源电压小于4000mv,电池电压小于3500mv,则显示低电图标
        else if(BSP_PWR_E_POWER_LOW==BSP_PWR_E_POWER_LOW)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres18;
            if((Hci_pbuf[13]&(1<<3))==0)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
                GUI_FillRect(235+(16+8)*1,8,235+(16+8)*1+20,8+20);
                GUI_DrawBitmap(&bmres18,235+(16+8)*1,8);
                Hci_pbuf[13]=(1<<3);
            }
            Hci_PowerLowCmt   =  0;
        }
        else
        {
            if(Hci_PowerLowCmt<HCI_ENTER_BAT_LOW_TIME_S)
            {
                Hci_PowerLowCmt++;
            }
        }
        GUI_SetTextMode(GUI_TM_TRANS);
        //画蓝牙图标
        if((Bluetooth_s_info.Mode==1) && (Hci_pbuf[11]!=Bluetooth_ConnectSign || Hci_pbuf[12]!=Bluetooth_ConnectOnOff))
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            GUI_FillRect(235,8,235+20,8+20);
            //蓝牙已连接
            if(Bluetooth_ConnectSign==1)
            {
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres13;
                    GUI_DrawBitmap(&bmres13,235,8);
                }
            }
            //蓝牙未开启
            else if(Bluetooth_ConnectOnOff==OFF)
            {
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres14;
                    GUI_DrawBitmap(&bmres14,235,8);
                }
            }
            //蓝牙未连接
            else
            {
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres15;
                    GUI_DrawBitmap(&bmres15,235,8);
                }
            }
            Hci_pbuf[11]=Bluetooth_ConnectSign;
            Hci_pbuf[12]=Bluetooth_ConnectOnOff;
        }
        //画MobileMoudle图标
        if(Hci_Power!=BSP_PWR_E_POWER_AC)
        {
            ;
        }
        else if(GprsNet_s_Info.OnOff==ON && Hci_pbuf[10]!=Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            GUI_FillRect(235+(16+8)*2,8,235+(16+8)*2+20,8+20);
            if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state==GPRSNET_E_CONNECT_STATE_CONNECTED)
            {
                if(GprsNet_s_Info.SignalVal==99)
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres12;
                    GUI_DrawBitmap(&bmres12,235+16+8+16+8,8);
                }
                else if(GprsNet_s_Info.SignalVal<=12)
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres29;
                    GUI_DrawBitmap(&bmres29,235+16+8+16+8,8);
                }
                else if(GprsNet_s_Info.SignalVal<=24)
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres30;
                    GUI_DrawBitmap(&bmres30,235+16+8+16+8,8);
                }
                else
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres11;
                    GUI_DrawBitmap(&bmres11,235+16+8+16+8,8);
                }
            }
            else if(GprsNet_s_Info.OnOff==ON)
            {
                {
                    extern GUI_CONST_STORAGE GUI_BITMAP bmres12;
                    GUI_DrawBitmap(&bmres12,235+16+8+16+8,8);
                }
            }
            Hci_pbuf[10]=Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state;
        }
        else if(GprsNet_s_Info.OnOff==OFF && GprsNet_s_Info.state==1)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            GUI_FillRect(235+(16+8)*2,8,235+(16+8)*2+20,8+20);
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres19;
                GUI_DrawBitmap(&bmres19,235+16+8+16+8,8);
            }
        }
        //显示日期
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_UC_SetEncodeUTF8();
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_DispStringAt((char*)pSTR_UTF8_MONTH[Main_Language],76+24*1-4-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        GUI_DispStringAt((char*)pSTR_UTF8_DAY[Main_Language],76+24*3-4-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        GUI_SetFont(&GUI_FontFont_HNLP_24);
        if(Hci_pbuf[1]!=ptm->tm_mon)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            res = sprintf((char*)pbuf,"%01d",Hci_pbuf[1]+1);
            if(res==1)
            {
                GUI_DispStringAt((char*)pbuf,76+24*0+3-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
            }
            else
            {
                GUI_DispStringAt((char*)pbuf,76+24*0-6-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
            }
            Hci_pbuf[1]=ptm->tm_mon;
            GUI_SetColor(GUI_WHITE);
        }
        res = sprintf((char*)pbuf,"%01d",ptm->tm_mon+1);
        if(res==1)
        {
            GUI_DispStringAt((char*)pbuf,76+24*0+3-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        }
        else
        {
            GUI_DispStringAt((char*)pbuf,76+24*0-6-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        }

        if(Hci_pbuf[2]!=ptm->tm_mday)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            res = sprintf((char*)pbuf,"%01d",Hci_pbuf[2]);
            if(res==1)
            {
                GUI_DispStringAt((char*)pbuf,76+24*2+1-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
            }
            else
            {
                GUI_DispStringAt((char*)pbuf,76+24*2-6-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
            }
            Hci_pbuf[2]=ptm->tm_mday;
            GUI_SetColor(GUI_WHITE);
        }
        res = sprintf((char*)pbuf,"%01d",ptm->tm_mday);
        if(res==1)
        {
            GUI_DispStringAt((char*)pbuf,76+24*2+1-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        }
        else
        {
            GUI_DispStringAt((char*)pbuf,76+24*2-6-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);
        }
        //显示星期
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        if(Hci_pbuf[3]!=ptm->tm_wday)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            GUI_DispStringAt((char*)pSTR_UTF8_WEEK_BUF[Hci_pbuf[3]][Main_Language],180-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);// 204
            Hci_pbuf[3]=ptm->tm_wday;
            GUI_SetColor(GUI_WHITE);
        }
        GUI_DispStringAt((char*)pSTR_UTF8_WEEK_BUF[ptm->tm_wday][Main_Language],180-HCI_MENU_DEFAULT1_DATATIME_OFFSET,39+6);// 204
        //显示时间
        GUI_SetFont(&GUI_FontFont_HNLP_72);
        if(Hci_pbuf[4]!=ptm->tm_hour)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            sprintf((char*)pbuf,"%02d",Hci_pbuf[4]);
            GUI_DispStringHCenterAt((char*)pbuf,320/2-48-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+6);
            Hci_pbuf[4]=ptm->tm_hour;
            GUI_SetColor(GUI_WHITE);
        }
        sprintf((char*)pbuf,"%02d",ptm->tm_hour);
        GUI_DispStringHCenterAt((char*)pbuf,320/2-48-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+6);
        if(Hci_pbuf[5]!=ptm->tm_min)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            sprintf((char*)pbuf,"%02d",Hci_pbuf[5]);
            GUI_DispStringHCenterAt((char*)pbuf,320/2+48-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+6);
            Hci_pbuf[5]=ptm->tm_min;
            GUI_SetColor(GUI_WHITE);
        }
        sprintf((char*)pbuf,"%02d",ptm->tm_min);
        GUI_DispStringHCenterAt((char*)pbuf,320/2+48-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+6);
        if(ptm->tm_sec%2)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
        }
        else
        {
            GUI_SetColor(GUI_WHITE);
        }
        //sprintf((char*)pbuf,":");
        //GUI_DispStringHCenterAt((char*)pbuf,320/2,28+108/2-48/2);
        GUI_FillCircle(320/2-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+26+6,3);
        GUI_FillCircle(320/2-HCI_MENU_DEFAULT1_DATATIME_OFFSET,28+108/2-48/2+48+6,3);
        if(HCI_MENU_DEFAULT1_DATATIME_OFFSET!=0)
        {
            GUI_SetColor(GUI_WHITE);
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            //画竖线
            GUI_DrawVLine(255-HCI_MENU_DEFAULT1_DATATIME_OFFSET,49,124);
            //室外温度信息
            GUI_SetFont(&GUI_FontFont_DQHT_24);
            GUI_DispStringAt((char*)pSTR_UTF8_SIGN_TEMP[Main_Language],255-HCI_MENU_DEFAULT1_DATATIME_OFFSET+5+40,45);
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            GUI_DispStringAt("-12",255-HCI_MENU_DEFAULT1_DATATIME_OFFSET+5,45);
#if 0
            //显示温度范围
            GUI_SetFont(&GUI_FontFont_ST_16);
            GUI_DispStringAt("-12",255-HCI_MENU_DEFAULT1_DATATIME_OFFSET+65,45-4);
            GUI_DispStringAt("-34",255-HCI_MENU_DEFAULT1_DATATIME_OFFSET+65,60-4);
            //显示风向与风速
            GUI_SetFont(&GUI_FontFont_DQHT_24);
            GUI_DispStringAt((char*)pSTR_UTF8_WIND_DIRECTION[0][Main_Language],260-HCI_MENU_DEFAULT1_DATATIME_OFFSET,45+30);
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            GUI_DispStringAt("3-4",260-HCI_MENU_DEFAULT1_DATATIME_OFFSET+48,45+30);
#else
            //显示温度范围
            GUI_SetFont(&GUI_FontFont_HNLP_18);
            GUI_DispStringAt("-12/+34",260-HCI_MENU_DEFAULT1_DATATIME_OFFSET,45+30);
            GUI_SetFont(&GUI_FontFont_DQHT_18);
            GUI_DispStringAt((char*)pSTR_UTF8_SIGN_TEMP[Main_Language],260-HCI_MENU_DEFAULT1_DATATIME_OFFSET+10*7,45+30);
#endif
            //显示天气与空气质量
            //GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_1);
            //GUI_FillRect(235+(16+8)*1,8,235+(16+8)*1+20,8+20);
            {
                extern GUI_CONST_STORAGE GUI_BITMAP bmres32;
                GUI_DrawBitmap(&bmres32,260-HCI_MENU_DEFAULT1_DATATIME_OFFSET,105);
            }
            GUI_SetFont(&GUI_FontFont_DQHT_18);
            GUI_DispStringAt((char*)pSTR_UTF8_AIR[Main_Language],260-HCI_MENU_DEFAULT1_DATATIME_OFFSET+30,105+7-2);
            GUI_SetFont(&GUI_FontFont_DQHT_24);
            GUI_DispStringAt((char*)pSTR_UTF8_AIR_QUALITY[0][Main_Language],260-HCI_MENU_DEFAULT1_DATATIME_OFFSET+30+36,105-2);
        }
        //
        GUI_SetColor(GUI_WHITE);
        if(temp==0&&humi==0)
        {
            /*
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            GUI_SetTextMode(GUI_TM_NORMAL);
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            GUI_DispStringAt((char*)"--",25+24*2+12-4-4,180);
            GUI_DispStringAt((char*)"--",25+24*2+12-4-4,205);
            */
        }
        else
        {
            //温度
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            if(Hci_pbuf[7]!=temp)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
                sprintf((char*)pbuf,"%02d",Hci_pbuf[7]);
                GUI_DispStringAt((char*)pbuf,25+24*2+12-4-4,180);
                Hci_pbuf[7]=temp;
                GUI_SetColor(GUI_WHITE);
            }
            sprintf((char*)pbuf,"%02d",temp);
            GUI_DispStringAt((char*)pbuf,25+24*2+12-4-4,180);
            //湿度
            GUI_SetFont(&GUI_FontFont_HNLP_24);
            if(Hci_pbuf[8]!=humi)
            {
                GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
                sprintf((char*)pbuf,"%02d",Hci_pbuf[8]);
                GUI_DispStringAt((char*)pbuf,25+24*2+12-4-4,205);
                Hci_pbuf[8]=humi;
                GUI_SetColor(GUI_WHITE);
            }
            sprintf((char*)pbuf,"%02d",humi);
            GUI_DispStringAt((char*)pbuf,25+24*2+12-4-4,205);
        }
        //显示环境舒适度控件
        if(Hci_pbuf[9]!=thi)
        {
            //---删除心
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
            //
            i16 = Hci_pbuf[9]*10-490;
            if(i16<10+5)
            {
                i16=10+5;
            }
            else if(i16>309-5)
            {
                i16=309-5;
            }
            GUI_DrawPixel(i16,150-0+10-6);
            GUI_DrawHLine(150-1+10-6,i16-1,i16+1);
            GUI_DrawHLine(150-2+10-6,i16-2,i16+2);
            GUI_DrawHLine(150-3+10-6,i16-3,i16+3);
            GUI_DrawHLine(150-4+10-6,i16-4,i16+4);
            GUI_DrawHLine(150-5+10-6,i16-5,i16+5);
            GUI_DrawHLine(150-6+10-6,i16-5,i16+5);
            GUI_DrawHLine(150-7+10-6,i16-5,i16-1);
            GUI_DrawHLine(150-7+10-6,i16+1,i16+5);
            GUI_DrawHLine(150-8+10-6,i16-4,i16-2);
            GUI_DrawHLine(150-8+10-6,i16+2,i16+4);
            GUI_DrawPixel(i16-3,150-9+10-6);
            GUI_DrawPixel(i16+3,150-9+10-6);
            //
            //GUI_SetColor(GUI_WHITE);
            //GUI_DrawHLine(150-5+10+1-6,i16-4,i16+4);
            //
            Hci_pbuf[9]=thi;
        }
        //---画心
        if(temp==0&&humi==0)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
        }
        else if(thi>=HCI_MENU_DEFAULT1_THI_10 && thi<=HCI_MENU_DEFAULT1_THI_11)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_3);
        }
        else if(thi>=HCI_MENU_DEFAULT1_THI_20 && thi<=HCI_MENU_DEFAULT1_THI_21)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_4);
        }
        else if(thi>=HCI_MENU_DEFAULT1_THI_30 && thi<=HCI_MENU_DEFAULT1_THI_31)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_5);
        }
        else if(thi>=HCI_MENU_DEFAULT1_THI_40 && thi<=HCI_MENU_DEFAULT1_THI_41)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_6);
        }
        else if(thi>=HCI_MENU_DEFAULT1_THI_50 && thi<=HCI_MENU_DEFAULT1_THI_51)
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_7);
        }
        else
        {
            GUI_SetColor(HCI_MENU_DEFAULT1_COLOR_2);
        }
        i16 = thi*10-490;
        if(i16<10+5)
        {
            i16=10+5;
        }
        else if(i16>309-5)
        {
            i16=309-5;
        }
        GUI_DrawPixel(i16,150-0+10-6);
        GUI_DrawHLine(150-1+10-6,i16-1,i16+1);
        GUI_DrawHLine(150-2+10-6,i16-2,i16+2);
        GUI_DrawHLine(150-3+10-6,i16-3,i16+3);
        GUI_DrawHLine(150-4+10-6,i16-4,i16+4);
        GUI_DrawHLine(150-5+10-6,i16-5,i16+5);
        GUI_DrawHLine(150-6+10-6,i16-5,i16+5);
        GUI_DrawHLine(150-7+10-6,i16-5,i16-1);
        GUI_DrawHLine(150-7+10-6,i16+1,i16+5);
        GUI_DrawHLine(150-8+10-6,i16-4,i16-2);
        GUI_DrawHLine(150-8+10-6,i16+2,i16+4);
        GUI_DrawPixel(i16-3,150-9+10-6);
        GUI_DrawPixel(i16+3,150-9+10-6);
//goto_HciMenuDefault1:
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //背光转换
    if(UCTSK_RFMS_E_RUNSTATE_START<=uctsk_Rfms_GetRunState()
       &&UCTSK_RFMS_E_RUNSTATE_STOP>=uctsk_Rfms_GetRunState())
    {
        //背光进入睡眠模式
        if(Hci_ReturnTimer==0)
        {
            MODULE_MEMORY_S_PARA *pspara;
            pspara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
            //如果入睡背光为关闭,则开启LCD驱动
            if(pspara->LcdBackLight2==HCI_SLEEP_LIGHT_LEVEL0&&HCI_SLEEP_LIGHT_LEVEL0==LCD_GetBackLight())
            {
                LCD_ILI9341_EnterWork();
                LCD_ClrScr(CL_BLACK);
                Hci_ParaChangeSign = 1;
            }
            //非电源供电 && 通常背光亮度大于限值 = 强行降低背光亮度到限值
            res = pspara->LcdBackLight1;
            if(Hci_Power != BSP_PWR_E_POWER_AC && res>BSP_PWR_LIGHT_LEVEL)
            {
                res=BSP_PWR_LIGHT_LEVEL;
            }
            LCD_SetBackLight(res);
            //
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
            Hci_ReturnTimer++;
        }
        else if(Hci_ReturnTimer<10*100)
        {
            Hci_ReturnTimer++;
        }
        else if(Hci_ReturnTimer==10*100)
        {
            MODULE_MEMORY_S_PARA *pspara;
            pspara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
            res = pspara->LcdBackLight2;
            if(Hci_Power != BSP_PWR_E_POWER_AC && res>HCI_SLEEP_LIGHT_LEVEL1)
            {
                res=HCI_SLEEP_LIGHT_LEVEL1;
            }
            LCD_SetBackLight(res);
            //如果背光关闭,则关闭LCD驱动
            if(pspara->LcdBackLight2==HCI_SLEEP_LIGHT_LEVEL0)
            {
                LCD_ILI9341_EnterSleep();
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
            //
            Hci_ReturnTimer++;
        }
    }
    else
    {
        //背光进入正常模式
        if(Hci_ReturnTimer>0)
        {
            MODULE_MEMORY_S_PARA *pspara;
            //驱动
            //背光
            pspara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
            if(pspara->LcdBackLight2==HCI_SLEEP_LIGHT_LEVEL0&&HCI_SLEEP_LIGHT_LEVEL0==LCD_GetBackLight())
            {
                LCD_ILI9341_EnterWork();
                LCD_ClrScr(CL_BLACK);
                Hci_ParaChangeSign = 1;
            }
            res = pspara->LcdBackLight1;
            if(Hci_Power != BSP_PWR_E_POWER_AC && res>BSP_PWR_LIGHT_LEVEL)
            {
                res=BSP_PWR_LIGHT_LEVEL;
            }
            LCD_SetBackLight(res);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
            Hci_ReturnTimer=0;
        }
    }
    //IAP提示
    if(Hci_IapHint==1&&Iap_AllPackage!=0)
    {
        Hci_IapHint=0;
        //
        pHci_MenuFun = Hci_Menu_IapToUser;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_CursorFlick     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
        // 释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
    }
    //按键处理
    //如果测量完毕，则页面自动切换到分数页面(虚拟按键方式)
    if((Hci_cursor&(1<<3)))
    {
        //等待数据处理结束
        while(UCTSK_RFMS_E_RUNSTATE_IDLE!=uctsk_Rfms_GetRunState())
        {
            MODULE_OS_DELAY_MS(10);
        }
        //七条未上传数据做提示
        {
            MODULE_MEMORY_S_DAY_INFO *psDayInfo;
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            psDayInfo=(MODULE_MEMORY_S_DAY_INFO*)&pbuf[128];
            //
            for(pbuf[0]=pbuf[1]=0; pbuf[0]<MODULE_MEMORY_ADDR_DAY_NUM; pbuf[0]++)
            {
                Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&pbuf[0]);
                if(psDayInfo->BeginByte==0xCC&&psDayInfo->UpdataToServerCmt==0)
                {
                    pbuf[1]+=1;
                }
            }
            if(pbuf[1]==7)
            {
                sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_DATA_FULL[Main_Language],\
                        pSTR_UTF8_HINT_DATA_FULL_KEY[Main_Language]);
                Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
                //页面跳转到曲线页面
            }
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            Hci_ParaChangeSign = 1;
        }
        //
        Hci_cursor = 0;
        BspKey_NewSign=1;
        BspKey_Value=HCI_KEY_RIGHT;
    }
    // 监测结束提示
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_PowerStopTimerS=0;
#if (HARDWARE_VER==7)
        //软关机
        if((BspKey_KeepTimer_ms>=2000) && (BspKey_Value == HCI_KEY_RIGHT))
        {
            Bsp_Pwr_EnterStop();
        }
#endif
        //如果闹钟正在响铃,则此次按键无效
        if(BSP_BUZZER_MUSIC_LIB_MORNING==Bsp_BuzzerMusic_GetState())
        {
            return;
        }
        //暗屏要先点亮
        if(Hci_ReturnTimer >= 10*100)
        {
            Hci_ReturnTimer=0;
            return;
        }
        //如果在睡眠测定状态,则只识别顶部按键
        res = uctsk_Rfms_GetRunState();
        if((UCTSK_RFMS_E_RUNSTATE_START<=res && UCTSK_RFMS_E_RUNSTATE_STOP>res))
        {
            if(BspKey_Value!=HCI_KEY_TOP && BspKey_Value!=HCI_KEY_ESC)
            {
                return;
            }
        }
        //解析按键
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                //短按处理
                if(BspKey_KeepTimer_ms<2000)
                {
                    pHci_MenuFun=Hci_Menu_SetMenu;
                    Hci_ReShowTimer     =  0;
                    Hci_ParaChangeSign  =  0;
                    Hci_cursor          =  0;
                    Hci_CursorFlick     =  0;
                    Hci_ReturnTimer     =  0;
                    Hci_NoFirst         =  0;
                    LCD_ClrScr(CL_BLACK);
                    // 释放缓存
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                }
                break;
            case HCI_KEY_ESC:
                //传感器还在监测中 && 系统未进入结束监测流程
                if((UCTSK_RFMS_E_RUNSTATE_START<=res && UCTSK_RFMS_E_RUNSTATE_STOP>=res)&&(Hci_pbuf[14]==0))
                {
                    //提示取消本次测量
                    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_ASK_CANCEL_MEASURE[Main_Language],\
                            pSTR_UTF8_ASK_CANCEL_MEASURE_KEY[Main_Language]);
                    if(HCI_KEY_ENTER==Hci_Menu_HintAsk(15,1,(char*)pSTR_UTF8_ASK[Main_Language],(char*)pbuf))
                    {
                        //取消测量
                        uctsk_Rfms_SetRunState(UCTSK_RFMS_E_RUNSTATE_IDLE);
                        //清空标志
                        Hci_cursor&=(uint8_t)(~(1<<1));
                    }
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    Hci_ParaChangeSign = 1;
                }
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                if(BspKey_KeepTimer_ms<2000)
                {
                    // 星期日显示广告
                    ptm = (struct tm *)Hci_pbuf;
                    BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
#if   (HARDWARE_VER==7)
                    pHci_MenuFun = Hci_Menu_SleepCurve;
#elif (HARDWARE_VER==8)
                    Hci_CalledFun=  0;
                    pHci_MenuFun = Hci_Menu_DayMoveCurve;
                    if(BspKey_Value==HCI_KEY_LEFT)
                    {
                        Hci_DayMoveCnt      =  6;
                    }
                    else if(BspKey_Value==HCI_KEY_RIGHT)
                    {
                        Hci_DayMoveCnt      =  0;
                    }
#else
                    if(ptm->tm_wday==0)
                    {
                        pHci_MenuFun = Hci_Menu_InfoSalesQrCode;
                    }
                    // 非星期日直接显示曲线
                    else
                    {
                        pHci_MenuFun = Hci_Menu_SleepCurve;
                    }
#endif
                    Hci_ReShowTimer     =  0;
                    Hci_ParaChangeSign  =  0;
                    Hci_CursorFlick     =  0;
                    Hci_NoFirst         =  0;
                    LCD_ClrScr(CL_BLACK);
                    // 释放缓存
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                }
                /*
                else
                {
                    Bsp_Pwr_EnterStop();
                }
                */
                break;
            case HCI_KEY_TOP:
                // 短按解析
                if(BspKey_KeepTimer_ms<2000)
                {
                    Hci_ReShowTimer=0;
                    res=uctsk_Rfms_GetRunState();
                    if(res==UCTSK_RFMS_E_RUNSTATE_IDLE)
                    {
                        //快速显示"监测中..."
                        Hci_pbuf[17]=1;
                    }
                    else if(res>=UCTSK_RFMS_E_RUNSTATE_START && res <=UCTSK_RFMS_E_RUNSTATE_GET && Hci_pbuf[14]==0)
                    {
                        Hci_pbuf[17]=3;
                    }
                    else
                    {
                        //弹出提示窗
                    }
                }
                break;
            default:
                break;
        }
    }
    //顶部按键3秒
    res = uctsk_Rfms_GetRunState();
    if(/*(res==UCTSK_RFMS_E_RUNSTATE_IDLE)\*/
        (BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]>3*100)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]>3*100)\
        &&(Hci_Power ==  BSP_PWR_E_POWER_AC))
    {
        if(res!=UCTSK_RFMS_E_RUNSTATE_IDLE)
        {
            ;
        }
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]=0xFFFF;
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_SOS;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
        // 释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
    }
    if(res==UCTSK_RFMS_E_RUNSTATE_IDLE&&BspKey_PressKeepTimerBuf_10ms[HCI_KEY_TOP-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_TOP-1]>3*100)
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_TOP-1]=0xFFFF;
        pHci_MenuFun        =  Hci_Menu_InfoQrCode;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
        // 释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
    }
    //返回按键3秒
    if(res==UCTSK_RFMS_E_RUNSTATE_IDLE&&BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]>3*100)
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]=0xFFFF;
        Bluetooth_App(BLUETOOTH_CMD_UPDATA_7DAY,NULL);
        if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state==GPRSNET_E_CONNECT_STATE_CONNECTED)
        {
            //提示数据正在上传
            GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SLEEP_DATA_ALL);
            Hci_ParaChangeSign = 1;
        }
        else if(GprsNet_s_Info.OnOff==ON)
        {
            //提示等待模块连接
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_WAIT_NET[Main_Language],\
                    pSTR_UTF8_HINT_ESC_RETURN[Main_Language]);
            Hci_Menu_HintAsk(5,0,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
            //Hci_Menu_Ask(5,0,(char*)pSTR_HINT[Main_Language],(char*)pSTR_WAIT_NET[Main_Language]);
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
            Hci_ParaChangeSign = 1;
        }
    }
    //设置按键8秒
    if(res==UCTSK_RFMS_E_RUNSTATE_IDLE&&BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>8*100)
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_I;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
        // 释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
    }
}
#endif
/*******************************************************************************
* 函数功能: 一级菜单(目前分为三项:信息/设置/测试)
*******************************************************************************/
static void Hci_Menu_SOS(void)
{
    uint8_t *pbuf;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=50;
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            LCD_ClrScr(CL_WHITE);
            BspRtc_ReadRealTime(NULL,NULL,NULL,GprsAppXkap_S_SOS.DateTime);
            GprsAppXkap_S_SOS.state=0;
            Hci_cursor  = 0;
            Hci_ReturnTimer = 0;
            //开启GPRS
            GprsNet_OnOff(ON);
        }
        LCD_SetBackLight(200);
        //
        Hci_cursor++;
        if((GprsAppXkap_S_SOS.state!=2)&&(Hci_cursor>6))
        {
            //if((GprsAppXkap_UpdataState==0))
            {
                Hci_cursor =  0;
                if(GprsAppXkap_S_SOS.state==0)
                {
                    GprsAppXkap_S_SOS.state   =  1;
                }
                GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SOS);
                if(Hci_ParaChangeSign<0xFF)
                {
                    Hci_ParaChangeSign++;
                }
            }
        }
        if(GprsAppXkap_S_SOS.state!=2)
        {
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
        }
        else if(Hci_ReturnTimer==0)
        {
            Hci_ReturnTimer=1;
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BBB,0);
        }
        // SOS
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetTextStyle(GUI_TS_NORMAL);
        GUI_SetFont(&GUI_FontFont_HNLP_72);
        if(Hci_CursorFlick==1)
        {
            Hci_CursorFlick   =  0;
            if(GprsAppXkap_S_SOS.state==2)
            {
                GUI_SetColor(GUI_GREEN);
            }
            else
            {
                GUI_SetColor(GUI_RED);
            }
        }
        else
        {
            Hci_CursorFlick   =  1;
            if(GprsAppXkap_S_SOS.state==2)
            {
                GUI_SetColor(GUI_GREEN);
            }
            else
            {
                GUI_SetColor(GUI_WHITE);
            }
        }
        GUI_DispStringHCenterAt((char*)"S0S",LCD_GetXSize()/2,LCD_GetYSize()/2-40);
        GUI_SetPenSize(3);
        GUI_DrawLine(36,LCD_GetYSize()/2-36-(36/4),36+36,LCD_GetYSize()/2-(36/2));
        GUI_DrawLine(36,LCD_GetYSize()/2,36+36,LCD_GetYSize()/2);
        GUI_DrawLine(36,LCD_GetYSize()/2+36+(36/4),36+36,LCD_GetYSize()/2+(36/2));
        GUI_DrawLine(LCD_GetXSize()-36,LCD_GetYSize()/2-36-(36/4),LCD_GetXSize()-36-36,LCD_GetYSize()/2-(36/2));
        GUI_DrawLine(LCD_GetXSize()-36,LCD_GetYSize()/2,LCD_GetXSize()-36-36,LCD_GetYSize()/2);
        GUI_DrawLine(LCD_GetXSize()-36,LCD_GetYSize()/2+36+(36/4),LCD_GetXSize()-36-36,LCD_GetYSize()/2+(36/2));
        GUI_SetPenSize(1);
        // 呼救信号发送中/发送成功
        pbuf   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        GUI_SetTextMode(GUI_TM_NORMAL);
        GUI_SetBkColor(GUI_WHITE);
        //GUI_SetColor(GUI_RED);
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        if(GprsAppXkap_S_SOS.state==2)
        {
            sprintf((char*)pbuf,"%s(%d)",(char*)pSTR_UTF8_SOS_SENDOK[Main_Language],Hci_ParaChangeSign);
            GUI_DispStringHCenterAt((char*)pbuf,LCD_GetXSize()/2,LCD_GetYSize()/5);
        }
        else
        {
            sprintf((char*)pbuf,"%s(%d)",(char*)pSTR_UTF8_SOS_SENDING[Main_Language],Hci_ParaChangeSign);
            GUI_DispStringHCenterAt((char*)pbuf,LCD_GetXSize()/2,LCD_GetYSize()/5);
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        // 按返回键返回主界面
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_SetColor(GUI_RED);
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],LCD_GetXSize()/2,(LCD_GetYSize()/4)*3);

    }
    //按键处理
    if(BspKey_KeepTimer_ms>2000)
    {
        BspKey_NewSign=0;
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = pHci_DefaultMenuFun;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                LCD_ClrScr(CL_BLACK);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 一级菜单(目前分为三项:信息/设置/测试)
*******************************************************************************/
static void Hci_Menu_I(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }

        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-I");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1.Information");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2.Set");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3.Test");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 4.Data Save and Load");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_KeepTimer_ms>2000)
    {
        BspKey_NewSign=0;
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        pHci_MenuFun = Hci_Menu_II_Info;
                        break;
                    case 1:
                        pHci_MenuFun = Hci_Menu_II_Set;
                        break;
                    case 2:
                        pHci_MenuFun = Hci_Menu_II_Test;
                        break;
                    case 3:
                        pHci_MenuFun = Hci_Menu_II_DataSaveAndLoad;
                        break;
                    default:
                        break;
                }
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = pHci_DefaultMenuFun;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=3;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=3)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 二级菜单-信息
*******************************************************************************/
static void Hci_Menu_II_Info(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-II-Information");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1.Info Chip");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2.Info Version");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3.Info OS");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 4.Info MobileModule");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 5.Info BlueTooth");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 6.Info RFMS");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 7.Info Sleep Begin Time");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 8.Info IAP");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //sprintf((char*)pbuf," 9.Info Gas Module");
        //LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 9.Info Day Move");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        pHci_MenuFun = Hci_Menu_InfoChip;
                        break;
                    case 1:
                        pHci_MenuFun = Hci_Menu_InfoVer;
                        break;
                    case 2:
                        pHci_MenuFun = Hci_Menu_InfoOS;
                        break;
                    case 3:
                        pHci_MenuFun = Hci_Menu_InfoMobileModule;
                        break;
                    case 4:
                        pHci_MenuFun = Hci_Menu_InfoBluetooth;
                        break;
                    case 5:
                        pHci_MenuFun = Hci_Menu_InfoRfms;
                        break;
                    case 6:
                        pHci_MenuFun = Hci_Menu_InfoSleepBeginTime;
                        break;
                    case 7:
                        pHci_MenuFun = Hci_Menu_InfoIAP;
                        break;
                    /*
                    case 8:
                        pHci_MenuFun = Hci_Menu_InfoGasModule;
                        break;
                        */
                    case 8:
                        pHci_MenuFun = Hci_Menu_DayMoveCurve;
                        Hci_CalledFun  =  1;
                        Hci_DayMoveCnt =  0;
                        break;
                    default:
                        break;
                }
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_I;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=8;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=8)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 二级菜单-设置
*******************************************************************************/
static void Hci_Menu_II_Set(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    uint8_t i;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-II-Set");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1.Set data/time/alarm");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2.Set Backlight");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3.Set Restart");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 4.Set Factory Reset(Half)");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 5.Set Factory Reset(All)");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 6.Set MobileModule");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 7.Set Language");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 8.Set IAP(Bluetooth)");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 9.Set Function On/Off");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        /*
        sprintf((char*)pbuf," 9,Set Theme");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        */
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        pHci_MenuFun = Hci_Menu_SetTime;
                        LCD_ClrScr(sfont.BackColor);
                        break;
                    case 1:
                        pHci_MenuFun = Hci_Menu_SetBackLight;
                        LCD_ClrScr(sfont.BackColor);
                        break;
                    case 2:
                        MCU_SOFT_RESET;
                        break;
                    case 3:
                        i = MODULE_MEMORY_CMD_FACTORY_MODE_HALF;
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
                        MCU_SOFT_RESET;
                        break;
                    case 4:
                        i = MODULE_MEMORY_CMD_FACTORY_MODE_ALL;
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&i);
                        MCU_SOFT_RESET;
                        break;
                    case 5:
                        pHci_MenuFun = Hci_Menu_SetMobileModule;
                        LCD_ClrScr(sfont.BackColor);
                        break;
                    case 6:
                        pHci_MenuFun = Hci_Menu_SetLanguage;
                        LCD_ClrScr(sfont.BackColor);
                        break;
                    case 7:
                        pHci_MenuFun = Hci_Menu_SetBluetoothIAP;
                        LCD_ClrScr(sfont.BackColor);
                        break;
                    case 8:
                        pHci_MenuFun = Hci_Menu_SetFunctionOnOff;
                        LCD_ClrScr(sfont.BackColor);
                        Hci_pbuf =  NULL;
                        break;
                    default:
                        break;
                }
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_I;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=8;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=8)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 二级菜单-测试
*******************************************************************************/
static void Hci_Menu_II_Test(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    //uint32_t i32;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-II-Test");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1.Test LCD");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2.Test Buzzer");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3.Test Run Test File Data");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //sprintf((char*)pbuf," 4,HardFault(system halted!)");
        sprintf((char*)pbuf," 4.Enter Stop Mode");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 5.Test GPRS");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 6.Test Show Sales QrCode");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 7.Test Show FactoryID QrCode");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 8.Test Show IMEI QrCode");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        pHci_MenuFun = Hci_Menu_TestLcd;
                        break;
                    case 1:
                        pHci_MenuFun = Hci_Menu_TestBuzzer;
                        break;
                    case 2:
                        uctsk_Rfms_ExtFlashReadSign=0;
                        pHci_MenuFun = pHci_DefaultMenuFun;
                        uctsk_Rfms_RunFileData_DebugTestOnOff(ON);
                        break;
                    case 3:
                        /*
                        //申请缓存
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        LCD_ClrScr(sfont.BackColor);
                        sprintf((char*)pbuf,"System Halted");
                        LCD_DispStr(0, 0, (char*)pbuf, &sfont);
                        sprintf((char*)pbuf,"Please Power on again!");
                        LCD_DispStr(0, 20, (char*)pbuf, &sfont);
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                        {
                            uint8_t *pbuf;
                            i32=1;
                            pbuf=NULL;
                            while(i32++)
                            {
                                pbuf[i32]=i32;
                            }
                        }
                        */
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_PWR_STOP[Main_Language],\
                                pSTR_UTF8_HINT_PWR_STOP_KEY[Main_Language]);
                        Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                        Bsp_Pwr_EnterStop();
                        break;
                    case 4:
                        pHci_MenuFun = Hci_Menu_TestGsm;
                        break;
                    case 5:
                        pHci_MenuFun = Hci_Menu_InfoSalesQrCode;
                        break;
                    case 6:
                        pHci_MenuFun = Hci_Menu_InfoFactoryIDQrCode;
                        break;
                    case 7:
                        pHci_MenuFun = Hci_Menu_InfoIMEIQrCode;
                        break;
                    default:
                        break;
                }
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_I;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=7;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=7)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 二级菜单-测试
*******************************************************************************/
MODULE_MEMORY_S_PARA *pPara;
static void Hci_Menu_II_DataSaveAndLoad(void)
{
    uint8_t res=OK;
    uint8_t *pbuf;

    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    uint32_t i32,j32,k32;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-II-DataSaveAndLoad");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1.Save All to TF");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2.Load Cfg from TF");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3.Load Boot from TF");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 4.App From TF To W25");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        ypos = 0;
                        //申请缓存
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
                        //
                        LCD_ClrScr(sfont.BackColor);
                        sprintf((char*)pbuf,"Save All to TF");
                        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
                        //新建文件
                        i32=0;
                        xpos = 0;
                        if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_W,"/cfg.dat",pbuf,&i32,&xpos,&k32)\
                           ||OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_W,"/app.dat",pbuf,&i32,&xpos,&k32)\
                           ||OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_W,"/boot.dat",pbuf,&i32,&xpos,&k32)\
                           ||OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_W,"/w25r.dat",pbuf,&i32,&xpos,&k32))
                        {
                            res = ERR;
                            sprintf((char*)pbuf,"TF Create File Err");
                            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                        }
                        else
                        {
                            xpos = 512;
                            //保存配置文件-->boot.dat
                            sprintf((char*)pbuf,"Saving CpuBoot To TF(boot.dat)...");
                            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                            for(i32=BSP_CPU_FLASH_BASE_ADDR; i32<BSP_CPU_FLASH_PARA_ADDR; i32+=xpos)
                            {
                                Bsp_CpuFlash_Read(i32,pbuf,xpos);
                                j32 = i32-BSP_CPU_FLASH_BASE_ADDR;
                                if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_ADD,"/boot.dat",pbuf,&j32,&xpos,&k32))
                                {
                                    res = ERR;
                                    break;
                                }
                            }
                            if(i32>=BSP_CPU_FLASH_PARA_ADDR)
                            {
                                sprintf((char*)pbuf,"OK");
                            }
                            else
                            {
                                sprintf((char*)pbuf,"ERR");
                            }
                            LCD_DispStr(g_LcdWidth-8*strlen((char*)pbuf), ypos, (char*)pbuf, &sfont);
                            //保存配置文件-->cfg.dat
                            sprintf((char*)pbuf,"Saving CpuCfg To TF(cfg.dat)...");
                            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                            for(i32=BSP_CPU_FLASH_PARA_ADDR; i32<BSP_CPU_FLASH_PARA_BACKUP_ADDR; i32+=xpos)
                            {
                                Bsp_CpuFlash_Read(i32,pbuf,xpos);
                                j32 = i32-BSP_CPU_FLASH_PARA_ADDR;
                                if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_ADD,"/cfg.dat",pbuf,&j32,&xpos,&k32))
                                {
                                    res = ERR;
                                    break;
                                }
                            }
                            if(i32>=BSP_CPU_FLASH_PARA_BACKUP_ADDR)
                            {
                                sprintf((char*)pbuf,"OK");
                            }
                            else
                            {
                                sprintf((char*)pbuf,"ERR");
                            }
                            LCD_DispStr(g_LcdWidth-8*strlen((char*)pbuf), ypos, (char*)pbuf, &sfont);
                            //保存程序文件-->app.dat
                            sprintf((char*)pbuf,"Saving CpuApp To TF(app.dat)...");
                            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                            for(i32=BSP_CPU_FLASH_APP_ADDR; i32<(BSP_CPU_FLASH_END_ADDR+1); i32+=xpos)
                            {
                                Bsp_CpuFlash_Read(i32,pbuf,xpos);
                                j32 = i32-BSP_CPU_FLASH_APP_ADDR;
                                if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_ADD,"/app.dat",pbuf,&j32,&xpos,&k32))
                                {
                                    res = ERR;
                                    break;
                                }
                            }
                            if(i32>=(BSP_CPU_FLASH_END_ADDR+1))
                            {
                                sprintf((char*)pbuf,"OK");
                            }
                            else
                            {
                                sprintf((char*)pbuf,"ERR");
                            }
                            LCD_DispStr(g_LcdWidth-8*strlen((char*)pbuf), ypos, (char*)pbuf, &sfont);
                            //保存W25数据
                            sprintf((char*)pbuf,"Saving W25 To TF(w25r.dat)...");
                            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                            for(i32=0; i32<BspExtFlash_s_Flash.TotalSize; i32+=xpos)
                            {
                                Bsp_ExtFlash_Read_nByte(i32,pbuf,xpos);
                                j32 = i32-0;
                                if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_ADD,"/w25r.dat",pbuf,&j32,&xpos,&k32))
                                {
                                    res = ERR;
                                    break;
                                }
                            }
                            if(i32>=BspExtFlash_s_Flash.TotalSize)
                            {
                                sprintf((char*)pbuf,"OK");
                            }
                            else
                            {
                                sprintf((char*)pbuf,"ERR");
                            }
                            LCD_DispStr(g_LcdWidth-8*strlen((char*)pbuf), ypos, (char*)pbuf, &sfont);
                        }
                        sprintf((char*)pbuf,"Wait for 5 seconds,automatically return");
                        LCD_DispStr(g_LcdWidth-8*strlen((char*)pbuf), ypos+=20, (char*)pbuf, &sfont);
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
                        //
                        MODULE_OS_DELAY_MS(5000);
                        break;
                    case 1:
                        //
                        xpos = 512;
                        //申请缓存
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
                        //
                        LCD_ClrScr(sfont.BackColor);
                        sprintf((char*)pbuf,"Loading......");
                        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, (g_LcdHeight-16)/2, (char*)pbuf, &sfont);
                        //装载配置文件-->cfg.dat
                        for(i32=BSP_CPU_FLASH_PARA_ADDR; i32<BSP_CPU_FLASH_PARA_BACKUP_ADDR; i32+=xpos)
                        {
                            j32 = i32-BSP_CPU_FLASH_PARA_ADDR;
                            xpos = 512;
                            if(OK==SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_R,"/cfg.dat",pbuf,&j32,&xpos,&k32))
                            {
                                Bsp_CpuFlash_Write(i32,pbuf,xpos);
                            }
                            else
                            {
                                //
                                res = ERR;
                                break;
                            }
                        }
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
                        //
                        if(res==OK)
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"Config Load OK!");
                        }
                        else
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"Config Load ERR!");
                        }
                        MCU_SOFT_RESET;
                        break;
                    case 2:
                        //
                        xpos = 512;
                        //申请缓存
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
                        //
                        LCD_ClrScr(sfont.BackColor);
                        sprintf((char*)pbuf,"Loading Boot ......");
                        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, (g_LcdHeight-16)/2, (char*)pbuf, &sfont);
                        //装载配置文件-->cfg.dat
                        for(i32=BSP_CPU_FLASH_BASE_ADDR; i32<BSP_CPU_FLASH_PARA_ADDR; i32+=xpos)
                        {
                            j32 = i32-BSP_CPU_FLASH_BASE_ADDR;
                            xpos = 512;
                            if(OK==SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_R,"/boot.dat",pbuf,&j32,&xpos,&k32))
                            {
                                Bsp_CpuFlash_Write(i32,pbuf,xpos);
                            }
                            else
                            {
                                res = ERR;
                                break;
                            }
                        }
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
                        //
                        if(res==OK)
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"Boot Load OK!");
                        }
                        else
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"Boot Load ERR!");
                        }
                        MCU_SOFT_RESET;
                        break;
                    case 3:
                        //tf安装情况
                        if(BspExtFlash_s_Flash.TotalSize==0)
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"W25 Not found!");
                            break;
                        }
                        xpos = 512;
                        //申请缓存
                        pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_2KB_BASIC);
                        pPara =  (MODULE_MEMORY_S_PARA*)&pbuf[1024];
                        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                        pPara->UpdataLen  =  0;
                        pPara->UpdataSum  =  0;
                        //
                        LCD_ClrScr(sfont.BackColor);
                        sprintf((char*)pbuf,"App From TF To W25 ......");
                        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, (g_LcdHeight-16)/2, (char*)pbuf, &sfont);
                        //
                        if(OK!=SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_GETSIZE,"/fw.bin",NULL,&j32,NULL,NULL))
                        {
                            res=ERR;
                        }
                        else
                        {
                            if(ERR==Bsp_ExtFlash_Erase(0,512*1024L-1))
                            {
                                res=ERR;
                            }
                            k32=0;
                            pPara->UpdataLen=j32;
                            for(i32=0; i32<j32; i32+=xpos)
                            {
                                memset((char*)pbuf,0xFF,xpos);
                                if(OK==SdFat_App(MODULE_SDFAT_E_APP_CMD_FILE_R,"/fw.bin",pbuf,&i32,&xpos,&k32))
                                {
                                    Bsp_ExtFlash_Write_nByte(i32,pbuf,k32);
                                    if(k32>256)
                                    {
                                        pPara->UpdataSum  += Count_Sum(0,pbuf,256);
                                        pPara->UpdataSum  += Count_Sum(0,&pbuf[256],k32-256);
                                    }
                                    else
                                    {
                                        pPara->UpdataSum  += Count_Sum(0,pbuf,k32);
                                    }
                                    if(i32+k32>=j32)
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    //
                                    res=ERR;
                                    break;
                                }
                            }
                        }
                        //
                        if(res==OK)
                        {
                            pPara->Updata=0x77654321;
                            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"App From TF To W25 OK!");
                        }
                        else
                        {
                            Hci_Menu_Ask(5,1,(char*)pSTR_HINT[Main_Language],"App From TF To W25 ERR!");
                        }
                        //释放缓存
                        MemManager_Free(E_MEM_MANAGER_TYPE_2KB_BASIC,pbuf);
                        break;
                    default:
                        break;
                }
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_I;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                Hci_cursor = 0;
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=3;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=3)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 设置时间
*******************************************************************************/
static void Hci_Menu_SetTime(void)
{
    uint8_t *pbuf;
    struct tm *ptm;
    FONT_T sfont;
    uint16_t xpos=0,ypos=0;
    uint16_t i16,j16;
    uint8_t i,j;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    //首次进入
    if(Hci_NoFirst   == 0)
    {
        Hci_NoFirst =  1;
        //申请缓存
        Hci_pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    }
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
        if(Hci_ReShowTimer==0)
        {
            Hci_ReShowTimer=100;
            //申请缓存
            pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            ptm = (struct tm *)&pbuf[128];
            //读取信息
            BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
            sprintf((char*)pbuf,"Current Data :%04d-%02d-%02d",ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday);
            LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Current Time :%02d:%02d:%02d(%d)",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,ptm->tm_wday);
            LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
            //释放缓存
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    else
    {
        Hci_ReShowTimer=100;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        ptm = (struct tm *)&pbuf[128];
        //读取信息
        BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
        BspRtc_ReadAlarm(1,&i16,&i);
        BspRtc_ReadAlarm(2,&j16,&j);
        //显示信息
        sprintf((char*)pbuf,"Menu-Set Time");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Current Data :%04d-%02d-%02d",ptm->tm_year,ptm->tm_mon+1,ptm->tm_mday);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Current Time :%02d:%02d:%02d(%d)",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,ptm->tm_wday);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //sprintf((char*)pbuf,"Current Alarm:(%02d)%02d:%02d (%02d)%02d:%02d",i,i16>>8,i16&0x00FF,j,j16>>8,j16&0x00FF);
        //LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            //时钟
            Hci_pbuf[0]  =  ptm->tm_year-2000;
            Hci_pbuf[1]  =  ptm->tm_mon;
            Hci_pbuf[2]  =  ptm->tm_mday;
            Hci_pbuf[3]  =  ptm->tm_hour;
            Hci_pbuf[4]  =  ptm->tm_min;
            Hci_pbuf[5]  =  ptm->tm_sec;
            //闹钟
            Hci_pbuf[6]  =  i;
            Hci_pbuf[7]  =  i16>>8;
            Hci_pbuf[8]  =  i16&0x00FF;
            Hci_pbuf[9]  =  i;
            Hci_pbuf[10] =  i16>>8;
            Hci_pbuf[11] =  i16&0x00FF;
            //提醒
            BspRtc_ReadWarn(1,&Hci_pbuf[12],&i16);
            Hci_pbuf[13] =  i16>>8;
            Hci_pbuf[14] =  i16&0x00FF;
            //自启动
            BspRtc_ReadAuto(&Hci_pbuf[15],&i16,&j16);
            Hci_pbuf[16] =  i16>>8;
            Hci_pbuf[17] =  i16&0x00FF;
            Hci_pbuf[18] =  j16>>8;
            Hci_pbuf[19] =  j16&0x00FF;
            //时间格式
            BspRtc_ReadFormat(&Hci_pbuf[20]);
        }
        sprintf((char*)pbuf,"Set Data :20%02d-%02d-%02d",Hci_pbuf[0],Hci_pbuf[1]+1,Hci_pbuf[2]);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Set Time :%02d:%02d:%02d",Hci_pbuf[3],Hci_pbuf[4],Hci_pbuf[5]);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Set Alarm:(%02d)%02d:%02d (%02d)%02d:%02d",\
                Hci_pbuf[6],Hci_pbuf[7],Hci_pbuf[8],Hci_pbuf[9],Hci_pbuf[10],Hci_pbuf[11]);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Set Warn :(%02d)%02d:%02d",Hci_pbuf[12],Hci_pbuf[13],Hci_pbuf[14]);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Set Auto :(%02d)%02d:%02d  ~  %02d:%02d",Hci_pbuf[15],Hci_pbuf[16],Hci_pbuf[17],Hci_pbuf[18],Hci_pbuf[19]);
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //
        if(Hci_pbuf[20]==0)
        {
            LCD_DispStr(xpos, ypos+=20, "SetFormat:24", &sfont);
        }
        else
        {
            LCD_DispStr(xpos, ypos+=20, "SetFormat:12", &sfont);
        }
        //显示光标
        sfont.FrontColor= CL_BLACK;
        sfont.BackColor=CL_YELLOW;
        sprintf((char*)pbuf,"%02d",Hci_pbuf[Hci_cursor]);
        switch(Hci_cursor)
        {
            case 0:
                LCD_DispStr(12*8, 3*20, (char*)pbuf, &sfont);
                break;
            case 1:
                sprintf((char*)pbuf,"%02d",Hci_pbuf[Hci_cursor]+1);
                LCD_DispStr(15*8, 3*20, (char*)pbuf, &sfont);
                break;
            case 2:
                LCD_DispStr(18*8, 3*20, (char*)pbuf, &sfont);
                break;
            case 3:
                LCD_DispStr(10*8, 4*20, (char*)pbuf, &sfont);
                break;
            case 4:
                LCD_DispStr(13*8, 4*20, (char*)pbuf, &sfont);
                break;
            case 5:
                LCD_DispStr(16*8, 4*20, (char*)pbuf, &sfont);
                break;
            case 6:
                LCD_DispStr(11*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 7:
                LCD_DispStr(14*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 8:
                LCD_DispStr(17*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 9:
                LCD_DispStr(21*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 10:
                LCD_DispStr(24*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 11:
                LCD_DispStr(27*8, 5*20, (char*)pbuf, &sfont);
                break;
            case 12:
                LCD_DispStr(11*8, 6*20, (char*)pbuf, &sfont);
                break;
            case 13:
                LCD_DispStr(14*8, 6*20, (char*)pbuf, &sfont);
                break;
            case 14:
                LCD_DispStr(17*8, 6*20, (char*)pbuf, &sfont);
                break;
            case 15:
                LCD_DispStr(11*8, 7*20, (char*)pbuf, &sfont);
                break;
            case 16:
                LCD_DispStr(14*8, 7*20, (char*)pbuf, &sfont);
                break;
            case 17:
                LCD_DispStr(17*8, 7*20, (char*)pbuf, &sfont);
                break;
            case 18:
                LCD_DispStr(24*8, 7*20, (char*)pbuf, &sfont);
                break;
            case 19:
                LCD_DispStr(27*8, 7*20, (char*)pbuf, &sfont);
                break;
            case 20:
                if(Hci_pbuf[Hci_cursor]==0)
                {
                    LCD_DispStr(10*8, 8*20, "24", &sfont);
                }
                else
                {
                    LCD_DispStr(10*8, 8*20, "12", &sfont);
                }
                break;
            default:
                break;
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                //容错
                //更新日期时间
                ptm=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                ptm->tm_year=2000+Hci_pbuf[0];
                ptm->tm_mon =Hci_pbuf[1];
                ptm->tm_mday=Hci_pbuf[2];
                ptm->tm_hour=Hci_pbuf[3];
                ptm->tm_min =Hci_pbuf[4];
                ptm->tm_sec =Hci_pbuf[5];
                BspRtc_SetRealTime(NULL,ptm,NULL,NULL);
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
                //更新闹钟
                i    =  Hci_pbuf[6];
                i16  =  Hci_pbuf[7]*256+Hci_pbuf[8];
                BspRtc_SetAlarm(1,&i16,&i);
                i    =  Hci_pbuf[9];
                i16  =  Hci_pbuf[10]*256+Hci_pbuf[11];
                BspRtc_SetAlarm(2,&i16,&i);
                //更新提醒
                i    =  Hci_pbuf[12];
                i16  =  Hci_pbuf[13]*256+Hci_pbuf[14];
                BspRtc_SetWarn(1,i,i16);
                //更新自启动
                i16  =  Hci_pbuf[16]*256+Hci_pbuf[17];
                j16  =  Hci_pbuf[18]*256+Hci_pbuf[19];
                BspRtc_SetAuto(Hci_pbuf[15],i16,j16);
                //更新制式
                BspRtc_SetFormat(Hci_pbuf[20]);
            //清除缓存
            //break;
            case HCI_KEY_ESC:
                Hci_cursor = 0;
                pHci_MenuFun = Hci_Menu_II_Set;
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                Hci_ParaChangeSign=2;
                Hci_pbuf[Hci_cursor]++;
                switch(Hci_cursor)
                {
                    case 0:
                        Hci_pbuf[Hci_cursor]%=38+1;
                        break;
                    case 1:
                        Hci_pbuf[Hci_cursor]%=11+1;
                        break;
                    case 2:
                        Hci_pbuf[Hci_cursor]%=31+1;
                        break;
                    case 3:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 4:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 5:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    //
                    case 6:
                        Hci_pbuf[Hci_cursor]%=2+1;
                        break;
                    case 7:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 8:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 9:
                        Hci_pbuf[Hci_cursor]%=2+1;
                        break;
                    case 10:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 11:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 12:
                        Hci_pbuf[Hci_cursor]%=1+1;
                        break;
                    case 13:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 14:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 15:
                        Hci_pbuf[Hci_cursor]%=1+1;
                        break;
                    case 16:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 17:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 18:
                        Hci_pbuf[Hci_cursor]%=23+1;
                        break;
                    case 19:
                        Hci_pbuf[Hci_cursor]%=59+1;
                        break;
                    case 20:
                        Hci_pbuf[Hci_cursor]%=1+1;
                    default:
                        break;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor>=20)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 设置背光
*******************************************************************************/
static void Hci_Menu_SetBackLight(void)
{
    uint8_t *pbuf;
    uint8_t res;
    FONT_T sfont;
    uint16_t ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;

    // 关闭智能省电功能
    uctsk_Rfms_PowerSavingOff = 1;
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-Set BackLight");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        res=LCD_GetBackLight();
        sprintf((char*)pbuf,"%03d",res);
        sfont.FontCode=FC_ST_64;
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*32)/2, (g_LcdHeight-64)/2, (char*)pbuf, &sfont);
        //显示光标
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
            case HCI_KEY_ESC:
                //保存现有参数
                if(Hci_ParaChangeSign==1)
                {
                    MODULE_MEMORY_S_PARA *pPara;
                    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                    pPara->LcdBackLight1=LCD_GetBackLight();
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
                }
                //
                pHci_MenuFun = Hci_Menu_II_Set;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                // 开启智能省电功能
                uctsk_Rfms_PowerSavingOff=0;
                break;
            case HCI_KEY_LEFT:
                Hci_ParaChangeSign=1;
                res=LCD_GetBackLight();
                if(res<5)
                    res+=1;
                else if(res<(255-5))
                    res+=5;
                LCD_SetBackLight(res);
                break;
            case HCI_KEY_RIGHT:
                Hci_ParaChangeSign=1;
                res=LCD_GetBackLight();
                if(res>5)
                    res-=5;
                else if(res>1)
                    res-=1;
                LCD_SetBackLight(res);
                break;
            default:
                break;
        }
    }
}
//------------------------------------------------------------------------------
static void Hci_Menu_SetMobileModule(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
        LCD_ClrScr(sfont.BackColor);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-Set MobileModule");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        //首次进入装载参数
        if(Hci_ParaChangeSign==0)
        {
            MODULE_MEMORY_S_PARA *pspara;
            //申请缓存
            pspara = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            //数据包
            Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
            Hci_cursor=pspara->GprsServer;
            //释放缓存
            MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
        }
        if(Hci_cursor==0)
        {
            sprintf((char*)pbuf,"Server: Formal");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"URL/IP: %s",GPRSAPP_XKAP_DNS_STR_FORMAL);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Port  : %s",GPRSAPP_XKAP_PORT_STR_FORMAL);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else if(Hci_cursor==1)
        {
            sprintf((char*)pbuf,"Server: Integration");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"URL/IP: %s",GPRSAPP_XKAP_DNS_STR_INTEGRATION);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Port  : %s",GPRSAPP_XKAP_PORT_STR_INTEGRATION);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else if(Hci_cursor==2)
        {
            sprintf((char*)pbuf,"Server: XK");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"URL/IP: %s",GPRSAPP_XKAP_DNS_STR_XK);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Port  : %s",GPRSAPP_XKAP_PORT_STR_XK);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else
        {
            sprintf((char*)pbuf,"Server: Format");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"URL/IP: %s",GPRSAPP_XKAP_DNS_STR_FORMAL);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Port  : %s",GPRSAPP_XKAP_PORT_STR_FORMAL);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            Hci_cursor=0;
        }

        //显示光标
        sfont.FrontColor   =  CL_BLACK;
        sfont.BackColor    =  CL_YELLOW;
        LCD_DispStr(8*12, ypos, (char*)&pbuf[12], &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                //保存现有参数
                if(Hci_ParaChangeSign==1)
                {
                    MODULE_MEMORY_S_PARA *pPara;
                    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                    pPara->GprsServer =  Hci_cursor;
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
                    MCU_SOFT_RESET;
                }
                //
                pHci_MenuFun = Hci_Menu_II_Set;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Set;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                break;
            case HCI_KEY_LEFT:
                Hci_ParaChangeSign=1;
                if(Hci_cursor<2)
                {
                    Hci_cursor++;
                }
                else
                {
                    Hci_cursor=0;
                }
                break;
            case HCI_KEY_RIGHT:
                Hci_ParaChangeSign=1;
                if(Hci_cursor!=0)
                {
                    Hci_cursor--;
                }
                else
                {
                    Hci_cursor=2;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 设置语言
*******************************************************************************/
static void Hci_Menu_SetLanguage(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-SetLanguage");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1,English");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2,Chinese");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3,Japanese");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                switch(Hci_cursor)
                {
                    case 0:
                        Language_Set(LANGUAGE_E_ENGLISH);
                        break;
                    case 1:
                        Language_Set(LANGUAGE_E_CHINESE);
                        break;
                    case 2:
                        Language_Set(LANGUAGE_E_JAPANESE);
                        break;
                    default:
                        break;
                }
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Set;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                Hci_cursor = 0;
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor>=2)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=2;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 设置功能开启与关闭
*******************************************************************************/
static void Hci_Menu_SetFunctionOnOff(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA* pPara;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            if(Hci_pbuf==NULL)
            {
                Hci_pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)Hci_pbuf,NULL);
            }
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-Set Function On/Off");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1,Gprs upload DayMove");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2,Gprs Keep On");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 3,NC");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示数值
        pPara=(MODULE_MEMORY_S_PARA*)Hci_pbuf;
        if(pPara->Flag_GprsUploadDayMove==ON)
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 20, (char*)"ON ", &sfont);
        }
        else
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 20, (char*)"OFF", &sfont);
        }
        if(pPara->Flag_GprsKeepOn==ON)
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 40, (char*)"ON ", &sfont);
        }
        else
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 40, (char*)"OFF", &sfont);
        }
        if(pPara->Flag_Res3==ON)
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 60, (char*)"ON ", &sfont);
        }
        else
        {
            LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 60, (char*)"OFF", &sfont);
        }
        //显示光标
        sfont.FrontColor=CL_BLACK;
        sfont.BackColor=CL_YELLOW;
        switch(Hci_cursor)
        {
            case 0:
                if(pPara->Flag_GprsUploadDayMove==ON)
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 20, (char*)"ON ", &sfont);
                }
                else
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 20, (char*)"OFF", &sfont);
                }
                break;
            case 1:
                if(pPara->Flag_GprsKeepOn==ON)
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 40, (char*)"ON ", &sfont);
                }
                else
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 40, (char*)"OFF", &sfont);
                }
                break;
            case 2:
                if(pPara->Flag_Res3==ON)
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 60, (char*)"ON ", &sfont);
                }
                else
                {
                    LCD_DispStr(g_LcdWidth-strlen((char*)pbuf)*5, 60, (char*)"OFF", &sfont);
                }
                break;
            default:
                break;
        }
        sfont.FrontColor=CL_YELLOW;
        sfont.BackColor=CL_BLACK;
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        pPara=(MODULE_MEMORY_S_PARA*)Hci_pbuf;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                if((pPara->Flag_GprsUploadDayMove!=ModuleMemory_psPara->Flag_GprsUploadDayMove)\
                   ||(pPara->Flag_GprsKeepOn!=ModuleMemory_psPara->Flag_GprsKeepOn)\
                   ||(pPara->Flag_Res3!=ModuleMemory_psPara->Flag_Res3)\
                  )
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)Hci_pbuf,NULL);
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Set;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                Hci_cursor = 0;
                if(Hci_pbuf!=NULL)
                {
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                }
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor>=2)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            case HCI_KEY_RIGHT:
                switch(Hci_cursor)
                {
                    case 0:
                        if(pPara->Flag_GprsUploadDayMove==ON)
                        {
                            pPara->Flag_GprsUploadDayMove=OFF;
                        }
                        else
                        {
                            pPara->Flag_GprsUploadDayMove=ON;
                        }
                        break;
                    case 1:
                        if(pPara->Flag_GprsKeepOn==ON)
                        {
                            pPara->Flag_GprsKeepOn=OFF;
                        }
                        else
                        {
                            pPara->Flag_GprsKeepOn=ON;
                        }
                        break;
                    case 2:
                        if(pPara->Flag_Res3==ON)
                        {
                            pPara->Flag_Res3=OFF;
                        }
                        else
                        {
                            pPara->Flag_Res3=ON;
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 设置主题
*******************************************************************************/
#if 0
static void Hci_Menu_SetTheme(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pPara;
    FONT_T sfont;
    uint16_t xpos=10,ypos=0;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_ParaChangeSign==0)
        {
            Hci_ParaChangeSign=1;
            LCD_ClrScr(sfont.BackColor);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示信息
        sprintf((char*)pbuf,"Menu-SetTheme");
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, ypos, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 1,Classics");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 2,Windows");
        LCD_DispStr(xpos, ypos+=20, (char*)pbuf, &sfont);
        //显示光标
        LCD_DispStr(xpos, (Hci_cursor+1)*20, "*", &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                // 保存主题参数
                switch(Hci_cursor)
                {
                    case 0:
                    case 1:
                        pPara->Theme=Hci_cursor;
                        break;
                    default:
                        pPara->Theme=1;
                        break;
                }
                // 修改主题
                switch(pPara->Theme)
                {
                    case 0:
                        pHci_DefaultMenuFun=Hci_Menu_Default1;
                        break;
                    case 1:
#ifdef HCI_EMWIN_ENABLE
                        pHci_DefaultMenuFun=Hci_Menu_Default1;
#else
                        pHci_DefaultMenuFun=Hci_Menu_Default1;
#endif
                        break;
                    default:
#ifdef HCI_EMWIN_ENABLE
                        pPara->Theme=1;
                        pHci_DefaultMenuFun=Hci_Menu_Default1;
#else
                        pPara->Theme=0;
                        pHci_DefaultMenuFun=Hci_Menu_Default1;
#endif
                        break;
                }
                //
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Set;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                Hci_cursor = 0;
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor>=1)
                {
                    Hci_cursor=0;
                }
                else
                {
                    Hci_cursor++;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor==0)
                {
                    Hci_cursor=1;
                }
                else
                {
                    Hci_cursor--;
                }
                break;
            default:
                break;
        }
    }
}
#endif
//------------------------------------------------------------------------------
static void Hci_Menu_TestLcd(void)
{
#if 0
    uint8_t i,j,k;

    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;

    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            // 申请缓存
            Hci_pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        }
        //EmWinXslApp_ShowStreamedBMP(NULL,0,0);
        GUI_SetPenSize(1);
        // 体动记录
        // 测试时间
        // 标题

        i=sprintf((char*)Hci_pbuf,"第%01d条记录: 睡眠结构曲线",1);
        //黄色显示"睡眠分级曲线"
        LCD_DispStr((g_LcdWidth-i*8)/2, 8, (char*)Hci_pbuf, &sfont);
        //白色重画
        sprintf((char*)Hci_pbuf,"第%01d条记录: ",1);
        sfont.FrontColor=CL_WHITE;
        LCD_DispStr((g_LcdWidth-i*8)/2, 8, (char*)Hci_pbuf, &sfont);

        // 画格
        GUI_SetColor(HCI_TEST_COLOR_1);
        for(i=0; i<4; i++)
        {
            GUI_DrawHLine(70+i*50,40,40+240);
            for(j=0; j<7; j++)
            {
                GUI_DrawVLine(40+40*j,70+i*50-25,70+i*50);
                if(j!=6)
                {
                    GUI_DrawVLine(40+10+40*j,70+i*50-10,70+i*50);
                    GUI_DrawVLine(40+20+40*j,70+i*50-15,70+i*50);
                    GUI_DrawVLine(40+30+40*j,70+i*50-10,70+i*50);
                }
                else
                {
                    break;
                }
            }
        }
        // 写时间
        GUI_SetColor(HCI_TEST_COLOR_2);
        GUI_SetFont(&GUI_FontFont_ST_16);
        k=11;
        for(i=0; i<4; i++)
        {
            for(j=0; j<7; j++)
            {
                k++;
                k%=24;
                sprintf((char*)Hci_pbuf,"%02d",k);
                GUI_DispStringHCenterAt  ((const char *) Hci_pbuf, 40+40*j, 70+i*50+3);
            }
        }

    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Test;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                LCD_ClrScr(CL_BLACK);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
    /*
    FONT_T sfont;
    static uint8_t sback=0;
    //static uint8_t scmt=0;
    static uint8_t si=0;
    //static uint8_t sj=0;
    //
    if(si==0)
    {
        si=10;
        uctsk_Rfms_PowerSavingOff=1;
    }
    //
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(sback==0)
    {
        sback=LCD_GetBackLight();
    }
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
        //
        {
            const uint16_t T1=4000;
            const uint16_t T2=250;
            const uint16_t T2_HALF=T2/2;
            //
            static uint16_t s_ms=0;
            uint16_t y;
            uint8_t num,num1;
            uint16_t ms;
            num = s_ms/T2_HALF;
            //
            if(s_ms<=(T1/2))
            {
                ms = s_ms;
                num1 = num;
            }
            else
            {
                ms = T1-s_ms;
                num1 = T1/T2_HALF-num-1;
            }
            //
            if(num1%2==0)
            {
                y = ((ms-T2_HALF*num1)*((num1+1)*T2_HALF/((T1/2)/T2)))/T2_HALF;
            }
            else
            {
                y = ((ms-T2_HALF*num1)*(-(num1*T2_HALF/((T1/2)/T2))))/T2_HALF +(num1*T2_HALF/((T1/2)/T2));
            }
            //
            s_ms += 10;
            if(s_ms>=T1)
            {
                s_ms=0;
            }
            LCD_SetBackLight((uint8_t)y);
        }
    }
    else
    {
        if(Hci_ParaChangeSign==0)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_WHITE);
        }
        else if(Hci_ParaChangeSign==1)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_BLUE);
        }
        else if(Hci_ParaChangeSign==2)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_GREEN);
        }
        else if(Hci_ParaChangeSign==3)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_RED);
        }
        else if(Hci_ParaChangeSign==4)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_BLACK);
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Test;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                //
                uctsk_Rfms_PowerSavingOff=0;
                LCD_SetBackLight(sback);
                break;
            case HCI_KEY_LEFT:
                if(Hci_ParaChangeSign>=4)
                {
                    Hci_ParaChangeSign=0;
                }
                else
                {
                    Hci_ParaChangeSign++;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_ParaChangeSign==0)
                {
                    Hci_ParaChangeSign=4;
                }
                else
                {
                    Hci_ParaChangeSign--;
                }
                break;
            default:
                break;
        }
    }
    */
#else
    //---------------------------------
    /*
    MainTask();
    pHci_MenuFun = Hci_Menu_II_Test;
    */
    //---------------------------------
    //uint8_t y=0;
    //uint8_t *pbuf;
    FONT_T sfont;
    //
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        if(Hci_ParaChangeSign==0)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_WHITE);
        }
        else if(Hci_ParaChangeSign==1)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_BLUE);
        }
        else if(Hci_ParaChangeSign==2)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_GREEN);
        }
        else if(Hci_ParaChangeSign==3)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_RED);
        }
        else if(Hci_ParaChangeSign==4)
        {
            Hci_ReShowTimer=0xFFFF;
            LCD_ClrScr(CL_BLACK);
        }
        /*
        else if(Hci_ParaChangeSign==5)
        {
            Hci_ReShowTimer=1;
            Hci_ParaChangeSign++;
            //清屏
            LCD_ClrScr(sfont.BackColor);
        }
        else
        {
            Hci_ReShowTimer=1;
            y=0;
            pbuf=(uint8_t*)"Menu-Test LCD";
            LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, y,(char*)pbuf, &sfont);
            y=20;
            //12*24字体
            sfont.FontCode=FC_ST_24;
            LCD_DispStr(0, y, "测0123456789:-", &sfont);
            y+=24;
            //16*32字体
            sfont.FontCode=FC_ST_32;
            LCD_DispStr(0, y, "测0123456789:-%C", &sfont);
            y+=32;
            //32*64字体
            sfont.FontCode=FC_ST_64;
            LCD_DispStr(0, y, "0123456789", &sfont);
            y+=64;
            //画控件
            {
                uint16_t ColorArray[]= {CL_RED,CL_GREEN,CL_BLUE};
                Hci_CursorFlick++;
                Hci_CursorFlick%=101;
                LCD_Draw_Bar1(20,y,320-20,y+((g_LcdHeight-y)/2),ColorArray,3,Hci_CursorFlick);
            }
        }
        */
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Test;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_ParaChangeSign>=4)
                {
                    Hci_ParaChangeSign=0;
                }
                else
                {
                    Hci_ParaChangeSign++;
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_ParaChangeSign==0)
                {
                    Hci_ParaChangeSign=4;
                }
                else
                {
                    Hci_ParaChangeSign--;
                }
                break;
            default:
                break;
        }
    }
#endif
}
//------------------------------------------------------------------------------
static void Hci_Menu_TestBuzzer(void)
{
    uint8_t *pbuf;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //清屏
        LCD_ClrScr(sfont.BackColor);
        pbuf=(uint8_t*)"Menu-Test Buzzer";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        pbuf=(uint8_t*)"Press Enter Run Buzzer Or Stop";
        LCD_DispStr(0, 20,(char*)pbuf, &sfont);
        pbuf=(uint8_t*)"Press Esc Return menu";
        LCD_DispStr(0, 40,(char*)pbuf, &sfont);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                if(NULL!=Bsp_BuzzerMusic_GetState())
                {
                    Bsp_BuzzerMusic_PlayStop(OFF,NULL,0);
                }
                else
                {
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_MORNING,0xFFFF);
                }
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Test;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                //
                Bsp_BuzzerMusic_PlayStop(OFF,NULL,0);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_TestGsm(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pPara;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //清屏
        LCD_ClrScr(sfont.BackColor);
        pbuf=(uint8_t*)"Menu-Test Gsm";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        pbuf=(uint8_t*)"Press Enter Send test data";
        LCD_DispStr(0, 20,(char*)pbuf, &sfont);
        pbuf=(uint8_t*)"Press Esc Return menu";
        LCD_DispStr(0, 40,(char*)pbuf, &sfont);
    }
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Bsp_CpuFlash_Read(BSP_CPU_FLASH_PARA_ADDR,(uint8_t*)pPara,sizeof(MODULE_MEMORY_S_PARA));
    if(pPara->Flag_GprsRegister==0)
    {
        pbuf=(uint8_t*)"--------------------(0)";
    }
    else
    {
        pbuf=(uint8_t*)"--------------------(1)";
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
    LCD_DispStr(0, 60,(char*)pbuf, &sfont);
    //
    if(GprsNet_s_Info.state==0)
    {
        pbuf=(uint8_t*)"State: Not found the GSM      ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(GprsNet_s_Info.Err_SimCardInstall==1)
    {
        pbuf=(uint8_t*)"State: Not found SimCard      ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state!=GPRSNET_E_CONNECT_STATE_CONNECTED)
    {
        pbuf=(uint8_t*)"State: GPRS Connecting...     ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(GprsAppXkap_UpdataState==1)
    {
        pbuf=(uint8_t*)"State: Gprs Data uploading... ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(GprsAppXkap_UpdataState==2)
    {
        GprsAppXkap_UpdataState=0;
        pbuf=(uint8_t*)"State: Gprs Data uploaded OK! ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(GprsAppXkap_UpdataState==3)
    {
        GprsAppXkap_UpdataState=0;
        pbuf=(uint8_t*)"State: Gprs Data uploaded ERR!";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    else if(Hci_NoFirst==0)
    {
        Hci_NoFirst=1;
        pbuf=(uint8_t*)"State: Please Press Enter!    ";
        LCD_DispStr(0, 80,(char*)pbuf, &sfont);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                if(GprsAppXkap_UpdataState==0)
                {
                    GprsAppXkap_WrCmd(GPRSAPP_XKAP_E_CMD_SEND_IMSI);
                }
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Test;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoChip(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *psPara;
    uint16_t ypos=0,i16;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        //Hci_ReShowTimer=100*5;
        Hci_ReShowTimer=10;
        //
        pbuf=(uint8_t*)"Menu-Info Chip";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        BspCpuId_Get(&pbuf[100],&i16);
        sprintf((char*)pbuf,"CpuID -%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x(%d KB)",
                pbuf[100],pbuf[101],pbuf[102],pbuf[103],pbuf[104],pbuf[105],
                pbuf[106],pbuf[107],pbuf[108],pbuf[109],pbuf[110],pbuf[111],i16);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //ExtFlash型号

        sprintf((char*)pbuf,"SPI Flash    - %s",BspExtFlash_s_Flash.ChipName);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);

        //上电次数
        psPara = (MODULE_MEMORY_S_PARA *)&pbuf[128];
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)psPara,NULL);
        sprintf((char*)pbuf,"PowerOn Count- %d",BSP_BKPRAM_READ(BSP_BKPRAM_POWER_CMT));
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //
        sprintf((char*)pbuf,"Batter Time  - %d(hour)",BSP_BKPRAM_READ(BSP_BKPRAM_BELL_TIME_H));
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //复位次数
        sprintf((char*)pbuf,"Reset Count  - %d",psPara->ResetCount);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示CPU温度
        sprintf((char*)pbuf,"CPU Temp     - %d",ADC_Temperature);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示参考电压
        sprintf((char*)pbuf,"CPU Vref     - %04d(mV)",ADC_Vref_mV);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
#ifdef STM32F4
        //显示电源电压
        sprintf((char*)pbuf,"Power DC     - %04d(mV)",ADC_mV[AD_E_CHANNEL_AC]*2);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示电池电压
        sprintf((char*)pbuf,"Power Cell   - %04d(mV)",ADC_mV[AD_E_CHANNEL_BAT]*2);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示Vbat电压
        ADC_Vbat_mV = AD_VBAT_Read_mv();
        sprintf((char*)pbuf,"CPU Vbat     - %04d(mV)",ADC_Vbat_mV);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
#endif
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoVer(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pPara;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
        pbuf=(uint8_t*)"Menu-Info Ver";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);

        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //显示产品名称
        sprintf((char*)pbuf,"Product Name: %s",PRODUCT_NAME);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //
        pPara=(MODULE_MEMORY_S_PARA *)pbuf;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        //显示BOOT版本
        sprintf((char*)pbuf,"Boot     Ver: %02d",pPara->BootVer);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示硬件版本
        sprintf((char*)pbuf,"Hardware Ver: %02d.%02d",HARDWARE_VER,HARDWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示软件版本
        sprintf((char*)pbuf,"Software Ver: %02d.%02d",SOFTWARE_VER,SOFTWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)pbuf , &sfont);
        //显示出厂日期
        sprintf((char*)pbuf,"Factory Date: %4d-%02d-%02d %02d:%02d:%02d",\
                FACTORY_TIME_YEAR,FACTORY_TIME_MONTH,FACTORY_TIME_DAY,\
                FACTORY_TIME_HOUR,FACTORY_TIME_MINUTE,FACTORY_TIME_SECONT);
        LCD_DispStr(0, ypos+=20, (char*)pbuf , &sfont);
        // RFMS版本信息
        sprintf((char*)pbuf,"Rfms H/S Ver: %03d.%03d",uctsk_Rfms_Hardware,uctsk_Rfms_Software);
        LCD_DispStr(0, ypos+=20, (char*)pbuf , &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}

static void Hci_Menu_InfoOS(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pPara;
    uint16_t ypos=0;
    uint16_t i16;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        //Hci_ReShowTimer=100*5;
        Hci_ReShowTimer=10;
        //
        pbuf=(uint8_t*)"Menu-Info OS";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pPara  =  (MODULE_MEMORY_S_PARA*)&pbuf[40];
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        //系统版本
        MODULE_OS_VER(i16);
        sprintf((char*)pbuf,"OS Version: %01d.%02d.%02d",i16/10000,(i16%10000)/100,i16%100);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示运行开机时长
        sprintf((char*)pbuf,"Run  Time: %ld(s)",BspRtc_GetBeginToNowS());
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示缓存使用
        sprintf((char*)pbuf,"256B used: now-%02d past-%02d max-%02d",MemManager_Mem_256B_Num,MemManager_Mem_256B_MaxNum,MEM_256B_BLK_NBR);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"1KB  used: now-%02d past-%02d max-%02d",MemManager_Mem_1KB_Num,MemManager_Mem_1KB_MaxNum,MEM_1KB_BLK_NBR);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"2KB  used: now-%02d past-%02d max-%02d",MemManager_Mem_2KB_Basic_Num,MemManager_Mem_2KB_Basic_MaxNum,MEM_2KB_BASIC_BLK_NBR);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"5KB  used: now-%02d past-%02d max-%02d",MemManager_Mem_5KB_Basic_Num,MemManager_Mem_5KB_Basic_MaxNum,MEM_5KB_BASIC_BLK_NBR);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"10KB used: now-%02d past-%02d max-%02d",MemManager_Mem_10KB_Num,MemManager_Mem_10KB_MaxNum,MEM_10KB_BLK_NBR);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示硬件错误发生次数
        sprintf((char*)pbuf,"HardFault Count: %d",pPara->HardFault_Count);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示硬件错误发生时间
        sprintf((char*)pbuf,"HardFault Time : %04d-%02d-%02d %02d:%02d:%02d",\
                pPara->HardFault_Time[0]+2000,pPara->HardFault_Time[1],pPara->HardFault_Time[2],
                pPara->HardFault_Time[3],pPara->HardFault_Time[4],pPara->HardFault_Time[5]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //显示硬件错误发生任务
        sprintf((char*)pbuf,"HardFault Task : %s",pPara->pHardFaultTaskName);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoMobileModule(void)
{
    uint8_t *pbuf;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=10;
        //
        pbuf=(uint8_t*)"Menu-Info MobileModule";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(GprsNet_s_Info.state==0)
        {
            sprintf((char*)pbuf,"Not found the GSM module!");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else if(GprsNet_s_Info.Err_SimCardInstall==1)
        {
            sprintf((char*)pbuf,"Not found the SimCard!");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else
        {
            //
            GprsNet_s_Info.PhoneNumber[11] = 0;
            sprintf((char*)pbuf,"IMEI  -%s(%s)",GprsNet_s_Info.IMEI,GprsNet_s_Info.MoudleStr);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"IMSI  -%s(%s)",GprsNet_s_Info.IMSI,GprsNet_s_Info.PhoneNumber);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"LocIp -%03d.%03d.%03d.%03d",GprsNet_s_Info.LotalIP[0],GprsNet_s_Info.LotalIP[1],GprsNet_s_Info.LotalIP[2],GprsNet_s_Info.LotalIP[3]);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"LAC-0x%02x%02x CID-0x%02x%02x",GprsNet_s_Info.LacCi[0],GprsNet_s_Info.LacCi[1],GprsNet_s_Info.LacCi[2],GprsNet_s_Info.LacCi[3]);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"GPS Lon-%03ld.%06ld Lat-%03ld.%06ld",GprsNet_s_Info.GPS_Longitude/1000000,GprsNet_s_Info.GPS_Longitude%1000000\
                    ,GprsNet_s_Info.GPS_Latitude/1000000,GprsNet_s_Info.GPS_Latitude%1000000);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Signal-%d TxNum-%ld RxNum-%ld",GprsNet_s_Info.SignalVal,GprsAppXkap_TxNum,GprsAppXkap_RxNum);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"MovePower TxNum-%ld RxNum-%ld",GprsAppXkap_MovePowerTxNum,GprsAppXkap_MovePowerRxNum);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Flow-%ld",GprsNet_s_Info.Flow_B);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"Login-%d",GprsAppXkap_Sign_Login);
            LCD_DispStr(8*20, ypos, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"1-%s:%s",Xkap_pConnectDnsStr,Xkap_pConnectPortStr);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoBluetooth(void)
{
    uint8_t i;
    uint8_t *pbuf;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            // 用于子页面翻屏
            Hci_ParaChangeSign=  0;
            // 用于清屏
            Hci_CursorFlick   =  0;
            // 光标位置
            Hci_cursor        =  0;
        }
        if(Hci_CursorFlick==1)
        {
            Hci_CursorFlick=0;
            LCD_ClrScr(sfont.BackColor);
        }
        //
        pbuf=(uint8_t*)"Menu-Info Bluetooth";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //S200B显示
        if(Bluetooth_s_info.Mode==1)
        {
            sprintf((char*)pbuf,"Type          : iSleep200B");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"MAC: %02x:%02x:%02x:%02x:%02x:%02x",\
                    Bluetooth_s_info.MacBuf[0],Bluetooth_s_info.MacBuf[1],\
                    Bluetooth_s_info.MacBuf[2],Bluetooth_s_info.MacBuf[3],\
                    Bluetooth_s_info.MacBuf[4],Bluetooth_s_info.MacBuf[5]);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"ChipPowerCount: %05d",Bluetooth_ChipPowerCount);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"ChipOkCount   : %05d",Bluetooth_ChipOkCount);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"ChipErrCount  : %05d",Bluetooth_ChipErrCount);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,"ChipNCCount   : %05d",Bluetooth_ChipNCCount);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        //S200G显示---(模块为广播透传模块)
        else if(Bluetooth_s_info.Type==1)
        {
            // 显示3屏信息
            switch(Hci_ParaChangeSign)
            {
                // 第一屏显示
                case 0:
                    sprintf((char*)pbuf,"Type          : iCareB-D");
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"Hardware Ver  : %02d.%02d",Bluetooth_s_info.HardwareVer_m,Bluetooth_s_info.HardwareVer_s);
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"Software Ver  : %02d.%02d",Bluetooth_s_info.SoftwareVer_m,Bluetooth_s_info.SoftwareVer_s);
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"MAC: %02x:%02x:%02x:%02x:%02x:%02x",\
                            Bluetooth_s_info.MacBuf[0],Bluetooth_s_info.MacBuf[1],\
                            Bluetooth_s_info.MacBuf[2],Bluetooth_s_info.MacBuf[3],\
                            Bluetooth_s_info.MacBuf[4],Bluetooth_s_info.MacBuf[5]);
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"----------");
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"Uart-Tx       : %d",Bluetooth_s_info.UartTxNum);
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    sprintf((char*)pbuf,"Uart-Rx       : %d",Bluetooth_s_info.UartRxNum);
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    break;
                // 第二屏显示
                case 1:
                    sprintf((char*)pbuf," No.MAC               Time  TYPE RSSI");
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    for(i=0; i<10; i++)
                    {
                        if(BluetoothDtu_s_SlaveBuf[i].state==0)
                        {
                            sprintf((char*)pbuf," %02d.NULL",i+1);
                        }
                        else
                        {
                            sprintf((char*)pbuf," %02d.%02x:%02x:%02x:%02x:%02x:%02x %05d %02X   %03d ",\
                                    i+1,\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[5],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[4],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[3],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[2],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[1],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[0],\
                                    BluetoothDtu_s_SlaveBuf[i].timeover_s,\
                                    BluetoothDtu_s_SlaveBuf[i].Type,\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.rssi);
                        }
                        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    }
                    // 显示*
                    if(Hci_cursor!=0)
                    {
                        sprintf((char*)pbuf,"*");
                        LCD_DispStr(0, 20+Hci_cursor*20, (char*)pbuf, &sfont);
                    }
                    break;
                // 第三屏显示
                case 2:
                    sprintf((char*)pbuf," No.MAC               Time  TYPE RSSI");
                    LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    for(i=10; i<20; i++)
                    {
                        if(BluetoothDtu_s_SlaveBuf[i].state==0)
                        {
                            sprintf((char*)pbuf," %02d.NULL",i+1);
                        }
                        else
                        {
                            sprintf((char*)pbuf," %02d.%02x:%02x:%02x:%02x:%02x:%02x %05d %02X   %03d ",\
                                    i+1,\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[5],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[4],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[3],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[2],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[1],\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.peer_addr.addr[0],\
                                    BluetoothDtu_s_SlaveBuf[i].timeover_s,\
                                    BluetoothDtu_s_SlaveBuf[i].Type,\
                                    BluetoothDtu_s_SlaveBuf[i].AdvReport.rssi);
                        }
                        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                    }
                    // 显示*
                    if(Hci_cursor!=0)
                    {
                        sprintf((char*)pbuf,"*");
                        LCD_DispStr(0, 20+Hci_cursor*20, (char*)pbuf, &sfont);
                    }
                    break;
                default:
                    Hci_ParaChangeSign = 0;
                    break;
            }

        }
        else
        {
            sprintf((char*)pbuf,"Not found the bluetooth module!");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        ypos=ypos;
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                if(Bluetooth_s_info.Type==1)
                {
                    // 首页不解析
                    if(Hci_ParaChangeSign==0)
                    {
                        break;
                    }
                    // 未启动光标,则开启光标
                    if(Hci_cursor==0 || Hci_cursor>10)
                    {
                        Hci_cursor=1;
                        break;
                    }
                    // 解析光标
                    i = (Hci_cursor - 1) + (Hci_ParaChangeSign-1)*10;
                    pHci_MenuFun = Hci_Menu_InfoBluetooth_Slave;
                    Hci_ReShowTimer     =  0;
                    Hci_ParaChangeSign  =  0;
                    Hci_CursorFlick     =  0;
                    Hci_NoFirst         =  0;
                    Hci_cursor          =  i;
                    LCD_ClrScr(sfont.BackColor);
                }
                break;
            case HCI_KEY_ESC:
                if(Bluetooth_s_info.Type==1)
                {
                    // 启动光标,则退出光标
                    if(Hci_cursor!=0)
                    {
                        Hci_cursor=0;
                        break;
                    }
                }
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor!=0)
                {
                    if(Hci_cursor!=1)
                    {
                        Hci_cursor--;
                    }
                }
                else if(Hci_ParaChangeSign!=0)
                {
                    Hci_ParaChangeSign--;
                    Hci_CursorFlick=1;
                }
                Hci_ReShowTimer = 0;
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor!=0)
                {
                    if(Hci_cursor<10)
                    {
                        Hci_cursor++;
                    }
                }
                else if(Hci_ParaChangeSign<2)
                {
                    Hci_ParaChangeSign++;
                    Hci_CursorFlick=1;
                }
                Hci_ReShowTimer = 0;
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoBluetooth_Slave(void)
{
    //uint8_t i;
    uint8_t *pbuf;
    int16_t i16=0,j16=0;
    uint32_t i32 =0;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        if(Hci_NoFirst==0)
        {
            Hci_NoFirst=1;
            Hci_ParaChangeSign   =  BluetoothDtu_s_SlaveBuf[Hci_cursor].Type;
        }
        if(Hci_ParaChangeSign!=BluetoothDtu_s_SlaveBuf[Hci_cursor].Type)
        {
            Hci_ParaChangeSign   =  BluetoothDtu_s_SlaveBuf[Hci_cursor].Type;
            LCD_ClrScr(sfont.BackColor);
        }
        //
        pbuf=(uint8_t*)"Menu-Info Bluetooth-Slave";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        // 设备
        sprintf((char*)pbuf," No.MAC               Time  TYPE RSSI");
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," %02d.%02x:%02x:%02x:%02x:%02x:%02x %05d %02X   %03d ",\
                Hci_cursor+1,\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[5],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[4],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[3],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[2],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[1],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.peer_addr.addr[0],\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].timeover_s,\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].Type,\
                BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.rssi);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        // 事件
        switch(BluetoothDtu_s_SlaveBuf[Hci_cursor].Type)
        {
            case BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE:
                sprintf((char*)pbuf,"Type : %s",BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_NAME);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"SN   : %05d",\
                        BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_SN]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"Key  : %02d %05d(ms)",\
                        BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_VALUE_KEYNUM],\
                        BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_VALUE_KEYTIME]*100);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
            case BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_TYPE:
                sprintf((char*)pbuf,"Type : %s",BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_NAME);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"SN   : %05d",\
                        BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_SN]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                switch(BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT])
                {
                    case 1:
                        sprintf((char*)pbuf,"Event: on-bed     ");
                        break;
                    case 2:
                        sprintf((char*)pbuf,"Event: bed-away   ");
                        break;
                    case 3:
                        sprintf((char*)pbuf,"Event: Calibration");
                        break;
                    case 4:
                        sprintf((char*)pbuf,"Event: Alarm On   ");
                        break;
                    case 5:
                        sprintf((char*)pbuf,"Event: Alarm Off  ");
                        break;
                    case 6:
                        sprintf((char*)pbuf,"Event: AlarmF On  ");
                        break;
                    case 7:
                        sprintf((char*)pbuf,"Event: AlarmF Off ");
                        break;
                    default:
                        sprintf((char*)pbuf,"Event: NULL!");
                        break;
                }
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
            case BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE:
                sprintf((char*)pbuf,"Type : %s",BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_NAME);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"SN   : %05d",\
                        BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_SN]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //
                i16 = 10*(120+BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT]);
                sprintf((char*)pbuf,"VCC(mV): %04d",i16);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //
                sprintf((char*)pbuf,"Key    : 1-%03d-%05d 2-%03d-%05d  "\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+1]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+2]*100\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+3]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+4]*100\
                       );
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //
                i16 = Count_2ByteToWord(BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+6]\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+5]);
                i32 = Count_4ByteToLong(0\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+9]\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+8]\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+7]);
                sprintf((char*)pbuf,"BMP180 : T-%02d.%d AP-%05d(pa) A-%d(m)  ",i16/10,i16%10,i32,(int8_t)BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+10]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //
                i16 = Count_2ByteToWord(BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+12]\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+11]);
                j16 = Count_2ByteToWord(BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+14]\
                                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+13]);
                sprintf((char*)pbuf,"AP3216 : L-%05d PS-%05d  ",i16,j16);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //
                sprintf((char*)pbuf,"MPU6050-P: %d  ",BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_VALUE+BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_M_VALUE_EVENT+15]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
            case BSP_NRF_BLE_BEACON_PROTOCOL_TYPE:
                sprintf((char*)pbuf,"Type : %s",BSP_NRF_BLE_BEACON_PROTOCOL_NAME);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //UUID
                sprintf((char*)pbuf,"UUID : %02X %02X %02X %02X %02X %02X %02X %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+3]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+4]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+5]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+6]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+7]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+8]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+9]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+10]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"       %02X %02X %02X %02X %02X %02X %02X %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+11]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+12]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+13]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+14]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+15]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+16]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+17]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+18]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //MAJOR
                sprintf((char*)pbuf,"MAJOR: %02X %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+19]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+20]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //MINOR
                sprintf((char*)pbuf,"MINOR: %02X %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+21]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+22]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                //RSSI
                sprintf((char*)pbuf,"RSSI : %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[BSP_NRF_BLE_FACTORY_PROTOCOL_VALUE2_COMPANY_H+23]);
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
            case BSP_NRF_BLE_FACTORY_PROTOCOL_NULL_TYPE:
            case BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_TYPE:
                if(BluetoothDtu_s_SlaveBuf[Hci_cursor].Type==BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_TYPE)
                {
                    sprintf((char*)pbuf,"Type : %s",BSP_NRF_BLE_HUAMI_PROTOCOL_MIBAND2_NAME);
                }
                else
                {
                    sprintf((char*)pbuf,"Type : NULL!");
                }
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"Value: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[0]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[1]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[2]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[3]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[4]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[5]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[6]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[7]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[8]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[9]\
                       );
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"       %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[10]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[11]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[12]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[13]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[14]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[15]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[16]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[17]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[18]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[19]\
                       );
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"       %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[20]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[21]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[22]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[23]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[24]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[25]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[26]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[27]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[28]\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[29]\
                       );
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                sprintf((char*)pbuf,"       %02X"\
                        ,BluetoothDtu_s_SlaveBuf[Hci_cursor].AdvReport.data[30]\
                       );
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
            default:
                sprintf((char*)pbuf,"Type : NULL!");
                LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
                break;
        }

        //
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        LCD_ClrScr(sfont.BackColor);
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_InfoBluetooth;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                break;
            case HCI_KEY_LEFT:
                if(Hci_cursor!=0)
                {
                    Hci_cursor--;
                }
                else
                {
                    Hci_cursor=(BLUETOOTH_DTU_S_SLAVEBUF_NUM-1);
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor<(BLUETOOTH_DTU_S_SLAVEBUF_NUM-1))
                {
                    Hci_cursor++;
                }
                else
                {
                    Hci_cursor=0;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 显示QR信息
*******************************************************************************/
#define HCI_MENU_INFOQRCODE_COLOR_1    Count_4ByteToLong(0,140,90,40)
static void Hci_Menu_InfoQrCode(void)
{
    //uint32_t *pi32;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_BLACK;
    sfont.BackColor=CL_WHITE;
    sfont.Space=0;
    //首次进入
    if(Hci_NoFirst   == 0)
    {
        Hci_NoFirst =  1;
        //申请缓存
        Hci_pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //00-49   CPUID字符串
        //50-99   MAC字符串
        //100-111 CpuId数组
    }
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
        //获取数据
        BspCpuId_Get(&Hci_pbuf[100],NULL);
        sprintf((char*)Hci_pbuf,"CpuID-%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                Hci_pbuf[100],Hci_pbuf[101],Hci_pbuf[102],Hci_pbuf[103],Hci_pbuf[104],Hci_pbuf[105],
                Hci_pbuf[106],Hci_pbuf[107],Hci_pbuf[108],Hci_pbuf[109],Hci_pbuf[110],Hci_pbuf[111]);
        sprintf((char*)&Hci_pbuf[120],"ID:%02x%02x%02x-%02x%02x%02x-%02x%02x%02x-%02x%02x%02x",
                Hci_pbuf[100],Hci_pbuf[101],Hci_pbuf[102],Hci_pbuf[103],Hci_pbuf[104],Hci_pbuf[105],
                Hci_pbuf[106],Hci_pbuf[107],Hci_pbuf[108],Hci_pbuf[109],Hci_pbuf[110],Hci_pbuf[111]);
        if(Bluetooth_s_info.Mode==1)
        {
            sprintf((char*)&Hci_pbuf[50],"MAC-%02x%02x%02x%02x%02x%02x",\
                    Bluetooth_s_info.MacBuf[0],Bluetooth_s_info.MacBuf[1],Bluetooth_s_info.MacBuf[2],\
                    Bluetooth_s_info.MacBuf[3],Bluetooth_s_info.MacBuf[4],Bluetooth_s_info.MacBuf[5]);
        }
        else
        {
            Hci_pbuf[50]=0;
        }
        sprintf((char*)Hci_pbuf,"%s\n%s",(char*)Hci_pbuf,(char*)&Hci_pbuf[50]);
        //
#ifdef   HCI_EMWIN_ENABLE
        /*
        GUI_DrawGradientV(0,0,LCD_GetXSize()-1,LCD_GetYSize()-1,HCI_GUI_COLOR_BACKGROUND_UP,HCI_GUI_COLOR_BACKGROUND_DOWN);
        */
        GUI_SetColor(HCI_MENU_INFOQRCODE_COLOR_1);
        GUI_FillRect(0,0,320-1,240-1);
        if(0)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres9;
            GUI_DrawBitmap(&bmres9,0,0);
        }
        //画小护士

        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres25;
            GUI_DrawBitmap(&bmres25,20-6,60+20);
        }

        /*
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres27;
            GUI_DrawBitmap(&bmres27,20-6,60+20);
        }
        */
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_Font8x16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_NORMAL);
        GUI_DispStringHCenterAt((char*)&Hci_pbuf[120],LCD_GetXSize()/2+15+10,LCD_GetYSize()-(16*3)+10+16-4);

        //sprintf((char*)Hci_pbuf,"Please use iSleep App scan to bind!");
        //GUI_DispStringHCenterAt((char*)Hci_pbuf,LCD_GetXSize()/2,LCD_GetYSize()-(16*3));
        GUI_SetFont(&GUI_FontFont_ST_16);
        //显示提示语
        GUI_DrawCircle(10+10+20+30+32,10+8+10,4);
        GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_BIND[Main_Language],30+20+30+32,10+10);
        GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_BIND1[Main_Language],30+20+30+32,10+16+4+10);
        //显示操作语
        GUI_DrawCircle(10+10+20+30+32,(10+16+4+16+8)+8+10,4);
        GUI_DispStringAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],30+20+30+32,10+16+4+16+8+10);
        //
        QR_Xsl_DisplayApp(32,32-4,(uint8_t *)Hci_pbuf);
#else
        //显示二维码
        //QR_Xsl_DisplayApp(50,80,(uint8_t *)Hci_pbuf);
        //显示数值
        LCD_DispStr((g_LcdWidth-strlen((char*)Hci_pbuf)*8)/2, 20, (char*)Hci_pbuf, &sfont);
        //显示提示
        sprintf((char*)Hci_pbuf,"Please use iSleep App scan to bind!");
        sfont.FontCode=FC_ST_16;
        LCD_DispStr((g_LcdWidth-strlen((char*)Hci_pbuf)*8)/2, g_LcdHeight-(16*3), (char*)Hci_pbuf, &sfont);
#endif
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                /*
                Hci_ReShowTimer = 0;
                //生成随机码
                pi32 = (uint32_t *)&Hci_pbuf[100];
                *pi32=Count_Rand();
                pi32++;
                *pi32=Count_Rand();
                pi32++;
                *pi32=Count_Rand();
                sprintf((char*)Hci_pbuf,"(%d)Test-%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                        ++Hci_pbuf[200],\
                        Hci_pbuf[100],Hci_pbuf[101],Hci_pbuf[102],Hci_pbuf[103],Hci_pbuf[104],Hci_pbuf[105],\
                        Hci_pbuf[106],Hci_pbuf[107],Hci_pbuf[108],Hci_pbuf[109],Hci_pbuf[110],Hci_pbuf[111]);
                */
                break;
            case HCI_KEY_ESC:
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                pHci_MenuFun = pHci_DefaultMenuFun;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                sfont.BackColor     =  CL_BLACK;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}

// 销售二维码
static void Hci_Menu_InfoSalesQrCode(void)
{
    //uint32_t *pi32;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_BLACK;
    sfont.BackColor=CL_WHITE;
    sfont.Space=0;
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
#ifdef   HCI_EMWIN_ENABLE
        GUI_SetColor(HCI_MENU_INFOQRCODE_COLOR_1);
        GUI_FillRect(0,0,320-1,240-1);
        if(0)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres9;
            GUI_DrawBitmap(&bmres9,0,0);
        }
        //画小护士

        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres25;
            GUI_DrawBitmap(&bmres25,20-6,60+20);
        }
        // 底行显示
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_FontFont_ST_16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_NORMAL);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],LCD_GetXSize()/2+15+10,LCD_GetYSize()-(16*3)+10+16-4);
        //显示提示语
        GUI_SetFont(&GUI_FontFont_ST_16);
        GUI_DrawCircle(10+10+20+30+32,10+8+10,4);
        GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_SALES[Main_Language],30+20+30+32,10+10);
        GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_SALES1[Main_Language],30+20+30+32,10+16+4+10);
        GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_SALES2[Main_Language],30+20+30+32,10+(16+4)*2+10);
        //GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_BIND[Main_Language],30+20+30+32,10+10);
        //GUI_DispStringAt((char*)pSTR_UTF8_QR_CODE_BIND1[Main_Language],30+20+30+32,10+16+4+10);
        //显示操作语
        //GUI_DrawCircle(10+10+20+30+32,(10+16+4+16+8)+8+10,4);
        //GUI_DispStringAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],30+20+30+32,10+16+4+16+8+10);
        //
        //QR_Xsl_DisplayApp(32,32-4,(uint8_t *)"http://qr12.cn/DrZcvO");
        QR_Xsl_DisplayApp(32,32-4,(uint8_t *)"https://weidian.com/item.html?itemID=2006401239&wfr=wechatpo_keywords_shop");
#else
#endif
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ESC:
                //释放缓存
                pHci_MenuFun = Hci_Menu_SleepCurve;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                sfont.BackColor     =  CL_BLACK;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
            case HCI_KEY_ENTER:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoFactoryIDQrCode(void)
{
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_BLACK;
    sfont.BackColor=CL_WHITE;
    sfont.Space=0;
    //首次进入
    if(Hci_NoFirst   == 0)
    {
        Hci_NoFirst =  1;
        //申请缓存
        Hci_pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //00-49   CPUID字符串
        //50-99   MAC字符串
        //100-111 CpuId数组
    }
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        if(ModuleMemory_psPara->FactoryID[0]!=0xFF)
            //if(1)
        {
            Hci_ReShowTimer=0xFFFF;
        }
        else
        {
            Hci_ReShowTimer=5*100;
        }
        //
        if(ModuleMemory_psPara->FactoryID[0]!=0xFF)
        {
            memcpy((char*)&Hci_pbuf[10],(char*)&(ModuleMemory_psPara->FactoryID[0]),24);
            //memcpy((char*)&Hci_pbuf[10],"123456789012345678901234",24);
            Hci_pbuf[10+24]=0;
            //获取数据
            sprintf((char*)Hci_pbuf,"IMEI:%s",(char*)&Hci_pbuf[10]);
        }
        //
        GUI_SetColor(HCI_MENU_INFOQRCODE_COLOR_1);
        GUI_FillRect(0,0,320-1,240-1);
        if(0)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres9;
            GUI_DrawBitmap(&bmres9,0,0);
        }
        //画小护士
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres25;
            GUI_DrawBitmap(&bmres25,20-6,60+20);
        }
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_Font8x16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_NORMAL);
        GUI_DispStringHCenterAt((char*)Hci_pbuf,LCD_GetXSize()/2+15+10,LCD_GetYSize()-(16*3)+10+16-4);

        //显示提示语
        GUI_DrawCircle(10+10+20+30+32,10+8+10,4);
        if(ModuleMemory_psPara->FactoryID[0]!=0xFF)
            //if(1)
        {
            GUI_DispStringAt((char*)"Factory ID - OK",30+20+30+32,10+10);
        }
        else
        {
            GUI_DispStringAt((char*)"Factory ID - ERR",30+20+30+32,10+10);
        }
        //显示操作语
        GUI_SetFont(&GUI_FontFont_ST_16);
        GUI_DrawCircle(10+10+20+30+32,(10+16+4+16+8)+8+10,4);
        GUI_DispStringAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],30+20+30+32,10+16+4+16+8+10);
        //
        if(ModuleMemory_psPara->FactoryID[0]!=0xFF)
        {
            QR_Xsl_DisplayApp(32,32-4,(uint8_t *)Hci_pbuf);
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                pHci_MenuFun = Hci_Menu_II_Test;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                sfont.BackColor     =  CL_BLACK;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
static void Hci_Menu_InfoIMEIQrCode()
{
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_BLACK;
    sfont.BackColor=CL_WHITE;
    sfont.Space=0;
    //首次进入
    if(Hci_NoFirst   == 0)
    {
        Hci_NoFirst =  1;
        //申请缓存
        Hci_pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //00-49   CPUID字符串
        //50-99   MAC字符串
        //100-111 CpuId数组
    }
    //
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        if(GprsNet_s_Info.IMEI[0]>='0'&&GprsNet_s_Info.IMEI[0]<='9')
            //if(1)
        {
            Hci_ReShowTimer=0xFFFF;
        }
        else
        {
            Hci_ReShowTimer=5*100;
        }
        //
        if(GprsNet_s_Info.IMEI[0]>='0'&&GprsNet_s_Info.IMEI[0]<='9')
        {
            memcpy((char*)&Hci_pbuf[10],(char*)&(GprsNet_s_Info.IMEI),16);
            //memcpy((char*)&Hci_pbuf[10],"123456789012345678901234",24);
            Hci_pbuf[10+16]=0;
            //获取数据
            sprintf((char*)Hci_pbuf,"IMEI:%s",(char*)&Hci_pbuf[10]);
        }
        //
        GUI_SetColor(HCI_MENU_INFOQRCODE_COLOR_1);
        GUI_FillRect(0,0,320-1,240-1);
        if(0)
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres9;
            GUI_DrawBitmap(&bmres9,0,0);
        }
        //画小护士
        {
            extern GUI_CONST_STORAGE GUI_BITMAP bmres25;
            GUI_DrawBitmap(&bmres25,20-6,60+20);
        }
        GUI_SetPenSize(10);
        GUI_SetFont(&GUI_Font8x16);
        GUI_SetColor(GUI_WHITE);
        GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_NORMAL);
        GUI_DispStringHCenterAt((char*)Hci_pbuf,LCD_GetXSize()/2+15+10,LCD_GetYSize()-(16*3)+10+16-4);

        //显示提示语
        GUI_DrawCircle(10+10+20+30+32,10+8+10,4);
        if(GprsNet_s_Info.IMEI[0]>='0'&&GprsNet_s_Info.IMEI[0]<='9')
            //if(1)
        {
            GUI_DispStringAt((char*)"IMEI - OK",30+20+30+32,10+10);
        }
        else
        {
            GUI_DispStringAt((char*)"IMEI - ERR",30+20+30+32,10+10);
        }
        //显示操作语
        GUI_SetFont(&GUI_FontFont_ST_16);
        GUI_DrawCircle(10+10+20+30+32,(10+16+4+16+8)+8+10,4);
        GUI_DispStringAt((char*)pSTR_UTF8_HINT_ESC_RETURN[Main_Language],30+20+30+32,10+16+4+16+8+10);
        //
        if(GprsNet_s_Info.IMEI[0]>='0'&&GprsNet_s_Info.IMEI[0]<='9')
        {
            QR_Xsl_DisplayApp(32,32-4,(uint8_t *)Hci_pbuf);
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                pHci_MenuFun = Hci_Menu_II_Test;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                sfont.BackColor     =  CL_BLACK;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 显示RFMS信息
*******************************************************************************/
extern uint16_t uctsk_Rfms_ExRxNumBuf[6];
extern uint16_t uctsk_Xkap_ExRfmsTxNumBuf[6];
static void Hci_Menu_InfoRfms(void)
{
    uint8_t *pbuf;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        //
        pbuf=(uint8_t*)"Menu-Info RFMS";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //发送数据
        sprintf((char*)pbuf,"RfmsTx - %09d  RfmsRx - %09d",uctsk_Rfms_TxNum,uctsk_Rfms_RxNum);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Value       - %04d  PwrSaveTimer  - %d",uctsk_Rfms_RxValue,Uctsk_Rfms_PowerSavingTimer_s);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"MonitorRfmsRx-%04d  ErrResetCmt - %04d",uctsk_Rfms_MonitorRxNum,Uctsk_Rfms_RxZeroCmtInfo);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"LoopMaxErr  - %04d  LoopAllErr  - %04d",UctskRfms_BufferLoopMaxErrCmt,UctskRfms_BufferLoopAllErrCmt);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"NoBodyThreV - %04d  NoBodyThreT - %04d",uctsk_Rfms_NoBodyThresholdValue,uctsk_Rfms_NoBodyThresholdVTimerS);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);

        sprintf((char*)pbuf,"Ex Rfms 1 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[0],uctsk_Xkap_ExRfmsTxNumBuf[0]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Ex Rfms 2 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[1],uctsk_Xkap_ExRfmsTxNumBuf[1]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Ex Rfms 3 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[2],uctsk_Xkap_ExRfmsTxNumBuf[2]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Ex Rfms 4 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[3],uctsk_Xkap_ExRfmsTxNumBuf[3]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Ex Rfms 5 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[4],uctsk_Xkap_ExRfmsTxNumBuf[4]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"Ex Rfms 6 Rx-%05d Tx-%05d",uctsk_Rfms_ExRxNumBuf[5],uctsk_Xkap_ExRfmsTxNumBuf[5]);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);

        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 显示入睡信息
*******************************************************************************/
static void Hci_Menu_InfoSleepBeginTime(void)
{
    uint8_t i;
    uint8_t *pbuf;
    MODULE_MEMORY_S_SLEEP_BEGIN_TIME *psSleepBeginTime;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //
        pbuf=(uint8_t*)"Menu-Info Sleep Begin Time";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psSleepBeginTime = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //读取历史数据
        Module_Memory_App(MODULE_MEMORY_APP_CMD_SLEEP_BEGIN_TIME_R,(uint8_t*)psSleepBeginTime,NULL);
        //发送数据
        for(i=0; i<27; i+=3)
        {
            sprintf((char*)pbuf,"%05d-%02d:%02d %05d-%02d:%02d %05d-%02d:%02d",\
                    psSleepBeginTime->Minute[i]  ,psSleepBeginTime->Minute[i]/60  ,psSleepBeginTime->Minute[i]%60,\
                    psSleepBeginTime->Minute[i+1],psSleepBeginTime->Minute[i+1]/60,psSleepBeginTime->Minute[i+1]%60,\
                    psSleepBeginTime->Minute[i+2],psSleepBeginTime->Minute[i+2]/60,psSleepBeginTime->Minute[i+2]%60);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        sprintf((char*)pbuf,"%05d-%02d:%02d",\
                psSleepBeginTime->Minute[i]  ,psSleepBeginTime->Minute[i]/60  ,psSleepBeginTime->Minute[i]%60);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"NextNum(0-27):%d",psSleepBeginTime->NextHhMmNum);
        LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psSleepBeginTime);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 时间与闹钟设置页面
* 说    明: 参考isleep界面
* 注    释：现在汉字字体为24，楷体
*******************************************************************************/
#define  HCI_MENU_SET_UP_SPACE   8
#define  HCI_MENU_SET_INTERVAL   (240-2*HCI_MENU_SET_UP_SPACE)/6
static void Hci_Menu_SetMenu(void)
{
    uint8_t *pbuf;
    struct tm *ptm;
    MODULE_MEMORY_S_PARA *pspara;
    uint8_t res,i;
    uint16_t alarm_value;
    //ww
    uint8_t auto_on_off;
    uint16_t auto_begin_value;
    uint16_t auto_end_value;
    uint8_t warn_on_off;
    uint16_t warn_value;
    //ww
    FONT_T sfont;
    sfont.FontCode=FC_ST_24;
    sfont.FrontColor=CL_WHITE;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    //首次进入
    if(Hci_NoFirst   == 0)
    {
        Hci_NoFirst =  1;
        //申请缓存
        Hci_pbuf  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        Hci_cursor=  0;
        i = 0;
        //读取闹钟信息
        BspRtc_ReadAlarm(1,&alarm_value,&Hci_pbuf[0]);
        Hci_pbuf[1]=alarm_value>>8;
        Hci_pbuf[2]=alarm_value;
        BspRtc_ReadAlarm(2,&alarm_value,&Hci_pbuf[3]);
        Hci_pbuf[4]=alarm_value>>8;
        Hci_pbuf[5]=alarm_value;
        //读取时钟信息
        ptm = (struct tm *)&Hci_pbuf[128];
        BspRtc_ReadRealTime(NULL,ptm,NULL,NULL);
        Hci_pbuf[6]=ptm->tm_year-2000;
        Hci_pbuf[7]=ptm->tm_mon;
        Hci_pbuf[8]=ptm->tm_mday;
        Hci_pbuf[9]=ptm->tm_hour;
        Hci_pbuf[10]=ptm->tm_min;
        //读取定时测量信息
        BspRtc_ReadAuto(&auto_on_off,&auto_begin_value,&auto_end_value);
        Hci_pbuf[11]=auto_on_off;
        Hci_pbuf[12]=auto_begin_value>>8;
        Hci_pbuf[13]=auto_begin_value;
        Hci_pbuf[14]=auto_end_value>>8;
        Hci_pbuf[15]=auto_end_value;
        //读取入睡提醒信息
        BspRtc_ReadWarn(1,&warn_on_off,&warn_value);
        Hci_pbuf[16]=warn_on_off;
        Hci_pbuf[17]=warn_value>>8;
        Hci_pbuf[18]=warn_value;
        i=19;
        //读取入睡背光信息
        pspara = (MODULE_MEMORY_S_PARA*)&Hci_pbuf[128];
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
        if(pspara->LcdBackLight2<HCI_SLEEP_LIGHT_LEVEL1)
        {
            Hci_pbuf[i++]=0;
        }
        else if(pspara->LcdBackLight2<HCI_SLEEP_LIGHT_LEVEL2)
        {
            Hci_pbuf[i++]=1;
        }
        else
        {
            Hci_pbuf[i++]=2;
        }
        //
        Hci_pbuf[20]=1;
        //
    }
    //定时刷新判断
    //刷新屏幕
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        if(Hci_pbuf[20]==1)
        {
            Hci_pbuf[20]=0;
            //标题显示
            sfont.FontCode=FC_ST_24;
            sfont.FrontColor=CL_CYAN;
            i   =  0;
            //---"闹钟1:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_ALARM_BUF[0][Main_Language], &sfont);
            //---"闹钟2:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_ALARM_BUF[1][Main_Language], &sfont);
            //---"时钟:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_CLOCK_BUF[Main_Language], &sfont);
            //---"定时测量:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_AUTO_MEASURE_BUF[Main_Language], &sfont);
            //---"入睡提醒:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_SLEEP_WARN_BUF[Main_Language], &sfont);
            //---"入睡背光:"
            LCD_DispStr(20, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)pSTR_SLEEP_BACKLIGHT_BUF[Main_Language], &sfont);
        }
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //格式化参数
        //---闹钟1
        sprintf((char*)&pbuf[0],"%02s  %02d:%02d",(char*)pSTR_ALARM_MODE_BUF[Hci_pbuf[0]][Main_Language],Hci_pbuf[1],Hci_pbuf[2]);
        //---闹钟2
        sprintf((char*)&pbuf[20],"%02s  %02d:%02d",(char*)pSTR_ALARM_MODE_BUF[Hci_pbuf[3]][Main_Language],Hci_pbuf[4],Hci_pbuf[5]);
        //---时钟
        sprintf((char*)&pbuf[40],"%02d-%02d-%02d %02d:%02d",Hci_pbuf[6],Hci_pbuf[7]+1,Hci_pbuf[8],Hci_pbuf[9],Hci_pbuf[10]);
        //---定时测量
        sprintf((char*)&pbuf[60],"%02s %02d:%02d-%02d:%02d",(char*)pSTR_ONOFF_BUF[Hci_pbuf[11]][Main_Language],Hci_pbuf[12],Hci_pbuf[13],Hci_pbuf[14],Hci_pbuf[15]);
        //---入睡提醒
        sprintf((char*)&pbuf[80],"%02s %02d:%02d",(char*)pSTR_ONOFF_BUF[Hci_pbuf[16]][Main_Language],Hci_pbuf[17],Hci_pbuf[18]);
        //---背光
        sprintf((char*)&pbuf[100],"%s",(char*)pSTR_SLEEPLIGHT_BUF[Hci_pbuf[19]][Main_Language]);
        //数值显示
        sfont.FontCode=FC_ST_24;
        sfont.FrontColor=CL_WHITE;
        i   =  0;
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[0], &sfont);
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[20], &sfont);
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[40], &sfont);
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[60], &sfont);
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[80], &sfont);
        LCD_DispStr(134, HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*(i++), (char*)&pbuf[100], &sfont);
        //显示光标
        Hci_CursorFlick ^= 0x01;
        //for(res=0; res<=19; res++)
        {
            res = Hci_cursor;
            if(res==Hci_cursor)
            {
                sfont.BackColor    =  CL_WHITE;
                sfont.FrontColor   =  CL_BLACK;
            }
            else
            {
                sfont.BackColor    =  CL_BLACK;
                sfont.FrontColor   =  CL_WHITE;
            }
            switch(res)
            {
                case 0:
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*0   , (char*)pSTR_ALARM_MODE_BUF[Hci_pbuf[res]][Main_Language], &sfont);
                    break;
                case 1:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(194+12 ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*0   , (char*)pbuf, &sfont);
                    break;
                case 2:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(230+12 ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*0   , (char*)pbuf, &sfont);
                    break;
                case 3:
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*1   , (char*)pSTR_ALARM_MODE_BUF[Hci_pbuf[res]][Main_Language], &sfont);
                    break;
                case 4:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(194+12 ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*1   , (char*)pbuf, &sfont);
                    break;
                case 5:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(230+12 ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*1   , (char*)pbuf, &sfont);
                    break;
                case 6:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*2   , (char*)pbuf, &sfont);
                    break;
                case 7:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]+1);
                    LCD_DispStr(170    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*2   , (char*)pbuf, &sfont);
                    break;
                case 8:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(206    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*2   , (char*)pbuf, &sfont);
                    break;
                case 9:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(242    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*2   , (char*)pbuf, &sfont);
                    break;
                case 10:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(278    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*2   , (char*)pbuf, &sfont);
                    break;
                case 11:
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*3   , (char*)pSTR_ONOFF_BUF[Hci_pbuf[res]][Main_Language], &sfont);
                    break;
                case 12:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(170    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*3   , (char*)pbuf, &sfont);
                    break;
                case 13:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(206    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*3   , (char*)pbuf, &sfont);
                    break;
                case 14:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(242    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*3   , (char*)pbuf, &sfont);
                    break;
                case 15:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(278    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*3   , (char*)pbuf, &sfont);
                    break;
                case 16:
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*4   , (char*)pSTR_ONOFF_BUF[Hci_pbuf[res]][Main_Language], &sfont);
                    break;
                case 17:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(170    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*4   , (char*)pbuf, &sfont);
                    break;
                case 18:
                    sprintf((char*)pbuf,"%02d",Hci_pbuf[res]);
                    LCD_DispStr(206    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*4   , (char*)pbuf, &sfont);
                    break;
                case 19:
                    sprintf((char*)pbuf,"%s",(char*)pSTR_SLEEPLIGHT_BUF[Hci_pbuf[res]][Main_Language]);
                    LCD_DispStr(134    ,  HCI_MENU_SET_UP_SPACE+HCI_MENU_SET_INTERVAL*5   , (char*)pbuf, &sfont);
                    break;
                default:
                    break;
            }
        }
        sfont.BackColor    =  CL_BLACK;
        sfont.FrontColor   =  CL_WHITE;
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    //大于30秒则返回默认页面
    if(Hci_ReturnTimer>=30*100)
    {
        BspKey_NewSign =  1;
        BspKey_Value   =  HCI_KEY_ESC;
    }
    else
    {
        Hci_ReturnTimer++;
    }
    //按键解析
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign     =  0;
        Hci_ReShowTimer    =  0;
        Hci_ReturnTimer    =  0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                Hci_CursorFlick = 0;
                Hci_cursor++;
                if(Hci_cursor>19)
                    Hci_cursor=0;
                break;
            case HCI_KEY_ESC:
                //验证数据设置的有效性
                if(1==Hci_pbuf[11])
                {
                    auto_begin_value =  Hci_pbuf[12]*60+Hci_pbuf[13];
                    auto_end_value   =  Hci_pbuf[14]*60+Hci_pbuf[15];
                    if(auto_end_value<auto_begin_value)
                    {
                        auto_end_value+=24*60;
                    }
                    if((auto_end_value-auto_begin_value)>12*60 || (auto_end_value-auto_begin_value)<1*60)
                    {
                        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                        sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_RESET_AUTOWORK[Main_Language],\
                                pSTR_UTF8_HINT_ESC_RETURN_RESET[Main_Language]);
                        Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
                        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                        Hci_pbuf[20]=1;
                        break;
                    }
                }
                if((Hci_ParaChangeSign&(1<<7))!=0x00&&(Hci_pbuf[19]==3||Hci_pbuf[19]==4))
                {
                    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    sprintf((char*)pbuf,"%s\n%s",(char*)pSTR_UTF8_HINT_SLEEP_BACKLIGHT_TOO_BRIGHT[Main_Language],\
                            pSTR_UTF8_HINT_ESC_RETURN_RESET[Main_Language]);
                    Hci_Menu_HintAsk(10,1,(char*)pSTR_UTF8_HINT[Main_Language],(char*)pbuf);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
                    Hci_pbuf[20]=1;
                }
                //更新闹钟1
                if((Hci_ParaChangeSign&(1<<0))!=0x00)
                {
                    alarm_value = Count_2ByteToWord(Hci_pbuf[1],Hci_pbuf[2]);
                    BspRtc_SetAlarm(1,&alarm_value,&Hci_pbuf[0]);
                }
                //更新闹钟2
                if((Hci_ParaChangeSign&(1<<1))!=0x00)
                {
                    alarm_value = Count_2ByteToWord(Hci_pbuf[4],Hci_pbuf[5]);
                    BspRtc_SetAlarm(2,&alarm_value,&Hci_pbuf[3]);
                }
                //更新日期时间
                if((Hci_ParaChangeSign&(1<<2))!=0x00)
                {
                    ptm=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    ptm->tm_year=2000+Hci_pbuf[6];;
                    ptm->tm_mon =Hci_pbuf[7];
                    ptm->tm_mday=Hci_pbuf[8];
                    ptm->tm_hour=Hci_pbuf[9];
                    ptm->tm_min =Hci_pbuf[10];
                    BspRtc_SetRealTime(NULL,ptm,NULL,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptm);
                }
                //更新定时测量时间
                if((Hci_ParaChangeSign&(1<<3))!=0x00)
                {
                    auto_on_off = Hci_pbuf[11];
                    auto_begin_value = Count_2ByteToWord(Hci_pbuf[12],Hci_pbuf[13]);
                    auto_end_value = Count_2ByteToWord(Hci_pbuf[14],Hci_pbuf[15]);
                    BspRtc_SetAuto(auto_on_off,auto_begin_value,auto_end_value);
                }
                //更新入睡提醒时间
                if((Hci_ParaChangeSign&(1<<4))!=0x00)
                {
                    warn_on_off = Hci_pbuf[16];
                    warn_value = Count_2ByteToWord(Hci_pbuf[17],Hci_pbuf[18]);
                    BspRtc_SetWarn(1,warn_on_off,warn_value);
                }
                //更新背光
                if((Hci_ParaChangeSign&(1<<7))!=0x00)
                {
                    pspara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
                    //pspara->LcdBackLight2 =  Hci_pbuf[25];
                    if(Hci_pbuf[19]==0)
                    {
                        pspara->LcdBackLight2=HCI_SLEEP_LIGHT_LEVEL0;
                    }
                    else if(Hci_pbuf[19]==1)
                    {
                        pspara->LcdBackLight2=HCI_SLEEP_LIGHT_LEVEL1;
                    }
                    else
                    {
                        pspara->LcdBackLight2=HCI_SLEEP_LIGHT_LEVEL2;
                    }
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pspara,NULL);
                    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
                }
                //
                if(Hci_ParaChangeSign!=0x00)
                {
                    GprsAppXkap_Sign_ParaUpload=0;
                }
                //
                Hci_cursor=0;
                pHci_MenuFun = pHci_DefaultMenuFun;
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_ReturnTimer     =  0;
                Hci_NoFirst         =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                switch(Hci_cursor)
                {
                    case 0:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 2;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 1:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 2:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 3:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 2;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 4:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 5:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 6:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 99;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 7:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 11;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 8:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==1 || Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 31;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 9:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 10:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 11:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 1;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 12:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 13:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 14:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 15:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 16:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 1;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 17:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 23;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 18:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==0)
                            Hci_pbuf[Hci_cursor] = 59;
                        else
                            Hci_pbuf[Hci_cursor]--;
                        break;
                    case 19:
                        Hci_ParaChangeSign |=  (1<<7);
                        if(Hci_pbuf[Hci_cursor]==0)
                        {
                            Hci_pbuf[Hci_cursor]=2;
                        }
                        else
                        {
                            Hci_pbuf[Hci_cursor]--;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case HCI_KEY_RIGHT:
                switch(Hci_cursor)
                {
                    case 0:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==2)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 1:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 2:
                        Hci_ParaChangeSign |=  (1<<0);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 3:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==2)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 4:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 5:
                        Hci_ParaChangeSign |=  (1<<1);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 6:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==99)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 7:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==11)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 8:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==31)
                            Hci_pbuf[Hci_cursor] = 1;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 9:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 10:
                        Hci_ParaChangeSign |=  (1<<2);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 11:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==1)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 12:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 13:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 14:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 15:
                        Hci_ParaChangeSign |=  (1<<3);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 16:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==1)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 17:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==23)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 18:
                        Hci_ParaChangeSign |=  (1<<4);
                        if(Hci_pbuf[Hci_cursor]==59)
                            Hci_pbuf[Hci_cursor] = 0;
                        else
                            Hci_pbuf[Hci_cursor]++;
                        break;
                    case 19:
                        Hci_ParaChangeSign |=  (1<<7);
                        if(Hci_pbuf[Hci_cursor]>=2)
                        {
                            Hci_pbuf[Hci_cursor]=0;
                        }
                        else
                        {
                            Hci_pbuf[Hci_cursor]++;
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 第1条睡眠曲线记录页面
* 说    明: 参考isleep界面ww
*******************************************************************************/
extern void Arithmetic_AI(unsigned char *s,unsigned char length);
static void Hci_Menu_SleepCurve(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    MODULE_MEMORY_S_DAY_SLEEPLEVEL *psDaySleepLevel;
    uint8_t sleeptime;
    uint8_t res;
    uint8_t Sleep_cnt;
    uint16_t i16;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
#if (HARDWARE_VER==8)
    if(Hci_DayMoveCnt==0)
    {
        Sleep_cnt = MODULE_MEMORY_ADDR_DAY_NUM;
    }
    else
    {
        Sleep_cnt = Hci_DayMoveCnt;
    }
#else
    Sleep_cnt = (curve_cnt+1)/2;
#endif
    //定时刷新判断
    //刷新屏幕
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //申请缓存
        pbuf            =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pspara          =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psDayInfo       =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psDaySleepLevel =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //标题
        res=sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR1[Main_Language],Sleep_cnt);
        //黄色显示"睡眠分级曲线"
        LCD_DispStr((g_LcdWidth-res*8)/2, 8, (char*)pbuf, &sfont);
        //白色重画
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR2[Main_Language],Sleep_cnt);
        sfont.FrontColor=CL_WHITE;
        LCD_DispStr((g_LcdWidth-res*8)/2, 8, (char*)pbuf, &sfont);
        //"评分   分"
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_CYAN;
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_GRADE[Main_Language]);
        LCD_DispStr(5, 78, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_SCORE[Main_Language]);
        LCD_DispStr(79, 78, (char*)pbuf, &sfont);
        //各种睡眠字体
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_CYAN;
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_TOTAL[Main_Language]);
        LCD_DispStr(105, 58, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_BODYMOVE[Main_Language]);
        LCD_DispStr(201, 58, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_WAKE[Main_Language]);
        LCD_DispStr(105, 78, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_LIGHTSLEEP[Main_Language]);
        LCD_DispStr(201, 78, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_MSLEEP[Main_Language]);
        LCD_DispStr(105, 98, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_DEEPSLEEP[Main_Language]);
        LCD_DispStr(201, 98, (char*)pbuf, &sfont);
        sfont.FrontColor=CL_WHITE;
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_NUM[Main_Language]);
        LCD_DispStr(265, 58, (char*)pbuf, &sfont);
        //睡眠曲线字体
        sfont.FrontColor=CL_CYAN;
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_WAKE1[Main_Language]);
        LCD_DispStr(10, 128-4, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_LIGHTSLEEP1[Main_Language]);
        LCD_DispStr(10, 151-4, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_MSLEEP1[Main_Language]);
        LCD_DispStr(10, 174-4, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,pSTR_SLEEP_CURVE_DEEPSLEEP1[Main_Language]);
        LCD_DispStr(10, 197-4, (char*)pbuf, &sfont);
        //虚线
        sfont.FontCode=FC_ST_12;
        sfont.FrontColor=CL_GREY5;
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - -");
        LCD_DispStr(58-2, 128, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - -");
        LCD_DispStr(58-2, 151, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - -");
        LCD_DispStr(58-2, 174, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - -");
        LCD_DispStr(58-2, 197, (char*)pbuf, &sfont);
        LCD_DrawLineH(58-2,220-8,303,CL_GREY1);
        //读取数据索引
#if (HARDWARE_VER==8)
        // 按日期索引
        for(res=0; res<MODULE_MEMORY_ADDR_DAY_NUM; res++)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
            if(Hci_DayMoveDate[0]==(Count_2ByteToWord(psDayInfo->BeginTime[0],psDayInfo->BeginTime[1])-2000)\
               &&Hci_DayMoveDate[1]==psDayInfo->BeginTime[2]\
               &&Hci_DayMoveDate[2]==psDayInfo->BeginTime[3])
            {
                break;
            }
        }
        Hci_DayMoveDate[0]=Hci_DayMoveDate[1]=Hci_DayMoveDate[2]=0;
#else
        // 按存储顺序索引
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
        res = pspara->MemoryDayNextP;
        res = Count_SubCyc(res,Sleep_cnt,MODULE_MEMORY_ADDR_DAY_NUM-1);
#endif
        //
        if(res>=MODULE_MEMORY_ADDR_DAY_NUM)
        {
            memset((char*)psDayInfo,0xFF,sizeof(MODULE_MEMORY_S_DAY_INFO));
            memset((char*)psDaySleepLevel,0xFF,sizeof(MODULE_MEMORY_S_DAY_SLEEPLEVEL));
        }
        else if(Hci_FlashDataSign==0)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_SLEEPLEVEL_R,(uint8_t*)psDaySleepLevel,&res);
        }
        else if(Hci_FlashDataSign==1)
        {
            if(uctsk_Rfms_ExtFlashReadSign)
            {
                memcpy((char*)psDayInfo,(char*)&uctsk_Rfms_s_DayInfo,sizeof(MODULE_MEMORY_S_DAY_INFO));
                memcpy((char*)psDaySleepLevel,(char*)&uctsk_Rfms_s_DaySleepLevel,sizeof(MODULE_MEMORY_S_DAY_SLEEPLEVEL));
            }
            else
            {
                uctsk_Rfms_RunFileData_DebugTestOnOff(ON);
            }
        }
        else
        {
            ;
        }
        //
        //Arithmetic_AI(psDaySleepLevel->DataBuf,psDayInfo->BodyMoveNum);
        //---绘制睡眠曲线
        //绘制睡眠曲线
        if(Hci_FlashDataSign==1&&uctsk_Rfms_ExtFlashReadSign==0)
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=CL_RED;
            sprintf((char*)pbuf,"Loading Data,Waiting...");
            LCD_DispStr(87, 137, (char*)pbuf, &sfont);
            Hci_ParaChangeSign = 1;
            Hci_ReShowTimer    = 100;
            goto HciMenuSleepCurve_Goto1;
        }
        else if(Hci_SleepDataValid(psDaySleepLevel->DataBuf, sizeof(psDaySleepLevel->DataBuf)) == ERR)
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=RGB(255,  100,  0);
            sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR3[Main_Language]);
            LCD_DispStr(87, 137, (char*)pbuf, &sfont);
            goto HciMenuSleepCurve_Goto1;
        }
        //画曲线
        {
            sfont.FontCode=FC_ST_16;
            if(Hci_ParaChangeSign==1)
            {
                sfont.FrontColor=sfont.BackColor;
                memset((char*)pbuf,' ',25);
                pbuf[25]=0;
                LCD_DispStr(87, 137, (char*)pbuf, &sfont);
                Hci_ParaChangeSign=0;
            }

            //ww
            /*
            if(psDayInfo->EndTime[4]-psDayInfo->BeginTime[4]>0)
                sleeptime = psDayInfo->EndTime[4]-psDayInfo->BeginTime[4];
            else
                sleeptime = 24-psDayInfo->BeginTime[4]+psDayInfo->EndTime[4];
            */
            /*
            sleeptime   =  (psDayInfo->BodyMoveNum*3)/60;
            if((psDayInfo->BodyMoveNum*3)%60)
            {
               sleeptime++;
            }
            */
            sleeptime=(psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)/60;
            if((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)%60)
            {
                sleeptime++;
            }
            if(sleeptime > 6)
            {
                for(i16=0; i16<240; i16++) //横轴范围为0-239,共240个像素点
                {
                    if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                        LCD_DrawLineV(58+i16+psDayInfo->BeginTime[5]*20/60,134-2,136-2,CL_RED);    //觉醒，红色，像素点高度为3
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                        LCD_DrawLineV(58+i16+psDayInfo->BeginTime[5]*20/60,157-2,159-2,CL_YELLOW); //浅睡，黄色，像素点高度为3
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                        LCD_DrawLineV(58+i16+psDayInfo->BeginTime[5]*20/60,180-2,182-2,CL_GREEN);   //中睡，蓝色，像素点高度为3
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                        LCD_DrawLineV(58+i16+psDayInfo->BeginTime[5]*20/60,203-2,205-2,CL_BLUE);  //深睡，绿色，像素点高度为3
                    //绘制过渡虚线
                    if(psDaySleepLevel->DataBuf[i16]>psDaySleepLevel->DataBuf[i16+1])
                    {
                        if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,157-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,160-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,183-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,160-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,203-2,CL_GREY5);
                    }
                    else if(psDaySleepLevel->DataBuf[i16]<psDaySleepLevel->DataBuf[i16+1])
                    {
                        if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,183-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,160-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,160-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,157-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                            LCD_DrawVDotLine(58+i16+psDayInfo->BeginTime[5]*20/60,137-2,203-2,CL_GREY5);
                    }
                }
            }
            else
            {
                for(i16=0; i16<120; i16++) //横轴范围为0-239,共240个像素点
                {
                    if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                    {
                        LCD_DrawLineV(58+i16*2+psDayInfo->BeginTime[5]*40/60  ,134-2,136-2,CL_RED);    //觉醒，红色，像素点高度为3
                        LCD_DrawLineV(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,134-2,136-2,CL_RED);
                    }
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                    {
                        LCD_DrawLineV(58+i16*2+psDayInfo->BeginTime[5]*40/60  ,157-2,159-2,CL_YELLOW); //浅睡，黄色，像素点高度为3
                        LCD_DrawLineV(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,157-2,159-2,CL_YELLOW);
                    }
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                    {
                        LCD_DrawLineV(58+i16*2+psDayInfo->BeginTime[5]*40/60  ,180-2,182-2,CL_GREEN);   //中睡，蓝色，像素点高度为3
                        LCD_DrawLineV(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,180-2,182-2,CL_GREEN);
                    }
                    else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                    {
                        LCD_DrawLineV(58+i16*2+psDayInfo->BeginTime[5]*40/60  ,203-2,205-2,CL_BLUE);  //深睡，绿色，像素点高度为3
                        LCD_DrawLineV(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,203-2,205-2,CL_BLUE);
                    }
                    //绘制过渡虚线
                    if(psDaySleepLevel->DataBuf[i16]>psDaySleepLevel->DataBuf[i16+1])
                    {
                        if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,157-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,160-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,183-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,160-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,203-2,CL_GREY5);
                    }
                    else if(psDaySleepLevel->DataBuf[i16]<psDaySleepLevel->DataBuf[i16+1])
                    {
                        if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,183-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,160-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,160-2,203-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_LIGHT)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,157-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_MIDDLE)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,180-2,CL_GREY5);
                        else if(psDaySleepLevel->DataBuf[i16] == MODULE_MEMORY_E_SLEEPLEVEL_WAKE && psDaySleepLevel->DataBuf[i16+1] == MODULE_MEMORY_E_SLEEPLEVEL_DEEP)
                            LCD_DrawVDotLine(58+i16*2+1+psDayInfo->BeginTime[5]*40/60,137-2,203-2,CL_GREY5);
                    }
                }
            }
            if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())
            {
                struct tm *ptm1,*ptm2;
                int32_t i32;
                //显示数据
                //---时间
                sfont.FontCode=FC_ST_16;
                sfont.FrontColor=CL_WHITE;
                sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR4[Main_Language],\
                        psDayInfo->BeginTime[2],psDayInfo->BeginTime[3],psDayInfo->BeginTime[4],psDayInfo->BeginTime[5],
                        psDayInfo->EndTime[2],psDayInfo->EndTime[3],psDayInfo->EndTime[4],psDayInfo->EndTime[5]);
                LCD_DispStr(5, 32, (char*)pbuf, &sfont);
                //---分数
                sfont.FontCode=FC_ST_32;
                sprintf((char*)pbuf,"%02d",psDayInfo->Scoring);
                LCD_DispStr(42, 68, (char*)pbuf, &sfont);
                //---各种睡眠时间，取实际时间
                sfont.FontCode=FC_ST_16;
                //i16=Count_2ByteToWord(psDayInfo->SleepTime_m[0],psDayInfo->SleepTime_m[1]);
                // 总睡时长
                /*
                i16 = (psDayInfo->BodyMoveNum)*3;
                sprintf((char*)pbuf,"%02d:%02d",i16/60,i16%60);
                LCD_DispStr(145, 58, (char*)pbuf, &sfont);
                */

                ptm1 = (struct tm *)&pbuf[0];
                ptm2 = (struct tm *)&pbuf[128];
                ptm1->tm_year = Count_2ByteToWord(psDayInfo->BeginTime[0],psDayInfo->BeginTime[1]);
                ptm1->tm_mon  = psDayInfo->BeginTime[2]-1;
                ptm1->tm_mday = psDayInfo->BeginTime[3];
                ptm1->tm_hour = psDayInfo->BeginTime[4];
                ptm1->tm_min  = psDayInfo->BeginTime[5];
                ptm1->tm_sec  = 0;
                ptm2->tm_year = Count_2ByteToWord(psDayInfo->EndTime[0],psDayInfo->EndTime[1]);
                ptm2->tm_mon  = psDayInfo->EndTime[2]-1;
                ptm2->tm_mday = psDayInfo->EndTime[3];
                ptm2->tm_hour = psDayInfo->EndTime[4];
                ptm2->tm_min  = psDayInfo->EndTime[5];
                ptm2->tm_sec  = 0;
                i32  =  Count_TimeCompare(ptm2,ptm1);
                sprintf((char*)pbuf,"%02d:%02d",(i32/3600)%100,(i32%3600)/60);
                LCD_DispStr(145, 58, (char*)pbuf, &sfont);
                // 体动
                sprintf((char*)pbuf,"%02d",Count_2ByteToWord(psDayInfo->BodyMoveCount[0],psDayInfo->BodyMoveCount[1]));
                LCD_DispStr(241, 58, (char*)pbuf, &sfont);
                // 觉醒
                i16=Count_2ByteToWord(psDayInfo->WakeLevelTime_m[0],psDayInfo->WakeLevelTime_m[1]);
                sprintf((char*)pbuf,"%02d:%02d",i16/60,i16%60);
                LCD_DispStr(145, 78, (char*)pbuf, &sfont);
                // 浅睡
                i16=Count_2ByteToWord(psDayInfo->LightLevelTime_m[0],psDayInfo->LightLevelTime_m[1]);
                sprintf((char*)pbuf,"%02d:%02d",i16/60,i16%60);
                LCD_DispStr(241, 78, (char*)pbuf, &sfont);
                // 中睡
                i16=Count_2ByteToWord(psDayInfo->MiddleLevelTime_m[0],psDayInfo->MiddleLevelTime_m[1]);
                sprintf((char*)pbuf,"%02d:%02d",i16/60,i16%60);
                LCD_DispStr(145, 98, (char*)pbuf, &sfont);
                // 深睡
                i16=Count_2ByteToWord(psDayInfo->DeepLevelTime_m[0],psDayInfo->DeepLevelTime_m[1]);
                sprintf((char*)pbuf,"%02d:%02d",i16/60,i16%60);
                LCD_DispStr(241, 98, (char*)pbuf, &sfont);
                //
                i16 = psDayInfo->BeginTime[4] + ((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)/60);
                if((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)%60)
                {
                    i16++;
                }
                i16 = i16%24;
                //由测量累计时间计算
                Hci_Draw_SleepCoordinate(58,222-8,psDayInfo->BeginTime[4],i16);
            }
        }
        //测试无效条件判定
        if(psDayInfo->Flag_DataValid==1)
        {
            sfont.FontCode=FC_ST_12;
            sfont.FrontColor=RGB(255,  100,  0);
            sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR5[Main_Language]);
            LCD_DispStr(160, 139, (char*)pbuf, &sfont);
            sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR6[Main_Language]);
            LCD_DispStr(160, 139+22, (char*)pbuf, &sfont);
        }
        //显示评分明细
        if(Hci_CursorFlick==1)
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=CL_WHITE;
            sprintf((char*)pbuf,"Mark:%02d %02d %02d %02d %02d %02d %02d %02d"\
                    ,psDayInfo->Mark_SleepStruct,psDayInfo->Mart_SleepIntoPeriod\
                    ,psDayInfo->Mark_BodyMovement,psDayInfo->Mark_WakeState\
                    ,psDayInfo->Mark_BeginSleepTimeChange,psDayInfo->Mark_Temp\
                    ,psDayInfo->Mark_Humi,psDayInfo->Mart_TotalSleep);
            LCD_DispStr(70, 116, (char*)pbuf, &sfont);
        }
        //标记上传状态
        if(psDayInfo->UpdataToServerCmt==0)
        {
            //未上传
            sprintf((char*)pbuf,pSTR_NO_SEND[Main_Language]);
            sfont.FrontColor   =  CL_RED;
        }
        else
        {
            //已上传
            sprintf((char*)pbuf,pSTR_SENDED[Main_Language]);
            sfont.FrontColor   =  CL_GREEN;
        }
        sfont.FontCode     =  FC_ST_16;
        LCD_DispStr(320-4*16, 8, (char*)pbuf, &sfont);
    HciMenuSleepCurve_Goto1:
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDaySleepLevel);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(/*(res==UCTSK_RFMS_E_RUNSTATE_IDLE)\*/
        (BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]>3*100)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]>3*100)\
        &&(Hci_Power ==  BSP_PWR_E_POWER_AC))
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]=0xFFFF;
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_SOS;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>8*100)
    {
        // 半初始化
        res = MODULE_MEMORY_CMD_FACTORY_MODE_HALF;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_FACTORY,NULL,&res);
        MCU_SOFT_RESET;
        // 返回主页面
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_Default1;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
    }
    //大于30秒则返回默认页面
    if(Hci_ReturnTimer>=60*100)
    {
        BspKey_NewSign =  1;
        BspKey_Value   =  HCI_KEY_ESC;
    }
    else
    {
        Hci_ReturnTimer++;
    }
    //
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign  =  0;
        Hci_ReturnTimer =  0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
#ifdef HCI_EMWIN_ENABLE
                Hci_Menu_SleepMark();
                GUI_SetBkColor(GUI_BLACK);
                GUI_Clear();
                Hci_ReShowTimer     =  0;
                BspKey_NewSign      =  0;
#endif
                break;
            case HCI_KEY_ESC:
                if(Hci_FlashDataSign==0)
                {
                    pHci_MenuFun = pHci_DefaultMenuFun;
                }
                else if(Hci_FlashDataSign==1)
                {
                    Hci_FlashDataSign = 0;
                    pHci_MenuFun = Hci_Menu_II_Info;
                }
                curve_cnt = 1;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_FlashDataSign==0)
                {
                    curve_cnt--;
                    if(curve_cnt == 0)
                        curve_cnt = MODULE_MEMORY_ADDR_DAY_NUM*2;
#if (HARDWARE_VER==8)
                    pHci_MenuFun = Hci_Menu_DayMoveCurve;
                    if(Hci_DayMoveCnt==0)
                    {
                        Hci_DayMoveCnt=MODULE_MEMORY_EXTFLASH_MOVE_NUM-1;
                    }
                    else
                    {
                        Hci_DayMoveCnt--;
                    }
#else
                    pHci_MenuFun = Hci_Menu_BreathHeartCurve;
#endif
                    Hci_ReShowTimer = 0;
                    Hci_CursorFlick = 0;
                    LCD_ClrScr(sfont.BackColor);
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_FlashDataSign==0)
                {
                    curve_cnt++;
                    if(curve_cnt>(MODULE_MEMORY_ADDR_DAY_NUM*2))
                        curve_cnt = 1;

#if (HARDWARE_VER==8)
                    pHci_MenuFun = Hci_Menu_DayMoveCurve;
#else
                    pHci_MenuFun = Hci_Menu_BreathHeartCurve;
#endif
                    Hci_ReShowTimer = 0;
                    Hci_CursorFlick = 0;
                    LCD_ClrScr(sfont.BackColor);
                }
                break;
            case HCI_KEY_TOP:
                /*
                if(BspKey_KeepTimer_ms>3000)
                {
                    Hci_CursorFlick=1;
                    Hci_ReShowTimer=0;
                }
                */
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 绘制呼吸心率曲线
* 参    数: mode  0->正式应用数据 1->ExtFlash数据
* 说    明: 参考isleep界面ww
*******************************************************************************/
static void Hci_Menu_BreathHeartCurve(void)
{
    uint8_t *pbuf;
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    MODULE_MEMORY_S_DAY_BREATH *psDayBreath;
    MODULE_MEMORY_S_DAY_HEARTRATE *psDayHeartRate;
    MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove;
    uint8_t res;
    uint16_t i16;
    uint8_t BreathHeart_cnt;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    BreathHeart_cnt = (curve_cnt+1)/2;
    //定时刷新判断
    //刷新屏幕
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=5*100;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //标题
        res=sprintf((char*)pbuf,pSTR_BHCURVE_STR1[Main_Language],BreathHeart_cnt);
        LCD_DispStr((g_LcdWidth-res*8)/2, 8, (char*)pbuf, &sfont);
        //重画颜色
        sprintf((char*)pbuf,pSTR_BHCURVE_STR2[Main_Language],BreathHeart_cnt);
        sfont.FrontColor=CL_WHITE;
        LCD_DispStr((g_LcdWidth-res*8)/2, 8, (char*)pbuf, &sfont);
        //"呼 吸"，"(次/分)"
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_CYAN;
        sprintf((char*)pbuf,pSTR_BHCURVE_STR3[Main_Language]);
        LCD_DispStr(5, 75, (char*)pbuf, &sfont);
        sfont.FrontColor=CL_WHITE;
        sprintf((char*)pbuf,pSTR_BHCURVE_STR4[Main_Language]);
        LCD_DispStr(0, 93, (char*)pbuf, &sfont);
        //"心 率"，"(次/分)"
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_RED;
        sprintf((char*)pbuf,pSTR_BHCURVE_STR5[Main_Language]);
        LCD_DispStr(5, 165, (char*)pbuf, &sfont);
        sfont.FrontColor=CL_WHITE;
        sprintf((char*)pbuf,pSTR_BHCURVE_STR4[Main_Language]);
        LCD_DispStr(0, 183, (char*)pbuf, &sfont);
        //虚线-呼吸
        sfont.FontCode=FC_ST_12;
        sfont.FrontColor=CL_GREY5;
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//30
        LCD_DispStr(75+6, 54, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//20
        LCD_DispStr(75+6, 74, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//10
        LCD_DispStr(75+6, 94, (char*)pbuf, &sfont);
        LCD_DrawLineH(75+6,121,308,CL_GREY1);                 //0
        //虚线-心率
        LCD_DrawLineH(75+6,155,308,CL_GREY1);                 //100
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//80
        LCD_DispStr(75+6, 169, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//60
        LCD_DispStr(75+6, 189, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"- - - - - - - - - - - - - - - - - - - - ");//40
        LCD_DispStr(75+6, 209, (char*)pbuf, &sfont);
        //数字-呼吸
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_WHITE;
        sprintf((char*)pbuf,"30");
        LCD_DispStr(58+6-4, 51, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"20");
        LCD_DispStr(58+6-4, 71, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"10");
        LCD_DispStr(58+6-4, 91, (char*)pbuf, &sfont);
        sprintf((char*)pbuf," 0");
        LCD_DispStr(58+6-4, 112, (char*)pbuf, &sfont);
        //数字-心率
        sfont.FontCode=FC_ST_16;
        sfont.FrontColor=CL_WHITE;
        sprintf((char*)pbuf,"100");
        LCD_DispStr(50+6-4, 147, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"80");
        LCD_DispStr(58+6-4, 166, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"60");
        LCD_DispStr(58+6-4, 186, (char*)pbuf, &sfont);
        sprintf((char*)pbuf,"40");
        LCD_DispStr(58+6-4, 206, (char*)pbuf, &sfont);
        //读取数据
        pspara         =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
        res = pspara->MemoryDayNextP;
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
        res = Count_SubCyc(res,BreathHeart_cnt,MODULE_MEMORY_ADDR_DAY_NUM-1);
        psDayInfo      =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psDayBreath    =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psDayHeartRate =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        psDayBodyMove  =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(Hci_FlashDataSign==0)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BREATH_R,(uint8_t*)psDayBreath,&res);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_HEARTRATE_R,(uint8_t*)psDayHeartRate,&res);
            Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_BODYMOVE_R,(uint8_t*)psDayBodyMove,&res);
        }
        else if(Hci_FlashDataSign==1)
        {
            if(uctsk_Rfms_ExtFlashReadSign)
            {
                memcpy((char*)psDayInfo,(char*)&uctsk_Rfms_s_DayInfo,sizeof(MODULE_MEMORY_S_DAY_INFO));
                memcpy((char*)psDayBreath,(char*)&uctsk_Rfms_s_DayBreath,sizeof(MODULE_MEMORY_S_DAY_BREATH));
                memcpy((char*)psDayHeartRate,(char*)&uctsk_Rfms_s_HeartRate,sizeof(MODULE_MEMORY_S_DAY_HEARTRATE));
                memcpy((char*)psDayBodyMove,(char*)&uctsk_Rfms_s_BodyMove,sizeof(MODULE_MEMORY_S_DAY_BODYMOVE));
            }
            else
            {
                uctsk_Rfms_RunFileData_DebugTestOnOff(ON);
            }
        }
        else
        {
            ;
        }
        //显示数据
        //---绘制呼吸心率曲线
        if(Hci_FlashDataSign==1&&uctsk_Rfms_ExtFlashReadSign==0)
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=RGB(255,  100,  0);
            sprintf((char*)pbuf,"Loading Data,Waiting...");
            LCD_DispStr(87, 137, (char*)pbuf, &sfont);
            goto HciMenuBreathHeartCurve_Goto1;
        }
        else if((Hci_SleepDataValid(psDayBreath->DataBuf, sizeof(psDayBreath->DataBuf)) == ERR) \
                && (Hci_SleepDataValid(psDayHeartRate->DataBuf, sizeof(psDayHeartRate->DataBuf)) == ERR))
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=RGB(255,  100,  0);
            sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR3[Main_Language]);
            LCD_DispStr(87, 130, (char*)pbuf, &sfont);
            goto HciMenuBreathHeartCurve_Goto1;
        }
        /*
        else if(Count_2ByteToWord(psDayInfo->BodyMoveCount[0],psDayInfo->BodyMoveCount[1])>=66)
        {
            sfont.FontCode=FC_ST_16;
            sfont.FrontColor=RGB(255,  100,  0);
            sprintf((char*)pbuf,"监测无效,请注意监测环境");
            LCD_DispStr(87, 130, (char*)pbuf, &sfont);
        }
        */
        else
        {
            //uint16_t i16;
            sfont.FontCode=FC_ST_16;
            if(Hci_ParaChangeSign==1)
            {
                sfont.FrontColor=sfont.BackColor;
                memset((char*)pbuf,' ',25);
                pbuf[25]=0;
                LCD_DispStr(87, 137, (char*)pbuf, &sfont);
                Hci_ParaChangeSign=0;
            }
            //画曲线
            Hci_Draw_Breath(psDayInfo,psDayBreath,psDayBodyMove);
            Hci_Draw_Heart(psDayInfo,psDayHeartRate,psDayBodyMove);
            //ww
            if(UCTSK_RFMS_E_RUNSTATE_IDLE==uctsk_Rfms_GetRunState())
            {
                //---测试时间
                sfont.FontCode=FC_ST_16;
                sfont.FrontColor=CL_WHITE;
                sprintf((char*)pbuf,pSTR_SLEEP_CURVE_STR4[Main_Language],\
                        psDayInfo->BeginTime[2],psDayInfo->BeginTime[3],psDayInfo->BeginTime[4],psDayInfo->BeginTime[5],
                        psDayInfo->EndTime[2],psDayInfo->EndTime[3],psDayInfo->EndTime[4],psDayInfo->EndTime[5]);
                LCD_DispStr(5, 32, (char*)pbuf, &sfont);
            }
            i16 = psDayInfo->BeginTime[4] + ((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)/60);
            if((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)%60)
            {
                i16++;
            }
            i16 = i16%24;
            //画横坐标 由测量累计时间计算
            Hci_Draw_HBCoordinate(75+6,130,psDayInfo->BeginTime[4],i16);
        }
        //标记上传状态
        if(psDayInfo->UpdataToServerCmt==0)
        {
            //未上传
            sprintf((char*)pbuf,pSTR_NO_SEND[Main_Language]);
            sfont.FrontColor   =  CL_RED;
        }
        else
        {
            //已上传
            sprintf((char*)pbuf,pSTR_SENDED[Main_Language]);
            sfont.FrontColor   =  CL_GREEN;
        }
        sfont.FontCode     =  FC_ST_16;
        LCD_DispStr(320-4*16, 8, (char*)pbuf, &sfont);
    HciMenuBreathHeartCurve_Goto1:
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayBodyMove);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayHeartRate);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayBreath);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    //按键处理
    if(/*(res==UCTSK_RFMS_E_RUNSTATE_IDLE)\*/
        (BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]>3*100)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]>3*100)\
        &&(Hci_Power ==  BSP_PWR_E_POWER_AC))
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]=0xFFFF;
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_SOS;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_cursor          =  0;
        Hci_CursorFlick     =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
    }
    //大于30秒则返回默认页面
    if(Hci_ReturnTimer>=60*100)
    {
        BspKey_NewSign =  1;
        BspKey_Value   =  HCI_KEY_ESC;
    }
    else
    {
        Hci_ReturnTimer++;
    }
    //
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign  =  0;
        Hci_ReturnTimer =  0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                if(Hci_FlashDataSign==0)
                {
                    pHci_MenuFun = pHci_DefaultMenuFun;
                }
                else if(Hci_FlashDataSign==1)
                {
                    Hci_FlashDataSign = 0;
                    pHci_MenuFun = Hci_Menu_II_Info;
                }
                curve_cnt = 1;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                if(Hci_FlashDataSign==0)
                {
                    if(curve_cnt > 1)
                        curve_cnt--;
                    else
                        curve_cnt = 1;
                    pHci_MenuFun = Hci_Menu_SleepCurve;
                    Hci_ReShowTimer = 0;
                    LCD_ClrScr(sfont.BackColor);
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_FlashDataSign==0)
                {
                    curve_cnt++;
                    if(BreathHeart_cnt>=MODULE_MEMORY_ADDR_DAY_NUM)
                        curve_cnt = 1;
                    pHci_MenuFun = Hci_Menu_SleepCurve;
                    Hci_ReShowTimer = 0;
                    LCD_ClrScr(sfont.BackColor);
                }
                break;
            case HCI_KEY_TOP:
                Hci_ReShowTimer=0;
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 画呼吸曲线
* 参    数:
* 说    明:
*******************************************************************************/
static void Hci_Draw_Breath(MODULE_MEMORY_S_DAY_INFO *psDayInfo,MODULE_MEMORY_S_DAY_BREATH *psDayBreath,MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove)
{
    uint16_t y0_breath = 114+8, y1_breath = 96+8, x0_breath = 75+6, x1_breath = 75+240;
    uint16_t dy_breath = (y0_breath - y1_breath)*3/2;
    uint8_t tempMin_breath, tempMax_breath;
    float tempCoeff_breath;
    uint16_t respColorMap_breath[] = {CL_YELLOW,/*CL_BLUE*/CL_CYAN};
    uint8_t step_breath;
    uint16_t dl_breath=x0_breath;
    uint16_t i_breath = 0;
    uint8_t sleeptime;
    //申请缓存
    uint8_t *ptempBuffer_breath;
    uint16_t *psignalBuffer_breath;
    ptempBuffer_breath   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    psignalBuffer_breath =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    // a.求呼吸体动系数
    if (psDayInfo->BreathNum <= 10)
    {
        tempMax_breath = 20;
        tempMin_breath = 12;
    }
    else if ((psDayInfo->BreathMax) > (uint8_t)((float)psDayInfo->BreathMin * 1.75f))
    {
        tempMax_breath =(uint8_t)((float)psDayInfo->BreathMax * 0.9f);
        tempMin_breath =(uint8_t)((float)psDayInfo->BreathMin * 1.1f);
    }
    else
    {
        tempMax_breath = psDayInfo->BreathMax;
        tempMin_breath = psDayInfo->BreathMin;
    }

    tempCoeff_breath = (float)(tempMax_breath - tempMin_breath) / psDayInfo->BodyMoveMax;
    //b.呼吸插值
    uctsk_Rfms_Interpolation_Proc(psDayBreath->DataBuf, psDayBodyMove->DataBuf, ptempBuffer_breath, psDayInfo->BodyMoveNum+1, tempMin_breath, tempCoeff_breath, 0xB5);
    //c.呼吸平滑处理----移动平均
    uctsk_Rfms_Average_Proc(ptempBuffer_breath, 15, psignalBuffer_breath, y0_breath-dy_breath, dy_breath, psDayInfo->BodyMoveNum+1, 0xB5,(float)dy_breath/15);//0~30's average
    //beyond 6 hours
    /*
    if(psDayInfo->BodyMoveNum>20*6)
        step_breath = 1;
    else
        step_breath = 2;
    */
    sleeptime=(psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)/60;
    if((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)%60)
    {
        sleeptime++;
    }
    if(sleeptime>6)
    {
        step_breath = 1;
    }
    else
    {
        step_breath = 2;
    }

    dl_breath += psDayInfo->BeginTime[5] *step_breath/3;

    //d.画呼吸曲线
    while ((0xB5 != *(psignalBuffer_breath+i_breath)) && (i_breath <= psDayInfo->BodyMoveNum) && (dl_breath < x1_breath))//&&(dl < 310))
    {
        LCD_DrawLine(dl_breath, psignalBuffer_breath[i_breath]-1, dl_breath, psignalBuffer_breath[i_breath], respColorMap_breath[(*(ptempBuffer_breath+i_breath)&0x80) != 0]);
        LCD_DrawLine(dl_breath+step_breath-1, psignalBuffer_breath[i_breath]-1, dl_breath+step_breath-1, psignalBuffer_breath[i_breath], respColorMap_breath[(*(ptempBuffer_breath+i_breath)&0x80) != 0]);
        dl_breath += step_breath;
        i_breath++;
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptempBuffer_breath);
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,psignalBuffer_breath);
}
/*******************************************************************************
* 函数功能: 画心率曲线
* 参    数:
* 说    明:
*******************************************************************************/
static void Hci_Draw_Heart(MODULE_MEMORY_S_DAY_INFO *psDayInfo,MODULE_MEMORY_S_DAY_HEARTRATE *psDayHeartRate,MODULE_MEMORY_S_DAY_BODYMOVE *psDayBodyMove)
{
    uint16_t y0_heart = 150+8, y1_heart = 168+8, x0_heart = 75+6, x1_heart = 75+240;
    uint16_t dy_heart = (y1_heart - y0_heart)*3/2;
    uint8_t tempMin_heart, tempMax_heart;
    float tempCoeff_heart;
    uint16_t respColorMap_heart[] = {CL_MAGENTA,CL_RED};
    uint8_t step_heart;
    uint16_t dl_heart=x0_heart;
    uint16_t i_heart = 0;
    uint8_t sleeptime;
    //申请缓存
    uint8_t *ptempBuffer_heart;
    uint16_t *psignalBuffer_heart;
    ptempBuffer_heart   =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    psignalBuffer_heart =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    // a.求心率体动系数
    if (psDayInfo->HeartRateNum <= 10)
    {
        tempMax_heart = 65;
        tempMin_heart = 45;
    }
    else if ((psDayInfo->HeartRateMax) > (uint8_t)((float)psDayInfo->HeartRateMin * 1.75f))
    {
        tempMax_heart =(uint8_t)((float)psDayInfo->HeartRateMax * 0.9f);
        tempMin_heart =(uint8_t)((float)psDayInfo->HeartRateMin * 1.1f);
    }
    else
    {
        tempMax_heart = psDayInfo->HeartRateMax;
        tempMin_heart = psDayInfo->HeartRateMin;
    }

    tempCoeff_heart = (float)(tempMax_heart - tempMin_heart) / psDayInfo->BodyMoveMax;

    //b.心率插值
    uctsk_Rfms_Interpolation_Proc(psDayHeartRate->DataBuf, psDayBodyMove->DataBuf, ptempBuffer_heart, psDayInfo->BodyMoveNum+1, tempMin_heart, tempCoeff_heart, 0xE5);
    //c.心率平滑处理----移动平均
    uctsk_Rfms_Average_Proc(ptempBuffer_heart, 70, psignalBuffer_heart, y0_heart+dy_heart, dy_heart, psDayInfo->BodyMoveNum+1, 0xE5,(float)dy_heart/30);//40~100's average
    /*
    if(psDayInfo->BodyMoveNum>20*6)//beyond 6 hours
        step_heart = 1;
    else
        step_heart = 2;
    */
    sleeptime=(psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)/60;
    if((psDayInfo->BeginTime[5]+psDayInfo->BodyMoveNum*3)%60)
    {
        sleeptime++;
    }
    if(sleeptime>6)
    {
        step_heart = 1;
    }
    else
    {
        step_heart = 2;
    }
    dl_heart += psDayInfo->BeginTime[5] *step_heart/3;

    //d.画心率曲线
    while ((0xE5 != *(psignalBuffer_heart+i_heart)) && (i_heart <= psDayInfo->BodyMoveNum) && (dl_heart < x1_heart))//&&(dl < 310))
    {
        LCD_DrawLine(dl_heart, psignalBuffer_heart[i_heart]-1, dl_heart, psignalBuffer_heart[i_heart], respColorMap_heart[(*(ptempBuffer_heart+i_heart)&0x80) != 0]);
        LCD_DrawLine(dl_heart+step_heart-1, psignalBuffer_heart[i_heart]-1, dl_heart+step_heart-1, psignalBuffer_heart[i_heart], respColorMap_heart[(*(ptempBuffer_heart+i_heart)&0x80) != 0]);
        dl_heart += step_heart;
        i_heart++;
    }
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,ptempBuffer_heart);
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB,psignalBuffer_heart);
}
/*******************************************************************************
* 函数功能: 画呼吸心率曲线坐标
* 参    数:
* 说    明:
*******************************************************************************/
static void Hci_Draw_HBCoordinate(uint16_t X_Coordinate, uint16_t Y_Coordinate, uint8_t Begintime, uint8_t Endtime)
{
    uint8_t sleeptime,i,j,time_temp;
    uint8_t pbuf[4];
    FONT_T sfont;
    sfont.FontCode=FC_ST_16;
    sfont.FrontColor=CL_WHITE;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    //
    if(Endtime-Begintime>0)
    {
        sleeptime = Endtime-Begintime;
    }
    else
    {
        sleeptime = 24-Begintime+Endtime;
    }
    //
    if(sleeptime > 6)
    {
        j=20;
    }
    else
    {
        //j=37;
        j=40;
    }
    //
    for(i=0; i<=sleeptime; i++)
    {
        if((X_Coordinate+i*j+2)>=LCD_GetXSize())
        {
            break;
        }
        if(Begintime+i > 23)
            time_temp = Begintime+i-24;
        else
            time_temp = Begintime+i;
        sprintf((char*)pbuf,"%d",time_temp);
        if(time_temp<10)
        {
            LCD_DispStr(X_Coordinate+i*j-2, Y_Coordinate, (char*)pbuf, &sfont);
        }
        else
        {
            LCD_DispStr(X_Coordinate+i*j-8, Y_Coordinate, (char*)pbuf, &sfont);
        }
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-3-7,    X_Coordinate+i*j+1,  CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-4-7,    X_Coordinate+i*j+1,  CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j  , Y_Coordinate-5-7,    X_Coordinate+i*j  ,  CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j  , Y_Coordinate-3+7+24, X_Coordinate+i*j  ,  CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-4+7+24, X_Coordinate+i*j+1,  CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-5+7+24, X_Coordinate+i*j+1,  CL_GREY3);
        //竖条(最后不已竖条结尾)
        if(i!=sleeptime)
        {
            //LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-5,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-7-4,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-7-3,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate+7+24-1-4,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate+7+24-1-3,X_Coordinate+i*j+(j/2), CL_GREY3);
        }
    }
}
/*******************************************************************************
* 函数功能: 画体动曲线坐标
* 参    数:
* 说    明:
*******************************************************************************/
#define HCI_DAYMOVE_COLOR_1   Count_4ByteToLong(0,213,155,91)
#define HCI_DAYMOVE_COLOR_2   Count_4ByteToLong(0,204,242,255)
#define HCI_DAYMOVE_COLOR_3   Count_4ByteToLong(0,0,0,255)
#define HCI_DAYMOVE_COORDINATE_X    40
#define HCI_DAYMOVE_COORDINATE_Y    70
#define HCI_DAYMOVE_COORDINATE_Y1   25
#define HCI_DAYMOVE_COORDINATE_Y2   8
#define HCI_DAYMOVE_COORDINATE_Y3   5
#define HCI_DAYMOVE_COORDINATE_YMAX 30
#define HCI_DAYMOVE_COORDINATE_YVALUEMIN  150000
//#define HCI_DAYMOVE_COORDINATE_YVALUEMAX  3000000
#define HCI_DAYMOVE_COORDINATE_YVALUEMAX  250000
const uint32_t HciDayMoveCoordinateYTable[HCI_DAYMOVE_COORDINATE_YMAX]=
{
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*1L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*2L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*3L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*4L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*5L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*6L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*7L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*8L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*9L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*10L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*11L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*12L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*13L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*14L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*15L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*16L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*17L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*18L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*19L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*20L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*21L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*22L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*23L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*24L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*25L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*26L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*27L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*28L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*29L,
    HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*30L,
};
static void Hci_Menu_DayMoveCurve(void)
{
    uint8_t *pbuf,*pbuf1;
    uint32_t *pbuf2;
    uint8_t res=0;
    uint8_t i=0,j=0,k=0;
    uint8_t Sleep_cnt=0;
    uint16_t i16=0;
    uint32_t i32=0,j32=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    Sleep_cnt = (curve_cnt+1)/2;
    //定时刷新判断
    //刷新屏幕
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100*60;
        //申请缓存
        pbuf            =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pbuf1           =  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
        pbuf2           =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        // 0   写调试数据
#if 0
        if(Hci_DayMoveCnt==0)
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_EXTFLASH_FORMAT,NULL ,NULL);
            for(res=0; res<MODULE_MEMORY_EXTFLASH_MOVE_NUM; res++)
            {
                uint32_t *pi32=(uint32_t *)pbuf;
                pbuf[250]=17;
                pbuf[251]=4;
                pbuf[252]=res+18;
                k = res;
                for(i16=0; i16<1440; i16++)
                {
                    for(j=0; j<6; j++)
                    {
                        if(((i16*6)+j)%9==0)
                        {
                            pi32[j]=k;
                            k++;
                            k=k%(HCI_DAYMOVE_COORDINATE_Y1+6);
                        }
                        else
                        {
                            pi32[j]=0;
                        }
                    }
                    pbuf[253]=i16/60;
                    pbuf[254]=i16%60;
                    Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_1MIN_W,pbuf,&pbuf[250]);
                }
            }
        }
#endif
        //  1  获取数据日期
        Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_DATE_R,pbuf,NULL);
        DebugOutHex("Move DateBuf:\r\n",pbuf,MODULE_MEMORY_EXTFLASH_MOVE_NUM*4);
        //  2  找到第n(Hci_DayMoveCnt)大日期索引(索引范围(0-6)-->j)
        for(res=0; res<MODULE_MEMORY_EXTFLASH_MOVE_NUM; res++)
        {
            i32=0;
            j=0;
            for(i=0; i<MODULE_MEMORY_EXTFLASH_MOVE_NUM; i++)
            {
                if(((pbuf[4*i]==0xFF)&&(pbuf[4*i+1]==0xFF)&&(pbuf[4*i+2]==0xFF)&&(pbuf[4*i+3]==0xFF))
                   ||((pbuf[4*i]==0)&&(pbuf[4*i+1]==0)&&(pbuf[4*i+2]==0)&&(pbuf[4*i+3]==0)))
                {
                    continue;
                }
                if(i32<=Count_4ByteToLong(0,pbuf[4*i+1],pbuf[4*i+2],pbuf[4*i+3]))
                {
                    i32=Count_4ByteToLong(0,pbuf[4*i+1],pbuf[4*i+2],pbuf[4*i+3]);
                    j=i;
                }
            }
            if(res>=Hci_DayMoveCnt)
            {
                break;
            }
            else
            {
                pbuf[4*j+0]=pbuf[4*j+1]=pbuf[4*j+2]=pbuf[4*j+3]=0;
            }
        }
        sprintf((char*)pbuf1,"Move MaxDateIndex:%d\r\n",j);
        DebugOutStr((int8_t*)pbuf1);
        //  3  日期变动则清空
        if(j!=Hci_ParaChangeSign)
        {
            Hci_ParaChangeSign=j;
            LCD_ClrScr(sfont.BackColor);
        }
        //  4  赋值相应日期
        pbuf[250]=pbuf[4*j];     // 状态
        pbuf[251]=pbuf[4*j+1];   // 年
        pbuf[252]=pbuf[4*j+2];   // 月
        pbuf[253]=pbuf[4*j+3];   // 日
        DebugOutHex("Move Date:\r\n",&pbuf[250],4);
        // 无有效日期处理
        //---标题
        if(pbuf[251]!=0xFF && pbuf[251]!=0x00)
        {
            res=sprintf((char*)pbuf,pSTR_DAYMOVECURVE_HEAD1[Main_Language],Hci_DayMoveCnt+1,pbuf[252],pbuf[253]);
            Hci_DayMoveDate[0]=pbuf[251];
            Hci_DayMoveDate[1]=pbuf[252];
            Hci_DayMoveDate[2]=pbuf[253]; 
        }
        else
        {
            res=sprintf((char*)pbuf,pSTR_DAYMOVECURVE_HEAD2[Main_Language],Hci_DayMoveCnt+1);
            Hci_DayMoveDate[0]=0;
            Hci_DayMoveDate[1]=0;
            Hci_DayMoveDate[2]=0;
        }
        //黄色显示
        LCD_DispStr((g_LcdWidth-res*8)/2-16, 8, (char*)pbuf, &sfont);
        //白色重画
        sprintf((char*)pbuf,pSTR_DAYMOVECURVE_HEAD3[Main_Language],Hci_DayMoveCnt+1);
        sfont.FrontColor=CL_WHITE;
        LCD_DispStr((g_LcdWidth-res*8)/2-16, 8, (char*)pbuf, &sfont);
        //显示show
        if(Hci_cursor==1)
        {
            j = Hci_CursorFlick+1;
            sprintf((char*)pbuf,"Show-%dX",12-Hci_CursorFlick);
            sfont.FrontColor=RGB(91,155,213);
            LCD_DispStr(40, 24, (char*)pbuf, &sfont);
        }
        else
        {
            Hci_CursorFlick=3;
            j = Hci_CursorFlick+1;
        }
        for(i=0; i<HCI_DAYMOVE_COORDINATE_YMAX; i++)
        {
            //pbuf2[i]=HciDayMoveCoordinateYTable[i]*j;
            pbuf2[i]=HCI_DAYMOVE_COORDINATE_YVALUEMIN+((HCI_DAYMOVE_COORDINATE_YVALUEMAX*j-HCI_DAYMOVE_COORDINATE_YVALUEMIN)/HCI_DAYMOVE_COORDINATE_YMAX)*(i+1);
        }
        //标记上传状态
        if(pbuf[251]!=0xFF && pbuf[251]!=0x00)
        {
            if(OK==Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_VALID_R,NULL,Hci_DayMoveDate))
            {
                //未上传
                sprintf((char*)pbuf,pSTR_NO_SEND[Main_Language]);
                sfont.FrontColor   =  CL_RED;
            }
            else
            {
                //已上传
                sprintf((char*)pbuf,pSTR_SENDED[Main_Language]);
                sfont.FrontColor   =  CL_GREEN;
            }
            sfont.FontCode     =  FC_ST_16;
            LCD_DispStr(320-4*16, 8, (char*)pbuf, &sfont);
        }
        // 画图表
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetColor(HCI_DAYMOVE_COLOR_1);
        for(i=0; i<4; i++)
        {
            GUI_DrawHLine(HCI_DAYMOVE_COORDINATE_Y+i*50,HCI_DAYMOVE_COORDINATE_X,HCI_DAYMOVE_COORDINATE_X+240);
            for(j=0; j<7; j++)
            {
                GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+40*j,HCI_DAYMOVE_COORDINATE_Y+i*50-HCI_DAYMOVE_COORDINATE_Y1,HCI_DAYMOVE_COORDINATE_Y+i*50);
                if(j!=6)
                {
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+10+40*j,HCI_DAYMOVE_COORDINATE_Y+i*50-HCI_DAYMOVE_COORDINATE_Y3,HCI_DAYMOVE_COORDINATE_Y+i*50);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+20+40*j,HCI_DAYMOVE_COORDINATE_Y+i*50-HCI_DAYMOVE_COORDINATE_Y2,HCI_DAYMOVE_COORDINATE_Y+i*50);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+30+40*j,HCI_DAYMOVE_COORDINATE_Y+i*50-HCI_DAYMOVE_COORDINATE_Y3,HCI_DAYMOVE_COORDINATE_Y+i*50);
                }
            }
        }
        // 写时间
        GUI_SetColor(HCI_DAYMOVE_COLOR_2);
        GUI_SetFont(&GUI_FontFont_ST_16);
        k=11;
        for(i=0; i<4; i++)
        {
            for(j=0; j<7; j++)
            {
                k++;
                k%=24;
                sprintf((char*)pbuf,"%02d",k);
                GUI_DispStringHCenterAt  ((const char *) pbuf, HCI_DAYMOVE_COORDINATE_X+40*j, HCI_DAYMOVE_COORDINATE_Y+i*50);
            }
            k--;
        }
        DebugOutHex("Move Date:\r\n",&pbuf[250],4);
        if(pbuf[251]==0xFF || pbuf[251]==0x00)
        {
            goto goto_Hci_Menu_DayMoveCurve;
        }
        // 打印所有数据
        for(i=0; i<144; i++)
        {
            pbuf[254]=i;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_10MIN_R,pbuf,&pbuf[251]);
            //-----
            sprintf((char*)&pbuf1[1000],"Move Data-%d:\r\n",i);
            DebugOutHex((char*)&pbuf1[1000],pbuf,240);
        }
        DebugOutStr("Move Date(90s):\r\n");
        // 获取整体数据
        k=0;
        i16=0;
        for(i=0; i<144; i++)
        {
            pbuf[254]=i;
            Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_10MIN_R,pbuf,&pbuf[251]);
            for(j=0; j<240; j+=4)
            {
                //i32 = Count_4ByteToLong(pbuf[j],pbuf[j+1],pbuf[j+2],pbuf[j+3]);
                i32 = Count_4ByteToLong(pbuf[j+3],pbuf[j+2],pbuf[j+1],pbuf[j+0]);
                if(i32!=0xFFFFFFFF)
                {
                    j32+=i32;
                }
                k++;
                if(k>=9)
                {
                    //分段
                    sprintf((char*)&pbuf1[1000],"%ld\r\n",j32);
                    DebugOutStr((int8_t*)&pbuf1[1000]);
                    if(j32==0)
                    {
                        pbuf1[i16++]=0xFF;
                    }
                    else
                    {
                        pbuf1[i16++]=Count_Classification(j32,(uint32_t*)pbuf2,HCI_DAYMOVE_COORDINATE_YMAX);
                    }
                    k=0;
                    j32 = 0;
                }
            }
        }
        DebugOutHex("Move pbuf1:\r\n",pbuf1,240*4);
        // 画数据
        for(i=0; i<4; i++)
        {
            // 数据载入
            /*
            for(j=0; j<241; j++)
            {
                pbuf[j]=(uint8_t)Count_Rand();
                pbuf[j]=pbuf[j]%(HCI_DAYMOVE_COORDINATE_Y1+1);
            }*/
            memcpy(pbuf,&pbuf1[240*i],240);
            for(j=0; j<240; j++)
            {
                if(pbuf[j]!=0xFF)
                {
                    pbuf[j]=pbuf[j]%(HCI_DAYMOVE_COORDINATE_YMAX+1);
                }
            }
            // 画数据
            GUI_SetColor(HCI_DAYMOVE_COLOR_3);
            for(j=0; j<240; j++)
            {
                if(pbuf[j]==0xFF)
                {
                    continue;
                }
                if(j%10!=0)
                {
                    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                    GUI_SetColor(HCI_DAYMOVE_COLOR_3);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+j,HCI_DAYMOVE_COORDINATE_Y+i*50-pbuf[j],HCI_DAYMOVE_COORDINATE_Y+i*50);
                    continue;
                }
                if(j%40==0)
                {
                    k=HCI_DAYMOVE_COORDINATE_Y1;
                }
                else if(j%20==0)
                {
                    k=HCI_DAYMOVE_COORDINATE_Y2;
                }
                else
                {
                    k=HCI_DAYMOVE_COORDINATE_Y3;
                }
                if(((j%40==0)&&(pbuf[j]<=HCI_DAYMOVE_COORDINATE_Y1))\
                   ||((j%20==0)&&(pbuf[j]<=HCI_DAYMOVE_COORDINATE_Y2))\
                   ||((j%10==0)&&(pbuf[j]<=HCI_DAYMOVE_COORDINATE_Y3)))
                {
                    // 幅值小于坐标,异或画幅值
                    GUI_SetDrawMode(GUI_DRAWMODE_XOR);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+j,HCI_DAYMOVE_COORDINATE_Y+i*50-pbuf[j],HCI_DAYMOVE_COORDINATE_Y+i*50);
                }
                else
                {
                    // 幅值大于坐标,先画幅值,再异或画坐标
                    GUI_SetDrawMode(GUI_DRAWMODE_XOR);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+j,HCI_DAYMOVE_COORDINATE_Y+i*50-k,HCI_DAYMOVE_COORDINATE_Y+i*50);
                    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                    GUI_DrawVLine(HCI_DAYMOVE_COORDINATE_X+j,HCI_DAYMOVE_COORDINATE_Y+i*50-pbuf[j],HCI_DAYMOVE_COORDINATE_Y+i*50-k-1);
                }
            }
        }
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    goto_Hci_Menu_DayMoveCurve:
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_1KB,pbuf1);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf2);
    }
    //按键处理
    if(
        (BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]>3*100)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]!=0xFFFF)\
        &&(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]>3*100)\
        &&(Hci_Power ==  BSP_PWR_E_POWER_AC))
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_LEFT-1]=0xFFFF;
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_RIGHT-1]=0xFFFF;
        pHci_MenuFun = Hci_Menu_SOS;
        Hci_ReShowTimer     =  0;
        Hci_ParaChangeSign  =  0;
        Hci_ReturnTimer     =  0;
        Hci_NoFirst         =  0;
        LCD_ClrScr(CL_BLACK);
    }
    //大于30秒则返回默认页面
    /*
    if(Hci_ReturnTimer>=60*100)
    {
        BspKey_NewSign =  1;
        BspKey_Value   =  HCI_KEY_ESC;
    }
    else
    {
        Hci_ReturnTimer++;
    }
    */
    //
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign  =  0;
        Hci_ReturnTimer =  0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                /*
                if(Hci_cursor==0)
                {
                    Hci_cursor=1;
                }
                else
                {
                    Hci_cursor=0;
                }
                LCD_ClrScr(CL_BLACK);
                Hci_ReShowTimer=0;
                */
                break;
            case HCI_KEY_ESC:
                if(Hci_cursor==1)
                {
                    Hci_cursor=0;
                    LCD_ClrScr(CL_BLACK);
                    Hci_ReShowTimer=0;
                    break;
                }
                if(Hci_CalledFun==0)
                {
                    pHci_MenuFun = pHci_DefaultMenuFun;
                }
                if(Hci_CalledFun==1)
                {
                    pHci_MenuFun = Hci_Menu_II_Info;
                }
                else if(Hci_CalledFun==2)
                {
                    pHci_MenuFun = Hci_Menu_II_Set;
                }
                else if(Hci_CalledFun==3)
                {
                    pHci_MenuFun = Hci_Menu_II_Test;
                }
                else
                {
                    pHci_MenuFun = pHci_DefaultMenuFun;
                }
                curve_cnt = 1;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(CL_BLACK);
                break;
            case HCI_KEY_LEFT:
                /*
                if(Hci_CalledFun==0)
                {
                    if(Hci_FlashDataSign==0)
                    {
                        curve_cnt--;
                        if(curve_cnt == 0)
                            curve_cnt = MODULE_MEMORY_ADDR_DAY_NUM*2;
                        pHci_MenuFun = Hci_Menu_SleepCurve;
                    }
                }
                */
                if(Hci_cursor==0)
                {
                    //
                    Hci_ReShowTimer=0;
                    Hci_CursorFlick = 0;
                    LCD_ClrScr(CL_BLACK);
                    if(Hci_DayMoveCnt==0)
                    {
                        Hci_DayMoveCnt=MODULE_MEMORY_EXTFLASH_MOVE_NUM-1;
                    }
                    else
                    {
                        Hci_DayMoveCnt--;
                    }
                }
                else
                {
                    if(Hci_CursorFlick<11)
                    {
                        Hci_CursorFlick++;
                        LCD_ClrScr(CL_BLACK);
                        Hci_ReShowTimer=0;
                    }
                }
                break;
            case HCI_KEY_RIGHT:
                if(Hci_cursor==0)
                {
                    if(Hci_CalledFun==0)
                    {
                        if(Hci_FlashDataSign==0)
                        {
                            curve_cnt++;
                            if(Sleep_cnt>MODULE_MEMORY_ADDR_DAY_NUM)
                                curve_cnt = 1;
                            pHci_MenuFun = Hci_Menu_SleepCurve;
                        }
                    }
                    Hci_ReShowTimer = 0;
                    Hci_CursorFlick = 0;
                    LCD_ClrScr(CL_BLACK);
                    //
                    if(Hci_DayMoveCnt>=MODULE_MEMORY_EXTFLASH_MOVE_NUM-1)
                    {
                        Hci_DayMoveCnt=0;
                    }
                    else
                    {
                        Hci_DayMoveCnt++;
                    }
                }
                else
                {
                    if(Hci_CursorFlick!=0)
                    {
                        Hci_CursorFlick--;
                        LCD_ClrScr(CL_BLACK);
                        Hci_ReShowTimer=0;
                    }
                }
                break;
            case HCI_KEY_TOP:
                break;
            default:
                break;
        }
    }
    if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]>3*100)
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]=0xFFFF;
        if((Hci_DayMoveDate[0]!=0)||(Hci_DayMoveDate[1]!=0)||(Hci_DayMoveDate[2]!=0))
        {
            Module_Memory_App(MODULE_MEMORY_APP_CMD_MOVE_UPLOAD_RESET,Hci_DayMoveDate,NULL);
            LCD_ClrScr(CL_BLACK);
        }
        while(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ESC-1]!=0)
        {
            MODULE_OS_DELAY_MS(20);
        }
        MODULE_OS_DELAY_MS(500);
        Hci_ReShowTimer =  0;
        BspKey_NewSign  =  0;
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]!=0xFFFF && BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>8*100)
    {
        BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]=0xFFFF;
        if(Hci_cursor==0)
        {
            Hci_cursor=1;
            LCD_ClrScr(CL_BLACK);
            Hci_ReShowTimer=0;
        }
        MODULE_OS_DELAY_MS(500);
        Hci_ReShowTimer =  0;
        BspKey_NewSign  =  0;
    }
}

/*******************************************************************************
* 函数功能: 判断数据是否有效
* 说    明: 参考isleep界面ww
*******************************************************************************/
static uint8_t Hci_SleepDataValid(uint8_t *pbuf, uint16_t len)
{
    uint16_t i;
    uint32_t sum=0;
    //数据全部为0x00或0xFF,则数据无效
    for(i=0; i<len; i++)
    {
        sum = sum+pbuf[i];
    }
    if(sum == 0 || sum == (len*0xFF))
        return ERR;
    else
        return OK;
}
/*******************************************************************************
* 函数功能: 画睡眠曲线坐标
* 参    数:
* 说    明:
*******************************************************************************/
static void Hci_Draw_SleepCoordinate(uint16_t X_Coordinate, uint16_t Y_Coordinate, uint8_t Begintime, uint8_t Endtime)
{
    uint8_t sleeptime,i,j,time_temp;
    uint8_t pbuf[4];
    FONT_T sfont;
    sfont.FontCode=FC_ST_16;
    sfont.FrontColor=CL_WHITE;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    //
    if(Endtime-Begintime>0)
    {
        sleeptime = Endtime-Begintime;
    }
    else
    {
        sleeptime = 24-Begintime+Endtime;
    }
    //
    if(sleeptime > 6)
    {
        j=20;
    }
    else
    {
        j=40;
    }
    //
    for(i=0; i<=sleeptime; i++)
    {
        if((X_Coordinate+i*j+2)>=LCD_GetXSize())
        {
            break;
        }
        if(Begintime+i > 23)
            time_temp = Begintime+i-24;
        else
            time_temp = Begintime+i;
        sprintf((char*)pbuf,"%d",time_temp);
        if(time_temp<10)
        {
            LCD_DispStr(X_Coordinate+i*j-2, Y_Coordinate, (char*)pbuf, &sfont);
        }
        else
        {
            LCD_DispStr(X_Coordinate+i*j-8, Y_Coordinate, (char*)pbuf, &sfont);
        }
        //画三角
        LCD_DrawLineH(X_Coordinate+i*j, Y_Coordinate-5, X_Coordinate+i*j, CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-4, X_Coordinate+i*j+1, CL_GREY3);
        LCD_DrawLineH(X_Coordinate+i*j-1, Y_Coordinate-3, X_Coordinate+i*j+1, CL_GREY3);
        //竖条(最后不已竖条结尾)
        if(i!=sleeptime)
        {
            //LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-5,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-4,X_Coordinate+i*j+(j/2), CL_GREY3);
            LCD_DrawLineH(X_Coordinate+i*j+(j/2), Y_Coordinate-3,X_Coordinate+i*j+(j/2), CL_GREY3);
        }
    }
}
/*******************************************************************************
* 函数功能: 软件升级
*******************************************************************************/
void Hci_Menu_SetBluetoothIAP(void)
{
    uint8_t res;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(Hci_NoFirst==0)
    {
        Hci_NoFirst=1;
        //升级时,蓝牙持续供电
        res=ON;
        Bluetooth_App(BLUETOOTH_CMD_KEEP_CONNECT_ONOFF,&res);
        //申请缓存
        Hci_pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        memset((char*)Hci_pbuf,0,256);
        //
    }
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;

        //
        sprintf((char*)Hci_pbuf,"Menu-Set SoftwareUpdata");
        LCD_DispStr((g_LcdWidth-strlen((char*)Hci_pbuf)*8)/2, 0,(char*)Hci_pbuf, &sfont);
        sprintf((char*)Hci_pbuf,"-----Now Ver-----");
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //显示硬件版本
        sprintf((char*)Hci_pbuf,"Hareware Ver: %02d.%02d",HARDWARE_VER,HARDWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //显示软件版本
        sprintf((char*)Hci_pbuf,"Software Ver: %02d.%02d",SOFTWARE_VER,SOFTWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf , &sfont);
        //显示出厂日期
        sprintf((char*)Hci_pbuf,"Factory Date: %4d-%02d-%02d %02d:%02d:%02d",\
                FACTORY_TIME_YEAR,FACTORY_TIME_MONTH,FACTORY_TIME_DAY,\
                FACTORY_TIME_HOUR,FACTORY_TIME_MINUTE,FACTORY_TIME_SECONT);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf , &sfont);
        sprintf((char*)Hci_pbuf,"-----------------");
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //蓝牙连接状态
        if(Bluetooth_ConnectSign==1)
        {
            sprintf((char*)Hci_pbuf,"Bluetooth State    : Connected");
        }
        else
        {
            sprintf((char*)Hci_pbuf,"Bluetooth State    : Waiting");
        }
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //总包数
        sprintf((char*)Hci_pbuf,"Iap_AllPackage     : %04d",Iap_AllPackage);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //当前包
        sprintf((char*)Hci_pbuf,"Iap_CurrentPackage : %04d",Iap_CurrentPackage);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //耗时
        sprintf((char*)Hci_pbuf,"Iap_Timer          : %d(s)",Hci_ReturnTimer);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        ypos=ypos;
        //时间累加
        if(Iap_CurrentPackage>0&&Hci_ReturnTimer==0)
        {
            Hci_ReturnTimer=1;
        }
        if(Hci_ReturnTimer!=0)
        {
            Hci_ReturnTimer++;
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                res=OFF;
                Bluetooth_App(BLUETOOTH_CMD_KEEP_CONNECT_ONOFF,&res);
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                //
                pHci_MenuFun        =  Hci_Menu_II_Set;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 软件升级
*******************************************************************************/
void Hci_Menu_InfoIAP(void)
{
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(Hci_NoFirst==0)
    {
        Hci_NoFirst=1;
        //申请缓存
        Hci_pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        memset((char*)Hci_pbuf,0,256);
        //
    }
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;

        //
        sprintf((char*)Hci_pbuf,"Menu-Set SoftwareUpdata");
        LCD_DispStr((g_LcdWidth-strlen((char*)Hci_pbuf)*8)/2, 0,(char*)Hci_pbuf, &sfont);
        sprintf((char*)Hci_pbuf,"-----Now Ver-----");
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //显示硬件版本
        sprintf((char*)Hci_pbuf,"Hareware Ver: %02d.%02d",HARDWARE_VER,HARDWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //显示软件版本
        sprintf((char*)Hci_pbuf,"Software Ver: %02d.%02d",SOFTWARE_VER,SOFTWARE_SUB_VER);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf , &sfont);
        //显示出厂日期
        sprintf((char*)Hci_pbuf,"Factory Date: %4d-%02d-%02d %02d:%02d:%02d",\
                FACTORY_TIME_YEAR,FACTORY_TIME_MONTH,FACTORY_TIME_DAY,\
                FACTORY_TIME_HOUR,FACTORY_TIME_MINUTE,FACTORY_TIME_SECONT);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf , &sfont);
        sprintf((char*)Hci_pbuf,"-----------------");
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //蓝牙连接状态
        if(Connect_s[GPRSNETAPP_XKAP_CONNECT_CH].state==GPRSNET_E_CONNECT_STATE_CONNECTED)
        {
            sprintf((char*)Hci_pbuf,"Gprs State    : Connected");
        }
        else
        {
            sprintf((char*)Hci_pbuf,"Gprs State    : Waiting");
        }
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //总包数
        sprintf((char*)Hci_pbuf,"Iap_AllPackage     : %04d",Iap_AllPackage);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //当前包
        sprintf((char*)Hci_pbuf,"Iap_CurrentPackage : %04d",Iap_CurrentPackage);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        //耗时
        sprintf((char*)Hci_pbuf,"Iap_Timer          : %d(s)",Hci_ReturnTimer);
        LCD_DispStr(0, ypos+=20, (char*)Hci_pbuf, &sfont);
        ypos=ypos;
        //时间累加
        if(Iap_CurrentPackage>0&&Hci_ReturnTimer==0)
        {
            Hci_ReturnTimer=1;
        }
        if(Hci_ReturnTimer!=0)
        {
            Hci_ReturnTimer++;
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                //
                pHci_MenuFun        =  Hci_Menu_II_Info;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*
static void Hci_Menu_InfoGasModule(void)
{
    char *pbuf;
    uint16_t ypos=0;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        //
        pbuf="Menu-Info Gas Module";
        LCD_DispStr((g_LcdWidth-strlen((char*)pbuf)*8)/2, 0,(char*)pbuf, &sfont);
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        if(GasModule_s_Info.RxNum==0)
        {
            sprintf(pbuf,"No detection Gas Module!");
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        else
        {
            sprintf(pbuf,"%04d-%02d-%02d %02d:%02d:%02d",\
                    GasModule_s_Info.DateTime[0]+2000,\
                    GasModule_s_Info.DateTime[1],\
                    GasModule_s_Info.DateTime[2],\
                    GasModule_s_Info.DateTime[3],\
                    GasModule_s_Info.DateTime[4],\
                    GasModule_s_Info.DateTime[5]\
                   );
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"Rx Num:%d",GasModule_s_Info.RxNum);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"ModuleState:%d",GasModule_s_Info.ModuleState);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"SensitiveResistance :%d  ",GasModule_s_Info.SensitiveResistance);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"Formaldehyde(66ppb) :%d  ",GasModule_s_Info.Formaldehyde_ppb);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"Alcohol(ppb)        :%d  ",GasModule_s_Info.Alcohol_ppb);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"eCO2(1000ppm)       :%d  ",GasModule_s_Info.eCO2_ppm);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
            sprintf(pbuf,"TVOC(400ppb)        :%d  ",GasModule_s_Info.TVOC_ppb);
            LCD_DispStr(0, ypos+=20, (char*)pbuf, &sfont);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                pHci_MenuFun = Hci_Menu_II_Info;
                //
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
                break;
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
*/
/*******************************************************************************
* 函数功能: 软件升级提示
*******************************************************************************/
static void Hci_Menu_IapToUser(void)
{
    static PROGBAR_Handle ahProgBar;
    FONT_T sfont;
    sfont.FontCode=0;
    sfont.FrontColor=CL_YELLOW;
    sfont.BackColor=CL_BLACK;
    sfont.Space=0;
    if(Hci_NoFirst==0)
    {
        Hci_NoFirst=1;
        //申请缓存
        Hci_pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        memset((char*)Hci_pbuf,0,256);
        //设置进度条皮肤
        PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
        //创建进度条
        ahProgBar = PROGBAR_Create(30,160,260,30,WM_CF_SHOW);
        //进度条使用内存设备
        //PROGBAR_EnableMemdev(ahProgBar);
        //设置进度条大小范围
        PROGBAR_SetMinMax(ahProgBar,0,Iap_AllPackage);
        //设置进度条字体
        PROGBAR_SetFont(ahProgBar,&GUI_FontFont_DQHT_18);
        //设置文字对齐方式
        PROGBAR_SetTextAlign(ahProgBar,GUI_TA_HCENTER);
        //设置颜色
        PROGBAR_SetBarColor(ahProgBar,0,GUI_GREEN);
        PROGBAR_SetBarColor(ahProgBar,1,GUI_RED);
    }
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=100;
        //提示:软件升级,请勿断电
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&GUI_FontFont_DQHT_24);
        GUI_SetTextMode(GUI_TM_TRANS);
        GUI_DispStringHCenterAt((char*)pSTR_UTF8_UPDATA_POWER[Main_Language],160,80);
        //

        //进度条
        {
            //要显示的文字
            sprintf((char*)Hci_pbuf,"%d/%d",Iap_CurrentPackage,Iap_AllPackage);
            PROGBAR_SetText(ahProgBar,(char*)Hci_pbuf);
            PROGBAR_SetValue(ahProgBar,Iap_CurrentPackage);
            GUI_Delay(1);
        }
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_ENTER:
                break;
            case HCI_KEY_ESC:
                //
                PROGBAR_Delete(ahProgBar);
                //释放缓存
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,Hci_pbuf);
                //
                pHci_MenuFun        =  pHci_DefaultMenuFun;
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_CursorFlick     =  0;
                Hci_NoFirst         =  0;
                Hci_ReturnTimer     =  0;
                LCD_ClrScr(sfont.BackColor);
                break;
            case HCI_KEY_LEFT:
            case HCI_KEY_RIGHT:
                break;
            default:
                break;
        }
    }
}
/*******************************************************************************
* 函数功能: 睡眠评分报告
*******************************************************************************/
#ifdef HCI_EMWIN_ENABLE
static void Hci_Menu_SleepMark(void)
{
    //
    uint8_t* pbuf;
    HEADER_Handle hHeader;
    uint8_t **pStrBuf;
    //
    MODULE_MEMORY_S_PARA *pspara;
    MODULE_MEMORY_S_DAY_INFO *psDayInfo;
    uint8_t res;
    //
    TEXT_Handle hText;
    LISTVIEW_Handle Hci_hListView;
    //-----读取数据
    pspara          =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pspara,NULL);
    res = pspara->MemoryDayNextP;
    res = Count_SubCyc(res,(curve_cnt+1)/2,MODULE_MEMORY_ADDR_DAY_NUM-1);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pspara);
    //
    psDayInfo       =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_DAY_INFO_R,(uint8_t*)psDayInfo,&res);
    if(psDayInfo->BeginByte!=0xCC)
    {
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
        return;
    }
    //-----
    //申请缓存
    pbuf = MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    //使能桌面窗口使用内存设备
    WM_EnableMemdev(WM_HBKWIN);
    //使能所有其他窗口使用内存设备
    WM_SetCreateFlags(WM_CF_MEMDEV);
    //设置桌面窗口的回调函数
    WM_SetCallback(WM_HBKWIN,Hci_CbBkWindow);
    //创建文本框
    {
        sprintf((char*)pbuf,"%02d/%02d",psDayInfo->BeginTime[2],psDayInfo->BeginTime[3]);
        hText=TEXT_CreateEx(250,17,16*5,16,WM_HBKWIN,WM_CF_SHOW,0,GUI_ID_TEXT0,(const char*)pbuf);
        TEXT_SetTextColor(hText,GUI_WHITE);
        TEXT_SetFont(hText,&GUI_FontFont_ST_16);
    }
    //创建LISTVIEW
    Hci_hListView = LISTVIEW_Create(10,40-2-2,300,190+6,0,1234,WM_CF_SHOW,0);
    //
    {
        // 获取空间句柄
        hHeader = LISTVIEW_GetHeader(Hci_hListView);
        WM_SetFocus(hHeader);
        // 加列
        // ---序号 项目 总分 得分 偏离
        HEADER_SetBkColor(hHeader,HCI_MENU_HintAsk_COLOR_1);
        HEADER_SetTextColor(hHeader,HCI_MENU_HintAsk_COLOR_3);
        HEADER_SetFont(hHeader,&GUI_FontFont_ST_16);
        LISTVIEW_AddColumn(Hci_hListView,42,(char*)pSTR_UTF8_SMR_SN[Main_Language],GUI_TA_CENTER);
        LISTVIEW_AddColumn(Hci_hListView,86,(char*)pSTR_UTF8_SMR_PROJECT[Main_Language],GUI_TA_CENTER);
        //LISTVIEW_AddColumn(Hci_hListView,50,(char*)pSTR_UTF8_SMR_FULL_SCORE[Main_Language],GUI_TA_CENTER);
        //LISTVIEW_AddColumn(Hci_hListView,50,(char*)pSTR_UTF8_SMR_GET_SCORE[Main_Language],GUI_TA_CENTER);
        LISTVIEW_AddColumn(Hci_hListView,86,(char*)pSTR_UTF8_SMR_MONITOR_RESULT[Main_Language],GUI_TA_CENTER);
        LISTVIEW_AddColumn(Hci_hListView,86,(char*)pSTR_UTF8_SMR_DEFLECT[Main_Language],GUI_TA_CENTER);
        GUI_Delay(1);
        // 加行
        LISTVIEW_SetBkColor(Hci_hListView,0,HCI_MENU_HintAsk_COLOR_2);
        LISTVIEW_SetTextColor(Hci_hListView,0,HCI_MENU_HintAsk_COLOR_4);
        GUI_Delay(1);
        LISTVIEW_SetFont(Hci_hListView,&GUI_FontFont_ST_16);
        // ---1 睡眠结构 xx xx xx
        // ---2 总睡时长 xx xx xx
        // ---3 入睡时长 xx xx xx
        // ---4 体动次数 xx xx xx
        // ---5 唤醒状态 xx xx xx
        // ---6 睡眠习惯 xx xx xx
        // ---7 温度 xx xx xx
        // ---8 湿度 xx xx xx
        pStrBuf = (uint8_t**)&pbuf[100];
        pStrBuf[0]=pbuf+0;
        pStrBuf[1]=pbuf+20;
        pStrBuf[2]=pbuf+40;
        pStrBuf[3]=pbuf+80;
        //pStrBuf[3]=pbuf+60;
        //pStrBuf[4]=pbuf+80;
        //
        /*
        sprintf((char*)&pbuf[0] ,"");
        sprintf((char*)&pbuf[20] ,"");
        sprintf((char*)&pbuf[40] ,"");
        sprintf((char*)&pbuf[80] ,"");
        LISTVIEW_SetRowHeight(Hci_hListView,2);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        */
        //
        sprintf((char*)&pbuf[0] ,"1");
        sprintf((char*)&pbuf[20],pSTR_UTF8_SMR_SLEEP_STRUCT[Main_Language]);
        sprintf((char*)&pbuf[40],"");
        //sprintf((char*)&pbuf[40],"%d",WEIGHTSLEEPSTRCT);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_SleepStruct);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_SleepStruct-WEIGHTSLEEPSTRCT);
        LISTVIEW_SetRowHeight(Hci_hListView,16+2);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        LISTVIEW_SetRowHeight(Hci_hListView,16+1);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"2");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_INTO_SLEEP_TIME[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",Count_2ByteToWord(psDayInfo->FallAsleepTime_m[0],psDayInfo->FallAsleepTime_m[1]));
        //sprintf((char*)&pbuf[40],"%d",WEIGHTSLEEPINTOPERIOD);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mart_SleepIntoPeriod);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mart_SleepIntoPeriod-WEIGHTSLEEPINTOPERIOD);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"3");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_BODY_MOVEMENT_CMT[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",Count_2ByteToWord(psDayInfo->BodyMoveCount[0],psDayInfo->BodyMoveCount[1]));
        //sprintf((char*)&pbuf[40],"%d",WEIGHTBODYMOVEMENTCOUNT);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_BodyMovement);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_BodyMovement-WEIGHTBODYMOVEMENTCOUNT);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"4");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_WAKE_STATE[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",psDayInfo->SleepLevelBeforeAlarm);
        //sprintf((char*)&pbuf[40],"%d",WEIGHTAWAKESTATE);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_WakeState);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_WakeState-WEIGHTAWAKESTATE);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"5");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_BEGIN_SLEEP_TIME[Main_Language]);
        sprintf((char*)&pbuf[40],"%02d:%02d",psDayInfo->BeginTime[4],psDayInfo->BeginTime[5]);
        //sprintf((char*)&pbuf[40],"%d",WEIGHTSTARTTIMEDIS);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_BeginSleepTimeChange);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_BeginSleepTimeChange-WEIGHTSTARTTIMEDIS);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"6");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_TEMP[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",psDayInfo->TempBuf[0]);
        //sprintf((char*)&pbuf[40],"%d",WEIGHTSLEEPTEMP);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_Temp);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_Temp-WEIGHTSLEEPTEMP);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"7");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_HUMI[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",psDayInfo->HumiBuf[0]);
        //sprintf((char*)&pbuf[40],"%d",WEIGHTSLEEPHUM);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mark_Humi);
        sprintf((char*)&pbuf[80],"%d",psDayInfo->Mark_Humi-WEIGHTSLEEPHUM);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        sprintf((char*)&pbuf[0] ,"8");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_TOTAL_SCORE[Main_Language]);
        //sprintf((char*)&pbuf[40],"");
        sprintf((char*)&pbuf[40],"%d",psDayInfo->Mark_SleepStruct\
                +psDayInfo->Mart_SleepIntoPeriod\
                +psDayInfo->Mark_BodyMovement\
                +psDayInfo->Mark_WakeState\
                +psDayInfo->Mark_BeginSleepTimeChange\
                +psDayInfo->Mark_Temp\
                +psDayInfo->Mark_Humi);
        sprintf((char*)&pbuf[80],"");
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        //
        sprintf((char*)&pbuf[0] ,"9");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_TOTAL_SLEEP_TIME[Main_Language]);
        sprintf((char*)&pbuf[40],"%d",(psDayInfo->BodyMoveNum)*3);
        //sprintf((char*)&pbuf[40],"%d",1000);
        //sprintf((char*)&pbuf[60],"%d",psDayInfo->Mart_TotalSleep);
        sprintf((char*)&pbuf[80],"%.2f",((float)(psDayInfo->Mart_TotalSleep))/1000);
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
        //
        sprintf((char*)&pbuf[0] ,"10");
        sprintf((char*)&pbuf[20],(char*)pSTR_UTF8_SMR_FINAL_SCORE[Main_Language]);
        //sprintf((char*)&pbuf[40]," ");
        sprintf((char*)&pbuf[40],"%d",(psDayInfo->Mark_SleepStruct\
                                       +psDayInfo->Mart_SleepIntoPeriod\
                                       +psDayInfo->Mark_BodyMovement\
                                       +psDayInfo->Mark_WakeState\
                                       +psDayInfo->Mark_BeginSleepTimeChange\
                                       +psDayInfo->Mark_Temp\
                                       +psDayInfo->Mark_Humi)*psDayInfo->Mart_TotalSleep/1000);
        sprintf((char*)&pbuf[80],"");
        LISTVIEW_AddRow(Hci_hListView,(const GUI_ConstString*)pStrBuf);
        //GUI_Delay(10);
    }
    //显示日期

    while(1)
    {
        if((BspKey_NewSign==1)&&(BspKey_Value==HCI_KEY_ESC))
        {
            BspKey_NewSign=0;
            Hci_ReturnTimer=0;
            break;
        }
        else
        {
            GUI_Delay(100);
            Hci_ReturnTimer++;
            if(Hci_ReturnTimer>=10*60)
            {
                BspKey_NewSign=0;
                Hci_ReturnTimer=0;
                break;
            }
        }
    }
    //
    LISTVIEW_Delete(Hci_hListView);
    TEXT_Delete(hText);
    //
    //使能桌面窗口使用内存设备
    WM_DisableMemdev(WM_HBKWIN);
    //
    //释放缓存
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,psDayInfo);
}
/*******************************************************************************
* 函数功能: 回调
*******************************************************************************/
#define HCI_MSG_USER_0  (WM_USER + 0)
#define HCI_MSG_USER_1  (WM_USER + 1)
static char _acMainText[100];
/*
static void Hci_SendMsgUser0(char *pstr)
{
    WM_MESSAGE Message;
    Message.MsgId    =  HCI_MSG_USER_0;
    Message.Data.p   =  pstr;
    WM_SendMessage(WM_HBKWIN,&Message);
    WM_InvalidateWindow(WM_HBKWIN);
}
*/
static void Hci_CbBkWindow(WM_MESSAGE* pMsg)
{
    int NCode, Id;
    //WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId)
    {
        //----------系统定义的消息----------
        //窗口创建后立即发送,使窗口可以初始化并创建任何子窗口
        case WM_CREATE:
            break;
        //要删除窗口前发送,告诉窗口释放其数据结构(如果有)
        case WM_DELETE:
            break;
        //看看窗口是否可以聚焦
        case WM_GET_ACCEPT_FOCUS:
            break;
        //请求窗口ID
        case WM_GET_ID:
            break;
        //创建对话框后立即发送到窗口
        case WM_INIT_DIALOG:
            break;
        //按下某个按键后发送到当前包含焦点的窗口
        case WM_KEY:
            switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
            {
                case GUI_KEY_ESCAPE:
                    break;
                case GUI_KEY_ENTER:
                    break;
                //......
                default:
                    break;
            }
            break;
        //窗口移动后立即发送到窗口
        case WM_MOVE:
            break;
        //告知父窗口,其子窗口中发生了某些变化
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc);
            NCode = pMsg->Data.v;
            switch(Id)
            {
                case GUI_ID_OK:
                    break;
                case GUI_ID_CANCEL:
                    break;
                case GUI_ID_BUTTON0:
                    break;
                //......
                default:
                    break;
            }
            switch(NCode)
            {
                //----------通知消息----------------
                // 窗口被删除前,发送消息到父窗口
                case WM_NOTIFICATION_CHILD_DELETED:
                    break;
                // 单击窗口
                case WM_NOTIFICATION_CLICKED:
                    break;
                // 窗口获得焦点
                case WM_NOTIFICATION_GOT_FOCUS:
                    break;
                // 窗口失去焦点
                case WM_NOTIFICATION_LOST_FOCUS:
                    break;
                // 此消息将在移出窗口并点击时发送
                case WM_NOTIFICATION_MOVED_OUT:
                    break;
                // 此消息将在被点击的小工具已被释放时发送
                case WM_NOTIFICATION_RELEASED:
                    break;
                // 此消息将在附加的SCROLLBAR(滚动条)小工具的滚动位置更改时发送
                case WM_NOTIFICATION_SCROLL_CHANGED:
                    break;
                // 此消息将在附加的SCROLLBAR(滚动条)小工具的滚动位置更改时发送
                case WM_NOTIFICATION_SCROLLBAR_ADDED:
                    break;
                // 此消息将在小工具选择已更改时发送
                case WM_NOTIFICATION_SEL_CHANGED:
                    break;
                // 此消息将在小工具特定值已更改时发送
                case WM_NOTIFICATION_VALUE_CHANGED:
                    break;
            }
            break;
        //窗口可见性修改时发送到窗口
        case WM_NOTIFY_VIS_CHANGED:
            break;
        //窗口变为无效并应重绘时,发送到窗口
        case WM_PAINT:
            GUI_SetBkColor(GUI_BLACK);
            GUI_Clear();
            GUI_SetColor(GUI_WHITE);
            GUI_SetFont(&GUI_FontFont_ST_24);
            GUI_DispStringHCenterAt((char*)pSTR_UTF8_SLEEP_MARK_REPORT[Main_Language],160,11-2);
            break;
        //最后一条 WM_PAIN 消息处理后发送到窗口
        case WM_POST_PAINT:
            break;
        //第一条 WM_PAIN 消息发送之前发送到窗口
        case WM_PRE_PAINT:
            break;
        //获取或失去输入焦点时发送到窗口
        case WM_SET_FOCUS:
            break;
        //发送到窗口更改窗口ID
        case WM_SET_ID:
            break;
        //窗口大小更改后发送到窗口
        case WM_SIZE:
            break;
        //定时器到期后发送到窗口
        case WM_TIMER:
            break;
        //----------指针输入设备消息--------
        // 发送到窗口实现运动的支持
        case WM_MOTION:
            break;
        // 指针输入设备接触到窗口轮廓时发送到窗口(仅支持鼠标时发送)
        case WM_MOUSEOVER:
            break;
        // 指针输入设备已移出窗口轮廓时发送到窗口(仅支持鼠标时发送)
        case WM_MOUSEOVER_END:
            break;
        // 按下状态已更改时,发送到指针输入设备指向的窗口
        case WM_PID_STATE_CHANGED:
            break;
        // 指针输入设备接触到处于按下状态的窗口轮廓时发送到窗口
        case WM_TOUCH:
            break;
        // 指针输入设备接触到子窗口时发送到父窗口
        case WM_TOUCH_CHILD:
            break;
        //----------用户消息----------------
        // 应用程序可使用 WM_USER 常数来定义私人消息,通常形式为 WM_USER + X.
        //case WM_USER:
        case HCI_MSG_USER_0:
            strcpy(_acMainText,(char const*)pMsg->Data.p);
            WM_InvalidateWindow(pMsg->hWin);
            break;
        case HCI_MSG_USER_1:
            break;
        //----------------------------------
        default:
            WM_DefaultProc(pMsg);
    }
}
#endif
#elif (defined(PROJECT_ARMFLY_V5_XSL))
#include "bsp_tft_lcd.h"
#include "bsp_touch.h"
#include "uctsk_Debug.h"
#include "Bsp_Camera.h"
#include "Bsp_GpioIIC.h"
//-------------------静态变量--------------------------------
MODULE_OS_TASK_TAB(App_TaskHciTCB);
MODULE_OS_TASK_STK(App_TaskHciStk,APP_TASK_HCI_STK_SIZE);
static void (*pHci_MenuFun)(void);
static void uctsk_Hci(void *pvParameters);
void TestCamera(void);
/*******************************************************************************
函数功能: Hci任务创建
*******************************************************************************/
void  App_HciTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Hci",\
                          uctsk_Hci,\
                          APP_TASK_HCI_PRIO,\
                          App_TaskHciStk,\
                          APP_TASK_HCI_STK_SIZE,\
                          App_TaskHciTCB,\
                          NULL);
}
static void uctsk_Hci (void *pvParameters)
{
    LCD_InitHard();
    TOUCH_InitHard();
    if(ModuleMemory_psPara->Touch_usAdcX1==0xFFFF)
    {
        TOUCH_Calibration();
    }
    pHci_MenuFun=TestCamera;
    for (; ;)
    {
        MODULE_OS_DELAY_MS(10);
        pHci_MenuFun();
    }
}

/* 定义界面结构 */
typedef struct
{
    FONT_T FontBlack;   /* 静态的文字 */
    FONT_T FontBlue;    /* 变化的文字字体 蓝色 */
    FONT_T FontRed;     /* 变化的文字字体 红色 */
    FONT_T FontBtn;     /* 按钮的字体 */
    FONT_T FontBox;     /* 分组框标题字体 */

    GROUP_T Box1;

    LABEL_T Label1;
    LABEL_T Label2; /* 状态 */
    LABEL_T Label3;
    LABEL_T Label4; /* 状态 */
    LABEL_T Label5;
    LABEL_T Label6; /* 状态 */

    BUTTON_T Btn1;      /* 打开摄像头 */
    BUTTON_T Btn2;      /* 暂停 */

    BUTTON_T BtnRet;

} FormCAM_T;

/* 窗体背景色 */
#define FORM_BACK_COLOR     CL_BTN_FACE

/* 4个框的坐标和大小 */
#define BOX1_X  5
#define BOX1_Y  5
#define BOX1_H  (g_LcdHeight - BOX1_Y - 10)
#define BOX1_W  (g_LcdWidth -  2 * BOX1_X)
#define BOX1_TEXT   "摄像头0V7670测试程序"

/* 返回按钮的坐标(屏幕右下角) */
#define BTN_RET_H   32
#define BTN_RET_W   60
#define BTN_RET_X   ((BOX1_X + BOX1_W) - BTN_RET_W - 4)
#define BTN_RET_Y   ((BOX1_Y  + BOX1_H) - BTN_RET_H - 4)
#define BTN_RET_TEXT    "返回"

#define BTN1_H  32
#define BTN1_W  100
#define BTN1_X  (BOX1_X + 330)
#define BTN1_Y  (BOX1_Y + 100)
#define BTN1_TEXT   "打开摄像头"

#define BTN2_H  32
#define BTN2_W  100
#define BTN2_X  BTN1_X
#define BTN2_Y  (BTN1_Y + BTN1_H + 10)
#define BTN2_TEXT   "关闭摄像头"

/* 标签 */
#define LABEL1_X    (BOX1_X + 330)
#define LABEL1_Y    (BOX1_Y + 20)
#define LABEL1_TEXT "Chip ID : "

#define LABEL2_X    (LABEL1_X + 80)
#define LABEL2_Y    LABEL1_Y
#define LABEL2_TEXT "--"

#define LABEL3_X    (LABEL1_X)
#define LABEL3_Y    (LABEL1_Y + 20)
#define LABEL3_TEXT "状态1   : "

#define LABEL4_X    (LABEL3_X + 80)
#define LABEL4_Y    (LABEL3_Y)
#define LABEL4_TEXT "--"

#define LABEL5_X    (LABEL1_X)
#define LABEL5_Y    (LABEL1_Y + 20 * 2)
#define LABEL5_TEXT "状态2   : "

#define LABEL6_X    (LABEL5_X + 80)
#define LABEL6_Y    (LABEL5_Y)
#define LABEL6_TEXT "--"

/* 摄像显示窗口位置和大小 */
#define PHOTO_X 10
#define PHOTO_Y 22
#define PHOTO_H 240
#define PHOTO_W 320

static void InitFormCam(void);
static void DispCamInitFace(void);

FormCAM_T *FormCam;

/*
*********************************************************************************************************
*   函 数 名: TestCamera
*   功能说明: 测试摄像头
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
void TestCamera(void)
{
    //uint8_t ucKeyCode;        /* 按键代码 */
    uint8_t ucTouch;        /* 触摸事件 */
    uint8_t fQuit = 0;
    int16_t tpX, tpY;
    uint16_t usChipID;
    char *pbuf;

    FormCAM_T *pform;
    uint8_t fRefresh;
    //
    //uint32_t *pi32;
    //
    pbuf =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    pform=  MemManager_Get(E_MEM_MANAGER_TYPE_1KB);
    //pi32 =    (uint32_t*)&pbuf[128];
    // 清除无效的触摸事件
    TOUCH_CelarFIFO();

    FormCam = pform;
    // 初始化界面变量
    InitFormCam();
    // 初始化摄像头
    bsp_InitCamera();
    // 初始化界面
    DispCamInitFace();
    // 读取OV7670信息
    if (BspGpioIIC_CheckDevice(OV7670_SLAVE_ADDRESS) == 0)
    {
        usChipID = OV_ReadID();
        sprintf(pbuf, "0x%04X", usChipID);

        FormCam->Label2.Font = &FormCam->FontBlue;
        FormCam->Label2.pCaption = pbuf;
    }
    else
    {
        sprintf(pbuf, "None  ");

        FormCam->Label2.Font = &FormCam->FontRed;
        FormCam->Label2.pCaption = pbuf;
    }
    // 更新控件
    LCD_DrawLabel(&FormCam->Label2);
    // 画画布
    LCD_DrawRect(PHOTO_X - 1, PHOTO_Y - 1, PHOTO_H + 2, PHOTO_W + 2, CL_RED);

    fRefresh = 1;

    // 进入主程序循环体
    while (fQuit == 0)
    {
        // 首次处理
        if (fRefresh)
        {
            fRefresh = 0;
        }
        // 如果图像捕获完成，则退出绘图模式
        if (g_tCam.CaptureOk == 1)
        {
            g_tCam.CaptureOk = 0;

            RA8875_QuitDirectDraw();
            g_tTP.Enable = 1;
            // 启动定时器
            Count_SoftTimer_Start(0, 50);
        }
        // 如果定时时间到达，则启动绘图模式
        if(Count_SoftTimer_Check(0))
        {
            g_tTP.Enable = 0;

            RA8875_StartDirectDraw(PHOTO_X, PHOTO_Y, PHOTO_H, PHOTO_W);
            CAM_Start(RA8875_GetDispMemAddr());
        }

        ucTouch = TOUCH_GetKey(&tpX, &tpY); /* 读取触摸事件 */
        if (ucTouch != TOUCH_NONE)
        {
            switch (ucTouch)
            {
                case TOUCH_DOWN:        /* 触笔按下事件 */
                    if (TOUCH_InRect(tpX, tpY, BTN_RET_X, BTN_RET_Y, BTN_RET_H, BTN_RET_W))
                    {
                        FormCam->BtnRet.Focus = 1;
                        LCD_DrawButton(&FormCam->BtnRet);
                    }
                    else if (TOUCH_InRect(tpX, tpY, BTN1_X, BTN1_Y, BTN1_H, BTN1_W))
                    {
                        FormCam->Btn1.Focus = 1;
                        LCD_DrawButton(&FormCam->Btn1);
                    }
                    else if (TOUCH_InRect(tpX, tpY, BTN2_X, BTN2_Y, BTN2_H, BTN2_W))
                    {
                        FormCam->Btn2.Focus = 1;
                        LCD_DrawButton(&FormCam->Btn2);
                    }
                    break;

                case TOUCH_MOVE:        /* 触笔移动事件 */
                    break;

                case TOUCH_RELEASE:     /* 触笔释放事件 */
                    if (TOUCH_InRect(tpX, tpY, BTN_RET_X, BTN_RET_Y, BTN_RET_H, BTN_RET_W))
                    {
                        FormCam->BtnRet.Focus = 0;
                        LCD_DrawButton(&FormCam->BtnRet);
                        fQuit = 1;  /* 返回 */
                    }
                    else if (TOUCH_InRect(tpX, tpY, BTN1_X, BTN1_Y, BTN1_H, BTN1_W))
                    {
                        FormCam->Btn1.Focus = 0;
                        LCD_DrawButton(&FormCam->Btn1);

                        {
                            if (BspGpioIIC_CheckDevice(OV7670_SLAVE_ADDRESS) == 0)
                            {
                                usChipID = OV_ReadID();
                                sprintf(pbuf, "0x%04X", usChipID);

                                FormCam->Label2.Font = &FormCam->FontBlue;
                                FormCam->Label2.pCaption = pbuf;
                            }
                            else
                            {
                                sprintf(pbuf, "None  ");

                                FormCam->Label2.Font = &FormCam->FontRed;
                                FormCam->Label2.pCaption = pbuf;
                            }
                            LCD_DrawLabel(&FormCam->Label2);
                        }

                        {
                            g_tTP.Enable = 0;

                            RA8875_StartDirectDraw(PHOTO_X, PHOTO_Y, PHOTO_H, PHOTO_W);
                            CAM_Start(RA8875_GetDispMemAddr());
                        }

                        fRefresh = 1;
                    }
                    else if (TOUCH_InRect(tpX, tpY, BTN2_X, BTN2_Y, BTN2_H, BTN2_W))
                    {
                        FormCam->Btn2.Focus = 0;
                        LCD_DrawButton(&FormCam->Btn2);
                        // 停止自动定时器
                        Count_SoftTimer_Stop(0);
                        fRefresh = 1;
                    }
                    else    /* 按钮失去焦点 */
                    {
                        FormCam->BtnRet.Focus = 0;
                        LCD_DrawButton(&FormCam->BtnRet);

                        FormCam->Btn1.Focus = 0;
                        LCD_DrawButton(&FormCam->Btn1);

                        FormCam->Btn2.Focus = 0;
                        LCD_DrawButton(&FormCam->Btn2);
                    }
                    break;
            }
        }

#if 0
        /* 处理按键事件 */
        ucKeyCode = bsp_GetKey();
        if (ucKeyCode > 0)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_DOWN_K1:       /* K1键 */
                    break;

                case KEY_DOWN_K2:       /* K2键按下 */
                    break;

                case KEY_DOWN_K3:       /* K3键按下 */
                    break;

                case JOY_DOWN_U:        /* 摇杆UP键按下 */
                    break;

                case JOY_DOWN_D:        /* 摇杆DOWN键按下 */
                    break;

                case JOY_DOWN_L:        /* 摇杆LEFT键按下 */
                    break;

                case JOY_DOWN_R:        /* 摇杆RIGHT键按下 */
                    break;

                case JOY_DOWN_OK:       /* 摇杆OK键按下 */
                    CAM_Stop();
                    RA8875_QuitDirectDraw();
                    g_tTP.Enable = 1;
                    break;

                default:
                    break;
            }
        }
#endif
    }

    //
    MemManager_Free(E_MEM_MANAGER_TYPE_256B, pbuf);
    MemManager_Free(E_MEM_MANAGER_TYPE_1KB, pform);
    //
    CAM_Stop();
    RA8875_QuitDirectDraw();
    g_tTP.Enable = 1;
}

/*
*********************************************************************************************************
*   函 数 名: InitFormCam
*   功能说明: 初始化GPS初始界面控件
*   形    参：无
*   返 回 值: 无
*********************************************************************************************************
*/
static void InitFormCam(void)
{
    /* 分组框标题字体 */
    FormCam->FontBox.FontCode = FC_ST_16;
    FormCam->FontBox.BackColor = CL_BTN_FACE;   /* 和背景色相同 */
    FormCam->FontBox.FrontColor = CL_BLACK;
    FormCam->FontBox.Space = 0;

    /* 字体1 用于静止标签 */
    FormCam->FontBlack.FontCode = FC_ST_16;
    FormCam->FontBlack.BackColor = CL_MASK;     /* 透明色 */
    FormCam->FontBlack.FrontColor = CL_BLACK;
    FormCam->FontBlack.Space = 0;

    /* 字体2 用于变化的文字 */
    FormCam->FontBlue.FontCode = FC_ST_16;
    FormCam->FontBlue.BackColor = CL_BTN_FACE;
    FormCam->FontBlue.FrontColor = CL_BLUE;
    FormCam->FontBlue.Space = 0;

    /* 字体3 用于变化的文字 */
    FormCam->FontRed.FontCode = FC_ST_16;
    FormCam->FontRed.BackColor = CL_BTN_FACE;
    FormCam->FontRed.FrontColor = CL_RED;
    FormCam->FontRed.Space = 0;

    /* 按钮字体 */
    FormCam->FontBtn.FontCode = FC_ST_16;
    FormCam->FontBtn.BackColor = CL_MASK;       /* 透明背景 */
    FormCam->FontBtn.FrontColor = CL_BLACK;
    FormCam->FontBtn.Space = 0;

    /* 分组框 */
    FormCam->Box1.Left = BOX1_X;
    FormCam->Box1.Top = BOX1_Y;
    FormCam->Box1.Height = BOX1_H;
    FormCam->Box1.Width = BOX1_W;
    FormCam->Box1.pCaption = BOX1_TEXT;
    FormCam->Box1.Font = &FormCam->FontBox;

    /* 静态标签 */
    FormCam->Label1.Left = LABEL1_X;
    FormCam->Label1.Top = LABEL1_Y;
    FormCam->Label1.MaxLen = 0;
    FormCam->Label1.pCaption = LABEL1_TEXT;
    FormCam->Label1.Font = &FormCam->FontBlack;

    FormCam->Label3.Left = LABEL3_X;
    FormCam->Label3.Top = LABEL3_Y;
    FormCam->Label3.MaxLen = 0;
    FormCam->Label3.pCaption = LABEL3_TEXT;
    FormCam->Label3.Font = &FormCam->FontBlack;

    FormCam->Label5.Left = LABEL5_X;
    FormCam->Label5.Top = LABEL5_Y;
    FormCam->Label5.MaxLen = 0;
    FormCam->Label5.pCaption = LABEL5_TEXT;
    FormCam->Label5.Font = &FormCam->FontBlack;

    /* 动态标签 */
    FormCam->Label2.Left = LABEL2_X;
    FormCam->Label2.Top = LABEL2_Y;
    FormCam->Label2.MaxLen = 0;
    FormCam->Label2.pCaption = LABEL2_TEXT;
    FormCam->Label2.Font = &FormCam->FontBlue;

    FormCam->Label4.Left = LABEL4_X;
    FormCam->Label4.Top = LABEL4_Y;
    FormCam->Label4.MaxLen = 0;
    FormCam->Label4.pCaption = LABEL4_TEXT;
    FormCam->Label4.Font = &FormCam->FontBlue;

    FormCam->Label6.Left = LABEL6_X;
    FormCam->Label6.Top = LABEL6_Y;
    FormCam->Label6.MaxLen = 0;
    FormCam->Label6.pCaption = LABEL6_TEXT;
    FormCam->Label6.Font = &FormCam->FontBlue;

    /* 按钮 */
    FormCam->BtnRet.Left = BTN_RET_X;
    FormCam->BtnRet.Top = BTN_RET_Y;
    FormCam->BtnRet.Height = BTN_RET_H;
    FormCam->BtnRet.Width = BTN_RET_W;
    FormCam->BtnRet.pCaption = BTN_RET_TEXT;
    FormCam->BtnRet.Font = &FormCam->FontBtn;
    FormCam->BtnRet.Focus = 0;

    FormCam->Btn1.Left = BTN1_X;
    FormCam->Btn1.Top = BTN1_Y;
    FormCam->Btn1.Height = BTN1_H;
    FormCam->Btn1.Width = BTN1_W;
    FormCam->Btn1.pCaption = BTN1_TEXT;
    FormCam->Btn1.Font = &FormCam->FontBtn;
    FormCam->Btn1.Focus = 0;

    FormCam->Btn2.Left = BTN2_X;
    FormCam->Btn2.Top = BTN2_Y;
    FormCam->Btn2.Height = BTN2_H;
    FormCam->Btn2.Width = BTN2_W;
    FormCam->Btn2.pCaption = BTN2_TEXT;
    FormCam->Btn2.Font = &FormCam->FontBtn;
    FormCam->Btn2.Focus = 0;
}

/*
*********************************************************************************************************
*   函 数 名: DispUSBInitFace
*   功能说明: 显示所有的控件
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void DispCamInitFace(void)
{
    LCD_ClrScr(CL_BTN_FACE);

    /* 分组框 */
    LCD_DrawGroupBox(&FormCam->Box1);

    /* 标签 */
    LCD_DrawLabel(&FormCam->Label1);
    LCD_DrawLabel(&FormCam->Label2);
    LCD_DrawLabel(&FormCam->Label3);
    LCD_DrawLabel(&FormCam->Label4);
    LCD_DrawLabel(&FormCam->Label5);
    LCD_DrawLabel(&FormCam->Label6);

    /* 按钮 */
    LCD_DrawButton(&FormCam->BtnRet);
    LCD_DrawButton(&FormCam->Btn1);
    LCD_DrawButton(&FormCam->Btn2);
}


#elif (defined(XKAP_ICARE_B_M))
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_Hx711.h"
#include "Bsp_BuzzerMusic.h"
#include "Bsp_Rtc.h"
#include "Module_Memory.h"
#include "uctsk_Debug.h"
#include "uctsk_AD.h"
#include "Bsp_Ap3216c.h"
#if   (defined(HAIER))
#include "Bsp_NrfBle.h"
#else
#include "uctsk_GprsAppXkap.h"
#endif
static uint8_t Hci_timer=0;
static uint8_t Hci_AlarmFunction_OnOff=ON;
//bit0   最终结果 0-离床1-在床
//bit1   最终结果更新
static INT8_S_BIT Hci_AbedOrAway= {0};
static uint16_t Hci_light_cmt=0;
//
static uint16_t Hci_OffSet_TimerS=0;
//
static MODULE_MEMORY_S_WEIGHT Hci_sWeight= {0};
//
static void Hci_Alarm_OnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        if(Hci_AlarmFunction_OnOff==OFF)
        {
            return;
        }
        BSP_LED_LED2_ON;
        BSP_LED_LED1_ON;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_MORNING,0xFFFF);
#if   (defined(HAIER))
        // 广播
        if(BspNrfBle_s_Tx.state==0)
        {
            BspNrfBle_s_Tx.buf[0]=  2;
            BspNrfBle_s_Tx.buf[1]=  0x82;
            BspNrfBle_s_Tx.buf[2]=  4;
            BspNrfBle_s_Tx.state =  1;
        }
#else
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=1;
            GprsAppXkap_S_Info.Value[0]=1;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
#endif
    }
    else
    {
        BSP_LED_LED2_OFF;
        BSP_LED_LED1_OFF;
        Bsp_BuzzerMusic_PlayStop(OFF,NULL,0);
#if   (defined(HAIER))
        // 广播
        if(BspNrfBle_s_Tx.state==0)
        {
            BspNrfBle_s_Tx.buf[0]=  2;
            BspNrfBle_s_Tx.buf[1]=  0x82;
            BspNrfBle_s_Tx.buf[2]=  5;
            BspNrfBle_s_Tx.state =  1;
        }
#else
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=1;
            GprsAppXkap_S_Info.Value[0]=0;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
#endif
    }
}
static void Hci_AlarmFunc_OnOff(uint8_t OnOff)
{
    if(OnOff==OFF)
    {
        Hci_AlarmFunction_OnOff=OFF;
#if   (defined(HAIER))
        // 广播
        if(BspNrfBle_s_Tx.state==0)
        {
            BspNrfBle_s_Tx.buf[0]=  2;
            BspNrfBle_s_Tx.buf[1]=  0x82;
            BspNrfBle_s_Tx.buf[2]=  7;
            BspNrfBle_s_Tx.state =  1;
        }
#else
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=2;
            GprsAppXkap_S_Info.Value[0]=0;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
#endif
    }
    else if(OnOff==ON)
    {
        Hci_AlarmFunction_OnOff=ON;
#if   (defined(HAIER))
        // 广播
        if(BspNrfBle_s_Tx.state==0)
        {
            BspNrfBle_s_Tx.buf[0]=  2;
            BspNrfBle_s_Tx.buf[1]=  0x82;
            BspNrfBle_s_Tx.buf[2]=  6;
            BspNrfBle_s_Tx.state =  1;
        }
#else
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=2;
            GprsAppXkap_S_Info.Value[0]=1;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
#endif
    }
}
void Hci_100ms(void)
{
    static uint8_t Hci_cmd=0;
    static uint8_t Hci_step=0xFF;
    static uint8_t s_led_cmt=0;
    static uint8_t s_count=0;
    static uint8_t s_keyBell=0;
    static INT8_S_BIT s_state= {0};     //bit0-sos  bit1-WeightMemory
    static uint8_t s_hx711_up_count=0;
    static uint8_t s_weight_memmory_cmt=0;
    uint16_t i16;
    uint32_t i32;
    if(Hci_step==0xFF)
    {
        s_led_cmt++;
        if(s_led_cmt==1)
        {
            //
            {
                MODULE_MEMORY_S_PARA *pPara;
                BspHx711_Get_ad(&i32,10);
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                pPara->ThreshsholdValue2=i32;
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            }
            //
            BspLed_Mode(1,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(2,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(3,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(4,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_LOW);
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_L,0);
        }
        else if(s_led_cmt>=20)
        {
            BspLed_Mode(1,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(2,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(3,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(4,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
            //
            BspLed_Mode(3,BSP_LED_E_MODE_PWM,BSP_LED_E_LEVEL_LOW);
            BspLed_Mode(4,BSP_LED_E_MODE_PWM,BSP_LED_E_LEVEL_LOW);
            //
            Hci_step=0;
            s_led_cmt=0;
        }
        return;
    }
    //
    if(GprsApp_RxLastToNowS>30)
    {
        BspLed_Mode(1,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_4);
        BspLed_Mode(2,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_4);
        BspLed_Mode(3,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_4);
        BspLed_Mode(4,BSP_LED_E_MODE_FLICK,BSP_LED_E_LEVEL_4);
    }
    else
    {
    	BspLed_Mode(1,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_4);
        BspLed_Mode(2,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_4);
        BspLed_Mode(3,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_4);
        BspLed_Mode(4,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_4);
    }
    //
    if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]==0xFFFF || BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]==0)
    {
        if(s_keyBell!=0)
        {
            BspLed_Mode(1,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_3);
            BspLed_Mode(2,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_3);
            BspLed_Mode(3,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_3);
            BspLed_Mode(4,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_3);
        }
        switch(s_keyBell)
        {
            case 1:
                if(Hci_cmd==0)
                {
                    // 绌哄簥鏍″噯
                    Hci_cmd=1;
                }
                break;
            case 2:
                if(Hci_cmd==0)
                {
                    // 鍦ㄥ簥鏍″噯
                    Hci_cmd=8;
                }
                break;
            case 3:
                if(Hci_cmd==0)
                {
                    // 浣胯兘/绂佺敤澶滅伅led
                    Hci_cmd=6;
                }
                break;
            case 4:
                if(Hci_cmd==0)
                {
                    // 閲囬泦鏁版嵁鍒癴lash
                    Hci_cmd=7;
                }
                break;
            default:
                break;
        }
        s_keyBell=0;
        //
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>1*100 && s_keyBell==0)
    {
        s_keyBell=1;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
        //
        BspLed_Mode(1,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_3);
        BspLed_Mode(2,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_3);
        BspLed_Mode(3,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_3);
        BspLed_Mode(4,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_3);
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>3*100 && s_keyBell==1)
    {
        s_keyBell=2;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BB,0);
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>5*100 && s_keyBell==2)
    {
        s_keyBell=3;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BBB,0);
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>7*100 && s_keyBell==3)
    {
        s_keyBell=4;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BBBB,0);
    }
    else if(BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]>9*100 && s_keyBell==4)
    {
        s_keyBell=5;
        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_L,0);
    }
    //按键处理
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        switch(BspKey_Value)
        {
            case HCI_KEY_SOS_1:
                if(Hci_cmd==0)
                {
                    Hci_cmd=2;
                }
                DebugOutStr((int8_t*)"KEY-SOS-1\r\n");
                break;
            case HCI_KEY_SOS_2:
                if(Hci_cmd==0)
                {
                    Hci_cmd=3;
                }
                DebugOutStr((int8_t*)"KEY-SOS-2\r\n");
                break;
            case HCI_KEY_SOS_3:
                if(Hci_cmd==0)
                {
                    Hci_cmd=4;
                }
                DebugOutStr((int8_t*)"KEY-SOS-3\r\n");
                break;
            case HCI_KEY_SOS_4:
                if(Hci_cmd==0)
                {
                    Hci_cmd=5;
                }
                DebugOutStr((int8_t*)"KEY-SOS-4\r\n");
                break;
#if   (defined(HAIER))
            case HCI_KEY_ENTER:
                // 设备校准
                if(BspKey_KeepTimer_ms>=8000)
                {

                    if(Hci_AlarmFunction_OnOff==ON)
                    {
                        Hci_AlarmFunc_OnOff(OFF);
                    }
                    else
                    {
                        Hci_AlarmFunc_OnOff(ON);
                    }
                }
                // 在床离床
                else if(BspKey_KeepTimer_ms>=6000)
                {
                    if(Hci_AbedOrAway.bit0==1)
                    {
                        // 离床
                        Hci_AbedOrAway.bit0=0;
                        if(BspNrfBle_s_Tx.state==0)
                        {
                            BspNrfBle_s_Tx.buf[0]=  2;
                            BspNrfBle_s_Tx.buf[1]=  0x82;
                            BspNrfBle_s_Tx.buf[2]=  2;
                            BspNrfBle_s_Tx.state =  1;
                        }
                    }
                    else
                    {
                        // 在床
                        Hci_AbedOrAway.bit0=1;
                        if(BspNrfBle_s_Tx.state==0)
                        {
                            BspNrfBle_s_Tx.buf[0]=  2;
                            BspNrfBle_s_Tx.buf[1]=  0x82;
                            BspNrfBle_s_Tx.buf[2]=  1;
                            BspNrfBle_s_Tx.state =  1;
                        }

                    }
                }
                // 开关报警
                else if(BspKey_KeepTimer_ms>=3000)
                {
                    if(Hci_cmd==0)
                    {
                        Hci_cmd=1;
                    }
                }
                // 开关报警功能
                else
                {
                    BspKey_PressKeepTimerBuf_10ms[HCI_KEY_ENTER-1]=0xFFFF;
                    //报警功能未开启,则退出
                    if(Hci_AlarmFunction_OnOff==OFF)
                    {
                        break;
                    }
                    //启动报警
                    if(BSP_LED_LED2_READ)
                    {
                        Hci_Alarm_OnOff(ON);
                    }
                    //关闭报警
                    else
                    {
                        Hci_Alarm_OnOff(OFF);
                    }

                }
                break;
#endif
            default:
                break;
        }
    }


    {
#if   (defined(HAIER))
        static uint8_t cmt=0;
        static uint8_t state=0xFF; //0xFF-初始 0-离床 1-在床
        // 采集
        BspHx711_Get_g(&i32,NULL);
        //初始状态判定
        if(state==0xFF)
        {
            if(i32>1000)
            {
                state=1;
            }
            else
            {
                state=0;
            }
        }
        //判断在床与离床
        if(i32>1000&&state==0)
        {
            if(cmt==0 && GprsAppXkap_S_Info.state!=1)
            {
                cmt=1;
                GprsAppXkap_S_Info.state=1;
                GprsAppXkap_S_Info.Tag=4;
                GprsAppXkap_S_Info.Value[0]=1;
                memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
                state=1;
            }
        }
        else if(i32<=1000&&state==1)
        {
            if(cmt==0 && GprsAppXkap_S_Info.state!=1)
            {
                cmt=1;
                GprsAppXkap_S_Info.state=1;
                GprsAppXkap_S_Info.Tag=4;
                GprsAppXkap_S_Info.Value[0]=0;
                memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
                state=0;
            }
        }
        //去抖
        if(cmt>=1)
        {
            cmt++;
            if(cmt>2)
            {
                cmt=0;
            }
        }
#else
        BspHx711_Get_ad(&i32,1);
        // 如果当前是离床，判断在床
        if(Hci_AbedOrAway.bit0==0)
        {
            if( ((i32>=(ModuleMemory_psPara->ThreshsholdValue2+(4300L*ModuleMemory_psPara->Offset_H[0])))&&(0==((ModuleMemory_psPara->ThreshsholdValue2)&0x80000000)))
                ||((i32>=(ModuleMemory_psPara->ThreshsholdValue2-0x80000000-(4300L*ModuleMemory_psPara->Offset_H[1])))&&(0!=((ModuleMemory_psPara->ThreshsholdValue2)&0x80000000))))
            {
                Hci_OffSet_TimerS++;
            }
            else
            {
                Hci_OffSet_TimerS=0;
            }
            if(Hci_OffSet_TimerS>=ModuleMemory_psPara->Offset_TimerS)
            {
                Hci_AbedOrAway.bit0=1;
                Hci_AbedOrAway.bit1=1;
                Hci_OffSet_TimerS=0;
            }
        }
        // 如果当前是在床，判断离床
        else
        {
            if( ((i32<=(ModuleMemory_psPara->ThreshsholdValue2+(4300*ModuleMemory_psPara->Offset_L[0])))&&(0==((ModuleMemory_psPara->ThreshsholdValue2)&0x80000000)))
                ||((i32<=(ModuleMemory_psPara->ThreshsholdValue2-0x80000000-(4300*ModuleMemory_psPara->Offset_L[1])))&&(0!=((ModuleMemory_psPara->ThreshsholdValue2)&0x80000000))))
            {
                Hci_OffSet_TimerS++;
            }
            else
            {
                Hci_OffSet_TimerS=0;
            }
            if(Hci_OffSet_TimerS>=ModuleMemory_psPara->Offset_TimerS)
            {
                Hci_AbedOrAway.bit0=0;
                Hci_AbedOrAway.bit1=1;
                Hci_OffSet_TimerS=0;
            }
        }
#endif
        //存储原始数据
        //---正在调试Flash || RTC没有网络同步,则不存储
        if(memory_WeightLock==1/*||GprsApp_Xkap_RtcReady==0*/||s_state.bit1==0)
        {
            ;
        }
        else
        {
            Hci_sWeight.Hx711[s_weight_memmory_cmt]         =   i32;
            AD_ReadValue(OFF,1,&i16);
            Hci_sWeight.AdValue[0][s_weight_memmory_cmt]    =   i16;
            AD_ReadValue(OFF,2,&i16);
            Hci_sWeight.AdValue[1][s_weight_memmory_cmt]    =   i16;
            AD_ReadValue(OFF,3,&i16);
            Hci_sWeight.AdValue[2][s_weight_memmory_cmt]    =   i16;
            AD_ReadValue(OFF,4,&i16);
            Hci_sWeight.AdValue[3][s_weight_memmory_cmt]    =   i16;
            s_weight_memmory_cmt++;
            if(s_weight_memmory_cmt>=10)
            {
                s_weight_memmory_cmt    =   0;
                //
                memcpy((char*)(Hci_sWeight.DateTime),(char*)BspRtc_CurrentTimeBuf,6);
                //存储
                Module_Memory_App(MODULE_MEMORY_APP_CMD_WEIGHT_W,(uint8_t*)&Hci_sWeight,NULL);
            }
            //pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            //sprintf((char*)pbuf,"BspHx711:%03d %05d\r\n",BspHx711_sWeight.num,i32);
            //DebugLogOutStr(DEBUG_E_LEVEL_DEBUG,(int8_t*)pbuf);
            //MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
        }
    }
    if(AD_DebugTest_Enable==1)
    {
        uint16_t *pi16;
        char *pbuf=NULL;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        pi16=(uint16_t*)&pbuf[128];
        AD_ReadValue(OFF,0,pi16);
        sprintf(pbuf,"%05d,%05d,%05d,%05d,%05d\r\n",pi16[0],pi16[1],pi16[2],pi16[3],pi16[4]);
        DebugOutStr((int8_t*)pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    // 浠诲姟鍫嗘爤涓诲姩婧㈠嚭娴嬭瘯
    if(0)
    {
        void StackOverflowTest(void);
        StackOverflowTest();
    }
    //照明灯条件判断
    //在床报警关灯
    if(Hci_AbedOrAway.bit0==1 && Hci_AbedOrAway.bit1==1 )
    {
        Hci_AbedOrAway.bit1   =  0;
        Hci_light_cmt         =  0;
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=4;
            GprsAppXkap_S_Info.Value[0]=1;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
    }
    //离床报警开灯
    else if(Hci_AbedOrAway.bit0==0 && Hci_AbedOrAway.bit1==1 )
    {
        Hci_AbedOrAway.bit1   =  0;
        if(GprsAppXkap_S_Info.state!=1)
        {
            GprsAppXkap_S_Info.state=1;
            GprsAppXkap_S_Info.Tag=4;
            GprsAppXkap_S_Info.Value[0]=0;
            memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
        }
        if(ModuleMemory_psPara->Flag_NightLight==ON)
        {
            BspAp3216c_Read(&i16,NULL,NULL);
            if(i16<=ModuleMemory_psPara->ThreshsholdValue3)
            {
                Hci_light_cmt         =  ModuleMemory_psPara->Reponse_TimerS;
            }
        }
    }
    //照明灯控制
    s_count++;
    if(s_count>=10)
    {
        s_count=0;
        //AD调试
        //
        if(s_hx711_up_count!=0xff)
        {
            s_hx711_up_count++;
        }
        if(s_hx711_up_count>=60)
        {
            if(GprsAppXkap_S_Info.state!=1)
            {
                s_hx711_up_count=s_hx711_up_count-60;
                BspHx711_Get_g(NULL,&i32);
                GprsAppXkap_S_Info.state=1;
                GprsAppXkap_S_Info.Tag=7;
                GprsAppXkap_S_Info.Value[0]=i32>>24;
                GprsAppXkap_S_Info.Value[1]=i32>>16;
                GprsAppXkap_S_Info.Value[2]=i32>>8;
                GprsAppXkap_S_Info.Value[3]=i32>>0;
                memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
            }
        }
        //
        if(Hci_light_cmt==0)
        {
            BspLed_Mode(1,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
        }
        else
        {
            BspAp3216c_Read(&i16,NULL,NULL);
            if(i16<=ModuleMemory_psPara->ThreshsholdValue3)
            {
                BspLed_Mode(1,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_LOW);
            }
            else
            {
                BspLed_Mode(1,BSP_LED_E_MODE_OFF,BSP_LED_E_LEVEL_LOW);
                Hci_light_cmt=0;
            }
        }
        if((0<Hci_light_cmt)&&(Hci_light_cmt<0xFFFF))
        {
            Hci_light_cmt--;
        }
    }
    //命令解析
    if(Hci_cmd==0)
    {
        s_led_cmt = 0;
        if(s_led_cmt>3)
        {
            ;
        }
        else
        {
            Hci_timer++;
            if(Hci_timer==1)
            {
                //BSP_LED_LED3_ON;
            }
            else if(Hci_timer==2)
            {
                //BSP_LED_LED3_OFF;
                s_led_cmt++;
            }
            else if((Hci_timer==10&&Hci_AlarmFunction_OnOff==ON)\
                    ||(Hci_timer==20&&Hci_AlarmFunction_OnOff==OFF)\
                    ||(Hci_timer>20))
            {
                Hci_timer=0;
            }
        }
    }
    // 绌哄簥鏍″噯 鎴?鍦ㄥ簥鏍″噯
    else if(Hci_cmd==1 || Hci_cmd==8)
    {
        switch(Hci_step)
        {
            case 0:
            {
                uint32_t i32;
                MODULE_MEMORY_S_PARA *pPara;
                BspHx711_Get_ad(&i32,10);
                pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
                if(Hci_cmd==1)
                {
                    pPara->ThreshsholdValue2=i32;
                }
                else
                {
                    pPara->ThreshsholdValue2=i32|0x80000000;
                }
                Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
                MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
            }
#if   (defined(HAIER))
            if(BspNrfBle_s_Tx.state==0)
            {
                BspNrfBle_s_Tx.buf[0]=  2;
                BspNrfBle_s_Tx.buf[1]=  0x82;
                BspNrfBle_s_Tx.buf[2]=  3;
                BspNrfBle_s_Tx.state =  1;
            }
#else
            if(GprsAppXkap_S_Info.state!=1)
            {
                GprsAppXkap_S_Info.state=1;
                GprsAppXkap_S_Info.Tag=3;
                GprsAppXkap_S_Info.Value[0]=0;
                memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
            }
#endif
            Hci_timer=0;
            Hci_step++;
            break;
            case 1:
                if(Hci_timer<50)
                {
                    Hci_timer++;
                }
                else
                {
                    Hci_step++;
                }
                break;
            default:
                BspLed_Mode(1,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                BspLed_Mode(2,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                BspLed_Mode(3,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                Hci_step=  0;
                Hci_cmd =  0;
                break;
        }
    }
    // SOS鎸夐敭浜嬩欢
    else if(Hci_cmd==2 || Hci_cmd==3 || Hci_cmd==4 || Hci_cmd==5)
    {
        switch(Hci_step)
        {
            case 0:
                if(Hci_cmd==2)
                {
                    if(s_state.bit0==0)
                    {
                        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_TYSG,0xFFFF);
                        BspLed_Mode(1,BSP_LED_E_MODE_ON,BSP_LED_E_LEVEL_HIGH);
                        BspLed_Mode(4,BSP_LED_E_MODE_FLICK,BSP_LED_E_LEVEL_HIGH);
                        s_state.bit0=1;
                    }
                    else
                    {
                        Bsp_BuzzerMusic_PlayStop(OFF,NULL,0);
                        BspLed_Mode(1,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_HIGH);
                        BspLed_Mode(4,BSP_LED_E_MODE_NULL,BSP_LED_E_LEVEL_HIGH);
                        s_state.bit0=0;
                    }
                }
                else if(Hci_cmd==3 && s_state.bit0==0)
                {
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
                    BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                }
                else if(Hci_cmd==4 && s_state.bit0==0)
                {
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
                    //Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BB,0);
                    BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                }
                else if(Hci_cmd==5 && s_state.bit0==0)
                {
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
                    //Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_BBB,0);
                    BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
                }

                if(GprsAppXkap_S_Info.state!=1)
                {
                    GprsAppXkap_S_Info.state=1;
                    GprsAppXkap_S_Info.Tag=(Hci_cmd<<4)+3;
                    GprsAppXkap_S_Info.Value[0]=0;
                    memcpy(GprsAppXkap_S_Info.DateTime,BspRtc_CurrentTimeBuf,6);
                }
                Hci_timer=0;
                Hci_step++;
                break;
            case 1:
                if(Hci_timer<3)
                {
                    Hci_timer++;
                }
                else
                {
                    Hci_step++;
                }
                break;
            default:
                Hci_step=  0;
                Hci_cmd =  0;
                break;
        }
    }
    // 澶滅伅寮€鍏?
    else if(Hci_cmd==6)
    {
        MODULE_MEMORY_S_PARA *pPara;
        pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
        if(pPara->Flag_NightLight==OFF)
        {
            pPara->Flag_NightLight=ON;
            BspLed_Mode(1,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
            BspLed_Mode(2,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
            BspLed_Mode(3,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
            BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
        }
        else
        {
            pPara->Flag_NightLight=OFF;
            BspLed_Mode(2,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
            BspLed_Mode(3,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
            BspLed_Mode(4,BSP_LED_E_MODE_ONCE,BSP_LED_E_LEVEL_2);
        }
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
        Hci_cmd =  0;
    }
    // 閲囬泦鏁版嵁鍒癴lash
    else if(Hci_cmd==7)
    {
        //娓呯┖mem
        Module_Memory_App(MODULE_MEMORY_APP_CMD_WEIGHT_CLR,NULL,NULL);
        //鍚姩鎺ユ敹
        s_state.bit1=1;
        Hci_cmd =  0;
    }
}



//static uint8_t Hci_DestDeviceType=1;
//static uint8_t Hci_DestAddr=2;
void Hci_Rx_Parse(uint8_t *pbuf,uint8_t len)
{
#if   (defined(HAIER))
    // 功能解析
    switch(pbuf[1])
    {
        // 按键
        case 0x81:
            // len验证
            if(pbuf[0]!=0x03)
            {
                break;
            }
            // 按键识别
            switch(pbuf[2])
            {
                case 1:
                    Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
                    // 关闭报警功能
                    if(pbuf[3]>20)
                    {
                        Hci_AlarmFunc_OnOff(OFF);
                    }
                    // 启动报警功能
                    else
                    {
                        Hci_AlarmFunc_OnOff(ON);
                    }
                    break;
                case 2:
                    // 校准
                    if(pbuf[3]>50)
                    {
                        // 校准
                        Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
                        BspKey_NewSign=1;
                        BspKey_Value=HCI_KEY_ENTER;
                        BspKey_KeepTimer_ms=8000;
                    }
                    // 关闭报警
                    else if(pbuf[3]>20)
                    {
                        Hci_Alarm_OnOff(OFF);

                    }
                    // 启动报警
                    else
                    {
                        Hci_Alarm_OnOff(ON);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
#else
    static uint8_t Hci_SourceDeviceType=3;
    static uint8_t Hci_SourceAddr=4;
    //长度校验
    if(len<6)
    {
        return;
    }
    //和校验
    //地址校验
    if(pbuf[2]!=Hci_SourceDeviceType \
       ||pbuf[3]!=Hci_SourceAddr)
    {
        return;
    }

    switch(pbuf[4])
    {
        case 1:
            if(pbuf[5]==0)
            {
                //关闭报警
                Hci_Alarm_OnOff(OFF);
            }
            else if(pbuf[5]==1)
            {
                //启动报警
                Hci_Alarm_OnOff(ON);
            }
            break;
        case 2:
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
            if(pbuf[5]==0)
            {
                //关闭报警功能
                Hci_AlarmFunc_OnOff(OFF);
            }
            else if(pbuf[5]==1)
            {
                //启动报警功能
                Hci_AlarmFunc_OnOff(ON);
            }
            break;
        case 3:
            Bsp_BuzzerMusic_PlayStop(ON,BSP_BUZZER_MUSIC_LIB_B,0);
            // 校准
            BspKey_NewSign=1;
            BspKey_Value=HCI_KEY_ENTER;
            BspKey_KeepTimer_ms=8000;
            break;
        default:
            break;
    }
#endif
    pbuf=pbuf;
    len=len;
}
#elif (defined(XKAP_ICARE_B_C))
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_NrfBle.h"
//#include "Bsp_NrfRfEsb.h"
static uint8_t Hci_cmd     =  0xFF;
static uint8_t Hci_step    =  0;
static uint8_t Hci_timer   =  0;
//
//static uint8_t Hci_SourceDeviceType=1;
//static uint8_t Hci_SourceAddr=2;
//static uint8_t Hci_DestDeviceType=3;
//static uint8_t Hci_DestAddr=4;
//
void Hci_100ms(void)
{
    uint8_t *pbuf;
    uint8_t i=0,j;
    // 首次进入
    if(Hci_cmd==0xFF)
    {
        Hci_cmd=0;

        BspNrfBle_s_Tx.buf[0] =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_LEN;
        BspNrfBle_s_Tx.buf[1] =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE;
        BspNrfBle_s_Tx.state  =  1;
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        memset(pbuf,0,APP_ADV_INFO_LENGTH-3);
//        pbuf[i++]   =  Hci_SourceDeviceType;
//        pbuf[i++]   =  Hci_SourceAddr;
//        pbuf[i++]   =  Hci_DestDeviceType;
//        pbuf[i++]   =  Hci_DestAddr;
        j   =  i;
        switch(BspKey_Value)
        {
            case HCI_KEY_2:
                pbuf[i++]   =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_LEN;
                pbuf[i++]   =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE;
                pbuf[i++]   =  2;
                pbuf[i++]   =  BspKey_KeepTimer_ms/100;
                // 校准
                if(BspKey_KeepTimer_ms>5000)
                {
                    Hci_cmd=5;
                    //pbuf[i++]   =  3;
                }
                // 关闭报警
                else if(BspKey_KeepTimer_ms>2000)
                {
                    Hci_cmd=2;
                    //pbuf[i++]   =  1;
                    //pbuf[i++]   =  0;
                }
                // 启动报警
                else
                {
                    Hci_cmd=1;
                    //pbuf[i++]   =  1;
                    //pbuf[i++]   =  1;
                }
                break;
            case HCI_KEY_1:
                pbuf[i++]   =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_LEN;
                pbuf[i++]   =  BSP_NRF_BLE_FACTORY_PROTOCOL_ICAREB_C_TYPE;
                pbuf[i++]   =  1;
                pbuf[i++]   =  BspKey_KeepTimer_ms/100;
                // 关闭报警功能
                if(BspKey_KeepTimer_ms>2000)
                {
                    Hci_cmd=4;
                    //pbuf[i++]   =  2;
                    //pbuf[i++]   =  0;
                }
                // 启动报警功能
                else
                {
                    Hci_cmd=3;
                    //pbuf[i++]   =  2;
                    //pbuf[i++]   =  1;
                }
                break;
            default:
                break;
        }
        if(i>j&&BspNrfBle_s_Tx.state==0)
        {
            //pbuf[i++]=Count_Sum(0,pbuf,i);
            //Bsp_NrfRfEsb_Tx(pbuf,i);
            memcpy(BspNrfBle_s_Tx.buf,(char*)pbuf,APP_ADV_INFO_LENGTH-3);
            BspNrfBle_s_Tx.state=1;
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    // 指示灯
    if(Hci_cmd==1||Hci_cmd==2||Hci_cmd==3||Hci_cmd==4||Hci_cmd==5)
    {
        switch(Hci_step)
        {
            case 0:
                if(Hci_cmd==1||Hci_cmd==2)
                {
                    BSP_LED_LED2_ON;
                }
                else if(Hci_cmd==3||Hci_cmd==4)
                {
                    BSP_LED_LED3_ON;
                }
                else if(Hci_cmd==5)
                {
                    BSP_LED_LED2_ON;
                    BSP_LED_LED3_ON;
                }
                Hci_timer=0;
                Hci_step++;
                break;
            case 1:
                if(Hci_timer<5&&Hci_cmd==1)
                {
                    BSP_LED_LED2_OFF;
                    Hci_step++;
                }
                else if(Hci_timer<10&&Hci_cmd==2)
                {
                    Hci_timer++;
                    if(Hci_timer==1)
                    {
                        BSP_LED_LED2_OFF;
                    }
                    else if(Hci_timer==2)
                    {
                        BSP_LED_LED2_ON;
                    }
                    else
                    {
                        BSP_LED_LED2_OFF;
                        Hci_step++;
                    }
                }
                else if(Hci_timer<5&&Hci_cmd==3)
                {
                    BSP_LED_LED3_OFF;
                    Hci_step++;
                }
                else if(Hci_timer<10&&Hci_cmd==4)
                {
                    Hci_timer++;
                    if(Hci_timer==1)
                    {
                        BSP_LED_LED3_OFF;
                    }
                    else if(Hci_timer==2)
                    {
                        BSP_LED_LED3_ON;
                    }
                    else
                    {
                        BSP_LED_LED3_OFF;
                        Hci_step++;
                    }
                }
                else if(Hci_timer<10&&Hci_cmd==5)
                {
                    Hci_timer++;
                    if(Hci_timer==1)
                    {
                        BSP_LED_LED2_OFF;
                        BSP_LED_LED3_OFF;
                    }
                    else if(Hci_timer==2)
                    {
                        BSP_LED_LED2_ON;
                        BSP_LED_LED3_ON;
                    }
                    else
                    {
                        BSP_LED_LED2_OFF;
                        BSP_LED_LED3_OFF;
                        Hci_step++;
                    }
                }
                else
                {
                    Hci_step++;
                }
                break;
            case 2:
                Hci_step   =  0;
                Hci_cmd    =  0;
                Hci_timer  =  0;
                break;
            default:
                break;
        }
    }
}
#elif (defined(XKAP_ICARE_B_D))
void Hci_100ms(void)
{}
#elif (defined(PROJECT_NRF5X_BLE))
#include "Bsp_Led.h"
#include "Bsp_Key.h"
#include "Bsp_NrfBle.h"
//
void Hci_Timer(uint16_t ms)
{
    static uint8_t sfirst=1;
    static uint8_t si=0;
    static uint8_t skey1_num=0,skey2_num=0;
    static uint8_t skey1_timer=0,skey2_timer=0;
    uint8_t *pbuf;
    uint8_t i=0,j;
    int16_t i16,j16;
    uint32_t i32;
    // 首次进入
    if(sfirst==1)
    {
        sfirst=0;

        BspNrfBle_s_Tx.buf[0] =  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_LEN;
        BspNrfBle_s_Tx.buf[1] =  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE;
        BspNrfBle_s_Tx.state  =  1;
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        memset(pbuf,0,APP_ADV_INFO_LENGTH-3);
        j   =  i;
        switch(BspKey_Value)
        {
            case HCI_KEY_2:
                if(skey2_num!=0xFF)
                {
                    skey2_num++;
                }
                else
                {
                    skey2_num=0;
                }
                skey2_timer = BspKey_KeepTimer_ms/100;
                break;
            case HCI_KEY_1:
                if(skey1_num!=0xFF)
                {
                    skey1_num++;
                }
                else
                {
                    skey1_num=0;
                }
                skey1_timer = BspKey_KeepTimer_ms/100;
                break;
            default:
                break;
        }
        if(i>j&&BspNrfBle_s_Tx.state==0)
        {
            memcpy(BspNrfBle_s_Tx.buf,(char*)pbuf,APP_ADV_INFO_LENGTH-3);
            BspNrfBle_s_Tx.state=1;
        }
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    // 秒执行
    if(ms==100)
    {
        si++;
        if(si<10)
        {
            return;
        }
        si=0;
    }
    else if(ms==1000)
    {
        si=0;
    }
    if(BspNrfBle_s_Tx.state == 0)
    {
        i=0;
        BspNrfBle_s_Tx.buf[i++]  =  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_LEN;
        BspNrfBle_s_Tx.buf[i++]  =  BSP_NRF_BLE_FACTORY_PROTOCOL_XSL_TAG_TYPE;
        // vcc
        if(ADC_Vbat_mV<1500)
        {
            BspNrfBle_s_Tx.buf[i++]  =  0;
        }
        else if(ADC_Vbat_mV>3700)
        {
            BspNrfBle_s_Tx.buf[i++] = 370-120;
        }
        else
        {
            BspNrfBle_s_Tx.buf[i++] = (ADC_Vbat_mV/10)-120;
        }
        // key1/2
        BspNrfBle_s_Tx.buf[i++]  =  skey1_num;
        BspNrfBle_s_Tx.buf[i++]  =  skey1_timer;
        BspNrfBle_s_Tx.buf[i++]  =  skey2_num;
        BspNrfBle_s_Tx.buf[i++]  =  skey2_timer;
        // bmp180
        BspBmp180_Read(&i16,&i32,&j16);
        BspNrfBle_s_Tx.buf[i++]  =  i16;
        BspNrfBle_s_Tx.buf[i++]  =  i16>>8;
        BspNrfBle_s_Tx.buf[i++]  =  i32;
        BspNrfBle_s_Tx.buf[i++]  =  i32>>8;
        BspNrfBle_s_Tx.buf[i++]  =  i32>>16;
        BspNrfBle_s_Tx.buf[i++]  =  j16;
        // Ap3216c
        BspAp3216c_Read((uint16_t*)&i16,(uint16_t*)&j16,NULL);
        BspNrfBle_s_Tx.buf[i++]  =  i16;
        BspNrfBle_s_Tx.buf[i++]  =  i16>>8;
        BspNrfBle_s_Tx.buf[i++]  =  j16;
        BspNrfBle_s_Tx.buf[i++]  =  j16>>8;
        // Mpu6050
        BspNrfBle_s_Tx.buf[i++]  =  BspMpu6050_TapCmt;
        //
        BspNrfBle_s_Tx.state     =  1;
    }
}
#elif (defined(PROJECT_BIB_TEST1))
#include "Bsp_Key.h"
#include "LCD_ST7735.h"
#include "QDTFT_demo.h"
#include "Bsp_Uart.h"
#include "GUI.h"
#include "Bsp_Rtc.h"
//外部变量声明
extern const unsigned char gImage_SET[];
extern const unsigned char gImage_pulse[];
extern const unsigned char gImage_doctor[];
// 任务变量
MODULE_OS_TASK_TAB(App_TaskHciTCB);
MODULE_OS_TASK_STK(App_TaskHciStk,APP_TASK_HCI_STK_SIZE);
//---用于刷新计时
static  uint16_t  Hci_ReShowTimer      =  0;
//---用于返回计时
static  uint16_t  Hci_ReturnTimer      =  0;
//---用于参数修改标记
static  uint8_t   Hci_ParaChangeSign   =  0;
//---用于光标位置
static  uint8_t   Hci_cursor           =  0;
//---用于光标闪烁计时
static  uint8_t   Hci_CursorFlick      =  0;
//---用于参数加载
//---用于表示第一次进入函数(表达指针转换)
static  uint8_t   Hci_NoFirst          =  0;
// 函数声明
static void (*pHci_MenuFun)(void);
static void Hci_Menu_PowerOn(void);
static void Hci_Menu_PowerOff(void);
static void Hci_Menu_Time(void);
static void Hci_Menu_Set(void);
static void uctsk_Hci (void *pvParameters);
// 任务创建
void  App_HciTaskCreate (void)
{
    MODULE_OS_TASK_CREATE("Task-Hci",\
                          uctsk_Hci,\
                          APP_TASK_HCI_PRIO,\
                          App_TaskHciStk,\
                          APP_TASK_HCI_STK_SIZE,\
                          App_TaskHciTCB,\
                          NULL);
}
// 任务实体
static void uctsk_Hci (void *pvParameters)
{
    uint16_t i16=0;
    i16=i16;
    //硬件初始化
    Lcd_Init();
    Lcd_Clear(BLACK);
    //防止警告
    Hci_ReShowTimer      =  Hci_ReShowTimer;
    Hci_ReturnTimer      =  Hci_ReturnTimer;
    Hci_ParaChangeSign   =  Hci_ParaChangeSign;
    Hci_cursor           =  Hci_cursor;
    Hci_CursorFlick      =  Hci_CursorFlick;
    Hci_NoFirst          =  Hci_NoFirst;
    //变量初始化
    pHci_MenuFun=Hci_Menu_PowerOn;
    //
    LCD_LED_SET;
    //
    UART_INIT(2,115200);
    for(;;)
    {
        MODULE_OS_DELAY_MS(100);
        //-----------------------
        (*pHci_MenuFun)();
    }
}
// 开机画面
static void Hci_Menu_PowerOn(void)
{
    uint8_t *pbuf;
    Hci_ReturnTimer++;
    if(Hci_ReturnTimer>10*3)
    {
        pHci_MenuFun = Hci_Menu_Time;
        //清屏
        Hci_ReShowTimer =  0;
        Hci_ParaChangeSign=0;
        Hci_CursorFlick =  0;
        Hci_ReturnTimer =  0;
        Hci_NoFirst     =  0;
        Lcd_Clear(GRAY0);
        return;
    }
    if(Hci_ReShowTimer==0)
    {
        Hci_ReShowTimer=0xFFFF;
        //
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Lcd_Clear(GRAY0);
        Gui_DrawFont_GBK16(16,0,BLUE,GRAY0,"Power ON");
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
// 关机画面
static void Hci_Menu_PowerOff(void)
{
    uint8_t *pbuf;
    Hci_ReturnTimer++;
    if(Hci_ReturnTimer>10*3)
    {
        pHci_MenuFun = Hci_Menu_Time;
        //清屏
        Hci_ReShowTimer =  0;
        Hci_ParaChangeSign=0;
        Hci_CursorFlick =  0;
        Hci_ReturnTimer =  0;
        Hci_NoFirst     =  0;
        Lcd_Clear(GRAY0);
        return;
    }
    if(Hci_ReShowTimer==0)
    {
        Hci_ReShowTimer=0xFFFF;
        //
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        Lcd_Clear(GRAY0);
        Gui_DrawFont_GBK16(16,0,RED,GRAY0, "Power OFF");
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
}
// 时间画面
static void Hci_Menu_Time(void)
{
    uint8_t *pbuf;
    static uint8_t si=0;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=1;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //Gui_DrawFont_Num32(0,50,RED,GRAY0,BspRtc_CurrentTimeBuf[3]%100/10);
        //Gui_DrawFont_Num32(20,50,RED,GRAY0,BspRtc_CurrentTimeBuf[3]%10);
        if(si!=BspRtc_CurrentTimeBuf[5])
        {
            si=BspRtc_CurrentTimeBuf[5];
            Gui_DrawFont_Num32(00,50,RED,GRAY0,BspRtc_CurrentTimeBuf[4]%100/10);
            Gui_DrawFont_Num32(30,50,RED,GRAY0,BspRtc_CurrentTimeBuf[4]%10);
            Gui_DrawFont_Num32(60,50,RED,GRAY0,BspRtc_CurrentTimeBuf[5]%100/10);
            Gui_DrawFont_Num32(90,50,RED,GRAY0,BspRtc_CurrentTimeBuf[5]%10);
        }
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_MOVE:
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                pHci_MenuFun=Hci_Menu_Set;
                Lcd_Clear(GRAY0);
                break;
            case HCI_KEY_SET:
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                Hci_ReturnTimer     =  0;
                pHci_MenuFun=Hci_Menu_PowerOff;
                Lcd_Clear(GRAY0);
                break;
            case HCI_KEY_ESC:
                if(BspKey_KeepTimer_ms>=2000)
                {
                    Bsp_Pwr_EnterStop();
                }
                break;
            default:
                break;
        }
    }
}
// 设置画面
static void Hci_Menu_Set(void)
{
    uint8_t *pbuf;
    if(0 != Hci_ReShowTimer)
    {
        Hci_ReShowTimer--;
    }
    else
    {
        Hci_ReShowTimer=0xFFFF;
        //申请缓存
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        //
        Lcd_Clear(BLACK);
        showimage4040(gImage_SET,15,15);
        showimage4040(gImage_pulse,15,80);
        showimage4040(gImage_doctor,80,15);
        //释放缓存
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
    if(BspKey_NewSign==1)
    {
        BspKey_NewSign=0;
        Hci_ReShowTimer = 0;
        switch(BspKey_Value)
        {
            case HCI_KEY_MOVE:
                break;
            case HCI_KEY_SET:
                break;
            case HCI_KEY_ESC:
                Hci_ReShowTimer     =  0;
                Hci_ParaChangeSign  =  0;
                Hci_cursor          =  0;
                Hci_CursorFlick     =  0;
                pHci_MenuFun=Hci_Menu_Time;
                Lcd_Clear(GRAY0);
                break;
            default:
                break;
        }
    }
}
#endif
//

