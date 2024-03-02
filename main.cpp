#include "debug.h"
#include "defs.h"
#include "classes.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>
#include <math.h>

std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());

SDL_Window *window;
SDL_Renderer *renderer;

const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_bool isRunning, win;

Uint64 miliseconds, deltaTime, cur, last, countsPerFrame, enemyAttack;
long double speedMul;

Player *player;
Invader *inv[INVADER_ROW + 2][INVADER_COLUMN + 2];
int curDir, remInvader, cntRow[INVADER_ROW + 2];
BulletFactory bulletContainer, enemyBulletContainer;
std::vector<Splat*> splatContainer;
Bunker bunkerPoints;

void resetAttack() {
	enemyAttack = (unsigned)rng() % 800000000 + 700000000;
}

void createBunker(int x, int y) {
	for(int i = 0; i < BUNKER_ROW; ++i) {
		int lout = std::max(0, BUNKER_COLUMN / 5 - i / 5 * 5);
		int rout = BUNKER_COLUMN - 1 - lout;
		int lin = BUNKER_COLUMN / 5 + (BUNKER_ROW - 1 - i) / 5 * 5;
		int rin = BUNKER_COLUMN - lin;
		if(lin + 5 >= rin) {
			for(int j = lout; j <= rout; ++j) {
				bunkerPoints.insertPoint(j + x, i + y);
			}
		}
		else {
			for(int j = lout; j <= lin; ++j) {
				bunkerPoints.insertPoint(j + x, i + y);
			}
			for(int j = rin; j <= rout; ++j) {
				bunkerPoints.insertPoint(j + x, i + y);
			}
		}
	}
}

void initGame(SDL_Renderer* renderer) {
	// reset renderer
	SDL_RenderClear(renderer);

	// direction at start
	curDir = ((unsigned)rng() & 1) + 1; // random 1 or 2
					    
	remInvader = INVADER_COLUMN * INVADER_ROW;
	for(int i = 0; i < INVADER_ROW; ++i) {
		cntRow[i] = INVADER_COLUMN;
	}

	speedMul = 1;
	resetAttack();

	/*** Create Invaders ***/
	int padding_top = 180;
	int padding_left = (WINDOW_WIDTH - INVADER_COLUMN * INVADER_SIZE - (INVADER_COLUMN - 1) * INVADER_COLUMN_GAP) / 2 + INVADER_SIZE + INVADER_COLUMN_GAP;
	for(int i = INVADER_ROW; i--; )
		for(int j = 0; j < INVADER_COLUMN; ++j) {
			SDL_Delay(15);

			if(i == 0) {
				inv[i][j] = new Invader(renderer, 	(INVADER_ROW - i),
									padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP) + INVADER_SIZE / 8,
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP) + INVADER_SIZE / 8,
									INVADER_SIZE * 7/6,
									INVADER_SIZE * 7/6,
									"./Assets/Sprites/Invader_01-1.png", "./Assets/Sprites/Invader_01-2.png");	
			}
			else if(i < 3) {
				inv[i][j] = new Invader(renderer, 	(INVADER_ROW - i),
									padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP),
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP),
									INVADER_SIZE * 7/6,
									INVADER_SIZE,
									"./Assets/Sprites/Invader_02-1.png", "./Assets/Sprites/Invader_02-2.png");	
			}
			else {
				inv[i][j] = new Invader(renderer, 	(INVADER_ROW - i),
									padding_left + (j - 1) * (INVADER_SIZE + INVADER_COLUMN_GAP),
									padding_top + (i - 1) * (INVADER_SIZE + INVADER_ROW_GAP),
									INVADER_SIZE * 7 / 6,
									INVADER_SIZE,
									"./Assets/Sprites/Invader_03-1.png", "./Assets/Sprites/Invader_03-2.png");	
			}

			// show invader
			inv[i][j]->render(renderer, 0);
			SDL_RenderPresent(renderer);
		}

	/*** Create player ***/
	player = new Player(renderer,	PLAYER_INIT_X,
					PLAYER_INIT_Y,
					PLAYER_SIZE,
					PLAYER_SIZE);
	//////////////////////////////////////////////
	///
	///
	/*** Create bunker ***/
	const int bunkerGap = 80;
	const int bunkerHeight = 600;
	int bunkerPadding = (WINDOW_WIDTH - 3 * BUNKER_COLUMN - 2 * bunkerGap) / 2; 
	createBunker(bunkerPadding, bunkerHeight);
	createBunker(bunkerPadding + BUNKER_COLUMN + bunkerGap, bunkerHeight);
	createBunker(bunkerPadding + 2 * (BUNKER_COLUMN + bunkerGap), bunkerHeight);
	//////////////////////////////////////////////
	
}
void gameClean() {
	for(int i = 0; i < INVADER_ROW; ++i)
		for(int j = 0; j < INVADER_COLUMN; ++i) {
			if(inv[i][j])
				delete inv[i][j];
		}
	delete player;
	for(Splat *ptr : splatContainer) {
		delete ptr;
	}
}

void gameExit() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
}

void Update() {
	/*** Player Update ***/
	int playerDir = 0;	
	if(keys[SDL_SCANCODE_LEFT])
		playerDir = 1;
	if(keys[SDL_SCANCODE_RIGHT])
		playerDir = 2;
	if(keys[SDL_SCANCODE_Z])
		player->shoot(renderer, bulletContainer, deltaTime);

	player->update(renderer, deltaTime, playerDir);
	///////////////////////////////////////////////////////////

	/*** Invaders Update ***/
	bool reverseDir = 0;
	for(int i = 0; i < INVADER_ROW; ++i) {
		if(cntRow[i] == 0) {
			continue;
		}
		for(int j = 0; j < INVADER_COLUMN; ++j) {
			if(inv[i][j] == NULL) {
				continue;
			}
			if(inv[i][j]->getIn()) {
				reverseDir = 1;
			}
		}
	}
	if(reverseDir) {
		curDir ^= 3;

		// advance one step
		for(int i = 0; i < INVADER_ROW; ++i) {
			if(cntRow[i] == 0) {
				continue;
			}
			for(int j = 0; j < INVADER_COLUMN; ++j) {
				if(inv[i][j] == NULL) {
					continue;
				}
				inv[i][j]->advance(2);
			}
		}
	}

	for(int i = INVADER_ROW; i--;) {
		if(cntRow[i] == 0) {
			continue;
		}

		for(int j = 0; j < INVADER_COLUMN; ++j) {
			if(inv[i][j] == NULL) {
				continue;
			}
			inv[i][j]->update(renderer, deltaTime * speedMul, curDir);

			// if hit by bullet
			if(bulletContainer.check(inv[i][j])) {
				--remInvader;

				if(remInvader == 0) {
					win = SDL_TRUE;
					isRunning = SDL_FALSE;
					return;
				}

				--cntRow[i];
				// enemy die -> create new splat
				SDL_Rect rect = inv[i][j]->getPos();
				Splat *ptr = new Splat(renderer, rect.x, rect.y);
				splatContainer.push_back(ptr);
				delete inv[i][j];
				inv[i][j] = NULL;

				// enemy die -> speed up
				if(remInvader % 10 == 0) {
					speedMul *= 1.5;
				}
				if(remInvader == 1) {
					speedMul *= 7;
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////
	///
	if(enemyBulletContainer.check(player)) {
		win = SDL_FALSE;
		isRunning = SDL_FALSE;
		return;
	}
		
	if(enemyAttack < deltaTime) {
		// release an attack
		// random or focus on player
		if((unsigned)rng() % 4) {
			std::vector<std::pair<int, int>> pot;

			for(int j = 0; j < INVADER_COLUMN; ++j) {
				for(int i = INVADER_ROW; i--; ) {
					if(inv[i][j]) {
						pot.emplace_back(i, j);
						break;
					}
				}
			}

			int closest = 0;
			for(int i = 1; i < (int)pot.size(); ++i) {
				if(	std::abs(player->getPos().x - inv[pot[i].first][pot[i].second]->getPos().x)
				<	std::abs(player->getPos().x - inv[pot[closest].first][pot[closest].second]->getPos().x)) {
					closest = i;
				}
			}
			SDL_Rect rect = inv[pot[closest].first][pot[closest].second]->getPos();
			enemyBulletContainer.getBullet(renderer, rect.x, rect.y - 10);
		}	
		else {
			std::vector<int> pot;
			for(int j = 0; j < INVADER_COLUMN; ++j) {
				for(int i = 0; i < INVADER_ROW; ++i) {
					if(inv[i][j]) {
						pot.push_back(j);
						break;
					}
				}
			}
			int k = pot[(unsigned)rng() % pot.size()];
			for(int i = INVADER_ROW; i--; ) {
				if(inv[i][k]) {
					SDL_Rect rect = inv[i][k]->getPos();
					enemyBulletContainer.getBullet(renderer, rect.x, rect.y - 10);
					break;
				}
			}
		}


		resetAttack();
	}
	else {
		enemyAttack -= deltaTime;	
	}

	bulletContainer.update(renderer, deltaTime, 0);// * std::min(std::max((long double)1, speedMul / 2), (long double)5));
	enemyBulletContainer.update(renderer, deltaTime, 1);// * std::min(std::max((long double)1, speedMul / 2), (long double)5));
	for(Splat *ptr : splatContainer) {
		ptr->update(renderer, deltaTime);
	}
	////////////////////
	///
	
	bunkerPoints.update(renderer, bulletContainer);
	bunkerPoints.update(renderer, enemyBulletContainer);
}

void sceneLoop() {
	// init scene
	cur = SDL_GetPerformanceCounter();
	last = cur;

	isRunning = SDL_TRUE;
	win = SDL_FALSE;

	// scene loop
	while(isRunning) {
		deltaTime = cur - last;
		last = cur;

		// reset renderer
		CC(SDL_SetRenderDrawColor(	renderer,
						0, 0, 0,
						255), SDL_GetError());
		SDL_RenderClear(renderer);

		// event handler
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					isRunning = SDL_FALSE;
					break;
			}
		}

		Update();

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
}

int main(int argc, char *argv[]) {
	/*** SDL INIT ***/
	window = NULL;
	renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);

	window = (SDL_Window*)CP(	(void*)SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								WINDOW_WIDTH, WINDOW_HEIGHT,
								SDL_WINDOW_SHOWN),
								
								SDL_GetError());

	renderer = (SDL_Renderer*)CP(	(void*)SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED), SDL_GetError());

	// at game exit free memory
	atexit(gameExit);
	////////////////////
	
	/*** FONT INIT ***/
	///////////////////////////////////////


	///////////////////////////////////////////
	

	countsPerFrame = SDL_GetPerformanceFrequency() / FPS;

	initGame(renderer);
	sceneLoop();
	gameClean();

	if(win) std::cout << "win" << std::endl;
	else std::cout << "lose" << std::endl;

	return 0;
}

