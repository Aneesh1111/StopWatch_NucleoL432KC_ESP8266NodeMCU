/*
 * PWM_tone.hpp
 *
 *  Created on: 23 Feb 2021
 *      Author: harbanse
 */

#ifndef INC_PWM_TONE_HPP_
#define INC_PWM_TONE_HPP_

#define SHORT 0x0001U
#define LONG 0x0002U

#include "main.h"
#include "cmsis_os2.h"
#include "event_flags.h"

extern TIM_HandleTypeDef htim16;

class PWM_tone
{
	uint16_t period;


public:

	void loop();


};



#endif /* INC_PWM_TONE_HPP_ */
