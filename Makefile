srcFiles := main.cpp ./src/classes.cpp ./src/debug.cpp
libraries := -I./src/include/ -I./SDL2/include
linker := -L./src/lib/

prefix := /home/tomoshibi/Github/SDL_SpaceInvaders/SDL2
exec_prefix := $(prefix)
libdir := $(exec_prefix)/lib
includedir := $(prefix)/include

Libs := -L$(libdir) -Wl,-rpath,$(libdir) -Wl,--enable-new-dtags -lSDL2


main: $(srcFiles)
	g++ $(srcFiles) -o main -Wall -std=c++17 $(libraries) $(linker) $(Libs) -lSDL2_image
