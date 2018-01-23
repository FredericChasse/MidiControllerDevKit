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

#ifndef __GPIO_INTERFACE_H__
#define __GPIO_INTERFACE_H__
  
  
typedef struct Gpio_t Gpio_t;
  
struct Gpio_t
{
  unsigned char (*Read) (void);
  void (*Write) (unsigned char);
};
  
  
#endif // __GPIO_INTERFACE_H__

/* [] END OF FILE */
