#include "menu.h"
#include <stdio.h>

void pd_driver(void);
int pulldown(int num);

char *fruit[] = {
	"Яблоко", "Апельсин",
	"Груша", "Грейпфрут",
	"Малина", "Клубника"
};

char *color[] = {
	"Красный", "Желтый",
	"Оранжевый", "Зеленый"
};

char *apple_type[] = {
	"Красный деликатес", 
	"Джонатан", 
	"Белый налив",
	"Антоновка"
};

void cls(void) 
{
	union REGS r;

	r.h.ah = 6; // код прокрутки экрана
	r.h.al = 0; // код очистки экрана
	r.h.ch = 0; // начальная строка 
	r.h.cl = 0; // начальная колонка
	r.h.dh = 24; // конечная строка
	r.h.dl = 79; // конечная колонка
	r.h.bh = 7; // очистка пустой строки 
	int86(0x10, &r, &r);	
}

int main(void) 
{
	int i;
	cls();
	goto_xy(0, 0);
	
	// создаем фреймы меню
	make_menu(0, fruit, "яагрмк", 6, 5, 20, BORDER);
	make_menu(1, color, "кжоз", 4, 9, 28, BORDER);
	make_menu(2, apple_type, "кдба", 4, 12, 32, BORDER);

	printf("Выберите фрукт: ");
	
	pd_driver(); // запуск системы меню
}


void pd_driver(void) 
{
	int choice1, choice2, selection;

	// активация окон по мере надобности 
	while ((choice1 = pulldown(0)) != -1) {
		switch (choice1) {
			case 0 : // яблоко
				     while((choice2 = pulldown(1)) != -1) {
						if(choice2 == 0) selection = pulldown(2); // красное яблоко
					 }
					 restore_video(1);
					 break;
			case 1 : 
			case 2 : goto_xy(1, 0); printf("Неверный выбор!"); break;
			case 3 : // грейпфрут
					 selection = pulldown(3);
					 restore_video(3);
					 break;
			case 4 : 
			case 5 : goto_xy(1, 0); printf("Неверный выбор!"); break;
		}
	}
	restore_video(0);
}

int pulldown(int num)
{
	int vmode, choice;

	vmode = video_mode();

	if((vmode != 2) && (vmode != 3) && (vmode != 7)) {
		printf(" Должен быть 80 символьный текстовый режим!");
		exit(1);
	}

	// присвоим соответствующий адрес видео памяти 
	if(vmode == 7) vid_mem = (char *) 0xB0000000;
	else vid_mem = (char *) 0xB8000000;

	// узнаем активные окна
	if(!frame[num].active) {
		save_video(num);
		frame[num].active = 1;
	}

	if(frame[num].border) draw_border(num);

	display_menu(num);

	return get_resp(num);
}
