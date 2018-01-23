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

#include "Keyboard.h"
#include "Keyboard_bsp.h"
#include <project.h>
#include "MidiInterface.h"


// Private definitions
//======================================================================

typedef struct
{
  Keyboard_t public;
  MidiInterface_t *midi;
  uint8_t notes[KEYBOARD_N_ROWS][KEYBOARD_N_COLUMNS];
  uint8_t notesIdx[KEYBOARD_N_ROWS][KEYBOARD_N_COLUMNS];
  uint8_t notesOffset;
  KeyboardRow_t *rows[KEYBOARD_N_ROWS];
  uint8_t oReadyToScan;
} _Keyboard_t;


// Private prototypes
//======================================================================

void _Keyboard_Init (_Keyboard_t *self);
void _Keyboard_Scan (_Keyboard_t *self);
uint8_t _Keyboard_FsmStep (_Keyboard_t *self);
void _Keyboard_Timer_cb (_Keyboard_t *self);
void _Keyboard_Sleep (_Keyboard_t *self);
void _Keyboard_WakeUp (_Keyboard_t *self);


// Private variables
//======================================================================

_Keyboard_t _keyboard = 
{
  .public.Init = (void(*)(Keyboard_t*))&_Keyboard_Init
 ,.public.FsmStep = (uint8_t(*)(Keyboard_t*)) &_Keyboard_FsmStep
 ,.public.Sleep = (void(*)(Keyboard_t*)) &_Keyboard_Sleep
 ,.public.WakeUp = (void(*)(Keyboard_t*)) &_Keyboard_WakeUp
};


// Private functions
//======================================================================

void _Keyboard_Init (_Keyboard_t *self)
{
  uint8_t i, j, k;
  
  self->oReadyToScan = 0;
  
  KeyboardBsp_Init((void(*)(void*))&_Keyboard_Timer_cb, (void*) self);
  
  self->midi = MidiInterface_GetHandle();
  
  self->notesOffset = keyboardNotesInitialOffset;
  
  for (i = 0; i < KEYBOARD_N_ROWS; i++)
  {
    self->rows[i] = KeyboardBsp_GetRowHandle(i);
  }
  
  for (i = 0, k = 0; i < KEYBOARD_N_ROWS; i++)
  {
    for (j = 0; j < KEYBOARD_N_COLUMNS; j++, k++)
    {
      self->notesIdx[i][j] = keyboardNotes[k];
      self->notes[i][j] = KEYBOARD_KEY_RELEASED;
    }
  }
}


uint8_t _Keyboard_FsmStep (_Keyboard_t *self)
{
  uint8_t ret = 0;
  
  if (self->oReadyToScan)
  {
    self->oReadyToScan = 0;
    _Keyboard_Scan(self);
  }
  
  return ret;
}


void _Keyboard_Scan (_Keyboard_t *self)
{
  uint8_t i, j;
  uint8_t col[KEYBOARD_N_COLUMNS];
  
  for (i = 0; i < KEYBOARD_N_ROWS; i++)
  {
    self->rows[i]->ScanRow(self->rows[i], col);
    for (j = 0; j < KEYBOARD_N_COLUMNS; j++)
    {
      if (col[j] != self->notes[i][j])
      {
        self->notes[i][j] = col[j];
        if (col[j] == KEYBOARD_KEY_PRESSED)
        {
          self->midi->NewKeyMsg(self->midi, self->notesIdx[i][j] + self->notesOffset, KEYBOARD_DEFAULT_VELOCITY, MIDI_NOTE_ON);
        }
        else
        {
          self->midi->NewKeyMsg(self->midi, self->notesIdx[i][j] + self->notesOffset, KEYBOARD_DEFAULT_VELOCITY, MIDI_NOTE_OFF);
        }
      }
    }
  }
}


void _Keyboard_Timer_cb (_Keyboard_t *self)
{
  self->oReadyToScan = 1;
}

void _Keyboard_Sleep (_Keyboard_t *self)
{
  KeyboardBsp_Sleep();
  self->oReadyToScan = 0;
}


void _Keyboard_WakeUp (_Keyboard_t *self)
{
  KeyboardBsp_WakeUp();
}


// Public functions
//======================================================================

Keyboard_t * Keyboard_GetHandle (void)
{
  return &_keyboard.public;
}



/* [] END OF FILE */
