CXX := g++
CXXFLAGS := -Wall -fPIC -Iinclude
LDFLAGS := -shared  -g -O0 

SRC_DIR := Source
OBJ_DIR := obj
LIB_DIR := lib

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
TARGET  := $(LIB_DIR)/libyazf.so

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(LIB_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR)

.PHONY: all clean
