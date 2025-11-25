CC = gcc

CFLAGS  = -Wall -O2 -fopenmp \
-I./src/utils -I./src/game -I./src/render \
-I./src/render/menu -I./src/render/game -I./src/game/chunks \
-I./src/render/credits

LDFLAGS = -mconsole -lopengl32 -lgdi32 -lwinmm -lraylib

SRC_MAIN = $(wildcard src/*.c src/utils/*.c src/game/*.c src/render/menu/*.c \
src/render/*.c src/render/game/*.c src/game/chunks/*.c src/render/credits/*.c)

EXEC_MAIN = game.exe

$(EXEC_MAIN):
	$(CC) $(SRC_MAIN) -o $(EXEC_MAIN) $(CFLAGS) $(LDFLAGS)

run: $(EXEC_MAIN)
	./$(EXEC_MAIN)

clean:
	del /Q $(EXEC_MAIN) 2>nul || true

# mingw32-make clean && mingw32-make run