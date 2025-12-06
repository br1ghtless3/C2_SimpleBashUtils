CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
TARGET = s21_cat
SRC = s21_cat.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c s21_cat.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild