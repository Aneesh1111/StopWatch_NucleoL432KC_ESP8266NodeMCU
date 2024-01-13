/*
 * SSD_1331.cpp
 *
 *  Created on: 4 Feb 2021
 *      Author: harbanse
 */

#include <math.h>
#include <bitset>
#include <SSD_1331.hpp>

#define OUT_PIN(name, state) HAL_GPIO_WritePin(name##_GPIO_Port, name##_Pin, GPIO_PIN_##state)

const uint8_t unlock = 0xFD;
const uint8_t display_off = 0xAE;
const uint8_t remap = 0xA0;
const uint8_t display_start = 0xA1;
const uint8_t display_offset = 0xA2;
const uint8_t display_normal = 0xA4;
const uint8_t set_mux = 0xA8;
const uint8_t set_master_config = 0xAD;
const uint8_t power_save_mode = 0xB0;
const uint8_t set_phase_length = 0xB1;
const uint8_t display_clk_ratio = 0xB3;
const uint8_t precharge_A  = 0x8A;
const uint8_t precharge_B  = 0x8B;
const uint8_t precharge_C  = 0x8C;
const uint8_t precharge_all  = 0xBB;
const uint8_t vcomH  = 0xBE;
const uint8_t attenuation_factor  = 0x87;
const uint8_t contrast_A = 0x81;
const uint8_t contrast_B = 0x82;
const uint8_t contrast_C = 0x83;
const uint8_t scroll_off = 0x2E;
const uint8_t clear = 0x25;
const uint8_t display_on = 0xAF;

void SSD_1331::begin()
{
	//power on sequence from Digilent
	OUT_PIN(DC, RESET);
	OUT_PIN(RST, SET);
	OUT_PIN(VCCEN, RESET);
	OUT_PIN(PMODEN, SET);
	osDelay(200);
	OUT_PIN(VCCEN, SET);
	OUT_PIN(RST, SET);
	osDelay(100);		//only takes 3 us
	//enable the driver IC to accept commands
//	command(unlock);
//	command(0x12);

	command(display_off);

	command(remap);
	command(0x72);

	command(display_start);
	command(0x00);

	command(display_offset);
	command(0x00);

	command(display_normal);

	command(set_mux);
	command(0x3F);

	command(set_master_config);
	command(0x8E);			//TODO: check, external Vcc?

	command(power_save_mode);
	command(0x0B);

	command(set_phase_length);
	command(0x31);

	command(display_clk_ratio);
	command(0xF0);

	command(precharge_A);
	command(0x64);
	command(precharge_B);
	command(0x78);
	command(precharge_C);
	command(0x64);
	command(precharge_all);
	command(0x3A);

	command(vcomH);
	command(0x3E);

	command(attenuation_factor);
	command(0x06);

	command(contrast_A);
	command(0x91);
	command(contrast_B);
	command(0x50);
	command(contrast_C);
	command(0x7D);

	OUT_PIN(VCCEN, SET);
	osDelay(200);

	command(display_on);
	osDelay(200);
}

void SSD_1331::command(uint8_t c)
{

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);	//dc = 0;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);	//cs = 0;
	//mspi.write(c);
	HAL_SPI_Transmit(&hspi1, &c, 1, 0);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;
}

void SSD_1331::data(uint8_t c)
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);	//dc = 1;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);	//cs = 0;
	//mspi.write(c);
	HAL_SPI_Transmit(&hspi1, &c, 1, 0);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;
}

//set or clear the pixel in the given position
void SSD_1331::set_pixel(uint8_t row, uint8_t col)
{
	uint16_t position = row*2 + col*width*2;

	if (set_clear_pixel_flag)
	{
		if (position < width*height*2)
		{
			buffer[position] = RGBcolour & 0xFF;
			buffer[position + 1] = (RGBcolour >> 8);
		}
	}
	else
	{
		if (position < width*height*2)
				{
					buffer[position] = 0;
					buffer[position + 1] = 0;
				}
	}

}

void SSD_1331::circle(int8_t poX, int8_t poY, int8_t r)
{
    int x = -r, y = 0, err = 2-2*r, e2;
    do
    {
    	set_pixel(poX-x, poY+y);
        set_pixel(poX+x, poY+y);
        set_pixel(poX+x, poY-y);
        set_pixel(poX-x, poY-y);
        e2 = err;
        if(e2 <= y)
        {
            err += ++y*2+1;
            if(-x == y && e2 <= x) e2 = 0;
        }
        if(e2 > x) err += ++x*2+1;
    } while(x <= 0);
}

// Send the display buffer out to the display
void SSD_1331::display(void)
{
	command(display_start);  // low col = 0
	command(0x00);
	command(display_offset);
	command(0x00);

	command(display_normal);

	sendDisplayBuffer();
}

//only draw horizontal or vertical lines
void SSD_1331::draw_pixel_line(int x1, int y1, int x2, int y2)
{
    int i=0;
    if (y1==y2)
    {
        for (i=x1; i<x2; i++)
        {
            set_pixel( i, y1);
        }
    }
    else
    {
        for (i = y1; i<y2; i++)
        {
            set_pixel(x1, i);
        }
    }
}

void SSD_1331::draw_line(Point start, int length, Direction d, int thickness)
{
    Point end_line;
    if (d==horiz)
    {
        end_line.x = start.x + length;
        start.y -= thickness/2;
        end_line.y = start.y;
        for ( int i = 0; i < thickness; i++)
        {
            draw_pixel_line(start.x, start.y +i, end_line.x, end_line.y+i);
        }
    }
    else
    {
        start.x -= thickness/2;
        end_line.x = start.x;
        end_line.y = start.y + length;
        for ( int i = 0; i < thickness; i++)
        {
            draw_pixel_line(start.x+i, start.y, end_line.x+i, end_line.y);
        }
    }
}

void SSD_1331::write_char(uint8_t row, uint8_t col, uint8_t text)
{
	//write text character at row, col
	uint16_t target_addr = row*6*2 + col*8*width*2;
	uint8_t char_code = text;
	for (uint8_t i=0; i<5; i++)
	{
		uint8_t c = font[char_code*5 + i];
		uint16_t t = target_addr;
		for (uint8_t j=0; j<8; j++)
		{
			if (c & 1)
			{
				buffer[t] = RGBcolour & 0xFF;
				buffer[t+1] = (RGBcolour >> 8);
			}
			c >>=1;
			t += width*2;
		}
		target_addr += 2;
	}
}

void SSD_1331::write_char_double(uint8_t row, uint8_t col, uint8_t text)
{
	//write text character at row, col
	uint16_t target_addr = row*6*2*2 + col*8*width*2*2;
	uint8_t char_code = text;
	for (uint8_t i=0; i<5; i++)
	{
		uint8_t c = font[char_code*5 + i];
		uint16_t t = target_addr;
		for (uint8_t j=0; j<8; j++)
		{
			if (c & 1)
			{
				for (uint8_t k=0; k<4; k++)
				{
					if (k%2 == 0)
					{
						buffer[t+k] = RGBcolour & 0xFF;
						buffer[t + width*2 + k] = RGBcolour & 0xFF;

					}
					else
					{
						buffer[t+k] = (RGBcolour >> 8);
						buffer[t + width*2 + k] = (RGBcolour >> 8);

					}
				}

			}
			c >>=1;
			t += width*2*2;
		}
		target_addr += 2*2;
	}
}

void SSD_1331::write_char_quad(uint8_t row, uint8_t col, uint8_t text)
{
	//write text character at row, col
	uint16_t target_addr = row*6*2*2*2 + col*8*width*2*2*2;
	uint8_t char_code = text;
	for (uint8_t i=0; i<5; i++)
	{
		uint8_t c = font[char_code*5 + i];
		uint16_t t = target_addr;
		for (uint8_t j=0; j<8; j++)
		{
			if (c & 1)
			{
				for (uint8_t k=0; k<8; k++)
				{
					if (k%2 == 0)
					{
						buffer[t+k] = RGBcolour & 0xFF;
						buffer[t + width*2 +k] = RGBcolour & 0xFF;
						buffer[t + width*4 +k] = RGBcolour & 0xFF;
						buffer[t + width*6 +k] = RGBcolour & 0xFF;

					}
					else
					{
						buffer[t+k] = (RGBcolour >> 8);
						buffer[t + width*2 + k] = (RGBcolour >> 8);
						buffer[t + width*4 + k] = (RGBcolour >> 8);
						buffer[t + width*6 + k] = (RGBcolour >> 8);

					}
				}

			}
			c >>=1;
			t += width*2*2*2;
		}
		target_addr += 2*2*2;
	}
}

void SSD_1331::set_colour(uint8_t r, uint8_t g, uint8_t b)
{
	red = r % 0x20;
	green = g %  0x30;
	blue = b % 0x20;
	RGBcolour = red << 3;
	RGBcolour |= (blue << 8);
	//lsbs of RBBcolur green are the brighter ones!
	RGBcolour |= ((green & 0x07) << 14);
	RGBcolour |= ( (green & 0xE0) >> 3);
}


void SSD_1331::sendDisplayBuffer()
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);	//dc = 1;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);	//cs = 0;

	uint16_t q = buffer.size();
	//uint8_t item;

	uint8_t *buff_ptr = &buffer.front();
	HAL_SPI_Transmit(&hspi1, buff_ptr, q, 100);	//increased timeout to allow data send

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //cs = 1;

}

void SSD_1331::test_display(uint8_t row, uint8_t col, uint8_t text)
{

#define CHARS
#ifdef CHARS
	//write chars
	uint16_t target_addr = row*6*2 + col*8*width*2;
	uint8_t char_code = text;
	for (uint8_t i=0; i<5; i++)
	{
		uint8_t c = font[char_code*5+i];
		uint16_t t = target_addr;
		for (uint8_t j=0; j<8; j++)
		{
			if (c & 1)
			{
				buffer[t] = RGBcolour & 0xFF;
				buffer[t+1] = RGBcolour >> 8;
			}
			c >>=1;
			t+=width*2;
		}
		target_addr += 2;
	}
#endif

#ifdef DOTS_AND_COLOURS
	for (uint16_t i = 0; i < 10; i+=2)
	{
		buffer[i] = RGBcolour & 0xFF;
		buffer[i+1] = RGBcolour >> 8;

//		uint16_t colour = rand()%0xFFF;
//		uint16_t addr = rand()%6144;
//		buffer[addr*2] = colour & 0xFF;
//		buffer[addr*2+1] = (colour >>8) &0xFF;

	}
#endif

#ifdef DOTTED_LINE
	//colour msb/lsb: GGGBBBBB RRRRRGGG
	static uint16_t patt = 0xFFFF;
	for (uint16_t i = 0; i< (width*2); i+=4)
	{
		buffer[i] = patt & 0xff;
		buffer[i+1] = (patt>>8) & 0xff;
		//patt = (patt==0xAA) ? 0x55: 0xAA;
	}
	//patt <<= 1;
#endif

#ifdef DRAW_TEST
	//static uint8_t y2
	//draw rectangle
	command(0x22);
	command(2);
	command(3);
	command(95);
	command(63);

	//colour
	command(0xff);
	command(0xff);
	command(0x00);
	command(0);
	command(0);
	command(0);
#endif

#ifdef FILL_TEST
	static uint8_t patt = 0x55;
	for (uint16_t i=0; i<width*height*2; i++)
	{
		buffer[i] = patt;
	}

	patt = (patt==0x55) ? 0xAA : 0x55;
#endif

}


//convert binary to 7 seg a,b,c,d,e,f,g
void SSD_1331::seg(uint8_t segments)
{
	int thickness = set_size/6;
	int x1 = left, x2 = left + set_size/2;
	int y1 = top, y2 = top + set_size/2, y3 = top + set_size;
	int seg_array[][4] = {
			{x1, y1, x2, y1},
			{x2, y1, x2, y2},
			{x2, y2, x2, y3},
			{x1, y3, x2, y3},
			{x1, y2, x1, y3},
			{x1, y1, x1, y2},
			{x1, y2, x2, y2}
	};

	for (uint8_t i=0; i<8; i++)
	{
		if (segments & (1 << i))
		{
            Point s = {seg_array[i][0], seg_array[i][1]};
            Direction dir = vert;
            switch (i)
            {
                case 0:
                case 3:
                case 6:
                    dir = horiz;
                    break;
                default:
                    break;
            }
            draw_line(s, set_size/2, dir, thickness);
		}
	}
	left += digit_gap;
	if (left > width-digit_gap)
	{
		left = top;	//width - 2*digit_gap;
	}

}
void SSD_1331::seven_segs(int n)
{
	switch (n)
	{
	case 0:
		seg( 0b0111111);
		break;
	case 1:
		seg( 0b0000110);
		break;
	case 2:
		seg( 0b1011011);
		break;
	case 3:
		seg( 0b1001111);
		break;
	case 4:
		seg( 0b1100110);
		break;
	case 5:
		seg( 0b1101101);
		break;
	case 6:
		seg( 0b1111101);
		break;
	case 7:
		seg( 0b0000111);
		break;
	case 8:
		seg( 0b1111111);
		break;
	case 9:
		seg( 0b1101111);
		break;
	default:
		seg( 0b1000000);
		break;
	}
}

//colon always on the left, resets positioning
void SSD_1331::draw_colon(bool flag)
{
	//uint8_t left = width - set_size*64/32;
	left = 0;
	if (flag)
	{
		Point s = {left, set_size/3};
		Point t = {s.x, s.y + set_size/2};
		draw_line(s, set_size/4,  horiz, set_size/4 );
		draw_line(t, set_size/4,  horiz, set_size/4 );
	}
	//specoal spaing for colon
	left += set_size*21/48;
}

void SSD_1331::countdown(const osEventFlagsId_t pwm_evnt, const uint32_t LONG, const uint32_t SHORT)
{
	uint32_t tick_colon_on, tick_colon_off;
	set_seven_seg_size(48);
	set_colour(0, 0x2f, 0);

	for (int8_t r = 10; r>-1; r--)
	{
		tick_colon_on = osKernelGetTickCount() + 500;
		tick_colon_off = tick_colon_on + 500;
		if (r < 5)
		{
			set_colour(0x1f, 0, 0);
		}
		else
		{
			set_colour(0, 0x2f, 0);
		}

		clear();
		//flash colon every half second
		draw_colon(true);

		//set_left(set_size*21/48);
		seven_segs((r/10)%10);
		seven_segs(r%10);
		display();
		osDelayUntil(tick_colon_on);

		//beep for 0.25 sec when count is < 5
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
		clear();
		draw_colon(false);
		//set_left(set_size*21/48);
		seven_segs((r/10)%10);
		seven_segs(r%10);
		display();
		osDelayUntil(tick_colon_off);
	}
}

void SSD_1331::min_sec(uint16_t counter, uint8_t set_size)
{
	set_seven_seg_size(set_size);
//	uint8_t set_size = 16;
//	uint8_t top = set_size/6/2+1;
//	uint8_t digit_gap = set_size*25/32;
//	uint8_t left = width;

	uint32_t tick;

	set_colour(0, 0x2f, 0);

	//uint16_t counter = 1234;
	uint16_t r;
	for (;;)
	{
		tick = osKernelGetTickCount() + 100;
		clear();

		r = counter;
		for (uint8_t i = 0; i < 5; i++)
		{
			uint8_t digit = r %10;
			r /= 10;
			//seven_segs(left - (i+1)*digit_gap, top, digit, set_size);
			seven_segs(digit);
		}

//		seven_segs(left + digit_gap, top, r%10, set_size);
//		seven_segs(left, top, (r/10)%10, set_size);
		counter++;
		display();
		//reset position
		set_seven_seg_size(set_size);

		osDelayUntil(tick);
	}

}


