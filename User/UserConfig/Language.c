/*
***********************************************************************************
*                    ��    ��: ������
*                    ����ʱ��: 2015-06-03
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
    "�������ֿ�",
    "XslZk",
};

const char *pSTR_WEEK_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "Sun",
        "������",
        "������",
    },
    {
        "Mon",
        "����һ",
        "������",
    },
    {
        "Tue",
        "���ڶ�",
        "������",
    },
    {
        "Wed",
        "������",
        "ˮ����",
    },
    {
        "Thurs",
        "������",
        "ľ����",
    },
    {
        "Fri",
        "������",
        "������",
    },
    {
        "Sat",
        "������",
        "������",
    },
};
char const *pSTR_ALARM_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "Alarm  1:",
        "��  �� 1:",
        "Alarm  1:",
    },
    {
        "Alarm  2:",
        "��  �� 2:",
        "Alarm  2:",
    },
};
char const *pSTR_CLOCK_BUF[LANGUAGE_E_MAXNUM]=
{
    "DataTime:",
    "����ʱ��:",
    "DataTime:",
};
char const *pSTR_AUTO_MEASURE_BUF[LANGUAGE_E_MAXNUM]=
{
    "AutoWork:",
    "�Զ����:",
    "AutoWork:",
};
char const *pSTR_SLEEP_BACKLIGHT_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Light:",
    "��˯����:",
    "SL Light:",
};
char const *pSTR_SLEEP_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "SL Warn :",
    "��˯����:",
    "SL Warn :",
};
char const *pSTR_DRUG_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DrugWarn:",
    "��ҩ����:",
    "DrugWarn:",
};
char const *pSTR_DRINK_WARN_BUF[LANGUAGE_E_MAXNUM]=
{
    "DK Warn :",
    "��ˮ����:",
    "DK Warn :",
};
char const *pSTR_ALARM_MODE_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF ",
        "�ر�",
        "OFF ",
    },
    {
        "TIM ",
        "��ʱ",
        "TIM ",
    },
    {
        "Auto",
        "����",
        "Auto",
    },
};
char const *pSTR_ONOFF_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF",
        "��",
        "��",
    },
    {
        "ON",
        "��",
        "��",
    },
};
char const *pSTR_COMFORT_BUF[LANGUAGE_E_MAXNUM]=
{
    "Comfort",
    "���ʶ� ",
    "Comfort",
};
char const *pSTR_SLEEPLIGHT_BUF[][LANGUAGE_E_MAXNUM]=
{
    {
        "OFF",
        "�ر�",
        "OFF",
    },
    {
        "LEVEL-1",
        "����",
        "LEVEL-1",
    },
    {
        "LEVEL-2",
        "΢��",
        "LEVEL-2",
    },
};
char const *pSTR_HINT[LANGUAGE_E_MAXNUM]=
{
    "Hint",
    "��ʾ",
    "Hint",
};
char const *pSTR_ASK[LANGUAGE_E_MAXNUM]=
{
    "Ask",
    "ѯ��",
    "Ask",
};
char const *pSTR_HINT_SLEEP[LANGUAGE_E_MAXNUM]=
{
    "Master,Sleep!",
    "����˯����!",
    "Master,Sleep!",
};
char const *pSTR_HINT_DRUG[LANGUAGE_E_MAXNUM]=
{
    "Master,Drug!",
    "���ó�ҩ��!",
    "Master,Drug!",
};
char const *pSTR_HINT_DRINK[LANGUAGE_E_MAXNUM]=
{
    "Master,Drink!",
    "���ú�ˮ��!",
    "Master,Drink!",
};
char const *pSTR_HINT_RTC_LOW[LANGUAGE_E_MAXNUM]=
{
    "Please replace RTC battery!",
    "�����ʱ�ӵ��!",
    "Please replace RTC battery!",  
};
char const *pSTR_HINT_INSTALL_SIMCARD[LANGUAGE_E_MAXNUM]=
{
    "Please Install SimCard!",
    "�밲װSIM��!",
    "Please Install SimCard!",  
};
char const *pSTR_GPRS_DATA_UPLOADED_OK[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded OK!",
    "GPRS�����ϴ��ɹ�!",
    "Gprs Data uploaded OK!",  
};
char const *pSTR_GPRS_DATA_UPLOADED_ERR[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded ERR!",
    "GPRS�����ϴ�ʧ��!",
    "Gprs Data uploaded ERR!",  
};
char const *pSTR_GPRS_DATA_UPLOADING[LANGUAGE_E_MAXNUM]=
{
    "Gprs Data uploaded ...",
    "GPRS�����ϴ���...",
    "Gprs Data uploaded ...",  
};
char const *pSTR_WAIT_NET[LANGUAGE_E_MAXNUM]=
{
    "Waiting for net.\nTry again later!",
    "�ȴ�����,�Ժ�����!",
    "Waiting for net.\nTry again later!",  
};
char const *pSTR_NOT_VALID_DATA[LANGUAGE_E_MAXNUM]=
{
    "There is not valid data!",
    "����Ч����!",
    "There is not valid data!",  
};
char const *pSTR_ASK_CANCEL_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "Cancel measure now?\nPress (Set) Sure",
    "�Ƿ�ȡ�����?\n��(����)��ȷ��",
    "Cancel measure now?\nPress (Set) Sure",
};
char const *pSTR_ASK_STOP_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "Stop measure now?\nPress (Set) Sure",
    "�Ƿ�ֹͣ���?\n��(����)��ȷ��",
    "Stop measure now?\nPress (Set) Sure",
};
char const *pSTR_HINT_INVALID_MEASURE[LANGUAGE_E_MAXNUM]=
{
    "The test is less than 1 hour.\r\nSo is invalid!",
    "�����Ч(ʱ��С��1Сʱ)!",
    "The test is less than 1 hour.\r\nSo is invalid!",
};
char const *pSTR_HINT_RESET_AUTOWORK[LANGUAGE_E_MAXNUM]=
{
    "Please Reset AutoWork Value!\r\n(Diff Range:1-12 hour)",
    "���������Զ����\r\n(���:1-12 Сʱ)",
    "Please Reset AutoWork Value!\r\n(Diff Range:1-12 hour)",
};
char const *pSTR_DAYMOVECURVE_HEAD1[LANGUAGE_E_MAXNUM]=
{
		"%01d: Day Move Curve(%02d/%02d)",
		"��%01d����¼: �嶯��¼(%02d/%02d)",
		"%01d: Day Move Curve(%02d/%02d)",
};
char const *pSTR_DAYMOVECURVE_HEAD2[LANGUAGE_E_MAXNUM]=
{
		"%01d: Day Move Curve(none)",
		"��%01d����¼: �嶯��¼(������)",
		"%01d: Day Move Curve(none)",
};
char const *pSTR_DAYMOVECURVE_HEAD3[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"��%01d����¼: ",
		"%01d:",
};
char const *pSTR_SLEEP_CURVE_STR1[LANGUAGE_E_MAXNUM]=
{
		"%01d",
		"��%01d����¼: ˯�߽ṹ����",
		"%01d",
};
char const *pSTR_SLEEP_CURVE_STR2[LANGUAGE_E_MAXNUM]=
{
		"%01d",
		"��%01d����¼: ",
		"%01d",
};
char const *pSTR_SLEEP_CURVE_STR3[LANGUAGE_E_MAXNUM]=
{
		"None",
		"��������,����",
		"None",
};
char const *pSTR_SLEEP_CURVE_STR4[LANGUAGE_E_MAXNUM]=
{
		"%02d-%02d %02d:%02d - %02d-%02d %02d:%02d",
		"���ʱ��:%02d��%02d��%02d:%02d - %02d��%02d��%02d:%02d",
		"%02d-%02d %02d:%02d - %02d-%02d %02d:%02d",
};
char const *pSTR_SLEEP_CURVE_STR5[LANGUAGE_E_MAXNUM]=
{
		"None",
		"�����Ч",
		"None",
};
char const *pSTR_SLEEP_CURVE_STR6[LANGUAGE_E_MAXNUM]=
{
		"None",
		"��ע���⻷������ϵ�ͷ�",
		"None",
};
char const *pSTR_SLEEP_CURVE_GRADE[LANGUAGE_E_MAXNUM]=
{
		"GRADE",
		"����",
		"GRADE",
};
char const *pSTR_SLEEP_CURVE_SCORE[LANGUAGE_E_MAXNUM]=
{
		"SCORE",
		"��",
		"SCORE",
};
char const *pSTR_SLEEP_CURVE_TOTAL[LANGUAGE_E_MAXNUM]=
{
		"TOTAL:",
		"��˯:",
		"TOTAL:",
};
char const *pSTR_SLEEP_CURVE_BODYMOVE[LANGUAGE_E_MAXNUM]=
{
		"BODYMOVE:",
		"�嶯:",
		"BODYMOVE:",
};
char const *pSTR_SLEEP_CURVE_WAKE[LANGUAGE_E_MAXNUM]=
{
		"Wake:",
		"����:",
		"wake:",
};
char const *pSTR_SLEEP_CURVE_LIGHTSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Light Sleep:",
		"ǳ˯:",
		"Light Sleep:",
};
char const *pSTR_SLEEP_CURVE_MSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Middle Sleep:",
		"��˯:",
		"Middle Sleep:",
};
char const *pSTR_SLEEP_CURVE_DEEPSLEEP[LANGUAGE_E_MAXNUM]=
{
		"Deep Sleep:",
		"��˯:",
		"Deep Sleep:",
};
char const *pSTR_SLEEP_CURVE_NUM[LANGUAGE_E_MAXNUM]=
{
		"NUM",
		"��",
		"NUM",
};
char const *pSTR_SLEEP_CURVE_WAKE1[LANGUAGE_E_MAXNUM]=
{
		"Wake:",
		"�� ��",
		"wake:",
};
char const *pSTR_SLEEP_CURVE_LIGHTSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Light Sleep:",
		"ǳ ˯",
		"Light Sleep:",
};
char const *pSTR_SLEEP_CURVE_MSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Middle Sleep:",
		"�� ˯",
		"Middle Sleep:",
};
char const *pSTR_SLEEP_CURVE_DEEPSLEEP1[LANGUAGE_E_MAXNUM]=
{
		"Deep Sleep:",
		"�� ˯",
		"Deep Sleep:",
};
char const *pSTR_BHCURVE_STR1[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"��%01d����¼: ������������",
		"%01d:",
};
char const *pSTR_BHCURVE_STR2[LANGUAGE_E_MAXNUM]=
{
		"%01d:",
		"��%01d����¼: ",
		"%01d:",
};
char const *pSTR_BHCURVE_STR3[LANGUAGE_E_MAXNUM]=
{
		"Breath",
		"�� ��",
		"Breath",
};
char const *pSTR_BHCURVE_STR4[LANGUAGE_E_MAXNUM]=
{
		"num/min",
		"(��/��)",
		"num/min",
};
char const *pSTR_BHCURVE_STR5[LANGUAGE_E_MAXNUM]=
{
		"Heart",
		"�� ��",
		"Heart",
};
char const *pSTR_SENDED[LANGUAGE_E_MAXNUM]=
{
		"Sended",
		"(�Ѵ�)",
		"Sended",
};
char const *pSTR_NO_SEND[LANGUAGE_E_MAXNUM]=
{
		"Not Send",
		"(����)",
		"Not Send",
};
//----------------------------------------------
