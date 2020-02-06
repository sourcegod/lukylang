CC=g++
CFLAGS=-Wall -Wextra
BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
SRCS = $(wildcard *.cpp)
OBJs = $(wildcard *.o)
luky: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

main.o: $(SRCS)
	$(CC) -o -c $@ $^ $(CFLAGS)


.phony: clean
clean:
	rm -f $(OBJS) luky

