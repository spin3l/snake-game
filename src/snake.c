#include "snake.h"

#define WINDOW_X      0
#define WINDOW_Y      0
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 1000

#define GRID_SIZE 100
#define GRID_DIM  800

#define DELAY 1

enum snake_dir {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};
typedef enum snake_dir SnakeDir;

typedef struct {
    int x, y;
    int score;
} apple;

struct snake {
    int x, y;
    SnakeDir dir;
    struct snake *next;
};
typedef struct snake Snake;

Snake *head, *tail;
apple Apple;
int top_score = 0;

void init_snake() {
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL;

    head = new;
    tail = new;

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
    if (top_score < Apple.score) {
        top_score = Apple.score;
    }

    return;
}

void render_grid(SDL_Renderer *renderer, int offset_x, int offset_y) {
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

    for (int i = 0; i < 255; i++) {        
        SDL_SetRenderDrawColor(renderer, 0, 100 - i, 255 - i, 255 - i);
        
        SDL_Rect outline;
        outline.x = offset_x - i;
        outline.y = offset_y - i;
        outline.w = GRID_DIM + i + i;
        outline.h = GRID_DIM + i + i;

        SDL_RenderDrawRect(renderer, &outline);
    }
    
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

    Apple.score = 0;

    return;
}

void render_snake(SDL_Renderer *renderer, int x, int y, bool isFlash) {

    int seg_size = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;
    seg.w = seg_size - 2;
    seg.h = seg_size - 2;

    SDL_Rect seg_out;
    seg_out.w = seg_size;
    seg_out.h = seg_size;

    Snake *track = head;

    int bright = 255;
    int b_dir = 0;

    int r = 0x00;
    int g = 0xff;
    int b = 0x00;

    if (isFlash) {
        r = rand() % 255;
        g = rand() % 255;
        b = rand() % 255;
    }

    while (track != NULL) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, bright, SDL_ALPHA_OPAQUE);

        seg_out.x = x + track->x * seg_size + 1;
        seg_out.y = y + track->y * seg_size + 1;

        SDL_RenderFillRect(renderer, &seg_out);

        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        seg.x = x + track->x * seg_size + 1;
        seg.y = y + track->y * seg_size + 1;

        SDL_RenderFillRect(renderer, &seg);

        track = track->next;

        if (b_dir == 0) {
            bright -= 5;
            if (bright < 150) {
                b_dir = 1;
            }
        }

        if (b_dir == 1) {
            bright += 5;
            if (bright > 250) {
                b_dir = 0;
            }
        }
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
    Apple.score += 1;
    if (Apple.score == GRID_SIZE * GRID_SIZE - 1) {
        printf("YOU WON!\n");
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

void render_score(SDL_Renderer *renderer, TTF_Font *font, int offset_y) {
    
    char score_text[16], top_score_text[16];
    sprintf(score_text, "SCORE %3d", Apple.score);
    sprintf(top_score_text, "TOP SCORE %3d", top_score);

    SDL_Color score_color = {0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE};
    SDL_Color top_score_color = {0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE};


    SDL_Surface *message_surface = TTF_RenderText_Solid(font, score_text, score_color);
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
    SDL_FreeSurface(message_surface);

    message_surface = TTF_RenderText_Solid(font, top_score_text, top_score_color);
    SDL_Texture *top_score_texture = SDL_CreateTextureFromSurface(renderer, message_surface);
    SDL_FreeSurface(message_surface);

    SDL_Rect score_rect;
    TTF_SizeText(font, score_text, &score_rect.w, &score_rect.h);
    score_rect.x = (WINDOW_WIDTH - score_rect.w) / 2;
    score_rect.y = offset_y + (score_rect.h / 2);

    SDL_Rect top_score_rect;
    TTF_SizeText(font, top_score_text, &top_score_rect.w, &top_score_rect.h);
    top_score_rect.x = (WINDOW_WIDTH - top_score_rect.w) / 2;
    top_score_rect.y = offset_y - top_score_rect.h;


    SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
    SDL_RenderCopy(renderer, top_score_texture, NULL, &top_score_rect);

    SDL_DestroyTexture(score_texture);
    SDL_DestroyTexture(top_score_texture);
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

bool detect_crash() {

    if (position_outside_grid(head->x, head->y)) {
        reset_snake();
        return true;
    }

    Snake *track = head;
    if (track->next != NULL) {
        track = track->next;
    }

    while (track != NULL) {
        if (track->x == head->x && track->y == head->y) {
            reset_snake();
            return true;
        }
        track = track->next;
    }

    return false;
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
        return -1000; // early stop
    }

    // DETECT TAIL
    if (position_in_snake(try_x, try_y)) {
        return -1000; // early stop
    }

    // Detect apple
    if (try_x == Apple.x && try_y == Apple.y) {
        return 10000;
    }

    // Move towards apple
    int diff_x = abs(head->x - Apple.x);
    int diff_y = abs(head->y - Apple.y);
    int try_diff_x = abs(try_x - Apple.x);
    int try_diff_y = abs(try_y - Apple.y);
    
    if (try_diff_x < diff_x) {
        reward += 100;
    }
    if (try_diff_y < diff_y) {
        reward += 100;
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

    Apple.score = -1;
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

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_RenderClear(renderer);

    int grid_x = WINDOW_WIDTH / 2 - GRID_DIM / 2;
    int grid_y = WINDOW_HEIGHT / 2 - GRID_DIM / 2;

    // Score font
    TTF_Font *sans = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", 32);
    if (sans == NULL) {
        fprintf(stderr, "ERROR: !font\n");
        exit(EXIT_FAILURE);
    }
    

    bool quit = false;
    SDL_Event event;

    int flash = 0;
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
        if (detect_crash()) {
            flash = 0;
        }

        if ((Apple.score % 10) == 0 && Apple.score != 0) {
            flash = 10;
        }
        render_grid(renderer, grid_x, grid_y);
        render_score(renderer, sans, grid_x);
        render_snake(renderer, grid_x, grid_y, flash);
        render_apple(renderer, grid_x, grid_y);

        if (flash > 0) {
            flash--;
        }

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