# Name of the project
PROJECT := gameboy

# Compiler to use
CC := g++

# Executable output directory
BIN     := bin
BUILD   := build
PROGRAM := $(BIN)/$(PROJECT)

# List of all source files
SRCS := emulator/hal/src/hal.cpp           \
        emulator/mmu/src/mmu.cpp           \
        emulator/ppu/src/ppu.cpp           \
        emulator/joypad/src/joypad.cpp     \
        emulator/cpu/src/cpu.cpp           \
	      emulator/cpu/src/cpu_instr.cpp     \
        emulator/apu/src/apu.cpp           \
        emulator/timer/src/timer.cpp       \
        emulator/Emulator.cpp              \
        src/main.cpp

# Include directories for header files
CFLAGS := -std=c++20 -g                 \
          -Iemulator/platform/include   \
          -Iemulator/hal/include        \
          -Iemulator/mmu/include        \
          -Iemulator/ppu/include        \
          -Iemulator/joypad/include     \
	        -Iemulator/cpu/include        \
          -Iemulator/apu/include        \
          -Iemulator/timer/include      \
          -Iemulator/
CFLAGS += -fpic
CFLAGS += -DEMULATOR_LOG_LEVEL=3

# Linker flags for libraries
LDFLAGS := -lpthread -Wall -lm

# List of all object files, derived from the source files list
OBJS=$(SRCS:.cpp=.o)
OBJS_PATHS=$(patsubst %, $(BUILD)/%, $(OBJS))

# Set flags for each operating system
# @todo port to Windows
ifeq ($(OS),Windows_NT)
  $(error Windows not yet supported)
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    CFLAGS += -DLINUX=1
    LDFLAGS += -lGL -lopenal -lGLU -lglut
    LDFLAGS += -lrt
    LDFLAGS += -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
  endif
  ifeq ($(UNAME_S),Darwin)
    # @todo this could be included better
    CFLAGS += -DOSX=1 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
    LDFLAGS += -framework OpenGL -framework GLUT -framework OpenAL
    LDFLAGS += -L/opt/homebrew/lib -lSDL2
  endif
endif

CFLAGS += -DTEST=$(TEST)
# Driver Config
CFLAGS += -DUSE_SDL_GRAPHICS=$(USE_SDL_GRAPHICS)
CFLAGS += -DUSE_SDL_AUDIO=$(USE_SDL_AUDIO)
CFLAGS += -DUSE_OPENGL_GRAPHICS=$(USE_OPENGL_GRAPHICS)
CFLAGS += -DUSE_OPENAL_AUDIO=$(USE_OPENAL_AUDIO)

# Debug settings
CFLAGS += -DSHOW_TILE_OUTLINES=$(SHOW_TILE_OUTLINES)
CFLAGS += -DSHOW_WINDOW=$(SHOW_WINDOW)

# Default target
all: $(PROGRAM)

# Target for building the project executable
$(PROGRAM): $(OBJS_PATHS)
	$(CC) $(CFLAGS) $(OBJS_PATHS) $(LDFLAGS) -o $(PROGRAM)

# Target for building all object files
$(BUILD)/%.o: %.cpp
	mkdir -p $(shell dirname $(OBJS_PATHS)) $(BIN)
	$(CC) $(CFLAGS) -c $< -o $@

# Target for cleaning up object files and the project executable
clean:
	rm -rf $(BUILD) $(PROGRAM)
