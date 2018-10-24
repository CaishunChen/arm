/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "includes.h"
#include "Language.h"
//----------------------------------------------------------------------------
char const *pSTR_UTF8_XSLZK[LANGUAGE_E_MAXNUM]=
{
    "XslZk",
    "徐松亮字库",
    "XslZk",
};
char const *pSTR_UTF8_YEAR[LANGUAGE_E_MAXNUM]=
{
    "--",
    "年",
    "--",
};
char const *pSTR_UTF8_MONTH[LANGUAGE_E_MAXNUM]=
{
    "--",
    "月",
    "--",
};
char const *pSTR_UTF8_DAY[LANGUAGE_E_MAXNUM]=
{
    "  ",
    "日",
    "  ",
};
char const *pSTR_UTF8_WEEK_BUF[][LANGUAGE_E_MAXNUM]=
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
char const *pSTR_UTF8_TEMP[LANGUAGE_E_MAXNUM]=
{
    "Temp:",
    "温度：",
    "Temp:",
};
char const *pSTR_UTF8_HUMI[LANGUAGE_E_MAXNUM]=
{
    "Humi:",
    "湿度：",
    "Humi:",
};
char const *pSTR_UTF8_CLOCK_BUF[LANGUAGE_E_MAXNUM]=
{
    "DataTime:",
    "时    钟:",
    "DataTime:",
};
char const *pSTR_UTF8_AUTO_MEASURE_BUF[LANGUAGE_E_MAXNUM]=
{
    "AutoWork:",
    "自动监测:",
    "AutoWork:",
};
char const *pSTR_UTF8_SLEEP_BACKLIGHT_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Light:",
    "入睡背光:",
    "SL Light:",
};
char const *pSTR_UTF8_SLEEP_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Warn :",
    "入睡提醒:",
    "SL Warn :",
};
char const *pSTR_UTF8_DRUG_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DrugWarn:",
    "服药提醒:",
    "DrugWarn:",
};
char const *pSTR_UTF8_DRINK_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DK Warn :",
    "喝水提醒:",
    "DK Warn :",
};
char const *pSTR_UTF8_ALARM_MODE_BUF[][LANGUAGE_E_MAXNUM]=
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
char const *pSTR_UTF8_ONOFF_BUF[][LANGUAGE_E_MAXNUM]=
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
char const *pSTR_UTF8_COMFORT_BUF[LANGUAGE_E_MAXNUM]=
{
    "Comfort",
    "舒适度 ",
    "Comfort",
};
char const *pSTR_UTF8_SLEEPLIGHT_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF",
        "关闭",
        "OFF",
    },
    {
        "LEVEL-1",
        "微亮",
        "LEVEL-1",
    },
    {
        "LEVEL-2",
        "标准",
        "LEVEL-2",
    },
    {
        "LEVEL-3",
        "高亮",
        "LEVEL-3",
    },
    {
        "LEVEL-4",
        "全亮",
        "LEVEL-4",
    },
};
char const *pSTR_UTF8_HINT[LANGUAGE_E_MAXNUM]=
{
    "Hint",
    "提    示",
    "Hint",
};
char const *pSTR_UTF8_ASK[LANGUAGE_E_MAXNUM]=
{
    "Ask",
    "确    认",
    "Ask",
};
char const *pSTR_UTF8_GETUP[LANGUAGE_E_MAXNUM]=
{
    "Get Up!",
    "起床啦 起床啦!",
    "Get Up!",
};
char const *pSTR_UTF8_MONITORING[LANGUAGE_E_MAXNUM]=
{
    "Monitoring...",
    "监测中...",
    "Monitoring...",
};
char const *pSTR_UTF8_MONITOR_OVER[LANGUAGE_E_MAXNUM]=
{
    "Monitor Over",
    "监测结束",
    "Monitor Over",
};
char const *pSTR_UTF8_BLE_UPLOADING[LANGUAGE_E_MAXNUM]=
{
    "BLE Uploading!",
    "蓝牙数据上传中!",
    "BLE Uploading!",
};
char const *pSTR_UTF8_BLE_UPLOAD_OK[LANGUAGE_E_MAXNUM]=
{
    "BLE Upload OK!",
    "蓝牙上传成功!",
    "BLE Upload OK!",
};
char const *pSTR_UTF8_BLE_UPLOAD_ERR[LANGUAGE_E_MAXNUM]=
{
    "BLE Upload Err!",
    "蓝牙上传失败!",
    "BLE Upload Err!",
};
char const *pSTR_UTF8_BLE_CONNECT_ERR[LANGUAGE_E_MAXNUM]=
{
    "BLE Connect Err!",
    "蓝牙连接失败!",
    "BLE Connect Err!",
};
char const *pSTR_UTF8_OPENBLE[LANGUAGE_E_MAXNUM]=
{
    "Please Open BLE!",
    "请开启手机蓝牙!",
    "Please Open BLE!",
};
char const *pSTR_UTF8_THI[][LANGUAGE_E_MAXNUM]=
{
/*	
    "THI",
    "环境舒适度",
    "THI",
*/
    {
        "Cold",
        "寒冷",
        "Cold",
    },
    {
        "Cool",
        "偏凉",
        "Coll",
    },
    {
        "Comfort",
        "舒适",
        "Comfort",
    },
    {
        "Warm",
        "偏热",
        "Warm",
    },
    {
        "Heat",
        "闷热",
        "Heat",
    },  
};
char const *pSTR_UTF8_SIGN_PERCENT[LANGUAGE_E_MAXNUM]=
{
    "％",
    "％",
    "％",
};
char const *pSTR_UTF8_SIGN_TEMP[LANGUAGE_E_MAXNUM]=
{
    "℃",
    "℃",
    "℃",
};
char const *pSTR_UTF8_DATA_ANALYSIS[LANGUAGE_E_MAXNUM]=
{
    "Sleep Data Analysis...",
    "睡眠数据分析中...",
    "Sleep Data Analysis...",
};
char const *pSTR_UTF8_GPRS_WAITING[LANGUAGE_E_MAXNUM]=
{
    "GPRS Net Waiting...",
    "等待GPRS网络连接...",
    "GPRS Net Waiting...",
};
char const *pSTR_UTF8_GPRS_NET_ERR[LANGUAGE_E_MAXNUM]=
{
    "GPRS Net Err...",
    "GPRS网络连接失败",
    "GPRS Net Err...",
};
char const *pSTR_UTF8_GPRS_UPLOADING[LANGUAGE_E_MAXNUM]=
{
    "GPRS Uploading...",
    "睡眠数据上传中...",
    "GPRS Uploading...",
};
char const *pSTR_UTF8_QR_CODE_BIND[LANGUAGE_E_MAXNUM]=
{
    "",
    "请用iSleep APP扫描下方",
    "",
};
char const *pSTR_UTF8_QR_CODE_BIND1[LANGUAGE_E_MAXNUM]=
{
    "",
    "二维码,完成设备绑定",
    "",
};
char const *pSTR_UTF8_QR_CODE_SALES[LANGUAGE_E_MAXNUM]=
{
    "",
    "现推出改善睡眠质量的睡",
    "",
};
char const *pSTR_UTF8_QR_CODE_SALES1[LANGUAGE_E_MAXNUM]=
{
    "",
    "眠评估改善服务,微信扫描",
    "",
};
char const *pSTR_UTF8_QR_CODE_SALES2[LANGUAGE_E_MAXNUM]=
{
    "",
    "下方二维码了解详情.",
    "",
};
char const *pSTR_UTF8_ENTER_PWR_STOP[LANGUAGE_E_MAXNUM]=
{
    "waiting for stop...",
    "正进入待机模式,请等待...",
    "waiting for stop...",
};
char const *pSTR_UTF8_UPDATA_POWER[LANGUAGE_E_MAXNUM]=
{
    "Updata,Keep Power",
    "固件升级,请勿断电",
    "Updata,Keep Power",
};
char const *pSTR_UTF8_JTNR[LANGUAGE_E_MAXNUM]=
{
    "",
    "具体内容",
    "",
};
char const *pSTR_UTF8_XBCZ[LANGUAGE_E_MAXNUM]=
{
    "",
    "下步操作",
    "",
};

char const *pSTR_UTF8_HINT_ESC_RETURN[LANGUAGE_E_MAXNUM]=
{
    "",
    "按[返回]键返回主界面",
    "",
};
char const *pSTR_UTF8_HINT_ESC_RETURN_RESET[LANGUAGE_E_MAXNUM]=
{
    "",
    "按[返回]键重新设置",
    "",
};
char const *pSTR_UTF8_HINT_RTC_LOW[LANGUAGE_E_MAXNUM]=
{
    "",
    "设备主板电池电量较低,\r请尽快进行更换.\r请拨打4008-590-590客服\r咨询.",
    "",
};
char const *pSTR_UTF8_HINT_INSTALL_SIMCARD[LANGUAGE_E_MAXNUM]=
{
    "",
    "设备中未插入SIM卡,\r数据将无法上传.",
    "",
};
char const *pSTR_UTF8_HINT_SLEEP[LANGUAGE_E_MAXNUM]=
{
    "",
    "您该睡觉了,\r保持睡眠好习惯,\r美容养颜身体健.",
    "",
};
char const *pSTR_UTF8_HINT_NOT_VALID_DATA[LANGUAGE_E_MAXNUM]=
{
	  "",
    "没有需要上传的数据.",
    "",
};
char const *pSTR_UTF8_ASK_STOP_MEASURE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "您确定停止本次监测吗?\r(监测时间短于1小时放弃\r本次监测,长于1小时完成\r本次监测).",
    "",
};
char const *pSTR_UTF8_ASK_STOP_KEY[LANGUAGE_E_MAXNUM]=
{
	  "",
    "按[顶部]键停止监测\r按[返回]键继续监测",
    "",
};
char const *pSTR_UTF8_HINT_INVALID_MEASURE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "您的监测时间小于1小时,\r本次睡眠监测无效,\r请监测1小时以上.",
    "",
};
char const *pSTR_UTF8_ASK_CANCEL_MEASURE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "您确定要取消本次监测吗?",
    "",
};
char const *pSTR_UTF8_ASK_CANCEL_MEASURE_KEY[LANGUAGE_E_MAXNUM]=
{
	  "",
    "按[设置]键取消监测\r按[返回]键继续监测",
    "",
};
char const *pSTR_UTF8_HINT_WAIT_NET[LANGUAGE_E_MAXNUM]=
{
	  "",
    "等待联网,稍后再试.",
    "",
};
char const *pSTR_UTF8_HINT_RESET_AUTOWORK[LANGUAGE_E_MAXNUM]=
{
	  "",
    "请将自动监测时间范围\r设为1-12小时.",
    "",
};
char const *pSTR_UTF8_HINT_PWR_STOP[LANGUAGE_E_MAXNUM]=
{
	  "",
    "设备准备进入休眠模式",
    "",
};
char const *pSTR_UTF8_HINT_PWR_STOP_KEY[LANGUAGE_E_MAXNUM]=
{
	  "",
    "按任意键可唤醒设备",
    "",
};
char const *pSTR_UTF8_HINT_SLEEP_BACKLIGHT_TOO_BRIGHT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "睡眠监测中屏幕过亮\r可能会干扰您的睡眠,\r请您确认设置无误.",
    "",
};
char const *pSTR_UTF8_HINT_BELL[LANGUAGE_E_MAXNUM]=
{
	  "",
    "使用电池时设备自动启动\r[节能模式].\r节能模式将关闭网络通信,\r并降低屏幕亮度.",
    "",
};
char const *pSTR_UTF8_HINT_BELL_KEY[LANGUAGE_E_MAXNUM]=
{
	  "",
    "使用电源恢复[标准模式]",
    "",
};
char const *pSTR_UTF8_HINT_DATA_FULL[LANGUAGE_E_MAXNUM]=
{
	  "",
    "您的设备只能保留7条\r最新睡眠数据,\r请您尽快上传睡眠数据,\r以免数据丢失.",
    "",
};
char const *pSTR_UTF8_HINT_DATA_FULL_KEY[LANGUAGE_E_MAXNUM]=
{
	  "",
    "使用电源供电,将自动上传\r睡眠数据.",
    "",
};
char const *pSTR_UTF8_SLEEP_MARK_REPORT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "睡眠评分报告",
    "",
};
char const *pSTR_UTF8_SMR_SN[LANGUAGE_E_MAXNUM]=
{
	  "",
    "序号",
    "",
};
char const *pSTR_UTF8_SMR_PROJECT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "项目",
    "",
};
char const *pSTR_UTF8_SMR_FULL_SCORE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "满分",
    "",
};
char const *pSTR_UTF8_SMR_GET_SCORE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "得分",
    "",
};
char const *pSTR_UTF8_SMR_MONITOR_RESULT[LANGUAGE_E_MAXNUM]=
{
	  "",
	  "监测结果",
	  "",
};
char const *pSTR_UTF8_SMR_DEFLECT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "分数偏离",
    "",
};
char const *pSTR_UTF8_SMR_SLEEP_STRUCT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "睡眠结构",
    "",
};
char const *pSTR_UTF8_SMR_TOTAL_SLEEP_TIME[LANGUAGE_E_MAXNUM]=
{
	  "",
    "总睡时长",
    "",
};
char const *pSTR_UTF8_SMR_INTO_SLEEP_TIME[LANGUAGE_E_MAXNUM]=
{
	  "",
    "入睡时长",
    "",
};
char const *pSTR_UTF8_SMR_BODY_MOVEMENT_CMT[LANGUAGE_E_MAXNUM]=
{
	  "",
    "体动次数",
    "",
};
char const *pSTR_UTF8_SMR_WAKE_STATE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "唤醒状态",
    "",
};
char const *pSTR_UTF8_SMR_BEGIN_SLEEP_TIME[LANGUAGE_E_MAXNUM]=
{
	  "",
    "睡眠时间",
    "",
};
char const *pSTR_UTF8_SMR_TEMP[LANGUAGE_E_MAXNUM]=
{
	  "",
    "环境温度",
    "",
};
char const *pSTR_UTF8_SMR_HUMI[LANGUAGE_E_MAXNUM]=
{
	  "",
    "环境湿度",
    "",
};
char const *pSTR_UTF8_SMR_TOTAL_SCORE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "累计分数",
    "",
};
char const *pSTR_UTF8_SMR_FINAL_SCORE[LANGUAGE_E_MAXNUM]=
{
	  "",
    "最终得分",
    "",
};
char const *pSTR_UTF8_AIR[LANGUAGE_E_MAXNUM]=
{
 		"",
    "空气",
    "",
};
char const *pSTR_UTF8_AIR_QUALITY[][LANGUAGE_E_MAXNUM]=
{
	  {
        "",
        "优",
        "",
    },
    {
        "",
        "良",
        "",
    },
    {
        "",
        "轻",
        "",
    },
    {
        "",
        "中",
        "",
    },
    {
        "",
        "重",
        "",
    },
};
char const *pSTR_UTF8_WIND_DIRECTION[][LANGUAGE_E_MAXNUM]=
{
	  {
        "",
        "东",
        "",
    },
    {
        "",
        "东南",
        "",
    },
    {
        "",
        "南",
        "",
    },
    {
        "",
        "西南",
        "",
    },
    {
        "",
        "西",
        "",
    },
    {
        "",
        "西北",
        "",
    },
    {
        "",
        "北",
        "",
    },
    {
        "",
        "东北",
        "",
    },
};
char const *pSTR_UTF8_SOS_SENDING[LANGUAGE_E_MAXNUM]=
{
 		"",
    "呼救信号正在发送",
    "",
};
char const *pSTR_UTF8_SOS_SENDOK[LANGUAGE_E_MAXNUM]=
{
 		"",
    "呼救信号发送成功",
    "",
};
char const *pSTR_UTF8_SOS_SENDERR[LANGUAGE_E_MAXNUM]=
{
 		"",
    "呼救信号正在失败",
    "",
};
char const *pSTR_UTF8_INVALID_DATA[LANGUAGE_E_MAXNUM]=
{
 		"",
    "无效数据",
    "",
};
//----------------------------------------------
