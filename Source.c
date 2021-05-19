// Для удобного пользования предлагается использовать
// точечные шрифты 16х12

#include <stdio.h>
#include <windows.h>
#include <malloc.h>
#include <random>
#include <time.h>
#include <conio.h>

#define MAP_WIDTH 11 // ширина карты
#define MAP_HEIGHT ( 17 + MAP_BUFFER ) // высота карты
#define MAP_BUFFER 2 // в буффере создаются новые фигуры на карте
#define FRONT_COLOR Black // цвет фона
#define TEXT_COLOR Green // цвет текста
#define CELLS_IN_FIGURE 4 // количество клеток в фигуре
#define SPEED_TIME 70 // время задерки падения фигуры

#define SPECIAL -32 // код специальной клавиши
#define ESC 27
#define LEFT 75
#define DOWN 80
#define RIGHT 77
#define SPACEBAR 32

/* Используется система координат:
 y^
  |
  |
  |
  L-------->
         x
*/

// цвета в консоли
enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

// типы фигур
enum TypeFigure {
	L = 0, J = 1, Z = 2, S = 3, T = 4, G = 5, I = 6
};

// направления движения
enum Direction {
	Left = -1, Down = 0, Right = 1
};

typedef struct 
{
	int color; // цвет клетки
	int x, y; // координаты
} s_cell;

typedef struct
{
	s_cell cell[CELLS_IN_FIGURE]; // каждая фигура состоит из CELLS_IN_FIGURE клеток
	TypeFigure type;
} s_figure;

// задание начальных значений фигуре
s_figure initialise_figure(ConsoleColor color)
{
	s_figure figure;
	figure.type = L;
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
	{
		figure.cell[i].color = color;
		figure.cell[i].x = figure.cell[i].y = -2*MAP_HEIGHT;
	}
	return figure;
}

// ###  
// #
s_figure create_L(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = L;
	for (int i = 0; i < 3; i++)
		figure.cell[i].x = MAP_WIDTH / 2 + i - 1; // ### (x)
	for (int i = 0; i < 3; i++)
		figure.cell[i].y = MAP_HEIGHT - 1; // ### (y)
	figure.cell[3].x = MAP_WIDTH / 2 - 1; // # (x)
	figure.cell[3].y = MAP_HEIGHT - 2; // # (y)
	return figure;
}

// #
// ###  
s_figure create_J(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = J;
	figure.cell[0].x = MAP_WIDTH / 2 - 1; // # (x)
	figure.cell[0].y = MAP_HEIGHT - 1; // # (y)
	for (int i = 1; i < 4; i++)
		figure.cell[i].x = MAP_WIDTH / 2 + i - 2; // ### (x)
	for (int i = 1; i < 4; i++)
		figure.cell[i].y = MAP_HEIGHT - 2; // ### (y)
	return figure;
}

// ##
//  ##  
s_figure create_Z(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = Z;
	figure.cell[0].x = MAP_WIDTH / 2 - 1; // ## (up x1)
	figure.cell[1].x = MAP_WIDTH / 2; // ## (up x2)
	figure.cell[2].x = MAP_WIDTH / 2; // ## (down x1)
	figure.cell[3].x = MAP_WIDTH / 2 + 1; // ## (down x2)
	for (int i = 0; i < 2; i++)
		figure.cell[i * 2].y = figure.cell[i * 2 + 1].y = MAP_HEIGHT - 1 - i; // ## (y)
	return figure;
}

//  ##
// ##  
s_figure create_S(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = S;
	figure.cell[0].x = MAP_WIDTH / 2; // ## (up x1)
	figure.cell[1].x = MAP_WIDTH / 2 + 1; // ## (up x2)
	figure.cell[2].x = MAP_WIDTH / 2 - 1; // ## (down x1)
	figure.cell[3].x = MAP_WIDTH / 2; // ## (down x2)
	for (int i = 0; i < 2; i++)
		figure.cell[i * 2].y = figure.cell[i * 2 + 1].y = MAP_HEIGHT - 1 - i; // ## (y)
	return figure;
}

// ###
//  #  
s_figure create_T(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = T;
	for (int i = 0; i < 3; i++)
	{
		figure.cell[i].x = MAP_WIDTH / 2 + i - 1; // ### (x)
		figure.cell[i].y = MAP_HEIGHT - 1; // ### (y)
	}
	figure.cell[3].x = MAP_WIDTH / 2; // # (x)
	figure.cell[3].y = MAP_HEIGHT - 2; // # (y)
	return figure;
}

// ##
// ##  
s_figure create_G(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = G;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			figure.cell[i * 2 + j].x = MAP_WIDTH / 2 + j - 1; // ##(x)
			figure.cell[i * 2 + j].y = MAP_HEIGHT - 1 - i; // ##(y)
		}
	return figure;
}

// ####
s_figure create_I(ConsoleColor color)
{
	s_figure figure = initialise_figure(color);
	figure.type = I;
	for (int i = 0; i < 4; i++)
	{
		figure.cell[i].x = MAP_WIDTH / 2 + i - 2; // ####(x)
		figure.cell[i].y = MAP_HEIGHT - 1; // ####(y)
	}
	return figure;
}

// вывод на экран одной клетки
void print_cell(s_cell cell)
{
	// подключаем дескриптор стандартного вывода
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// изменяем цвет фона на FRONT_COLOR, цвет текста на cell.color
	SetConsoleTextAttribute(hConsole, (WORD)(FRONT_COLOR << 4 | cell.color));
	printf("%c", char(254));
	// клетка закрашена, возвращаем значения цветов
	SetConsoleTextAttribute(hConsole, (WORD)(FRONT_COLOR << 4 | TEXT_COLOR));
}

// вывод карты на экран
void print_map(s_cell** map)
{
	system("cls"); // очистка экрана
	// верхняя граница
	printf("\n\n  %c", char(201));
	for (int i = 0; i < MAP_WIDTH; i++)
		printf("%c",char(205));
	printf("%c\n", char(187));
	// карта
	for (int y = MAP_HEIGHT - MAP_BUFFER - 1; y >= 0; y--)
	{
		printf("  %c",char(186));
		for (int x = 0; x < MAP_WIDTH; x++)
			print_cell(map[x][y]);
		printf("%c\n", char(186));
	}
	// нижняя граница
	printf("  %c", char(200));
	for (int i = 0; i < MAP_WIDTH; i++)
		printf("%c", char(205));
	printf("%c\n\n", char(188));
}

// создание карты
s_cell** create_map()
{
	s_cell** map = (s_cell**)malloc(sizeof(s_cell*) * MAP_WIDTH);
	if (!map)
		return NULL; // память не выделилась
	for (int i = 0; i < MAP_WIDTH; i++)
		if (!(map[i] = (s_cell*)malloc(sizeof(s_cell) * MAP_HEIGHT)))
		{// если память на i-том шаге не выделилась,
		// очищаем выделенную память до i-того шага
			for (int j = 0; j < i; j++)
				free(map[j]);
			free(map);
			return NULL;
		}
	// задаём начальные данные
	for (int y = MAP_HEIGHT - 1; y >= 0; y--)
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			map[x][y].color = FRONT_COLOR;
			map[x][y].x = x;
			map[x][y].y = y;
		}
	return map;
}

// освобождение памяти карты
s_cell** free_map(s_cell** map)
{
	for (int i = 0; i < MAP_WIDTH; i++)
		free(map[i]);
	free(map);
	return NULL;
}

// размещение фигуры на карте
// если разместить нельзя, вернётся 0
int figure_on_map(s_cell** map, s_figure figure)
{
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure.cell[i].x == -2 * MAP_HEIGHT || figure.cell[i].y == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			if (figure.cell[i].x < 0 || figure.cell[i].x >= MAP_WIDTH ||
				figure.cell[i].y < 0 || figure.cell[i].y >= MAP_HEIGHT ||
				map[figure.cell[i].x][figure.cell[i].y].color != FRONT_COLOR)
				return 0; // клетка занята или не существует
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure.cell[i].x == -2 * MAP_HEIGHT || figure.cell[i].y == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			map[figure.cell[i].x][figure.cell[i].y] = figure.cell[i];
	return 1;
}

// преобразование типа функции для создания массива
typedef s_figure (*FuncFigureType)(ConsoleColor);

// создание новой случайной фигуры случайного цвета
s_figure new_figure()
{
	// массив цветов фигур
	ConsoleColor figure_color[8] = { Brown, LightBlue, LightCyan, LightGray, LightMagenta, LightRed, Yellow, White };
	// каждый элемент массива является функцией,
	// которая возвращает фигуру заданного типа
	FuncFigureType figure[7] = { create_L, create_J, create_Z, create_S, create_T, create_G, create_I };
	// вызов функции create_* от случайного цвета 
	return figure[rand() % 7](figure_color[rand() % 8]);
}

// если переместить фигуру влево нельзя, вернётся 0
int can_move_left(s_cell** map, s_figure* figure)
{
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			if (figure->cell[i].x < 1)
				return 0; // край карты
			else
			{
				int j;
				for (j = 0; j < CELLS_IN_FIGURE; j++)
					if (i != j && figure->cell[i].x - 1 == figure->cell[j].x && figure->cell[i].y == figure->cell[j].y)
						break; // переход в другую клетку этой же фигуры
				if (j < CELLS_IN_FIGURE)
					continue;
				if (map[figure->cell[i].x - 1][figure->cell[i].y].color != FRONT_COLOR)
					return 0; // клетка занята
			}
	return 1; // все клетки фигуры могут перейти влево
}

// если переместить фигуру вправо нельзя, вернётся 0
int can_move_right(s_cell** map, s_figure* figure)
{
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигуа завершена
		else
			if (figure->cell[i].x > MAP_WIDTH - 2)
				return 0; // край карты
			else
			{
				int j;
				for (j = 0; j < CELLS_IN_FIGURE; j++)
					if (i != j && figure->cell[i].x + 1 == figure->cell[j].x && figure->cell[i].y == figure->cell[j].y)
						break; // переход в  другую клетку этой же фигуры
				if (j < CELLS_IN_FIGURE)
					continue;
				if (map[figure->cell[i].x + 1][figure->cell[i].y].color != FRONT_COLOR)
					return 0; // клетка занята
			}
	return 1; // все клетки фигуры могут перейти вправо
}

// если переместить фигуру вниз нельзя, вернётся 0
int can_move_down(s_cell** map, s_figure* figure)
{
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			if (figure->cell[i].y < 1)
				return 0; // край карты
			else
			{
				int j = 0;
				for (j = 0; j < CELLS_IN_FIGURE; j++)
					if (i != j && figure->cell[i].y - 1 == figure->cell[j].y && figure->cell[i].x == figure->cell[j].x)
						break; // переход в другую клетку этой же фигуры
				if (j < CELLS_IN_FIGURE)
					continue;
				if (map[figure->cell[i].x][figure->cell[i].y - 1].color != FRONT_COLOR)
					return 0; // клетка занята
			}
	return 1; // все клетки фигуры могут перейти вниз
}

// перемещение фигуры влево
// если переместить нельзя, вернётся 0
int move_left(s_cell** map, s_figure* figure)
{
	if(!can_move_left(map, figure))
		return 0;
	// убираем фигуру с карты
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// ставим фигуру с новыми координатами
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
		{
			figure->cell[i].x--;
			map[figure->cell[i].x][figure->cell[i].y].color = figure->cell[0].color;
		}
	return 1;
}

// перемещение фигуры вправо
// если переместить нельзя, вернётся 0
int move_right(s_cell** map, s_figure* figure)
{
	if (!can_move_right(map, figure))
		return 0;
	// убираем фигуру с карты
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// ставим фигуру с новыми координатами
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break;
		else
		{
			figure->cell[i].x++;
			map[figure->cell[i].x][figure->cell[i].y].color = figure->cell[0].color;
		}
	return 1;
}

// перемещение фигуры вниз
// если переместить нельзя, вернётся 0
int move_down(s_cell** map, s_figure* figure)
{
	if (!can_move_down(map, figure))
		return 0;
	// убираем фигуру с карты
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break; // фигура завершена
		else
			map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// ставим фигуру с новыми координатами
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure->cell[i].x == -2 * MAP_HEIGHT)
			break;
		else
		{
			figure->cell[i].y--;
			map[figure->cell[i].x][figure->cell[i].y].color = figure->cell[0].color;
		}
	return 1;
}

// перемещение фигуры в зданном направлении
// если переместить нельзя, вернётся 0
int move(s_cell** map, s_figure* figure, Direction d)
{
	switch (d)
	{
	case Left: return move_left(map, figure);
	case Down: return move_down(map, figure);
	case Right: return move_right(map, figure);
	default: return 1;
	}
	return 0;
}

// поворот L
int rotate_L(s_cell** map, s_figure* figure)
{
	// изменение координат клеток относительно клетки 1
	int dx[4] = { 0, };
	int dy[4] = { 0, };
	// изменение координат клетки 0
	dx[0] = (figure->cell[1].y - figure->cell[0].y) - (figure->cell[1].x - figure->cell[0].x);
	dy[0] = -(figure->cell[1].y - figure->cell[0].y) - (figure->cell[1].x - figure->cell[0].x);
	// клетка 1 остаётся на месте
	// изменение координат клетки 2
	dx[2] = -dx[0];
	dy[2] = -dy[0];
	// изменение координат клетки 3
	dx[3] = (figure->cell[1].y - figure->cell[3].y) - (figure->cell[1].x - figure->cell[3].x);
	dy[3] = -(figure->cell[1].y - figure->cell[3].y) - (figure->cell[1].x - figure->cell[3].x);
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	for (int i = 0; i < 4; i++)
	{
		new_figure.cell[i].x += -dx[i];
		new_figure.cell[i].y += -dy[i];
	}
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот J
int rotate_J(s_cell** map, s_figure* figure)
{
	// изменение координат клеток относительно клетки 2
 	int dx[4] = { 0, };
	int dy[4] = { 0, };
	// изменение координат клетки 0
	dx[0] = (figure->cell[2].y - figure->cell[0].y) - (figure->cell[2].x - figure->cell[0].x);
	dy[0] = -((figure->cell[2].y - figure->cell[0].y) + (figure->cell[2].x - figure->cell[0].x));
	// изменение координат клетки 1
	dx[1] = (figure->cell[2].y - figure->cell[1].y) - (figure->cell[2].x - figure->cell[1].x);
	dy[1] = -(figure->cell[2].y - figure->cell[1].y) - (figure->cell[2].x - figure->cell[1].x);
	// клетка 2 остаётся на месте
	// изменение координат клетки 3
	dx[3] = -dx[1];
	dy[3] = -dy[1];
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	for (int i = 0; i < 4; i++)
	{
		new_figure.cell[i].x += -dx[i];
		new_figure.cell[i].y += -dy[i];
	}
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот Z
int rotate_Z(s_cell** map, s_figure* figure)
{
	// изменение координат клеток относительно клетки 1
	int dx[4] = { 0, };
	int dy[4] = { 0, };
	// изменение координат клетки 0
	dx[0] = -(figure->cell[1].y - figure->cell[0].y) - (figure->cell[1].x - figure->cell[0].x);
	dy[0] = dx[0];
	// клетка 1 остаётся на месте
	// изменение координат клетки 2
	dx[2] = -dx[0];
	dy[2] = dx[0];
	// изменение координат клетки 3
	dx[3] = 2 * dx[2];
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	for (int i = 0; i < 4; i++)
	{
		new_figure.cell[i].x += -dx[i];
		new_figure.cell[i].y += -dy[i];
	}
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот S
int rotate_S(s_cell** map, s_figure* figure)
{
	// изменение координат клеток относительно клетки 0
	int dx[4] = { 0, };
	int dy[4] = { 0, };
	// клетка 0 остаётся на месте
	// изменение координат клетки 1
	dx[1] = -(figure->cell[0].y - figure->cell[1].y) - (figure->cell[0].x - figure->cell[1].x);
	dy[1] = dx[1];
	// изменение координат клетки 2
	dy[2] = -2 * dx[1];
	// изменение координат клетки 3
	dx[3] = dx[1];
	dy[3] = -dx[1];
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	for (int i = 0; i < 4; i++)
	{
		new_figure.cell[i].x += -dx[i];
		new_figure.cell[i].y += -dy[i];
	}
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот T
int rotate_T(s_cell** map, s_figure* figure)
{
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	new_figure.cell[2] = figure->cell[3];
	new_figure.cell[3] = figure->cell[0];
	new_figure.cell[0].x = 2 * new_figure.cell[1].x - new_figure.cell[2].x;
	new_figure.cell[0].y = 2 * new_figure.cell[1].y - new_figure.cell[2].y;
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот I
int rotate_I(s_cell** map, s_figure* figure)
{
	int dx = figure->cell[1].x - figure->cell[0].x; // 1, если ####
	int dy = figure->cell[0].y - figure->cell[1].y; // 0, если ####
	int d = dy - dx; // разница используется чаще
	// убирание текущей фигуры
	for (int i = 0; i < 4; i++)
		map[figure->cell[i].x][figure->cell[i].y].color = FRONT_COLOR;
	// новая фигура, полученная поворотом исходной
	s_figure new_figure = *figure;
	// изменение координат клетки 0
	new_figure.cell[0].x += -d;
	new_figure.cell[0].y += -d;
	// клетка 1 остаётся на месте
	// изменение координат клетки 2
	new_figure.cell[2].x += d;
	new_figure.cell[2].y += d;
	// изменение координат клетки 3
	new_figure.cell[3].x += 2*d;
	new_figure.cell[3].y += 2*d;
	// если нельзя разместить повёрнутую фигуру,
	// размещаем старую и возвращаем 0
	if (!figure_on_map(map, new_figure))
		return !figure_on_map(map, *figure);
	*figure = new_figure;
	return 1;
}

// поворот фигуры по часовой стрелке
// если повернуть нельзя, вернётся 0
int rotate(s_cell** map, s_figure* figure)
{
	switch (figure->type)
	{
	case L: return rotate_L(map, figure);
	case J: return rotate_J(map, figure);
	case Z: return rotate_Z(map, figure);
	case S: return rotate_S(map, figure);
	case T: return rotate_T(map, figure);
	case G: return 1; // поворот не требуется
	case I: return rotate_I(map, figure);
	default: return 0;
	}
	return 0;
}

// выполнение действия по нажатой клавише
int action(s_cell** map, s_figure* figure, int score)
{
	char bottom = _getch();
	if (bottom == SPECIAL)
		bottom = _getch();
	switch (bottom)
	{
	case ESC: return 0; // выход из игры
	case DOWN: return 1; // фигура опускается без задежки времени
	case LEFT: // сдвиг фигуры влево
		move(map, figure, Left);
		break;
	case RIGHT: // сдвигг фигуры вправо
		move(map, figure, Right);
		break;
	case SPACEBAR: // поворот фигуры
		rotate(map, figure);
		break;
	default: // клавиша без назначенного действия
		break;
	}
	Sleep(SPEED_TIME);
	print_map(map);
	printf("  Total score: %d", score);
	return 1;
}

// смещение клеток, находящихся выше убранной линии,
// вниз на 1 ряд
void falling_cells(s_cell** map, int line, int score)
{
	int flag = 1; // есть ли клетки, подлежащие смещению вниз
	for (int y = line; y < MAP_HEIGHT - 1; y++)
		if (flag) // на линии y есть клетки для смещения вниз
		{
			print_map(map);
			printf("  Total score: %d", score);
			Sleep(70); // не зависит от скорости падения фигуры
			flag = 0;
			// сдвиг линии выше на текущую
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (map[x][y + 1].color != FRONT_COLOR)
					flag = 1;// на линии y+1 есть клетки для смещения вниз
				map[x][y].color = map[x][y + 1].color;
				map[x][y+1].color = FRONT_COLOR;
			}
		}
}

// является ли линия полной
int is_full_line(s_cell** map, int y)
{
	for (int x = 0; x < MAP_WIDTH; x++)
		if (map[x][y].color == FRONT_COLOR)
			return 0; // в линии есть незаполненная клетка
	return 1; // линия заполнена
}

// убирание рядов
// за каждую клетку начисляется одно очко
// каждый дополнительный ряд удваивает его стоимость
int delete_lines(s_cell** map, s_figure figure, int score)
{
	int line = MAP_HEIGHT; // линия, с которой начнётся удаление
	int multiplicate = 1; // множитель стоимости ряда
	// находим номер линии нижней клетки фигуры
	for (int i = 0; i < CELLS_IN_FIGURE; i++)
		if (figure.cell[i].x == -2 * MAP_HEIGHT)
			break;
		else
			if (figure.cell[i].y < line)
				line = figure.cell[i].y;
	// одна фигура может заполнить максимум CELLS_IN_FIGURE линий
	for (int i = 0; i < CELLS_IN_FIGURE && line < MAP_HEIGHT; i++)
	{
		// если линия полна, убираем её
		if (is_full_line(map, line))
		{
			for (int x = 0; x < MAP_WIDTH; x++)
				map[x][line].color = FRONT_COLOR;
			// сдвигаем все линии выше
			falling_cells(map, line, score);
			score += MAP_WIDTH*multiplicate;
			multiplicate *= 2; // цена следующего ряда возрастает в два раза
		}
		else // переходим к следующей линии
			line++;
	}
	return score;
}

// процесс игры
// результатом является количество убранных блоков
int process(s_cell** map, s_figure* figure)
{
	int score = 0;
	// пока есть место для новой фигуры
	while (figure_on_map(map, *figure))
	{
		do // фигура опускается вниз
		{
			print_map(map);
			printf("  Total score: %d", score);
			// за одно нажатие можно сместить фигуру на 3 шага
			for (int i = 0; i < 3; i++)
				if (_kbhit()) // нажата клавиша
					if (!action(map, figure, score))
						return -score;
					else;
				else // или не сместить
					Sleep(SPEED_TIME);
		} while (move(map, figure, Down));
		score = delete_lines(map, *figure, score);
		*figure = new_figure();
	}
	return score;
}

void main()
{
	srand(time(NULL)); // точка отсчёта для генерации случайных чисел
	system("color 02"); // цвет экрана - чёрный, цвет текста - зелёный
	s_cell** map = create_map(); // создание карты
	if (!map) return; // память не выделилась
	s_figure figure = new_figure(); // создание фигуры
	int score = process(map, &figure); // процесс игры
	printf("\n GAME OVER\n\nYou have %d scores\n\n", score);
	map = free_map(map); // освобождение памяти
	system("pause"); // задержка консоли
	return;
}