CC := gcc
CFLAGS := -std=c11 -Wall -Werror -Wextra
OBJS := build/brick_game/tetris/backend.o build/gui/cli/frontend.o build/brick_game/tetris/figures.o build/brick_game/tetris/update_funcs.o build/brick_game/tetris/save.o
SOURCES_TEST := brick_game/tetris/backend.c brick_game/tetris/figures.c brick_game/tetris/update_funcs.c brick_game/tetris/save.c brick_game/tetris/tests.c

all: clean tetris

tetris: $(OBJS)
	$(CC) $(CFLAGS) $^ -o build/$@ -lncurses

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -r build

test:
	@mkdir -p build
	$(CC) $(SOURCES_TEST) -lncurses -o build/tests -lcheck -lsubunit -lm
	./build/tests

gcov_report:
	@mkdir -p build
	$(CC) --coverage $(SOURCES_TEST) -lncurses -o build/tests -lcheck -lsubunit -lm
	./build/tests
	lcov -t "tests" -o build/tests.info -c -d .
	genhtml -o ./build/report build/tests.info
	xdg-open ./build/report/index.html

dvi:
	@cat dvi.txt

dist: all
	mkdir dist
	mkdir dist/src
	cp -r brick_game gui Makefile dvi.txt dist/src
	cp build/tetris dist
	tar -cf tetris.tar dist
	rm -rf dist 

install: clean all
	mkdir install
	cp build/tetris install

uninstall:
	rm -r install