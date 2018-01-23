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


// Private definitions
//=============================================================================

typedef struct
{
  MidiInterface_t public;
  uint8_t oInitialized;
} _Midi_t;


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
  
  self->oInitialized = 1;
}


int8_t _Midi_NewKeyMsg (_Midi_t *self, uint8_t key, uint8_t velocity, MidiNoteState_t onOff)
{
  int8_t ret = 0;
  
  return ret;
}



// Public functions
//=============================================================================






/* [] END OF FILE */
