#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QTimer>

#include "../../brick_game/lib.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  void UpdateGame();

 private:
  void DrawField(QPainter& painter);
  void DrawNextPiece(QPainter& painter);
  void DrawGameInfo(QPainter& painter);
  void ShowStartScreen(QPainter& painter);
  void ShowGameOverScreen();

  QTimer* timer_;
  GameInfo_t gameInfo_;
  bool isGameStarted_;
  bool isGameOver_;
};

#endif
