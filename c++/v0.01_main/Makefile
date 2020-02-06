# Simple makefile to compile all *.cpp files in a directory

CC := g++
CFLAGS := -std=c++11 -Wall -Wextra -pedantic -g
SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/luky
# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current
# directory and dir "/xxx/xxx/"
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to
# ".o" file name strings
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
# OBJS := $(SRCS:.cpp=.o)
# for debugging
# $(info srcs:  $(SRCS))
# $(info objs:  $(OBJS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -v $(OBJS) $(TARGET)

