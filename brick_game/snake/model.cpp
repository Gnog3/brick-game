#include "model.h"

#include <fstream>
#include <random>

#include "defines.h"

namespace s21 {

Model::Model() {
  current_state_ = GameState::SPAWNING;
  score_ = 0;
  high_score_ = LoadHighscore();
  speed_ = DEFAULT_SPEED;
  current_direction_ = Direction::UP;
  new_direction_ = Direction::UP;
  speed_up_ = false;
}

void Model::Update() {
  switch (current_state_) {
    case GameState::SPAWNING:
      SpawnInitialState();
      current_state_ = GameState::AWAITING_MOVE;
      last_update_time_ = std::chrono::steady_clock::now();
      break;

    case GameState::AWAITING_MOVE: {
      auto now = std::chrono::steady_clock::now();
      if (std::chrono::duration_cast<std::chrono::milliseconds>(
              now - last_update_time_)
              .count() > GetSpeed()) {
        current_state_ = GameState::PROCESSING_MOVE;
        last_update_time_ = now;
      }
      break;
    }

    case GameState::PROCESSING_MOVE:
      ProcessMove();
      break;

    case GameState::GROWING:
      GrowSnakeAndScore();
      CalculateSpeed();
      if (snake_.size() >= WIN_LENGTH) {
        current_state_ = GameState::GAME_OVER;
      } else {
        current_state_ = GameState::AWAITING_MOVE;
      }
      break;

    default:
      break;
  }
}

void Model::StartGame() { current_state_ = GameState::SPAWNING; }

void Model::ChangeDirection(Direction newDirection2) {
  if ((current_direction_ == Direction::UP &&
       newDirection2 != Direction::DOWN) ||
      (current_direction_ == Direction::DOWN &&
       newDirection2 != Direction::UP) ||
      (current_direction_ == Direction::LEFT &&
       newDirection2 != Direction::RIGHT) ||
      (current_direction_ == Direction::RIGHT &&
       newDirection2 != Direction::LEFT)) {
    new_direction_ = newDirection2;
  }
}

void Model::TogglePause() {
  if (current_state_ == GameState::PAUSED) {
    current_state_ = GameState::AWAITING_MOVE;
    last_update_time_ = std::chrono::steady_clock::now();
  } else if (current_state_ == GameState::AWAITING_MOVE) {
    current_state_ = GameState::PAUSED;
  }
}

void Model::GameOver() { current_state_ = GameState::GAME_OVER; }

void Model::Action() { speed_up_ = !speed_up_; }

GameState Model::current_state() const { return current_state_; };

const std::vector<std::pair<int, int>>& Model::snake() const { return snake_; }

const std::pair<int, int>& Model::apple() const { return apple_; }

int Model::score() const { return score_; }

int Model::high_score() const { return high_score_; }

int Model::GetSpeed() const { return speed_ / (speed_up_ ? 2 : 1); }

int Model::level() const { return std::min(score_ / 5 + 1, 10); }

void Model::SpawnInitialState() {
  score_ = 0;
  speed_ = 500;
  current_direction_ = Direction::UP;

  int start_x = PLAYING_FIELD_WIDTH / 2;
  int start_y = PLAYING_FIELD_HEIGHT - INITIAL_SNAKE_LENGTH - 1;

  snake_.clear();
  for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
    snake_.push_back({start_x, start_y + i});
  }

  PlaceApple();
}

void Model::ProcessMove() {
  current_direction_ = new_direction_;
  auto head = snake_.front();
  std::pair<int, int> next_head = head;
  switch (current_direction_) {
    case Direction::UP:
      next_head.second--;
      break;
    case Direction::DOWN:
      next_head.second++;
      break;
    case Direction::LEFT:
      next_head.first--;
      break;
    case Direction::RIGHT:
      next_head.first++;
      break;
  }

  if (CheckCollision(next_head)) {
    current_state_ = GameState::GAME_OVER;
    return;
  }

  if (next_head == apple_) {
    snake_.insert(snake_.begin(), next_head);
    current_state_ = GameState::GROWING;
  } else {
    snake_.insert(snake_.begin(), next_head);
    snake_.pop_back();
    current_state_ = GameState::AWAITING_MOVE;
  }
}

void Model::CalculateSpeed() {
  speed_ = DEFAULT_SPEED / (1.0 + 0.2 * (level() - 1));
}

void Model::GrowSnakeAndScore() {
  score_++;
  if (score_ > high_score_) {
    high_score_ = score_;
    SaveHighscore(high_score_);
  }
  PlaceApple();
}

void Model::PlaceApple() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribX(0, PLAYING_FIELD_WIDTH - 1);
  std::uniform_int_distribution<> distribY(0, PLAYING_FIELD_HEIGHT - 1);

  bool on_snake;
  do {
    on_snake = false;
    apple_ = {distribX(gen), distribY(gen)};
    for (const auto& segment : snake_) {
      if (segment == apple_) {
        on_snake = true;
        break;
      }
    }
  } while (on_snake);
}

bool Model::CheckCollision(const std::pair<int, int>& head) {
  if (head.first < 0 || head.first >= PLAYING_FIELD_WIDTH || head.second < 0 ||
      head.second >= PLAYING_FIELD_HEIGHT) {
    return true;
  }

  for (size_t i = 1; i < snake_.size() - 1; ++i) {
    if (head == snake_[i]) {
      return true;
    }
  }
  return false;
}

void Model::SaveHighscore(int score_to_save) {
  std::ofstream file(HIGHSCORE_FILENAME);
  if (file.is_open()) {
    file << score_to_save;
  }
}

int Model::LoadHighscore() {
  int loaded_score = 0;
  std::ifstream file(HIGHSCORE_FILENAME);
  if (file.is_open()) {
    file >> loaded_score;
  }
  return loaded_score;
}

}  // namespace s21
