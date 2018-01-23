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
} _Keyboard_t;


// Private prototypes
//======================================================================

void _Keyboard_Init (_Keyboard_t *self);
void _Keyboard_Scan (_Keyboard_t *self);
CY_ISR_PROTO(_KeyboardTimer_isr);


// Private variables
//======================================================================

_Keyboard_t _keyboard = 
{
  .public.Init = (void(*)(Keyboard_t*))&_Keyboard_Init
 ,.notes = {{0}, {0}, {0}, {0}}
};

volatile uint8_t _oReadyToScan = 0;


// Private functions
//======================================================================

void _Keyboard_Init (_Keyboard_t *self)
{
  uint8_t i, j, k;
  
  KeyboardBsp_Init();
  
  self->midi = MidiInterface_GetHandle();
  self->midi->Init(self->midi);
  
  self->notesOffset = keyboardNotesOffset;
  
  for (i = 0; i < KEYBOARD_N_ROWS; i++)
  {
    self->rows[i] = KeyboardBsp_GetRowHandle(i);
  }
  
  for (i = 0, k = 0; i < KEYBOARD_N_ROWS; i++)
  {
    for (j = 0; j < KEYBOARD_N_COLUMNS; j++, k++)
    {
      self->notesIdx[i][j] = keyboardNotes[k];
    }
  }
}


uint8_t _Keyboard_FsmStep (_Keyboard_t *self)
{
  uint8_t ret = 0;
  uint8_t i;
  
  if (_oReadyToScan)
  {
    _oReadyToScan = 0;
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


CY_ISR(_KeyboardTimer_isr)
{
  _oReadyToScan = 1;
}


// Public functions
//======================================================================

Keyboard_t * Keyboard_GetHandle (void)
{
  return &_keyboard.public;
}



/* [] END OF FILE */
