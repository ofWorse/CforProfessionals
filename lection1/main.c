#include "menu.h"

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
	for(i = 0; i < 25; i++) printf("Это тест исчезающего меню!\n");
	popup(fruit, "яагрмк", 6, 1, 3, BORDER);
	popup(color, "кжоз", 4, 5, 10, BORDER);
	popup(apple_type, "кдба", 4, 10, 18, BORDER);
	return EXIT_SUCCESS;
}
