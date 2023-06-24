#include <stdio.h>
#define MAX_FRAME 100

struct window_frame {
	int startx, starty, endx, endy;
	int curx, cury;
	unsigned char *p;
	char *header;
	int border;
	int active;
} frame[MAX_FRAME];
