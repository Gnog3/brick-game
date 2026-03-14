#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "controller.h"
#include "model.h"
#include "view.h"

namespace s21 {

class GameEngine {
 public:
  GameEngine();
  void ProcessInput(UserAction_t action);
  GameInfo_t GetUpdatedState();
  void StartGame();

 private:
  Model model_;
  View view_;
  Controller controller_;
};

}  // namespace s21

#endif
