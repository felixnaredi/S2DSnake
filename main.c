#include <simple2d.h>
#include <math.h>

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 256
#define TILE_SIZE 16
#define FOOD_SIZE 8
#define N_ROWS 16
#define N_COLUMNS 32
#define nTiles (WINDOW_WIDTH * WINDOW_HEIGHT / (TILE_SIZE * TILE_SIZE))


//--------------------------------------------------------Structs
typedef struct Points {
    int x, y;
}Point;

typedef struct Squares {
    Point pos;
    Point lastPos;
    void (*draw) (struct Squares*);

}Square;

typedef struct tHead {
    Point dir;
    Square headPart;
    Square* body;
    void (*move) (struct tHead*, S2D_Event* e);
}Head;

typedef struct Foods {
    Point pos;
    void (*draw) (struct Foods*);
}Food;

//------------------------------------------Predefined functions
void Squares_draw(Square* square) ;
void head_move(Head* head, S2D_Event* e) ;
void updateBody(Square* body);
void updateTiles(Point pos, Point nextP);
//---------------------------------------------------------------Static

Square body[30];

Head head = {
        1, 0,
        {32, 0,
         0, 0,
         Squares_draw}, body,
        head_move
};

int** tiles;
int buffer = 0;

//-----------------------------------------------------------Functions

void init_tiles(int** arr) {

    int i, j;
    for(i = 0; i < N_COLUMNS; i++) {
        for(j = 0; j < N_ROWS; j++) {
            arr[i][j] = 0;
        }
    }

}

void Squares_draw(Square* square) {

    int x = square->pos.x;
    int y = square->pos.y;

    S2D_DrawQuad(x, y, 1, 1, 1, 1,
                x + TILE_SIZE, y, 1, 1, 1, 1,
                x + TILE_SIZE, y + TILE_SIZE, 1, 1, 1, 1,
                x, y + TILE_SIZE, 1, 1, 1, 1);

}

Point nextPos(Point dir, Point pos) {

    if(dir.x) {
        return (Point) {pos.x + dir.x * TILE_SIZE, pos.y};
    }
    return (Point) {pos.x, pos.y + dir.y * TILE_SIZE};

}

void head_move(Head* head, S2D_Event* e) {

    if(buffer >= 60 || e) {
        Point dir = head->dir;
        Point pos = head->headPart.pos;
        Point nextP = nextPos(dir, pos);

        if(tiles[nextP.x / TILE_SIZE][nextP.y / TILE_SIZE]) {
            head->headPart.pos = (Point){0, 0};//gameover
            return;
        }
        updateTiles(pos, nextP);

        head->headPart.pos = nextP;

        updateBody(head->body);
        head->headPart.lastPos = head->headPart.pos;

        buffer = 0;
        return;
    }
    buffer += 1;
}

void updateBody(Square* body) {

    updateTiles(body[0].pos, head.headPart.lastPos);
    body[0].pos = head.headPart.lastPos;
    for(int i = 0; i < 2; i++) {
        updateTiles(body[i + 1].pos, body[i].lastPos);
        body[i + 1].pos = body[i].lastPos;
        body[i].lastPos = body[i].pos;
    }

}

void updateTiles(Point pos, Point nextP) {

    tiles[nextP.x / TILE_SIZE][nextP.y / TILE_SIZE] = 1;
    tiles[pos.x / TILE_SIZE][pos.y / TILE_SIZE] = 0;

}

void Foods_draw(Food* food) {

    int x = food->pos.x;
    int y = food->pos.y;

    S2D_DrawQuad(x, y, 30, 144, 255, 1,
                 x + FOOD_SIZE, y, 30, 144, 255, 1,
                 x + FOOD_SIZE, y + FOOD_SIZE, 30, 144, 255, 1,
                 x, y + FOOD_SIZE, 30, 144, 255, 1);

}

//---------------------------------------------------------------Core Functions

void render() {

    head.headPart.draw(&head.headPart);
    body[0].draw(&body[0]);
    body[1].draw(&body[1]);

}

void on_key(S2D_Event e) {

    if(e.type == S2D_KEY_DOWN) {
        if(strcmp(e.key, "A") == 0) {

            if(head.dir.x == -1)
                return;
            head.dir.x = -1;
            head.dir.y = 0;
            head.move(&head, &e);

        }else if(strcmp(e.key, "S") == 0) {

            if(head.dir.y == 1)
                return;
            head.dir.y = 1;
            head.dir.x = 0;
            head.move(&head, &e);

        }else if(strcmp(e.key, "W") == 0) {

            if(head.dir.y == -1)
                return;
            head.dir.y = -1;
            head.dir.x = 0;
            head.move(&head, &e);

        }else if(strcmp(e.key, "D") == 0) {

            if(head.dir.x == 1)
                return;
            head.dir.x = 1;
            head.dir.y = 0;
            head.move(&head, &e);

        }
    }

}

void update() {

    head.move(&head, NULL);

}

int main() {

    tiles = (int**) malloc(N_COLUMNS * sizeof(int*));
    for(int i = 0; i < N_COLUMNS; i++) {
        tiles[i] = (int*) malloc(N_ROWS * sizeof(int));
    }

    init_tiles(tiles);

    body[0] = (Square){0, 0, 0, 0, Squares_draw};
    body[1] = (Square){16, 0, 0, 0, Squares_draw};

    S2D_Window *window = S2D_CreateWindow(
            "SimpleSnake", WINDOW_WIDTH, WINDOW_HEIGHT, update, render, 0
    );

    window->on_key = on_key;

    S2D_Show(window);
    S2D_FreeWindow(window);
    free(tiles);
    return 0;
}
