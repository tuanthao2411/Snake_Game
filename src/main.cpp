#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "snake.h"
#include "fruit.h"
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;

bool init(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow("Snake Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != nullptr;
}

void drawGrid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    for (int x = 0; x < SCREEN_WIDTH; x += GRID_SIZE)
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    for (int y = 0; y < SCREEN_HEIGHT; y += GRID_SIZE)
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
}

void displayGameOver(SDL_Renderer* renderer, int score) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_Rect gameOverRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4 };
    SDL_RenderFillRect(renderer, &gameOverRect);

    // Hiển thị điểm số
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);  // Hiển thị game over trong 2 giây
}

int main() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!init(window, renderer)) return -1;

    Snake snake(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, GRID_SIZE);
    Fruit fruit(GRID_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool running = true;
    SDL_Event e;
    Uint32 lastMove = SDL_GetTicks();
    const Uint32 MOVE_INTERVAL = 100;
    int score = 0;  // Điểm số

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    snake.changeDirection(UP); break;
                    case SDLK_DOWN:  snake.changeDirection(DOWN); break;
                    case SDLK_LEFT:  snake.changeDirection(LEFT); break;
                    case SDLK_RIGHT: snake.changeDirection(RIGHT); break;
                }
            }
        }

        if (SDL_GetTicks() - lastMove > MOVE_INTERVAL) {
            snake.move();

            // Kiểm tra thua: đụng tường
            if (snake.checkCollisionWithWall(SCREEN_WIDTH, SCREEN_HEIGHT)) {
                displayGameOver(renderer, score);
                running = false;
            }

            // Kiểm tra thua: tự cắn
            if (snake.checkCollisionWithSelf()) {
                displayGameOver(renderer, score);
                running = false;
            }

            // Kiểm tra ăn quả
            if (SDL_HasIntersection(&snake.getHead(), &fruit.getRect())) {
                snake.grow();
                fruit.respawn();
                score++;  // Tăng điểm sau mỗi lần ăn quả
            }

            lastMove = SDL_GetTicks();
        }

        // Vẽ game
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawGrid(renderer);
        fruit.render(renderer);
        snake.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);  // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
