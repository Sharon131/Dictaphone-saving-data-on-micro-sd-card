#ifndef _ADC1_H_
#define _ADC1_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
// Function to initialize ADC1 interface with channel 7 
// which will be used for AUDIO in signal
void ADC1_Init(ADC_HandleTypeDef *hadc);

// Function that will start conversion - software trigger
void ADC1_Start(ADC_HandleTypeDef* hadc);

// Checks for conversion finished flag
HAL_StatusTypeDef ADC1_conv_finished(ADC_HandleTypeDef* hadc, uint8_t checktime);
	
// returns conversion value
uint32_t ADC1_Get_Value(ADC_HandleTypeDef* hadc);

#endif
