#define SIMPLE_BACKEND_SOFTWARE
#include <simple.h>

int main(void) {
	init(800, 600, "Software - X11 - simple");
	while (!should_quit()) {
		clear(1.0f, 0.0f, 0.0f, 1.0f);
		for (int y = window_height() / 4; y < window_height() * 0.75f; y++) {
			for (int x = window_width() / 4; x < window_width() * 0.75f; x++) {
				pixels()[y * pixels_width() + x] = 0xff00ff00;
			}
		}
		display();
		poll_events();
	}
	quit();
}
