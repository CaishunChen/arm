/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "includes.h"
#include "Language.h"
#include "Module_Memory.h"
#include "Bsp_CpuFlash.h"
//----------------------------------------------------------------------------
uint8_t Main_Language   =  LANGUAGE_E_CHINESE;
void Language_Init(void)
{
    MODULE_MEMORY_S_PARA *pPara;
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
    Main_Language  =  pPara->Language;
    if(Main_Language>=LANGUAGE_E_MAXNUM)
    {
        Main_Language=LANGUAGE_E_CHINESE;
        pPara->Language=LANGUAGE_E_CHINESE;
        Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_W,(uint8_t*)pPara,NULL);
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
}
void Language_Set(uint8_t language)
{
    MODULE_MEMORY_S_PARA *pPara;
    if(language>=LANGUAGE_E_MAXNUM)
    {
        return;
    }
    pPara=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    Module_Memory_App(MODULE_MEMORY_APP_CMD_GLOBAL_R,(uint8_t*)pPara,NULL);
    Main_Language    =  language;
    pPara->Language  =  language;
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pPara);
}

const char *pSTR_XSLZK[LANGUAGE_E_MAXNUM]=
{
    "XslZk",
    "徐松亮字库",
    "XslZk",
};

const char *pSTR_WEEK_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "Sun",
        "星期日",
        "日曜日",
    },
    {
        "Mon",
        "星期一",
        "月曜日",
    },
    {
        "Tue",
        "星期二",
        "火曜日",
    },
    {
        "Wed",
        "星期三",
        "水曜日",
    },
    {
        "Thurs",
        "星期四",
        "木曜日",
    },
    {
        "Fri",
        "星期五",
        "金曜日",
    },
    {
        "Sat",
        "星期六",
        "土曜日",
    },
};
char const *pSTR_ALARM_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "Alarm  1:",
        "闹  钟 1:",
        "Alarm  1:",
    },
    {
        "Alarm  2:",
        "闹  钟 2:",
        "Alarm  2:",
    },
};
char const *pSTR_CLOCK_BUF[LANGUAGE_E_MAXNUM]=
{
    "DataTime:",
    "日期时间:",
    "DataTime:",
};
char const *pSTR_AUTO_MEASURE_BUF[LANGUAGE_E_MAXNUM]=
{
    "AutoWork:",
    "自动监测:",
    "AutoWork:",
};
char const *pSTR_SLEEP_BACKLIGHT_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Light:",
    "入睡背光:",
    "SL Light:",
};
char const *pSTR_SLEEP_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Warn :",
    "入睡提醒:",
    "SL Warn :",
};
char const *pSTR_DRUG_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DrugWarn:",
    "服药提醒:",
    "DrugWarn:",
};
char const *pSTR_DRINK_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DK Warn :",
    "喝水提醒:",
    "DK Warn :",
};
char const *pSTR_ALARM_MODE_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF ",
        "关闭",
        "OFF ",
    },
    {
        "TIM ",
        "定时",
        "TIM ",
    },
    {
        "Auto",
        "智能",
        "Auto",
    },
};
char const *pSTR_ONOFF_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF",
        "关",
        "关",
    },
    {
        "ON",
        "开",
        "开",
    },
};
char const *pSTR_COMFORT_BUF[LANGUAGE_E_MAXNUM]=
{
    "Comfort",
    "舒适度 ",
    "Comfort",
};
char const *pSTR_SLEEPLIGHT_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF",
        "关闭",
        "OFF",
    },
    {
        "LEVEL-1",
        "极暗",
        "LEVEL-1",
    },
    {
        "LEVEL-2",
        "微光",
        "LEVEL-2",
    },
};
char const *pSTR_HINT[LANGUAGE_E_MAXNUM]=
{
    "Hint",
    "提示",
    "Hint",
};
char const *pSTR_ASK[LANGUAGE_E_MAXNUM]=
{
    "Ask",
    "询问",
    "Ask",
};
char const *pSTR_HINT_SLEEP[LANGUAGE_E_MAXNUM]=
{
    "Master,Sleep!",
    "您该睡觉了!",
    "Master,Sleep!",
};
char const *pSTR_HINT_DRUG[LANGUAGE_E_MAXNUM]=
{
    "Master,Drug!",
    "您该吃药了!",
    "Master,Drug!",
};
char const *pSTR_HINT_DRINK[LANGUAGE_E_MAXNUM]=
{
    "Master,Drink!",
    "您该喝水了!",
    "Master,Drink!",
};
char const *pSTR_HINT_RTC_LOW[LANGUAGE_E_MAXNUM]=
{
    "Please replace RTC battery!",
    "请更换时钟电池!",
    "Please replace RTC battery!",  
};
char const *pSTR_HINT_INSTALL_SIMCARD[LANGUAGE_E_MAXNUM]=
{
    "Please Install SimCard!",
    "请安装SIM卡!",
    "Please Install SimCard!",  
};
char const *pSTR_GPRS_DATA_UPLOADED_OK[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded OK!",
    "GPRS数据上传成功!",
    "Gprs Data uploaded OK!",  
};
char const *pSTR_GPRS_DATA_UPLOADED_ERR[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded ERR!",
    "GPRS数据上传失败!",
    "Gprs Data uploaded ERR!",  
};
char const *pSTR_GPRS_DATA_UPLOADING[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded ...",
    "GPRS数据上传中...",
    "Gprs Data uploaded ...",  
};
char const *pSTR_WAIT_NET[LANGUAGE_E_MAXNUM]=
{
    "Waiting for net.\nTry again later!",
    "等待连网,稍后再试!",
    "Waiting for net.\nTry again later!",  
};
char const *pSTR_NOT_VALID_DATA[LANGUAGE_E_MAXNUM]=
{
    "There is not valid data!",
    "无有效数据!",
    "There is not valid data!",  
};
char const *pSTR_ASK_CANCEL_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "Cancel measure now?\nPress (Set) Sure",
    "是否取消监测?\n按(设置)键确认",
    "Cancel measure now?\nPress (Set) Sure",
};
char const *pSTR_ASK_STOP_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "Stop measure now?\nPress (Set) Sure",
    "是否停止监测?\n按(设置)键确认",
    "Stop measure now?\nPress (Set) Sure",
};
char const *pSTR_HINT_INVALID_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "The test is less than 1 hour.\r\nSo is invalid!",
    "监测无效(时间小于1小时)!",
    "The test is less than 1 hour.\r\nSo is invalid!",
};
char const *pSTR_HINT_RESET_AUTOWORK[LANGUAGE_E_MAXNUM]=
{
    "Please Reset AutoWork Value!\r\n(Diff Range:1-12 hour)",
    "重新设置自动监测\r\n(间隔:1-12 小时)",
    "Please Reset AutoWork Value!\r\n(Diff Range:1-12 hour)",
};
char const *pSTR_DAYMOVECURVE_HEAD1[LANGUAGE_E_MAXNUM]=
{
		"%01d: Day Move Curve(%02d/%02d)",
		"第%01d条记录: 体动记录(%02d/%02d)",
		"%01d: Day Move Curve(%02d/%02d)",
};
char const *pSTR_DAYMOVECURVE_HEAD2[LANGUAGE_E_MAXNUM]=
{
		"%01d: Day Move Curve(none)",
		"第%01d条记录: 体动记录(无数据)",
		"%01d: Day Move Curve(none)",
};
char const *pSTR_DAYMOVECURVE_HEAD3[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"第%01d条记录: ",
		"%01d:",
};
char const *pSTR_SLEEP_CURVE_STR1[LANGUAGE_E_MAXNUM]=
{
		"%01d",
		"第%01d条记录: 睡眠结构曲线",
		"%01d",
};
char const *pSTR_SLEEP_CURVE_STR2[LANGUAGE_E_MAXNUM]=
{
		"%01d",
		"第%01d条记录: ",
		"%01d",
};
char const *pSTR_SLEEP_CURVE_STR3[LANGUAGE_E_MAXNUM]=
{
		"None",
		"暂无数据,请监测",
		"None",
};
char const *pSTR_SLEEP_CURVE_STR4[LANGUAGE_E_MAXNUM]=
{
		"%02d-%02d %02d:%02d - %02d-%02d %02d:%02d",
		"监测时间:%02d月%02d日%02d:%02d - %02d月%02d日%02d:%02d",
		"%02d-%02d %02d:%02d - %02d-%02d %02d:%02d",
};
char const *pSTR_SLEEP_CURVE_STR5[LANGUAGE_E_MAXNUM]=
{
		"None",
		"监测无效",
		"None",
};
char const *pSTR_SLEEP_CURVE_STR6[LANGUAGE_E_MAXNUM]=
{
		"None",
		"请注意监测环境或联系客服",
		"None",
};
char const *pSTR_SLEEP_CURVE_GRADE[LANGUAGE_E_MAXNUM]=
{
		"GRADE",
		"评分",
		"GRADE",
};
char const *pSTR_SLEEP_CURVE_SCORE[LANGUAGE_E_MAXNUM]=
{
		"SCORE",
		"分",
		"SCORE",
};
char const *pSTR_SLEEP_CURVE_TOTAL[LANGUAGE_E_MAXNUM]=
{
		"TOTAL:",
		"总睡:",
		"TOTAL:",
};
char const *pSTR_SLEEP_CURVE_BODYMOVE[LANGUAGE_E_MAXNUM]=
{
		"BODYMOVE:",
		"体动:",
		"BODYMOVE:",
};
char const *pSTR_SLEEP_CURVE_WAKE[LANGUAGE_E_MAXNUM]=
{
		"Wake:",
		"觉醒:",
		"wake:",
};
char const *pSTR_SLEEP_CURVE_LIGHTSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Light Sleep:",
		"浅睡:",
		"Light Sleep:",
};
char const *pSTR_SLEEP_CURVE_MSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Middle Sleep:",
		"中睡:",
		"Middle Sleep:",
};
char const *pSTR_SLEEP_CURVE_DEEPSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Deep Sleep:",
		"深睡:",
		"Deep Sleep:",
};
char const *pSTR_SLEEP_CURVE_NUM[LANGUAGE_E_MAXNUM]=
{
		"NUM",
		"次",
		"NUM",
};
char const *pSTR_SLEEP_CURVE_WAKE1[LANGUAGE_E_MAXNUM]=
{
		"Wake:",
		"觉 醒",
		"wake:",
};
char const *pSTR_SLEEP_CURVE_LIGHTSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Light Sleep:",
		"浅 睡",
		"Light Sleep:",
};
char const *pSTR_SLEEP_CURVE_MSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Middle Sleep:",
		"中 睡",
		"Middle Sleep:",
};
char const *pSTR_SLEEP_CURVE_DEEPSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Deep Sleep:",
		"深 睡",
		"Deep Sleep:",
};
char const *pSTR_BHCURVE_STR1[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"第%01d条记录: 呼吸心率曲线",
		"%01d:",
};
char const *pSTR_BHCURVE_STR2[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"第%01d条记录: ",
		"%01d:",
};
char const *pSTR_BHCURVE_STR3[LANGUAGE_E_MAXNUM]=
{
		"Breath",
		"呼 吸",
		"Breath",
};
char const *pSTR_BHCURVE_STR4[LANGUAGE_E_MAXNUM]=
{
		"num/min",
		"(次/分)",
		"num/min",
};
char const *pSTR_BHCURVE_STR5[LANGUAGE_E_MAXNUM]=
{
		"Heart",
		"心 率",
		"Heart",
};
char const *pSTR_SENDED[LANGUAGE_E_MAXNUM]=
{
		"Sended",
		"(已传)",
		"Sended",
};
char const *pSTR_NO_SEND[LANGUAGE_E_MAXNUM]=
{
		"Not Send",
		"(待传)",
		"Not Send",
};
//----------------------------------------------
