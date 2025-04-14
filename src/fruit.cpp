#include "fruit.h"
#include <cstdlib>
#include <ctime>

Fruit::Fruit(int gridSize, int screenWidth, int screenHeight)
    : gridSize(gridSize), screenWidth(screenWidth), screenHeight(screenHeight) {
    std::srand(std::time(0));  // Seed for random generation
    respawn();  // Tạo quả ngay khi khởi tạo
}

void Fruit::respawn() {
    rect.x = (std::rand() % (screenWidth / gridSize)) * gridSize;
    rect.y = (std::rand() % (screenHeight / gridSize)) * gridSize;
    rect.w = gridSize;
    rect.h = gridSize;
}

void Fruit::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Màu đỏ cho quả
    SDL_RenderFillRect(renderer, &rect);
}

SDL_Rect Fruit::getRect() const {
    return rect;
}
