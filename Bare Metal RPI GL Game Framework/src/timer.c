#include "timer.h"

volatile unsigned long long *timer_counter = (unsigned long long *) (RPI_TIMER_BASE + RPI_TIMER_COUNTER);

unsigned long long timer_last_fps_check = 0;
int timer_fps_count = 0;
int timer_fps_current = 0;

void timer_count_frame() {
	timer_fps_count++;
	unsigned long long elapsed = *timer_counter - timer_last_fps_check;
	if (elapsed >= TIMER_SEC) {
		timer_fps_current = timer_fps_count;
		timer_fps_count = 0;
		timer_last_fps_check = *timer_counter;
	}
}

void timer_sleep(unsigned long long delay) {
	volatile unsigned long long wait = *timer_counter + delay;
	while (*timer_counter < wait);
}