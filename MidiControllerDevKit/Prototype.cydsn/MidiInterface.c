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


#include "MidiInterface.h"
#include <project.h>
#include "USB_midi.h"
#include "UsbInterface.h"
#include "MidiNotes.h"


// Private definitions
//=============================================================================

typedef struct
{
  MidiInterface_t public;
  uint8_t oInitialized;
  UsbInterface_t *usb;
} _Midi_t;

#define MIDI_MSG_SIZE           (3)



// Private prototypes
//=============================================================================

void _Midi_Init (_Midi_t *self);
int8_t _Midi_NewKeyMsg (_Midi_t *self, uint8_t key, uint8_t velocity, MidiNoteState_t onOff);


// Private variables
//=============================================================================

_Midi_t _midi = 
{
  .public.Init = (void(*)(MidiInterface_t*)) &_Midi_Init
 ,.public.NewKeyMsg = (int8_t(*)(MidiInterface_t*, uint8_t, uint8_t, MidiNoteState_t)) &_Midi_NewKeyMsg
 ,.oInitialized = 0
};



// Private functions
//=============================================================================

void _Midi_Init (_Midi_t *self)
{
  if (self->oInitialized)
  {
    return;
  }
  
  self->usb = UsbInterface_GetHandle();
  
  self->oInitialized = 1;
}


int8_t _Midi_NewKeyMsg (_Midi_t *self, uint8_t key, uint8_t velocity, MidiNoteState_t onOff)
{
#define MIDI_MSG_TYPE           (0u)
#define MIDI_NOTE_NUMBER        (1u)
#define MIDI_NOTE_VELOCITY      (2u)
  
  int8_t ret = 0;
  uint8 midiMsg[MIDI_MSG_SIZE];

  if ( (key > MIDI_NOTE_MAX) || (velocity > MIDI_NOTE_MAX_VELOCITY) || (onOff > MIDI_NOTE_ON) )
  {
    return -1;
  }
  
  if (onOff == MIDI_NOTE_ON)
  {
    midiMsg[MIDI_MSG_TYPE] = USB_MIDI_NOTE_ON;
  }
  else
  {
    midiMsg[MIDI_MSG_TYPE] = USB_MIDI_NOTE_OFF;
  }
  
  midiMsg[MIDI_NOTE_VELOCITY] = velocity;
  
  midiMsg[MIDI_NOTE_NUMBER] = key;
  
  ret = self->usb->NewMidiMsg(self->usb, midiMsg, MIDI_MSG_SIZE);
  
  return ret;
}



// Public functions
//=============================================================================

MidiInterface_t* MidiInterface_GetHandle (void)
{
  return &_midi.public;
}




/* [] END OF FILE */
