#define SIMPLE_BACKEND_OPENGL
#include "../simple.h"

#include <GL/gl.h>

int main(void) {
	init(800, 600, "OpenGL - X11 - simple");

	while (!should_quit()) {
		clear(1.0, 0.0, 0.0, 1.0);
		refresh();
		poll_events();
	}
	quit();
}
