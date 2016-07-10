#include <TaskAction.h>

#include "display.h"
#include "seven_segment_map.h"
#include "counter.h"

static const int START_PIN = A0;
static const int RESET_PIN = A0;

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

	if (debounce_pin(START_PIN, debounce[0], 10))
	{
		reset_counter(s_counter, s_counter_values);
		update_both_displays();
		s_countdown_task.ResetTime();
		s_counting = true;
	}

	if (debounce_pin(RESET_PIN, debounce[1], 10))
	{
		reset_counter(s_counter, s_counter_values);
		update_both_displays();
	}
}
static TaskAction s_buttons_task(buttons_task_fn, 10, INFINITE_TICKS);

void setup()
{
	Serial.begin(115200);

	display_setup();

	reset_counter(s_counter, s_counter_values);
}

void loop()
{
	s_countdown_task.tick();
	s_buttons_task.tick();
}
