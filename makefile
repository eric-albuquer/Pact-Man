CC = gcc

CFLAGS  = -Wall -O1 -fopenmp \
-I./src/utils -I./src/game -I./src/render \
-I./src/render/menu -I./src/render/game -I./src/game/chunks \
-I./src/render/credits

LDFLAGS = -mconsole -lopengl32 -lgdi32 -lwinmm -lraylib

SRC_MAIN = $(wildcard src/*.c src/utils/*.c src/game/*.c src/render/menu/*.c \
src/render/*.c src/render/game/*.c src/game/chunks/*.c src/render/credits/*.c)

SRC_DATABASE = $(wildcard src/utils/*.c data/*.c)

EXEC_MAIN = Pact-Man.exe

EXEC_DATABASE = manager.exe

$(EXEC_MAIN):
	$(CC) $(SRC_MAIN) -o $(EXEC_MAIN) $(CFLAGS) $(LDFLAGS)

$(EXEC_DATABASE):
	$(CC) $(SRC_DATABASE) -o $(EXEC_DATABASE) $(CFLAGS) $(LDFLAGS)

run: $(EXEC_MAIN)
	./$(EXEC_MAIN)

database: $(EXEC_DATABASE)
	./$(EXEC_DATABASE)

clean:
	del /Q $(EXEC_MAIN) $(EXEC_DATABASE) 2>nul || true

# Compilar e executar o jogo:
# mingw32-make clean && mingw32-make run

# Compilar e executar o banco de dados:
# mingw32-make clean && mingw32-make database