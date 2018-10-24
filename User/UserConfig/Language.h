/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-06-03
***********************************************************************************
*/
#ifndef __LANGUAGE_H
#define __LANGUAGE_H
//---------------------------------------------------------------------------------
//
enum LANGUAGE_E
{
    LANGUAGE_E_ENGLISH=0,
    LANGUAGE_E_CHINESE,
    LANGUAGE_E_JAPANESE,
    LANGUAGE_E_MAXNUM,
};
//GBK
extern uint8_t Main_Language;
extern char const *pSTR_XSLZK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_WEEK_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_ALARM_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_CLOCK_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_AUTO_MEASURE_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_BACKLIGHT_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_DRUG_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_DRINK_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_ALARM_MODE_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_ONOFF_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_COMFORT_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEPLIGHT_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_ASK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_SLEEP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_DRUG[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_DRINK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_RTC_LOW[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_INSTALL_SIMCARD[LANGUAGE_E_MAXNUM];
extern char const *pSTR_GPRS_DATA_UPLOADED_OK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_GPRS_DATA_UPLOADED_ERR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_GPRS_DATA_UPLOADING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_WAIT_NET[LANGUAGE_E_MAXNUM];
extern char const *pSTR_NOT_VALID_DATA[LANGUAGE_E_MAXNUM];
extern char const *pSTR_ASK_CANCEL_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_ASK_STOP_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_INVALID_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_HINT_RESET_AUTOWORK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_DAYMOVECURVE_HEAD1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_DAYMOVECURVE_HEAD2[LANGUAGE_E_MAXNUM];
extern char const *pSTR_DAYMOVECURVE_HEAD3[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR2[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR3[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR4[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR5[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_STR6[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_GRADE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_SCORE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_TOTAL[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_BODYMOVE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_WAKE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_LIGHTSLEEP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_MSLEEP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_DEEPSLEEP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_NUM[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_WAKE1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_LIGHTSLEEP1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_MSLEEP1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SLEEP_CURVE_DEEPSLEEP1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_BHCURVE_STR1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_BHCURVE_STR2[LANGUAGE_E_MAXNUM];
extern char const *pSTR_BHCURVE_STR3[LANGUAGE_E_MAXNUM];
extern char const *pSTR_BHCURVE_STR4[LANGUAGE_E_MAXNUM];
extern char const *pSTR_BHCURVE_STR5[LANGUAGE_E_MAXNUM];
extern char const *pSTR_SENDED[LANGUAGE_E_MAXNUM];
extern char const *pSTR_NO_SEND[LANGUAGE_E_MAXNUM];
//UTF8
extern char const *pSTR_UTF8_XSLZK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_YEAR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_MONTH[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_DAY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_WEEK_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_TEMP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HUMI[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_CLOCK_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_AUTO_MEASURE_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SLEEP_BACKLIGHT_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SLEEP_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_DRUG_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_DRINK_WARN_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ALARM_MODE_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ONOFF_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_COMFORT_BUF[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SLEEPLIGHT_BUF[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ASK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_GETUP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_MONITORING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_MONITOR_OVER[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_BLE_UPLOADING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_BLE_UPLOAD_OK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_BLE_UPLOAD_ERR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_BLE_CONNECT_ERR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_OPENBLE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_THI[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SIGN_PERCENT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SIGN_TEMP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_DATA_ANALYSIS[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_GPRS_WAITING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_GPRS_NET_ERR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_GPRS_UPLOADING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_QR_CODE_BIND[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_QR_CODE_BIND1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_QR_CODE_SALES[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_QR_CODE_SALES1[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_QR_CODE_SALES2[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ENTER_PWR_STOP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_UPDATA_POWER[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_JTNR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_XBCZ[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_ESC_RETURN[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_ESC_RETURN_RESET[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_RTC_LOW[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_INSTALL_SIMCARD[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_SLEEP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_NOT_VALID_DATA[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ASK_STOP_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ASK_STOP_KEY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_INVALID_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ASK_CANCEL_MEASURE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_ASK_CANCEL_MEASURE_KEY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_WAIT_NET[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_RESET_AUTOWORK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_PWR_STOP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_PWR_STOP_KEY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_SLEEP_BACKLIGHT_TOO_BRIGHT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_BELL[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_BELL_KEY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_DATA_FULL[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_HINT_DATA_FULL_KEY[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SLEEP_MARK_REPORT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_SN[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_PROJECT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_FULL_SCORE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_GET_SCORE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_MONITOR_RESULT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_DEFLECT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_SLEEP_STRUCT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_TOTAL_SLEEP_TIME[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_INTO_SLEEP_TIME[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_BODY_MOVEMENT_CMT[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_WAKE_STATE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_BEGIN_SLEEP_TIME[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_TEMP[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_HUMI[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_TOTAL_SCORE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SMR_FINAL_SCORE[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_AIR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_AIR_QUALITY[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_WIND_DIRECTION[][LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SOS_SENDING[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SOS_SENDOK[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_SOS_SENDERR[LANGUAGE_E_MAXNUM];
extern char const *pSTR_UTF8_INVALID_DATA[LANGUAGE_E_MAXNUM];
//---------------------------------------------------------------------------------
extern void Language_Init(void);
extern void Language_Set(uint8_t language);
//---------------------------------------------------------------------------------
#endif

/**********************************************************************************
      END FILE
***********************************************************************************/


