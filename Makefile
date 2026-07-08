NAME			= conan-game
BIN				= $(NAME)
BIN_DIR   		= $(PWD)/bin
TARGET 			= $(BIN_DIR)/$(BIN)
TEST_BIN		= $(BIN_DIR)/tests

CC = g++
LIB = -L/usr/local/lib -Wl,-rpath=/usr/local/lib \
	-lstormenginev2 \
	-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -ltinyxml2
INCLUDE = -I/usr/local/include
CCFLAGS = -Wall -c -g -std=c++17 \
	-Wno-reorder -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function $(INCLUDE)

SRCS	= $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJS 	= $(SRCS:.cpp=.o)

all: clean $(TARGET)

$(TARGET) : $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $^ $(LIB) -o $@

%.o: %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f bin/* && rm -f src/*.o src/**/*.o specs/*.o specs/**/*.o

run:
	$(TARGET)

# ── Specs (igloo, pure headers — no SDL needed to link) ─────────────────────
SPECSRCS  = $(wildcard specs/*.cpp) $(wildcard specs/**/*.cpp)
SPECOBJS  = $(SPECSRCS:.cpp=.o)

test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): $(SPECOBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

memcheck:
	valgrind --log-file=valgrind.output --leak-check=yes --leak-check=full --tool=memcheck -s $(TARGET)
