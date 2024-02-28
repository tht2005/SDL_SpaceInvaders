#pragma once

#include <vector>
#include <SDL2/SDL_image.h>

#include "defs.h"

class Object {
	private:
	
	Uint64 Time;
	int dir;
	double xx, yy;	
	SDL_Rect render_position;
	std::vector<SDL_Surface*> sprites;	
	std::vector<SDL_Texture*> textures;

	public:


	Object(SDL_Renderer*, int, int, int, int, int...);
	~Object();

	SDL_Rect getPos();
	void setPos(const SDL_Rect&);

	bool insideWindow();
	void turnDir();
	bool checkCollision(Object*);

	void setDir(int);
	void move(Uint64, Uint64);
	void updateState(SDL_Renderer*, Uint64);
}; 

class Player : Object {
	public:
	
	void shoot();
};

class Invader : public Object {
	private:

	public:
	
	Invader(SDL_Renderer*, int, int, int, int, const char*, const char*);

	void solveCollision(Invader*);
};

class Bullet : Object {
};


