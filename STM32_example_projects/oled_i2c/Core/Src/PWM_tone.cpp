/*
 * PWM_tone.cpp
 *
 *  Created on: 23 Feb 2021
 *      Author: harbanse
 */

#include "PWM_tone.hpp"

extern osEventFlagsId_t pwm_evnt;

void PWM_tone::loop()
{
	for (;;)
	{
		uint32_t evnt = osEventFlagsWait(pwm_evnt, SHORT | LONG, osFlagsWaitAny, osWaitForever);

		if (evnt == SHORT)
		{
			period = 200;
		}
		else
		{
			period = 500;
		}

		HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
		osDelay(period);
		HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
	}

}
