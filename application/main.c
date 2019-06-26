#include "stm32f4xx_hal.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "adc1.h"

extern volatile int tick_counter;
int executionTime = 0;

SemaphoreHandle_t buttonSemaphore;
SemaphoreHandle_t WavMutex;
QueueHandle_t WavQueue;
ADC_HandleTypeDef *hadc1;			 // handle for ADC1
uint32_t ADC1Data;   					 // 12 bit data from adc

TaskHandle_t TxHandle = NULL;
TaskHandle_t RxHandle = NULL;


void SystemClock_Config(void); // 180 MHz clock from 8 MHz XTAL and PLL
static void EXTI2_Init(void);
void EXTI2_IRQHandler(void);
	
void taskButton(void* params)
{
	buttonSemaphore = xSemaphoreCreateBinary();
	xSemaphoreTake(WavMutex, 0);
	static uint8_t state;
	while(1)
	{
			if(xSemaphoreTake(buttonSemaphore, portMAX_DELAY))
			{
				HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // green
				state = state ^ 1; 												// State switches between 1 - rising edge and 0 - falling edge
				if(state)																	// State 1 - create tasks: start recording and saving files
				{		
					xSemaphoreGive(WavMutex);	
				}
				else																			// State 0 - delete tasks, stop everything
				{	
					xSemaphoreTake(WavMutex, 0);
				}	
			}	
			
	}
}

void taskADCtoQue(void* params)
{
	WavQueue = xQueueCreate(100, sizeof(uint32_t));
	WavMutex = xSemaphoreCreateMutex();
  while (1) 
	{	 
		
		if(xSemaphoreTake(WavMutex, 0))
		{		

			//ADC1_Start(hadc1);
			//ADC1Data = ADC1_Get_Value(hadc1);
			ADC1Data = 1;
			if (xQueueSend(WavQueue, &ADC1Data, 1000) != pdPASS)
			{
					// Failed to put new element into the queue, even after 1000 ticks.				
			}
			xSemaphoreGive(WavMutex);
		
		}
		
	}
} 

void taskSDfromQue(void* params)
{
	WavQueue = xQueueCreate(100, sizeof(uint32_t));
	WavMutex = xSemaphoreCreateMutex();
  while (1) 
	{  
		
		if(xSemaphoreTake(WavMutex, 0))
		{		
		
			if (xQueueReceive(WavQueue, &ADC1Data, 1000 ) == pdTRUE)  
			{
				// element was received successfully
				USART_WriteString("z");
			}	
			xSemaphoreGive(WavMutex);
		
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
	
		if (pdPASS != xTaskCreate(taskButton, "Button", configMINIMAL_STACK_SIZE, NULL, 3, NULL)) 
		{
			USART_WriteString("hejka.\n\r");
		}
		
		if (pdPASS != xTaskCreate(taskADCtoQue, "ADC to Queue", configMINIMAL_STACK_SIZE, NULL, 3, &TxHandle)) 
		{
			USART_WriteString("hejka.\n\r");
		}
		if (pdPASS != xTaskCreate(taskSDfromQue, "Queue to ADC", configMINIMAL_STACK_SIZE, NULL, 3, &RxHandle)) 
		{
			USART_WriteString("hejka.\n\r");
		}
	
    vTaskStartScheduler();
}

void SystemClock_Config(void) {
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
static void EXTI2_Init(void)
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
 
        
  // Enable and set EXTI Line2 Interrupt to the lowest priority
  HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 1);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
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
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);	  // Toggle LED red 
		xSemaphoreGiveFromISR(buttonSemaphore, 0);
  }
}
