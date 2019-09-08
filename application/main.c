#include "stm32f4xx_hal.h"
#include "led.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "string.h"
#include "ff.h"
#include "diskio.h"
#include "sd_stm32.h"

extern volatile int tick_counter;
int executionTime = 0;
SemaphoreHandle_t mySemaphore;
SemaphoreHandle_t myMutex;
QueueHandle_t myQueue;

typedef struct {
        GPIO_TypeDef* gpio;    // GPIO port
        uint16_t      pin;     // GPIO pin
        TickType_t    ticks;   // delay expressed in system ticks
} BlinkParams;


static BlinkParams bp1 = { .gpio = GPIOG, .pin = GPIO_PIN_13, .ticks = 500};
static BlinkParams bp2 = { .gpio = GPIOG, .pin = GPIO_PIN_14, .ticks = 1000};
static BlinkParams bp3 = { .gpio = GPIOG, .pin = GPIO_PIN_14, .ticks = 1000};
static BlinkParams buttonp = { .gpio = GPIOG, .pin = GPIO_PIN_14, .ticks = 100};

void CLI_Init(void);
void SystemClock_Config(void);

static void EXTI2_Init(void);
void EXTI2_IRQHandler(void);
void USART_POLL_WriteString(const char *string);


int main(void)
{
  HAL_Init();
	SystemClock_Config();
  LED_Init();
	EXTI2_Init();	
	TimerInit();
	
	
	
	while(1){
		;
	}
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
  HAL_NVIC_SetPriority(EXTI2_IRQn, 15, 0);
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
    // Clear the interrupt (has to be done for EXTI)
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
    // Toggle LED   
		xSemaphoreGiveFromISR(mySemaphore, NULL);
  }
}
