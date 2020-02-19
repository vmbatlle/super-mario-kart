#OBJS specifies which files to compile as part of the project
OBJS = sfml_espacio.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -IC:\mingw_dev_lib\include -MMD -MP -DSFML_STATIC

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\mingw_dev_lib\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS =  -O3 -std=c++11 -Wall -W -pedantic -static -static-libgcc -static-libstdc++

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = sfml_espacio.exe

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)