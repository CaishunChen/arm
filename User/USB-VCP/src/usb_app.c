/* Includes ------------------------------------------------------------------*/ 
#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"

#include "uctsk_Debug.h"
#if   (defined(PROJECT_TCI_V30))
#elif (defined(PROJECT_XKAP_V3)||defined(XKAP_ICARE_B_D_M))
#endif


__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */
uint32_t USB_TxWrite(uint8_t *buffter, uint32_t writeLen);
extern uint16_t VCP_DataTx   (uint8_t* Buf, uint32_t Len);
void USB_Config(void)
{
  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
} 
//此函数不用再别处调用,有数据来回进入到此
uint32_t USB_RxRead(uint8_t *buffter, uint32_t buffterSize)
{
	Debug_InterruptRx(buffter,(uint16_t)buffterSize);
	return 0;
}
//发送函数
uint32_t USB_TxWrite(uint8_t *buffter, uint32_t writeLen)
{
	VCP_DataTx(buffter,writeLen);
	return 0;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
