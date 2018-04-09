#include "stm32f10x_HC-SR04.h"
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_tim.h>
#include <stm32l4xx_ll_gpio.h>

static void initMeasureTimer()
{
	LL_RCC_ClocksTypeDef clocks;
	LL_RCC_GetSystemClocksFreq(&clocks);
	uint16_t const prescaler = clocks.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)

	LL_TIM_DeInit(US_TIMER);

	LL_TIM_InitTypeDef timInit;
	LL_TIM_StructInit(&timInit);
	timInit.Prescaler = prescaler;
	timInit.Autoreload = 0xFFFF; // Period
	timInit.RepetitionCounter = 1;
	LL_TIM_Init(US_TIMER, &timInit);

	LL_TIM_OC_InitTypeDef ocInit;
	LL_TIM_OC_StructInit(&ocInit);
	ocInit.OCMode = LL_TIM_OCMODE_PWM1;
	ocInit.CompareValue = 15; // Pulse
	ocInit.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	ocInit.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(US_TIMER, LL_TIM_CHANNEL_CH3, &ocInit);

	LL_TIM_IC_InitTypeDef icInit;
	LL_TIM_IC_StructInit(&icInit);
	icInit.ICPolarity = LL_TIM_IC_POLARITY_RISING;
	icInit.ICPrescaler = LL_TIM_ICPSC_DIV1;
	icInit.ICFilter = LL_TIM_IC_FILTER_FDIV1; // 0
	icInit.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	LL_TIM_IC_Init(US_TIMER, LL_TIM_CHANNEL_CH1, &icInit);

	LL_TIM_SetTriggerInput(US_TIMER, US_TIMER_TRIG_SOURCE);
	LL_TIM_EnableMasterSlaveMode(US_TIMER);

	MODIFY_REG(US_TIMER->BDTR, 0, TIM_BDTR_MOE); //TIM_CtrlPWMOutputs(US_TIMER, ENABLE);

	LL_TIM_ClearFlag_UPDATE(US_TIMER);
}

static void initPins()
{
	LL_GPIO_InitTypeDef gpioInit;
	gpioInit.Pin = US_TRIG_PIN;
	gpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM; //GPIO_Speed_50MHz;
	gpioInit.Mode = LL_GPIO_MODE_ALTERNATE; //GPIO_Mode_AF_PP;
	gpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInit.Pull = LL_GPIO_PULL_NO;
	gpioInit.Alternate = LL_GPIO_AF_0; // 0..15 ?
	LL_GPIO_Init(US_TRIG_PORT, &gpioInit);

	gpioInit.Pin = US_ECHO_PIN;
	gpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;//GPIO_Speed_50MHz;
	gpioInit.Mode = LL_GPIO_MODE_INPUT; // GPIO_Mode_IN_FLOATING;
	LL_GPIO_Init(US_ECHO_PORT, &gpioInit);
}

void InitHCSR04()
{
	EnableHCSR04PeriphClock();
	initPins();
	initMeasureTimer();
}

uint32_t HCSR04GetDistance()
{
	LL_TIM_SetCounter(US_TIMER, 0);
	LL_TIM_EnableCounter(US_TIMER);
	while(!LL_TIM_IsActiveFlag_UPDATE(US_TIMER));
	LL_TIM_DisableCounter(US_TIMER);
	LL_TIM_ClearFlag_UPDATE(US_TIMER);

	return (LL_TIM_IC_GetCaptureCH2(US_TIMER) - LL_TIM_IC_GetCaptureCH1(US_TIMER))*165U/1000U;
}

void EnableHCSR04PeriphClock()
 {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}