CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lncurses

SRC = src/main.c src/ui.c src/scanner.c
OBJ = $(SRC:.c=.o)

TARGET = fs-usage-analyzer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean