#pragma once

#include <stdbool.h>
#include <stdint.h>

//#define US_TIMER					TIM1

//#define US_TRIG_PORT				GPIOA
//#define US_TRIG_PIN					LL_GPIO_PIN_10		//TIM Ch3 (trig output)

//#define US_ECHO_PORT				GPIOA
//#define US_ECHO_PIN					LL_GPIO_PIN_8		//TIM Ch1 (echo input)
#define US_TIMER_TRIG_SOURCE		LL_TIM_TS_TI1FP1    //TIM_TS_TI1FP1

/**
 *	How to use this driver:
 * 	1. Implement EnableHCSR04PeriphClock function and turn on clock for used peripherals
 * 		ex:
 * 		void EnableHCSR04PeriphClock() {
 *			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 *			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 *			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 *		}
 *	2. Call InitHCSR04();
 *	3. Get distance (in mm) using HCSR04GetDistance function.
 *		Value lower than zero means no echo received (distance >3m)
 *		Measuring takes about 65ms
 *
 *	If necessary, change defines above, but be careful, US_ECHO_PIN must be tim ch1
 *	and US_TRIG_PIN must be ch3 or ch4. In case of ch4 change TIM_OC3Init into TIM_OC4Init in stm32f10x_HC-SR04.c file
 */

typedef struct HCSR04
{
    TIM_TypeDef* Timer;

    uint32_t TriggerChannel;
    GPIO_TypeDef* TriggerPort; // must correspond to the timer's channel
    uint32_t TriggerPin; // must correspond to the timer's channel

    uint32_t EchoChannel;
    GPIO_TypeDef* EchoPort; // must correspond to the timer's channel
    uint32_t EchoPin; // must correspond to the timer's channel
} HCSR04;

/**
 * Implement this function. See instruction at the top of this file.
 */
void EnableHCSR04PeriphClock(HCSR04 const* hcsr04);

/**
 * Initialization of HCSR04's peripherals
 */
bool InitHCSR04(HCSR04 const* hcsr04);

/**
 * Measure distance and get value in mm. Lover than 0 means no echo signal: distance more than ~3m.
 */
uint32_t HCSR04GetDistance(HCSR04 const* hcsr04);
