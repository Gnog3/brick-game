#ifndef MODEL_H
#define MODEL_H

#include <chrono>
#include <string>
#include <vector>

#include "defines.h"

namespace s21 {

class TestableModel;

class Model {
 public:
  Model();

  void Update();

  void StartGame();
  void ChangeDirection(Direction newDirection);
  void TogglePause();
  void GameOver();
  void Action();

  GameState current_state() const;
  const std::vector<std::pair<int, int>>& snake() const;
  const std::pair<int, int>& apple() const;
  int score() const;
  int high_score() const;
  int GetSpeed() const;
  int level() const;

 private:
  void SpawnInitialState();
  void ProcessMove();
  void CalculateSpeed();
  void GrowSnakeAndScore();
  void PlaceApple();
  bool CheckCollision(const std::pair<int, int>& head);

  void SaveHighscore(int score_to_save);
  int LoadHighscore();

  GameState current_state_;
  int score_;
  int high_score_;
  int speed_;
  std::chrono::steady_clock::time_point last_update_time_;
  std::vector<std::pair<int, int>> snake_;
  std::pair<int, int> apple_;
  Direction current_direction_;
  Direction new_direction_;
  bool speed_up_;

  friend class TestableModel;
};

}  // namespace s21

#endif
