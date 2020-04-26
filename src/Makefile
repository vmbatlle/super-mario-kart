ifeq ($(OS),Windows_NT)
	EXE := ../super_mario_kart.exe
	RM := del /s
	MKDIR := mkdir
else
    EXE := ../super_mario_kart
	MKDIR := mkdir -p
endif

BINDIR := ../bin

SRC := $(wildcard **/*.cpp) $(wildcard *.cpp)
OBJ := $(patsubst %.cpp, $(BINDIR)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)

CPPFLAGS := 
CXXFLAGS := -std=c++11  -Wall -W
LDFLAGS := 
LDLIBS := 

.PHONY: all clean debug release

all: debug

debug: CPPFLAGS += 
debug: CXXFLAGS += -O0 -ggdb
debug: LDFLAGS += -pthread
debug: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
debug: folders $(EXE)

release: CPPFLAGS += -MMD -MP -DSFML_STATIC
release: CXXFLAGS += -O3 -pedantic
release: LDLIBS := -static-libgcc -static-libstdc++ -pthread
ifeq  ($(OS),Windows_NT) 
release: LDFLAGS := -mwindows -static
# each line has a [sfml-module] followed by its [dependencies] (may be repeated)
# see: https://www.sfml-dev.org/tutorials/2.4/start-cb.php
release: LDLIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32
release: LDLIBS += -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32
release: LDLIBS += -lsfml-audio-s -lsfml-system-s -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg
release: LDLIBS += -lsfml-system-s -lwinmm
else
# tested on debian 10
release: LDLIBS := -static-libgcc -static-libstdc++ -pthread	
release: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
endif
release: folders $(EXE)

folders: $(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio

$(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio:
	$(MKDIR) "$@"

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

clean:
	$(RM) $(EXE) $(OBJ) $(DEP)