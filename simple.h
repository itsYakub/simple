#if defined (__cplusplus)
# pragma once
#endif
#if !defined (_simple_h_)
# define _simple_h_

# if !defined (SIMPLE_VERSION)
#  define SIMPLE_VERSION "1.0"
# endif

/*	API: General
 * */

int		init(unsigned, unsigned, const char *);
int		quit(void);

int		poll_events(void);
int		should_quit(void);
int		clear(float, float, float, float);
int		display(void);

int		window_width(void);
int		window_height(void);
int		monitor_width(void);
int		monitor_height(void);

double	delta_time(void);
double	get_time(void);

int		key_up(int);
int		key_down(int);
int		key_press(int);
int		key_release(int);

/*	API: Software Back - End specific
 * */

int		*pixels(void);
int		*pixel(int, int);
int		pixels_width(void);
int		pixels_height(void);

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
#  endif

#  if !defined (SIMPLE_BACKEND_SOFTWARE) && !defined (SIMPLE_BACKEND_OPENGL)
#   error "error: no backend specified"
#  elif defined (SIMPLE_BACKEND_SOFTWARE) && defined (SIMPLE_BACKEND_OPENGL)
#   error "error: multiple backends specified"
#  endif

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
#    endif
#   endif
#  endif

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
#  endif

/* Macro - Definitions */

#  if !defined (SIMPLE_LOG_INFO)
#   define SIMPLE_LOG_INFO(...) (fprintf(stdout, "[ info ] "__VA_ARGS__))
#  endif
#  if !defined (SIMPLE_LOG_WARN)
#   define SIMPLE_LOG_WARN(...) (fprintf(stderr, "[ warn ] "__VA_ARGS__))
#  endif
#  if !defined (SIMPLE_LOG_ERROR)
#   define SIMPLE_LOG_ERROR(...) (fprintf(stderr, "[ error ] "__VA_ARGS__))
#  endif

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

#   endif

		Atom		wm_message_quit;

#  elif defined (_WIN32)
#   warning "warn: win32 to be added"
#  endif

	}	s_window;

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	struct {
		uint32_t	*pixels;
		uint32_t	width;
		uint32_t	height;
		uint32_t	bits;
	}	s_framebuffer;

#  endif

	struct {
		double		current;
		double		previous;
		double		delta;
	}	s_time;

	struct {
		uint8_t	key_current[65536];
		uint8_t	key_previous[65536];
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

#   endif
#  endif

/*	API
 * */

int	init(unsigned w, unsigned h, const char *t) {
	SIMPLE.s_window.width = w;
	SIMPLE.s_window.height = h;
	SIMPLE.s_window.quit = false;
	memset(SIMPLE.s_input.key_current, 0, sizeof(SIMPLE.s_input.key_current));
	memset(SIMPLE.s_input.key_previous, 0, sizeof(SIMPLE.s_input.key_previous));

#  if defined (__linux__)
#   if defined (SIMPLE_BACKEND_OPENGL)

	GLXFBConfig				*_fbconf_arr;
	GLXFBConfig				_fbconf;
	int32_t					_fbconf_best,
							_fbconf_count,
							_fbconf_samples,
							_fbconf_sample_buffers;

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	XWindowAttributes		_winattr;

#   endif

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

#   endif

	_swinattr.colormap = XCreateColormap(
		SIMPLE.s_window.dsp,
		SIMPLE.s_window.r_id,
		_vi->visual,
		AllocNone
	);
	_swinattr.event_mask |= ClientMessage
		| StructureNotifyMask
		| KeyPressMask
		| KeyReleaseMask;
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

#   endif

	SIMPLE.s_window.wm_message_quit = XInternAtom(SIMPLE.s_window.dsp, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id, &SIMPLE.s_window.wm_message_quit, 1);
	XMapWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XFree(_vi);

#  elif defined (_WIN32)

#  endif

#  if defined (__linux__)
	SIMPLE_LOG_INFO("Platform - GNU/Linux\n");
	SIMPLE_LOG_INFO("Front - End: X11\n");
#  elif defined (_WIN32)
	SIMPLE_LOG_INFO("Platform - Windows\n");
	SIMPLE_LOG_INFO("Front - End: Win32\n");
#  endif
#  if defined (SIMPLE_BACKEND_OPENGL)
	SIMPLE_LOG_INFO("Back - End: OpenGL\n");
#  elif defined (SIMPLE_BACKEND_SOFTWARE)
	SIMPLE_LOG_INFO("Back - End: Software\n");
#  endif

	return (1);
}

int	quit(void) {

#  if defined (__linux__)
#   if defined (SIMPLE_BACKEND_OPENGL)

	glXDestroyContext(SIMPLE.s_window.dsp, SIMPLE.s_window.ctx);

#   elif defined (SIMPLE_BACKEND_SOFTWARE)

	XDestroyImage(SIMPLE.s_window.ximg);

#   endif

	XUnmapWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XDestroyWindow(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);
	XCloseDisplay(SIMPLE.s_window.dsp);

#  endif

	return (1);
}

int	poll_events(void) {

#  if defined (__linux__)

	XEvent	_event;

	memcpy(SIMPLE.s_input.key_previous, SIMPLE.s_input.key_current, sizeof(SIMPLE.s_input.key_previous));
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

#   endif
							
			} break;

			case (ClientMessage): {
				if ((Atom) _event.xclient.data.l[0] == SIMPLE.s_window.wm_message_quit) {
					SIMPLE.s_window.quit = true;
				}
			} break;

			case (KeyPress): {
				size_t	_keysym;

				_keysym = XkbKeycodeToKeysym(SIMPLE.s_window.dsp, _event.xkey.keycode, 0, 0);
				SIMPLE.s_input.key_current[_keysym] = 1;
			} break;
			
			case (KeyRelease): {
				size_t	_keysym;

				_keysym = XkbKeycodeToKeysym(SIMPLE.s_window.dsp, _event.xkey.keycode, 0, 0);
				SIMPLE.s_input.key_current[_keysym] = 0;
			} break;
		}
	}

#  elif defined (_WIN32)
#   error "error: win32 to be added"
#  endif
	
	SIMPLE.s_time.previous = SIMPLE.s_time.current;
	SIMPLE.s_time.current = get_time();

	return (1);
}

int	should_quit(void) {
	return (SIMPLE.s_window.quit);
}

int	clear(float r, float g, float b, float a) {
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

#  if defined (__linux__)

	_rgba |= ((uint8_t) (r * 255)) << (8 * 2);
	_rgba |= ((uint8_t) (g * 255)) << (8 * 1);
	_rgba |= ((uint8_t) (b * 255)) << (8 * 0);
	_rgba |= ((uint8_t) (a * 255)) << (8 * 3);

#  elif defined (_WIN32)

	_rgba |= ((uint8_t) (r * 255)) << (8 * 0);
	_rgba |= ((uint8_t) (g * 255)) << (8 * 1);
	_rgba |= ((uint8_t) (b * 255)) << (8 * 2);
	_rgba |= ((uint8_t) (a * 255)) << (8 * 3);

#  endif

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

#  endif

	return (1);
}

int	display(void) {

#  if defined (SIMPLE_BACKEND_OPENGL)
#   if defined (__linux__)

	glXSwapBuffers(SIMPLE.s_window.dsp, SIMPLE.s_window.w_id);

#   elif defined (_WIN32)
#   error "error: win32 to be added"
#   endif
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

#   if defined (__linux__)
#   elif defined (_WIN32)
#   endif
#  endif
	return (1);
}

int	window_width(void) {
	return (SIMPLE.s_window.width);
}

int	window_height(void) {
	return (SIMPLE.s_window.height);
}

int	monitor_width(void) {
	XWindowAttributes	_attr;

#  if defined (__linux__)

	XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.r_id, &_attr);

#  elif defined (_WIN32)
#   error "error: win32 to be added"
#  endif

	return (_attr.width);
}

int	monitor_hegith(void) {
	XWindowAttributes	_attr;

#  if defined (__linux__)

	XGetWindowAttributes(SIMPLE.s_window.dsp, SIMPLE.s_window.r_id, &_attr);

#  elif defined (_WIN32)
#   error "error: win32 to be added"
#  endif

	return (_attr.height);
}

double	delta_time(void) {
	return (SIMPLE.s_time.delta = (SIMPLE.s_time.current - SIMPLE.s_time.previous) / 1000.0);
}

double	get_time(void) {
	double	_time;

#  if defined (__linux__)
	struct timeval	_t;

	if (gettimeofday(&_t, 0) < 0) {
		return (!SIMPLE_LOG_ERROR("syscall 'gettimeofday' failed\n"));
	}
	_time = _t.tv_sec * 1000.0 + _t.tv_usec / 1000.0;

#  elif defined (_WIN32)
#   error "error: win32 to be added"
#  endif

	return (_time);
}

int	key_up(int key) {
	return (!SIMPLE.s_input.key_current[key]);
}

int	key_down(int key) {
	return (SIMPLE.s_input.key_current[key]);
}

int	key_press(int key) {
	return (SIMPLE.s_input.key_current[key] && !SIMPLE.s_input.key_previous[key]);
}

int	key_release(int key) {
	return (!SIMPLE.s_input.key_current[key] && SIMPLE.s_input.key_previous[key]);
}

int	*pixels(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return ((int32_t *) SIMPLE.s_framebuffer.pixels);

#  endif

	return (0);
}

int		*pixel(int x, int y) {
#  if defined (SIMPLE_BACKEND_SOFTWARE)

	x = (x < 0 ? 0 : (x >= SIMPLE.s_framebuffer.width ? SIMPLE.s_framebuffer.width - 1 : x));
	y = (y < 0 ? 0 : (y >= SIMPLE.s_framebuffer.height ? SIMPLE.s_framebuffer.height - 1 : y));
	return ((int32_t *) &SIMPLE.s_framebuffer.pixels[y * pixels_width() + x]);

#  endif

	return (0);
}

int	pixels_width(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return (SIMPLE.s_framebuffer.width);

#  endif

	return (0);
}

int	pixels_height(void) {

#  if defined (SIMPLE_BACKEND_SOFTWARE)

	return (SIMPLE.s_framebuffer.height);

#  endif

	return (0);
}
# endif

#endif
