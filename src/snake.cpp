#include "snake.h"
#include <iostream>
#include <SDL2/SDL_image.h>

Snake::Snake(int gridSize, SDL_Rect bounds, const SDL_FRect* fixedWalls, int wallSize)
    : m_gridSize(gridSize), m_direction(RIGHT) ,m_bounds(bounds)
{
    // Khởi tạo con rắn với một đoạn đầu
    SDL_Rect head = {
        bounds.x + gridSize * 5,
        bounds.y + gridSize * 5,
        gridSize,
        gridSize
    };
    bool safe = true;
    if (fixedWalls != nullptr) {
        for (int i = 0; i < wallSize; ++i) {
            SDL_FRect wall = fixedWalls[i];
            if (head.x < wall.x + wall.w && head.x + head.w > wall.x &&
                head.y < wall.y + wall.h && head.y + head.h > wall.y) {
                safe = false;
                break;
            }
        }
    }
    if (safe) {
        m_body.push_back(head);
    } else {
        // Nếu không an toàn, thử vị trí khác
        m_body.push_back({bounds.x + gridSize * 10, bounds.y + gridSize * 10, gridSize, gridSize});
        SDL_Log("Initial snake position was unsafe, moved to (%d, %d)", bounds.x + gridSize * 10, bounds.y + gridSize * 10);
    }
}

Snake::~Snake() {
    if (m_headTexture) {
        SDL_DestroyTexture(m_headTexture);
        m_headTexture = nullptr;
    }
    if (m_bodyTexture) {
        SDL_DestroyTexture(m_bodyTexture);
        m_bodyTexture = nullptr;
    }
}

bool Snake::loadHeadTexture(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        SDL_Log("Không thể tải hình ảnh đầu rắn: %s | Đường dẫn: %s", IMG_GetError(), imagePath);
        return false;
    }

    m_headTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!m_headTexture) {
        SDL_Log("Không thể tạo texture đầu rắn: %s", SDL_GetError());
        return false;
    }
    SDL_Log("Tải texture đầu rắn thành công!");
    return true;
}

bool Snake::loadBodyTexture(SDL_Renderer* renderer, const char* imagePath) {
    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        SDL_Log("Không thể tải hình ảnh thân rắn: %s | Đường dẫn: %s", IMG_GetError(), imagePath);
        return false;
    }

    m_bodyTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!m_bodyTexture) {
        SDL_Log("Không thể tạo texture thân rắn: %s", SDL_GetError());
        return false;
    }
    SDL_Log("Tải texture thân rắn thành công!");
    return true;
}

void Snake::move() {
    // Lấy vị trí đầu con rắn
    SDL_Rect newHead = m_body.front();

    // Dịch chuyển theo hướng hiện tại
    switch (m_direction) {
        case UP:    newHead.y -= m_gridSize; break;
        case DOWN:  newHead.y += m_gridSize; break;
        case LEFT:  newHead.x -= m_gridSize; break;
        case RIGHT: newHead.x += m_gridSize; break;
    }

    // Thêm đầu mới vào danh sách body
    m_body.insert(m_body.begin(), newHead);

    // Xóa đuôi con rắn (nếu không ăn táo)
    m_body.pop_back();
}

bool Snake::isOutsideBounds() const {
    const SDL_Rect& head = m_body.front();
    return head.x < m_bounds.x || head.x >= m_bounds.x + m_bounds.w ||
           head.y < m_bounds.y || head.y >= m_bounds.y + m_bounds.h;
}

void Snake::changeDirection(Direction newDirection) {
    // Không cho phép rắn quay 180 độ
    if ((newDirection == UP && m_direction != DOWN) ||
        (newDirection == DOWN && m_direction != UP) ||
        (newDirection == LEFT && m_direction != RIGHT) ||
        (newDirection == RIGHT && m_direction != LEFT)) {
        m_direction = newDirection;
    }
}

bool Snake::checkCollisionWithSelf() const {
    // Kiểm tra va chạm giữa đầu và các đoạn thân
    SDL_Rect head = m_body.front();
    for (size_t i = 1; i < m_body.size(); ++i) {
        if (head.x == m_body[i].x && head.y == m_body[i].y) {
            return true;  // Va chạm với chính mình
        }
    }
    return false;
}

void Snake::render(SDL_Renderer* renderer) const {
    for (size_t i = 0; i < m_body.size(); ++i) {
        const auto& segment = m_body[i];
        if (i == 0 && m_headTexture) {
            SDL_RenderCopy(renderer, m_headTexture, nullptr, &segment);
        } else if (m_bodyTexture) {
            SDL_RenderCopy(renderer, m_bodyTexture, nullptr, &segment);
        } else {
            SDL_Log("Texture rắn bị null, vẽ hộp màu xanh thay thế.");
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &segment);
        }
    }
}

void Snake::grow() {
    // Thêm một đoạn vào đuôi rắn (thêm đoạn ở vị trí cuối cùng)
    SDL_Rect tail = m_body.back();
    m_body.push_back(tail);
}

SDL_Rect Snake::getHead() const {
    return m_body.front(); // Trả về đầu của con rắn
}

const std::vector<SDL_Rect>& Snake::getBody() const {
    return m_body;
}


