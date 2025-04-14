#include "snake.h"
#include <SDL2/SDL.h>

Snake::Snake(int x, int y, int gridSize) : gridSize(gridSize), currentDirection(RIGHT) {
    body.push_back({x, y, gridSize, gridSize});
}

void Snake::move() {
    SDL_Rect newHead = body.front();

    switch (currentDirection) {
        case UP: newHead.y -= gridSize; break;
        case DOWN: newHead.y += gridSize; break;
        case LEFT: newHead.x -= gridSize; break;
        case RIGHT: newHead.x += gridSize; break;
    }

    body.insert(body.begin(), newHead);  // Thêm đầu mới
    body.pop_back();  // Xóa đuôi
}

void Snake::grow() {
    SDL_Rect newSegment = body.back();
    body.push_back(newSegment);  // Thêm một đoạn mới ở cuối
}

void Snake::changeDirection(Direction dir) {
    if ((currentDirection == UP && dir != DOWN) ||
        (currentDirection == DOWN && dir != UP) ||
        (currentDirection == LEFT && dir != RIGHT) ||
        (currentDirection == RIGHT && dir != LEFT)) {
        currentDirection = dir;
    }
}

void Snake::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : body) {
        SDL_RenderFillRect(renderer, &segment);
    }
}

SDL_Rect Snake::getHead() const {
    return body.front();
}

bool Snake::checkCollisionWithWall(int screenWidth, int screenHeight) {
    const SDL_Rect& head = body.front();
    return head.x < 0 || head.x >= screenWidth || head.y < 0 || head.y >= screenHeight;
}

bool Snake::checkCollisionWithSelf() {
    const SDL_Rect& head = body.front();
    for (size_t i = 1; i < body.size(); ++i) {
        if (SDL_HasIntersection(&head, &body[i])) {
            return true;
        }
    }
    return false;
}
