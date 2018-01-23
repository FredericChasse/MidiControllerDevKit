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

#include "UsbInterface.h"



// Private definitions
//=============================================================================

typedef struct
{
  UsbInterface_t public;
  uint8_t oInitialized;
} _Usb_t;


// Private prototypes
//=============================================================================
  
void _Usb_Init (_Usb_t *self);
uint8_t _Usb_FsmStep (_Usb_t *self);
int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize);


// Private variables
//=============================================================================

_Usb_t _usb = 
{
  .public.Init = (void(*)(UsbInterface_t*)) &_Usb_Init
 ,.public.FsmStep = (uint8_t(*)(UsbInterface_t*)) &_Usb_FsmStep
 ,.public.NewMidiMsg = (int8_t(*)(UsbInterface_t*,uint8_t*,uint8_t)) &_Usb_NewMidiMsg
 ,.oInitialized = 0
};


// Private functions
//=============================================================================

void _Usb_Init (_Usb_t *self)
{
  if (self->oInitialized)
  {
    return;
  }
  
  self->oInitialized = 1;
}


uint8_t _Usb_FsmStep (_Usb_t *self)
{
  uint8_t ret = 0;
  
  
  return ret;
}


int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize)
{
  int8_t ret = 0;
  
  if (!USB_GetConfiguration())    // Device not configured
  {
    return -1;
  }
  
  if (msgSize != USB_3BYTE_COMMON)
  {
    return -1;
  }
  
  ret = USB_PutUsbMidiIn(USB_3BYTE_COMMON, msg, USB_MIDI_CABLE_00);
  
  if (ret == USB_FALSE)
  {
    ret = 0;
  }
  else
  {
    ret = -1;
  }
  
  return ret;
}


// Public functions
//=============================================================================

UsbInterface_t* UsbInterface_GetHandle (void)
{
  return &_usb.public;
}



/* [] END OF FILE */
