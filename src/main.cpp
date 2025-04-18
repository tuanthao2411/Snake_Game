#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "snake.h"
#include "fruit.h"

#define GRID_WIDTH 32
#define GRID_HEIGHT 24
#define TILE_SIZE 20
#define WINDOW_WIDTH (GRID_WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * TILE_SIZE)
#define CELL_SIZE TILE_SIZE
#define MOVE_INTERVAL 150  // thời gian giữa các lần di chuyển (ms)

void renderGrid(SDL_Renderer* renderer) {
    // Nền đen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ viền đỏ
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);

    // Viền trên và dưới
    for (int x = 0; x < GRID_WIDTH; ++x) {
        SDL_Rect top = { x * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
        SDL_Rect bottom = { x * TILE_SIZE, (GRID_HEIGHT - 1) * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &top);
        SDL_RenderFillRect(renderer, &bottom);
    }

    // Viền trái và phải
    for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
        SDL_Rect left = { 0, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_Rect right = { (GRID_WIDTH - 1) * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &left);
        SDL_RenderFillRect(renderer, &right);
    }

    // Vẽ lưới xám
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    for (int x = 0; x <= GRID_WIDTH; ++x)
        SDL_RenderDrawLine(renderer, x * TILE_SIZE, 0, x * TILE_SIZE, GRID_HEIGHT * TILE_SIZE);
    for (int y = 0; y <= GRID_HEIGHT; ++y)
        SDL_RenderDrawLine(renderer, 0, y * TILE_SIZE, GRID_WIDTH * TILE_SIZE, y * TILE_SIZE);
}

void displayGameOver(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
}

int main(int argc, char* argv[]) {
    std::cout << "Game starting.\n";
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Simple Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect gameBounds = {
        CELL_SIZE, CELL_SIZE,
        WINDOW_WIDTH - 2 * CELL_SIZE,
        WINDOW_HEIGHT - 2 * CELL_SIZE
    };

    Snake snake(CELL_SIZE, gameBounds);
    Fruit fruit(CELL_SIZE, gameBounds);
    fruit.loadTexture(renderer, "Apple.png");

    bool running = true;
    SDL_Event e;
    Uint32 lastMove = 0;

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
            SDL_Rect head = snake.getHead();

            // Va chạm viền chơi
            if (head.x < gameBounds.x || head.y < gameBounds.y ||
                head.x + head.w > gameBounds.x + gameBounds.w ||
                head.y + head.h > gameBounds.y + gameBounds.h) {
                displayGameOver(renderer);
                running = false;
            }

            // Tự cắn
            if (snake.checkCollisionWithSelf()) {
                displayGameOver(renderer);
                running = false;
            }

            // Ăn táo
            if (SDL_HasIntersection(&head, &fruit.getRect())) {
                snake.grow();
                fruit.respawn(gameBounds.x, gameBounds.y, gameBounds.w, gameBounds.h, snake.getBody());
            }

            lastMove = SDL_GetTicks();
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        renderGrid(renderer);
        fruit.render(renderer);
        snake.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}