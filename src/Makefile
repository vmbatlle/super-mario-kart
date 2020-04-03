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
debug: LDFLAGS +=
debug: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
debug: folders $(EXE)

release: CPPFLAGS += -MMD -MP -DSFML_STATIC
release: CXXFLAGS += -O3 -pedantic
ifeq  ($(OS),Windows_NT) 
release: LDFLAGS := -mwindows -static
release: LDLIBS += -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s -lopengl32
release: LDLIBS += -lwinmm -lgdi32
endif
release: LDLIBS := -static-libgcc -static-libstdc++
release: LDLIBS += -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
release: folders $(EXE)

folders: $(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai

$(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai:
	$(MKDIR) "$@"

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

clean:
	$(RM) $(EXE) $(OBJ) $(DEP)