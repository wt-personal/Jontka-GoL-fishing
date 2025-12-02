# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99 -I"C:/raylib/include"   # Add Raylib include here

# Raylib linking flags for MinGW
LIBS = -L"C:/raylib/lib" -lraylib -lopengl32 -lgdi32 -lwinmm

# Target executable
TARGET = game_of_life.exe

# Source and object files
SRCS = main.c gol_file.c render.c
OBJS = $(SRCS:.c=.o)

# Default rule
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Compile .c files into .o files
%.o: %.c header.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	cmd /C del /Q $(OBJS) $(TARGET) 2>nul || exit 0
