#include "snake.h"

#define WINDOW_X      0
#define WINDOW_Y      0
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000

#define GRID_SIZE 28
#define GRID_DIM  800

#define DELAY 20

enum snake_dir {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};
typedef enum snake_dir SnakeDir;

typedef struct {
    int x, y;
} apple;

struct snake {
    int x, y;
    SnakeDir dir;
    struct snake *next;
};
typedef struct snake Snake;

Snake *head, *tail;
apple Apple;
int snake_size = 0;

void init_snake() {
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL;

    head = new;
    tail = new;

    snake_size = 1;

    return;
}

void increase_snake() {
    Snake *new = malloc(sizeof(Snake));

    switch (tail->dir) {
    case SNAKE_UP:
        new->x = tail->x;
        new->y = tail->y + 1;
        break;
    case SNAKE_DOWN:
        new->x = tail->x;
        new->y = tail->y - 1;
        break;
    case SNAKE_LEFT:
        new->x = tail->x + 1;
        new->y = tail->y;
        break;
    case SNAKE_RIGHT:
        new->x = tail->x - 1;
        new->y = tail->y;
        break;
    }
    new->dir = tail->dir;
    new->next = NULL;
    tail->next = new;

    tail = new;
    snake_size += 1;

    return;
}

void render_grid(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0xff, SDL_ALPHA_OPAQUE);

#if 0
    int cell_size = GRID_DIM / GRID_SIZE;
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            cell.x = x + i * cell_size;
            cell.y = y + j * cell_size;
            SDL_RenderDrawRect(renderer, &cell);
        }
    }

#else

    SDL_Rect outline;
    outline.x = x;
    outline.y = y;
    outline.w = GRID_DIM;
    outline.h = GRID_DIM;

    SDL_RenderDrawRect(renderer, &outline);
#endif
    return;
}

void move_snake() {
    int prev_x = head->x;
    int prev_y = head->y;
    SnakeDir prev_dir = head->dir;

    switch (head->dir) {
    case SNAKE_UP:
        head->y--;
        break;
    case SNAKE_DOWN:
        head->y++;
        break;
    case SNAKE_LEFT:
        head->x--;
        break;
    case SNAKE_RIGHT:
        head->x++;
        break;
    }

    Snake *track = NULL;
    if (head->next != NULL) {
        track = head->next;
    }

    while (track != NULL) {
        int save_x = track->x;
        int save_y = track->y;
        SnakeDir save_dir = track->dir;

        track->x = prev_x;
        track->y = prev_y;
        track->dir = prev_dir;

        track = track->next;

        prev_x = save_x;
        prev_y = save_y;
        prev_dir = save_dir;
    }

    return;
}

void reset_snake() {
    Snake *track = head;
    Snake *temp;

    while (track != NULL) {
        temp = track;
        track = track->next;
        free(temp);
    }

    init_snake();
    increase_snake();
    increase_snake();
    increase_snake();

    return;
}

void render_snake(SDL_Renderer *renderer, int x, int y) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE);

    int seg_size = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;
    seg.w = seg_size;
    seg.h = seg_size;

    Snake *track = head;
    while (track != NULL) {
        seg.x = x + track->x * seg_size;
        seg.y = y + track->y * seg_size;

        SDL_RenderFillRect(renderer, &seg);

        track = track->next;
    }

    return;
}

bool position_in_snake(int x, int y) {
    Snake *track = head;
    while (track != NULL) {
        if (x == track->x && y == track->y) {
            return true;
        }
        track = track->next;
    }

    return false;
}

void gen_apple() {
    if (snake_size == GRID_SIZE * GRID_SIZE - 1) {
        printf("YOU WON!");
        exit(EXIT_SUCCESS);
    }

    do {
        Apple.x = rand() % GRID_SIZE;
        Apple.y = rand() % GRID_SIZE;
    } while (position_in_snake(Apple.x, Apple.y));

    return;
}

void render_apple(SDL_Renderer *renderer, int x, int y) {
    int apple_size = GRID_DIM / GRID_SIZE;

    SDL_Rect apple;
    apple.w = apple_size;
    apple.h = apple_size;

    apple.x = x + Apple.x * apple_size;
    apple.y = y + Apple.y * apple_size;

    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &apple);

    return;
}

void render_score(SDL_Renderer *renderer, TTF_Font *font) {
    
    char text[16];
    sprintf(text, "Score: %d", snake_size - 4);

    SDL_Surface *message_surface = TTF_RenderText_Solid(font, text, (SDL_Color) {0xff, 0xff, 0xff, 0xff});
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, message_surface);
    SDL_FreeSurface(message_surface);

    SDL_Rect message_rect;
    message_rect.x = 50;
    message_rect.y = 50;
    TTF_SizeText(font, text, &message_rect.w, &message_rect.h);

    SDL_RenderCopy(renderer, message, NULL, &message_rect);

    SDL_DestroyTexture(message);
}

void detect_apple() {
    if (head->x == Apple.x && head->y == Apple.y) {
        gen_apple();
        increase_snake();
    }

    return;
}

bool position_outside_grid(int x, int y) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
        return true;
    }
    return false;
}

void detect_crash() {

    if (position_outside_grid(head->x, head->y)) {
        reset_snake();
    }

    Snake *track = head;
    if (track->next != NULL) {
        track = track->next;
    }

    while (track != NULL) {
        if (track->x == head->x && track->y == head->y) {
            reset_snake();
        }
        track = track->next;
    }

    return;
}

void turn_left() {

    switch (head->dir) {
    case SNAKE_UP:
        head->dir = SNAKE_LEFT;
        break;
    case SNAKE_DOWN:
        head->dir = SNAKE_RIGHT;
        break;
    case SNAKE_LEFT:
        head->dir = SNAKE_DOWN;
        break;
    case SNAKE_RIGHT:
        head->dir = SNAKE_UP;
        break;
    }

    return;
}

void turn_right() {

    switch (head->dir) {
    case SNAKE_UP:
        head->dir = SNAKE_RIGHT;
        break;
    case SNAKE_DOWN:
        head->dir = SNAKE_LEFT;
        break;
    case SNAKE_LEFT:
        head->dir = SNAKE_UP;
        break;
    case SNAKE_RIGHT:
        head->dir = SNAKE_DOWN;
        break;
    }

    return;
}

enum State {
    TRY_FORWARD,
    TRY_LEFT,
    TRY_RIGHT,
};

int state(enum State try) {
    int reward = 0;

    int try_x = head->x;
    int try_y = head->y;

    switch (head->dir) {
    case SNAKE_UP:
        switch (try) {
        case TRY_FORWARD:
            try_y = try_y - 1;
            break;
        case TRY_LEFT:
            try_x = try_x - 1;
            break;
        case TRY_RIGHT:
            try_x = try_x + 1;
            break;
        }
        break;
    case SNAKE_DOWN:
        switch (try) {
        case TRY_FORWARD:
            try_y = try_y + 1;
            break;
        case TRY_LEFT:
            try_x = try_x + 1;
            break;
        case TRY_RIGHT:
            try_x = try_x - 1;
            break;
        }
        break;
    case SNAKE_LEFT:
        switch (try) {
        case TRY_FORWARD:
            try_x = try_x - 1;
            break;
        case TRY_LEFT:
            try_y = try_y + 1;
            break;
        case TRY_RIGHT:
            try_y = try_y - 1;
            break;
        }
        break;
    case SNAKE_RIGHT:
        switch (try) {
        case TRY_FORWARD:
            try_x = try_x + 1;
            break;
        case TRY_LEFT:
            try_y = try_y - 1;
            break;
        case TRY_RIGHT:
            try_y = try_y + 1;
            break;
        }
        break;
    }

    if (position_outside_grid(try_x, try_y)) {
        reward = reward - 100;
    }

    // DETECT TAIL
    if (position_in_snake(try_x, try_y)) {
        reward = reward - 100;
        return reward; // early stop
    }

    // Detect apple
    if (try_x == Apple.x && try_y == Apple.y) {
        reward = reward + 100;
    }

    // Move towards apple
    int diff_x = abs(head->x - Apple.x);
    int diff_y = abs(head->y - Apple.y);
    int try_diff_x = abs(try_x - Apple.x);
    int try_diff_y = abs(try_y - Apple.y);
    
    if (try_diff_x < diff_x) {
        reward += 5;
    }
    if (try_diff_y < diff_y) {
        reward += 5;
    }

    return reward;
}

void ai() {
    int try_f = state(TRY_FORWARD);
    int try_l = state(TRY_LEFT);
    int try_r = state(TRY_RIGHT);

    if (try_f >= try_l && try_f >= try_r) {
        // CONTINUE FORWARD
    } else if (try_l >= try_r) {
        turn_left();
    } else {
        turn_right();
    }

    return;
}

int main() {
    srand(time(0));

    init_snake();
    increase_snake();
    increase_snake();
    increase_snake();

    gen_apple();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    TTF_Init();

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_INIT_VIDEO < 0) {
        fprintf(stderr, "ERROR: SDL_INIT_VIDEO");
    }

    window = SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y, WINDOW_WIDTH,
                              WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

    if (!window) {
        fprintf(stderr, "ERROR: !window");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        fprintf(stderr, "ERROR: !renderer");
    }


    SDL_RenderClear(renderer);

    int grid_x = WINDOW_WIDTH / 2 - GRID_DIM / 2;
    int grid_y = WINDOW_HEIGHT / 2 - GRID_DIM / 2;

    // Score font
    TTF_Font *sans = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSansBold.ttf", 48);
    if (sans == NULL) {
        fprintf(stderr, "ERROR: !font\n");
        exit(EXIT_FAILURE);
    }
    

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent((&event))) {
            SDL_FlushEvents(SDL_USEREVENT, SDL_LASTEVENT);
            switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_UP:
                case SDLK_w:
                    if (head->dir != SNAKE_DOWN) {
                        head->dir = SNAKE_UP;
                    }
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    if (head->dir != SNAKE_UP) {
                        head->dir = SNAKE_DOWN;
                    }
                    break;
                case SDLK_LEFT:
                case SDLK_a:
                    if (head->dir != SNAKE_RIGHT) {
                        head->dir = SNAKE_LEFT;
                    }
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    if (head->dir != SNAKE_LEFT) {
                        head->dir = SNAKE_RIGHT;
                    }
                    break;
                }
            }
        }
        SDL_RenderClear(renderer);
        // RENDER LOOP START

        move_snake();
        detect_apple();
        detect_crash();

        render_grid(renderer, grid_x, grid_y);
        render_snake(renderer, grid_x, grid_y);
        render_apple(renderer, grid_x, grid_y);
        render_score(renderer, sans);

        ai();

        // RENDER LOOP END
        SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, SDL_ALPHA_OPAQUE);
        SDL_RenderPresent(renderer);

        SDL_Delay(DELAY);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    TTF_CloseFont(sans);

    TTF_Quit();
    SDL_Quit();


    return 0;
}