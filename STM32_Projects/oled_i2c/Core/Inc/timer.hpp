/*
 * timer.hpp
 *
 *  Created on: 24 Feb 2021
 *      Author: harbanse
 */

#ifndef INC_TIMER_HPP_
#define INC_TIMER_HPP_

#include "SSD_1331.hpp"
#include "event_flags.h"


typedef int8_t time_sec;	//countdown in seconds up to 127 secconds

class Timer
{
private:
	uint32_t tick_colon_on, tick_colon_off;
	Point position;
	time_sec start_time, current_time;
	SSD_1331& Oled;
	uint8_t seg_size;

public:
	//set position, size and colour?
	Timer(SSD_1331& ref_oled, uint8_t x, uint8_t y, int8_t set_time, uint8_t set_size) : Oled(ref_oled)
	{
		position.x = x;
		position.y = y;
		start_time = set_time;
		current_time = set_time;
		seg_size = set_size;
	}

	void loop();

	void countdown();

	void dual_count();

	void step();

	void show();

	void clear();

};



#endif /* INC_TIMER_HPP_ */
