/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-26
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "Bsp_NrfRfEsb.h"

#include "nrf_esb.h"

#include <stdbool.h>
#include <stdint.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_esb_error_codes.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
//#include "boards.h"

#include "Bsp_Led.h"
#include "Bsp_Key.h"
//#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

uint8_t led_nr;
nrf_esb_payload_t rx_payload;
//----------------------------------------------------
//-----------------------------------------------xsl add
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "uctsk_Debug.h"
#define RX_MAX_NUM  6
static uint32_t RxCounterBuf[RX_MAX_NUM] = {0};
static uint32_t RxCounterAll=0;
static const uint8_t FREQ_BUF[RX_MAX_NUM]= {2,3,26,27,79,80};
static uint8_t FreqIndex=0;
static uint32_t esb_init( uint8_t ch ,nrf_esb_mode_t tx_or_rx);
//static uint8_t timer_100ms_flag=0;
#define COMPARE_COUNTERTIME  (3UL)

static nrf_esb_payload_t tx_payload = NRF_ESB_CREATE_PAYLOAD(0,21,0,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 0,0);
static uint8_t BspNrfRfEsb_DebugTestWave_Enable=0;
static uint8_t BspNrfRfEsb_DebugTest_Enable=0;
//-----------------------------------------------
void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            NRF_LOG_DEBUG("TX SUCCESS EVENT\r\n");
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            NRF_LOG_DEBUG("TX FAILED EVENT\r\n");
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
#if (defined(XKAP_ICARE_A_M))
                if(rx_payload.data[0]==5)
                {
                    BSP_LED_LED2_TOGGLE;
                }
                else if(rx_payload.data[0]==0)
                {
                    BSP_LED_LED3_TOGGLE;
                }
                RxCounterBuf[FreqIndex]++;
#endif
#ifdef   BSP_NRFRFESB_RX_PARSE_PRO
                BSP_NRFRFESB_RX_PARSE_PRO(rx_payload.data,rx_payload.length);
#endif
            }
            RxCounterAll++;
            if(BspNrfRfEsb_DebugTest_Enable==1)
            {
                DebugOutHex("NrfRf-Rx:",rx_payload.data,rx_payload.length);
            }
            break;
    }
    //NRF_GPIO->OUTCLR = 0xFUL << 12;
    //NRF_GPIO->OUTSET = (p_event->tx_attempts & 0x0F) << 12;
}
static uint32_t esb_init( uint8_t ch ,nrf_esb_mode_t tx_or_rx)
{
    uint32_t err_code;
    ch = ch;
    tx_or_rx = tx_or_rx;
    RxCounterBuf[0]=RxCounterBuf[0];
    // 基地址0
    uint8_t base_addr_0[4] = BSP_NRFRFESB_BASEADDR_0;
    // 基地址1
    uint8_t base_addr_1[4] = BSP_NRFRFESB_BASEADDR_1;
    // 引导
    uint8_t addr_prefix[8] = BSP_NRFRFESB_PREFIX;
    // 配置
    nrf_esb_config_t nrf_esb_config         = BSP_NRF_ESB_DEFAULT_CONFIG;
    // 配置 ---   主接收模式
    nrf_esb_config.mode                     = tx_or_rx;
    // 初始化  ---   配置
    err_code = nrf_esb_init(&nrf_esb_config);
    VERIFY_SUCCESS(err_code);
    // 设置 ---   基地址0
    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);
    // 设置 ---   基地址1
    err_code = nrf_esb_set_base_address_1(base_addr_1);
    VERIFY_SUCCESS(err_code);
    // 设置 ---   地址长度
    err_code = nrf_esb_set_address_length(5);
    //VERIFY_SUCCESS(err_code);
    // 设置 ---   引导
    err_code = nrf_esb_set_prefixes(addr_prefix, 2);
    VERIFY_SUCCESS(err_code);
    // 设置 ---     通道
    err_code = nrf_esb_set_rf_channel(ch);
    VERIFY_SUCCESS(err_code);
    // 返回错误码
    return err_code;
}
void Bsp_NrfRfEsb_ClocksStart( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}
void Bsp_NrfRfEsb_Init(void)
{
    uint32_t err_code;
    // 初始化  ---   时钟
    Bsp_NrfRfEsb_ClocksStart();
    // 初始化  ---   定时器
    //Bsp_NrfRfEsb_TimerInit();
    // 初始化  ---   esb
#if   (defined(XKAP_ICARE_A_M)||defined(XKAP_ICARE_B_M))
    FreqIndex= BSP_NRFRFESB_USER_CH_NUM;
    err_code = esb_init(FREQ_BUF[FreqIndex],NRF_ESB_MODE_PRX);
    APP_ERROR_CHECK(err_code);
    // 启动    ---   esb接收
    err_code = nrf_esb_start_rx();
    APP_ERROR_CHECK(err_code);
#elif (defined(XKAP_ICARE_A_S)||defined(XKAP_ICARE_A_C))
    if(BSP_NRFRFESB_USER_CH_NUM!=0xFF)
    {
        FreqIndex= BSP_NRFRFESB_USER_CH_NUM;
        err_code = esb_init(FREQ_BUF[FreqIndex],NRF_ESB_MODE_PTX);
    }
    else
    {
        for(err_code=0; err_code<RX_MAX_NUM; err_code++)
        {
            if(BSP_NRFRFESB_USER_CH==FREQ_BUF[err_code])
            {
                FreqIndex=err_code;
                break;
            }
        }
        err_code = esb_init(BSP_NRFRFESB_USER_CH,NRF_ESB_MODE_PTX);
    }
    APP_ERROR_CHECK(err_code);
#elif (defined(XKAP_ICARE_B_C))
    FreqIndex= BSP_NRFRFESB_USER_CH_NUM;
    err_code = esb_init(FREQ_BUF[FreqIndex],NRF_ESB_MODE_PTX);
    APP_ERROR_CHECK(err_code);
#endif
}
void Bsp_NrfRfEsb_Tx(uint8_t *pbuf,uint8_t len)
{
    tx_payload.noack = true;
    tx_payload.length=len;
    memcpy((char*)tx_payload.data,(char*)pbuf,len);
    if(nrf_esb_write_payload(&tx_payload) == NRF_SUCCESS)
    {
        ;
    }
}
void Bsp_NrfRfEsb_1ms(void)
{
    uint32_t err_code;
    static uint8_t si=0;
    err_code = err_code;
    si++;
    if(si!=10)
    {
        return;
    }
    si=0;
#if   (defined(XKAP_ICARE_A_M))
    //频率切换
    NRF_LOG_DEBUG("RxCounter: %d %d %d %d %d %d\r\n",\
                  RxCounterBuf[0],\
                  RxCounterBuf[1],\
                  RxCounterBuf[2],\
                  RxCounterBuf[3],\
                  RxCounterBuf[4],\
                  RxCounterBuf[5]);
    FreqIndex++;
    if(FreqIndex>=RX_MAX_NUM)
    {
        FreqIndex=0;
    }
    //nrf_esb_set_rf_channel(FREQ_BUF[FreqIndex]);
    do
    {
        err_code=nrf_esb_flush_rx();
        if(err_code!=NRF_SUCCESS)
            continue;
        err_code=nrf_esb_stop_rx();
        if(err_code!=NRF_SUCCESS)
            continue;
        err_code=esb_init(FREQ_BUF[FreqIndex],NRF_ESB_MODE_PRX);
        if(err_code!=NRF_SUCCESS)
            continue;
        err_code=nrf_esb_start_rx();
        if(err_code!=NRF_SUCCESS)
            continue;
        break;
    }
    while(1);
#elif (defined(XKAP_ICARE_A_S)||defined(XKAP_ICARE_A_C))
    tx_payload.noack = true;
    tx_payload.length=21;
    //tx_payload.length=10;
    if(nrf_esb_write_payload(&tx_payload) == NRF_SUCCESS)
    {
        BSP_LED_LED2_TOGGLE;
    }
#endif
}
void Bsp_NrfRfEsb_AD(uint16_t value)
{
    int8_t pbuf[20];
    uint16_t i16;
    static uint16_t scnt=0;
    static uint32_t si32=0,sj32=0;
    static uint32_t sbuf[6]= {0};
    static uint8_t si=0;
    //去值绝对曲线
    i16=abs(value-512);
    //10去值累加值
    if(scnt==0)
    {
        si32=0;
    }
    si32+=i16;
    scnt++;
    if(scnt>=1000)
    {
        scnt=0;
        sj32=si32;
        sbuf[si]=si32;
        si++;
        if(si>=6)
        {
            si=0;
            //tx_payload.data[0]=BSP_NRFRFESB_USER_CH;
            tx_payload.data[0]=FreqIndex;
            tx_payload.data[1]=sbuf[0]>>16;
            tx_payload.data[2]=sbuf[0]>>8;
            tx_payload.data[3]=sbuf[0];
            tx_payload.data[4]=sbuf[1]>>16;
            tx_payload.data[5]=sbuf[1]>>8;
            tx_payload.data[6]=sbuf[1];
            tx_payload.data[7]=sbuf[2]>>16;
            tx_payload.data[8]=sbuf[2]>>8;
            tx_payload.data[9]=sbuf[2];
            tx_payload.data[10]=sbuf[3]>>16;
            tx_payload.data[11]=sbuf[3]>>8;
            tx_payload.data[12]=sbuf[3];
            tx_payload.data[13]=sbuf[4]>>16;
            tx_payload.data[14]=sbuf[4]>>8;
            tx_payload.data[15]=sbuf[4];
            tx_payload.data[16]=sbuf[5]>>16;
            tx_payload.data[17]=sbuf[5]>>8;
            tx_payload.data[18]=sbuf[5];
            tx_payload.data[19]=Count_Sum(0,tx_payload.data,19);
            tx_payload.data[20]+=1;

            /*
            tx_payload.data[0]=1;
            tx_payload.data[1]=2;
            tx_payload.data[2]=3;
            tx_payload.data[3]=4;
            tx_payload.data[4]=5;
            tx_payload.data[5]=6;
            tx_payload.data[6]=7;
            tx_payload.data[7]=8;
            tx_payload.data[8]=9;
            tx_payload.data[9]=10;
            tx_payload.data[10]=11;
            tx_payload.data[11]=12;
            tx_payload.data[12]=13;
            tx_payload.data[13]=14;
            tx_payload.data[14]=15;
            tx_payload.data[15]=16;
            tx_payload.data[16]=17;
            tx_payload.data[17]=18;
            tx_payload.data[18]=19;
            tx_payload.data[19]=20;
            tx_payload.data[20]=21;
            */
        }
    }
    //原始曲线+去值绝对曲线
    if(BspNrfRfEsb_DebugTestWave_Enable==1)
    {
        sprintf((char *)pbuf,"%05d,%05d,%05d\r\n",value,i16,sj32/1000);
        DebugOutStr(pbuf);
    }
}

void Bsp_NrfRfEsb_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspNrfRfEsb_DebugTest_Enable=1;
    }
    else
    {
        BspNrfRfEsb_DebugTest_Enable=0;
    }
}
void Bsp_NrfRfEsb_DebugTestWaveOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspNrfRfEsb_DebugTestWave_Enable=1;
    }
    else
    {
        BspNrfRfEsb_DebugTestWave_Enable=0;
    }
}
