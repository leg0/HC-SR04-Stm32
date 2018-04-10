#include "stm32f10x_HC-SR04.h"
#include <stm32l4xx_hal_rcc.h>
#include <stm32l4xx_ll_rcc.h>
#include <stm32l4xx_ll_tim.h>
#include <stm32l4xx_ll_gpio.h>

static ErrorStatus initMeasureTimer(HCSR04 const* hcsr04)
{
	LL_RCC_ClocksTypeDef clocks;
	LL_RCC_GetSystemClocksFreq(&clocks);
	uint16_t const prescaler = clocks.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)

	if (LL_TIM_DeInit(hcsr04->Timer) == ERROR)
		return ERROR;

	LL_TIM_InitTypeDef timInit;
	LL_TIM_StructInit(&timInit);
	timInit.Prescaler = prescaler;
	timInit.Autoreload = 0xFFFF; // Period
	timInit.RepetitionCounter = 1;
	if (LL_TIM_Init(hcsr04->Timer, &timInit) == ERROR)
		return ERROR;

	LL_TIM_OC_InitTypeDef ocInit;
	LL_TIM_OC_StructInit(&ocInit);
	ocInit.OCMode = LL_TIM_OCMODE_PWM1;
	ocInit.CompareValue = 15; // Pulse
	ocInit.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	ocInit.OCState = LL_TIM_OCSTATE_ENABLE;
	if (LL_TIM_OC_Init(hcsr04->Timer, hcsr04->TriggerChannel, &ocInit) == ERROR)
		return ERROR;

	LL_TIM_IC_InitTypeDef icInit;
	LL_TIM_IC_StructInit(&icInit);
	icInit.ICPolarity = LL_TIM_IC_POLARITY_RISING;
	icInit.ICPrescaler = LL_TIM_ICPSC_DIV1;
	icInit.ICFilter = LL_TIM_IC_FILTER_FDIV1; // 0
	icInit.ICActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
	if (LL_TIM_IC_Init(hcsr04->Timer, hcsr04->EchoChannel, &icInit) == ERROR)
		return ERROR;

	LL_TIM_SetTriggerInput(hcsr04->Timer, US_TIMER_TRIG_SOURCE);
	LL_TIM_EnableMasterSlaveMode(hcsr04->Timer);

	MODIFY_REG(hcsr04->Timer->BDTR, 0, TIM_BDTR_MOE); //TIM_CtrlPWMOutputs(US_TIMER, ENABLE);

	LL_TIM_ClearFlag_UPDATE(hcsr04->Timer);
	return SUCCESS;
}

static ErrorStatus initPins(HCSR04 const* hcsr04)
{
	LL_GPIO_InitTypeDef gpioInit;
	LL_GPIO_StructInit(&gpioInit);
	gpioInit.Pin = hcsr04->TriggerPin;
	gpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInit.Alternate = LL_GPIO_AF_1;
	gpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInit.Pull = LL_GPIO_PULL_NO;
	gpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
	if (LL_GPIO_Init(hcsr04->TriggerPort, &gpioInit) == ERROR)
		return ERROR;

	LL_GPIO_StructInit(&gpioInit);
	gpioInit.Pin = hcsr04->EchoPin;
	gpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
	gpioInit.Alternate = LL_GPIO_AF_1;
	gpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpioInit.Pull = LL_GPIO_PULL_NO;
	gpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
	if (LL_GPIO_Init(hcsr04->EchoPort, &gpioInit) == ERROR)
		return ERROR;

	return SUCCESS;
}

bool InitHCSR04(HCSR04 const* hcsr04)
{
	// TODO: assert that the timer supports all the required features.

	EnableHCSR04PeriphClock(hcsr04);
	if (initPins(hcsr04) == ERROR)
		return false;
	if (initMeasureTimer(hcsr04) == ERROR)
		return false;
	return true;
}

uint32_t HCSR04GetDistance(HCSR04 const* hcsr04)
{
	TIM_TypeDef* const tim = hcsr04->Timer;

	LL_TIM_SetCounter(tim, 0);
	LL_TIM_EnableCounter(tim);
	while(!LL_TIM_IsActiveFlag_UPDATE(tim));
	LL_TIM_DisableCounter(tim);
	LL_TIM_ClearFlag_UPDATE(tim);

	return (LL_TIM_IC_GetCaptureCH2(tim) - LL_TIM_IC_GetCaptureCH1(tim))*165U/1000U;
}

static void EnableGpioClock(GPIO_TypeDef* gpio)
{
	#ifdef GPIOA
	if (gpio == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#ifdef GPIOB	
	if (gpio == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#ifdef GPIOC
	if (gpio == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#ifdef GPIOD
	if (gpio == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#ifdef GPIOE
	if (gpio == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#ifdef GPIOF
	if (gpio == GPIOF)
		__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#ifdef GPIOG
	if (gpio == GPIOG)
		__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
	#ifdef GPIOH
	if (gpio == GPIOH)
		__HAL_RCC_GPIOH_CLK_ENABLE();
	#endif
	#ifdef GPIOI
	if (gpio == GPIOI)
		__HAL_RCC_GPIOI_CLK_ENABLE();
	#endif
}

static void EnableTimerClock(TIM_TypeDef* tim)
{
	#ifdef TIM1
	if (tim == TIM1)
		__HAL_RCC_TIM1_CLK_ENABLE();
	#endif
	#ifdef TIM2
	if (tim == TIM2)
		__HAL_RCC_TIM2_CLK_ENABLE();
	#endif
	#ifdef TIM3
	if (tim == TIM3)
		__HAL_RCC_TIM3_CLK_ENABLE();
	#endif
	#ifdef TIM4
	if (tim == TIM4)
		__HAL_RCC_TIM4_CLK_ENABLE();
	#endif
	#ifdef TIM5
	if (tim == TIM5)
		__HAL_RCC_TIM5_CLK_ENABLE();
	#endif
	#ifdef TIM6
	if (tim == TIM6)
		__HAL_RCC_TIM6_CLK_ENABLE();
	#endif
	#ifdef TIM7
	if (tim == TIM7)
		__HAL_RCC_TIM7_CLK_ENABLE();
	#endif
	#ifdef TIM8
	if (tim == TIM8)
		__HAL_RCC_TIM8_CLK_ENABLE();
	#endif
	#ifdef TIM9
	if (tim == TIM9)
		__HAL_RCC_TIM9_CLK_ENABLE();
	#endif
	#ifdef TIM10
	if (tim == TIM10)
		__HAL_RCC_TIM10_CLK_ENABLE();
	#endif
	#ifdef TIM11
	if (tim == TIM11)
		__HAL_RCC_TIM11_CLK_ENABLE();
	#endif
	#ifdef TIM12
	if (tim == TIM12)
		__HAL_RCC_TIM12_CLK_ENABLE();
	#endif
	#ifdef TIM13
	if (tim == TIM13)
		__HAL_RCC_TIM13_CLK_ENABLE();
	#endif
	#ifdef TIM14
	if (tim == TIM14)
		__HAL_RCC_TIM14_CLK_ENABLE();
	#endif
	#ifdef TIM15
	if (tim == TIM15)
		__HAL_RCC_TIM15_CLK_ENABLE();
	#endif
	#ifdef TIM16
	if (tim == TIM16)
		__HAL_RCC_TIM16_CLK_ENABLE();
	#endif
}

void EnableHCSR04PeriphClock(HCSR04 const* hcsr04)
{
	EnableGpioClock(hcsr04->EchoPort);
	if (hcsr04->EchoPort != hcsr04->TriggerPort)
		EnableGpioClock(hcsr04->TriggerPort);

	EnableTimerClock(hcsr04->Timer);
}
