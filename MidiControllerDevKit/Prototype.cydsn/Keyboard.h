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

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
  
  
#include "stdint.h"
  
  
  
typedef struct Keyboard_t Keyboard_t;
struct Keyboard_t
{
  void (*Init) (Keyboard_t *self);
};
  
  
  
  
  
#endif // __KEYBOARD_H__

/* [] END OF FILE */
