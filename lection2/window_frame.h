#include <stdio.h>
#define MAX_FRAME 10

struct window_frame 
{
	int startx,	endx, starty, endy; // позиции окна
	int curx, cury; // текущая позиция курсора в окне
	unsigned char *p; // указатель на буфер
	char *header; // имя окна
	int border; // есть/нет границы
	int active; // на экране или невидимо
} frame[MAX_FRAME];





