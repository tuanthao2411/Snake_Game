#include "fruit.h"
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdio>

Fruit::Fruit(int gridSize, SDL_Rect bounds)
    : m_gridSize(gridSize), gen(rd())
{
    // Khởi tạo vị trí ngẫu nhiên
    
    m_rect.w = m_rect.h = m_gridSize;
    std::vector<SDL_Rect> empty;
    respawn(bounds.x, bounds.y, bounds.w, bounds.h, empty, nullptr, 0 );
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

void Fruit::respawn(int offsetX, int offsetY , int width, int height, const std::vector<SDL_Rect>& snakeBody, const SDL_FRect* fixedWalls, int wallSize) {
    // Tạo vị trí ngẫu nhiên trong gameBounds
    int cols = width / m_gridSize;
    int rows = height / m_gridSize;

    std::vector<SDL_Point> emptyCells;
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            SDL_Point cell = {offsetX + x * m_gridSize, offsetY + y * m_gridSize};
            bool isOccupied = false;

            // Kiểm tra va chạm với rắn
            for (const auto& segment : snakeBody) {
                if (cell.x == segment.x && cell.y == segment.y) {
                    isOccupied = true;
                    break;
                }
            }

            // Kiểm tra va chạm với tường cố định
            if (!isOccupied && fixedWalls != nullptr) {
                SDL_Rect cellRect = {cell.x, cell.y, m_gridSize, m_gridSize};
                for (int i = 0; i < wallSize; ++i) {
                    SDL_FRect wall = fixedWalls[i];
                    if (cellRect.x < wall.x + wall.w &&
                        cellRect.x + cellRect.w > wall.x &&
                        cellRect.y < wall.y + wall.h &&
                        cellRect.y + cellRect.h > wall.y) {
                        isOccupied = true;
                        break;
                    }
                }
            }


            if (!isOccupied) {
                emptyCells.push_back(cell);
            }
        }
    }

    // Chọn ngẫu nhiên một ô trống
    if (!emptyCells.empty()) {
        std::uniform_int_distribution<> dist(0, emptyCells.size() - 1);
        int index = dist(gen);
        m_rect.x = emptyCells[index].x;
        m_rect.y = emptyCells[index].y;
    } else {
        // Nếu không còn ô trống, xử lý đặc biệt (ví dụ, kết thúc trò chơi)
        SDL_Log("No empty cells available for fruit! Game Over.");
        m_rect.x = offsetX;
        m_rect.y = offsetY;
    }
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
        #ifdef DEBUG
    SDL_Log("Fruit rendered at (%d, %d)", m_rect.x, m_rect.y);
        #endif
    }else {
    SDL_Log("Fruit texture is null, fallback to green box.");
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &m_rect);
    }
}