#include "counter.h"

void reset_counter(int & counter, int *counter_values)
{
	counter = 900;
	counter_values[2] = 9;
	counter_values[1] = 0;
	counter_values[0] = 0;
}

void countdown(int& counter, int *counter_values)
{
	counter = (counter > 0) ? counter - 1 : 0;

	int value = counter;

	counter_values[2] = value / 100;
	value -= (counter_values[2] * 100);

	counter_values[1] = value / 10;
	value -= (counter_values[1] * 10);

	counter_values[0] = value;
}

#ifndef ARDUINO

#include <iostream>

static int s_counter;
static int s_counter_values[3];

int main(int argc, char * argv[])
{
	(void)argc;
	(void)argv;

	reset_counter(s_counter, s_counter_values);

	while(s_counter > 0)
	{
		std::cout << s_counter << " ("<< s_counter_values[2] << ", " << s_counter_values[1] << ", " << s_counter_values[0] << ")" << std::endl;
		countdown(s_counter, s_counter_values);
	}

	return 0;
}
#endif