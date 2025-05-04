#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "snake.h"
#include "fruit.h"
#include <random>
#include <string>
using namespace std;    

#define GRID_WIDTH 32
#define GRID_HEIGHT 24
#define TILE_SIZE 20
#define WINDOW_WIDTH (GRID_WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * TILE_SIZE)
#define CELL_SIZE TILE_SIZE
#define BASE_MOVE_INTERVAL 300
#define DELAY_TIME 50

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

// Hằng số cho menu
const int START_BUTTON_EASY_X = 12;
const int START_BUTTON_EASY_Y = 9;
const int START_BUTTON_MEDIUM_X = 12;
const int START_BUTTON_MEDIUM_Y = 13;
const int START_BUTTON_HARD_X = 12;
const int START_BUTTON_HARD_Y = 17;

const int BUTTON_WIDTH = 8;
const int BUTTON_HEIGHT = 3;

enum GameMode { EASY, MEDIUM, HARD, NONE };

bool checkRectFRectCollision(const SDL_Rect& rect, const SDL_FRect& frect) {
    return rect.x < frect.x + frect.w &&
           rect.x + rect.w > frect.x &&
           rect.y < frect.y + frect.h &&
           rect.y + rect.h > frect.y;
}

void renderBorderWalls(SDL_Renderer* renderer,SDL_Texture* texture) {
    for (int x = 0; x < GRID_WIDTH; ++x) {
        SDL_Rect wall = { x * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE };
        SDL_RenderCopy(renderer, texture, NULL, &wall);
        wall.y = (GRID_HEIGHT - 1) * TILE_SIZE;
        SDL_RenderCopy(renderer, texture, NULL, &wall);
    }
    for (int y = 1; y < GRID_HEIGHT - 1; ++y) {
        SDL_Rect wall = { 0, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        SDL_RenderCopy(renderer, texture, NULL, &wall);
        wall.x = (GRID_WIDTH - 1) * TILE_SIZE;
        SDL_RenderCopy(renderer, texture, NULL, &wall);
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

void renderBackground(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect gameBounds) {
    int textureWidth, textureHeight;
    SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);

   /// Lặp lại texture để phủ kín khu vực chơi
   for (int x = 0; x < WINDOW_WIDTH; x += textureWidth) {
    for (int y = 0; y < WINDOW_HEIGHT; y += textureHeight) {
            SDL_Rect dstRect = { x, y, textureWidth, textureHeight };
            SDL_RenderCopy(renderer, texture, NULL, &dstRect);
        }
    }
}

bool displayGameOver(SDL_Renderer* renderer, SDL_Texture* backgroundTexture, SDL_Rect gameBounds, TTF_Font* font, int score, int playTimeSeconds) {
    bool restart = false;
    SDL_Event e;

    while (true) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                return false; // Thoát trò chơi
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
                restart = true; // Chơi lại
                break;
            }
        }
        if (restart) break;

        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderClear(renderer);

        SDL_Color textColor = {255, 255, 255, 255};
        SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
        SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        SDL_Rect gameOverRect = {WINDOW_WIDTH / 2 - gameOverSurface->w / 2, WINDOW_HEIGHT / 2 - 20 , gameOverSurface->w, gameOverSurface->h};
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
        SDL_FreeSurface(gameOverSurface);
        SDL_DestroyTexture(gameOverTexture);

        string scoreText = "Score: " + to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_Rect scoreRect = {WINDOW_WIDTH / 2 - scoreSurface->w /2 , WINDOW_HEIGHT / 2 + 20, scoreSurface->w, scoreSurface->h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);

        string timeText = "Time: " + to_string(playTimeSeconds) + "s";
        SDL_Surface* timeSurface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
        SDL_Texture* timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        SDL_Rect timeRect = {WINDOW_WIDTH / 2 - timeSurface->w / 2, WINDOW_HEIGHT / 2 + 60, timeSurface->w, timeSurface->h};
        SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);
        SDL_FreeSurface(timeSurface);
        SDL_DestroyTexture(timeTexture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    return restart;
}


// Hàm hỗ trợ tải texture
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
    return texture;
}

// Hàm chờ nhấp chuột
void waitUntilMousePressedIntoRectangle(SDL_Rect rect) {
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if ((x >= rect.x && x <= rect.x + rect.w) && (y >= rect.y && y <= rect.y + rect.h)) {
                    return;
                }
            }
        }
        SDL_Delay(DELAY_TIME);
    }
}


GameMode showMenu(SDL_Renderer* renderer,SDL_Texture* menuTexture, SDL_Texture* easyTexture, SDL_Texture* mediumTexture, SDL_Texture* hardTexture) {
    std::cout << "Entering showMenu" << std::endl;
    GameMode selectedMode = NONE;

    // Vị trí và kích thước của các nút
    SDL_Rect easyButtonRect = {START_BUTTON_EASY_X * CELL_SIZE, START_BUTTON_EASY_Y * CELL_SIZE, BUTTON_WIDTH * CELL_SIZE, BUTTON_HEIGHT * CELL_SIZE};
    SDL_Rect mediumButtonRect = {START_BUTTON_MEDIUM_X * CELL_SIZE, START_BUTTON_MEDIUM_Y * CELL_SIZE, BUTTON_WIDTH * CELL_SIZE, BUTTON_HEIGHT * CELL_SIZE};
    SDL_Rect hardButtonRect = {START_BUTTON_HARD_X * CELL_SIZE, START_BUTTON_HARD_Y * CELL_SIZE, BUTTON_WIDTH * CELL_SIZE, BUTTON_HEIGHT * CELL_SIZE};

    while (selectedMode == NONE) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (menuTexture) {
            SDL_Rect menuRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderCopy(renderer, menuTexture, NULL, &menuRect);
        }
        if (easyTexture) SDL_RenderCopy(renderer, easyTexture, NULL, &easyButtonRect);
        if (mediumTexture) SDL_RenderCopy(renderer, mediumTexture, NULL, &mediumButtonRect);
        if (hardTexture) SDL_RenderCopy(renderer, hardTexture, NULL, &hardButtonRect);

        SDL_RenderPresent(renderer);

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) return NONE;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= easyButtonRect.x && x <= easyButtonRect.x + easyButtonRect.w && y >= easyButtonRect.y && y <= easyButtonRect.y + easyButtonRect.h) {
                    selectedMode = EASY;
                } else if (x >= mediumButtonRect.x && x <= mediumButtonRect.x + mediumButtonRect.w && y >= mediumButtonRect.y && y <= mediumButtonRect.y + mediumButtonRect.h) {
                    selectedMode = MEDIUM;
                } else if (x >= hardButtonRect.x && x <= hardButtonRect.x + hardButtonRect.w && y >= hardButtonRect.y && y <= hardButtonRect.y + hardButtonRect.h) {
                    selectedMode = HARD;
                }
            }
        }
        SDL_Delay(DELAY_TIME);
    }
    return selectedMode;
    
}




int main(int argc, char* argv[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    SDL_Init(SDL_INIT_VIDEO);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "❌ SDL_image Init Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "❌ SDL_ttf Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
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

    // Tải font chữ (cần file font .ttf trong thư mục làm việc)
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải texture nền
    SDL_Texture* backgroundTexture = loadTexture("background.png", renderer);
    if (!backgroundTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Tải texture nền menu
    SDL_Texture* menuBackgroundTexture = loadTexture("menu.png", renderer);
    if (!menuBackgroundTexture) {
        std::cerr << "Lỗi tải nền menu: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* buttonEasyTexture = loadTexture("easy.png", renderer);
    if (!buttonEasyTexture) {
        std::cerr << "Lỗi tải nút Dễ: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(menuBackgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* buttonMediumTexture = loadTexture("medium.png", renderer);
    if (!buttonMediumTexture) {
        std::cerr << "Lỗi tải nút Trung Bình: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(menuBackgroundTexture);
        SDL_DestroyTexture(buttonEasyTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* buttonHardTexture = loadTexture("hard.png", renderer);
    if (!buttonHardTexture) {
        std::cerr << "Lỗi tải nút Khó: " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(menuBackgroundTexture);
        SDL_DestroyTexture(buttonEasyTexture);
        SDL_DestroyTexture(buttonMediumTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    

    SDL_Surface* mapSurface = IMG_Load("mapcodinh.png");
if (!mapSurface) {
    std::cerr << "Failed to load map: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
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
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 1;
}

    SDL_Surface* fixedWallSurface = IMG_Load("wallcodinh.png");
    if (!fixedWallSurface) {
        std::cerr << "Failed to load wallcodinh: " << IMG_GetError() << "\n";
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
        return 1;
    }
    SDL_Texture* fixedWallTexture = SDL_CreateTextureFromSurface(renderer, fixedWallSurface);
    SDL_FreeSurface(fixedWallSurface);

    if (!fixedWallTexture) {
        std::cerr << "Failed to create wall texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Rect gameBounds = {
        CELL_SIZE, CELL_SIZE,
        (GRID_WIDTH - 2) * CELL_SIZE,
        (GRID_HEIGHT - 2) * CELL_SIZE
    }; 
    bool running = true;
    while (running) {
        // Hiển thị menu và lấy chế độ được chọn
        GameMode selectedMode = showMenu(renderer, menuBackgroundTexture, buttonEasyTexture, buttonMediumTexture, buttonHardTexture);
        if (selectedMode == NONE) {
            running = false;
            break;
        }

        // Thiết lập tốc độ di chuyển dựa trên chế độ
        int moveInterval;
        switch (selectedMode) {
            case EASY:
                moveInterval = BASE_MOVE_INTERVAL / 1.5 ; // 200ms (1x)
                break;
            case MEDIUM:
                moveInterval = BASE_MOVE_INTERVAL / 2; // 150ms (1.5x)
                break;
            case HARD:
                moveInterval = BASE_MOVE_INTERVAL / 3; // 100ms (2x)
                break;
            default:
                moveInterval = BASE_MOVE_INTERVAL;
        }
    Snake snake(CELL_SIZE, gameBounds, fixedWalls, WALLSIZE);
    if (!snake.loadHeadTexture(renderer, "snake_head.png") || !snake.loadBodyTexture(renderer, "snake_body.png")) {
        std::cerr << "Không thể tải texture rắn" << std::endl;
        running = false;
        break;
    }
    
    Fruit fruit(CELL_SIZE, gameBounds);
    if (!fruit.loadTexture(renderer, "Apple.png")) {
        std::cerr << "Failed to load apple texture" << std::endl;
        running = false;
        break;
    }
    fruit.respawn(gameBounds.x, gameBounds.y, gameBounds.w, gameBounds.h, snake.getBody(), fixedWalls, WALLSIZE);
    
    
    SDL_Event e;
    Uint32 lastMove = 0;
    Uint32 startTime = SDL_GetTicks(); // Bắt đầu tính thời gian chơi
    int score = 0; // Điểm số (số quả táo ăn được)
    bool gameRunning = true;
    while (running && gameRunning) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)  {
                running = false;
                gameRunning = false;
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:    snake.changeDirection(UP); break;
                    case SDLK_DOWN:  snake.changeDirection(DOWN); break;
                    case SDLK_LEFT:  snake.changeDirection(LEFT); break;
                    case SDLK_RIGHT: snake.changeDirection(RIGHT); break;
                }
            }
        }

        if (SDL_GetTicks() - lastMove > moveInterval) {
            snake.move();
            SDL_Rect head = snake.getHead();

            if (snake.isOutsideBounds() || snake.checkCollisionWithSelf()) {
                int playTimeSeconds = (SDL_GetTicks() - startTime) / 1000;
                if (!displayGameOver(renderer, backgroundTexture, gameBounds, font, score, playTimeSeconds)) {
                    running = false; // Thoát nếu nhấn ESC
                }
                gameRunning = false; // Kết thúc ván hiện tại
                continue;
            }

            for (int i = 0; i < WALLSIZE; ++i) {
                if (checkRectFRectCollision(snake.getHead(), fixedWalls[i])) {
                    int playTimeSeconds = (SDL_GetTicks() - startTime) / 1000;
                    if (!displayGameOver(renderer, backgroundTexture, gameBounds, font, score, playTimeSeconds)) {
                        running = false;
                    }
                    gameRunning = false;
                    break;
                }
            }

            if (head.x == fruit.getRect().x && head.y == fruit.getRect().y) {
                snake.grow();
                score++;
                fruit.respawn(gameBounds.x, gameBounds.y, gameBounds.w, gameBounds.h, snake.getBody(),fixedWalls, WALLSIZE);
            }

            lastMove = SDL_GetTicks();
        }

        // Vẽ nền bằng texture trong gameBounds
        SDL_SetRenderDrawColor(renderer, 205, 182, 193, 255);
        SDL_RenderClear(renderer);
        renderBackground(renderer, backgroundTexture, gameBounds);
    
        renderBorderWalls(renderer,fixedWallTexture);
        renderFixedWalls(renderer, fixedWallTexture);
        snake.render(renderer);
        fruit.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}   

    // Giải phóng tài nguyên
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(menuBackgroundTexture);
    SDL_DestroyTexture(buttonEasyTexture);
    SDL_DestroyTexture(buttonMediumTexture);
    SDL_DestroyTexture(buttonHardTexture);
    SDL_DestroyTexture(fixedWallTexture);
    SDL_DestroyTexture(mapTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

