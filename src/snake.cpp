#include "snake.h"
#include <iostream>

Snake::Snake(int gridSize, SDL_Rect bounds)
    : m_gridSize(gridSize), m_direction(RIGHT) ,m_bounds(bounds)
{
    // Khởi tạo con rắn với một đoạn đầu
    m_body.push_back({
        bounds.x + gridSize * 5,
        bounds.y + gridSize * 5,
        gridSize,
        gridSize
    });
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
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh cho rắn
    for (const auto& segment : m_body) {
        SDL_RenderFillRect(renderer, &segment);
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


