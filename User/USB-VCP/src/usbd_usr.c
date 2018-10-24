/* Includes ------------------------------------------------------------------*/
#include "usbd_usr.h"
#include "usbd_ioreq.h"


/** @defgroup USBD_USR_Private_Variables
* @{
*/ 

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};


/** @defgroup USBD_USR_Private_Functions
* @{
*/ 

/**
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{  
  /* Initialize LEDs */
  //STM_EVAL_LEDInit(LED1);
  /* Initialize the LCD */
}

/**
* @brief  USBD_USR_DeviceReset 
*         Displays the message on LCD on device Reset Event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed )
{
 switch (speed)
 {
   case USB_OTG_SPEED_HIGH: 
     //LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [HS]" );
     break;
  case USB_OTG_SPEED_FULL: 
     //LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [FS]" );
     break;
 default:
     //LCD_LOG_SetFooter ("     USB Device Library v1.1.0 [??]" );
     break;
 }
}


/**
* @brief  USBD_USR_DeviceConfigured
*         Displays the message on LCD on device configuration Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConfigured (void)
{
  //LCD_UsrLog("> VCP Interface configured.\n");
}

/**
* @brief  USBD_USR_DeviceSuspended 
*         Displays the message on LCD on device suspend Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  //LCD_UsrLog("> USB Device in Suspend Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceResumed 
*         Displays the message on LCD on device resume Event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
    //LCD_UsrLog("> USB Device in Idle Mode.\n");
  /* Users can do their application actions here for the USB-Reset */
}


/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceConnected (void)
{
  //LCD_UsrLog("> USB Device Connected.\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Staus
*/
void USBD_USR_DeviceDisconnected (void)
{
  //LCD_UsrLog("> USB Device Disconnected.\n");
}
/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
