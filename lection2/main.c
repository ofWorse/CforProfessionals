#include "window_notepad.h"
#define BORDER 1
#define ESC 27 

int main(void)
{
	union inkey {
		char ch[2];
		int i;
	} c;

	int i;
	char ch;

	cls();
	goto_xy(0, 0);

	// делаем рамки окна
	make_window(0, " Editor [Esc to exit] ", 0, 0, 24, 78, BORDER);
	make_window(1, " Decimal to hex ", 7, 40, 10, 70, BORDER);
	make_window(2, " Calculator ", 8, 20, 12, 60, BORDER);
	make_window(3, " Notepad [F1 to exit] ", 5, 20, 17, 60, BORDER);

	// используем window для активации окна
	window(0);
	do {
		c.i = window_getche(0);
		ch = c.i; // использовать только младший байт 
		if(ch == '\r') // должен выполнять переход к началу следующей строки
			window_putchar(0, '\n');

		switch (c.ch[1]) {
			case 59 : 
				window(1);
				for(i = 0; i < 10; i++)
					if(window_xy(1, i, i)) window_putchar(1, 'X');
				getchar();
				deactivate(1);
				break;
			case 60 : 
				size(1);
				move(1);
				break;
			case 61 : 
				calc();
				break;
			case 62 : 
				dectohex();
				break;
			case 63 : 
				notepad();
				break;
			case 72 :
				window_upline(0);
				break;
			case 80 : 
				window_downline(0);
				break;
		}
	} while(ch != ESC);
	deactivate(0);
}
