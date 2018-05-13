#include <simple2d.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#define TILE_SIZE 32
#define FOOD_SIZE TILE_SIZE
#define N_ROWS 18
#define N_COLUMNS 20 
#define WINDOW_WIDTH TILE_SIZE * N_COLUMNS
#define WINDOW_HEIGHT TILE_SIZE * N_ROWS
#define nTiles (WINDOW_WIDTH * WINDOW_HEIGHT / (TILE_SIZE * TILE_SIZE))
#define MAX_BODY_LENGTH 30


//--------------------------------------------------------Structs
typedef struct Points {
	int x, y;
}Point;

typedef struct Squares {
	Point pos;
	Point lastPos;
	void(*draw) (struct Squares*);

}Square;

typedef struct tHead {
	Point dir;
	Square headPart;
	Square* body;
	int bodyLength;
	void(*move) (struct tHead*, S2D_Event* e);
}Head;

typedef struct Foods {
	Point pos;
	void(*draw) (struct Foods*);
}Food;

//------------------------------------------Predefined functions
void Squares_draw(Square* square);
void Foods_draw(Food* food);
void head_move(Head* head, S2D_Event* e);
void updateBody(Square* body);
void updateTiles(Point pos, Point nextP);
void addPartToBody();
//---------------------------------------------------------------Static

Square body[MAX_BODY_LENGTH];
int** tiles;
int buffer = 0;
float snake_speed = 15;

Head head = {
	1, 0,
	{ 4 * TILE_SIZE, 0, 4 * TILE_SIZE, 0, Squares_draw },
	body,
	2,
	head_move
};

Food food = {
	TILE_SIZE * 10, TILE_SIZE * 10,
	Foods_draw
};

//-----------------------------------------------------------Functions

void Squares_draw(Square* square) {

	int x = square->pos.x;
	int y = square->pos.y;

	S2D_DrawQuad(x, y, 0.9, 0.6, 0.3, 1,
		x + TILE_SIZE, y, 0.9, 0.6, 0.3, 1,
		x + TILE_SIZE, y + TILE_SIZE, 0.9, 0.6, 0.3, 1,
		x, y + TILE_SIZE, 0.9, 0.6, 0.3, 1);

}

void Foods_draw(Food* food) {

	int x = food->pos.x;
	int y = food->pos.y;

	S2D_DrawQuad(x, y, 1, 1, 1, 1,
		x + FOOD_SIZE, y, 1, 1, 1, 1,
		x + FOOD_SIZE, y + FOOD_SIZE, 1, 1, 1, 1,
		x, y + FOOD_SIZE, 1, 1, 1, 1);

}

Point nextPos(Point dir, Point pos) {

	if (dir.x) {
		return (Point) { pos.x + dir.x * TILE_SIZE, pos.y };
	}
	return (Point) { pos.x, pos.y + dir.y * TILE_SIZE };

}

void updateTiles(Point pos, Point nextP) {

	tiles[nextP.x / TILE_SIZE][nextP.y / TILE_SIZE] = 1;
	tiles[pos.x / TILE_SIZE][pos.y / TILE_SIZE] = 0;

}

void updateBody(Square* body) {

	updateTiles(body[0].pos, head.headPart.lastPos);
	body[0].pos = head.headPart.lastPos;

	for (int i = 0; i < head.bodyLength - 1; i++) {
		updateTiles(body[i + 1].pos, body[i].lastPos);
		body[i + 1].pos = body[i].lastPos;
		body[i].lastPos = body[i].pos;
	}
	body[head.bodyLength - 1].lastPos = body[head.bodyLength - 1].pos;
	tiles[body[head.bodyLength - 1].lastPos.x / TILE_SIZE][body[head.bodyLength - 1].lastPos.y / TILE_SIZE] = 1;

}

void addPartToBody() {

	if (head.bodyLength == MAX_BODY_LENGTH)
		return;

	Square* b = head.body;
	int newX = b[head.bodyLength - 1].pos.x;
	int newY = b[head.bodyLength - 1].pos.y;
	b[head.bodyLength] = (Square) { newX, newY, newX, newY, Squares_draw };

	tiles[newX / TILE_SIZE][newY / TILE_SIZE] = 1;
	tiles[food.pos.x / TILE_SIZE][food.pos.y / TILE_SIZE] = 1;

	Point emptyPoints[nTiles];
	int index = 0;
	for (int i = 0; i < N_COLUMNS; i++) {
		for (int j = 0; j < N_ROWS; j++) {
			if (!tiles[i][j]) {
				emptyPoints[index] = (Point) { i * TILE_SIZE, j * TILE_SIZE };
				index += 1;
			}
		}
	}

	int randIndex = rand() % index;
	food.pos = (Point) { emptyPoints[randIndex].x, emptyPoints[randIndex].y };
	tiles[food.pos.x / TILE_SIZE][food.pos.y / TILE_SIZE] = 1;

	head.bodyLength += 1;
	if (snake_speed > 8)
		snake_speed -= 0.3;
}

void head_move(Head* head, S2D_Event* e) {

	if (buffer >= snake_speed || e) {
		Point dir = head->dir;
		Point pos = head->headPart.pos;
		Point nextP = nextPos(dir, pos);


		if (nextP.x < 0 || nextP.x > WINDOW_WIDTH - TILE_SIZE)
			return;
		if (nextP.y < 0 || nextP.y > WINDOW_HEIGHT - TILE_SIZE)
			return;

		if (tiles[nextP.x / TILE_SIZE][nextP.y / TILE_SIZE]) { // if next tile is occupied

			if (nextP.x == food.pos.x && nextP.y == food.pos.y) { // if food on next tile
				addPartToBody();
			}
			else {
				//game over
				return;
			}
		}
		updateTiles(pos, nextP);

		head->headPart.pos = nextP; // move head

		updateBody(head->body); // move the rest
		head->headPart.lastPos = head->headPart.pos;

		buffer = 0;
		return;
	}
	buffer += 1;
}
//------------------------------------------------------------------------------------
void debug_draw(Square square) {

	int x = square.pos.x;
	int y = square.pos.y;

	S2D_DrawQuad(x, y, 0.7, 0, 0, 1,
		x + FOOD_SIZE, y, 0.7, 0, 0, 1,
		x + FOOD_SIZE, y + FOOD_SIZE, 0.7, 0, 0, 1,
		x, y + FOOD_SIZE, 0.7, 0, 0, 1);

}

//---------------------------------------------------------------Core Functions

void render() {

	float alpha = 0.4;
	for (int i = 0; i < N_COLUMNS; i++) {
		for (int j = 0; j < N_ROWS; j++) {

			if ((i + j) % 2)
				alpha = 0.36;

			S2D_DrawQuad(i * TILE_SIZE, j * TILE_SIZE, 0.2, 0.5, 0.7, alpha,
				i * TILE_SIZE + TILE_SIZE, j * TILE_SIZE, 0.2, 0.5, 0.7, alpha,
				i * TILE_SIZE + TILE_SIZE, j * TILE_SIZE + TILE_SIZE, 0.2, 0.5, 0.7, alpha,
				i * TILE_SIZE, j * TILE_SIZE + TILE_SIZE, 0.2, 0.5, 0.7, alpha);

			alpha = 0.4;
		}
	}
	//--------------------------------------------------------------------------------

	head.headPart.draw(&head.headPart);
	Square* body = head.body;
	for (int i = 0; i < head.bodyLength; i++) {
		body[i].draw(&body[i]);
	}
	food.draw(&food);

	//debug 
	/*
	for (int i = 0; i < N_COLUMNS; i++) {
		for (int j = 0; j < N_ROWS; j++) {
			if (tiles[i][j])
				debug_draw((Square){ i * TILE_SIZE, j * TILE_SIZE, 0, 0, Squares_draw });
		}
	} */

}

void update() {

	head.move(&head, NULL);

}

void on_key(S2D_Event e) {

	if (e.type == S2D_KEY_DOWN) {
		if (!strcmp(e.key, "A")) {

			if (head.dir.x == -1)
				return;
			head.dir = (Point) {-1, 0};
			head.move(&head, &e);

		}
		else if (!strcmp(e.key, "S")) {

			if (head.dir.y == 1)
				return;
			head.dir = (Point) { 0, 1 };
			head.move(&head, &e);

		}
		else if (!strcmp(e.key, "W")) {

			if (head.dir.y == -1)
				return;
			head.dir = (Point) { 0, -1 };
			head.move(&head, &e);

		}
		else if (!strcmp(e.key, "D")) {

			if (head.dir.x == 1)
				return;
			head.dir = (Point) { 1, 0 };
			head.move(&head, &e);

		}
	}

}

void init() {

	tiles = (int**)calloc(N_COLUMNS, sizeof(int*));
	for (int i = 0; i < N_COLUMNS; i++) {
		tiles[i] = (int*)calloc(N_ROWS, sizeof(int));
	}

	tiles[food.pos.x / TILE_SIZE][food.pos.y / TILE_SIZE] = 1;
	srand((unsigned int)time(NULL)); // do this only once

	body[0] = (Square) { TILE_SIZE * 3, 0, TILE_SIZE * 3, 0, Squares_draw };
	body[1] = (Square) { TILE_SIZE * 2, 0, TILE_SIZE * 2, 0, Squares_draw };
	tiles[2][0] = 1; // body[1]
	tiles[3][0] = 1; // body[0]
	tiles[4][0] = 1; // head

}

int WinMain(int argc, char **argv) {
//int main(int argc, char **argv) {

	init();

	S2D_Window *window = S2D_CreateWindow(
		"SimpleSnake", WINDOW_WIDTH, WINDOW_HEIGHT, update, render, 0
	);

	window->on_key = on_key;

	S2D_Show(window);
	S2D_FreeWindow(window);
	free(tiles);
	return 0;
}