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

#ifndef __PUSH_BUTTON_H__
#define __PUSH_BUTTON_H__

#include "stdint.h"
    
typedef struct PushButton_t PushButton_t;
    
typedef int8_t (*PushButton_Init_fct) (PushButton_t *self);


typedef enum
{
  PUSH_BUTTON_PRESSED
 ,PUSH_BUTTON_RELEASED
} PushButtonState_t;


struct PushButton_t
{
  PushButtonState_t   state;
  PushButton_Init_fct Init;
};

const PushButton_t* PushButton_Get(void);


#endif //__PUSH_BUTTON_H__

/* [] END OF FILE */
