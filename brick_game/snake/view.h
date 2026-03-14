#ifndef VIEW_H
#define VIEW_H

#include "../lib.h"

namespace s21 {

class Model;

class View {
 public:
  View();
  ~View();
  View(const View&) = delete;
  View& operator=(const View&) = delete;

  void Update(const Model& model);
  GameInfo_t info() const;

 private:
  GameInfo_t info_;
  int** field_buffer_;
};

}  // namespace s21

#endif
