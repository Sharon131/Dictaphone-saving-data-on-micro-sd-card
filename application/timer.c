#include "timer.h"
#include "stm32f4xx_hal.h"

#include "sd_stm32.h"

TIM_HandleTypeDef tim7;

void TimerInit(void){
	
	HAL_TIM_Base_Init(&tim7);
	
	TIM_MasterConfigTypeDef sMasterConfig = {0};

  tim7.Instance = TIM7;
  tim7.Init.Prescaler = 80;
  tim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim7.Init.Period = 21000;
  
	HAL_TIM_Base_Init(&tim7);
	
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
	HAL_TIMEx_MasterConfigSynchronization(&tim7, &sMasterConfig);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    __HAL_RCC_TIM7_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn); 
}

void TIM7_IRQHandler(void){
	
	sdcard_systick_timerproc();
	
	
  HAL_TIM_IRQHandler(&tim7);
}
