CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -fsanitize=address -fno-omit-frame-pointer $(shell sdl2-config --cflags)
LDFLAGS = -fsanitize=address $(shell sdl2-config --libs) -lcjson

SRCS = src/main.c src/bus.c src/cpu.c src/instruction.c src/prefix_instruction.c src/ppu.c src/jsontest.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -I include

TARGET = gameboy-emulator

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)