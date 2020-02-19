ifeq ($(OS),Windows_NT)
	EXE := sfml_espacio.exe
	RM := del /s
else
    EXE := sfml_espacio
endif
SRC := sfml_espacio.cpp
OBJ := $(SRC:.cpp=.o)
DEP := $(OBJ:.o=.d)

CPPFLAGS := 
CXXFLAGS := -std=c++11  -Wall -W
LDFLAGS := 
LDLIBS := 

.PHONY: all clean

all: debug

debug: CPPFLAGS += 
debug: CXXFLAGS += -O0 -ggdb
debug: LDFLAGS +=
debug: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system
debug: $(EXE)

release: CPPFLAGS += -MMD -MP -DSFML_STATIC
release: CXXFLAGS += -O3
release: CXXFLAGS += -pedantic -static -static-libgcc -static-libstdc++
release: LDFLAGS := -mwindows
release: LDLIBS := -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32
release: LDLIBS += -lwinmm -lgdi32
release: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(EXE) $(OBJ) $(DEP)