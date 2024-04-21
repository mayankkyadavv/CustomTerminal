# Defining the compiler
CC = gcc

# Compiler flags
CFLAGS = -Werror -std=c99

# Target executable
TARGET = myshell

# Source files
SOURCES = myshell.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET)
