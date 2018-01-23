/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef __USB_INTERFACE_H__
#define __USB_INTERFACE_H__
  
#include <project.h>
  
typedef struct UsbInterface_t UsbInterface_t;
struct UsbInterface_t
{
  void (*Init) (UsbInterface_t *self);
  uint8_t (*FsmStep) (UsbInterface_t *self);
  int8_t (*NewMidiMsg) (UsbInterface_t *self, uint8_t *msg, uint8_t msgSize);
};


UsbInterface_t* UsbInterface_GetHandle(void);
  
  
#endif // __USB_INTERFACE_H__

/* [] END OF FILE */
