#define SIMPLE_BACKEND_SOFTWARE
#define SIMPLE_IMPLEMENTATION
#include "../../simple.h"

int main(void) {
	init(800, 600, "Hello, simple!");
	while (!should_quit()) {
		clear(0.2f, 0.2f, 0.2f, 1.0f);

		for (int h = 0; h < window_height() / 2; h++) {
			for (int w = 0; w < window_width() / 2; w++) {
				pixels()[h * window_width() + w] = 0xffff0000;
			}
		}

		display();
		poll_events();
	}
	quit();
}
