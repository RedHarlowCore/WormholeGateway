CXX ?= g++
CC ?= gcc
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude -Isrc
CFLAGS ?= -std=c11 -Wall -Wextra -Iinclude

TARGET := build/WormholeGateway.exe
CPP_OBJECTS := build/main.o build/Wormhole.o build/GuiManager.o build/Renderer.o
C_OBJECTS := build/physics.o build/utils.o build/logger.o build/visualization.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(CPP_OBJECTS) $(C_OBJECTS) | build
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm -luser32 -lgdi32 -lwinmm -mwindows

build/main.o: src/main.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/Wormhole.o: src/Wormhole.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/GuiManager.o: gui/GuiManager.cpp | build
	$(CXX) $(CXXFLAGS) -Igui -c $< -o $@

build/Renderer.o: gui/Renderer.cpp | build
	$(CXX) $(CXXFLAGS) -Igui -c $< -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	@if not exist build mkdir build

run: $(TARGET)
	$(TARGET)

clean:
	@if exist build\*.o del /q build\*.o
	@if exist $(TARGET) del /q $(TARGET)
