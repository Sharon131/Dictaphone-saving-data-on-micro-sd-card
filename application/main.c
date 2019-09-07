#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "adc1.h"
#include "exti2.h"
#include "wav.h"

#define BUFFER_SIZE 256 // uint16 from adc

//FATFS myFATFS; //fatfs object
// FIL myFILE; //file object

static uint8_t adcState;
SemaphoreHandle_t onOffSemaphore;
SemaphoreHandle_t adcSemaphore;
SemaphoreHandle_t sdSemaphore;

WaveFile *wavHandler;
ADC_HandleTypeDef *adc1Handler;			 // handle for ADC1

uint16_t dataToSd[BUFFER_SIZE]; // SD card takes 512 bytes at one time

void SystemClock_Config(void); // 180 MHz clock from 8 MHz XTAL and PLL

void taskOnOff(void* params)
{
	static uint8_t fileCounter; // 10 .wav files maximum
	
  while (1) 
	{
		if(xSemaphoreTake(onOffSemaphore, portMAX_DELAY) == pdTRUE)
		{	
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);	  // Toggle LED red 			
			adcState = adcState ^ 1; 			// State switches between
																		//1 - rising edge and 0 - falling edge of exti irq	
			if(adcState) // dictaphone ON!
			{	
				xSemaphoreGive(adcSemaphore);
//				if(f_mount(&myFATFS, SD_Path, 1) == FR_OK)
//				{
//					char myPath[] = "file0.wav";
//					myPath[4] = fileCounter;
//					f_open(&myFILE, myPath, F_WRITE);
//					
//					
//					
//					wavHandler = &(makeWave(ADC1_Get_sampleRate(adc1Handler),
//																						ADC1_Get_numChannels(adc1Handler),
//																						ADC1_Get_bitsPerSample(adc1Handler)));
//					?? f_write(wavHandler)
//				}
			
			}
			else // dictaphone OFF! 
			{
//				f_close(myFILE)
//				fileCounter++;
			}
		}
	}
}

void taskADCtoBuffer(void* params)
{
	static uint32_t counter;
  while (1) 
	{	 
		if(xSemaphoreTake(adcSemaphore, 1) == pdTRUE)
		{				
			vTaskSuspendAll();
			//taskENTER_CRITICAL();	
			//ADC1_Start(adc1Handler);
			
			USART_WriteString("T");		
		
			//dataToSd[counter] = (uint16_t)ADC1_Get_Value(adc1Handler);;
			counter += 1;
			if(counter == BUFFER_SIZE - 1)
			{
				counter = 0;
				xSemaphoreGive(sdSemaphore);
			}
			if(adcState)
			{
				xSemaphoreGive(adcSemaphore);
			}
			else
			{
				//ADC1_Stop(adc1Handler);
				xSemaphoreGive(sdSemaphore);
			}				
			//taskEXIT_CRITICAL();
			xTaskResumeAll();
		}	
	}			
} 

void taskBufferToSD(void* params)
{
  while (1) 
	{  			
		if(xSemaphoreTake(sdSemaphore, portMAX_DELAY) == pdTRUE)
		{	
			// 512 bytes were received successfully
			vTaskSuspendAll();			
			taskENTER_CRITICAL();		
			USART_WriteString("\n\rELO\n\r");	
			taskEXIT_CRITICAL();
			xTaskResumeAll();
			//for (i = 0 i < BUFFER_SIZE, i++)
			//addWaveSample (dataToSD[i])
			//f_write dataToSD	
			//memset(dataToSd, 0, sizeof(dataToSd));
		}			
	}
}



int main(void)
{
	HAL_Init();
	SystemClock_Config();
  LED_Init();
  USART_Init();
  TRACE_Init();
	EXTI2_Init();	
	ADC1_Init(adc1Handler);	
	//SPI_Init()
	//Fatfs_init()
	
	adcSemaphore = xSemaphoreCreateBinary();
	sdSemaphore = xSemaphoreCreateBinary();
	onOffSemaphore = xSemaphoreCreateBinary();
	
	if (pdPASS != xTaskCreate(taskOnOff, "Start and Finish logic", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL)) 
	{
		USART_WriteString("ON/OFF TASK ERROR.\n\r");
	}
	
	if (pdPASS != xTaskCreate(taskADCtoBuffer, "ADC to Buffer", configMINIMAL_STACK_SIZE * 4, NULL, 2, NULL)) 
	{
		USART_WriteString("ADC TASK ERROR.\n\r");
	}
	
	if (pdPASS != xTaskCreate(taskBufferToSD, "Buffer to SD", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL)) 
	{
		USART_WriteString("SD TASK ERROR.\n\r");
	}
	
  vTaskStartScheduler();
}

void SystemClock_Config(void) 
{	
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
 
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
 
  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
 
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
 

/**
 * This function handles External line 2 interrupt request.
 */
void EXTI2_IRQHandler(void)
{	
  // Check if EXTI line interrupt was detected
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)  
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);			// Clear the interrupt (has to be done for EXTI)
		xSemaphoreGiveFromISR(onOffSemaphore, NULL);	
  }
}
