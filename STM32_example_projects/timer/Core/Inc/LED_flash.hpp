/*
 * LCD_flash.hpp
 *
 *  Created on: 29 Jan 2021
 *      Author: harbanse
 */

#ifndef SRC_LCD_FLASH_HPP_
#define SRC_LCD_FLASH_HPP_

#include "main.h"
#include "cmsis_os2.h"

class LED_flash
{
	uint32_t LED_period1 = 500;
	uint32_t LED_period2 = 500;
	GPIO_TypeDef* LED_port;
	uint16_t LED_pin;
	bool LED_out = 0;


public:
	LED_flash(uint32_t x, uint32_t y, GPIO_TypeDef* p, uint16_t q) :
		LED_period1(x), LED_period2(y),
		LED_port(p), LED_pin(q) { }

	void loop()
	{
		for(;;)
		{
			LED_out = LED_out ^ 1;

			if (LED_out==0)
			{
				HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_RESET);
				osDelay(LED_period1);
			}
			else
			{
				HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_SET);
				osDelay(LED_period2);
			}

		}
	}
};



#endif /* SRC_LCD_FLASH_HPP_ */
