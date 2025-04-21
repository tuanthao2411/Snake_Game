#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "snake.h"
#include "fruit.h"
#include <random>
using namespace std;

#define GRID_WIDTH 32
#define GRID_HEIGHT 24
#define TILE_SIZE 20
#define WINDOW_WIDTH (GRID_WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * TILE_SIZE)
#define CELL_SIZE TILE_SIZE
#define MOVE_INTERVAL 150

const int WALLSIZE = 7;
SDL_FRect fixedWalls[WALLSIZE] = {
    {8 * TILE_SIZE, 13 * TILE_SIZE, TILE_SIZE, 5 * TILE_SIZE},
    {7 * TILE_SIZE, 7 * TILE_SIZE, 10 * TILE_SIZE, TILE_SIZE},
    {2 * TILE_SIZE, 4 * TILE_SIZE, TILE_SIZE, 4 * TILE_SIZE},
    {18 * TILE_SIZE, 2 * TILE_SIZE, TILE_SIZE, 8 * TILE_SIZE},
    {16 * TILE_SIZE, 8 * TILE_SIZE, TILE_SIZE, 7 * TILE_SIZE},
    {3 * TILE_SIZE, 11 * TILE_SIZE, 11 * TILE_SIZE, TILE_SIZE},
    {16 * TILE_SIZE, 16 * TILE_SIZE, 7 * TILE_SIZE, TILE_SIZE},
};

bool checkRectFRectCollision(const SDL_Rect& rect, const SDL_FRect& frect) {
    return rect.x < frect.x + frect.w &&
           rect.x + rect.w > frect.x &&
           rect.y < frect.y + frect.h &&
           rect.y + rect.h > frect.y;
}

void renderBorderWalls(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    for (int x = 0; x < GRID_WIDTH; ++x) {
        SDL_Rect wall = { x * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &wall);
        wall.y = (GRID_HEIGHT - 1) * TILE_SIZE;
        SDL_RenderFillRect(renderer, &wall);
    }
    for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
        SDL_Rect wall = { 0, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderFillRect(renderer, &wall);
        wall.x = (GRID_WIDTH - 1) * TILE_SIZE;
        SDL_RenderFillRect(renderer, &wall);
    }
}

void renderFixedWalls(SDL_Renderer* renderer, SDL_Texture* texture) {
    for (int i = 0; i < WALLSIZE; ++i) {
        SDL_Rect rect = {
            static_cast<int>(fixedWalls[i].x),
            static_cast<int>(fixedWalls[i].y),
            static_cast<int>(fixedWalls[i].w),
            static_cast<int>(fixedWalls[i].h)
        };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }
}

void displayGameOver(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
}

int main(int argc, char* argv[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    SDL_Init(SDL_INIT_VIDEO);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "❌ SDL_image Init Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Simple Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
     if (!window) {
         std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
         SDL_Quit();
         return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Surface* mapSurface = IMG_Load("mapcodinh.png");
if (!mapSurface) {
    std::cerr << "Failed to load map: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 1;
}
SDL_Texture* mapTexture = SDL_CreateTextureFromSurface(renderer, mapSurface);
SDL_FreeSurface(mapSurface);

if (!mapTexture) {
    std::cerr << "Failed to create map texture: " << SDL_GetError() << std::endl;
    SDL_FreeSurface(mapSurface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 1;
}

    SDL_Surface* fixedWallSurface = IMG_Load("wallcodinh.png");
    if (!fixedWallSurface) {
        std::cerr << "Failed to load wallcodinh: " << IMG_GetError() << "\n";
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
        return 1;
    }
    SDL_Texture* fixedWallTexture = SDL_CreateTextureFromSurface(renderer, fixedWallSurface);
    SDL_FreeSurface(fixedWallSurface);

    if (!fixedWallTexture) {
        std::cerr << "Failed to create wall texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(fixedWallSurface);
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Rect gameBounds = {
        CELL_SIZE, CELL_SIZE,
        (GRID_WIDTH - 2) * CELL_SIZE,
        (GRID_HEIGHT - 2) * CELL_SIZE
    };

    Snake snake(CELL_SIZE, gameBounds);
    Fruit fruit(CELL_SIZE, gameBounds);
    if (!fruit.loadTexture(renderer, "Apple.png")) {
        std::cerr << "Failed to load apple texture" << std::endl;
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(fixedWallTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    fruit.respawn(gameBounds.x, gameBounds.y, gameBounds.w, gameBounds.h, snake.getBody());

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

            if (snake.isOutsideBounds() || snake.checkCollisionWithSelf()) {
                displayGameOver(renderer);
                running = false;
            }

            for (int i = 0; i < WALLSIZE; ++i) {
                if (checkRectFRectCollision(snake.getHead(), fixedWalls[i])) {
                    displayGameOver(renderer);
                    running = false;
                    break;
                }
            }

            if (head.x == fruit.getRect().x && head.y == fruit.getRect().y) {
                snake.grow();
                fruit.respawn(gameBounds.x, gameBounds.y, gameBounds.w, gameBounds.h, snake.getBody());
            }

            lastMove = SDL_GetTicks();
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_Rect mapRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderCopy(renderer, mapTexture, NULL, &mapRect);

        renderBorderWalls(renderer);
        renderFixedWalls(renderer, fixedWallTexture);
        snake.render(renderer);
        fruit.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(fixedWallTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
