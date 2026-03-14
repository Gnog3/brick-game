#include <gtest/gtest.h>

#include <chrono>

#include "defines.h"
#include "model.h"

namespace s21 {

class ModelTest : public testing::Test {
 protected:
  void SetUp() override { std::remove(HIGHSCORE_FILENAME); }

  void TearDown() override { std::remove(HIGHSCORE_FILENAME); }

  Model model;
};

class TestableModel : public Model {
 public:
  void testSpawnInitialState() { SpawnInitialState(); }
  void testProcessMove() { ProcessMove(); }
  void testCalculateSpeed() { CalculateSpeed(); }
  void testGrowSnakeAndScore() { GrowSnakeAndScore(); }
  void testPlaceApple() { PlaceApple(); }
  bool testCheckCollision(const std::pair<int, int>& head) {
    return CheckCollision(head);
  }
  void testSaveHighscore(int score) { SaveHighscore(score); }
  int testLoadHighscore() { return LoadHighscore(); }

  GameState& getStateRef() { return current_state_; }
  int& getScoreRef() { return score_; }
  int& getHighScoreRef() { return high_score_; }
  int& getSpeedRef() { return speed_; }
  std::vector<std::pair<int, int>>& getSnakeRef() { return snake_; }
  std::pair<int, int>& getAppleRef() { return apple_; }
  Direction& getCurrentDirectionRef() { return current_direction_; }
  Direction& getNewDirectionRef() { return new_direction_; }
  std::chrono::steady_clock::time_point& getLastUpdateTimeRef() {
    return last_update_time_;
  }
};

class TestableModelTest : public testing::Test {
 protected:
  void SetUp() override { std::remove(HIGHSCORE_FILENAME); }

  void TearDown() override { std::remove(HIGHSCORE_FILENAME); }

  TestableModel model;
};

TEST_F(ModelTest, ConstructorInitializesCorrectly) {
  EXPECT_EQ(model.current_state(), GameState::SPAWNING);
  EXPECT_EQ(model.score(), 0);
  EXPECT_EQ(model.GetSpeed(), DEFAULT_SPEED);
}

TEST_F(ModelTest, StartGameSetsSpawningState) {
  Model testModel;
  testModel.StartGame();
  EXPECT_EQ(testModel.current_state(), GameState::SPAWNING);
}

TEST_F(ModelTest, GameOverSetsGameOverState) {
  model.GameOver();
  EXPECT_EQ(model.current_state(), GameState::GAME_OVER);
}

TEST_F(ModelTest, ChangeDirectionValidMoves) {
  TestableModel testModel;
  testModel.getCurrentDirectionRef() = Direction::UP;

  testModel.ChangeDirection(Direction::LEFT);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::LEFT);

  testModel.ChangeDirection(Direction::RIGHT);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::RIGHT);

  testModel.ChangeDirection(Direction::DOWN);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::RIGHT);
}

TEST_F(ModelTest, ChangeDirectionInvalidMoves) {
  TestableModel testModel;

  testModel.getCurrentDirectionRef() = Direction::UP;
  testModel.getNewDirectionRef() = Direction::UP;
  testModel.ChangeDirection(Direction::DOWN);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::UP);

  testModel.getCurrentDirectionRef() = Direction::DOWN;
  testModel.getNewDirectionRef() = Direction::DOWN;
  testModel.ChangeDirection(Direction::UP);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::DOWN);

  testModel.getCurrentDirectionRef() = Direction::LEFT;
  testModel.getNewDirectionRef() = Direction::LEFT;
  testModel.ChangeDirection(Direction::RIGHT);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::LEFT);

  testModel.getCurrentDirectionRef() = Direction::RIGHT;
  testModel.getNewDirectionRef() = Direction::RIGHT;
  testModel.ChangeDirection(Direction::LEFT);
  EXPECT_EQ(testModel.getNewDirectionRef(), Direction::RIGHT);
}

TEST_F(TestableModelTest, TogglePauseFromAwaitingMove) {
  model.getStateRef() = GameState::AWAITING_MOVE;
  model.TogglePause();
  EXPECT_EQ(model.current_state(), GameState::PAUSED);
}

TEST_F(TestableModelTest, TogglePauseFromPaused) {
  model.getStateRef() = GameState::PAUSED;
  model.TogglePause();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
}

TEST_F(TestableModelTest, SpawnInitialStateCorrectly) {
  model.testSpawnInitialState();

  EXPECT_EQ(model.score(), 0);
  EXPECT_EQ(model.getSpeedRef(), DEFAULT_SPEED);
  EXPECT_EQ(model.getCurrentDirectionRef(), Direction::UP);
  EXPECT_EQ(model.getSnakeRef().size(), INITIAL_SNAKE_LENGTH);

  int start_x = PLAYING_FIELD_WIDTH / 2;
  int start_y = PLAYING_FIELD_HEIGHT - INITIAL_SNAKE_LENGTH - 1;

  for (size_t i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
    EXPECT_EQ(model.getSnakeRef()[i].first, start_x);
    EXPECT_EQ(model.getSnakeRef()[i].second, start_y + i);
  }
}

TEST_F(TestableModelTest, GetLevelCalculation) {
  model.getScoreRef() = 0;
  EXPECT_EQ(model.level(), 1);

  model.getScoreRef() = 5;
  EXPECT_EQ(model.level(), 2);

  model.getScoreRef() = 45;
  EXPECT_EQ(model.level(), 10);

  model.getScoreRef() = 100;
  EXPECT_EQ(model.level(), 10);
}

TEST_F(TestableModelTest, CalculateSpeedCorrectly) {
  model.getScoreRef() = 0;
  model.testCalculateSpeed();
  EXPECT_EQ(model.GetSpeed(), DEFAULT_SPEED);

  model.getScoreRef() = 5;
  model.testCalculateSpeed();
  int expectedSpeed = DEFAULT_SPEED / (1.0 + 0.2 * (2 - 1));
  EXPECT_EQ(model.GetSpeed(), expectedSpeed);
}

TEST_F(TestableModelTest, CheckCollisionWalls) {
  model.testSpawnInitialState();

  EXPECT_TRUE(model.testCheckCollision({-1, 5}));
  EXPECT_TRUE(model.testCheckCollision({PLAYING_FIELD_WIDTH, 5}));
  EXPECT_TRUE(model.testCheckCollision({5, -1}));
  EXPECT_TRUE(model.testCheckCollision({5, PLAYING_FIELD_HEIGHT}));

  EXPECT_FALSE(model.testCheckCollision({5, 5}));
}

TEST_F(TestableModelTest, CheckCollisionSnakeBody) {
  model.testSpawnInitialState();

  if (model.getSnakeRef().size() > 2) {
    EXPECT_TRUE(model.testCheckCollision(model.getSnakeRef()[1]));
  }
}

TEST_F(TestableModelTest, PlaceAppleNotOnSnake) {
  model.testSpawnInitialState();
  model.testPlaceApple();

  bool appleOnSnake = false;
  for (const auto& segment : model.getSnakeRef()) {
    if (segment == model.getAppleRef()) {
      appleOnSnake = true;
      break;
    }
  }
  EXPECT_FALSE(appleOnSnake);

  EXPECT_GE(model.getAppleRef().first, 0);
  EXPECT_LT(model.getAppleRef().first, PLAYING_FIELD_WIDTH);
  EXPECT_GE(model.getAppleRef().second, 0);
  EXPECT_LT(model.getAppleRef().second, PLAYING_FIELD_HEIGHT);
}

TEST_F(TestableModelTest, GrowSnakeAndScore) {
  model.testSpawnInitialState();
  int initialScore = model.score();
  int initialHighScore = model.high_score();

  model.testGrowSnakeAndScore();

  EXPECT_EQ(model.score(), initialScore + 1);
  EXPECT_GE(model.high_score(), initialHighScore);
}

TEST_F(TestableModelTest, ProcessMoveUp) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::UP;
  model.getNewDirectionRef() = Direction::UP;

  auto initialHead = model.getSnakeRef().front();
  model.testProcessMove();

  auto newHead = model.getSnakeRef().front();
  EXPECT_EQ(newHead.first, initialHead.first);
  EXPECT_EQ(newHead.second, initialHead.second - 1);
}

TEST_F(TestableModelTest, ProcessMoveDown) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::LEFT;
  model.getNewDirectionRef() = Direction::LEFT;

  auto initialHead = model.getSnakeRef().front();
  model.testProcessMove();

  model.getCurrentDirectionRef() = Direction::DOWN;
  model.getNewDirectionRef() = Direction::DOWN;

  model.testProcessMove();

  auto newHead = model.getSnakeRef().front();
  EXPECT_EQ(newHead.first, initialHead.first - 1);
  EXPECT_EQ(newHead.second, initialHead.second + 1);
}

TEST_F(TestableModelTest, ProcessMoveLeft) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::LEFT;
  model.getNewDirectionRef() = Direction::LEFT;

  auto initialHead = model.getSnakeRef().front();
  model.testProcessMove();

  auto newHead = model.getSnakeRef().front();
  EXPECT_EQ(newHead.first, initialHead.first - 1);
  EXPECT_EQ(newHead.second, initialHead.second);
}

TEST_F(TestableModelTest, ProcessMoveRight) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::RIGHT;
  model.getNewDirectionRef() = Direction::RIGHT;

  auto initialHead = model.getSnakeRef().front();
  model.testProcessMove();

  auto newHead = model.getSnakeRef().front();
  EXPECT_EQ(newHead.first, initialHead.first + 1);
  EXPECT_EQ(newHead.second, initialHead.second);
}

TEST_F(TestableModelTest, ProcessMoveCollision) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::UP;
  model.getNewDirectionRef() = Direction::UP;

  auto& snake = model.getSnakeRef();
  for (auto& segment : snake) {
    segment.second = 0;
  }

  model.testProcessMove();
  EXPECT_EQ(model.current_state(), GameState::GAME_OVER);
}

TEST_F(TestableModelTest, ProcessMoveEatApple) {
  model.testSpawnInitialState();
  model.getCurrentDirectionRef() = Direction::UP;
  model.getNewDirectionRef() = Direction::UP;

  auto head = model.getSnakeRef().front();
  model.getAppleRef() = {head.first, head.second - 1};

  size_t initialSize = model.getSnakeRef().size();
  model.testProcessMove();

  EXPECT_EQ(model.getSnakeRef().size(), initialSize + 1);
  EXPECT_EQ(model.current_state(), GameState::GROWING);
}

TEST_F(TestableModelTest, SaveAndLoadHighscore) {
  int testScore = 42;
  model.testSaveHighscore(testScore);

  int loadedScore = model.testLoadHighscore();
  EXPECT_EQ(loadedScore, testScore);
}

TEST_F(TestableModelTest, LoadHighscoreNonExistentFile) {
  std::remove(HIGHSCORE_FILENAME);
  int loadedScore = model.testLoadHighscore();
  EXPECT_EQ(loadedScore, 0);
}

TEST_F(TestableModelTest, UpdateSpawningState) {
  model.getStateRef() = GameState::SPAWNING;
  model.Update();

  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
  EXPECT_EQ(model.getSnakeRef().size(), INITIAL_SNAKE_LENGTH);
}

TEST_F(TestableModelTest, UpdateAwaitingMoveState) {
  model.getStateRef() = GameState::AWAITING_MOVE;
  model.getLastUpdateTimeRef() =
      std::chrono::steady_clock::now() - std::chrono::milliseconds(600);
  model.getSpeedRef() = 500;

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::PROCESSING_MOVE);
}

TEST_F(TestableModelTest, UpdateAwaitingMoveStateNotEnoughTime) {
  model.getStateRef() = GameState::AWAITING_MOVE;
  model.getLastUpdateTimeRef() = std::chrono::steady_clock::now();
  model.getSpeedRef() = 500;

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
}

TEST_F(TestableModelTest, UpdateProcessingMoveState) {
  model.testSpawnInitialState();
  model.getStateRef() = GameState::PROCESSING_MOVE;
  model.getCurrentDirectionRef() = Direction::RIGHT;
  model.getNewDirectionRef() = Direction::RIGHT;

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
}

TEST_F(TestableModelTest, UpdateGrowingState) {
  model.testSpawnInitialState();
  model.getStateRef() = GameState::GROWING;

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
}

TEST_F(TestableModelTest, UpdateGrowingStateWinCondition) {
  model.testSpawnInitialState();
  model.getStateRef() = GameState::GROWING;

  auto& snake = model.getSnakeRef();
  snake.clear();
  for (int i = 0; i < WIN_LENGTH; ++i) {
    snake.push_back({i, 0});
  }

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::GAME_OVER);
}

TEST_F(TestableModelTest, UpdateDefaultCase) {
  model.getStateRef() = GameState::GAME_OVER;
  model.Update();
  EXPECT_EQ(model.current_state(), GameState::GAME_OVER);
}

TEST_F(TestableModelTest, CompleteGameFlow) {
  model.StartGame();
  EXPECT_EQ(model.current_state(), GameState::SPAWNING);

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);

  model.ChangeDirection(Direction::RIGHT);

  model.getLastUpdateTimeRef() =
      std::chrono::steady_clock::now() - std::chrono::milliseconds(600);
  model.Update();
  EXPECT_EQ(model.current_state(), GameState::PROCESSING_MOVE);

  model.Update();
  EXPECT_EQ(model.current_state(), GameState::AWAITING_MOVE);
}

}  // namespace s21

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
