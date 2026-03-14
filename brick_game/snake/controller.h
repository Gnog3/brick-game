#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../lib.h"

namespace s21 {

class Model;

class Controller {
 public:
  Controller(Model& model);
  void HandleUserInput(UserAction_t action);

 private:
  Model& model_;
};

}  // namespace s21

#endif
