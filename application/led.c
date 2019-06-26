#include "led.h"

void LED_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct_13;
    GPIO_InitTypeDef GPIO_InitStruct_14;
 
  // GPIO Ports Clock Enable
  __GPIOG_CLK_ENABLE();
 
  // Configure GPIO pin PG13
  GPIO_InitStruct_13.Pin   = GPIO_PIN_13;
  GPIO_InitStruct_13.Mode  = GPIO_MODE_OUTPUT_PP;        // push-pull output
  GPIO_InitStruct_13.Pull  = GPIO_PULLDOWN;              // pull-down enabled
  GPIO_InitStruct_13.Speed = GPIO_SPEED_LOW;             // analog pin bandwidth limited
  
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_13); 
    
  // Configure GPIO pin PG14
  GPIO_InitStruct_14.Pin   = GPIO_PIN_14;
  GPIO_InitStruct_14.Mode  = GPIO_MODE_OUTPUT_PP;        // push-pull output
  GPIO_InitStruct_14.Pull  = GPIO_PULLDOWN;              // pull-down enabled
  GPIO_InitStruct_14.Speed = GPIO_SPEED_LOW;             // analog pin bandwidth limited
  
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct_14); 
   
}

