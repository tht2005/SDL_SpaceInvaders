#include "classes.h"
#include "debug.h"

#include <algorithm>

bool iSect(const SDL_Rect& a, const SDL_Rect& b) {
	return 	std::max(a.x, b.x) <= std::min(a.x + a.w, b.x + b.w) &&
			std::max(a.y, b.y) <= std::min(a.y + a.h, b.y + b.h);	
}

Object::Object(SDL_Renderer *renderer, int x, int y, int w, int h, int n...) : dir(0), Time(0), xx(x), yy(y), sprites(n), textures(n) {
	render_position = { x, y, w, h };	

	va_list args;
	va_start(args, n);
	for(int i = 0; i < n; ++i) {
		const char *s = va_arg(args, char*);
		sprites[i] = 		(SDL_Surface*)CP((void*)IMG_Load(s), "load image error");
		textures[i] = 		(SDL_Texture*)CP((void*)SDL_CreateTextureFromSurface(renderer, sprites[i]), "create texture error");
	}
	va_end(args);
}
Object::~Object() {
	// im solving enemy delete error
	for(int i = 0; i < (int)sprites.size(); ++i) {
		SDL_FreeSurface(sprites[i]);
		SDL_DestroyTexture(textures[i]);
	}
}

bool Object::insideWindow() {
	return 		0 <= xx && xx + render_position.w <= WINDOW_WIDTH &&
				0 <= yy && yy + render_position.h <= WINDOW_HEIGHT;
}
bool Object::getIn() {
	if(xx < 0) {
		xx = 0;
		return 1;
	}	
	if(xx + render_position.w > WINDOW_WIDTH) {
		xx = WINDOW_WIDTH - render_position.w;
		return 1;
	}
	return 0;
}

void Object::setDir(int d) {
	dir = d;	
}

bool Object::checkCollision(Object* b) {
	return iSect(render_position, b->render_position);
}

void Object::move(Uint64 moveSpeed, Uint64 deltaTime) {
	/**
		dir = 0001b: left
		dir = 0010b: right
		dir = 0100b: top
		dir = 1000b: down

		dir ^= 0011b to change left right
		dir ^= 1100b to change top down
	**/

	long double dist = (long double)deltaTime * moveSpeed / (int)1e9;

	if(dir & 1) {
		xx -= dist;
	}
	if(dir >> 1 & 1) {
		xx += dist;
	}
	if(dir >> 2 & 1) {
		yy -= dist;
	}
	if(dir >> 3 & 1) {
		yy += dist;
	}

	render_position.x = xx;
	render_position.y = yy;
}

SDL_Rect Object::getPos() {
	return render_position;
}
void Object::setPos(const SDL_Rect& r) {
	xx = r.x;
	yy = r.y;
	render_position = r;
}

void Object::render(SDL_Renderer* renderer, int index) {
	SDL_RenderCopy(renderer, textures[index], NULL, &render_position);
}

void Invader::update(SDL_Renderer *renderer, Uint64 deltaTime) {
	Time += deltaTime;
	render(renderer, (Time / INVADER_SPRITE_FREQ) & 1);
}


Invader::Invader(SDL_Renderer *renderer, int x, int y, int w, int h, const char* s0, const char* s1) : Object(renderer, x, y, w, h, 2, s0, s1) {}

/*
void Invader::solveCollision(Invader *b) {
	if(!checkCollision(b)) {
		return;
	}
	SDL_Rect u = getPos(), v = b->getPos();
	if(u.x <= v.x) {
		u.x = v.x - u.w;
		setPos(u);	
		turnDir();
		b->turnDir();
	}			
	else {
		b->solveCollision(this);
	}
}
*/



Player::Player(SDL_Renderer* renderer, int x, int y, int w, int h) : Object(renderer, x, y, w, h, 1, "../Assets/Sprites/Player.png"), bulletTime(0) {}

void Player::update(SDL_Renderer *renderer, Uint64 deltaTime) {
	render(renderer, 0);

	if(bulletTime < deltaTime)
		bulletTime = 0;
	else
		bulletTime -= deltaTime;
}
void Player::shoot(SDL_Renderer *renderer, BulletFactory& bulletContainer, Uint64 deltaTime) {
	if(bulletTime > 0)
		return;

	bulletTime += LAZER_COOLDOWN * 1000000000ULL;
	bulletContainer.getBullet(renderer, render_position.x + PLAYER_SIZE / 2 - LAZER_WIDTH / 2, render_position.y - 30);
}


Bullet::Bullet(SDL_Renderer *renderer, int x, int y) : Object(renderer, x, y, LAZER_WIDTH, LAZER_HEIGHT, 1, "../Assets/Sprites/Laser.png") {
	dir = 4;
}


BulletFactory::~BulletFactory() {
	for(Bullet *ptr : vect) {
		delete ptr;
	}
}

Bullet* BulletFactory::getBullet(SDL_Renderer *renderer, int x, int y) {	
	SDL_Rect rect = { x, y, LAZER_WIDTH, LAZER_HEIGHT };
	for(Bullet *ptr : vect) {
		if(!ptr->insideWindow()) {
			ptr->setPos(rect);
			return ptr;
		}
	}
	Bullet *ptr = new Bullet(renderer, x, y);
	ptr->setPos(rect);
	vect.push_back(ptr);
	return ptr;
}

void BulletFactory::update(SDL_Renderer *renderer, Uint64 deltaTime) {
	for(Bullet *ptr : vect) {
		if(ptr->insideWindow()) {
			ptr->render(renderer, 0);
			ptr->move(LAZER_SPEED, deltaTime);
		}
	}
}

void BulletFactory::remove(Bullet *b) {
	SDL_Rect rect = b->getPos();
	rect.x = WINDOW_WIDTH + 100;
	rect.y = WINDOW_HEIGHT + 100;
	b->setPos(rect);
}

bool BulletFactory::check(Invader *I) {
	for(Bullet *ptr : vect) {
		if(ptr->checkCollision(I)) {
			remove(ptr);
			return true;
		}
	}
	return false;
}


Splat::Splat(SDL_Renderer *renderer, int x, int y) : 	Object(renderer, x, y, SPLAT_SIZE, SPLAT_SIZE, 1, "../Assets/Sprites/Splat.png"),
							remTime(SPLAT_TIME * 100000000ULL) {}

#include <iostream>

void Splat::update(SDL_Renderer* renderer, Uint64 deltaTime) {
	if(remTime < deltaTime)
		remTime = 0;
	else
		remTime -= deltaTime;

	if(remTime == 0)
		return;

	render(renderer, 0);
}







