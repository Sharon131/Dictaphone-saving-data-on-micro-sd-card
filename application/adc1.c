#include "adc1.h"

void ADC1_Init(ADC_HandleTypeDef *hadc)
{
  // Configure ADC (Clock, Resolution, Data Alignment and number of conversion) 
  
	// ADC in single shot mode - processing each ADC sample when it's available
  hadc->Instance = ADC1;           
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;						 // Divide clock frequency by 8
  hadc->Init.Resolution = ADC_RESOLUTION_12B;											 // The most accurate resolution - 12b
  hadc->Init.ScanConvMode = DISABLE;															 // Allows multiple channel conversion at the same time
  hadc->Init.ContinuousConvMode = DISABLE;												 // Next conversion immediately after last finishes. Only for DMA or Interrupts
  hadc->Init.DiscontinuousConvMode = DISABLE;											 // 
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  // External event that will start conversion
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;								 // Conversion started by software
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;											 // Result data in register is alligned to the RIGHT
  hadc->Init.NbrOfConversion = 1;																 	 // 1 conversion
  hadc->Init.DMAContinuousRequests = DISABLE;											 // For DMA
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;									 // End Of Conversion flag is set after conversion finishes
 
	 ADC_ChannelConfTypeDef sConfig;   // ADC1 channel options
  // Configure selected ADC channel 
  sConfig.Channel = ADC_CHANNEL_7;																 // 7 channel is where (AUDIO in) from expanding board is
  sConfig.Rank = 1;																								 // 1 position in measurments queue
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; 								 // Conversion time 480 clock cycles, wchich is maximum
	// sampling rate is about 44KHz: 180000000/(8*480) = 46875Hz
	// Final conversion time will be (8 * (480 + 12(constant cycles)) ) / (ADC clock = 180MHz)	
}

void ADC1_Start(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Start(hadc);
}

void ADC1_Stop(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop(hadc);
}

HAL_StatusTypeDef ADC1_conv_finished(ADC_HandleTypeDef* hadc, uint32_t checktime)
{
	return HAL_ADC_PollForConversion(hadc, checktime); 								
}

uint32_t ADC1_Get_Value(ADC_HandleTypeDef* hadc)
{
	if(ADC1_conv_finished(hadc, 10) == HAL_OK)
	{
		return HAL_ADC_GetValue(hadc);
	}
	return 0;
}

uint32_t ADC1_Get_sampleRate(ADC_HandleTypeDef* hadc)
{
	return 46875;
}

uint16_t ADC1_Get_numChannels(ADC_HandleTypeDef* hadc)
{
	return 1;
}

uint16_t ADC1_Get_bitsPerSample(ADC_HandleTypeDef* hadc)
{
	return 12;
}
