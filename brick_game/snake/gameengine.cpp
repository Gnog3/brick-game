#include "gameengine.h"

namespace s21 {

GameEngine::GameEngine() : model_(), view_(), controller_(model_) {}

void GameEngine::ProcessInput(UserAction_t action) {
  controller_.HandleUserInput(action);
}

GameInfo_t GameEngine::GetUpdatedState() {
  model_.Update();
  view_.Update(model_);
  return view_.info();
}

void GameEngine::StartGame() { model_.StartGame(); }

}  // namespace s21
