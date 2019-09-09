#ifndef _CORE_H_
#define _CORE_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

void CORE_EnterCriticalSection(void);

void CORE_ExitCriticalSection(void);

uint32_t isLittleEndian(void);

void reverseEndianness(void* value, uint32_t size);

#endif // _CORE_H_

