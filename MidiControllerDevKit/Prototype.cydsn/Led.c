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

#include "Led.h"

#ifndef NULL
  #define NULL (0)
#endif


// Private definitions
//================================================================================

typedef struct
{
  Led_t          public;
} _Led_t;


// Private prototypes
//================================================================================

int8_t _Led_Init (_Led_t *self);

// Private variables
//================================================================================

_Led_t _led = 
{
  .public.state = LED_OFF
 ,.public.Init = (Led_Init_fct) &_Led_Init
};




// Private functions
//================================================================================
int8_t _Led_Init (_Led_t *self)
{
  self->public.state = LED_OFF;
  
  return 0;
}


// Public functions
//================================================================================

const Led_t* Led_Get (void)
{
  return &_led.public;
}

/* [] END OF FILE */
