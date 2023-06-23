#include <stdio.h>
#include "window_maker.h"
#define MAX 100

int push(int i);
int pop();


// десятично-шестнадцатиричный преобразователь
void dectohex(void)
{
	char in[80], out[80];
	int n;

	window(1);
	do {
		window_xy(1, 0, 0); // переход к первой строке
		window_cleol(1); // очистить строку
						 //
		window_puts(1, "dec: "); // prompter
		window_gets(1, in); // считать число
		window_putchar(1, '\n'); // перейти к следующей строке
		window_cleol(1); // очистить ее
		sscanf(in, "%d", &n); // преобразовать во внутренний формат
		sprintf(out, "%s%X", "hex: ", n); // преобразовать в шестнадцатиричное значение
		window_puts(1, out); // вывести шестнадцатиричное число
	} while(*in);
	deactivate(1);
}

int *p; // указатель стека
int *tos; // вершина стека
int *bos; // дно стека

// сам постфиксный стековый калькулятор...
void calc(void)
{
	char in[80], out[80];
	int answer, stack[MAX];
	int a, b;

	p = stack;
	tos = p;
	bos = p + MAX - 1;
	
	window(2);
	do {
		window_xy(2, 0, 0);
		window_cleol(2);
		window_puts(2, ">>> "); // промптер калькулятора
		window_gets(2, in);

		window_puts(2, "\n ");
		window_cleol(2);
		switch(*in) {
			case '+' : a = pop(); b = pop(); answer = a + b; push(answer); break;
			case '-' : a = pop(); b = pop(); answer = b - a; push(answer); break;
			case '*' : a = pop(); b = pop(); answer = b * a; push(answer); break;
			case '/' : a = pop(); b = pop(); if(a==0) { window_putch("Делить на 0 нельзя!"); break; }
					   answer = b/a; push(answer); break;
			default  : push(atoi(in)); continue;
		}
		sprintf(out, "%d", answer);
		window_puts(2, out);
	} while(*in);
	deactivate(2);
}

int push(int i)
{
	if(p > bos) return 0;

	*p = i;
	p++;
	return 1;
}

int pop(void)
{
	p--;
	if(p < tos) { p++; return 0; }
	return *p;
}

