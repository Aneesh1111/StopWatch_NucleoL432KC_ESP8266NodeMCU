/*
 * Tasks.c
 *
 *  Created on: Jan 22, 2021
 *      Author: harbanse
 */

#include "main.h"
#include "cmsis_os2.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "tasks.h"

#define lcd_buffer_size 33
char LCD_line[lcd_buffer_size] = {0};
uint8_t lcd_position = 0;

extern UART_HandleTypeDef huart2;

void data_bus_write(uint8_t data);
void set_E(_Bool f);
void set_RS(_Bool f);
void LCD_send_now(uint8_t inst);
void LCD_send_bits(uint8_t value);
void set_position(uint8_t pos);
void write_text(uint8_t pos, char *str);
void LCD_init(void);
uint8_t ascii_hex(uint8_t a);
void LCD_clear(void);
void LCD_add(char usb_char);


//LED Flash task
void LED_flash(void *pvParameters)
{
	const uint32_t LED_period1 = 900;
	const uint32_t LED_period2 = 100;

	_Bool LED_out = 0;
	for(;;)
	{
		LED_out = LED_out ^ 1;

		if (LED_out==0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			osDelay(LED_period1);
		}
		else
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			osDelay(LED_period2);
		}

	}
}

//PA4-7 are the data bits to be written
void data_bus_write(uint8_t data)
{
	uint16_t lcd_data = 0;
	lcd_data = data << 4;
	uint16_t tmp = GPIOA->IDR;
	tmp = tmp & 0xFF0F;
	tmp |= lcd_data;
	GPIOA->ODR = tmp;
}

void set_E(_Bool f)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, f);
}

void set_RS(_Bool f)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, f);
}

//initialise the LCD before setting 4 bit interface
void LCD_send_now(uint8_t inst)
{
    //minimum lcd_wait 5 ms
	uint16_t lcd_wait = 5;
	data_bus_write(inst);
    set_E(1);
    osDelay(lcd_wait);
    set_E(0);

    osDelay(lcd_wait);
}

//send upper 4 bits first
void LCD_send_bits(uint8_t value)
{
    //minimum wait_time = 0.00001;
    uint16_t wait_time = 1;	//1 ms, could be less

    data_bus_write(value >> 4);
    set_E(1);
    osDelay(wait_time);
    set_E(0);
    osDelay(wait_time);

    data_bus_write(value & 0x0F);
    set_E(1);
    osDelay(wait_time);
    set_E(0);
    osDelay(wait_time);
}


void set_position(uint8_t pos)
{
    set_RS(0);
    LCD_send_bits(0x80 | pos);
    set_RS(1);
}

//zero marks the end of the string
//wrap message to second line if necessary
//line 1: 0x00-0x0f, line 2: 0x40-0x4f
void write_text(uint8_t pos, char *str)
{
    uint8_t i, j=0;

    set_position(pos);
    set_RS(1);
    i = str[j];
    while (i)
    {
        LCD_send_bits(i);
        pos++;
        if (pos < 0x10)
        {
        	//nothing to do
        }
        else if (pos < 0x40)
        {
        	pos = 0x40;
        	set_position(pos);
        }
        else if (pos < 0x50)
        {
        	//nothing to do
        }
        else
        {
        	pos = 0x00;
        	set_position(pos);
        }

        j++;
        i = str[j];
    }
}

void LCD_init(void)
{
    //minimum instruction_time = 40 us
    uint16_t instruction_time = 1 ;	//could be less
    data_bus_write(0);
    set_E(0);
    set_RS(0);

    //initialise 4 bit data
    LCD_send_now(0x03);
    LCD_send_now(0x03);
    LCD_send_now(0x03);
    LCD_send_now(0x02);
    //now in 4 bit mode
    LCD_send_bits(0x20);
    osDelay(instruction_time);
    LCD_send_bits(0x28);    // 2 lines, 5x8
    osDelay(instruction_time);
    LCD_send_bits(0x08);    // display off
    osDelay(instruction_time);
    LCD_send_bits(0x01);    // display clear
    osDelay(instruction_time);
    LCD_send_bits(0x06);    // entry mode?
    osDelay(instruction_time);
    LCD_send_bits(0x02);    // return cursor to home
    osDelay(instruction_time);
    LCD_send_bits(0x0C);    // display on, cursor on
    osDelay(instruction_time);
}

//helps to convert a byte to hex for display debugging
uint8_t ascii_hex(uint8_t a)
{
	uint8_t half = a & 0x0F;
	if (half > 9)
	{
		half += 0x37;
	}
	else
	{
		half += 0x30;
	}
	return half;
}

//File the LCD buffer with spaces and a final null terminator
void LCD_clear(void)
{
	uint8_t i;
	for (i=0; i < (lcd_buffer_size-1); i++)
	{
		LCD_line[i] = ' ';
	}
	LCD_line[i] = 0;
	lcd_position = 0;
}

//add a character to the LCD buffer, if it is printable
void LCD_add(char usb_char)
{
	if (lcd_position == (lcd_buffer_size-1) )
	{
		//no more space,
	}
	else
	{
		LCD_line[lcd_position] = usb_char;
		lcd_position++;
	}
}

//USB to LCD text task
void LCD_text(void *pvParameters)
{
	LCD_init();
	//any direct writes of data to the LCD (LCD_sendbits)
	//will need to be preceded by set_RS(1)
	//the function write_text does this automatically

    //try a write
    {
		char greeting[] = "USB serial text";
        write_text(0, greeting);

    }

    for (;;)
    {

    	//USB data, write to LCD when CR receive, drop all non-printing chars
    	uint8_t num_usb_bytes = usb_check_buffer();
    	if (num_usb_bytes)
    	{
    		while (num_usb_bytes)
    		{
				char usb_data = usb_get_byte();
				if (usb_data == 13)	//CR
				{
					write_text(0, LCD_line);
					LCD_clear();
				}
				if (usb_data >= ' ' )
				{
					LCD_add(usb_data);
				}
				num_usb_bytes--;
    		}
    	}

    }

}

