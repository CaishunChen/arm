/**
  ******************************************************************************
  * @file    Bsp_StepperMotor.c
  * @author  徐松亮 许红宁(5387603@qq.com)
  * @version V1.0.0
  * @date    2018/01/01
  ******************************************************************************
  * @attention
  *
  * GNU General Public License (GPL)
  *
  * <h2><center>&copy; COPYRIGHT 2017 XSLXHN</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "Bsp_StepperMotor.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
BSP_STEPPER_MOTOR_S BspStepperMotor_s[BSP_STEPPER_MOTOR_NUM];
static uint8_t BspStepperMotor_DebugTest_Enable=0;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   初始化函数
 * @note    初始化GPIO
 * @param   None
 * @return  None
 */
void BspStepperMotor_Init(void)
{
    uint8_t i;
#if   (defined(STM32F1)||defined(STM32F4))
    GPIO_InitTypeDef GPIO_InitStructure;
    //时钟初始化
    BSP_STEPPER_MOTOR_RCC_ENABLE;
#if   (defined(STM32F1))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
#elif (defined(STM32F4))
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
#endif
#ifdef BSP_STEPPER_MOTOR_1_A_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_1_A_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_1_A_PORT, &GPIO_InitStructure);
    //BSP_STEPPER_MOTOR_1_A_SET;
    BSP_STEPPER_MOTOR_1_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_B_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_1_B_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_1_B_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_1_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_C_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_1_C_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_1_C_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_1_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_D_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_1_D_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_1_D_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_1_D_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_A_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_2_A_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_2_A_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_2_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_B_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_2_B_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_2_B_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_2_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_C_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_2_C_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_2_C_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_2_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_D_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_2_D_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_2_D_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_2_D_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_A_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_3_A_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_3_A_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_3_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_B_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_3_B_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_3_B_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_3_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_C_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_3_C_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_3_C_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_3_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_D_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_3_D_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_3_D_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_3_D_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_A_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_4_A_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_4_A_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_4_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_B_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_4_B_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_4_B_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_4_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_C_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_4_C_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_4_C_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_4_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_D_PIN
    GPIO_InitStructure.GPIO_Pin     =  BSP_STEPPER_MOTOR_4_D_PIN;
    GPIO_Init(BSP_STEPPER_MOTOR_4_D_PORT, &GPIO_InitStructure);
    BSP_STEPPER_MOTOR_4_D_SET;
#endif
#elif (defined(NRF52)||defined(NRF51))
#ifdef BSP_STEPPER_MOTOR_1_A_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_1_A_PIN);
    BSP_STEPPER_MOTOR_1_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_B_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_1_B_PIN);
    BSP_STEPPER_MOTOR_1_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_C_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_1_C_PIN);
    BSP_STEPPER_MOTOR_1_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_1_D_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_1_D_PIN);
    BSP_STEPPER_MOTOR_1_D_SET;
#endif

#ifdef BSP_STEPPER_MOTOR_2_A_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_2_A_PIN);
    BSP_STEPPER_MOTOR_2_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_B_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_2_B_PIN);
    BSP_STEPPER_MOTOR_2_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_C_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_2_C_PIN);
    BSP_STEPPER_MOTOR_2_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_2_D_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_2_D_PIN);
    BSP_STEPPER_MOTOR_2_D_SET;
#endif

#ifdef BSP_STEPPER_MOTOR_3_A_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_3_A_PIN);
    BSP_STEPPER_MOTOR_3_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_B_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_3_B_PIN);
    BSP_STEPPER_MOTOR_3_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_C_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_3_C_PIN);
    BSP_STEPPER_MOTOR_3_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_3_D_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_3_D_PIN);
    BSP_STEPPER_MOTOR_3_D_SET;
#endif

#ifdef BSP_STEPPER_MOTOR_4_A_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_4_A_PIN);
    BSP_STEPPER_MOTOR_4_A_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_B_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_4_B_PIN);
    BSP_STEPPER_MOTOR_4_B_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_C_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_4_C_PIN);
    BSP_STEPPER_MOTOR_4_C_SET;
#endif
#ifdef BSP_STEPPER_MOTOR_4_D_PIN
    nrf_gpio_cfg_output(BSP_STEPPER_MOTOR_4_D_PIN);
    BSP_STEPPER_MOTOR_4_D_SET;
#endif

#endif
    //初始化
    for(i=0; i<BSP_STEPPER_MOTOR_NUM; i++)
    {
        BspStepperMotor_s[i].Speed    =  0xFFFF;  // 不转
        BspStepperMotor_s[i].StepNum  =  0;       // 不转
        BspStepperMotor_s[i].Flag_PN  =  0;       // 正转
        switch(i)
        {
            case 0:
#ifdef   BSP_STEPPER_MOTOR_1_PHASE_NUM
                BspStepperMotor_s[i].PhaseNumber      =  BSP_STEPPER_MOTOR_1_PHASE_NUM;
                BspStepperMotor_s[i].ReductionRatio   =  BSP_STEPPER_MOTOR_1_REDUCTION_RATIO;
#endif
                break;
            case 1:
#ifdef   BSP_STEPPER_MOTOR_2_PHASE_NUM
                BspStepperMotor_s[i].PhaseNumber      =  BSP_STEPPER_MOTOR_2_PHASE_NUM;
                BspStepperMotor_s[i].ReductionRatio   =  BSP_STEPPER_MOTOR_2_REDUCTION_RATIO;
#endif
                break;
            case 2:
#ifdef   BSP_STEPPER_MOTOR_3_PHASE_NUM
                BspStepperMotor_s[i].PhaseNumber      =  BSP_STEPPER_MOTOR_3_PHASE_NUM;
                BspStepperMotor_s[i].ReductionRatio   =  BSP_STEPPER_MOTOR_3_REDUCTION_RATIO;
#endif
                break;
            case 3:
#ifdef   BSP_STEPPER_MOTOR_4_PHASE_NUM
                BspStepperMotor_s[i].PhaseNumber      =  BSP_STEPPER_MOTOR_4_PHASE_NUM;
                BspStepperMotor_s[i].ReductionRatio   =  BSP_STEPPER_MOTOR_4_REDUCTION_RATIO;
#endif
                break;
            default:
                break;
        }
    }
}
/**
 * @brief   周期处理函数
 * @note    放在1ms循环中执行
 * @param   None
 * @return  None
 */
void BspStepperMotor_1ms(void)
{
    static uint8_t step_buf[BSP_STEPPER_MOTOR_NUM]= {0};
    static uint16_t scmt_buf[BSP_STEPPER_MOTOR_NUM]= {0};
    uint8_t i;
    for(i=0; i<BSP_STEPPER_MOTOR_NUM; i++)
    {
        if(BspStepperMotor_s[i].Speed==0xFFFF || BspStepperMotor_s[i].StepNum==0)
        {
            continue;
        }
        else if(scmt_buf[i]<BspStepperMotor_s[i].Speed)
        {
            scmt_buf[i]++;
            continue;
        }
        scmt_buf[i]=0;
        BspStepperMotor_s[i].StepNum--;
        switch(i)
        {
#ifdef   BSP_STEPPER_MOTOR_1_A_PIN
            case 0:
                switch(step_buf[i])
                {
                    case  0:
                        BSP_STEPPER_MOTOR_1_D_SET;
                        BSP_STEPPER_MOTOR_1_B_SET;
                        BSP_STEPPER_MOTOR_1_A_CLR;
                        break;
                    case  1:
                        BSP_STEPPER_MOTOR_1_A_CLR;
                        BSP_STEPPER_MOTOR_1_B_CLR;
                        break;
                    case  2:
                        BSP_STEPPER_MOTOR_1_A_SET;
                        BSP_STEPPER_MOTOR_1_C_SET;
                        BSP_STEPPER_MOTOR_1_B_CLR;
                        break;
                    case  3:
                        BSP_STEPPER_MOTOR_1_B_CLR;
                        BSP_STEPPER_MOTOR_1_C_CLR;
                        break;
                    case  4:
                        BSP_STEPPER_MOTOR_1_B_SET;
                        BSP_STEPPER_MOTOR_1_D_SET;
                        BSP_STEPPER_MOTOR_1_C_CLR;
                        break;
                    case  5:
                        BSP_STEPPER_MOTOR_1_C_CLR;
                        BSP_STEPPER_MOTOR_1_D_CLR;
                        break;
                    case  6:
                        BSP_STEPPER_MOTOR_1_A_SET;
                        BSP_STEPPER_MOTOR_1_C_SET;
                        BSP_STEPPER_MOTOR_1_D_CLR;
                        break;
                    case  7:
                        BSP_STEPPER_MOTOR_1_D_CLR;
                        BSP_STEPPER_MOTOR_1_A_CLR;
                        break;
                    default:
                        break;
                }
                break;
#endif
#ifdef   BSP_STEPPER_MOTOR_2_A_PIN
            case 1:
                switch(step_buf[i])
                {
                    case  0:
                        BSP_STEPPER_MOTOR_2_D_SET;
                        BSP_STEPPER_MOTOR_2_B_SET;
                        BSP_STEPPER_MOTOR_2_A_CLR;
                        break;
                    case  1:
                        BSP_STEPPER_MOTOR_2_A_CLR;
                        BSP_STEPPER_MOTOR_2_B_CLR;
                        break;
                    case  2:
                        BSP_STEPPER_MOTOR_2_A_SET;
                        BSP_STEPPER_MOTOR_2_C_SET;
                        BSP_STEPPER_MOTOR_2_B_CLR;
                        break;
                    case  3:
                        BSP_STEPPER_MOTOR_2_B_CLR;
                        BSP_STEPPER_MOTOR_2_C_CLR;
                        break;
                    case  4:
                        BSP_STEPPER_MOTOR_2_B_SET;
                        BSP_STEPPER_MOTOR_2_D_SET;
                        BSP_STEPPER_MOTOR_2_C_CLR;
                        break;
                    case  5:
                        BSP_STEPPER_MOTOR_2_C_CLR;
                        BSP_STEPPER_MOTOR_2_D_CLR;
                        break;
                    case  6:
                        BSP_STEPPER_MOTOR_2_A_SET;
                        BSP_STEPPER_MOTOR_2_C_SET;
                        BSP_STEPPER_MOTOR_2_D_CLR;
                        break;
                    case  7:
                        BSP_STEPPER_MOTOR_2_D_CLR;
                        BSP_STEPPER_MOTOR_2_A_CLR;
                        break;
                    default:
                        break;
                }
                break;
#endif
#ifdef   BSP_STEPPER_MOTOR_3_A_PIN
            case 2:
                switch(step_buf[i])
                {
                    case  0:
                        BSP_STEPPER_MOTOR_3_B_SET;
                        BSP_STEPPER_MOTOR_3_D_SET;
                        BSP_STEPPER_MOTOR_3_A_CLR;
                        break;
                    case  1:
                        BSP_STEPPER_MOTOR_3_A_CLR;
                        BSP_STEPPER_MOTOR_3_B_CLR;
                        break;
                    case  2:
                        BSP_STEPPER_MOTOR_3_A_SET;
                        BSP_STEPPER_MOTOR_3_C_SET;
                        BSP_STEPPER_MOTOR_3_B_CLR;
                        break;
                    case  3:
                        BSP_STEPPER_MOTOR_3_B_CLR;
                        BSP_STEPPER_MOTOR_3_C_CLR;
                        break;
                    case  4:
                        BSP_STEPPER_MOTOR_3_B_SET;
                        BSP_STEPPER_MOTOR_3_D_SET;
                        BSP_STEPPER_MOTOR_3_C_CLR;
                        break;
                    case  5:
                        BSP_STEPPER_MOTOR_3_C_CLR;
                        BSP_STEPPER_MOTOR_3_D_CLR;
                        break;
                    case  6:
                        BSP_STEPPER_MOTOR_3_A_SET;
                        BSP_STEPPER_MOTOR_3_C_SET;
                        BSP_STEPPER_MOTOR_3_D_CLR;
                        break;
                    case  7:
                        BSP_STEPPER_MOTOR_3_D_CLR;
                        BSP_STEPPER_MOTOR_3_A_CLR;
                        break;
                    default:
                        break;
                }
                break;
#endif
#ifdef   BSP_STEPPER_MOTOR_4_A_PIN
            case 3:
                switch(step_buf[i])
                {
                    case  0:
                        BSP_STEPPER_MOTOR_4_B_SET;
                        BSP_STEPPER_MOTOR_4_D_SET;
                        BSP_STEPPER_MOTOR_4_A_CLR;
                        break;
                    case  1:
                        BSP_STEPPER_MOTOR_4_A_CLR;
                        BSP_STEPPER_MOTOR_4_B_CLR;
                        break;
                    case  2:
                        BSP_STEPPER_MOTOR_4_A_SET;
                        BSP_STEPPER_MOTOR_4_C_SET;
                        BSP_STEPPER_MOTOR_4_B_CLR;
                        break;
                    case  3:
                        BSP_STEPPER_MOTOR_4_B_CLR;
                        BSP_STEPPER_MOTOR_4_C_CLR;
                        break;
                    case  4:
                        BSP_STEPPER_MOTOR_4_B_SET;
                        BSP_STEPPER_MOTOR_4_D_SET;
                        BSP_STEPPER_MOTOR_4_C_CLR;
                        break;
                    case  5:
                        BSP_STEPPER_MOTOR_4_C_CLR;
                        BSP_STEPPER_MOTOR_4_D_CLR;
                        break;
                    case  6:
                        BSP_STEPPER_MOTOR_4_A_SET;
                        BSP_STEPPER_MOTOR_4_C_SET;
                        BSP_STEPPER_MOTOR_4_D_CLR;
                        break;
                    case  7:
                        BSP_STEPPER_MOTOR_4_D_CLR;
                        BSP_STEPPER_MOTOR_4_A_CLR;
                        break;
                    default:
                        break;
                }
                break;
#endif
            default:
                break;
        }

        //正转
        if(BspStepperMotor_s[i].Flag_PN==0)
        {
            if(step_buf[i]>=(BspStepperMotor_s[i].PhaseNumber*2-1))
            {
                step_buf[i]=0;
            }
            else
            {
                step_buf[i]++;
            }
        }
        //反转
        else
        {
            if(step_buf[i]==0)
            {
                step_buf[i]=(BspStepperMotor_s[i].PhaseNumber*2-1);
            }
            else
            {
                step_buf[i]--;
            }
        }
    }
    //测试
    if(BspStepperMotor_DebugTest_Enable==1)
    {
        for(i=0; i<BSP_STEPPER_MOTOR_NUM; i++)
        {
            if(BspStepperMotor_s[i].StepNum!=0)
            {
                continue;
            }
            if(BspStepperMotor_s[i].Flag_PN==0)
            {
                BspStepperMotor_s[i].Flag_PN=1;
            }
            else
            {
                BspStepperMotor_s[i].Flag_PN=0;
            }
            switch(i)
            {
                case 0:
                    BspStepperMotor_s[i].Speed=10;
                    BspStepperMotor_s[i].StepNum=240*BspStepperMotor_s[i].PhaseNumber;
                    break;
                case 1:
                    BspStepperMotor_s[i].Speed=10;
                    BspStepperMotor_s[i].StepNum=120*BspStepperMotor_s[i].PhaseNumber;
                    break;
                case 2:
                    BspStepperMotor_s[i].Speed=2;
                    BspStepperMotor_s[i].StepNum=1000*BspStepperMotor_s[i].PhaseNumber;
                    break;
                case 3:
                    BspStepperMotor_s[i].Speed=2;
                    BspStepperMotor_s[i].StepNum=1000*BspStepperMotor_s[i].PhaseNumber;
                    break;
                default:
                    break;
            }
        }
    }
}
/**
 * @brief   Debug接口函数
 * @note    开启Debug相关指令后会循环点亮，关闭Debug后所有LED全部关闭
 * @param   OnOff-判断指令进入与退出
 * @return  None
 */
void BspStepperMotor_DebugTestOnOff(uint8_t OnOff)
{
    uint8_t i;
    if(OnOff==ON)
    {
        BspStepperMotor_DebugTest_Enable=1;
    }
    else
    {
        BspStepperMotor_DebugTest_Enable=0;
        for(i=0; i<BSP_STEPPER_MOTOR_NUM; i++)
        {
        	BspStepperMotor_s[i].Speed	=	0xFFFF;
			BspStepperMotor_s[i].StepNum=	0;
        }
    }
}

//------------------------------------------------------------

