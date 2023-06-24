#include <stdio.h>
#include "stdlib.h"
/*
 * Функция прерывания для достпупа к экрану. 
 * Прерывание 16 имеет несколько режимов, выбор
 * которых выполняется по значению регистра AH.
 * Если функция возвращает значение, то оно 
 * присваивается регистру AL.*/

struct WORDEGS {
	unsigned int ax, bx, cx, dx, si, di, cflag, flags;
};

struct BYTREGS {
	unsigned char al, ah, bl, bh, cl, ch, dl, dh;
};

union REGS {
	struct WORDEGS x;
	struct BYTREGS h;
};

int int86(int num, union REGS *inregs, union REGS *outers);
//int num; // номер прерывания 
//union REGS *inregs; // входные значения регистров
//union REGS *outers; // выхожные значения регистров

// устновка курсора в x, y;
void goto_xy(int x, int y)
{
	union REGS r;

	r.h.ah = 2; // функция установки курсора
	r.h.dl = y; // координата колонки
	r.h.dh = x; // координата строки
	r.h.bh = 0; // видео страница
	int86(0x10, &r, &r);
}

/*
// Функция save_video считывает часть экрана, сохраняет информацию
// в буфер и очищает эту чать экрана.
void save_video(int startx, int endx, int starty, int endy, unsigned *buf_ptr) 
{
	union REGS r;
	int i, j;

	for(i = starty; i < endy; i++) {
		for(j = startx; j < endx; j++) {
			goto_xy(j, i);
			r.h.ah = 8; // функция чтения символа
			r.h.bh = 0; // видео страница
			*buf_ptr++ = int86(0x10, &r, &r);
			putchar(' '); // очистка экрана
		}
	}
}
*/

/*
// восстановление части экрана 
void restore_video(int startx, int endx, int starty, int endy, unsigned *buf_ptr) 
{
	union REGS r;
	int i, j;

	for(i = starty; i < endy; i++) {
		for(j = startx; j < endx; j++) {
			goto_xy(j, i);
			r.h.ah = 9; // функция записи символа 
			r.h.bh = 0; // видео страница
			r.x.cx = 1; // число повторений символа 
			r.h.al = *buf_ptr++; // символ
			r.h.bl = *buf_ptr++; // атрибут
								 //
			*buf_ptr++ = int86(0x10, &r, &r);
		}
	}
}

*/

