CC = gcc
SRC = src/chip-8.c
CC_FLAGS = -Wall -Wextra -pedantic -fPIC
TARGET = chip-8
INC_PATH = -I lib
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $< $(CC_FLAGS) $(INC_PATH) -o $@ -lSDL2 

clean:
	rm -f $(TARGET)
	
	