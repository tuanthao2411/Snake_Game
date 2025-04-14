#ifndef FRUIT_H
#define FRUIT_H

#include <SDL2/SDL.h>

class Fruit {
public:
    Fruit(int gridSize, int screenWidth, int screenHeight);
    void respawn();  // Tạo quả tại vị trí ngẫu nhiên
    void render(SDL_Renderer* renderer);  // Vẽ quả lên màn hình
    SDL_Rect getRect() const;  // Lấy vị trí của quả

private:
    SDL_Rect rect;  // Hình chữ nhật mô tả quả
    int gridSize;   // Kích thước mỗi ô vuông của lưới
    int screenWidth; // Chiều rộng màn hình
    int screenHeight; // Chiều cao màn hình
};

#endif
