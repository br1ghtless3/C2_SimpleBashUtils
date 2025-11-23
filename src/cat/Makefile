CC = gcc
CFLAGC = -std=c11 -Wall -Werror -Wextra 

TARGET = s21_cat
SRCS = s21_cat.c parser.c file_processor.c
OBJECTS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGC) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean