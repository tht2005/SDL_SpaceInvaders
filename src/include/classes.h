#pragma once

#include <vector>
#include <SDL2/SDL_image.h>

#include "defs.h"

class Object {
	protected:
	double xx, yy;	
	SDL_Rect render_position;
	std::vector<SDL_Surface*> sprites;	
	std::vector<SDL_Texture*> textures;

	public:

	Object(SDL_Renderer*, int, int, int, int, int...);
	~Object();

	const SDL_Rect& getPos();
	void setPos(const SDL_Rect&);

	bool insideWindow();
	bool getIn();
	bool checkCollision(Object*);

	void move(Uint64, Uint64, int);
	void render(SDL_Renderer*, int);
}; 

class Invader : public Object {
	private:
	Uint64 moveTime, stopTime, advanceTime;

	public:
	
	Invader(SDL_Renderer*, Uint64, int, int, int, int, const char*, const char*);

	void advance(Uint64);
	void update(SDL_Renderer*, Uint64, int);
};

class Bullet : public Object {
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
	const std::vector<Bullet*>& getVect() const;
};


class Player : public Object {
	private:
	Uint64 bulletTime;

	public:

	Player(SDL_Renderer*, int, int, int, int);
	
	void update(SDL_Renderer*, Uint64, int);
	void shoot(SDL_Renderer*, BulletFactory&, Uint64);
};

class Splat : public Object {
	private:
	Uint64 remTime;
	public:
	Splat(SDL_Renderer*, int, int);

	void update(SDL_Renderer*, Uint64);
};

class EnemyBullet : public Object {
	public:
	EnemyBullet(SDL_Renderer*, int, int);
};

class Bunker {
	private:
	std::vector<SDL_Point> a;

	public:
	void insertPoint(int, int);
	void update(SDL_Renderer*, BulletFactory&);
	void remove(SDL_Renderer*, int, int);
};

