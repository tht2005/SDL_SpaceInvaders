#pragma once

#include <vector>
#include <SDL2/SDL_image.h>

#include "defs.h"

class Object {
	protected:
	
	int dir;
	Uint64 Time;
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
	bool getIn();
	bool checkCollision(Object*);

	void setDir(int);
	void move(Uint64, Uint64);

	void render(SDL_Renderer*, int);
}; 

class Invader : public Object {
	private:

	public:
	
	Invader(SDL_Renderer*, int, int, int, int, const char*, const char*);

	void solveCollision(Invader*);
	void update(SDL_Renderer*, Uint64);
};

class Bullet : public Object {
	private:

	public:

	Bullet(SDL_Renderer*, int, int);
};


class BulletFactory {
	private:
	std::vector<Bullet*> vect;
	
	public:
	~BulletFactory();
	
	Bullet *getBullet(SDL_Renderer*, int, int);
	void update(SDL_Renderer*, Uint64);

	bool check(Invader*);
	void remove(Bullet*);
};


class Player : public Object {
	private:
	Uint64 bulletTime;

	public:

	Player(SDL_Renderer*, int, int, int, int);
	
	void update(SDL_Renderer*, Uint64);
	void shoot(SDL_Renderer*, BulletFactory&, Uint64);
};

class Splat : public Object {
	private:
	Uint64 remTime;
	public:
	Splat(SDL_Renderer*, int, int);

	void update(SDL_Renderer*, Uint64);
};







