/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This example project demonstrates the MIDI interface device operation.  The 
*  project enumerates as a USB Audio Device with the MIDI feature and does not 
*  require additional drivers. The main goal of the USB MIDI interface is to 
*  transfer and convert MIDI data between external MIDI devices that use the 
*  UART interface, and a PC through the USB bus.			
*   
* Related Document:
*  Universal Serial Bus Specification Revision 2.0 
*  Universal Serial Bus Device Class Definition for MIDI Devices Release 1.0
*  MIDI 1.0 Detailed Specification Document Version 4.2
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

#include "UsbInterface.h"
#include "MidiInterface.h"
#include "Keyboard.h"

Keyboard_t *keyboard;
UsbInterface_t *usb;
MidiInterface_t *midi;


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*       1. Starts the USBFS device and waits for enumaration.
*
*******************************************************************************/
int main()
{

  /* Enable Global Interrupts */
  CyGlobalIntEnable;

  usb = UsbInterface_GetHandle();
  midi = MidiInterface_GetHandle();
  keyboard = Keyboard_GetHandle();

  usb->Init(usb);
  midi->Init(midi);
  keyboard->Init(keyboard);
  
  uint8_t sysSleep = 0;

  while(1)
  {
    keyboard->FsmStep(keyboard);
    sysSleep = usb->FsmStep(usb);
    
    if (sysSleep)
    {
      sysSleep = 0;
      
      keyboard->Sleep(keyboard);
      usb->Sleep(usb);

      /* Prepares system clocks for sleep mode */
      CyPmSaveClocks();

      /***************************************************************
      * Switch to the Sleep Mode for the PSoC 3 or PSoC 5LP devices:
      *  - PM_SLEEP_TIME_NONE: wakeup time is defined by PICU source
      *  - PM_SLEEP_SRC_PICU: PICU wakeup source 
      ***************************************************************/
      CyPmSleep(PM_SLEEP_TIME_NONE, PM_SLEEP_SRC_PICU);

      /* Restore clock configuration */
      CyPmRestoreClocks();
      
      usb->WakeUp(usb);
      keyboard->WakeUp(keyboard);
    }
  }
}


/* [] END OF FILE */
