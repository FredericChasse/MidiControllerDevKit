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

#ifndef __KEYBOARD_BSP_H__
#define __KEYBOARD_BSP_H__
  
#include "stdint.h"
  
// Definitions
//======================================================================
#define KEYBOARD_KEY_PRESSED        (0)
#define KEYBOARD_KEY_RELEASED       (1)

#define KEYBOARD_N_COLUMNS          (8)
#define KEYBOARD_N_ROWS             (4)
  
#define KEYBOARD_N_NOTES            (32)

#define KEYBOARD_DEFAULT_VELOCITY   (100)
  
extern const uint8_t keyboardNotes[KEYBOARD_N_NOTES];
extern const uint8_t keyboardNotesInitialOffset;
  

typedef struct KeyboardRow_t KeyboardRow_t;
struct KeyboardRow_t
{
  void (*ScanRow)(KeyboardRow_t *self, uint8_t *keys);
};

KeyboardRow_t* KeyboardBsp_GetRowHandle (uint8_t id);
void KeyboardBsp_Init (void(*cb)(void*), void* cb_ctx);

void KeyboardBsp_Sleep (void);
void KeyboardBsp_WakeUp (void);
  
  
#endif // __KEYBOARD_BSP_H__

/* [] END OF FILE */
