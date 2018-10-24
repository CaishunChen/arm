#ifndef __BSP_BTCOM_H
#define __BSP_BTCOM_H
//
#include "uctsk_BluetoothDTU.h"
//
#define BSP_BTCOM_FULL_MODE            1 
#define BSP_BTCOM_ONCE_MODE            0
//
extern uint8_t BTCom_DataTransmissionState;
extern uint8_t BSP_BTCOM_tempbuf[];
//
uint8_t Bsp_BTCOM_Upload(uint8_t mode);

#endif
