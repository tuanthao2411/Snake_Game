#ifndef SNAKE_H
#define SNAKE_H

#include <SDL2/SDL.h>
#include <vector>

enum Direction { UP, DOWN, LEFT, RIGHT };

class Snake {
public:
    Snake(int startX, int startY, int gridSize);
    Snake(int gridSize, SDL_Rect bounds);

    // Di chuyển rắn
    void move();

    // Thay đổi hướng của rắn
    void changeDirection(Direction newDirection);

    // Kiểm tra va chạm với chính mình
    bool checkCollisionWithSelf() const;

    // Render rắn lên màn hình
    void render(SDL_Renderer* renderer) const;

    // Thêm một đoạn vào con rắn
    void grow();

    // Lấy hình chữ nhật đầu của con rắn
    SDL_Rect getHead() const;

    bool isOutsideBounds() const;

    const std::vector<SDL_Rect>& getBody() const;

private:
    int m_gridSize;            // Kích thước lưới
    Direction m_direction;     // Hướng hiện tại
    std::vector<SDL_Rect> m_body;  // Vị trí của các đoạn
    SDL_Rect m_bounds;    // Giới hạn biên chơi game
};

#endif