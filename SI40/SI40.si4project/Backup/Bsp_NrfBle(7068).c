/*
***********************************************************************************
*                    作    者: 徐松亮
*                    更新时间: 2015-05-26
***********************************************************************************
*/
//------------------------------- Includes -----------------------------------
#include "Bsp_NrfBle.h"
#include "uctsk_Debug.h"
#include "ble_gap.h"
//------------------------------- Define -------------------------------------
//------------------------------- 局部变量 -----------------------------------
// 广播
#if   (defined(BSP_NRFBLE_ENABLE_ADVERTISE))
// 广播参数(Parameters to be passed to the stack when starting advertising)
static ble_gap_adv_params_t m_adv_params;
// 广播信息(Information advertised by the Beacon)
static uint8_t m_advertise_info[APP_ADV_INFO_LENGTH] = {0};
// -----XSL-----
static uint8_t BspNrfBle_TxSerialNum   =  0;
BSP_NRFBLE_S_MANUF_DATA BspNrfBle_s_Tx;
// -------------
#endif
// Beacon
#if   (defined(BSP_NRFBLE_ENABLE_BEACON))
// 广播参数
static ble_gap_adv_params_t m_adv_params;
// 广播信息
static uint8_t m_advertise_info[APP_ADV_INFO_LENGTH] =
{
    APP_DEVICE_TYPE,     // Manufacturer specific information. Specifies the device type in this
    // implementation.
    APP_ADV_DATA_LENGTH, // Manufacturer specific information. Specifies the length of the
    // manufacturer specific data in this implementation.
    APP_BEACON_UUID,     // 128 bit UUID value.
    APP_MAJOR_VALUE,     // Major arbitrary value that can be used to distinguish between Beacons.
    APP_MINOR_VALUE,     // Minor arbitrary value that can be used to distinguish between Beacons.
    APP_MEASURED_RSSI    // Manufacturer specific information. The Beacon's measured TX power in
    // this implementation.
};
#endif
// 扫描
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
/**
 * @brief Parameters used when scanning.
 */
static const ble_gap_scan_params_t m_scan_params =
{
    .active   = 1,
    .interval = SCAN_INTERVAL,
    .window   = SCAN_WINDOW,
    .timeout  = SCAN_TIMEOUT,
#if (NRF_SD_BLE_API_VERSION == 2)
    .selective   = 0,
    .p_whitelist = NULL,
#endif
#if (NRF_SD_BLE_API_VERSION == 3)
    .use_whitelist = 0,
#endif
};
#endif
// 定时器
#if   (defined(OS_FREERTOS))
#include "Bsp_Rtc.h"
#include "timers.h"
static TimerHandle_t m_rtc_timer;
#ifdef   BSP_NRFBLE_ENABLE_TIMER_1MS
static TimerHandle_t m_1ms_timer;
#endif
#ifdef   BSP_NRFBLE_ENABLE_TIMER_10MS
static TimerHandle_t m_10ms_timer;
#endif
#else
app_timer_id_t SecondID;
#define ONESECOND_INTERVAL APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)
#endif
// Debug
static uint8_t BspNrfBle_DebugTest_Enable=0;
// 项目
#if   (defined(XKAP_ICARE_B_D))
#define BSP_NRFBLE_BUFFERLOOP_BUF_MAX   256
#define BSP_NRFBLE_BUFFERLOOP_NODE_MAX  10
static uint8_t BspNrfBle_BufferLoop_Buf[BSP_NRFBLE_BUFFERLOOP_BUF_MAX];
static COUNT_BUFFERLOOP_S_NODE BspNrfBle_BufferLoop_Node[BSP_NRFBLE_BUFFERLOOP_NODE_MAX];
static COUNT_BUFFERLOOP_S_LIMIT BspNrfBle_BufferLoop_Limit;
static COUNT_BUFFERLOOP_S BspNrfBle_BufferLoop;
static uint8_t *pBspNrfBle_UartTx=NULL;
#endif
//-------------------------------
//------------------------------- 函数 ---------------------------------------
#if   (!defined(BSP_NRFBLE_DISABLE))
static void BspNrfBle_AdvDataParse(const ble_gap_evt_adv_report_t *ps);
static void on_ble_evt(ble_evt_t * p_ble_evt);

#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
static ble_nus_c_t              m_ble_nus_c;                    /**< Instance of NUS service. Must be passed to all NUS_C API calls. */
//static ble_db_discovery_t       m_ble_db_discovery;             /**< Instance of database discovery module. Must be passed to all db_discovert API calls */
#endif

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}



/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
#if   (defined(BSP_NRFBLE_ENABLE_BEACON)||defined(BSP_NRFBLE_ENABLE_ADVERTISE))
    uint32_t      err_code;
    ble_advdata_t advdata;
    uint8_t       flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

    ble_advdata_manuf_data_t manuf_specific_data;

    // -----赋值用户数据
    //
    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;

#if   (defined(USE_UICR_FOR_MAJ_MIN_VALUES)&&defined(BSP_NRFBLE_ENABLE_BEACON))
    // If USE_UICR_FOR_MAJ_MIN_VALUES is defined, the major and minor values will be read from the
    // UICR instead of using the default values. The major and minor values obtained from the UICR
    // are encoded into advertising data in big endian order (MSB First).
    // To set the UICR used by this example to a desired value, write to the address 0x10001080
    // using the nrfjprog tool. The command to be used is as follows.
    // nrfjprog --snr <Segger-chip-Serial-Number> --memwr 0x10001080 --val <your major/minor value>
    // For example, for a major value and minor value of 0xabcd and 0x0102 respectively, the
    // the following command should be used.
    // nrfjprog --snr <Segger-chip-Serial-Number> --memwr 0x10001080 --val 0xabcd0102
    uint16_t major_value = ((*(uint32_t *)UICR_ADDRESS) & 0xFFFF0000) >> 16;
    uint16_t minor_value = ((*(uint32_t *)UICR_ADDRESS) & 0x0000FFFF);

    uint8_t index = MAJ_VAL_OFFSET_IN_BEACON_INFO;

    m_advertise_info[index++] = MSB_16(major_value);
    m_advertise_info[index++] = LSB_16(major_value);

    m_advertise_info[index++] = MSB_16(minor_value);
    m_advertise_info[index++] = LSB_16(minor_value);
#endif

    manuf_specific_data.data.p_data = (uint8_t *) m_advertise_info;
    manuf_specific_data.data.size   = APP_ADV_INFO_LENGTH;

    // -----赋值广播数据
    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));
    // 广播名称设置(无名/短名/长名)
    advdata.name_type             = BLE_ADVDATA_NO_NAME;
    //
    advdata.flags                 = flags;
    advdata.p_manuf_specific_data = &manuf_specific_data;

    // -----设置广播数据
    err_code = ble_advdata_set(&advdata, NULL);
    APP_ERROR_CHECK(err_code);

    // -----初始化广播参数
    // Initialize advertising parameters (used when starting advertising).
    memset(&m_adv_params, 0, sizeof(m_adv_params));

    // 广播类型
    //   BLE_GAP_ADV_TYPE_ADV_IND         -  无向连接
    //   BLE_GAP_ADV_TYPE_ADV_DIRECT_IND  -  有向连接
    //   BLE_GAP_ADV_TYPE_ADV_SCAN_IND    -  无向扫描
    //   BLE_GAP_ADV_TYPE_ADV_NONCONN_IND -  非无向连接
    m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
    // Undirected advertisement.
    m_adv_params.p_peer_addr = NULL;
    // 广播滤波类型
    //   BLE_GAP_ADV_FP_ANY               -  允许任何设备的扫描请求与连接请求
    //   BLE_GAP_ADV_FP_FILTER_SCANREQ    -  允许白名单扫描请求
    //   BLE_GAP_ADV_FP_FILTER_CONNREQ    -  允许白名单连接请求
    //   BLE_GAP_ADV_FP_FILTER_BOTH       -  允许白名单扫描与连接请求
    m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
    // 发送间隔
    m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
    // 发送超时
    m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
#endif
    // 变量初始化
    memset((char*)&BspNrfBle_s_Tx,0,sizeof(BspNrfBle_s_Tx));
    // 防止报错
    BspNrfBle_TxSerialNum = BspNrfBle_TxSerialNum;
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
#if   (defined(BSP_NRFBLE_ENABLE_BEACON)||defined(BSP_NRFBLE_ENABLE_ADVERTISE))
    uint32_t err_code;

    err_code = sd_ble_gap_adv_start(&m_adv_params);
    APP_ERROR_CHECK(err_code);

    //err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
    //APP_ERROR_CHECK(err_code);
#endif
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}
#endif
/** @brief Function for initializing the Database Discovery Module.
 */
static void db_discovery_init(void)
{
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
    uint32_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);
#endif
}

/**@brief Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 *
 * @details This function is called from the scheduler in the main loop after a BLE stack event has
 *          been received.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void ble_evt_dispatch(ble_evt_t * p_ble_evt)
{
    on_ble_evt(p_ble_evt);
    //bsp_btn_ble_on_ble_evt(p_ble_evt);
    //ble_db_discovery_on_ble_evt(&m_ble_db_discovery, p_ble_evt);
    //ble_nus_c_on_ble_evt(&m_ble_nus_c,p_ble_evt);
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */

static void ble_stack_init(void)
{
    uint32_t err_code;
    ble_enable_params_t ble_enable_params;
    // -----使能低频时钟源
    // SD使用的低频时钟源
    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
    // 初始化SD时钟与句柄
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);
    // -----获取默认配置
    //ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
               PERIPHERAL_LINK_COUNT,
               &ble_enable_params);
    APP_ERROR_CHECK(err_code);
    // -----监测SD的RAM需求
    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    // -----使能堆栈
    // Enable BLE stack.
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
    // -----注册回调事件句柄
    // 注册BLE事件
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);
#endif
#if   (defined(BSP_NRFBLE_ENABLE_WAIT))
    // 注册系统事件
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
#endif
}

/**@brief Function to start scanning.
 */
static void scan_start(void)
{
#if   (defined(BSP_NRFBLE_ENABLE_SCAN))
    ret_code_t ret;

    ret = sd_ble_gap_scan_start(&m_scan_params);
    APP_ERROR_CHECK(ret);

    //ret = bsp_indication_set(BSP_INDICATE_SCANNING);
    //APP_ERROR_CHECK(ret);
#endif
}

/**@brief Function for doing power management.
 */
/*
static void power_manage(void)
{
    uint32_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}
*/
#endif
#if   (defined(BSP_NRFBLE_ENABLE_WAIT))
static void sys_evt_dispatch(uint32_t sys_evt)
{
    fs_sys_event_handler(sys_evt);
    ble_advertising_on_sys_evt(sys_evt);
}
/**@brief Function for initializing the NUS Client.
 */
static void nus_c_init(void)
{
    uint32_t         err_code;
    ble_nus_c_init_t nus_c_init_t;

    nus_c_init_t.evt_handler = ble_nus_c_evt_handler;

    err_code = ble_nus_c_init(&m_ble_nus_c, &nus_c_init_t);
    APP_ERROR_CHECK(err_code);
}
/**@brief Callback handling NUS Client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS Client Handle. This identifies the NUS client
 * @param[in]   p_ble_nus_evt Pointer to the NUS Client event.
 */

/**@snippet [Handling events from the ble_nus_c module] */
static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, const ble_nus_c_evt_t * p_ble_nus_evt)
{
    uint32_t err_code;
    switch (p_ble_nus_evt->evt_type)
    {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nus_c_rx_notif_enable(p_ble_nus_c);
            APP_ERROR_CHECK(err_code);
            printf("The device has the Nordic UART Service\r\n");
            break;

        case BLE_NUS_C_EVT_NUS_RX_EVT:
            for (uint32_t i = 0; i < p_ble_nus_evt->data_len; i++)
            {
                //while (app_uart_put( p_ble_nus_evt->p_data[i]) != NRF_SUCCESS);
            }
            break;

        case BLE_NUS_C_EVT_DISCONNECTED:
            printf("Disconnected\r\n");
            scan_start();
            break;
    }
}
#endif
/*
******************************************************************************
* 函数功能: 初始化
******************************************************************************
*/

static void BspNrfBle_Handler_Timer_RTC(void * p_context);
#ifdef   BSP_NRFBLE_ENABLE_TIMER_1MS
static void BspNrfBle_Handler_Timer_1ms(void * p_context);
#endif
#ifdef   BSP_NRFBLE_ENABLE_TIMER_10MS
static void BspNrfBle_Handler_Timer_10ms(void * p_context);
#endif
void BspNrfBle_Init(void)
{
#if   (defined(OS_FREERTOS))
    m_rtc_timer = xTimerCreate("RTC-1s",
                               1000,
                               pdTRUE,
                               NULL,
                               BspNrfBle_Handler_Timer_RTC);
    if ( NULL == m_rtc_timer)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    if (pdPASS != xTimerStart(m_rtc_timer, 2))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    // 定时器
#ifdef   BSP_NRFBLE_ENABLE_TIMER_10MS
    m_10ms_timer = xTimerCreate("10ms",
                                10,
                                pdTRUE,
                                NULL,
                                BspNrfBle_Handler_Timer_10ms);
    if ( NULL == m_10ms_timer)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    if (pdPASS != xTimerStart(m_10ms_timer, 2))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif
#ifdef   BSP_NRFBLE_ENABLE_TIMER_1MS
    m_1ms_timer = xTimerCreate("1ms",
                               1,
                               pdTRUE,
                               NULL,
                               BspNrfBle_Handler_Timer_1ms);
    if ( NULL == m_1ms_timer)
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
    if (pdPASS != xTimerStart(m_1ms_timer, 2))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif

#else
    uint32_t err_code;
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
    err_code = app_timer_create(&SecondID, APP_TIMER_MODE_REPEATED, BspNrfBle_Handler_Timer_RTC);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(SecondID, ONESECOND_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
#endif

#if   (!defined(BSP_NRFBLE_DISABLE))
    // SD协议栈初始化
    ble_stack_init();

    // 数据库探索模块(扫描应用使用)
    db_discovery_init();
    // Service注册
    // nus_c_init();
    scan_start();
    // 必须
    //gap_params_init();
    // 跟自己创建的服务相关，不同的服务细节不同但大体建立
    //services_init();
    // 广播初始化(设置广播数据以及扫描相应数据)
    advertising_init();
    // 是情况而定，如果连接后不需要连接参数的协商，该初始化也可不要
    //conn_params_init();
    // 安全参数初始化,如果没用到配对绑定相关这个也可以不初始化
    //sec_params_init();
    // 启动广播(设置广播类型、白名单、间隔、超时等特性，并开始广播)
    advertising_start();
#endif
    //读取与修改本地MAC地址
#ifdef  NRF51
    {
        uint8_t *pbuf=NULL;
        ble_gap_addr_t ble_addr;
        sd_ble_gap_address_get(&ble_addr);
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        sprintf((char*)pbuf,"NRFBLE:Ble-Local-Mac %02X:%02X:%02X:%02X:%02X:%02X\r\n"\
                ,ble_addr.addr[0]\
                ,ble_addr.addr[1]\
                ,ble_addr.addr[2]\
                ,ble_addr.addr[3]\
                ,ble_addr.addr[4]\
                ,ble_addr.addr[5]);
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
        /*
        ble_addr.addr[0]=0x01;
        ble_addr.addr[1]=0x23;
        ble_addr.addr[2]=0x45;
        ble_addr.addr[3]=0x67;
        ble_addr.addr[4]=0x89;
        ble_addr.addr[5]=0xab|0xC0;
        err_code=sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE,&ble_addr);
        if(err_code==NRF_SUCCESS)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:NRF_SUCCESS\r\n");
        }
        else if(err_code==NRF_ERROR_INVALID_ADDR)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:NRF_ERROR_INVALID_ADDR\r\n");
        }
        else if(err_code==NRF_ERROR_INVALID_PARAM)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:NRF_ERROR_INVALID_PARAM\r\n");
        }
        else if(err_code==BLE_ERROR_GAP_INVALID_BLE_ADDR)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:BLE_ERROR_GAP_INVALID_BLE_ADDR\r\n");
        }
        else if(err_code==NRF_ERROR_BUSY)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:NRF_ERROR_BUSY\r\n");
        }
        else if(err_code==NRF_ERROR_INVALID_STATE)
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:NRF_ERROR_INVALID_STATE\r\n");
        }
        else
        {
            sprintf((char*)pbuf,"sd_ble_gap_address_set:%ld\r\n",i32);
        }
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,pbuf);
        */
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#endif
    //读取与修改发射功率
#if   (defined(BSP_NRFBLE_TXPOWER))
    {
        uint8_t *pbuf=NULL;
        uint32_t err_code;
        pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
        err_code = sd_ble_gap_tx_power_set(BSP_NRFBLE_TXPOWER);
        if(err_code==NRF_SUCCESS)
        {
            sprintf((char*)pbuf,"NRFBLE:sd_ble_gap_tx_power_set:NRF_SUCCESS(%d db)\r\n",BSP_NRFBLE_TXPOWER);
        }
        else if(err_code == NRF_ERROR_INVALID_PARAM)
        {
            sprintf((char*)pbuf,"NRFBLE:sd_ble_gap_tx_power_set:NRF_ERROR_INVALID_PARAM\r\n");
        }
        else
        {
            sprintf((char*)pbuf,"NRFBLE:sd_ble_gap_tx_power_set:NULL\r\n");
        }
        DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
    }
#endif
#if   (defined(XKAP_ICARE_B_D))
    // 硬件初始化
    UART_INIT(BSP_NRFBLE_UART_X,BSP_NRFBLE_UART_BSP);
    // 数据初始化
    Count_ModbusSlaveAddr = BSP_NRFBLE_UART_ADDR;
    //初始化数据结构
    Count_BufferLoopInit(&BspNrfBle_BufferLoop,\
                         BspNrfBle_BufferLoop_Buf,\
                         BSP_NRFBLE_BUFFERLOOP_BUF_MAX,\
                         BspNrfBle_BufferLoop_Node,\
                         BSP_NRFBLE_BUFFERLOOP_NODE_MAX,\
                         &BspNrfBle_BufferLoop_Limit);
#endif
}
/*
******************************************************************************
* 函数功能: 回调函数
******************************************************************************
*/
extern uint32_t   Rtc_Timer_s;
static void BspNrfBle_Handler_Timer_RTC(void * p_context)
{
    (void)p_context;
    BspRtc_1000ms();
    Rtc_Timer_s++;
}
//
#ifdef   BSP_NRFBLE_ENABLE_TIMER_1MS
static void BspNrfBle_Handler_Timer_1ms(void * p_context)
{
#ifdef   BSP_NRFBLE_TIMER_1MS_CH1
    BSP_NRFBLE_TIMER_1MS_CH1;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH2
    BSP_NRFBLE_TIMER_1MS_CH2;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH3
    BSP_NRFBLE_TIMER_1MS_CH3;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH4
    BSP_NRFBLE_TIMER_1MS_CH4;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH5
    BSP_NRFBLE_TIMER_1MS_CH5;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH6
    BSP_NRFBLE_TIMER_10MS_CH6;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH7
    BSP_NRFBLE_TIMER_1MS_CH7;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH8
    BSP_NRFBLE_TIMER_1MS_CH8;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH9
    BSP_NRFBLE_TIMER_1MS_CH9;
#endif
#ifdef   BSP_NRFBLE_TIMER_1MS_CH10
    BSP_NRFBLE_TIMER_1MS_CH10;
#endif
}
#endif
//
#ifdef   BSP_NRFBLE_ENABLE_TIMER_10MS
static void BspNrfBle_Handler_Timer_10ms(void * p_context)
{
#ifdef   BSP_NRFBLE_TIMER_10MS_CH1
    BSP_NRFBLE_TIMER_10MS_CH1;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH2
    BSP_NRFBLE_TIMER_10MS_CH2;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH3
    BSP_NRFBLE_TIMER_10MS_CH3;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH4
    BSP_NRFBLE_TIMER_10MS_CH4;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH5
    BSP_NRFBLE_TIMER_10MS_CH5;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH6
    BSP_NRFBLE_TIMER_10MS_CH6;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH7
    BSP_NRFBLE_TIMER_10MS_CH7;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH8
    BSP_NRFBLE_TIMER_10MS_CH8;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH9
    BSP_NRFBLE_TIMER_10MS_CH9;
#endif
#ifdef   BSP_NRFBLE_TIMER_10MS_CH10
    BSP_NRFBLE_TIMER_10MS_CH10;
#endif
}
#endif

#ifndef  BSP_NRFBLE_DISABLE
/**@brief Function for handling the Application's BLE Stack events.
 *
 * @param[in] p_ble_evt  Bluetooth stack event.
 */
static void on_ble_evt(ble_evt_t * p_ble_evt)
{
    static uint16_t s_adv_cmt=0;
    uint8_t *pbuf=NULL;
    //uint32_t              err_code;
    // 提取gap(通用访问规范)事件
    const ble_gap_evt_t * p_gap_evt = &p_ble_evt->evt.gap_evt;

    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);

    switch (p_ble_evt->header.evt_id)
    {
        // 已连接
        case BLE_GAP_EVT_CONNECTED:
#if 0
            //NRF_LOG_DEBUG("Connected to target\r\n");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);

            // start discovery of services. The NUS Client waits for a discovery result
            err_code = ble_db_discovery_start(&m_ble_db_discovery, p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);
#endif
            break;
        // 断开连接
        case BLE_GAP_EVT_DISCONNECTED:
            break;
        // 连接参数更新
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            break;
        // 请求提供安全参数
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
#if 0
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
#endif
            break;
        // 请求提供安全信息
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            break;
        // 请求给用户显示密码
        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            break;
        // 远程按键通知
        case BLE_GAP_EVT_KEY_PRESSED:
            break;
        // 请求提供认证密钥
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            break;
        // 请求计算一个LE安全连接DHKey
        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            break;
        // 身份验证过程完成
        case BLE_GAP_EVT_AUTH_STATUS:
            break;
        // 连接安全更新
        case BLE_GAP_EVT_CONN_SEC_UPDATE:
            break;
        // 超时
        case BLE_GAP_EVT_TIMEOUT:
            sprintf((char*)pbuf,"BLE-evt:BLE_GAP_EVT_TIMEOUT\r\n");
            DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
#if 0
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_SCAN)
            {
                //NRF_LOG_DEBUG("Scan timed out.\r\n");
                scan_start();
            }
            else if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                printf("Connection Request timed out.\r\n");
            }
#endif
            break;
        // RSSI变更
        case BLE_GAP_EVT_RSSI_CHANGED:
            sprintf((char*)pbuf,"BLE-evt:BLE_GAP_EVT_RSSI_CHANGED\r\n");
            DebugLogOutStr(DEBUG_E_LEVEL_INFO,(int8_t*)pbuf);
            break;
        // 广播接收报告
        case BLE_GAP_EVT_ADV_REPORT:
        {
            const ble_gap_evt_adv_report_t * p_adv_report = &p_gap_evt->params.adv_report;
            //
            BspNrfBle_AdvDataParse(p_adv_report);
            //
            BSP_NRFBLE_SCAN_RX_LED;
            ++s_adv_cmt;
            if(BspNrfBle_DebugTest_Enable==1)
            {
                // 提取广播报告
                const uint8_t *pstrbuf[4]=
                {
                    "ADV_IND        ",
                    "ADV_DIRECT_IND ",
                    "ADV_SCAN_IND   ",
                    "ADV_NONCONN_IND"
                };
                sprintf((char*)pbuf,"BLE-evt:ADV(cmt-%d)(%s)(%02X:%02X:%02X:%02X:%02X:%02X)(RSSI-%02d)(len-%02d)"\
                        ,s_adv_cmt\
                        ,pstrbuf[p_adv_report->type]\
                        ,p_adv_report->peer_addr.addr[0]\
                        ,p_adv_report->peer_addr.addr[1]\
                        ,p_adv_report->peer_addr.addr[2]\
                        ,p_adv_report->peer_addr.addr[3]\
                        ,p_adv_report->peer_addr.addr[4]\
                        ,p_adv_report->peer_addr.addr[5]\
                        ,p_adv_report->rssi\
                        ,p_adv_report->dlen\
                       );
                DebugOutStr((int8_t*)pbuf);
                DebugOutHex(NULL,(uint8_t*)p_adv_report->data,p_adv_report->dlen);
            }
#if   (defined(XKAP_ICARE_B_D))
            // 压栈
            Count_BufferLoopPush(&BspNrfBle_BufferLoop,(uint8_t*)p_adv_report,sizeof(ble_gap_evt_adv_report_t));
#endif

#if 1
            /*
            if (is_uuid_present(&m_nus_uuid, p_adv_report))
            {

                err_code = sd_ble_gap_connect(&p_adv_report->peer_addr,
                                              &m_scan_params,
                                              &m_connection_param);

                if (err_code == NRF_SUCCESS)
                {
                    // scan is automatically stopped by the connect
                    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
                    APP_ERROR_CHECK(err_code);
                    printf("Connecting to target %02x%02x%02x%02x%02x%02x\r\n",
                           p_adv_report->peer_addr.addr[0],
                           p_adv_report->peer_addr.addr[1],
                           p_adv_report->peer_addr.addr[2],
                           p_adv_report->peer_addr.addr[3],
                           p_adv_report->peer_addr.addr[4],
                           p_adv_report->peer_addr.addr[5]
                          );
                }
            }
            */
        }
#endif
        break;
        // 安全请求
        case BLE_GAP_EVT_SEC_REQUEST:
            break;
        // 连接参数更新请求
        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
#if 0
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
#endif
            break;
        // 扫描请求
        case BLE_GAP_EVT_SCAN_REQ_REPORT:
            break;
        // ----------------------------
        // 发现响应事件 ---   主服务
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            break;
        // 发现响应事件 ---   关联
        case BLE_GATTC_EVT_REL_DISC_RSP:
            break;
        // 发现响应事件 ---   特征
        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            break;
        // 发现响应事件 ---   描述符
        case BLE_GATTC_EVT_DESC_DISC_RSP:
            break;
        // 响应事件     ---   属性信息
        case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
            break;
        // 读取响应事件 ---   UUID
        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
            break;
        // 读取响应事件 ---   读取
        case BLE_GATTC_EVT_READ_RSP:
            break;
        // 读取响应事件 ---   多个
        case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
            break;
        // 编写响应事件
        case BLE_GATTC_EVT_WRITE_RSP:
            break;
        // 通知与指示 handle 值
        case BLE_GATTC_EVT_HVX:
            break;
        // 超时事件
        case BLE_GATTC_EVT_TIMEOUT:
#if 0
            // Disconnect on GATT Client timeout event.
            //NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
#endif
            break; // BLE_GATTC_EVT_TIMEOUT
        // -----------------------------
        // 执行写操作
        case BLE_GATTS_EVT_WRITE:
            break;
        // 读写授权请求
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            break;
        // 系统属性访问正在等待
        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            break;
        // 处理值确认
        case BLE_GATTS_EVT_HVC:
            break;
        // 服务改变确认,不应用其他事件结构
        case BLE_GATTS_EVT_SC_CONFIRM:
            break;
        // 超时
        case BLE_GATTS_EVT_TIMEOUT:
#if 0
            // Disconnect on GATT Server timeout event.
            //NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
#endif
            break;

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
#if 0
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                       NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
#endif
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif
        default:
            break;
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
}
/*
******************************************************************************
* 函数功能: 广播数据解析
******************************************************************************
*/
static void BspNrfBle_AdvDataParse(const ble_gap_evt_adv_report_t *ps)
{
    ps=ps;
#if   (defined(XKAP_ICARE_B_M))
    static uint8_t adv_data_bak_buf[7]= {0};
    // 验证地址
    // 验证长度
    if(ps->dlen!=(APP_ADV_INFO_LENGTH+7))
    {
        return;
    }
    // 验证特征值
    if((ps->data[0]!=0x02)\
       ||(ps->data[1]!=0x01)\
       ||(!(ps->data[2]==0x04)||(ps->data[2]==0x06))\
       ||(ps->data[3]!=0x1a)\
       ||(ps->data[4]!=0xff)\
       ||(ps->data[5]!=0x59)\
       ||(ps->data[6]!=0x00)\
      )
    {
        return;
    }
    // 验证校验值
    // 去重复
    if((adv_data_bak_buf[0]==ps->peer_addr.addr[0])\
       &&(adv_data_bak_buf[1]==ps->peer_addr.addr[1])\
       &&(adv_data_bak_buf[2]==ps->peer_addr.addr[2])\
       &&(adv_data_bak_buf[3]==ps->peer_addr.addr[3])\
       &&(adv_data_bak_buf[4]==ps->peer_addr.addr[4])\
       &&(adv_data_bak_buf[5]==ps->peer_addr.addr[5])\
       &&(adv_data_bak_buf[6]==ps->data[7]))
    {
        return;
    }
    else
    {
        adv_data_bak_buf[0]   =  ps->peer_addr.addr[0];
        adv_data_bak_buf[1]   =  ps->peer_addr.addr[1];
        adv_data_bak_buf[2]   =  ps->peer_addr.addr[2];
        adv_data_bak_buf[3]   =  ps->peer_addr.addr[3];
        adv_data_bak_buf[4]   =  ps->peer_addr.addr[4];
        adv_data_bak_buf[5]   =  ps->peer_addr.addr[5];
        adv_data_bak_buf[6]   =  ps->data[7];
    }
    // 解析数据
    Hci_Rx_Parse((uint8_t *)&(ps->data[8]),NULL);
#endif
}
#endif
/*
******************************************************************************
* 函数功能: 广播数据更新
******************************************************************************
*/
static void BspNrfBle_AdvDataUpdata(void)
{
#if   (defined(BSP_NRFBLE_ENABLE_ADVERTISE))
    /*
    uint8_t i=0;
    int16_t i16,j16;
    uint32_t i32;
    // 序号
    m_advertise_info[i++]  =  BspNrfBle_TxSerialNum++;
    // 更新BMP180
    // ---Len
    m_advertise_info[i++]  =  7;
    // ---Type
    m_advertise_info[i++]  =  1;
    // ---Data
    BspBmp180_Read(&i16,&i32,&j16);
    m_advertise_info[i++]  =  i16>>8;
    m_advertise_info[i++]  =  i16;
    m_advertise_info[i++]  =  i32>>16;
    m_advertise_info[i++]  =  i32>>8;
    m_advertise_info[i++]  =  i32;
    m_advertise_info[i++]  =  j16;
    // 更新AP3216
    // ---Data
    BspAp3216c_Read((uint16_t*)&i16,(uint16_t*)&j16,NULL);
    m_advertise_info[i++]  =  i16>>8;
    m_advertise_info[i++]  =  i16;
    m_advertise_info[i++]  =  j16>>8;
    m_advertise_info[i++]  =  j16;
    // 更新MPU6050
    // ---Len
    m_advertise_info[i++]  =  2;
    // ---Type
    m_advertise_info[i++]  =  3;
    // ---Data
    m_advertise_info[i++]  =  BspMpu6050_TapCmt;
    // 执行更新
    advertising_init();
    */
#if (defined(XKAP_ICARE_B_C)||defined(XKAP_ICARE_B_M)||defined(PROJECT_NRF5X_BLE))
    uint8_t i=0;
    if(BspNrfBle_s_Tx.state==1)
    {
        BspNrfBle_s_Tx.state=0;
        // 写入串号
        BspNrfBle_s_Tx.SerialNum =  BspNrfBle_TxSerialNum++;
        // 计算CRC16
        BspNrfBle_s_Tx.crc16     =  Count_CRC16(BspNrfBle_s_Tx.buf,APP_ADV_INFO_LENGTH-3);
        // 数据转存
        m_advertise_info[i++]    =  BspNrfBle_s_Tx.SerialNum;
        memcpy(&m_advertise_info[i],BspNrfBle_s_Tx.buf,APP_ADV_INFO_LENGTH-3);
        i   += (APP_ADV_INFO_LENGTH-3);
        m_advertise_info[i++]    =  BspNrfBle_s_Tx.crc16;
        m_advertise_info[i++]    =  BspNrfBle_s_Tx.crc16>>8;
        // 执行更新
        advertising_init();
    }
#endif
#endif
}

/*
******************************************************************************
* 函数功能: 100ms周期处理
******************************************************************************
*/
void BspNrfBle_Timer(uint16_t ms)
{
    static uint8_t si=0;
    //
#if   (defined(XKAP_ICARE_B_D))
    uint8_t *pbuf;
    ble_gap_evt_adv_report_t *ps;
    uint16_t i16;
    uint8_t i;
    pbuf=MemManager_Get(E_MEM_MANAGER_TYPE_256B);
    ps  =  (ble_gap_evt_adv_report_t*)&pbuf[128];
    i=0;
    i = Count_BufferLoopPop(&BspNrfBle_BufferLoop,(uint8_t*)ps,&i16,COUNT_BUFFERLOOP_E_POPMODE_DELETE);
    if(i==OK)
    {
        i=0;
        // 地址01
        pbuf[i++]=0x01;
        // 功能码03
        pbuf[i++]=COUNT_MODBUS_FC_UPLOAD;
        // 数据长度XX
        pbuf[i++]=0x00;
        // 数据
        pbuf[i++]=ps->peer_addr.addr[0];
        pbuf[i++]=ps->peer_addr.addr[1];
        pbuf[i++]=ps->peer_addr.addr[2];
        pbuf[i++]=ps->peer_addr.addr[3];
        pbuf[i++]=ps->peer_addr.addr[4];
        pbuf[i++]=ps->peer_addr.addr[5];
        pbuf[i++]=ps->rssi;
        pbuf[i++]=ps->scan_rsp;
        pbuf[i++]=ps->type;
        pbuf[i++]=ps->dlen;
        memcpy(&pbuf[i],ps->data,31);
        i+=31;
        pbuf[2]=i-3;
        // CRC
        i16 = Count_CRC16(pbuf,i);
        pbuf[i++]=(uint8_t)(i16);
        pbuf[i++]=(uint8_t)(i16>>8);
        // 发送
        UART_DMA_Tx(BSP_NRFBLE_UART_X,pbuf,i);
    }
    else if(pBspNrfBle_UartTx!=NULL)
    {
        UART_DMA_Tx(BSP_NRFBLE_UART_X,&pBspNrfBle_UartTx[1],pBspNrfBle_UartTx[0]);
        MemManager_Free(E_MEM_MANAGER_TYPE_256B,pBspNrfBle_UartTx);
        pBspNrfBle_UartTx=NULL;
    }
    MemManager_Free(E_MEM_MANAGER_TYPE_256B,pbuf);
#endif
//
    if(ms==100)
    {
        si++;
        if(si<10)
            return;
        si=0;
    }
    else if(ms==1000)
    {
        si=0;
    }
//
    if(BspNrfBle_DebugTest_Enable==1)
    {
    }
    BspNrfBle_AdvDataUpdata();
//
}
void BspNrfBle_DebugTestOnOff(uint8_t OnOff)
{
    if(OnOff==ON)
    {
        BspNrfBle_DebugTest_Enable=1;
    }
    else
    {
        BspNrfBle_DebugTest_Enable=0;
    }
}
//
void BspNrfBle_ProtocolParse(uint8_t *pbuf,uint16_t len)
{
#if   (defined(XKAP_ICARE_B_D))
    uint8_t i;
    uint16_t i16,j16;
    ble_gap_addr_t *pble_addr;

    BSP_LED_LED1_TOGGLE;
    // 规则判定
    i = Count_Modbus_Check(pbuf,len,BSP_NRFBLE_UART_ADDR);
    if(i==ERR)
    {
        return;
    }
    // 解析功能码
    switch(pbuf[1])
    {
        case COUNT_MODBUS_FC_READ_HOLDING_REGISTERS:
            //获取寄存器
            i16 = Count_2ByteToWord(pbuf[3],pbuf[2]);
            //获取长度
            j16 = Count_2ByteToWord(pbuf[5],pbuf[4]);
            //
            if(pBspNrfBle_UartTx!=NULL)
            {
                break;
            }
            if(i16!=0x0000&&j16!=0x0005)
            {
                break;
            }
            // 获取缓存
            pBspNrfBle_UartTx =  MemManager_Get(E_MEM_MANAGER_TYPE_256B);
            pble_addr         =  (ble_gap_addr_t*)&pBspNrfBle_UartTx[128];
            sd_ble_gap_address_get(pble_addr);
            // 赋值数据
            pBspNrfBle_UartTx[100]  =  j16*2;
            pBspNrfBle_UartTx[101]  =  HARDWARE_SUB_VER;
            pBspNrfBle_UartTx[102]  =  HARDWARE_VER;
            pBspNrfBle_UartTx[103]  =  SOFTWARE_SUB_VER;
            pBspNrfBle_UartTx[104]  =  SOFTWARE_VER;
            pBspNrfBle_UartTx[105]  =  pble_addr->addr[0];
            pBspNrfBle_UartTx[106]  =  pble_addr->addr[1];
            pBspNrfBle_UartTx[107]  =  pble_addr->addr[2];
            pBspNrfBle_UartTx[108]  =  pble_addr->addr[3];
            pBspNrfBle_UartTx[109]  =  pble_addr->addr[4];
            pBspNrfBle_UartTx[110]  =  pble_addr->addr[5];
            Count_Modbus_Array(&pBspNrfBle_UartTx[1],\
                               &i16,\
                               BSP_NRFBLE_UART_ADDR,\
                               COUNT_MODBUS_FC_READ_HOLDING_REGISTERS,\
                               NULL,\
                               NULL,\
                               &pBspNrfBle_UartTx[100],\
                               &pBspNrfBle_UartTx[101],\
                               pBspNrfBle_UartTx[100]);
            pBspNrfBle_UartTx[0]=i16;
            break;
        case COUNT_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
            break;
        default:
            break;
    }
#endif
    pbuf  =  pbuf;
    len   =  len;
}
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

