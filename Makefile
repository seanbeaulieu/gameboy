CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCS = src/main.c src/bus.c src/cpu.c src/instruction.c src/prefix_instruction.c
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