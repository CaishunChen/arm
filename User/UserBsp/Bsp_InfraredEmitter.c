/*
***********************************************************************************
实现红外发送使用两个定时器，一个用来发送38K载波信号，PPI实现，不需要CPU干涉
另一个使用10us定时器发送调制波信号
***********************************************************************************
*/
#include "Bsp_InfraredEmitter.h"
#include "string.h"

#if defined RTOS
#include "Module_OS.h"
#endif

static uint8_t	Infrared_Emitter_Data[4] = {0};
static bool			Infrared_Emitter_busy = 0;

static void Bsp_NEC_Coding_Modulate_InTimer10us(void);	//10us执行函数

static	void timer_dummy_handler(nrf_timer_event_t event_type, void * p_context){}	//38K定时器中断函数
static void Timer2_EventHandler(nrf_timer_event_t event_type, void* p_context)	//10us定时器函数
{
	Bsp_NEC_Coding_Modulate_InTimer10us();	
}

const	nrf_drv_timer_t Timer_38k = NRF_DRV_TIMER_INSTANCE(BSP_INFRARED_38K_TIMER);
const	nrf_drv_timer_t Timer_10us = NRF_DRV_TIMER_INSTANCE(BSP_10US_TIMER);
	
#define	BSP_GPIO_INFRARED_MODULATE_OFF nrf_drv_timer_disable(&Timer_10us)
#define	BSP_GPIO_INFRARED_MODULATE_ON  nrf_drv_timer_enable(&Timer_10us)

#define	BSP_GPIO_INFRARED_EMITTER_LOW		nrf_drv_gpiote_out_task_enable(BSP_INFRARED_PIN);
#define	BSP_GPIO_INFRARED_EMITTER_HIGH	nrf_drv_gpiote_out_task_disable(BSP_INFRARED_PIN);\
																				nrf_drv_gpiote_out_clear(BSP_INFRARED_PIN)
	
nrf_drv_gpiote_out_config_t config = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
	
/*设置PPI通道*/
void Bsp_Gpiote_Timer_Task(void)
{
	uint32_t compare_evt_addr;
	uint32_t gpiote_task_addr;
	nrf_ppi_channel_t ppi_channel = BSP_38K_PPI_CHANNEL;
						 
	nrf_drv_gpiote_out_init(BSP_INFRARED_PIN, &config);																		//初始化GPIO，生成task
	
	nrf_drv_timer_extended_compare(&Timer_38k, NRF_TIMER_CC_CHANNEL0, 212, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);	//设置比较模式
	nrf_drv_ppi_channel_alloc(&ppi_channel);
	
	compare_evt_addr = nrf_drv_timer_event_address_get(&Timer_38k, NRF_TIMER_EVENT_COMPARE0);						//得到任务地址
	gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(BSP_INFRARED_PIN);														//得到事件地址
	
	nrf_drv_ppi_channel_assign(ppi_channel, compare_evt_addr, gpiote_task_addr);			//使用ppi将task和event联系起来
	
	nrf_drv_ppi_channel_enable(ppi_channel);																					//使能ppi	
}

/*初始化38k定时器，及ppi*/
void Bsp_Timer_38k_Init(void)
{
	  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;	//初始化定时器
		timer_cfg.frequency = NRF_TIMER_FREQ_16MHz;
	
    nrf_drv_ppi_init();		//初始化ppi

    nrf_drv_gpiote_init();	//初始化gpioe
		
    nrf_drv_timer_init(&Timer_38k, &timer_cfg, timer_dummy_handler);
		Bsp_Gpiote_Timer_Task();		//设置task和event
	
		nrf_drv_timer_enable(&Timer_38k);	//使能定时器
}

void Bsp_Timer_10us_Init(void)
{
	   uint32_t time_ticks;
    const uint32_t time_us = 10;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	
		nrf_drv_timer_init(&Timer_10us, &timer_cfg, Timer2_EventHandler);
	
    // 设定时间
    time_ticks = nrf_drv_timer_us_to_ticks(&Timer_10us, time_us);
    // 自动清空
    nrf_drv_timer_extended_compare(&Timer_10us, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
}		




void	Bsp_Infrared_Emitter_Init(void)
{	
	Bsp_Timer_38k_Init();	
	Bsp_Timer_10us_Init();
	BSP_GPIO_INFRARED_MODULATE_OFF;
	BSP_GPIO_INFRARED_EMITTER_HIGH;

}


bool Bsp_NEC_Coding_Modulate_4Byte(uint8_t *data)
{
	static uint8_t cnt_10us ,state ,bit_cnt = 0;
	static uint8_t bit;
	switch(state)
	{
		case 0:
		{
			if(bit_cnt >= 32)	//发送完32位
			{
				state = 1;
			}
			else
			{
				if(bit_cnt%8)
					bit = ((data[bit_cnt/8] >> (bit_cnt%8)) & 0x01);		
				else
					bit = (data[bit_cnt/8] & 0x01);
				bit_cnt++;
				state = 2;
			}
			break;
		}
		
		case 1:					//发送第33位
		{
			if(bit_cnt == 33)
			{
				bit = 0;
				state = 0;
				cnt_10us = 0;
				bit_cnt = 0;
				return true;
			}
			else
			{
				bit = 0;		
				bit_cnt++;
				state = 2;
			}
			break;
		}
		
		case 2: //发低电平
		{
		    BSP_GPIO_INFRARED_EMITTER_LOW;
		    cnt_10us++;
		    state = 3;
		}
		
		case 3: //等待560us
		{
			if(cnt_10us == 56)
			{
				cnt_10us = 0;
				state = 4;				
			}
			else
			{
				cnt_10us++;
			}
			break;
		}
		
		case 4: //发高电平
		{
		    BSP_GPIO_INFRARED_EMITTER_HIGH;
		    cnt_10us++;
		    state = 5;
		}
		
		case 5:
		{
			if(bit)
			{
				if(cnt_10us == 167)	//预留10us给case1
				{
					cnt_10us = 0;
					state = 0;
				}
				else
				{
					cnt_10us++;
				}
			}
			else
			{
				if(cnt_10us == 55)	//预留10us给case1
				{
					cnt_10us = 0;
					state = 0;
				}		
				else
				{
					cnt_10us++;
				}				
			}
			break;
		}
		
		default :
			break;
	}

	return false;
}

/*
*********************************************************************************************************
*	函 数 名: Bsp_NEC_Coding_Modulate_InTimer10us
*	功能说明: 在10us定时器中执行的程序，使用timer计时，降低CPU占用率
*	形    参: 无
*	返 回 值: 无
*	注		意：每帧数据由68个高低电平组成
**********************************************************************************************************/



void Bsp_NEC_Coding_Modulate_InTimer10us(void)
{
	static uint16_t cnt_10us = 0;
	static uint8_t state = 0;
	switch(state)
	{
		
		case 0:	//发送同步码低电平
		{
			BSP_GPIO_INFRARED_EMITTER_LOW;
			state = 1;
			break;
		}
		case 1:								
		{			
			if(cnt_10us == 900)	//9ms低电平
			{
				BSP_GPIO_INFRARED_EMITTER_HIGH;
				state = 2;
				break;
			}
		}
		case 2:
		{
			if(cnt_10us == (450+900))	//4.5ms高电平
			{
				state = 3;
			}
			break;
		}
		
		case 3:								//发送数据
		{
			if(Bsp_NEC_Coding_Modulate_4Byte(Infrared_Emitter_Data)) state = 4;
			break;
		}
		
		case 4:
		{
			BSP_GPIO_INFRARED_EMITTER_HIGH;
			if(cnt_10us == 11000)
			{
				cnt_10us = 0;
				state = 0;
				BSP_GPIO_INFRARED_EMITTER_HIGH;
				Infrared_Emitter_busy = false;
				BSP_GPIO_INFRARED_MODULATE_OFF;		//关闭定时器
			}
			break;
		}
	}
	cnt_10us += 1;
}

/*
*********************************************************************************************************
*	函 数 名: Bsp_NEC_Coding_Modulate_1Frame
*	功能说明: 按照NEC编码格式，发送一帧数据
*	形    参: addr1，addr2，control1，control2
*	返 回 值: 无
*	注		意：addr2和control2为校准使用，但不同厂商的校准协议不同，一般是发送原码的反码校准,另外，每帧占用的时间为110ms，后面补齐
*********************************************************************************************************
*/
bool Bsp_NEC_Coding_Modulate_1Frame(uint8_t * data)
{
	if(Infrared_Emitter_busy == true)
	{
		return false;
	}
	else
	{
		Infrared_Emitter_busy = true;
//		BSP_INFRARED_MODULATE_TIMER_HANDLER = Bsp_NEC_Coding_Modulate_InTimer10us;	//函数指针赋值
		memcpy(Infrared_Emitter_Data,data,sizeof(Infrared_Emitter_Data));
		BSP_GPIO_INFRARED_MODULATE_ON;	//打开10us定时器
		
		return true;
	}
}
