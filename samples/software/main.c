#define SIMPLE_BACKEND_SOFTWARE
#define SIMPLE_IMPLEMENTATION
#include "../../simple.h"

static int	__draw_square(int, int, int, int);
static int	__draw_circle(int, int, int, int);

int main(void) {
	int	x;
	int	y;

	init(800, 600, "Hello, simple!");
	while (!should_quit()) {
		printf("%i %i\n", mouse_position_x(), mouse_position_y());
		clear(0.2f, 0.2f, 0.2f, 1.0f);
		for (int i = 1; i < window_width() / 100; i++) {
			for (int j = 1; j < window_height() / 100; j++) {
				if (j % 2) {
					if (i % 2) {
						__draw_circle(i * 100, j * 100, 48, 0xff00ff00);
					}
					else {
						__draw_square(i * 100 - 32, j * 100 - 32, 64, 0xffff0000);
					}
				}
				else {
					if (!(i % 2)) {
						__draw_circle(i * 100, j * 100, 48, 0xff00ff00);
					}
					else {
						__draw_square(i * 100 - 32, j * 100 - 32, 64, 0xffff0000);
					}

				}
			}
		}
		display();
		poll_events();
	}
	quit();
}

static int	__draw_square(int x, int y, int s, int pix) {
	for (int i = y; i < y + s; i++) {
		for (int j = x; j < x + s; j++) {
			*pixel(j, i) = pix;
		}
	}
	return (1);
}

static int	__draw_circle(int x, int y, int r, int pix) {
	int	_x0, _y0;
	int	_x1, _y1;

	_x0 = x - r;
	_x1 = x + r;
	_y0 = y - r;
	_y1 = y + r;
	for (int i = _y0; i < _y1; i++) {
		for (int j = _x0; j < _x1; j++) {
			int	_dx;
			int	_dy;

			_dx = j - x;
			_dy = i - y;
			if ((_dx * _dx) + (_dy * _dy) <= (r * r)) {
				*pixel(j, i) = pix;
			}
		}
	}
	return (1);
}
