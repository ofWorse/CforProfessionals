#include "window_frame.h"
#include "../lection1/functions.h"
#include <stdlib.h>
#define NORM_VID 7
#define REV_VID 70H

char far *vid_mem;

int make_window(int num, char *header, int startx, int starty, int endx, int endy, int border)
{
	unsigned char *p;

	if(num > MAX_FRAME) {
		printf("Слишком много окон!");	
		return 0;
	}
	
	if((startx > 24) || (startx < 0) || (starty > 78) || (starty < 0)) {
		printf("Ошибка установки диапозона углов по X и Y!");	
		return 0;
	}

	if((endx > 24) || (endy > 79)) {
		printf("Окно не влезет!");
		return 0;
	}

	// теперь отводим достаточное кол-во памяти...
	p = (unsigned char *) malloc(2*(endx-startx+1)*(endy-starty+1));

	if(!p) exit(1); // TODO реализовать собственный обработчик событий!

	// создать собственную рамку
	//
	frame[num].startx = startx; frame[num].endx = endx;
	frame[num].starty = starty; frame[num].endy = endy;
	frame[num].p = p;
	frame[num].header = header;
	frame[num].border = border;
	frame[num].active = 0;
	frame[num].curx = 0; frame[num].cury = 0;

	return 1;
}

// функция деактивации окна
void deactivate(int num) 
{
	// установка курсора в левый верхний угол
	frame[num].curx = 0;
	frame[num].cury = 0;
	restore_video(num);
}

void display_header(int num)
{
	register int y, len;
	y = frame[num].starty;

	// Вычислить начальную позицию относительно центра текста 
	// заголовка, если отрицательная, то текст не подходит
	len = strlen(frame[num].header);
	len = (frame[num].endy - y - len) / 2;
	if(len < 0) return; // не выводить на экран
	y = y + len;

	write_string(frame[num].startx, y, frame[num].header, NORM_VID);
}

// вывести окно на экран
void window(int num)
{
	int vmode, choice;
	int x, y;

	vmode = video_mode();
	if((vmode != 2) && (vmode != 3) && (vmode != 7)) {
		printf("Видео должно быть в текстовом режиме с 80 столбцами!");	
		exit(1);
	}

	// установить соответствующий адрес видеопамяти
	if(vmode == 7) vid_mem =(char far *) 0xB0000000;
	else vid_mem = (char far *) 0xB0000000;

	// делаем окно активным
	if(!frame[num].active) {
		save_video(num); // сохраняем текущее содержание экрана 
		frame[num].active = 1; // устанавливаем флаг активности
	}

	if(!frame[num].border) draw_border(num);
	display_header(num); // выводим окно на экран
	
	x = frame[num].startx + frame[num].curx + 1;
	y = frame[num].starty + frame[num].cury + 1;
	goto_xy(x,y);
}

// установка курсора в определенную позицию окна
// возвратить 0 при выходе за границу и не 0 в противном случае
int window_xy(int num, int x, int y)
{
	if(x < 0 || x + frame[num].startx >= frame[num].endx - 1)
		return 0;
	if(x < 0 || y + frame[num].starty >= frame[num].endy - 1)
		return 0;
	frame[num].curx = x;
	frame[num].cury = y;
	goto_xy(frame[num].startx + x + 1, frame[num].starty + y + 1);
	
	return 1;
}

// чтение 16-ти битного скан кода клавиши 
int get_key() 
{
	union REGS r;
	r.h.ah = 0;
	return int86(0x16, &r, &r);
}

int bioskey(int c) 
{
	switch (c) {
		case 0 : return get_key();
		case 1 : return 0;///kbhit();
	}
	return 0;
}


// ввести с клавиатуры символ в окно. Возвратить 
// полный 16-ти разрядный скан-код
int window_getche(int num) 
{
	union inkey {
		char ch[2];
		int i;
	} c;

	if(!frame[num].active) return 0; // окно не активное 
	
	window_xy(num, frame[num].curx, frame[num].cury);

	c.i = bioskey(0); // принимаем символ от клавиатуры 

	if(c.ch[0]) {
		switch (c.ch[0]) {
			case '\r' : break; // если нажата клавиша ENTER
			case BKSP : break; // возврат
			default   :
			if(frame[num].cury + frame[num].starty < frame[num].endy - 1) {
				write_char(frame[num].startx = frame[num].curx = 1,
						   frame[num].starty + frame[num].cury + 1, c.ch[0], NORM_VID);
				frame[num].cury++;
			}
		}
		if(frame[num].curx < 0) frame[num].curx = 0;
		if(frame[num].curx + frame[num].startx > frame[num].endx - 2)
			frame[num].curx--;
		window_xy(num, frame[num].curx, frame[num].cury);
	}

	return c.i;
}

// вывести строку начиная с текущей позиции курсора в окне.
// возвратить 0 если окно не активное и 1 если активное.
int window_puts(int num, char *str)
{
	if(!frame[num].active) return 0;

	for(; *str; str++) 
		window_putchar(num, *str);

	return 1;
}


