#include "classes.h"

#include <iostream>

Object::Object(SDL_Renderer *renderer, int x, int y, int n...) : sprites(n), textures(n) {
	position = { x, y, INVADER_SIZE, INVADER_SIZE };	

	va_list args;
	va_start(args, n);
	for(int i = 0; i < n; ++i) {
		const char *s = va_arg(args, char*);
		sprites[i] = IMG_Load(s);
		textures[i] = SDL_CreateTextureFromSurface(renderer, sprites[i]);
	}
	va_end(args);
}
Object::~Object() {
	for(int i = 0; i < (int)sprites.size(); ++i) {
		SDL_FreeSurface(sprites[i]);
		SDL_DestroyTexture(textures[i]);
	}
}

void Object::updateState(SDL_Renderer *renderer) {
	SDL_RenderCopy(renderer, textures[0], NULL, &position);
}

