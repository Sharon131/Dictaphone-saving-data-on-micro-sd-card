#include "exti2.h"

void EXTI2_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
        
  // Enable GPIOG clock
  __GPIOG_CLK_ENABLE();
  
  // Configure PG2 pin as input with EXTI interrupt on the falling edge and pull-up
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
 
        
  // Enable and set EXTI Line2 Interrupt to the !highest! priority
  HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 15);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}


