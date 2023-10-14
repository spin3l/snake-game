#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Graphics
#include <SDL2/SDL.h>
// Font
#include <SDL2/SDL_ttf.h>


typedef enum {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
} SnakeDir;

typedef struct {
    int x, y;
    int score;
} apple;


typedef struct {
    int x, y;
    SnakeDir dir;
} BodyTile;

typedef struct {
    int x, y;
    SnakeDir dir;
    struct snake *next;
} Snake;