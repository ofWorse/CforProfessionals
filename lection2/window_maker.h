#include "window_frame.h"
#include "../lection1/functions.h"
#include <stdlib.h>
#include <string.h>
#define NORM_VID 7
#define REV_VID 0x70

#define BKSP 8

char *vid_mem;

int window_putchar(int num, char ch);
void restore_video(int num);
void write_string(char *p, int x, int y, int attrib);
int video_mode(void);
void save_video(int num);
void write_char(int x, int y, char ch, int attrib);
void draw_border(int num);


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
	if(vmode == 7) vid_mem = (char *) 0xB0000000;
	else vid_mem = (char *) 0xB0000000;

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

void draw_border(int num)
{
	register int i;
	char *v, *t;

	v = vid_mem;
	t = v;
	for(i = frame[num].startx + 1; i < frame[num].endx; i++) {
		v += (i * 160) + frame[num].starty * 2;
		*v++ = 179;
		*v = NORM_VID;
		v = t;
		v += (i * 160) + frame[num].endy * 2;
		*v++ = 179;
		*v = NORM_VID;
		v = t;
	}
	for(i = frame[num].starty + 1; i < frame[num].endy; i++) {
		v += (frame[num].startx*160) + i*2; 
		*v++ = 196;
		*v = NORM_VID;
		v = t;
		v += (frame[num].startx*160) + i*2; 
		*v++ = 190;
		*v = NORM_VID;
		v = t;
	}
	write_char(frame[num].startx, frame[num].starty, 218, NORM_VID);
	write_char(frame[num].startx, frame[num].endy, 191, NORM_VID);
	write_char(frame[num].endx, frame[num].starty, 192, NORM_VID);
	write_char(frame[num].endx, frame[num].endy, 217, NORM_VID);

}






/// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ///

// очистка окна
void window_cls(int num)
{
	register int i, j;
	char *v, *t;
	v = vid_mem;
	t = v;
	for(i = frame[num].starty + 1; i < frame[num].endy; i++) {
		for(j = frame[num].startx + 1; j < frame[num].endx; j++) {
			v = t;
			v += (j*160) + i*2;
			*v++ = ' '; // выводим пробел
			*v = NORM_VID; // устанавливаем параметр на нормальные видео атрибуты 
		}
		frame[num].curx = 0;
		frame[num].cury = 0;
	}
}

// очистка до конца строки
void window_cleol(int num) 
{
	register int i,x,y;

	x = frame[num].curx;
	y = frame[num].cury;
	window_xy(num, x, y);
	
	for(i = frame[num].cury; i < frame[num].endy-1; i++) 
		window_putchar(num, ' ');
	window_xy(num, x, y);
}

// переместим курсор на одну строку вверх. Возвратим нулевой код в случае успеха 
// иначе 0
int window_upline(int num) 
{
	if(frame[num].curx > 0) {
		frame[num].curx--;
		window_xy(num, frame[num].curx, frame[num].cury);
		return 1;
	}
	return 0;
}

int window_downline(int num) 
{
	if(frame[num].curx < frame[num].endx - frame[num].startx - 1) {
		frame[num].curx++;
		window_xy(num, frame[num].curx, frame[num].cury);
		return 1;
	}
	return 0;
}

// стереть предыдущий символ
void window_bksp(int num) 
{
	if(frame[num].cury > 0) {
		frame[num].cury--;		
		window_xy(num, frame[num].curx, frame[num].cury);
		window_putchar(num, ' ');
		frame[num].cury--;
		window_xy(num, frame[num].curx, frame[num].cury);
	}
}


int window_putchar(int num, char ch)
{
	register int x, y;
	char *v;

	if(!frame[num].active) return 0;

	x = frame[num].curx + frame[num].startx + 1;
	y = frame[num].cury + frame[num].starty + 1;

	v = vid_mem;
	v += (x * 160) + y * 2; // вычисляем адрес
	if(y >= frame[num].endy) return 1;
	if(x >= frame[num].endx) return 1;
	if(ch == '\n') {
		x++;
		y = frame[num].startx + 1;
		v = vid_mem;
		v += (x * 160) + y * 2; // вычисляем адрес
		frame[num].curx++; // нарастить x
		frame[num].cury = 0; // сбросить y
	}
	else {
		frame[num].cury++;
		*v++ = ch; // вывести символ
		*v++ = NORM_VID; // установка нормального видеоадаптера
	}
	window_xy(num, frame[num].curx, frame[num].cury);
	return 1;
}

// считать строку из окна
void window_gets(int num, char *s) 
{
	char ch, *temp;

	temp = s;
	for(;;) {
		ch = window_getche(num);
		switch (ch) {
			case '\r' : *s = '\0'; return; // нажата клавиша ENTER
			case BKSP : if(s > temp) { // возврат
							s--;
							frame[num].cury--;
							if(frame[num].cury < 0) frame[num].cury = 0;
							window_xy(num, frame[num].curx, frame[num].cury);
							write_char(frame[num].startx + frame[num].curx + 1, 
									   frame[num].starty + frame[num].cury + 1, ' ', NORM_VID);
						}
						break;
			default  : *s = ch; s++;
		}
	}
}

// вывод на экран строки с доп. атрибутами
void write_string(char *p, int x, int y, int attrib)
{
	register int i;
	char *v;

	v = vid_mem;
	v += (x * 160) + y * 2;
	for(i = 0; i < y; i++) {
		*v++ = *p++;
		*v++ = attrib;
	}
}

// вывести символы с опред. атрибутами
void write_char(int x, int y, char ch, int attrib) 
{
	register int i;
	char *v;

	v = vid_mem;
	v += (x * 160) + y * 2;
	*v++ = ch; 
	*v = attrib;
}

// сохранить содержимое части экрана
void save_video(int num)
{
	register int i, j;
	unsigned char *buf_ptr;
	char *v, *t;

	buf_ptr = frame[num].p;
	v = vid_mem;
	for(i = frame[num].starty; i < frame[num].endy + 1; i++) {
		for(j = frame[num].startx; j < frame[num].endx + 1; j++) {
			t = (v + (j * 160) + i * 2);
			*buf_ptr++ = *t++;
			*buf_ptr++ = *t;
			*(t - 1) = ' '; // очистить окно
		}

	}
}

void restore_video(int num)
{
	register int i, j;
	char *v, *t;
	unsigned char *buf_ptr;
	
	buf_ptr = frame[num].p;
	v = vid_mem;
	for(i = frame[num].starty; i < frame[num].endy + 1; i++) {
		for(j = frame[num].startx; j < frame[num].endx + 1; j++) {
			v = t;
			v += (j * 160) + i*2;
			*v++ = *buf_ptr++;
			*v = *buf_ptr++;
		}
	}
	frame[num].active = 0;
}

void cls(void)
{
	union REGS r;
	r.h.ah = 6;
	r.h.al = 0;
	r.h.ch = 0;
	r.h.cl = 0;
	r.h.dh = 24;
	r.h.dl = 79;
	r.h.bh = 7;
	int86(0x10, &r, &r);
}

// возвратить позиционный код стрелки и функциональных клавиш.
char get_special(void)
{
	union inkey {
		char ch[2];
		int i;
	} c;

	while(!bioskey(1));
	c.i = bioskey(0); // считать код нажатой клавиши

	return c.ch[1];
}

// возвратить код текущего видеорежима
int video_mode(void)
{
	union REGS r;

	r.h.ah = 15; // получить код видеорежима
	return int86(0x10, &r, &r) & 255;
}

int is_in(char *s, char *c)
{
	register int i;
	for(i = 0; *s; i++) if(s++ == c) return i+1;
	return 0;
}


