CC = gcc
LDFLAGS = -lopengl32 -lgdi32 -lwinmm -lraylib
CFLAGS  = -Wall -O2 -fopenmp -I./src/utils -I./src/tests -I./src/game -I./src/render -I./src/render/menu -I./src/render/game -I./src/game/chunks

# Compila todos os arquivos .c da src/ e subpastas game, render e utils
SRC_MAIN = $(wildcard src/*.c src/utils/*.c src/game/*.c src/render/menu/*.c src/render/*.c src/render/game/*.c src/game/chunks/*.c)
SRC_TEST = $(wildcard src/tests/*.c src/utils/*.c)

EXEC_MAIN = game.exe
EXEC_TEST = test.exe

# Compila o jogo principal com Raylib
$(EXEC_MAIN):
	$(CC) $(SRC_MAIN) -o $(EXEC_MAIN) $(CFLAGS) $(LDFLAGS)

# Compila os testes (sem Raylib)
$(EXEC_TEST):
	$(CC) $(SRC_TEST) -o $(EXEC_TEST) $(CFLAGS)

# Alvos de conveniência
run: $(EXEC_MAIN)
	./$(EXEC_MAIN)

test: $(EXEC_TEST)
	./$(EXEC_TEST)

clean:
	del /Q $(EXEC_MAIN) $(EXEC_TEST) 2>nul || true

# mingw32-make run
# mingw32-make test
# mingw32-make clean

# mingw32-make clean && mingw32-make run
