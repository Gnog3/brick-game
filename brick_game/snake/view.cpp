#include "view.h"

#include "model.h"

namespace s21 {

View::View() {
  field_buffer_ = new int*[PLAYING_FIELD_HEIGHT];
  for (int i = 0; i < PLAYING_FIELD_HEIGHT; ++i) {
    field_buffer_[i] = new int[PLAYING_FIELD_WIDTH]();
  }
  info_ = {field_buffer_, nullptr, 0, 0, 1, 0, 0};
}

View::~View() {
  if (field_buffer_) {
    for (int i = 0; i < PLAYING_FIELD_HEIGHT; ++i) {
      delete[] field_buffer_[i];
    }
    delete[] field_buffer_;
  }
}

void View::Update(const Model& model) {
  if (model.current_state() == GameState::GAME_OVER) {
    info_.field = nullptr;
    return;
  }

  info_.field = field_buffer_;

  // Очистка поля
  for (int i = 0; i < PLAYING_FIELD_HEIGHT; ++i) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; ++j) {
      field_buffer_[i][j] = 0;
    }
  }

  // Отрисовка яблока
  const auto& apple = model.apple();
  field_buffer_[apple.second][apple.first] = 1;

  // Отрисовка змейки
  const auto& snake = model.snake();
  for (const auto& segment : snake) {
    field_buffer_[segment.second][segment.first] = 1;
  }

  // Обновление остальных полей
  info_.score = model.score();
  info_.high_score = model.high_score();
  info_.level = model.level();
  info_.speed = model.GetSpeed();
  info_.pause = (model.current_state() == GameState::PAUSED);
}

GameInfo_t View::info() const { return info_; }

}  // namespace s21
