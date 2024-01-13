/*
 * tasks.c
 *
 *  Created on: 29 Jan 2021
 *      Author: harbanse
 */

#include "LCD_flash.hpp"
#include "tasks.h"

void start_task(void *pvParameters)
{
	LED_flash LD3(800,200);

	LD3.loop();
	while(1)
	{

	}
}
