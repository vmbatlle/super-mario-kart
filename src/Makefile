# Check if OS variable is defined, if not, assign a default value
ifndef OS
OS := $(shell uname -s)
endif
WINDRES=windres

MKDIR := mkdir -p
BINDIR := ../bin

SRC := $(wildcard **/*.cpp) $(wildcard *.cpp)
OBJ := $(patsubst %.cpp, $(BINDIR)/%.o, $(SRC))
DEP := $(OBJ:.o=.d)

ifeq ($(OS),Mingw)
EXE := $(BINDIR)/super_mario_kart.exe
COMMON_CXXFLAGS := -std=gnu++11 -Wall -W
COMMON_CPPFLAGS := -I/home/user/src/SFML-2.6.1/include
COMMON_LDFLAGS := -L/usr/i686-w64-mingw32/sys-root/mingw/lib
COMMON_LDLIBS := 

DEBUG_CPPFLAGS := -DDEBUG
DEBUG_CXXFLAGS := -O0 -ggdb
DEBUG_LDFLAGS := -pthread
DEBUG_LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

RELEASE_CPPFLAGS := -MMD -MP -DSFML_STATIC
RELEASE_CXXFLAGS := -O3 -pedantic
RELEASE_LDFLAGS := -mwindows -static -lmsvcrt
RELEASE_LDLIBS := -static-libgcc -static-libstdc++ -pthread -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -lsfml-audio-s -lsfml-system-s -lopenal32 -lFLAC -lvorbisenc -lvorbisfile -lvorbis -logg
else ifeq ($(OS),Linux)
EXE := $(BINDIR)/super_mario_kart
COMMON_CXXFLAGS := -std=c++11 -Wall -W
COMMON_CPPFLAGS := 
COMMON_LDFLAGS :=
COMMON_LDLIBS := -pthread -static-libgcc -static-libstdc++

DEBUG_CPPFLAGS := -DDEBUG
DEBUG_CXXFLAGS := -O0 -ggdb
DEBUG_LDFLAGS :=
DEBUG_LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

RELEASE_CPPFLAGS := -MMD -MP -DSFML_STATIC -fPIC -lqn -Wl,-rpath,$$ORIGIN/lib
RELEASE_CXXFLAGS := -O3 -pedantic
RELEASE_LDFLAGS :=
RELEASE_LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
else ifeq ($(OS),Darwin)
EXE := $(BINDIR)/super_mario_kart
COMMON_CXXFLAGS := -std=c++11 -Wall -W
COMMON_CPPFLAGS := `pkg-config --cflags sfml-graphics sfml-window sfml-system`
COMMON_LDFLAGS := -lsfml-audio `pkg-config --libs sfml-graphics sfml-window sfml-system`
COMMON_LDLIBS := -pthread

DEBUG_CPPFLAGS := -DDEBUG
DEBUG_CXXFLAGS := -O0 -ggdb
DEBUG_LDFLAGS :=
DEBUG_LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

RELEASE_CPPFLAGS := -MMD -MP -DSFML_STATIC -fPIC -lqn -Wl,-rpath,$$ORIGIN/lib
RELEASE_CXXFLAGS := -O3 -pedantic
RELEASE_LDFLAGS := 
RELEASE_LDLIBS :=
endif

CXXFLAGS := $(COMMON_CXXFLAGS)
CPPFLAGS := $(COMMON_CPPFLAGS)
LDFLAGS := $(COMMON_LDFLAGS)
LDLIBS := $(COMMON_LDLIBS)

.PHONY: all clean debug release

all: debug

debug: CPPFLAGS := $(COMMON_CPPFLAGS) $(DEBUG_CPPFLAGS)
debug: CXXFLAGS := $(COMMON_CXXFLAGS) $(DEBUG_CXXFLAGS)
debug: LDFLAGS := $(COMMON_LDFLAGS) $(DEBUG_LDFLAGS)
debug: LDLIBS := $(COMMON_LDLIBS) $(DEBUG_LDLIBS)
debug: folders $(EXE)

release: CPPFLAGS :=  $(COMMON_CPPFLAGS) $(RELEASE_CPPFLAGS)
release: CXXFLAGS := $(COMMON_CXXFLAGS) $(RELEASE_CXXFLAGS)
release: LDFLAGS := $(COMMON_LDFLAGS) $(RELEASE_LDFLAGS)
release: LDLIBS := $(COMMON_LDLIBS) $(RELEASE_LDLIBS)
release: folders $(EXE)

folders: $(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio

$(BINDIR) $(BINDIR)/states $(BINDIR)/input $(BINDIR)/map $(BINDIR)/entities $(BINDIR)/gui $(BINDIR)/ai $(BINDIR)/audio:
	$(MKDIR) "$@"

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/%.o: %.cpp
	$(CXX) -c $(LDFLAGS) $(CXXFLAGS) $(CPPFLAGS) -I. $< -o $@

$(BINDIR)/super_mario_kart.res: super_mario_kart.rc
	$(WINDRES) super_mario_kart.rc -O coff -o $(BINDIR)/super_mario_kart.res

clean:
	$(RM) $(EXE) $(OBJ) $(DEP)