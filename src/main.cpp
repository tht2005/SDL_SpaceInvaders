#include <stdio.h>

#include "debug.h"
#include "defs.h"
#include "classes.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <chrono>
#include <random>

std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

Invader *inv[INVADER_ROW + 2][INVADER_COLUMN + 2];

int main(int argc, char *argv[]) {
	/*** SDL INIT ***/
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);

	window = (SDL_Window*)CP(	(void*)SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								WINDOW_WIDTH, WINDOW_HEIGHT,
								SDL_WINDOW_SHOWN),
								
								SDL_GetError());

	renderer = (SDL_Renderer*)CP(	(void*)SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED), SDL_GetError());

	CC(SDL_SetRenderDrawColor(	renderer,
                   			0, 0, 0,
                   			255), SDL_GetError());
	////////////////////
	/*** FONT INIT ***/
	///////////////////////////////////////


	int curDir = ((unsigned)rng() & 1) + 1; // random 1 or 2

	/*** Create Invaders ***/
	int padding_top = 230;
	int padding_left = (WINDOW_WIDTH - INVADER_COLUMN * INVADER_SIZE - (INVADER_COLUMN - 1) * INVADER_COLUMN_GAP) / 2 + INVADER_SIZE + INVADER_COLUMN_GAP;
	for(int i = 0; i < INVADER_ROW; ++i)
		for(int j = 0; j < INVADER_COLUMN; ++j) {
			if(i == 0) {
				inv[i][j] = new Invader(renderer, 	padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP) + INVADER_SIZE / 8,
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP) + INVADER_SIZE / 8,
									INVADER_SIZE,
									INVADER_SIZE,
									"../Assets/Sprites/Invader_01-1.png", "../Assets/Sprites/Invader_01-2.png");	
			}
			else if(i < 3) {
				inv[i][j] = new Invader(renderer, 	padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP),
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP),
									INVADER_SIZE * 7/6,
									INVADER_SIZE,
									"../Assets/Sprites/Invader_02-1.png", "../Assets/Sprites/Invader_02-2.png");	
			}
			else {
				inv[i][j] = new Invader(renderer, 	padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP),
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP),
									INVADER_SIZE * 7 / 6,
									INVADER_SIZE,
									"../Assets/Sprites/Invader_03-1.png", "../Assets/Sprites/Invader_03-2.png");	
			}

			//init direction left/right
			inv[i][j]->setDir(curDir);
		}
	///////////////////////////////////////////

	/*** Create player ***/
	Player *player = new Player(renderer, 	PLAYER_INIT_X,
						PLAYER_INIT_Y,
						PLAYER_SIZE,
						PLAYER_SIZE);
	//////////////////////////////////////////////


	BulletFactory bulletContainer;
	std::vector<Splat*> splatContainer;


	Uint64 deltaTime, cur = SDL_GetPerformanceCounter(), countsPerFrame = SDL_GetPerformanceFrequency() / FPS, last = cur;
	Uint64 miliseconds;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	SDL_bool isRunning = SDL_TRUE;
	while(isRunning) {
		deltaTime = cur - last;
		last = cur;

		// reset renderer
		SDL_RenderClear(renderer);

		// event handler
		SDL_Event event;
		int playerDir;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					isRunning = SDL_FALSE;
					break;
			}
		}
		//keyboard
		playerDir = -1;	
		if(keys[SDL_SCANCODE_LEFT])
			playerDir = 1;
		if(keys[SDL_SCANCODE_RIGHT])
			playerDir = 2;
		if(keys[SDL_SCANCODE_Z])
			player->shoot(renderer, bulletContainer, deltaTime);

		if(playerDir != -1) {
			player->setDir(playerDir);
			player->move(PLAYER_MOVE_SPEED, deltaTime);
		}
		else if(playerDir == 2) {
			player->setDir(2);
			player->move(PLAYER_MOVE_SPEED, deltaTime);
		}

		// re-render
		player->update(renderer, deltaTime);

		bool reverseDir = 0;
		for(int i = 0; i < INVADER_ROW; ++i)
			for(int j = 0; j < INVADER_COLUMN; ++j) {
				if(inv[i][j] == NULL) {
					continue;
				}
				if(inv[i][j]->getIn())
					reverseDir = 1;
			}

		if(reverseDir)
			curDir ^= 3;

		for(int i = 0; i < INVADER_ROW; ++i)
			for(int j = 0; j < INVADER_COLUMN; ++j) {
				if(inv[i][j] == NULL) {
					continue;
				}

				inv[i][j]->setDir(curDir);

				inv[i][j]->move(INVADER_MOVE_SPEED, deltaTime);
				inv[i][j]->update(renderer, deltaTime);

				if(bulletContainer.check(inv[i][j])) {
					// enemy die new splat
					SDL_Rect rect = inv[i][j]->getPos();
					Splat *ptr = new Splat(renderer, rect.x, rect.y);
					splatContainer.push_back(ptr);

					delete inv[i][j];
					inv[i][j] = NULL;
				}
			}

		bulletContainer.update(renderer, deltaTime);
		for(Splat *ptr : splatContainer) {
			ptr->update(renderer, deltaTime);
		}
		////////////////////

		// FPS adjusting
		cur = SDL_GetPerformanceCounter();
		if(last + countsPerFrame > cur) {
			miliseconds = (last + countsPerFrame - cur) * 1000 / FPS / countsPerFrame;
			SDL_Delay(miliseconds);
		}
		////////////////////////////////

		// push renderer to window (make screen change)
		SDL_RenderPresent(renderer);
	}

	for(int i = 0; i < INVADER_ROW; ++i)
		for(int j = 0; j < INVADER_COLUMN; ++i) {
			delete inv[i][j];
		}
	delete player;
	for(Splat *ptr : splatContainer) {
		delete ptr;
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
	return 0;
}

