srcFiles := main.cpp ./src/classes.cpp ./src/debug.cpp

main: $(srcFiles)
	g++ $(srcFiles) -o main -Wall -std=c++17 -I./src/include -L./src/lib -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -ldl
