srcFiles := main.cpp ./src/classes.cpp ./src/debug.cpp
libraries := -I./src/include/ -I./SDL2/include
linker := -L./src/lib/

prefix := ./SDL2
exec_prefix := $(prefix)
libdir := $(exec_prefix)/lib
includedir := $(prefix)/include

Libs := -L$(libdir) -Wl,-rpath,$(libdir) -Wl,--enable-new-dtags


main: $(srcFiles)
	g++ $(srcFiles) -o main -Wall -std=c++17 $(libraries) $(linker) $(Libs) -lSDL2 -lSDL2_image -lSDL2_ttf
