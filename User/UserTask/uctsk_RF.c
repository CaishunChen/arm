/*******************************************************************************
应用说明:
*串口与RF模块通过UART3串口通讯的程序任务
*如果更改UART端口,需要注意以下参数:
     USART3_DR_Base
		 RF_USART
		 RF_INIT()函数
*******************************************************************************/
//------------------------------- Includes -----------------------------------
#include <includes.h> 
#define USART3_DR_Base  0x40004804
#define  RF_USART				     USART3  
//--------------------------------GPRS
#define RF_SET_H        GPIOB->BSRR = GPIO_Pin_12
#define RF_SET_L        GPIOB->BRR  = GPIO_Pin_12
#define RF_RESET_H      GPIOB->BSRR = GPIO_Pin_13
#define RF_RESET_L      GPIOB->BRR  = GPIO_Pin_13
//------------------------------- 用户变量 --------------------------
static  OS_STK  App_TaskRFStk[APP_TASK_RF_STK_SIZE];
//-----环
#define RF_RBUF0_MAX				500 //(RF)接收缓存环儿容量 
uint8_t   RF_RBuf0[RF_RBUF0_MAX]; //(RF)接收缓存环儿
uint16_t  RFRBufNewAddr;				  //环儿存储新地址
uint8_t   RF_RBuf1[10];						//提取缓冲环
//-----数据提取缓存区
//-----RF信息变量
//------------------------------- 外部变量/函数声明 -----------
extern struct tm CurrentDate;
//------------------------------- 用户函数声明 ----------------
static void RF_INIT(void);
static void uctsk_RF (void);
//------------------------------------------------------------------------------任务创建---BEGIN
/*******************************************************************************
函数功能: RF任务创建 
*******************************************************************************/
void  App_RFTaskCreate (void)
{
  CPU_INT08U  os_err;
	os_err = os_err; 					
	os_err = OSTaskCreateExt(//函数体
	                         (void (*)(void *))uctsk_RF,                                 
                           //参数
													 (void *)0,                                
                           //堆栈
													 (OS_STK *)&App_TaskRFStk[APP_TASK_RF_STK_SIZE-1],
                           //优先级
													 APP_TASK_RF_PRIO,                            
                           APP_TASK_RF_PRIO,                             
                           (OS_STK *)&App_TaskRFStk[0],              
                           APP_TASK_RF_STK_SIZE,                              
                           (void *)0,                                  
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
													 );
	#if OS_TASK_NAME_EN > 0
    	OSTaskNameSet(APP_TASK_RF_PRIO, "Task RF", &os_err);
	#endif
}

//-----------------------------------------------------------------------------任务创建---END
//-----------------------------------------------------------------------------任务实体---BEGIN
/*******************************************************************************
函数功能: GSM分段任务实体 
说    明: 分段AT指令
*******************************************************************************/
uint32_t AD_value_mV;
//extern OS_EVENT *RtcMbox;
static void uctsk_RF (void) 
{           
	uint16_t  RFRBufOldAddr=0;				//环儿处理地址
	uint8_t  i=0,j,len=0;
	uint8_t	 step=0,sum=0;

	RF_INIT();
	for(;;)
		{
			RFRBufNewAddr = RF_RBUF0_MAX - DMA_GetCurrDataCounter(DMA1_Channel3);
			if(RFRBufNewAddr == RFRBufOldAddr)
				{
					OSTimeDlyHMSM(0, 0, 0, 200);
				}
			else
				{
					//解析-BEGIN
					//-----单个字节处理,所以不需要缓冲区
					while(RFRBufOldAddr != RFRBufNewAddr)
					  {
						  //提取数据,更新地址
							i = RF_RBuf0[RFRBufOldAddr];
							RFRBufOldAddr++;
							if(RFRBufOldAddr >= RF_RBUF0_MAX)
							  {
								  RFRBufOldAddr = 0;
								}
							//解析数据
							switch(step)
								{
								  case 0://帧头
									    if(i==0xA5)
											  {
												  step++;
													RF_RBuf1[0]=i;
												}
											else 
											  {
												  ;
												}
									    break;
									case 1://指令
									    switch(i)
											  {
												  case 0x01://放音乐指令
													    len = 1;
															step++;
															break;
													case 0x02://继电器开启
													    len = 3;
															step++;
															break;
													case 0x03://开关量读取
													    len = 1;
															step++;
													    break;
													case 0x04://RTC时间读取
													    len = 1;
															step++;
													    break;
													case 0x05://RTC时间设置
													    len = 6;
															step++;
															break;
													case 0x06://AD读取
													    len = 1;
															step++;
													    break;
													case 0xA5://数据为帧头
													    step = 1;
													    break;
													default:
													    step = 0;
															break;
												}
											RF_RBuf1[1]=i;
											j = 0;
									    break;
									case 2://数据
									    RF_RBuf1[j+2]=i;
											j++;
											if(j >= len)
											  {
												  step++;
												}
									    break;
									case 3://校验和(累加)
									    len += 2; 
									    for(j=sum=0;j<len;j++)
											  {
												  sum += RF_RBuf1[j];
												}
											if(sum == i)
											  {//校验成功,解析指令
												  switch(RF_RBuf1[1])
													  {
														  case 0x01://放音乐
																	RF_RBuf1[0] = 0x5A;
																	RF_RBuf1[1] = RF_RBuf1[1];
																	RF_RBuf1[2]	= 0x01;
																	RF_RBuf1[3]	= RF_RBuf1[0] + RF_RBuf1[1] + RF_RBuf1[2];
																	UART_DMA_Tx(3,RF_RBuf1,4);
																	break;
															case 0x02://开启继电器															  
																	UART_DMA_Tx(3,RF_RBuf1,4);
																	break;
															case 0x03://开关量读取 
																	UART_DMA_Tx(3,RF_RBuf1,4);
															    break;
															case 0x04://RTC时间读取
															    RF_RBuf1[0] = 0x5A;
																	RF_RBuf1[1] = RF_RBuf1[1];																	
																	RF_RBuf1[2] = CurrentDate.tm_year%100;
																	RF_RBuf1[3] = CurrentDate.tm_mon+1;
																	RF_RBuf1[4] = CurrentDate.tm_mday;
																	RF_RBuf1[5] = CurrentDate.tm_hour;
																	RF_RBuf1[6] = CurrentDate.tm_min;
																	RF_RBuf1[7] = CurrentDate.tm_sec;
																	RF_RBuf1[8]	= RF_RBuf1[0] + RF_RBuf1[1] + RF_RBuf1[2] + RF_RBuf1[3] +
																	              RF_RBuf1[4] + RF_RBuf1[5] + RF_RBuf1[6] + RF_RBuf1[7];
																	UART_DMA_Tx(3,RF_RBuf1,9);
															    break;
															case 0x05://RTC时间设置
															    CurrentDate.tm_sec   = RF_RBuf1[7];
  		                            CurrentDate.tm_min   = RF_RBuf1[6];
  		                            CurrentDate.tm_hour  = RF_RBuf1[5];
  		                            CurrentDate.tm_mday  = RF_RBuf1[4];
  		                            CurrentDate.tm_mon   = RF_RBuf1[3]-1;
  		                            CurrentDate.tm_year  = RF_RBuf1[2] + 2000;
                                  //重新配置RTC
                                  //OSMboxPost (RtcMbox,(void *)1);
																	//回应
															    RF_RBuf1[0] = 0x5A;
																	RF_RBuf1[1] = RF_RBuf1[1];
																	RF_RBuf1[2]	= 0x01;
																	RF_RBuf1[3]	= RF_RBuf1[0] + RF_RBuf1[1] + RF_RBuf1[2];
																	UART_DMA_Tx(3,RF_RBuf1,4);
																	break;
															case 0x06://AD读取
															    RF_RBuf1[0] = 0x5A;
																	RF_RBuf1[1] = RF_RBuf1[1];
																	j = RF_RBuf1[2];
																	if(j&0x01);
																	if(j&0x02);
																	if(j&0x04);
																	if(j&0x08)
																	  {//暂时程序,今后要改进,目前AD只支持第四路
																		  RF_RBuf1[2] = AD_value_mV>>8;
																			RF_RBuf1[3] = AD_value_mV;
																			RF_RBuf1[4] = RF_RBuf1[0] + RF_RBuf1[1] + RF_RBuf1[2] + RF_RBuf1[3];
																			UART_DMA_Tx(3,RF_RBuf1,5);
																		}
																	if(j&0x10);
																	if(j&0x20);
																	if(j&0x40);
																	if(j&0x80);
															    break;
															default:
															    break;
														}
													step = 0;
												}
											else
											  {
												  if(i == 0xA5)
													  {
														  step = 1;
														}
													else
													  {
														  step = 0;
														}
												}
									    break;
									default:break;
								}
						}
					//解析-END
					OSTimeDlyHMSM(0, 0, 0, 10);
				}
		}
}
//-----------------------------------------------------------------------------任务实体---END
/*******************************************************************************
							  GPIO初始化
*******************************************************************************/
static void RF_INIT(void)
  {															
    GPIO_InitTypeDef GPIO_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		USART_InitTypeDef USART_InitStructure;
    //时钟初始化
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1,ENABLE);
		//GPIO初始化
    //-----USART3-Rx-floating
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //-----USART3-Tx
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //-----XSL_RF_SET | XSL_RF_RST
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		//DMA初始化
		DMA_DeInit(DMA1_Channel3);  
  	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART3->DR);
  	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RF_RBuf0;
  	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  	DMA_InitStructure.DMA_BufferSize = RF_RBUF0_MAX;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	//循环模式存储
  	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
		//UART初始化
		USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);
		//使能DMA的Rx/Tx请求
		USART_DMACmd(USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
		//使能DMA通道
		DMA_Cmd(DMA1_Channel2, ENABLE);//UART3-Tx
		DMA_Cmd(DMA1_Channel3, ENABLE);//UART3-Rx
		//使能 USART
		USART_Cmd(USART3, ENABLE);
  }
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
