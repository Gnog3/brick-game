CC := gcc
CXX := g++
CFLAGS := -std=c11 -Wall -Werror -Wextra
CXXFLAGS := -std=c++20 -Wall -Werror -Wextra
OBJS_TETRIS := build/brick_game/tetris/backend.o build/brick_game/tetris/figures.o build/brick_game/tetris/lib.o build/brick_game/tetris/save.o build/brick_game/tetris/update_funcs.o
OBJS_CLI := build/gui/cli/frontend.o
OBJS_SNAKE := build/cpp/brick_game/snake/controller.o build/cpp/brick_game/snake/gameengine.o build/cpp/brick_game/snake/model.o build/cpp/brick_game/snake/snake.o build/cpp/brick_game/snake/view.o
OBJS_DESKTOP := build/cpp/gui/desktop/mainwindow.o build/cpp/gui/desktop/main.o
SOURCES_TEST := brick_game/snake/model.cpp brick_game/snake/tests.cpp

QT_FLAGS = -fPIC
QT_INCLUDE_PATH = $(shell pkg-config --cflags Qt5Widgets Qt5Core)
QT_LIB_PATH = $(shell pkg-config --libs Qt5Widgets Qt5Core)
MOC = moc

BUILD_DIR = build

QT_HEADERS = gui/desktop/mainwindow.h
MOC_SOURCES = $(patsubst %.h, $(BUILD_DIR)/moc_%.cpp, $(notdir $(QT_HEADERS)))
MOC_OBJECTS = $(patsubst %.cpp, %.o, $(MOC_SOURCES))

all: tetris_cli snake_cli tetris_desktop snake_desktop

tetris_cli: $(OBJS_TETRIS) $(OBJS_CLI)
	$(CC) $(CFLAGS) $^ -o build/$@ -lncurses

tetris_desktop: $(OBJS_TETRIS) $(OBJS_DESKTOP) $(MOC_OBJECTS)
	$(CXX) $(CXXFLAGS) $(QT_FLAGS) $^ $(QT_LIB_PATH) -o build/tetris_desktop

snake_cli: $(OBJS_SNAKE) $(OBJS_CLI)
	$(CXX) $(CXXFLAGS) $^ -o build/$@ -lncurses

snake_desktop: $(OBJS_SNAKE) $(OBJS_DESKTOP) $(MOC_OBJECTS)
	$(CXX) $(CXXFLAGS) $(QT_FLAGS) $^ $(QT_LIB_PATH) -o build/snake_desktop

build/moc_mainwindow.cpp: gui/desktop/mainwindow.h
	@mkdir -p $(dir $@)
	$(MOC) $(QT_INCLUDE_PATH) $< -o $@

build/moc_%.o: build/moc_%.cpp
	$(CXX) $(CXXFLAGS) $(QT_FLAGS) $(QT_INCLUDE_PATH) -c $< -o $@

build/cpp/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(QT_FLAGS) $(QT_INCLUDE_PATH) -c $< -o $@

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r build

test:
	@mkdir -p build
	$(CXX) $(SOURCES_TEST) -o build/tests -lgtest
	./build/tests

gcov_report:
	@mkdir -p build
	$(CXX) --coverage $(SOURCES_TEST) -o build/tests -lgtest
	./build/tests
	lcov -t "tests" -o build/tests.info -c -d . --ignore-errors mismatch
	genhtml -o ./build/report build/tests.info
	xdg-open ./build/report/index.html

dvi:
	@cat dvi.txt

dist: all
	@mkdir -p dist/src
	cp -r brick_game gui Makefile dvi.txt dist/src
	cp build/snake_cli build/snake_desktop build/tetris_cli build/tetris_desktop dist
	tar -cf tetris.tar dist
	rm -rf dist 

install: all
	mkdir install
	cp build/snake_cli build/snake_desktop build/tetris_cli build/tetris_desktop install

uninstall:
	rm -r install