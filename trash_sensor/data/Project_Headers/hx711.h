/*
 * hx711.h
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

#ifndef __HX711_H_
#define __HX711_H_

#include <types.h>
#include <xbee_config.h>

#define SCALE_FACTOR	-101900.0f
#define GAIN 	       	1
#define PD_SCK 	       	XPIN_7
#define DOUT 	       	XPIN_4

enum logic_level {
	LOW = 0,
	HIGH = 1
};

void hx_init(void);
void hx_power_down(void);
uint8_t hx_is_ready(void);
static uint8_t shift_in(xpin_t data_pin, xpin_t clock_pin);
uint32_t hx_read(void);
int32_t hx_read_average(uint8_t times);
float hx_offset_average(uint8_t times);
float hx_scaled_average(uint8_t times);
void hx_tare(void);
int32_t hx_get_offset(void);

#endif /* __HX711_H_ */
