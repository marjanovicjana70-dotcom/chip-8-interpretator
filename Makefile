CC = gcc
LIB = chip-8
TARGET = chip-8.h
SRC = src/chip-8.c


CC_FLAGS = -Wall -Wextra -pedantic -std=99

.PHONY all clear

all: $(TARGET)
	make $(TARGET)

$(TARGET): 

