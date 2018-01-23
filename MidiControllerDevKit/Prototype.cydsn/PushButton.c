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

#include "PushButton.h"

#ifndef NULL
  #define NULL (0)
#endif


// Private definitions
//================================================================================

typedef struct
{
  PushButton_t  public;
} _PushButton_t;


// Private prototypes
//================================================================================

int8_t _PushButton_Init (_PushButton_t *self);

// Private variables
//================================================================================

_PushButton_t _pb = 
{
  .public.state = PUSH_BUTTON_RELEASED
 ,.public.Init = (PushButton_Init_fct) &_PushButton_Init
};




// Private functions
//================================================================================
int8_t _PushButton_Init (_PushButton_t *self)
{
  self->public.state = PUSH_BUTTON_RELEASED;
  
  return 0;
}


// Public functions
//================================================================================

const PushButton_t* PushButton_Get (void)
{
  return &_pb.public;
}


/* [] END OF FILE */
