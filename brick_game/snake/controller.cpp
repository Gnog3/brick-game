#include "controller.h"

#include "model.h"

namespace s21 {

Controller::Controller(Model& model) : model_(model) {}

void Controller::HandleUserInput(UserAction_t action) {
  switch (action) {
    case Up:
      model_.ChangeDirection(Direction::UP);
      break;
    case Down:
      model_.ChangeDirection(Direction::DOWN);
      break;
    case Left:
      model_.ChangeDirection(Direction::LEFT);
      break;
    case Right:
      model_.ChangeDirection(Direction::RIGHT);
      break;
    case Pause:
      model_.TogglePause();
      break;
    case Terminate:
      model_.GameOver();
      break;
    case Action:
      model_.Action();
      break;
    default:
      break;
  }
}

}  // namespace s21
