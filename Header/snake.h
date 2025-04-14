#ifndef SNAKE_H
#define SNAKE_H

#include <SDL2/SDL.h>
#include <vector>

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake(int x, int y, int gridSize);
    void move();
    void grow();
    void changeDirection(Direction dir);
    void render(SDL_Renderer* renderer);
    SDL_Rect getHead() const;
    bool checkCollisionWithWall(int screenWidth, int screenHeight);
    bool checkCollisionWithSelf();

private:
    std::vector<SDL_Rect> body;  // Danh sách các đoạn thân của rắn
    Direction currentDirection;   // Hướng di chuyển của rắn
    int gridSize;                 // Kích thước mỗi ô vuông của lưới
};

#endif
