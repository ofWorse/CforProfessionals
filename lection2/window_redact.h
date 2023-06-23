#include "window_maker.h"

// интерактианое изменение размера окна
void size(int num) 
{
	char ch;
	int x, y, startx, starty;

	// активировать если необходимо
	if(!frame[num].active) window(num);
	startx = x = frame[num].startx;
	starty = y = frame[num].starty;
	window_xy(num, 0, 0);

	do {
		ch = get_special();
		switch(ch) {
			case 75 : starty--; break; // влево
			case 77 : starty++; break; // вправо
			case 72 : startx--; break; // вверх
			case 80 : startx++; break; // вниз
			case 71 : startx--; starty--; break; // влево вверх
			case 73 : startx--; starty++; break; // вправо вверх
			case 79 : startx++; starty--; break; // влево вниз
			case 81 : startx++; starty++; break; // вправо вниз
//			F2: отменить и веррнуться к исходному коду
			case 60 : startx = x; starty = y; ch = 59;
		}

		if(startx < 0) startx++;
		if(startx >= frame[num].endx) startx--;
		if(starty < 0) starty++;
		if(starty >= frame[num].endy) starty--;
		deactivate(num); //стереть окно старого размера
		frame[num].startx = startx;
		frame[num].starty = starty;
		window(num); // вывести окно с новым размером
	} while(ch != 59); // F1 для подтверждения нового размера
	deactivate(num);
}

// интерактивное перемещение окна
void move(int num)
{
	char ch;
	int x,y,ex,ey,startx,starty,endx,endy;

	if(!frame[num].active) window(num);
	startx = x = frame[num].startx;
	starty = y = frame[num].starty;
	endx = ex = frame[num].endx;	
	endy = ey = frame[num].endy;	
	window_xy(num, 0, 0);
	
	do {
		ch = get_special();
		switch (ch) {
			case 75 : starty--; endy--; break; // влево
			case 77 : starty++; endy++; break; // вправо
			case 72 : startx--; endx--; break; // вверх
			case 80 : startx++; endx++; break; // вниз
			case 71 : startx--; starty--;
					  endx--; endy--; break; // влево вверх
			case 73 : startx--; starty++;
					  endx--; endy++; break; // вправо вверх
			case 79 : startx++; starty--;
					  endx++; endy--; break; // влево вниз
			case 81 : startx++; starty++;
					  endx++; endy++; break; // вправо вниз
			case 60 : startx = x; starty = y;
					  endx = ex; endy = ey;
					  ch = 59; break; // F2 ...
		}
		
		if(startx < 0) {
			startx++; endx++;
		}
		if(endx >= 25) {
			startx--; endx--;
		}
		if(starty < 0) {
			starty++; endy++;
		}
		if(endy >= 79) {
			starty--; endy--;
		}

		// стереть окно в старой позиции
		deactivate(num);
		frame[num].startx = startx;
		frame[num].starty = starty;
		frame[num].endx = endx;
		frame[num].endy = endy;
		// вывести окно в новую позицию
		window(num);
	} while(ch != 59);
	deactivate(num);
}

