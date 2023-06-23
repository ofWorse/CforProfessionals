#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BORDER   1
#define ESC      27
#define REV_VID  0x70
#define NORM_VID 7

// высветить исчезающее меню и возвратить набор
//       текст меню     горячие кл. число режимов. коорд. л.в.у.  Если 0 то без рамки 
int popup(char *menu[], char *keys, int count, int x, int y, int border);

// высвечивание меню на своем месте
void display_menu(char *menu[], int x, int y, int count) 
{
	int i;

	for(i = 0; i < count; i++, x++) {
		goto_xy(x, y);
		printf("%s", menu[i]);
	}
}

// Высвечивание рамки 
void draw_border(int startx, int starty, int endx, int endy)  
{
	int i;

	for(i = startx+1; i < endx; i++) {
		goto_xy(i, starty);
		putchar(179);
		goto_xy(i, endy);
		putchar(179);
	}
	for(i = starty+1; i < endy; i++) {
		goto_xy(startx, i);
		putchar(196);
		goto_xy(endx, i);
		putchar(196);
	}

	goto_xy(startx, starty);   putchar(218);
	goto_xy(startx, endy  );   putchar(191);
	goto_xy(endx  , starty);   putchar(192);
	goto_xy(endx  , endy  );   putchar(217);
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

// вывод строки с определенным атрибутом 
void write_video(int x, int y, char *p, int attrib) 
{
	union REGS r;
	int i, j;

	for(i = y; *p; i++) {
		goto_xy(x, i);
		r.h.ah = 9; // функция записи символа
		r.h.bh = 0; // видео страница
		r.x.cx = 1; // число повторений символа
		r.h.al = *p++; // символ
		r.h.bl = attrib; // атрибут
		int86(0x10, &r, &r);
	}
}

// возвращает позицию горячей клавиши в строке. Если пользователь 
// нажал не ключевую клавишу, то is_in возвращает 0.

int is_in(char *s, char c) 
{
	int i;

	for(i = 0; *s; i++) 
		if(*s++ == c ) return i+1;
	return 0;
}


// Ввести выбор пользователя
int get_resp(int x, int y, int count, char *menu[], char* keys) 
{
	union inkey {
		char ch[2];
		int i;
	} c;

	int arrow_choice = 0, key_choice;
	
	y++;
	
	// осветить первый выбор 
	goto_xy(x, y);
	write_video(x, y, menu[0], REV_VID);

	for(;;) {
		while(!bioskey(1)); // ждать нажатия
		c.i = bioskey(0);
	}

	// вернем в нормальный режим
	goto_xy(arrow_choice, y);
	write_video(x+arrow_choice, y, menu[arrow_choice], NORM_VID);

	if(c.ch[0]) { // обычная клавиша
		key_choice = is_in(keys, c.ch[0]);
		if(key_choice) return key_choice-1;

		switch (c.ch[0]) {
			case '\r' : return arrow_choice;
			case ' '  : arrow_choice++;
						break;
			case ESC  : return -1; // выйти
		}
	}
	else { // специальная клавиша 
		switch (c.ch[1]) {
			case 72 : arrow_choice--; // стрелка вниз
					  break;
			case 80 : arrow_choice++; // стрелка вверх
					  break;
		}
	}

	if(arrow_choice == count) arrow_choice = 0;
	if(arrow_choice < 0)      arrow_choice = count - 1;
	
	// подсветить выбранную опцию
	goto_xy(x+arrow_choice, y);
	write_video(x+arrow_choice, y, menu[arrow_choice], REV_VID);
}

/* Вывести исчезающее меню и вернуть выбор.
 * возвращает -2, если меню не может быть создано,
 * возвращает -1, если пользователь нажал клавишу ESC,
 * в остальных случаях она возвращает номер выбранной
 * альтернативы, начиная с 0 
*/
int popup(char *menu[], char *keys, int count, int x, int y, int border)
{
	int i, len;
	int endx, endy, choice;
	unsigned int *p;

	if((x > 24) || (x < 0) || (y > 79) || (y < 0)) {
		printf("%s", "Выход за пределы экрана!");
		return -2;
	}

	// вычисление размеров
	len = 0;
	for(i = 0; i < count; i++) 
		if(strlen(menu[i]) > len) len = strlen(menu[i]);
	endy = len + 2 + y;
	endx = count + 1 + x;
	if((endx+1 > 24) || (endy+1 > 79)) {
		printf("%s", "Выход за пределы экрана!");
		return -2;
	}

	// размещение памяти для видео буфера.
	p = (unsigned int *) malloc((endx - x + 1) * (endy - y + 1));
	if(!p) exit(1);

	save_video(x, endx, y, endy, p);

	if(border) draw_border(x, y, endx, endy);

	display_menu(menu, x, y, count);

	choice = get_resp(x, y, count, menu, keys);

	restore_video(x, endx, y, endy, p);
	free(p);
	return choice;
}
