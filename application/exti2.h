#ifndef _EXTI2_H_
#define _EXTI2_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"

/**
 * Configures EXTI Line2 (connected to PG2 pin) in interrupt mode
 */
void EXTI2_Init(void);
void EXTI2_IRQHandler(void);

#endif
