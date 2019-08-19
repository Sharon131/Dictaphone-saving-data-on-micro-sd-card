#include "sd_card.h"


void SD_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//CS Init
	GPIO_InitStructure.Pin = GPIO_PIN_2;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//MISO, MOSI Init
	GPIO_InitStructure.Pin = GPIO_PIN_14 | GPIO_PIN_9;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;	//nie czasem to: GPIO_SPEED_HIGH???
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Alternate = GPIO_AF_5;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	//SCKL Init
	GPIO_InitStructure.Pin = GPIO_PIN_6;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//GPIO_PinRemapConfig();
}
