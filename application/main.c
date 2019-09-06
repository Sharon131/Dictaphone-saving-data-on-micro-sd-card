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

static uint8_t semaphoreState;
SemaphoreHandle_t WavSemaphore;
QueueHandle_t WavQueue;				 // Queue for ADC1 data
ADC_HandleTypeDef *hadc1;			 // handle for ADC1
uint32_t ADC1Data;   					 // 12 bit data from adc

TaskHandle_t TxHandle = NULL;
TaskHandle_t RxHandle = NULL;

typedef struct 
{															// [BE] = Big Endian, [LE] = Little Endian													
		// The "RIFF" chunk descriptor: header
	uint8_t ChunkID[4]; 						// 0x52 49 46 46 == "RIFF" [BE]
	uint32_t ChunkSize;						// 36 + Subchunk2Size [LE]
	uint8_t Format[4];							// 0x57 41 56 45 == "WAVE" [BE]
		
		// The "fmt" sub-chunk: sound data's format
	uint8_t Subchunk1ID[4];					// 0x66 6d 74 20 = "fmt " [BE]
	uint32_t Subchunk1Size;				// 16 for PCM, size of the rest of this subchunk [LE]
	uint16_t AudioFormat;					// PCM = 1 (li near quantization) !=1 for compression [LE]
	uint16_t NumChannels;					// mono = 1, Stereo = 2 [LE]
	uint32_t SampleRate;					// 8000, 44100(most common), etc. [LE]
	uint32_t ByteRate;						// == Sample Rate * NumChannels * BitsPerSample/8 [LE]
	uint16_t BlockAlign;					// == NumChannels * BitsPerSample/8 [LE]
																// Number of bytes for one sample including all channels	
	uint16_t BitsPerSample;				// 8 bits = 8, 16 bits = 16, etc. [LE]
		
		// the "data" sub-chunk: size of data and actual sound
	uint8_t Subchunk2ID[4];					// 0x64 61 74 61 == "data" [BE]
	uint32_t Subchunk2Size;				// == NumSamples * NumChannels * BitsPerSample/8 [LE]
	//uint8_t data[];	// actual sound data, [Subchunk2Size] [LE]
} WavFileHeader;
	
typedef struct
{
		WavFileHeader header;
	uint8_t* data;
	uint64_t index;
	uint64_t size;
	uint64_t nSamples;
} WaveFile;

	WavFileHeader generateWaveHeader(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample);
	WaveFile makeWave(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample);
	
void SystemClock_Config(void); // 180 MHz clock from 8 MHz XTAL and PLL


void taskADCtoQue(void* params)
{
	
	WavSemaphore = xSemaphoreCreateBinary();
	
  while (1) 
	{	 
		if(WavSemaphore != NULL)
		{
			if(xSemaphoreTake(WavSemaphore, 1) == pdTRUE)
			{		
				//ADC1_Start(hadc1);
				//ADC1Data = ADC1_Get_Value(hadc1);
				ADC1Data = 1;
				if (xQueueSend(WavQueue, &ADC1Data, 1000) != pdPASS)
				{	
						// Failed to put new element into the queue, even after 1000 ticks.			
						USART_WriteString("*");				
				}
				USART_WriteString("T");
				if(semaphoreState)
				{
						xSemaphoreGive(WavSemaphore);
				}							
			}	
		}			
	}
} 

void taskSDfromQue(void* params)
{
	
  while (1) 
	{  			
		if (xQueueReceive(WavQueue, &ADC1Data, 1000 ) == pdTRUE)  
		{
			// element was received successfully
			USART_WriteString("R");
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
	WavQueue = xQueueCreate(100, sizeof(uint32_t));
	
	if (pdPASS != xTaskCreate(taskADCtoQue, "ADC to Queue", configMINIMAL_STACK_SIZE * 4, NULL, 4, &TxHandle)) 
	{
		USART_WriteString("hejka.\n\r");
	}
	if (pdPASS != xTaskCreate(taskSDfromQue, "Queue to SD", configMINIMAL_STACK_SIZE * 4, NULL, 2, &RxHandle)) 
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
 

WavFileHeader generateWaveHeader(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample)
{
	//RIFF WAVE Header
	WavFileHeader newHeader;
	newHeader.ChunkID[0] = 'R';
	newHeader.ChunkID[1] = 'I';
	newHeader.ChunkID[2] = 'F';
	newHeader.ChunkID[3] = 'F';
	newHeader.Format[0] = 'W';
	newHeader.Format[1] = 'A';
	newHeader.Format[2] = 'V';
	newHeader.Format[3] = 'E';
	
	//Format subchunk
	newHeader.Subchunk1ID[0] = 'f';
  newHeader.Subchunk1ID[1] = 'm';
  newHeader.Subchunk1ID[2] = 't';
  newHeader.Subchunk1ID[3] = ' ';
	newHeader.AudioFormat = 1; // PCM
	newHeader.NumChannels = numChannels;
	newHeader.SampleRate = sampleRate; // most likely 44100 hertz
	newHeader.BitsPerSample = bitsPerSample; // 
  newHeader.ByteRate = newHeader.SampleRate * newHeader.NumChannels * newHeader.BitsPerSample / 8;
  newHeader.BlockAlign = newHeader.NumChannels * newHeader.BitsPerSample/8;
	
	// Data subchunk
  newHeader.Subchunk2ID[0] = 'd';
  newHeader.Subchunk2ID[1] = 'a';
  newHeader.Subchunk2ID[2] = 't';
  newHeader.Subchunk2ID[3] = 'a';
	
	// All sizes:
  // chuckSize = 4 + (8 + subChunk1Size) + (8 + subChubk2Size)
  // subChunk1Size is constanst, i'm using 16 and staying with PCM
  // subChunk2Size = nSamples * nChannels * bitsPerSample/8
  // Whenever a sample is added:
  //    chunkSize += (nChannels * bitsPerSample/8)
  //    subChunk2Size += (nChannels * bitsPerSample/8)
	newHeader.ChunkSize = 4+8+16+8+0;
  newHeader.Subchunk1Size = 16;
  newHeader.Subchunk2Size = 0;
	
	return newHeader;
}

WaveFile makeWave(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample)
{
	WaveFile newWave;
	newWave.header = generateWaveHeader(sampleRate, numChannels, bitsPerSample);
	
	return newWave;
	
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
		
		semaphoreState = semaphoreState ^ 1; 			// State switches between 1 - rising edge and 0 - falling edge		
		if(semaphoreState)
		{
		xSemaphoreGiveFromISR(WavSemaphore, NULL);
		}
  }
}
