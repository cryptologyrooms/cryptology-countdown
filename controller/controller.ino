#include <TaskAction.h>

#include "display.h"
#include "seven_segment_map.h"
#include "counter.h"

static const int START_PIN = A0;
static const int RESET_PIN = A1;

static bool s_counting = false;

static int s_counter;
static int s_counter_values[3];

static void update_both_displays()
{
	display_update(s_counter_values, 0);
	display_update(s_counter_values, 1);
}

static void countdown_task_fn()
{
	if (s_counting)
	{
		countdown(s_counter, s_counter_values);
	}

	update_both_displays();

	if (s_counter == 0)
	{
		s_counting = false;
	}
}
static TaskAction s_countdown_task(countdown_task_fn, 100, INFINITE_TICKS);

static void heartbeat_task_fn()
{
	static bool led_on = false;
	digitalWrite(13, led_on = !led_on);
}
static TaskAction s_heartbeat_task(heartbeat_task_fn, 500, INFINITE_TICKS);

static bool debounce_pin(int pin, int& debounce_value, const int max_value)
{
	debounce_value += digitalRead(pin) == LOW ? 1 : -1;
	debounce_value = max(debounce_value, 0);
	debounce_value = min(debounce_value, max_value);

	return debounce_value == max_value; 
}

static void buttons_task_fn()
{
	static int debounce[2] {0, 0};
	static bool triggered[2] = {false, false};

	if (debounce_pin(START_PIN, debounce[0], 10))
	{
		if (!triggered[0])
		{	
			triggered[0] = true;
			reset_counter(s_counter, s_counter_values);
			update_both_displays();
			s_countdown_task.ResetTime();
			s_counting = true;
		}
	}
	else
	{
		triggered[0] = false;
	}

	if (debounce_pin(RESET_PIN, debounce[1], 10))
	{
		if (!triggered[1])
		{	
			triggered[1] = true;
			s_counting = false;
			reset_counter(s_counter, s_counter_values);
			update_both_displays();
		}
	}
	else
	{
		triggered[1] = false;
	}
}
static TaskAction s_buttons_task(buttons_task_fn, 10, INFINITE_TICKS);

static void test_display(int between_digit_delay)
{
	reset_counter(s_counter, s_counter_values);

	while(s_counter > 0)
	{
		countdown(s_counter, s_counter_values);
		update_both_displays();
		delay(between_digit_delay);
	}
}

void setup()
{

	delay(2000);

	Serial.begin(115200);

	pinMode(13, OUTPUT);
	pinMode(START_PIN, INPUT_PULLUP);
	pinMode(RESET_PIN, INPUT_PULLUP);

	display_setup();

	test_display(3);

	reset_counter(s_counter, s_counter_values);
	update_both_displays();
}

void loop()
{
	s_countdown_task.tick();
	s_buttons_task.tick();
	s_heartbeat_task.tick();
}
