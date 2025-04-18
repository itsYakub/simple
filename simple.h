#if defined (__cplusplus)
# pragma once
#endif
#if !defined (_simple_h_)
# define _simple_h_

# if !defined (SIMPLE_BACKEND_SOFTWARE) && !defined (SIMPLE_BACKEND_OPENGL)
#  error "error: no backend defined"
# elif defined (SIMPLE_BACKEND_SOFTWARE) && defined (SIMPLE_BACKEND_OPENGL)
#  error "error: multiple backend defined"
# endif

int	init(unsigned, unsigned, const char *);
int	quit(void);
int	poll_events(void);
int	should_quit(void);
int	clear(float, float, float, float);
int	refresh(void);
int	window_width(void);
int	window_height(void);
int	monitor_width(void);
int	monitor_height(void);

#endif
