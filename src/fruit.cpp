#include "fruit.h"
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdio>

Fruit::Fruit(int gridSize, SDL_Rect bounds)
    : m_gridSize(gridSize)
{
    // Khởi tạo vị trí ngẫu nhiên
    srand(static_cast<unsigned int>(time(0)));
    m_rect.w = m_rect.h = m_gridSize;
    std::vector<SDL_Rect> empty;
    respawn(bounds.x, bounds.y, bounds.w, bounds.h, empty );
}

Fruit::~Fruit() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

bool Fruit::loadTexture(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        SDL_Log(" Failed to load image: %s | Path: %s", IMG_GetError(), imagePath);
        return false;
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!m_texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return false;
    }
    SDL_Log("Apple texture loaded successfully!");
    return true;
}

void Fruit::respawn(int offsetX, int offsetY , int width, int height, const std::vector<SDL_Rect>& snakeBody) {
    // Tạo vị trí ngẫu nhiên trong gameBounds
    int cols = width / m_gridSize;
    int rows = height / m_gridSize;

    bool overlaps; 
    do{
        overlaps = false;
        int x = 1 + rand() % (cols - 2);  // Cột 1 đến cols-2
        int y = 1 + rand() % (rows - 2);  // Hàng 1 đến rows-2
    
    m_rect.x = offsetX + x * m_gridSize;
    m_rect.y = offsetY + y * m_gridSize;
    for (const auto& segment : snakeBody) {
        if (m_rect.x == segment.x && m_rect.y == segment.y) {
            overlaps = true;
            break;
        }
    }

    } while (overlaps);
}

bool Fruit::collidesWithSnake(const std::vector<SDL_Rect>& snakeBody) const {
    for (const SDL_Rect& segment : snakeBody) {
        if (segment.x == m_rect.x && segment.y == m_rect.y) {
            return true;
        }
    }
    return false;
}

SDL_Rect Fruit::getRect() const {
    return m_rect;
}

void Fruit::render(SDL_Renderer* renderer) {
    if (m_texture) {
        SDL_RenderCopy(renderer, m_texture, nullptr, &m_rect);
        SDL_Log("Fruit rendered at (%d, %d)", m_rect.x, m_rect.y);
    }else {
    SDL_Log("Fruit texture is null, fallback to green box.");
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // 🔴 Quả màu đỏ
    SDL_RenderFillRect(renderer, &m_rect);
    }
}