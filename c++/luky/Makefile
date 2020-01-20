# makefile for luky project
# Last Update: 08/09/19

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

# pull in dependency info for *existing* .o files
# TODO: how to rebuild when headerfiles are changed
# not ideal: 
#$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp $(SRC_DIR)/main.cpp
# compile and generate dependency info
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
	# $(CC) -MM $(CFLAGS) $(SRC_DIR)/$*.cpp > $(BUILD_DIR)/$*.d


clean:
	rm -f $(BUILD_DIR)/*.o ./$(TARGET)
# Run the interpreter
run:
	rlwrap ./$(TARGET)
	#
# Debug the interpreter with gdb
debug:
	rlwrap gdb ./$(TARGET)


# -include $(OBJS:.o=.d)
