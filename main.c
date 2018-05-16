#include <simple2d.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define TILE_SIZE 32
#define N_ROWS 18
#define N_COLUMNS 20
#define WINDOW_WIDTH (TILE_SIZE * N_COLUMNS)
#define WINDOW_HEIGHT (TILE_SIZE * N_ROWS)
#define MAX_BODY_LENGTH ((N_COLUMNS * N_ROWS) / 2)
#define ORIG_MOVE_DURATION 15
#define MIN_MOVE_DURATION 5


//--------------------------------------------------------Structs
typedef struct __type_Point {
	int x, y;
} Point;

typedef struct __type_Snake {
	Point dir;
	Point head;
	Point body[MAX_BODY_LENGTH];
	int bodyLength;
} Snake;

//------------------------------------------Function Declarations

void drawSquare(Point pos, float r, float g, float b);
void moveFood(const Snake* snake);
Snake makeSnake(void);
int moveSnake(Snake* snake);

//---------------------------------------------------------------Static

int buffer = 0;
float move_duration = ORIG_MOVE_DURATION;
Snake snake;
Point food;

//-----------------------------------------------------------Functions

Snake makeSnake() {
	
	return (Snake) {
		.dir = {1, 0},
		.head = {4, 0},
		.body = {
			{3, 0}, {2, 0},
		}, 
		.bodyLength = 2,
	};
}

void drawSquare(Point point, float r, float g, float b) {
	
	Point pixel = (Point) {point.x * TILE_SIZE, point.y * TILE_SIZE};
	
	S2D_DrawQuad(
		pixel.x,             pixel.y,             r, g, b, 1,
		pixel.x + TILE_SIZE, pixel.y,             r, g, b, 1,
		pixel.x + TILE_SIZE, pixel.y + TILE_SIZE, r, g, b, 1,
		pixel.x,             pixel.y + TILE_SIZE, r, g, b, 1
	);
}

int snakeContainsPoint(const Snake* snake, Point point) {      

	if(point.x == snake->head.x && point.y == snake->head.y)
		return 1;
	for(int i = 0; i < snake->bodyLength; i++) {
		if(point.x == snake->body[i].x && point.y == snake->body[i].y)
			return 1;
	}
	return 0;
}

void moveFood(const Snake* snake) {

	while(1) {
		Point point = {
			.x = rand() % N_COLUMNS,
			.y = rand() % N_ROWS,
		};
		if(snakeContainsPoint(snake, point))
			continue;
		food = point;
		return;
	}		
}
 
int moveSnake(Snake* snake) {
	
	Point pos = snake->head;
	Point dir = snake->dir;
	Point next = (Point) {
		.x = pos.x + dir.x,
		.y = pos.y + dir.y,
	};
	Point *body = snake->body;

	if(next.x < 0 || next.x >= N_COLUMNS ||
	   next.y < 0 || next.y >= N_ROWS ||
	   snakeContainsPoint(snake, next))
		return 0;

	if(next.x == food.x && next.y == food.y) {
		moveFood(snake);
		if(ORIG_MOVE_DURATION > MIN_MOVE_DURATION)
			move_duration -= 0.4;
		if(snake->bodyLength < MAX_BODY_LENGTH)
			snake->bodyLength++;
	}	

	for(int i = snake->bodyLength - 1; i > 0; i--)
		body[i] = body[i - 1];
	body[0] = snake->head;
	snake->head = next;	

	buffer = 0;
	return 1;
}

//---------------------------------------------------------------Core Functions

void render(void) {

	for (int x = 0; x < N_COLUMNS; x++) {
		for (int y = 0; y < N_ROWS; y++)			
			drawSquare(
				(Point) {x, y},
				0.2,
				0.3 + (x + y) % 2 * 0.1,
				0.5
			);
	}
	
	drawSquare(snake.head, 0.9, 0.6, 0.3);
	for(int i = 0; i < snake.bodyLength; i++)
		drawSquare(snake.body[i], 0.9, 0.6, 0.3);
	
	drawSquare(food, 1.0, 1.0, 1.0);
}

void update(void) {

	if(buffer++ < move_duration)
		return;
	moveSnake(&snake);
} 

void resetGame(void) {
	
	move_duration = ORIG_MOVE_DURATION;
	snake = makeSnake();
	moveFood(&snake);
}

void on_key(S2D_Event e) {

	if(e.type != S2D_KEY_DOWN || strlen(e.key) > 1)
		return;
	
	switch(e.key[0]) {
	case 'W':
		if(snake.dir.y)
			return;
		snake.dir = (Point){0, -1};
		break;
	case 'S':
		if(snake.dir.y)
			return;
		snake.dir = (Point){0, 1};
		break;
	case 'A':
		if(snake.dir.x)
			return;
		snake.dir = (Point){-1, 0};
		break;
	case 'D':
		if(snake.dir.x)
			return;
		snake.dir = (Point){1, 0};
		break;
	case 'R':
		resetGame();
		break;
	}
	moveSnake(&snake);
}

int init() {
	srand((unsigned int)time(NULL)); // do this only once
	resetGame();

	return 1;
}

#ifdef WINDOWS
int WinMain(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	if(!init())
		return -1;

	S2D_Window *window = S2D_CreateWindow(
		"SimpleSnake", WINDOW_WIDTH, WINDOW_HEIGHT, update, render, 0
	);

	window->on_key = on_key;

	S2D_Show(window);
	S2D_FreeWindow(window);

	return 0;
}
