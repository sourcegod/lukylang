# makefile for luky project
# Last Update: 29/03/2020

CC := g++
# condition for passing to "make" debug as option
# set DEBUG at 0 if no option
DEBUG ?= 0 
ifeq ($(DEBUG),1)
	CFLAGS := -std=c++11 -DDEBUG -Wall -Wextra -pedantic -g
else
	CFLAGS := -std=c++11 -DNDEBUG -Wall -Wextra -pedantic -g
endif

DEBFLAGS := -std=c++11 -DDEBUG -Wall -Wextra -pedantic -g

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
	$(CC) $(CFLAGS)  -c -o $@ $<
	# $(CC) -MM $(CFLAGS) $(SRC_DIR)/$*.cpp > $(BUILD_DIR)/$*.d


.PHONY: build clean release

clean = rm -f $(BUILD_DIR)/*.o ./$(TARGET)


clean:
	$(clean)


# Run the interpreter
run:
	rlwrap ./$(TARGET)
	#
# Debug the interpreter with gdb
gdb:
	rlwrap gdb ./$(TARGET)

rebuild: clean all


# -include $(OBJS:.o=.d)
