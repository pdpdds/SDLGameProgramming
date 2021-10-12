SDL_CXXFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

CXXFLAGS = -std=c++11 -Wall -Werror
LIBS = -lm -lSDL2_image -lSDL2_mixer -lGL

CPP_FILES := $(wildcard ./*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

CC = g++

ODIR = obj

$(ODIR)/%.o: %.cpp
	$(CC) -c -o $@ $< $(SDL_CXXFLAGS) $(CXXFLAGS)

all: $(OBJ_FILES)
	$(CC) -o $(ODIR)/BattleCitySDL $^ $(CXXFLAGS) $(SDL_LDFLAGS) $(LIBS)

.PHONY: clean all

clean:
	-rm -f $(ODIR)/*.o $(ODIR)/BattleCitySDL
