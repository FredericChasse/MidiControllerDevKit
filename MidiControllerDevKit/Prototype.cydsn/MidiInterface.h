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

#ifndef __MIDI_INTERFACE_H__
#define __MIDI_INTERFACE_H__
  
#include "stdint.h"
  
typedef enum
{
  MIDI_NOTE_OFF
 ,MIDI_NOTE_ON
} MidiNoteState_t;

typedef struct MidiInterface_t MidiInterface_t;
struct MidiInterface_t
{
  void (*Init) (MidiInterface_t *self);
  int8_t (*NewKeyMsg) (MidiInterface_t *self, uint8_t key, uint8_t velocity, MidiNoteState_t onOff);
};
  
  
MidiInterface_t* MidiInterface_GetHandle(void);
  
  
#endif // __MIDI_INTERFACE_H__

/* [] END OF FILE */
