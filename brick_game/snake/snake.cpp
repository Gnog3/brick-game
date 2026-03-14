#include <memory>

#include "../lib.h"
#include "gameengine.h"

static std::unique_ptr<s21::GameEngine> gameInstance = nullptr;

void init() {
  if (!gameInstance) {
    gameInstance = std::make_unique<s21::GameEngine>();
    gameInstance->StartGame();
  }
}

void userInput(UserAction_t action, bool hold) {
  init();
  if (hold) {
    gameInstance->ProcessInput(action);
  }
}

GameInfo_t updateCurrentState() {
  init();
  GameInfo_t game_info = gameInstance->GetUpdatedState();
  if (!game_info.field) {
    gameInstance.reset(nullptr);
  }
  return game_info;
}
