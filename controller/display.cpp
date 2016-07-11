#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>

#include "display.h"
#include "seven_segment_map.h"

struct display_pins_struct
{
	int ndigits;
	int data;
	int oe;
	int latch;
	int clk;
	int clr;
};
typedef struct display_pins_struct display_pins;

static const display_pins s_display_pins[2] = {
	{3, 12, 11, 10, 9, 8},
	{2, 7, 6, 5, 4, 3}
};

static SEVEN_SEGMENT_MAP s_segment_map = {
	4, 3, 0, 1, 2, 6, 7, 5
};

static uint8_t digits[10];

#ifndef ARDUINO
static void digitalWrite(int pin, int value);
static void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value);
#define LSBFIRST 0
#define MSBFIRST 1
#define LOW 0
#define HIGH 1
#endif

void display_setup()
{
	int i;

	for(i = 0; i <= 9; i++)
	{
		digits[i] = SSEG_CreateDigit(i, &s_segment_map, false);
	}

	for(i = 0; i < 2; i++)
	{
		pinMode(s_display_pins[i].data, OUTPUT);
		pinMode(s_display_pins[i].oe, OUTPUT);
		pinMode(s_display_pins[i].latch, OUTPUT);
		pinMode(s_display_pins[i].clk, OUTPUT);
		pinMode(s_display_pins[i].clr, OUTPUT);
		digitalWrite(s_display_pins[i].clr, HIGH);
	}
}

void display_update(int const * const values, int display_number)
{
	const display_pins& pins = s_display_pins[display_number];

	int i;
	digitalWrite(pins.latch, LOW);
	uint8_t segment_data;
	for (i = 0; i < 3; i++)
	{
		segment_data = digits[values[i]];

		if (pins.ndigits == 3 && i == 1) { SSEG_AddDecimal(&segment_data, &s_segment_map, true); }

		shiftOut(pins.data, pins.clk, LSBFIRST, segment_data);
	}
	digitalWrite(pins.latch, HIGH);
}

#ifndef ARDUINO

#include <iostream>
#include <bitset>

static void digitalWrite(int pin, int value) {(void)pin; (void)value;}
static void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t value)
{
	(void)dataPin; (void)clockPin; (void)bitOrder;
	std::bitset<8> x(value);
	std::cout << x << std::endl;
}

int main(int argc, char * argv[])
{
	(void)argc;
	(void)argv;

	int values[] = {9, 0, 0};

	display_setup();

	display_update(values, 0);
	display_update(values, 1);
}

#endif
