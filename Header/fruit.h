#pragma once
#ifndef FRUIT_H
#define FRUIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

class Fruit {
public:
    Fruit(int gridSize, SDL_Rect bounds);
    ~Fruit();
    // Tạo quả táo mới tại vị trí ngẫu nhiên
    void respawn(int offsetX, int offsetY, int width, int height,const std::vector<SDL_Rect>& snakeBody);

    // Lấy hình chữ nhật của quả táo (dùng cho kiểm tra va chạm)
    SDL_Rect getRect() const;

    // Vẽ quả táo lên màn hình
    void render(SDL_Renderer* renderer);
    bool loadTexture(SDL_Renderer* renderer,const char* imagePath);

private:
    int m_gridSize;         // Kích thước ô lưới
    SDL_Rect m_rect;        // Hình chữ nhật của quả táo
    SDL_Texture* m_texture = nullptr;
    bool collidesWithSnake(const std::vector<SDL_Rect>& snakeBody) const;
};

#endif
