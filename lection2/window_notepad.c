#include "window_notepad.h"

char notes[MAX_NOTE][80];

void notepad(void)
{
	static int first = 1;
	register int i, j;
	union inkey {
		char ch[2];
		int i;
	} c;
	char ch;

	// опциональная инициализация массива записей
	if(first) {
		for(i = 0; i < MAX_NOTE; i++) 
			*notes[i] = '\0';
		first = !first;
	}

	window(3);
	
	// выведем на экран существующие записи
	for(i = 0; i < MAX_NOTE; i++) {
		if(*notes[i]) window_puts(3, notes[i]);
		window_putchar(3, '\n');
	}

	i = 0;
	window_xy(3, 0, 0);
	
	for(;;) {
		c.i = window_getche(3); // считываем символ, вводимый с клавиатуры

		if(tolower(c.ch[1]) == 59) { // F1 - завершение
			deactivate(3);
			break;
		}
	}

	// если символ обычный, то вводим его в запись
	if(isprint(c.ch[0] || c.ch[0] == BKSP)) {
		window_cleol(3);
		notes[i][0] = c.ch[0];
		j = 1;
		window_putchar(3, notes[i][0]);
		do {
			ch = window_getche(3);
			if(ch == BKSP) {
				if(j > 0) {
					j--;
					window_bksp(3);
				}
			}
			else {
				notes[i][j] = ch;
				j++;
			}
		} while(notes[i][j-1] != '\r');
		notes[i][j-1] = '\0';
		i++;
		window_putchar(3, '\n');
	}
	else { // это специальная клавиша 
		switch (c.ch[1]) {
			case 72 : if(i > 0) { i--; window_upline(3); } break; // стрелка вверх
			case 80 : if(i < MAX_NOTE-1) { i++; window_downline(3); } break; // стрелка вниз 
		}
	}

}
