CXX      := -g++
CXXFLAGS := -pedantic-errors --std=c++2a -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
APP_DIR  := $(BUILD)/apps
TEST_DIR := ./test
OBJ_DIR  := $(BUILD)/objects
TARGET   := program
INCLUDE  := -I.
SRC      :=                   \
	 $(wildcard ttre/*.cc)      \

TEST      :=                  \
	 $(wildcard test/*.cc)      \

OBJECTS  := $(SRC:%.cc=$(OBJ_DIR)/%.o)
DEPENDENCIES \
				 := $(OBJECTS:.o=.d)

TESTS  := $(TEST:%.cc=$(OBJ_DIR)/%.o)
DEPENDENCIES \
				 := $(TESTS:.o=.d)


all: build $(APP_DIR)/$(TARGET)

test: build $(TEST_DIR)/$(TARGET)
	./test/test_runner 
	@rm -f ./test/test_runner

$(OBJ_DIR)/%.o: %.cc
	 @mkdir -p $(@D)
	 $(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	 @mkdir -p $(@D)
	 $(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

$(TEST_DIR)/$(TARGET): $(OBJECTS) $(TESTS)
	 @mkdir -p $(@D)
	 $(CXX) $(CXXFLAGS) -o $(TEST_DIR)/test_runner $^ $(LDFLAGS)


-include $(DEPENDENCIES)

.PHONY: all build clean debug release info test

build:
	 @mkdir -p $(APP_DIR)
	 @mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	 -@rm -rvf $(OBJ_DIR)/*
	 -@rm -rvf $(APP_DIR)/*

info:
	 @echo "[*] Application dir: ${APP_DIR}     "
	 @echo "[*] Object dir:      ${OBJ_DIR}     "
	 @echo "[*] Sources:         ${SRC}         "
	 @echo "[*] Objects:         ${OBJECTS}     "
	 @echo "[*] Dependencies:    ${DEPENDENCIES}"