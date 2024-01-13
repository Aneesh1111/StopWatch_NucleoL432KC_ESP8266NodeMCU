/*
 * timer.cpp
 *
 *  Created on: 25 Feb 2021
 *      Author: harbanse
 */

#include "timer.hpp"
#include "SSD_1331.hpp"

extern osEventFlagsId_t pwm_evnt;

void Timer::loop()
{
	Oled.set_colour(0, 0x2f, 0);
	Oled.set_seven_seg_size(24);
	for (;;)
	{
		//countdown();
		dual_count();
	}
}

void Timer::countdown()
{
	Oled.set_colour(0, 0x2f, 0);

	for (int8_t r = 10; r>-1; r--)
	{
		tick_colon_on = osKernelGetTickCount() + 500;
		tick_colon_off = tick_colon_on + 500;
		if (r < 5)
		{
			Oled.set_colour(0x1f, 0, 0);
		}
		else
		{
			Oled.set_colour(0, 0x2f, 0);
		}	//OledRGB.set_colour(rand()%0x1f, rand()%0x2f, rand()%0x1f);

		//beep for 0.25 sec when count is < 5

		Oled.clear();

		//flash colon every half second
		Oled.draw_colon(true);
		Oled.seven_segs((r/10)%10);
		Oled.seven_segs(r%10);
		Oled.display();
		osDelayUntil(tick_colon_on);

		if (r<5)
		{
			if (r<1)
			{
				osEventFlagsSet(pwm_evnt, LONG);
			}
			else
			{
				osEventFlagsSet(pwm_evnt, SHORT);
			}
		}
		Oled.clear();
		Oled.draw_colon(false);
		Oled.seven_segs((r/10)%10);
		Oled.seven_segs(r%10);
		Oled.display();
		osDelayUntil(tick_colon_off);
	}
}

void Timer::dual_count()
{
	Oled.set_colour(0, 0x2f, 0);

	for (time_sec i = 40; i>=0; i--)
	{
		tick_colon_on = osKernelGetTickCount() + 1000;
		Oled.clear();
		Oled.set_left(8);
		Oled.seven_segs((i/10)%10);
		Oled.seven_segs(i%10);
		Oled.display();
		osDelayUntil(tick_colon_on);
	}
}

//run the timer down to zero
void Timer::step()
{
	Oled.set_seven_seg_size(seg_size);

	while (current_time >=0)
	{
		tick_colon_on = osKernelGetTickCount() + 1000;
		//Oled.clear();
		Oled.set_left(position.x);
		//Oled.set_or_clear(true);
		Oled.set_colour(0x1f, 0x00, 0x00);

		Oled.seven_segs((current_time/10)%10);
		Oled.seven_segs(current_time%10);
		Oled.display();
		osDelayUntil(tick_colon_on);

		//erase only if value > 0
		if (current_time > 0)
		{
			Oled.set_colour(0x03, 0x00, 0x00);
			Oled.set_left(position.x);
			//Oled.set_or_clear(false);
			Oled.seven_segs((current_time/10)%10);
			Oled.seven_segs(current_time%10);
		}
		else
		{
			Oled.set_colour(0x01, 0x00, 0x00);
			Oled.set_left(position.x);
			//Oled.set_or_clear(false);
			Oled.seven_segs(8);
			Oled.seven_segs(8);
		}

		current_time--;
	}
	if (current_time < 0)
	{
		current_time = start_time;
	}
}

void Timer::show()
{
	Oled.set_colour(0x0f, 0x00, 0x00);
	Oled.set_seven_seg_size(seg_size);
	Oled.set_left(position.x);
	Oled.seven_segs((current_time/10)%10);
	Oled.seven_segs(current_time%10);
	Oled.display();
}

void Timer::clear()
{
	Oled.clear();
	Oled.display();
}
