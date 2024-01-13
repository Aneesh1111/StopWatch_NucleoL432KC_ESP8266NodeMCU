/*
 * SSD_1306.h
 *
 *  Created on: Jan 29, 2021
 *      Author: harbanse
 */

#ifndef INC_SSD_1306_HPP_
#define INC_SSD_1306_HPP_

#include "main.h"
#include <vector>
#include "cmsis_os2.h"
//#include "glcdfont.h"
#include "fiveBySevenFont.h"

extern SPI_HandleTypeDef hspi1;
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

class SSD_1306
{
private:
	uint8_t height, width, text_size=1;
public:
	SSD_1306(uint8_t h = 32, uint8_t w =128) : height(h), width(w)
	{
			buffer.resize(height*width/8);
			begin();
			//splash();
			display();
	}

	virtual void begin(uint8_t switchvcc = SSD1306_SWITCHCAPVCC);

	// These must be implemented in the derived transport driver
	virtual void command(uint8_t c);

	virtual void data(uint8_t c);

	/// Clear the display buffer
	void clearDisplay(void);

	void invertDisplay(bool i);

	/// Cause the display to be updated with the buffer content.
	virtual void display();

	void splash();

	void write_OLED(uint16_t pos, uint8_t pattern)
	{
		if (pos <512)
		{
			buffer[pos] = pattern;
		}
	}

	void write_char(uint8_t text_position, uint8_t c);

	void write_char_16(uint16_t text_position, uint8_t c);

	void write_char_32(uint16_t text_position, uint8_t c);

	void write_str(uint8_t text_position, char* str);
	void write_str_16(uint8_t text_position, char* str);

	void set_text_size(uint8_t new_size)
	{
		text_size = new_size;
	}

protected:
	virtual void sendDisplayBuffer();

	// the memory buffer for the LCD
	std::vector<uint8_t> buffer;
};


#endif /* INC_SSD_1306_HPP_ */
