CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -fsanitize=address -fno-omit-frame-pointer
LDFLAGS = -fsanitize=address

SRCS = src/main.c src/bus.c src/cpu.c src/instruction.c src/prefix_instruction.c src/ppu.c
OBJS = $(SRCS:.c=.o)
INCLUDES = -I include

TARGET = gameboy-emulator

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)