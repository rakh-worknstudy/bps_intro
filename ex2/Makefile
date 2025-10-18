CC = gcc

CFLAGS = -I./src -Wall -Wextra
LDFLAGS =

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

TARGET = ex2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o build/$@

%.o: src/%.c
	$(CC) $(CFLAGS) -c $^

clean:
	rm $(OBJS)

.PHONY: all clean

