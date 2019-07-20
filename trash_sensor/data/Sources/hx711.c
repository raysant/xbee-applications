/*
 * hx711.c
 *
 * Created on: February 24, 2018
 * 	   Author: Ray Santana
 *
 * This is a modified version of the HX711 library made by 
 * user bogde on GitHub.	https://github.com/bogde/HX711
 *	
 * MIT License
 *
 * Copyright (c) 2018 Bogdan Necula
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <types.h>
#include <xbee_config.h>
#include <hx711.h>

static int32_t weight_offset = 0;

void hx_init(void)
{
	/* Power up HX711 sensor. The first couple of measurements might be wrong
	 * so fetch them and do nothing. */
	uint32_t val;
	
	gpio_set(PD_SCK, LOW);
	val = hx_read_average(2);
}

void hx_power_down(void)
{
	gpio_set(PD_SCK, LOW);
	gpio_set(PD_SCK, HIGH);
}

uint8_t hx_is_ready(void)
{
	return !gpio_get(DOUT);
}

static uint8_t shift_in(xpin_t data_pin, xpin_t clock_pin)
{
	uint8_t val = 0;
	uint8_t i;
	
	/* Shift in one byte from the HX711 with the MSB first. */
	for (i = 8; i != 0; i--) {
		gpio_set(clock_pin, HIGH);
		val |= gpio_get(data_pin) << (i - 1);
		gpio_set(clock_pin, LOW);
	}
	
	return val;
}

uint32_t hx_read(void)
{
	uint32_t val = 0;
	
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;
	uint8_t i;
	
	uint8_t ready = 0;
	while (!ready)
	{
		ready = hx_is_ready();
		sys_watchdog_reset();
	}
	
	/* Pulse clock pin 24 times to retrieve data */
	data[2] = shift_in(DOUT, PD_SCK);
	data[1] = shift_in(DOUT, PD_SCK);
	data[0] = shift_in(DOUT, PD_SCK);
	
	/* Pulse clock pin after retrieving data in order to
	 * set amplifier gain. */
	for (i = GAIN; i != 0; i--)
	{
		gpio_set(PD_SCK, HIGH);
		gpio_set(PD_SCK, LOW);
	}
	
	/* Padding 32-bit integer by repeating MSB. */
	if (data[2] & 0x80)
	{
		filler = 0xFF;	
	} else {
		filler = 0x00;
	}
	
	/* Construct 32-bit unsigned integer. */
	val = ( ((uint32_t) filler << 24)
		  | ((uint32_t) data[2] << 16)
		  | ((uint32_t) data[1] << 8)
		  | ((uint32_t) data[0]) );
	
	sys_watchdog_reset();
	return val;
}

int32_t hx_read_average(uint8_t times)
{
	/* Average several readings from HX711 to minimize error. */
	int32_t sum = 0;
	uint8_t i;
	
	for (i = times; i != 0; i--)
	{
		sum += hx_read();
	}
	
	return (sum / times);
}

float hx_offset_average(uint8_t times)
{
	/* Get average and apply offset. */
	return (float)(hx_read_average(times) - weight_offset);
}

float hx_scaled_average(uint8_t times)
{
	/* Use scaling factor to convert to pounds. */
	return (hx_offset_average(times) / SCALE_FACTOR);
}

void hx_tare(void)
{
	/* Calculate and set weight offset */
	int32_t val = hx_read_average(10);
	weight_offset = val;
}

int32_t hx_get_offset(void)
{
	return weight_offset;
}
