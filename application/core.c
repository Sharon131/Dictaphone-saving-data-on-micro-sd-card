/* Includes ------------------------------------------------------------------*/
#include "core.h"
#include "stm32f4xx_hal.h"


void CORE_EnterCriticalSection(void){	
	__disable_irq();
}

void CORE_ExitCriticalSection(void) {
	__enable_irq();
}

uint32_t isLittleEndian(void)
{
	uint32_t test = 1;
	uint8_t *p = (uint8_t*)&test;
	
	return p[0] == 1;
}

void reverseEndianness(void* value, uint32_t size)
{
	uint32_t i;
	uint8_t result[32]; // room for 32 bytes
	
	for(i = 0; i < size; i++)
	{
		result[i] = ((uint8_t*)value)[size - i - 1];
	}
	
	for(i = 0; i < size; i++)
	{
		((uint8_t*)value)[i] = result[i];		
	}
}
