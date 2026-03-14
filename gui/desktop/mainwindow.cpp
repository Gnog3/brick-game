#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QPainter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), isGameStarted_(false), isGameOver_(false) {
  this->resize(420, 480);
  this->setFixedSize(420, 480);
  this->setWindowTitle("Game Console");

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &MainWindow::UpdateGame);
}

MainWindow::~MainWindow() {}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (isGameOver_) {
    QWidget::keyPressEvent(event);
    return;
  }

  if (!isGameStarted_) {
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
      isGameStarted_ = true;
      UpdateGame();
      timer_->start(5);
    }
  } else {
    switch (event->key()) {
      case Qt::Key_Left:
        userInput(Left, true);
        break;
      case Qt::Key_Right:
        userInput(Right, true);
        break;
      case Qt::Key_Up:
        userInput(Up, true);
        break;
      case Qt::Key_Down:
        userInput(Down, true);
        break;
      case Qt::Key_R:
        userInput(Action, true);
        break;
      case Qt::Key_P:
        userInput(Pause, true);
        break;
      case Qt::Key_Q:
        userInput(Terminate, true);
        break;
      default:
        QWidget::keyPressEvent(event);
        return;
    }
  }

  update();
}

void MainWindow::UpdateGame() {
  if (isGameOver_ || !isGameStarted_) {
    return;
  }

  gameInfo_ = updateCurrentState();

  if (gameInfo_.field == nullptr) {
    isGameOver_ = true;
    timer_->stop();
    ShowGameOverScreen();
    return;
  }

  update();
}

void MainWindow::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::darkGray);

  if (!isGameStarted_) {
    ShowStartScreen(painter);
  } else {
    DrawField(painter);
    DrawNextPiece(painter);
    DrawGameInfo(painter);

    if (gameInfo_.pause) {
      painter.setPen(Qt::white);
      painter.setFont(QFont("Arial", 28, QFont::Bold));
      painter.drawText(rect(), Qt::AlignCenter, "PAUSE");
    }
  }
}

void MainWindow::ShowStartScreen(QPainter& painter) {
  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 20, QFont::Bold));
  painter.drawText(rect(), Qt::AlignCenter, "PRESS ENTER TO START");
}

void MainWindow::DrawField(QPainter& painter) {
  if (!gameInfo_.field) return;

  int block_size = 20;
  int field_x_offset = 20;
  int field_y_offset = 20;

  for (int i = 0; i < PLAYING_FIELD_HEIGHT; ++i) {
    for (int j = 0; j < PLAYING_FIELD_WIDTH; ++j) {
      QRect block(field_x_offset + j * block_size,
                  field_y_offset + i * block_size, block_size, block_size);
      QColor color =
          (gameInfo_.field[i][j] != 0) ? QColor(30, 144, 255) : Qt::black;
      painter.fillRect(block, color);
      painter.setPen(Qt::darkGray);
      painter.drawRect(block);
    }
  }
}

void MainWindow::DrawNextPiece(QPainter& painter) {
  if (!gameInfo_.next) return;

  int block_size = 20;
  int next_x_offset = 250;
  int next_y_offset = 80;

  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 12, QFont::Bold));
  painter.drawText(next_x_offset, next_y_offset - 10, "NEXT:");

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (gameInfo_.next[i][j] != 0) {
        QRect block(next_x_offset + j * block_size,
                    next_y_offset + i * block_size, block_size, block_size);
        painter.fillRect(block, QColor(30, 144, 255));
        painter.setPen(Qt::darkGray);
        painter.drawRect(block);
      }
    }
  }
}

void MainWindow::DrawGameInfo(QPainter& painter) {
  int info_x_offset = 250;
  int info_y_offset = 220;

  painter.setPen(Qt::white);
  painter.setFont(QFont("Arial", 12, QFont::Bold));

  painter.drawText(info_x_offset, info_y_offset,
                   "SCORE: " + QString::number(gameInfo_.score));
  painter.drawText(info_x_offset, info_y_offset + 30,
                   "HIGHSCORE: " + QString::number(gameInfo_.high_score));
  painter.drawText(info_x_offset, info_y_offset + 60,
                   "LEVEL: " + QString::number(gameInfo_.level));
}

void MainWindow::ShowGameOverScreen() {
  QMessageBox::information(this, "GAME OVER",
                           "YOUR RESULT: " + QString::number(gameInfo_.score));
  QApplication::quit();
}
