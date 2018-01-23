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
#include "BipBuffer.h"
#include "byte_buffer.h"


// Private definitions
//=============================================================================


typedef struct _Usb_t _Usb_t;
struct _Usb_t
{
  UsbInterface_t public;
  void (*Process) (_Usb_t *self);
  uint8_t oInitialized;
  uint8_t oActive;
  uint8_t activityCounter;
  uint8_t maxActivityCount;
  byte_buffer_t *cmdSizeBuf;
  BipBuffer_t *cmdBuf;
};

#define USB_DEVICE            (0)
#define USB_SUSPEND_TIMEOUT   (2)

#define USB_MAX_CMDS          (128)
#define USB_CMD_SIZE_MAX      (3)


// Private prototypes
//=============================================================================
  
void _Usb_Init (_Usb_t *self);
uint8_t _Usb_FsmStep (_Usb_t *self);
int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize);
void _Usb_Sleep (_Usb_t *self);
void _Usb_WakeUp (_Usb_t *self);
CY_ISR_PROTO(_Usb_SleepIsr);
void _Usb_ProcessMessages (_Usb_t *self);


// Private variables
//=============================================================================

_Usb_t _usb = 
{
  .public.Init = (void(*)(UsbInterface_t*)) &_Usb_Init
 ,.public.FsmStep = (uint8_t(*)(UsbInterface_t*)) &_Usb_FsmStep
 ,.public.NewMidiMsg = (int8_t(*)(UsbInterface_t*,uint8_t*,uint8_t)) &_Usb_NewMidiMsg
 ,.public.Sleep = (void(*)(UsbInterface_t*)) &_Usb_Sleep
 ,.public.WakeUp = (void(*)(UsbInterface_t*)) &_Usb_WakeUp
 ,.Process = &_Usb_ProcessMessages
 ,.oInitialized = 0
};

byte_buffer_t _cmdSizeBuf;
uint8_t       _cmdBufArray[USB_MAX_CMDS * USB_CMD_SIZE_MAX];
BipBuffer_t   _cmdBuf;
uint8_t       _cmdSizeBufArray[USB_MAX_CMDS];

/* Identity Reply message */
const uint8 CYCODE MIDI_IDENTITY_REPLY[] = {
    0xF0u,      /* SysEx */
    0x7Eu,      /* Non-real time */
    0x7Fu,      /* ID of target device (7F - "All Call") */
    0x06u,      /* Sub-ID#1 - General Information */
    0x02u,      /* Sub-ID#2 - Identity Reply */
    0x7Du,      /* Manufacturer's ID: 7D - Educational Use */
    0xB4u, 0x04u,               /* Family code */
    0x32u, 0xD2u,               /* Model number */
    0x01u, 0x00u, 0x00u, 0x00u, /* Version number */
    /*0xF7         End of SysEx automatically appended */
};

/* Need for Identity Reply message */
volatile uint8_t USB_MIDI1_InqFlags;
volatile uint8_t USB_MIDI2_InqFlags;
volatile uint8_t inqFlagsOld = 0;


// Private functions
//=============================================================================

void _Usb_Init (_Usb_t *self)
{
  if (self->oInitialized)
  {
    return;
  }
  
  self->cmdSizeBuf = &_cmdSizeBuf;
  byte_buffer_init(self->cmdSizeBuf, _cmdSizeBufArray, USB_MAX_CMDS);
  byte_buffer_reset(self->cmdSizeBuf);
  
  self->cmdBuf = &_cmdBuf;
  bipbuf_init(self->cmdBuf, _cmdBufArray, USB_MAX_CMDS*USB_CMD_SIZE_MAX);
  bipbuf_reset(self->cmdBuf);
  
  /* Start USBFS device 0 with VDDD operation */
  USB_Start(USB_DEVICE, USB_DWR_VDDD_OPERATION); 
  
  self->oInitialized = 1;
}


uint8_t _Usb_FsmStep (_Usb_t *self)
{
  uint8_t ret = 0;
  
  /* Host can send double SET_INTERFACE request */
  if(0u != USB_IsConfigurationChanged())
  {
    /* Initialize IN endpoints when device configured */
    if(0u != USB_GetConfiguration())   
    {
      /* Start ISR to determine sleep condition */		
      UsbSleep_isr_StartEx(_Usb_SleepIsr);

      /* Start SleepTimer's operation */
      UsbSleepTimer_Start();

      /* Enable output endpoint */
      USB_MIDI_Init();
    }
    else
    {
      UsbSleepTimer_Stop();
    }    
  }
  
 /* Service USB MIDI when device is configured */
  if(0u != USB_GetConfiguration())    
  {
    /* Call this API from UART RX ISR for Auto DMA mode */
    #if(!USB_EP_MANAGEMENT_DMA_AUTO) 
      USB_MIDI_IN_Service();
    #endif
    
    /* In Manual EP Memory Management mode OUT_EP_Service() 
    *  may have to be called from main foreground or from OUT EP ISR
    */
    #if(!USB_EP_MANAGEMENT_DMA_AUTO) 
      USB_MIDI_OUT_Service();
    #endif

    /* Sending Identity Reply Universal System Exclusive message 
    * back to computer */
    if(0u != (USB_MIDI1_InqFlags & USB_INQ_IDENTITY_REQ_FLAG))
    {
      USB_PutUsbMidiIn(sizeof(MIDI_IDENTITY_REPLY), (uint8 *) MIDI_IDENTITY_REPLY, USB_MIDI_CABLE_00);
      USB_MIDI1_InqFlags &= ~USB_INQ_IDENTITY_REQ_FLAG;
    }
    #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
      if(0u != (USB_MIDI2_InqFlags & USB_INQ_IDENTITY_REQ_FLAG))
      {
        USB_PutUsbMidiIn(sizeof(MIDI_IDENTITY_REPLY), \
          (uint8 *)MIDI_IDENTITY_REPLY, USB_MIDI_CABLE_01);
        USB_MIDI2_InqFlags &= ~USB_INQ_IDENTITY_REQ_FLAG;
      }
    #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */

    // This is where we service
    //----------------------------------------------------------------------------
    self->Process(self);
    //----------------------------------------------------------------------------
    
    #if(USB_EP_MANAGEMENT_DMA_AUTO) 
      #if (USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF)
        MIDI1_UART_DisableRxInt();
        #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
          MIDI2_UART_DisableRxInt();
        #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */
      #endif /* End USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF */            
      USB_MIDI_IN_Service();
      #if (USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF)
        MIDI1_UART_EnableRxInt();
        #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
          MIDI2_UART_EnableRxInt();
        #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */
      #endif /* End USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF */                
    #endif
    
  }
  else
  {
    byte_buffer_reset(self->cmdSizeBuf);
    bipbuf_reset(self->cmdBuf);
  }
  
  /* Check if host requested USB Suspend */
  if( self->activityCounter >= USB_SUSPEND_TIMEOUT ) 
  {
    self->activityCounter = 0;
    ret = 1;
  }
  else
  {
    ret = 0;
  }
  
  return ret;
}


int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize)
{
  if (msgSize != USB_3BYTE_COMMON)
  {
    return -1;
  }
  
  if (bipbuf_unused(self->cmdBuf) < msgSize)
  {
    return -2;
  }
  
  bipbuf_offer(self->cmdBuf, msg, msgSize);
  buffer_add_fast(self->cmdSizeBuf, msgSize);
  
  return 0;
}


void _Usb_ProcessMessages (_Usb_t *self)
{
  int8_t ret;
  uint8_t *msg;
  uint8_t msgSize;
  
  if (bipbuf_is_empty(self->cmdBuf))
  {
    return;
  }
  
  if (!USB_GetConfiguration())    // Device not configured
  {
    return;
  }
  
  uint16_t i;
  uint16_t nMsgs = self->cmdSizeBuf->count;
  for (i = 0; i < nMsgs; i++)
  {
    byte_buffer_get(self->cmdSizeBuf, &msgSize);
    msg = bipbuf_poll(self->cmdBuf, msgSize);
    do
    {
      ret = USB_PutUsbMidiIn(msgSize, msg, USB_MIDI_CABLE_00);
    } while (ret != USB_FALSE);
  }
}


void _Usb_Sleep (_Usb_t *self)
{
  /***************************************************************
  * Disable USBFS block and set DP Interrupt for wake-up 
  * from sleep mode. 
  ***************************************************************/
  USB_Suspend(); 
}


void _Usb_WakeUp (_Usb_t *self)
{

  /* Enable USBFS block after power-down mode */
  USB_Resume();

  /* Enable output endpoint */
  USB_MIDI_Init();
  
  self->activityCounter = 0;
  
}


/*******************************************************************************
* Function Name: SleepIsr
********************************************************************************
* Summary:
*  The sleep interrupt-service-routine used to determine a sleep condition.
*  The device goes into the Suspend state when there is a constant Idle 
*  state on its upstream-facing bus-lines for more than 3.0 ms. 
*  The device must be suspended drawing only suspend current from the 
*  bus after no more than 10 ms of the bus inactivity on all its ports.
*  This ISR is run each 4 ms, so after a second turn without the USB activity,  
*  the device should be suspended.
*
*******************************************************************************/
CY_ISR_PROTO(_Usb_SleepIsr)
{
  
    /* Check USB activity */
    if(0u != USB_CheckActivity()) 
    {
        _usb.activityCounter = 0u;
    } 
    else 
    {
        _usb.activityCounter++;
    }
    /* Clear Pending Interrupt */
    UsbSleepTimer_GetStatus();
}


/*******************************************************************************
* Function Name: USB_callbackLocalMidiEvent
********************************************************************************
* Summary: Local processing of the USB MIDI out-events.
*
*******************************************************************************/
void USB_callbackLocalMidiEvent(uint8 cable, uint8 *midiMsg) CYREENTRANT
{
  /* Support General System On/Off Message. */
  if((0u == (USB_MIDI1_InqFlags & USB_INQ_SYSEX_FLAG)) \
  && (0u != (inqFlagsOld & USB_INQ_SYSEX_FLAG)))
  {
    if(midiMsg[USB_EVENT_BYTE0] == USB_MIDI_SYSEX_GEN_MESSAGE)
    {
      if(midiMsg[USB_EVENT_BYTE1] == USB_MIDI_SYSEX_SYSTEM_ON)
      {
      // MIDI_PWR_Write(0u); /* Power ON */
      }
      else if(midiMsg[USB_EVENT_BYTE1] == USB_MIDI_SYSEX_SYSTEM_OFF)
      {
      // MIDI_PWR_Write(1u); /* Power OFF */
      }
    }
  }
  inqFlagsOld = USB_MIDI1_InqFlags;
  cable = cable;
}    


/*******************************************************************************
* Function Name: USB_MIDI1_ProcessUsbOut_EntryCallback
********************************************************************************
* Summary:  Turn the LED_OutA on at the beginning of the function
* USB_MIDI1_ProcessUsbOut() when data comes to be put in the UART1 out
* buffer.
* 
*******************************************************************************/
void USB_MIDI1_ProcessUsbOut_EntryCallback(void)
{
  LED_OutA_Write(1);
}


/*******************************************************************************
* Function Name: USB_MIDI1_ProcessUsbOut_ExitCallback
********************************************************************************
* Summary:  Turn the LED_OutA off at the end of the function  
* USB_MIDI1_ProcessUsbOut() when data is put in the UART1 out-buffer.
* 
*******************************************************************************/
void USB_MIDI1_ProcessUsbOut_ExitCallback(void)
{
  LED_OutA_Write(0);
}


/*******************************************************************************
* Function Name: USB_MIDI2_ProcessUsbOut_EntryCallback
********************************************************************************
* Summary:  Turn the LED_OutB on at the beginning of the function  
* USB_MIDI2_ProcessUsbOut() when data comes to be put in the UART2 out- 
* buffer  
* 
*******************************************************************************/
void USB_MIDI2_ProcessUsbOut_EntryCallback(void)
{
  LED_OutB_Write(1);
}


/*******************************************************************************
* Function Name: USB_MIDI2_ProcessUsbOut_ExitCallback
********************************************************************************
* Summary:  Turn the LED_OutB off at the end of the function  
* USB_MIDI2_ProcessUsbOut() when data is put in the UART2 out-buffer.
* 
*******************************************************************************/
void USB_MIDI2_ProcessUsbOut_ExitCallback(void)
{
  LED_OutB_Write(0);
}


// Public functions
//=============================================================================

UsbInterface_t* UsbInterface_GetHandle (void)
{
  return &_usb.public;
}



/* [] END OF FILE */
