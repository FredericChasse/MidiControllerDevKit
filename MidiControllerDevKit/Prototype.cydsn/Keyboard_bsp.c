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

#include "stdint.h"
#include "Keyboard_bsp.h"
#include "GpioInterface.h"
#include <project.h>
#include "MidiNotes.h"

// Private definitions
//======================================================================
#define KEYBOARD_ROW_ON        (0)
#define KEYBOARD_ROW_OFF       (1)

typedef struct
{
  KeyboardRow_t public;
  uint8_t nColumns;
  Gpio_t *columns;
  Gpio_t row;
} _KeyboardRow_t;


Gpio_t _columns[KEYBOARD_N_COLUMNS];
_KeyboardRow_t _rows[KEYBOARD_N_ROWS];

// Private prototypes
//======================================================================

void _KeyboardBsp_ScanRow (_KeyboardRow_t *self, uint8_t *keys);


// Private variables
//======================================================================



// Private functions
//======================================================================

void _KeyboardBsp_ScanRow (_KeyboardRow_t *self, uint8_t *keys)
{
  uint8_t i;
  
  self->row.Write(KEYBOARD_ROW_ON);
  CyDelayUs(100);
  
  for (i = 0; i < self->nColumns; i++)
  {
    keys[i] = self->columns[i].Read();
  }
  
  self->row.Write(KEYBOARD_ROW_OFF);
}


// Public functions
//======================================================================

void KeyboardBsp_Init (void)
{  
  _columns[0].Read  = &KeyboardCol_1_Read;
  _columns[0].Write = &KeyboardCol_1_Write;
  _columns[1].Read  = &KeyboardCol_2_Read;
  _columns[1].Write = &KeyboardCol_2_Write;
  _columns[2].Read  = &KeyboardCol_3_Read;
  _columns[2].Write = &KeyboardCol_3_Write;
  _columns[3].Read  = &KeyboardCol_4_Read;
  _columns[3].Write = &KeyboardCol_4_Write;
  _columns[4].Read  = &KeyboardCol_5_Read;
  _columns[4].Write = &KeyboardCol_5_Write;
  _columns[5].Read  = &KeyboardCol_6_Read;
  _columns[5].Write = &KeyboardCol_6_Write;
  _columns[6].Read  = &KeyboardCol_7_Read;
  _columns[6].Write = &KeyboardCol_7_Write;
  _columns[7].Read  = &KeyboardCol_8_Read;
  _columns[7].Write = &KeyboardCol_8_Write;
  
  _rows[0].row.Read  = &KeyboardRow_1_Read;
  _rows[0].row.Write = &KeyboardRow_1_Write;
  _rows[0].nColumns = KEYBOARD_N_COLUMNS;
  _rows[0].columns = _columns;
  _rows[0].public.ScanRow = (void(*)(KeyboardRow_t*,uint8_t*)) &_KeyboardBsp_ScanRow;
  
  _rows[0].row.Read  = &KeyboardRow_2_Read;
  _rows[0].row.Write = &KeyboardRow_2_Write;
  _rows[0].nColumns = KEYBOARD_N_COLUMNS;
  _rows[0].columns = _columns;
  _rows[0].public.ScanRow = (void(*)(KeyboardRow_t*,uint8_t*)) &_KeyboardBsp_ScanRow;
  
  _rows[0].row.Read  = &KeyboardRow_3_Read;
  _rows[0].row.Write = &KeyboardRow_3_Write;
  _rows[0].nColumns = KEYBOARD_N_COLUMNS;
  _rows[0].columns = _columns;
  _rows[0].public.ScanRow = (void(*)(KeyboardRow_t*,uint8_t*)) &_KeyboardBsp_ScanRow;
  
  _rows[0].row.Read  = &KeyboardRow_4_Read;
  _rows[0].row.Write = &KeyboardRow_4_Write;
  _rows[0].nColumns = KEYBOARD_N_COLUMNS;
  _rows[0].columns = _columns;
  _rows[0].public.ScanRow = (void(*)(KeyboardRow_t*,uint8_t*)) &_KeyboardBsp_ScanRow;
}


KeyboardRow_t* KeyboardBsp_GetRowHandle (uint8_t id)
{
  if (id >= KEYBOARD_N_ROWS)
  {
    return NULL;
  }
  
  return &_rows[id].public;
}


// Public constants
//======================================================================

const uint8_t keyboardNotes[KEYBOARD_N_NOTES] = 
{  0,  1,  2,  3,  4,  5,  6,  7
,  8,  9, 10, 11, 12, 13, 14, 15
, 16, 17, 18, 19, 20, 21, 22, 23
, 24, 25, 26, 27, 28, 29, 30, 31 };

const uint8_t keyboardNotesOffset = F_4;

/* [] END OF FILE */
