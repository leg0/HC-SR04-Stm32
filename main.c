#include <stm32f10x_HC-SR04.h>
#include <stm32l4xx_ll_gpio.h>
#include <stm32l4xx_ll_tim.h>
int main(void)
{
	LL_Init();

	HCSR04 hcsr04;
	hcsr04.Timer = TIM1;
	hcsr04.TriggerChannel = LL_TIM_CHANNEL_CH3;
	hcsr04.TriggerPort = GPIOA;
	hcsr04.TriggerPin = LL_GPIO_PIN_10;
	hcsr04.EchoChannel = LL_TIM_CHANNEL_CH1;
	hcsr04.EchoPort = GPIOA;
	hcsr04.EchoPin = LL_GPIO_PIN_8;
	InitHCSR04(&hcsr04);

	uint32_t dist = HCSR04GetDistance(&hcsr04);

	while (1);
}
