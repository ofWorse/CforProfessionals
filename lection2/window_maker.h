#include "functions.h"
#include <stdlib.h>
#include <string.h>

#define NORM_VID 7
#define REV_VID 0x70
#define BKSP 8

int window_putchar(int num, char ch);
void restore_video(int num);
void write_string(int x, int y, char *p, int attrib);
int video_mode(void);
void save_video(int num);
void write_char(int x, int y, char ch, int attrib);
void draw_border(int num);
int make_window(int num, char *header, int startx, int starty, int endx, int endy, int border);
// функция деактивации окна
void deactivate(int num);
void display_header(int num);
// вывести окно на экран
void window(int num);
// установка курсора в определенную позицию окна
// возвратить 0 при выходе за границу и не 0 в противном случае
int window_xy(int num, int x, int y);
// чтение 16-ти битного скан кода клавиши 
int get_key();
// ввести с клавиатуры символ в окно. Возвратить 
// полный 16-ти разрядный скан-код
int window_getche(int num);
// вывести строку начиная с текущей позиции курсора в окне.
// возвратить 0 если окно не активное и 1 если активное.
int window_puts(int num, char *str);
void draw_border(int num);
// очистка окна
void window_cls(int num);
// очистка до конца строки
void window_cleol(int num);
// переместим курсор на одну строку вверх. Возвратим нулевой код в случае успеха 
// иначе 0
int window_upline(int num);
int window_downline(int num);
// стереть предыдущий символ
void window_bksp(int num);
int window_putchar(int num, char ch);
// считать строку из окна
void window_gets(int num, char *s);
// вывод на экран строки с доп. атрибутами
void write_string(int x, int y, char *p, int attrib);
// вывести символы с опред. атрибутами
void write_char(int x, int y, char ch, int attrib);
// сохранить содержимое части экрана
void save_video(int num);
void restore_video(int num);
void cls(void);
// возвратить позиционный код стрелки и функциональных клавиш.
char get_special(void);
// возвратить код текущего видеорежима
int video_mode(void);
int is_in(char *s, char *c);


