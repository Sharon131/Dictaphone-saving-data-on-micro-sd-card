#include "stm32f4xx_hal.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "adc1.h"
#include "exti2.h"

extern volatile int tick_counter;
int executionTime = 0;

SemaphoreHandle_t buttonSemaphore;
SemaphoreHandle_t WavMutex;
QueueHandle_t WavQueue;				 // Queue for ADC1 data
ADC_HandleTypeDef *hadc1;			 // handle for ADC1
uint32_t ADC1Data;   					 // 12 bit data from adc

TaskHandle_t TxHandle = NULL;
TaskHandle_t RxHandle = NULL;


void SystemClock_Config(void); // 180 MHz clock from 8 MHz XTAL and PLL

void taskButton(void* params)
{
	buttonSemaphore = xSemaphoreCreateBinary();
	//xSemaphoreTake(WavMutex, 0);
	static uint8_t state;
	while(1)
	{	
	USART_WriteString("*");			
			if(xSemaphoreTake(buttonSemaphore, 0))
			{
				//taskENTER_CRITICAL();
				HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // green
				state = state ^ 1; 												// State switches between 1 - rising edge and 0 - falling edge
				if(state)																	// State 1 - create tasks: start recording and saving files
				{		
					xSemaphoreGive(WavMutex);	
					USART_WriteString("G");	
				}
				else																			// State 0 - delete tasks, stop everything
				{	
					xSemaphoreTake(WavMutex, 0);
					USART_WriteString("T");	
				}	
				//taskEXIT_CRITICAL();
			}	
	}
}

void taskADCtoQue(void* params)
{
	WavQueue = xQueueCreate(100, sizeof(uint32_t));
	//WavMutex = xSemaphoreCreateMutex();
	//TickType_t startTime = xTaskGetTickCount();
  while (1) 
	{	 
		if(xSemaphoreTake(WavMutex, 0))
		{		
			taskENTER_CRITICAL();
			//ADC1_Start(hadc1);
			//ADC1Data = ADC1_Get_Value(hadc1);
			ADC1Data = 1;
			if (xQueueSend(WavQueue, &ADC1Data, 1000) != pdPASS)
			{	
					// Failed to put new element into the queue, even after 1000 ticks.			
					USART_WriteString("*");				
			}
			USART_WriteString("K");
			xSemaphoreGive(WavMutex);
			//vTaskDelayUntil(&startTime,1);
			taskEXIT_CRITICAL();
			taskYIELD();
		}			
	}
} 

void taskSDfromQue(void* params)
{
	WavQueue = xQueueCreate(100, sizeof(uint32_t));
	//WavMutex = xSemaphoreCreateMutex();
	//TickType_t startTime = xTaskGetTickCount();
  while (1) 
	{  
		if(xSemaphoreTake(WavMutex, 0))
		{		
			taskENTER_CRITICAL();
			if (xQueueReceive(WavQueue, &ADC1Data, 1000 ) == pdTRUE)  
			{
				// element was received successfully
				USART_WriteString("N");
			}	
			xSemaphoreGive(WavMutex);
			//vTaskDelayUntil(&startTime,1);
			taskEXIT_CRITICAL();
			taskYIELD();
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
	ADC1_Init(hadc1);		
	
	if (pdPASS != xTaskCreate(taskADCtoQue, "ADC to Queue", configMINIMAL_STACK_SIZE, NULL, 3, &TxHandle)) 
	{
		USART_WriteString("hejka.\n\r");
	}
	if (pdPASS != xTaskCreate(taskSDfromQue, "Queue to ADC", configMINIMAL_STACK_SIZE, NULL, 3, &RxHandle)) 
	{
		USART_WriteString("hejka.\n\r");
	}
	if (pdPASS != xTaskCreate(taskButton, "Button", configMINIMAL_STACK_SIZE, NULL, 3, NULL)) 
	{
		USART_WriteString("hejka.\n\r");
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
 * Configures EXTI Line2 (connected to PG2 pin) in interrupt mode
 */

 
/**
 * This function handles External line 2 interrupt request.
 */
void EXTI2_IRQHandler(void)
{	
  // Check if EXTI line interrupt was detected
  if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)  
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);			// Clear the interrupt (has to be done for EXTI)
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);	  // Toggle LED red 
		xSemaphoreGiveFromISR(buttonSemaphore, 0);
  }
}
