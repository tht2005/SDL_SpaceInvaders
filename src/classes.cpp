#include "classes.h"
#include "debug.h"

#include <algorithm>

bool iSect(const SDL_Rect& a, const SDL_Rect& b) {
	return 	std::max(a.x, b.x) <= std::min(a.x + a.w, b.x + b.w) &&
			std::max(a.y, b.y) <= std::min(a.y + a.h, b.y + b.h);	
}

Object::Object(SDL_Renderer *renderer, int x, int y, int w, int h, int n...) : Time(0), xx(x), yy(y), sprites(n), textures(n) {
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
	for(int i = 0; i < (int)sprites.size(); ++i) {
		SDL_FreeSurface(sprites[i]);
		SDL_DestroyTexture(textures[i]);
	}
}

bool Object::insideWindow() {
	return 		0 <= xx && xx + render_position.w <= WINDOW_WIDTH;
}
void Object::turnDir() {
	if(xx < 0) {
		xx = 0;
	}	
	if(xx + render_position.w > WINDOW_WIDTH) {
		xx = WINDOW_WIDTH - render_position.w;
	}
	dir ^= 15;
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

	if(!insideWindow()) {
		turnDir();
	}
}

SDL_Rect Object::getPos() {
	return render_position;
}
void Object::setPos(const SDL_Rect& r) {
	xx = r.x;
	yy = r.y;
	render_position = r;
}

void Object::updateState(SDL_Renderer *renderer, Uint64 deltaTime) {
	Time += deltaTime;

	move(INVADER_MOVE_SPEED, deltaTime);

	// render object
	if((int)textures.size() == 1) {
		SDL_RenderCopy(renderer, textures[1], NULL, &render_position);
	}
	else {
		SDL_RenderCopy(renderer, textures[(Time / INVADER_SPRITE_FREQ) & 1], NULL, &render_position);
	}
}


Invader::Invader(SDL_Renderer *renderer, int x, int y, int w, int h, const char* s0, const char* s1) : Object(renderer, x, y, w, h, 2, s0, s1) {}

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







