NAME			= conan-game
BIN				= $(NAME)
BIN_DIR   		= $(PWD)/bin
TARGET 			= $(BIN_DIR)/$(BIN)
DATA_PREFIX   	= $(PWD)/data/

CC = g++
LIB = -L/usr/local/lib -Wl,-rpath=/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lz -ltinyxml
INCLUDE = -isystem -I/usr/local/include
CCFLAGS = -Wall -c -g -std=c++17  -DDATA_PREFIX=\"$(DATA_PREFIX)\" \
	-Wno-reorder -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function $(INCLUDE) 

SRCS	= $(wildcard src/*.cpp)
OBJS 	= $(SRCS:.cpp=.o)

all: clean $(TARGET)

$(TARGET) : $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $^ $(LIB) -o $@
	/usr/bin/time -f "Compilation completed in : %E" $(TARGET)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f bin/* && rm -f $(shell find $(ROOT_DIR) -name "*.o")

run:
	$(TARGET)
	
memcheck:
	valgrind --log-file=valgrind.output --leak-check=yes --leak-check=full --tool=memcheck -s $(TARGET)


