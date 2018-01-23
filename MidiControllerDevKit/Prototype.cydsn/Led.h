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

#ifndef __LED_H__
#define __LED_H__

#include "stdint.h"
    
typedef struct Led_t Led_t;
    
typedef int8_t (*Led_Init_fct) (Led_t *self);


typedef enum
{
  LED_ON
 ,LED_OFF
} LedState_t;


struct Led_t
{
  LedState_t   state;
  Led_Init_fct Init;
};

const Led_t* Led_Get (void);


#endif //__LED_H__

/* [] END OF FILE */
