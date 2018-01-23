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

typedef struct
{
  UsbInterface_t public;
  uint8_t oInitialized;
  uint8_t oActive;
  uint8_t activityCounter;
  uint8_t maxActivityCount;
  byte_buffer_t *cmdSizeBuf;
  BipBuffer_t *cmdBuf;
} _Usb_t;

#define USB_DEVICE    (0)

#define USB_MAX_CMDS      (128)
#define USB_CMD_SIZE_MAX  (3)


// Private prototypes
//=============================================================================
  
void _Usb_Init (_Usb_t *self);
uint8_t _Usb_FsmStep (_Usb_t *self);
int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize);
void _Usb_Sleep (_Usb_t *self);
void _Usb_WakeUp (_Usb_t *self);
CY_ISR_PROTO(_Usb_SleepIsr);


// Private variables
//=============================================================================

_Usb_t _usb = 
{
  .public.Init = (void(*)(UsbInterface_t*)) &_Usb_Init
 ,.public.FsmStep = (uint8_t(*)(UsbInterface_t*)) &_Usb_FsmStep
 ,.public.NewMidiMsg = (int8_t(*)(UsbInterface_t*,uint8_t*,uint8_t)) &_Usb_NewMidiMsg
 ,.public.Sleep = (void(*)(UsbInterface_t*)) &_Usb_Sleep
 ,.public.WakeUp = (void(*)(UsbInterface_t*)) &_Usb_WakeUp
 ,.oInitialized = 0
};

byte_buffer_t _cmdSizeBuf;
uint8_t       _cmdBufArray[USB_MAX_CMDS * USB_CMD_SIZE_MAX];
BipBuffer_t   _cmdBuf;
uint8_t       _cmdSizeBufArray[USB_MAX_CMDS];


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
      Sleep_isr_StartEx(_Usb_SleepIsr);

      /* Start SleepTimer's operation */
      SleepTimer_Start();

      /* Enable output endpoint */
      USB_MIDI_Init();
    }
    else
    {
      SleepTimer_Stop();
    }    
  }
  
// /* Service USB MIDI when device is configured */
//  if(0u != USB_GetConfiguration())    
//  {
//    /* Call this API from UART RX ISR for Auto DMA mode */
//    #if(!USB_EP_MANAGEMENT_DMA_AUTO) 
//      USB_MIDI_IN_Service();
//    #endif
//    
//    /* In Manual EP Memory Management mode OUT_EP_Service() 
//    *  may have to be called from main foreground or from OUT EP ISR
//    */
//    #if(!USB_EP_MANAGEMENT_DMA_AUTO) 
//      USB_MIDI_OUT_Service();
//    #endif

//    /* Sending Identity Reply Universal System Exclusive message 
//    * back to computer */
//    if(0u != (USB_MIDI1_InqFlags & USB_INQ_IDENTITY_REQ_FLAG))
//    {
//      USB_PutUsbMidiIn(sizeof(MIDI_IDENTITY_REPLY), (uint8 *) MIDI_IDENTITY_REPLY, USB_MIDI_CABLE_00);
//      USB_MIDI1_InqFlags &= ~USB_INQ_IDENTITY_REQ_FLAG;
//    }
//    #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
//      if(0u != (USB_MIDI2_InqFlags & USB_INQ_IDENTITY_REQ_FLAG))
//      {
//        USB_PutUsbMidiIn(sizeof(MIDI_IDENTITY_REPLY), \
//          (uint8 *)MIDI_IDENTITY_REPLY, USB_MIDI_CABLE_01);
//        USB_MIDI2_InqFlags &= ~USB_INQ_IDENTITY_REQ_FLAG;
//      }
//    #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */
//
//    /* Service Keys */
////    if (0u == SW1_Read()) 
////    {
////      csButtStates |= BUTT1;
////    }
////    else
////    {
////      csButtStates &= ~BUTT1;
////    }
////    if (0u == SW2_Read()) 
////    {
////      csButtStates |= BUTT2;
////    }
////    else
////    {
////      csButtStates &= ~BUTT2;
////    }
//    /* Process any button change */
//    if (0)
////    if (0u != (csButtChange = csButtStates ^ csButtStatesOld)) 
//    {
//      csButtStatesOld = csButtStates;
//
//      /* All buttons are mapped to Note-On/Off messages */
//      midiMsg[MIDI_MSG_TYPE] = USB_MIDI_NOTE_ON;
//
//      /* Button 1 */
//      if (0u != (csButtChange & BUTT1)) 
//      {
//        /* Button determines note number */			
//        midiMsg[MIDI_NOTE_NUMBER] = NOTE_72;
//        if (0u != (csButtStates & BUTT1))
//        {
//          /* Note On */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_ON;		
//        }
//        else
//        {
//          /* Note Off */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_OFF;			
//        }    
//        /* Put MIDI Note-On/Off message into input endpoint */
//        USB_PutUsbMidiIn(USB_3BYTE_COMMON, midiMsg, USB_MIDI_CABLE_00);
//      }
//
//      /* Button 2 */
//      if (0u != (csButtChange & BUTT2))
//      {
//        /* Button determines note number */			
//        //    				midiMsg[MIDI_NOTE_NUMBER] = NOTE_76;	
//        midiMsg[MIDI_NOTE_NUMBER] = NOTE_72;
//        if (0u != (csButtStates & BUTT2))
//        {
//          /* Note On */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_ON;  
//        }
//        else
//        {
//          /* Note Off */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_OFF; 
//        }    
//        /* Put MIDI Note-On/Off message into input endpoint */
//        USB_PutUsbMidiIn(USB_3BYTE_COMMON, midiMsg, USB_MIDI_CABLE_00);
//        /* Second Note message */
//        midiMsg[MIDI_MSG_TYPE] = USB_MIDI_NOTE_ON;
//        midiMsg[MIDI_NOTE_NUMBER] = NOTE_72;
//        if (0u != (csButtStates & BUTT2))
//        {
//          /* Note On */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_ON;		    
//        }
//        else
//        {
//          /* Note Off */
//          midiMsg[MIDI_NOTE_VELOCITY] = VOLUME_OFF;			
//        }    
//        /* Put MIDI Note-On/Off message into input endpoint */
//        USB_PutUsbMidiIn(USB_3BYTE_COMMON, midiMsg, USB_MIDI_CABLE_00);
//      }
//      #if(USB_EP_MANAGEMENT_DMA_AUTO) 
//        #if (USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF)
//          MIDI1_UART_DisableRxInt();
//          #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
//            MIDI2_UART_DisableRxInt();
//          #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */
//        #endif /* End USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF */            
//        USB_MIDI_IN_Service();
//        #if (USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF)
//          MIDI1_UART_EnableRxInt();
//          #if (USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF)
//            MIDI2_UART_EnableRxInt();
//          #endif /* End USB_MIDI_EXT_MODE >= USB_TWO_EXT_INTRF */
//        #endif /* End USB_MIDI_EXT_MODE >= USB_ONE_EXT_INTRF */                
//      #endif
//    }
//      /* Check if host requested USB Suspend */
//      if( usbActivityCounter >= USB_SUSPEND_TIMEOUT ) 
//      {
//
//        /***************************************************************
//        * Disable USBFS block and set DP Interrupt for wake-up 
//        * from sleep mode. 
//        ***************************************************************/
//        USB_Suspend(); 
//
//        /* Prepares system clocks for sleep mode */
//        CyPmSaveClocks();
//
//        /***************************************************************
//        * Switch to the Sleep Mode for the PSoC 3 or PSoC 5LP devices:
//        *  - PM_SLEEP_TIME_NONE: wakeup time is defined by PICU source
//        *  - PM_SLEEP_SRC_PICU: PICU wakeup source 
//        ***************************************************************/
//        CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_PICU);
//
//        /* Restore clock configuration */
//        CyPmRestoreClocks();
//
//        /* Enable USBFS block after power-down mode */
//        USB_Resume();
//
//        /* Enable output endpoint */
//        USB_MIDI_Init();
//
//        usbActivityCounter = 0u; /* Re-init USB Activity Counter*/
//      }
  
  return ret;
}


int8_t _Usb_NewMidiMsg (_Usb_t *self, uint8_t *msg, uint8_t msgSize)
{
  int8_t ret = 0;
  
  return 0;
  
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


void _Usb_Sleep (_Usb_t *self)
{
  
}


void _Usb_WakeUp (_Usb_t *self)
{
  
}


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
    SleepTimer_GetStatus();
}


// Public functions
//=============================================================================

UsbInterface_t* UsbInterface_GetHandle (void)
{
  return &_usb.public;
}



/* [] END OF FILE */
