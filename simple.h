#if defined (__cplusplus)
# pragma once
#endif /* __cplusplus */
#if !defined (_simple_h_)
# define _simple_h_

# if !defined (SIMPLE_VERSION)
#  define SIMPLE_VERSION "1.0"
# endif /* SIMPLE_VERSION */
# if !defined (SAPI)
#  if !defined (__cplusplus)
#   define SAPI static inline
#  else
#   define SAPI extern "C"
#  endif /* __cplusplus */
# endif /* SAPI */

# if defined (__cplusplus)

extern "C" {

# endif /* __cplusplus */

/*	API: General
 * */

SAPI int	init(unsigned, unsigned, const char *);
SAPI int	quit(void);

SAPI int	poll_events(void);
SAPI int	should_quit(void);
SAPI int	clear(float, float, float, float);
SAPI int	display(void);

SAPI int	window_width(void);
SAPI int	window_height(void);
SAPI int	monitor_width(void);
SAPI int	monitor_height(void);

SAPI double	delta_time(void);
SAPI double	get_time(void);

SAPI int	key_up(int);
SAPI int	key_down(int);
SAPI int	key_press(int);
SAPI int	key_release(int);

SAPI int	mouse_x(void);
SAPI int	mouse_y(void);
SAPI int	button_up(int);
SAPI int	button_down(int);
SAPI int	button_press(int);
SAPI int	button_release(int);

/*	API: Software Back - End specific
 * */

SAPI int	*pixels(void);
SAPI int	*pixel(int, int);
SAPI int	pixels_width(void);
SAPI int	pixels_height(void);

# if defined (__cplusplus)

}

# endif /* __cplusplus */

# if defined (SIMPLE_IMPLEMENTATION)
#  if !defined (__cplusplus)
#   include <stdio.h>
#   include <stdlib.h>
#   include <stdbool.h>
#   include <stdint.h>
#   include <string.h>
#  elif
#   include <cstdio>
#   include <cstdlib>
#   include <cstdint>
#   include <cstring>
#  endif /* __cplusplus */

#  if !defined (SIMPLE_BACKEND_SOFTWARE) && !defined (SIMPLE_BACKEND_OPENGL)
#   error "error: no backend specified"
#  elif defined (SIMPLE_BACKEND_SOFTWARE) && defined (SIMPLE_BACKEND_OPENGL)
#   error "error: multiple backends specified"
#  endif /* SIMPLE_BACKEND_SOFTWARE, SIMPLE_BACKEND_OPENGL */

/* Back - End inclusions */

#  if !defined (SIMPLE_BACKEND_SOFTWARE)
#   if defined (SIMPLE_BACKEND_OPENGL)
#    include <GL/gl.h>
#    if defined (__linux__)
#     include <GL/glx.h>
#    elif defined (_WIN32)
#     include <GL/wgl.h>
#    else
#     error "error: no backend selected"
#    endif /* __linux__, _WIN32 */
#   endif /* SIMPLE_BACKEND_OPENGL */
#  endif /* SIMPLE_BACKEND_SOFTWARE */

/* Front - End inclusions */

#  if defined (__linux__)
#   include <X11/X.h>
#   include <X11/Xlib.h>
#   include <X11/Xutil.h>
#   include <X11/XKBlib.h>
#   include <X11/keysym.h>
#   include <X11/keysymdef.h>
#   include <sys/time.h>
#  elif defined (_WIN32)
#   include <windows.h>
#   include <wingdi.h>
#  endif /* __linux__, _WIN32 */

/* Macro - Definitions */

#  if !defined (SIMPLE_LOG_INFO)
#   define SIMPLE_LOG_INFO(...) (fprintf(stdout, "[ info ] "__VA_ARGS__))
#  endif /* SIMPLE_LOG_INFO */
#  if !defined (SIMPLE_LOG_WARN)
#   define SIMPLE_LOG_WARN(...) (fprintf(stderr, "[ warn ] "__VA_ARGS__))
#  endif /* SIMPLE_LOG_WARN */
#  if !defined (SIMPLE_LOG_ERROR)
#   define SIMPLE_LOG_ERROR(...) (fprintf(stderr, "[ error ] "__VA_ARGS__))
#  endif /* SIMPLE_LOG_ERROR */

# if !defined (SIMPLE_INPUT_KEYBOARD_KEYS)
#  define SIMPLE_INPUT_KEYBOARD_KEYS 16384
# endif
# if !defined (SIMPLE_INPUT_MOUSE_BUTTONS)
#  define SIMPLE_INPUT_MOUSE_BUTTONS 8
# endif

struct s_simple {
	struct {

		uint32_t	width;
		uint32_t	height;
		bool		quit;

#  if defined (__linux__)

		Display		*dsp;
		Window		r_id;
		Window		w_id;

#   if defined (SIMPLE_BACKEND_OPENGL)

		GLXContext	ctx;

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

		XImage		*ximg;
		GC			ctx;

#   endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

		Atom		wm_message_quit;

#  elif defined (_WIN32)
#   warning "warn: win32 to be added"
#  endif /* __linux__, _WIN32 */

	}	s_window;

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	struct {
		uint32_t	*pixels;
		uint32_t	width;
		uint32_t	height;
		uint32_t	bits;
	}	s_framebuffer;

#  endif /* SIMPLE_BACKEND_SOFTWARE */

	struct {
		double		current;
		double		previous;
		double		delta;
	}	s_time;

	struct {
		uint8_t		key_input_current[SIMPLE_INPUT_KEYBOARD_KEYS];
		uint8_t		key_input_previous[SIMPLE_INPUT_KEYBOARD_KEYS];
		uint8_t		mouse_input_current[SIMPLE_INPUT_MOUSE_BUTTONS];
		uint8_t		mouse_input_previous[SIMPLE_INPUT_MOUSE_BUTTONS];
		uint32_t	mouse_position_current[2];
		uint32_t	mouse_position_previous[2];
	}	s_input;
};

typedef struct s_simple	t_simple;

static t_simple	SIMPLE;

#  if defined (__linux__)
#   if defined (SIMPLE_BACKEND_OPENGL)

typedef GLXContext (* PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
static PFNGLXCREATECONTEXTATTRIBSARBPROC	glXCreateContextAttribsARB;

static int	_window_attributes[] = {
	GLX_USE_GL,				1,
	GLX_DOUBLEBUFFER,		1,
	GLX_DEPTH_SIZE,         24,                            
	GLX_RED_SIZE,           8,
	GLX_GREEN_SIZE,         8,
	GLX_BLUE_SIZE,          8,
	GLX_ALPHA_SIZE,         8,
	GLX_X_VISUAL_TYPE,      GLX_TRUE_COLOR,
	GLX_RENDER_TYPE,        GLX_RGBA_BIT,
	GLX_DRAWABLE_TYPE,      GLX_WINDOW_BIT,
	None
};

static int	_context_attributes[] = {
	GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
	GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
	GLX_CONTEXT_MINOR_VERSION_ARB, 3,
	None
};

#   endif /* SIMPLE_BACKEND_OPENGL */
#  endif /* __linux__ */

/*	API - Linux Implementation
 *
 *	Useful sources:
 *	- http://xahlee.info/linux/linux_x11_mouse_button_number.html
 * */

#  if defined (__linux__)

SAPI int	init(unsigned w, unsigned h, const char *t) {
	SIMPLE.s_window.width = w;
	SIMPLE.s_window.height = h;
	SIMPLE.s_window.quit = false;
	memset(SIMPLE.s_input.key_input_current, 0, sizeof(SIMPLE.s_input.key_input_current));
	memset(SIMPLE.s_input.key_input_previous, 0, sizeof(SIMPLE.s_input.key_input_previous));
	memset(SIMPLE.s_input.mouse_input_current, 0, sizeof(SIMPLE.s_input.mouse_input_current));
	memset(SIMPLE.s_input.mouse_input_previous, 0, sizeof(SIMPLE.s_input.mouse_input_previous));
	memset(SIMPLE.s_input.mouse_position_current, 0, sizeof(SIMPLE.s_input.mouse_position_current));
	memset(SIMPLE.s_input.mouse_position_previous, 0, sizeof(SIMPLE.s_input.mouse_position_previous));

#   if defined (SIMPLE_BACKEND_OPENGL)

	GLXFBConfig				*_fbconf_arr;
	GLXFBConfig				_fbconf;
	int32_t					_fbconf_best,
							_fbconf_count,
							_fbconf_samples,
							_fbconf_sample_buffers;

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	XWindowAttributes		_winattr;

#   endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	XSetWindowAttributes	_swinattr;
	XVisualInfo				*_vi;
	uint32_t				_winmask;

	_winmask = 0;
	_winmask |= CWColormap
		| CWBorderPixel
		| CWBackPixel
		| CWEventMask;
	_swinattr = (XSetWindowAttributes) { 0 };

	SIMPLE.s_window.dsp = XOpenDisplay(NULL);
	if (!SIMPLE.s_window.dsp) {
		return (!SIMPLE_LOG_ERROR("Could't open an X11 Display\n"));
	}
	SIMPLE.s_window.r_id = DefaultRootWindow(SIMPLE.s_window.dsp);
	if (SIMPLE.s_window.r_id == None) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Could't find a default root window\n"));
	}

#   if defined (SIMPLE_BACKEND_OPENGL)

	_fbconf_best = -1;
	_fbconf_arr = glXChooseFBConfig(
		SIMPLE.s_window.dsp,
		DefaultScreen(SIMPLE.s_window.dsp),
		_window_attributes,
		&_fbconf_count
	);
	for (size_t i = 0; i < (size_t) _fbconf_count; i++) {
		_vi = glXGetVisualFromFBConfig(SIMPLE.s_window.dsp, _fbconf_arr[i]);
		if (!_vi) {
			continue;
		}
		XFree(_vi);
		glXGetFBConfigAttrib(SIMPLE.s_window.dsp, _fbconf_arr[i], GLX_SAMPLE_BUFFERS, &_fbconf_sample_buffers);
		glXGetFBConfigAttrib(SIMPLE.s_window.dsp, _fbconf_arr[i], GLX_SAMPLES, &_fbconf_samples);
		if ((_fbconf_best < 0 || _fbconf_sample_buffers) && (!_fbconf_samples && _fbconf_best == -1)) {
			_fbconf_best = i;
		}
	}
	if (_fbconf_best == -1) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Couldn't find the best GLXFBConfig\n"));
	}
	_fbconf = _fbconf_arr[_fbconf_best];
	XFree(_fbconf_arr);
	_vi = glXGetVisualFromFBConfig(SIMPLE.s_window.dsp, _fbconf);
	if (!_vi) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Couldn't create an XVisualInfo object\n"));
	}

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	_vi = (XVisualInfo *) malloc(sizeof(XVisualInfo));
	if (!_vi) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Couldn't create an XVisualInfo object\n"));
	}
	XMatchVisualInfo(SIMPLE.s_window.dsp, DefaultScreen(SIMPLE.s_window.dsp), 24, TrueColor, _vi);
	if (!_vi) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Couldn't create an XVisualInfo object\n"));
	}

#   endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	_swinattr.colormap = XCreateColormap(
		SIMPLE.s_window.dsp,
		SIMPLE.s_window.r_id,
		_vi->visual,
		AllocNone
	);
	_swinattr.event_mask |= ClientMessage
		| StructureNotifyMask
		| KeyPressMask
		| KeyReleaseMask
		| PointerMotionMask
		| ButtonPressMask
		| ButtonReleaseMask;
	SIMPLE.s_window.w_id = XCreateWindow(
		SIMPLE.s_window.dsp,
		SIMPLE.s_window.r_id,
		0, 0, w, h, 0,
		_vi->depth,
		InputOutput,
		_vi->visual,
		_winmask,
		&_swinattr
	);
	if (SIMPLE.s_window.w_id == None) {
		XCloseDisplay(SIMPLE.s_window.dsp);
		return (!SIMPLE_LOG_ERROR("Couldn't create a window\n"));
	}
	XStoreName(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id, t);

#   if defined (SIMPLE_BACKEND_OPENGL)

	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((GLubyte *) "glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB) {
		SIMPLE.s_window.ctx = glXCreateContextAttribsARB(SIMPLE.s_window.dsp, _fbconf, 0, 1, _context_attributes);
		if (!SIMPLE.s_window.ctx) {
			return (!SIMPLE_LOG_ERROR("GLX Context creation failed\n"));
		}
	}
	else {
		SIMPLE.s_window.ctx = glXCreateContext(SIMPLE.s_window.dsp, _vi, 0, 1);
		if (!SIMPLE.s_window.ctx) {
			return (!SIMPLE_LOG_ERROR("GLX Context creation failed\n"));
		}
	}
	glXMakeCurrent(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id, SIMPLE.s_window.ctx);

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.r_id, &_winattr);
	SIMPLE.s_window.ctx = XDefaultGC(SIMPLE.s_window.dsp, DefaultScreen(SIMPLE.s_window.dsp));
	SIMPLE.s_framebuffer.width = _winattr.width;
	SIMPLE.s_framebuffer.height = _winattr.height;
	SIMPLE.s_framebuffer.bits = 32;
	SIMPLE.s_framebuffer.pixels = (uint32_t *) malloc(_winattr.width * _winattr.height * sizeof(uint32_t));
	SIMPLE.s_window.ximg = XCreateImage(
		SIMPLE.s_window.dsp,
		_vi->visual,
		_vi->depth,
		ZPixmap,
		None,
		(char *) SIMPLE.s_framebuffer.pixels,
		SIMPLE.s_framebuffer.width,
		SIMPLE.s_framebuffer.height,
		SIMPLE.s_framebuffer.bits,
		SIMPLE.s_framebuffer.width * sizeof(int32_t)
	);

#   endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	SIMPLE.s_window.wm_message_quit = XInternAtom(SIMPLE.s_window.dsp, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id, &SIMPLE.s_window.wm_message_quit, 1);
	XMapWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XFree(_vi);

	SIMPLE_LOG_INFO("Platform - GNU/Linux\n");
	SIMPLE_LOG_INFO("Front - End: X11\n");
#  if defined (SIMPLE_BACKEND_OPENGL)
	SIMPLE_LOG_INFO("Back - End: OpenGL\n");
#  elif defined (SIMPLE_BACKEND_SOFTWARE)
	SIMPLE_LOG_INFO("Back - End: Software\n");
#  endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	return (1);
}

SAPI int	quit(void) {

#   if defined (SIMPLE_BACKEND_OPENGL)

	glXDestroyContext(SIMPLE.s_window.dsp, SIMPLE.s_window.ctx);

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	XDestroyImage(SIMPLE.s_window.ximg);

#   endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	XUnmapWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XDestroyWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XCloseDisplay(SIMPLE.s_window.dsp);

	return (1);
}

SAPI int	poll_events(void) {
	XEvent	_event;

	memcpy(SIMPLE.s_input.key_input_previous, SIMPLE.s_input.key_input_current, sizeof(SIMPLE.s_input.key_input_previous));
	memcpy(SIMPLE.s_input.mouse_input_previous, SIMPLE.s_input.mouse_input_current, sizeof(SIMPLE.s_input.mouse_input_previous));
	memcpy(SIMPLE.s_input.mouse_position_previous, SIMPLE.s_input.mouse_position_current, sizeof(SIMPLE.s_input.mouse_position_previous));
	while (XPending(SIMPLE.s_window.dsp)) {
		XNextEvent(SIMPLE.s_window.dsp, &_event);
		switch (_event.type) {
			case (ConfigureNotify): {
				XWindowAttributes	_attr;

				XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id, &_attr);
				SIMPLE.s_window.width = _attr.width;
				SIMPLE.s_window.height = _attr.height;

#   if defined (SIMPLE_BACKEND_OPENGL)

				glViewport(0, 0, window_width(), window_height());

#   endif /* SIMPLE_BACKEND_OPENGL */
							
			} break;

			case (ClientMessage): {
				if ((Atom) _event.xclient.data.l[0] == SIMPLE.s_window.wm_message_quit) {
					SIMPLE.s_window.quit = true;
				}
			} break;

			case (KeyPress): {
				size_t	_keysym;

				_keysym = XkbKeycodeToKeysym(SIMPLE.s_window.dsp, _event.xkey.keycode, 0, 0);
				SIMPLE.s_input.key_input_current[_keysym] = 1;
			} break;
			
			case (KeyRelease): {
				size_t	_keysym;

				_keysym = XkbKeycodeToKeysym(SIMPLE.s_window.dsp, _event.xkey.keycode, 0, 0);
				SIMPLE.s_input.key_input_current[_keysym] = 0;
			} break;

			case (ButtonPress): {
				uint8_t	*_ptr;

				switch (_event.xbutton.button) {
					case (1): { _ptr = &SIMPLE.s_input.mouse_input_current[0]; } break;
					case (2): { _ptr = &SIMPLE.s_input.mouse_input_current[1]; } break;
					case (3): { _ptr = &SIMPLE.s_input.mouse_input_current[2]; } break;
					case (8): { _ptr = &SIMPLE.s_input.mouse_input_current[3]; } break;
					case (9): { _ptr = &SIMPLE.s_input.mouse_input_current[4]; } break;
					default: { _ptr = 0; } break;
				}
				if (!_ptr) {
					break;
				}
				*_ptr = 1;
			} break;

			case (ButtonRelease): {
				uint8_t	*_ptr;

				switch (_event.xbutton.button) {
					case (1): { _ptr = &SIMPLE.s_input.mouse_input_current[0]; } break;
					case (2): { _ptr = &SIMPLE.s_input.mouse_input_current[1]; } break;
					case (3): { _ptr = &SIMPLE.s_input.mouse_input_current[2]; } break;
					case (8): { _ptr = &SIMPLE.s_input.mouse_input_current[3]; } break;
					case (9): { _ptr = &SIMPLE.s_input.mouse_input_current[4]; } break;
					default: { _ptr = 0; } break;
				}
				if (!_ptr) {
					break;
				}
				*_ptr = 0;
			} break;

			case (MotionNotify): {
				SIMPLE.s_input.mouse_position_current[0] = _event.xmotion.x;
				SIMPLE.s_input.mouse_position_current[1] = _event.xmotion.y;
			} break;
		}
	}
	
	SIMPLE.s_time.previous = SIMPLE.s_time.current;
	SIMPLE.s_time.current = get_time();

	return (1);
}

SAPI int	should_quit(void) {
	return (SIMPLE.s_window.quit);
}

SAPI int	clear(float r, float g, float b, float a) {
	(void) r;
	(void) g;
	(void) b;
	(void) a;

#  if defined (SIMPLE_BACKEND_OPENGL)

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(r, g, b, a);

#  elif defined (SIMPLE_BACKEND_SOFTWARE)

	uint32_t	_rgba;
	uint32_t	_w;
	uint32_t	_h;

	_rgba = 0;
	_rgba |= ((uint8_t) (r * 255)) << (8 * 2);
	_rgba |= ((uint8_t) (g * 255)) << (8 * 1);
	_rgba |= ((uint8_t) (b * 255)) << (8 * 0);
	_rgba |= ((uint8_t) (a * 255)) << (8 * 3);

	_w = window_width();
	_h = window_height();
	for (size_t y = 0; y < _h; y++) {
		for (size_t x = 0; x < _w; x++) {
			size_t	_pos;
			int8_t	*_dst;

			_pos = y * (SIMPLE.s_framebuffer.width * sizeof(uint32_t)) + x * (SIMPLE.s_framebuffer.bits / 8);
			_dst = (int8_t *) SIMPLE.s_framebuffer.pixels + _pos;
			*(uint32_t *) _dst = _rgba;
		}
	}

#  endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	return (1);
}

SAPI int	display(void) {

#  if defined (SIMPLE_BACKEND_OPENGL)

	glXSwapBuffers(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);

#  elif defined (SIMPLE_BACKEND_SOFTWARE)

	XPutImage(
		SIMPLE.s_window.dsp,
		SIMPLE.s_window.w_id,
		SIMPLE.s_window.ctx,
		SIMPLE.s_window.ximg,
		0, 0, 0, 0,
		SIMPLE.s_framebuffer.width,
		SIMPLE.s_framebuffer.height
	);

#  endif /* SIMPLE_BACKEND_OPENGL, SIMPLE_BACKEND_SOFTWARE */

	return (1);
}

SAPI int	window_width(void) {
	return (SIMPLE.s_window.width);
}

SAPI int	window_height(void) {
	return (SIMPLE.s_window.height);
}

SAPI int	monitor_width(void) {
	XWindowAttributes	_attr;

	XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.r_id, &_attr);
	return (_attr.width);
}

SAPI int	monitor_hegith(void) {
	XWindowAttributes	_attr;

	XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.r_id, &_attr);
	return (_attr.height);
}

SAPI double	delta_time(void) {
	return (SIMPLE.s_time.delta = (SIMPLE.s_time.current - SIMPLE.s_time.previous) / 1000.0);
}

SAPI double	get_time(void) {
	struct timeval	_t;
	double			_time;

	if (gettimeofday(&_t, 0) < 0) {
		return (!SIMPLE_LOG_ERROR("syscall 'gettimeofday' failed\n"));
	}
	_time = _t.tv_sec * 1000.0 + _t.tv_usec / 1000.0;

	return (_time);
}

SAPI int	key_up(int key) {
	return (!SIMPLE.s_input.key_input_current[key]);
}

SAPI int	key_down(int key) {
	return (SIMPLE.s_input.key_input_current[key]);
}

SAPI int	key_press(int key) {
	return (SIMPLE.s_input.key_input_current[key] && !SIMPLE.s_input.key_input_previous[key]);
}

SAPI int	key_release(int key) {
	return (!SIMPLE.s_input.key_input_current[key] && SIMPLE.s_input.key_input_previous[key]);
}

SAPI int	mouse_x(void) {
	return (SIMPLE.s_input.mouse_position_current[0]);
}

SAPI int	mouse_y(void) {
	return (SIMPLE.s_input.mouse_position_current[1]);
}

SAPI int	button_up(int button) {
	return (!SIMPLE.s_input.mouse_input_current[button]);
}

SAPI int	button_down(int button) {
	return (SIMPLE.s_input.mouse_input_current[button]);
}

SAPI int	button_press(int button) {
	return (SIMPLE.s_input.mouse_input_current[button] && !SIMPLE.s_input.mouse_input_previous[button]);
}

SAPI int	button_release(int button) {
	return (!SIMPLE.s_input.mouse_input_current[button] && SIMPLE.s_input.mouse_input_previous[button]);
}

SAPI int	*pixels(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return ((int32_t *) SIMPLE.s_framebuffer.pixels);

#  endif /* SIMPLE_BACKEND_SOFTWARE */

	return (0);
}

SAPI int	*pixel(int x, int y) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	x = (x < 0 ? 0 : (x >= SIMPLE.s_framebuffer.width ? SIMPLE.s_framebuffer.width - 1 : x));
	y = (y < 0 ? 0 : (y >= SIMPLE.s_framebuffer.height ? SIMPLE.s_framebuffer.height - 1 : y));
	return ((int32_t *) &SIMPLE.s_framebuffer.pixels[y * pixels_width() + x]);

#  endif /* SIMPLE_BACKEND_SOFTWARE */

	return (0);
}

SAPI int	pixels_width(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return (SIMPLE.s_framebuffer.width);

#  endif /* SIMPLE_BACKEND_SOFTWARE */

	return (0);
}

SAPI int	pixels_height(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return (SIMPLE.s_framebuffer.height);

#  endif /* SIMPLE_BACKEND_SOFTWARE */

	return (0);
}

/*	API - Windows Implementation
 * */

#  elif defined (_WIN32)
#   error "win32 to be added"
#  endif /* __linux__, _WIN32 */
# endif /* SIMPLE_IMPLEMENTATION */
#endif /* _simple_h_ */
