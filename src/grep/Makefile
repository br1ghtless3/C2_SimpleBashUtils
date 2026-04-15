CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -pedantic
TARGET = s21_grep
SRCS = s21_grep.c
OBJECTS = $(SRCS:.c=.o)

s21_grep: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

all: s21_grep

.PHONY: all s21_grep clean