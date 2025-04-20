#if defined (__cplusplus)
# pragma once
#endif
#if !defined (_simple_h_)
# define _simple_h_

# if !defined (SIMPLE_VERSION)
#  define SIMPLE_VERSION "1.0"
# endif

/*	API
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
int		pixels_width(void);
int		pixels_height(void);

#endif
