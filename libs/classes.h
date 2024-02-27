#pragma once

#include <vector>
#include <SDL2/SDL_image.h>

#include "defs.h"

class Object {
	//private:
	public:
	
	SDL_Rect position;
	std::vector<SDL_Surface*> sprites;	
	std::vector<SDL_Texture*> textures;

	public:

	Object(SDL_Renderer*, int, int, int...);
	~Object();

	void updateState(SDL_Renderer*);
}; 


