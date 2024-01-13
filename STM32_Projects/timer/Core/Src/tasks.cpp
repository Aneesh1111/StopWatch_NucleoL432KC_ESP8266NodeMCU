/*
 * tasks.c
 *
 *  Created on: 29 Jan 2021
 *      Author: harbanse
 */
#include <LED_flash.hpp>
#include <string>
#include "SSD_1306.hpp"
#include "SSD_1331.hpp"
#include "PWM_tone.hpp"
#include "timer.hpp"
#include "tasks.h"

osEventFlagsId_t pwm_evnt;

void LD3_task(void *pvParameters)
{
	LED_flash LD3(500,200, LD3_GPIO_Port, LD3_Pin);

	LD3.loop();
	while(1)
	{

	}
}
void LD4_task(void *pvParameters)
{
	LED_flash LD4(800,100, LD4_GPIO_Port, LD4_Pin);

	LD4.loop();
	while(1)
	{

	}
}

void PWM_task(void *pvParameters)
{
	PWM_tone bleep;
	bleep.loop();
}

#ifdef SUPERSEDED
void OLED_task(void *pvParameters)
{
	//96*64 OLED RGB module test
	SSD_1331 OledRGB;
	OledRGB.clear();
	uint8_t row=0, col=0, text='1';
	OledRGB.set_colour(0x1f, 0x2f, 0x1f);
	//OledRGB.draw_clock();
	//OledRGB.circle(96/2, 64/2 , 5);
	//OledRGB.write_char(0, 0, 'A');
//	for (;;)
//	{
//	OledRGB.test_display(0,0,'2');
//	OledRGB.display();
//	}

	for (;;)
	{
		OledRGB.countdown(pwm_evnt, LONG, SHORT);
		//OledRGB.min_sec(2345, 16);

	}

	OledRGB.set_colour(0x1f, 0, 0);
	text = 22;
	uint16_t time_elapsed = 2468;
	char show_time[] = "0000";
	for (;;)
	{
//		OledRGB.draw_line(0,0,95,63,255);
//		OledRGB.write_char(0x55, 0xAA);
		//OledRGB.set_colour(rand()%0x1f, rand()%0x2f, rand()%0x1f);

		uint16_t t = time_elapsed++;
		for (uint8_t i = 4; i > 0; i--)
		{
			uint8_t digit = t %10;
			digit += 22;	//LED digits
			show_time[i-1] = digit;
			t /= 10;
		}
		for (uint8_t i = 0; i <4; i++)
		{
			OledRGB.write_char_quad(i, 0, show_time[i]);
		}
		OledRGB.display();

		osDelay(20);
		OledRGB.clear();

//		row++;
//		text++;
//		if (text > 42)
//		{
//			text = 22;
//		}
//		if (row > 2){
//			col++;
//			if (col > 5)
//			{
//				col = 0;
//				OledRGB.display();
//				osDelay(2000);
//				OledRGB.clear();
//				//OledRGB.display();
//			}
//			row = 0;
//		}

	}
#ifdef OLED
// 128*32 oled module test
	SSD_1306 Poled;
	uint8_t n = 0;
	uint8_t p = '1';
	//TODO: the first cmd to display doesn't seem to do anything
	Poled.set_text_size(2);
	//Poled.display();

	uint16_t stop_watch = 3550;
	//msg isze is 5+1, 20+1 or 42+1
	char msg[21] = {0};
	enum brightness {high = 0x8f, low = 0x00};
	brightness contrast = high;
	bool colon_on = false;

	for (;;)
	{

		if (colon_on)
		{
			stop_watch++;
			snprintf(msg, sizeof(msg), " H  M  S  %02u:%02u:%02u", stop_watch/3600, (stop_watch/60)%60, stop_watch%60);
			colon_on = false;
		}
		else
		{
			snprintf(msg, sizeof(msg), " H  M  S  %02u %02u %02u", stop_watch/3600, (stop_watch/60)%60, stop_watch%60);
			colon_on = true;
		}

		Poled.write_str(0, msg);
		Poled.display();
		osDelay(500);

		//stop_watch++;
		//forever
	}
#endif

	while(1);
}

#endif

void Timer_task(void *pvParameters)
{
	SSD_1331 Oled;

	Timer countdown(Oled, 8, 0, 10, 24);
	Timer rest(Oled, 96/2, 8, 6, 24);

	countdown.show();
	rest.show();
	countdown.show();
	rest.show();
	for (;;)
	{
		rest.show();
		countdown.step();

		countdown.show();
		rest.step();
		//countdown.clear();
		//countdown.show();
	}

}
