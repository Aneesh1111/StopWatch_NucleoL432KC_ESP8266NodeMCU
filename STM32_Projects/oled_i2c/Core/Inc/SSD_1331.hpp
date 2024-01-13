/*
 * SSD_1331.h
 *
 *  Created on: 4 Feb 2021
 *      Author: harbanse
 */

#ifndef INC_SSD_1331_HPP_
#define INC_SSD_1331_HPP_

#include "main.h"
#include <vector>
#include "cmsis_os2.h"
//#include "glcdfont.h"
#include "fiveBySevenFont.h"
//#include "SSD_1331.hpp"

extern SPI_HandleTypeDef hspi1;

enum Direction { horiz, vert};
struct Point
{
    int x=0, y=0;
};

class SSD_1331 //: public SSD_1306
{
private:
	uint8_t timer = 0;
	uint8_t height, width, text_size=1;
	uint16_t RGBcolour = 0xFFFF;
	uint8_t red = 0x1f, green = 0x2f, blue = 0x1f;

	//for 7 segment drawing
	uint8_t set_size;
	uint8_t top;
	uint8_t digit_gap;
	uint8_t left;

	bool set_clear_pixel_flag = true;

public:
	SSD_1331(uint8_t h = 64, uint8_t w = 96) : height(h), width(w)
	{
			buffer.resize(height*width*16/8);	//16 bits per pixel
			begin();
			//splash();
			display();
			set_seven_seg_size();
	}

	void set_or_clear(bool flag)
	{
		set_clear_pixel_flag = flag;
	}

	void set_seven_seg_size(uint8_t new_size = 48)
	{
		set_size = new_size;
		top = set_size/6/2+1;
		digit_gap = set_size*25/32;
		left = top; // 96 - 2*digit_gap;
	}

	void set_left(uint8_t new_left)
	{
		left = new_left;
	}

	void begin( );

	// These must be implemented in the derived transport driver
	void command(uint8_t c);

	void data(uint8_t c);

	void clear()
	{
		for (auto iter = buffer.begin(); iter != buffer.end(); ++iter)
		{
			*iter = 0;
		}
	}

	void set_pixel(uint8_t row, uint8_t col);
	void circle(int8_t poX, int8_t poY, int8_t r);


	//void invertDisplay(bool i);

	/// Cause the display to be updated with the buffer content.
	void display();

	void splash();

	void draw_line(Point start, int length, Direction d, int thickness);
	void draw_pixel_line(int x1, int y1, int x2, int y2);

	void write_char(uint8_t row, uint8_t col, uint8_t text);
	void write_char_double(uint8_t row, uint8_t col, uint8_t text);
	void write_char_quad(uint8_t row, uint8_t col, uint8_t text);

	void set_colour(uint8_t red, uint8_t green, uint8_t blue);

	void test_display(uint8_t row, uint8_t col, uint8_t text);

	void seven_seg(int x, int y, int n, int size7);
	void seg(uint8_t segments);
	void seven_segs( int n);

	void draw_colon(bool flag);
	void countdown(const osEventFlagsId_t pwm_evnt, const uint32_t LONG, const uint32_t SHORT);
	void min_sec(uint16_t counter, uint8_t set_size);

	void beep();

protected:
	void sendDisplayBuffer();

	// the memory buffer for the LCD
	std::vector<uint8_t> buffer;
};




#endif /* INC_SSD_1331_HPP_ */
