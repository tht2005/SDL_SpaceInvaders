#include <stdio.h>

#include "debug.h"
#include "defs.h"

#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;	

	SDL_Init(SDL_INIT_VIDEO);

	window = (SDL_Window*)CP((void*)SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
									WINDOW_WIDTH, WINDOW_HEIGHT, 
									SDL_WINDOW_SHOWN), SDL_GetError()); 

	renderer = (SDL_Renderer*)CP((void*)SDL_CreateRenderer(window, -1,
										SDL_RENDERER_ACCELERATED), SDL_GetError());	

	CC(SDL_SetRenderDrawColor(renderer,
                   0, 0, 0,
                   255), SDL_GetError());	
	CC(SDL_RenderClear(renderer), SDL_GetError());
	SDL_RenderPresent(renderer);

	SDL_bool isRunning = SDL_TRUE;
	while(isRunning) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					isRunning = SDL_FALSE;
					break;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	
	return 0;
}
