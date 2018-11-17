#ifndef __j1PARTICLES_H__
#define __j1PARTICLES_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Log.h"
#include "p2Point.h"
#include "j1Collision.h"

#define MAX_ACTIVE_PARTICLES 500

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;
	SDL_Texture* texture = nullptr;
	uint fx = 0u;
	iPoint position;
	iPoint impactPosition = { NULL,NULL };
	//bool impactPositioning = false;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	Uint32 damage = 0;
	bool fx_played = false;

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);
	// variables to instantiate collision particle
	Particle* onCollisionWallParticle = nullptr;
	Particle* onCollisionGeneralParticle = nullptr;
	Particle* deathParticle = nullptr;

};

class j1Particles : public j1Module
{
public:

	j1Particles();
	~j1Particles();

	bool Awake(pugi::xml_node& node);
	bool Start();
	//bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	//void AddParticle(const Particle& particle, Animation& sourceAnim, int x, int y, Uint32 delay = 0, iPoint speed = { 0,0 }, Uint32 life = 0, char* fx = nullptr);
	void AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, iPoint speed = { 0,0 }, Uint32 delay = 0);

	bool LoadAnimation(pugi::xml_node &node, Animation &anim, bool sequential = false);

private:

	SDL_Texture* teleport01_tex = nullptr;
	SDL_Texture* teleport02_tex = nullptr;

	Particle* active[MAX_ACTIVE_PARTICLES];
	pugi::xml_node particleNode;

public:
	//Particle explosion;
	Particle impactShot;
	Particle teleport01;
	Particle teleport02;

};

#endif // __j1PARTICLES_H__