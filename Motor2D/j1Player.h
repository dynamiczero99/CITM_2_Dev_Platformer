#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"
#include "p2SString.h"
#include "p2Animation.h"

struct SDL_Texture;
struct SDL_Rect;
class Animation;
struct Collider;

class j1Player : public j1Module
{
	enum class dir: uint {
		left,
		right,
		up,
		down,
		max
	};

public:
	j1Player();
	// Called before render is available
	bool Awake(pugi::xml_node&);
	// Called before the first frame
	bool Start();
	// Called each loop iteration
	bool PreUpdate();
	// Called each loop iteration
	bool Update(float dt);
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	void OnCollision(Collider* c1, Collider* c2);

	inline float tile_to_pixel(uint pixel);

private:

private:
	//TODO: Set the velocity in the xml

	fPoint position;
	fPoint velocity;
	fPoint acceleration;
	//TODO: Add acceleration (to use incorporate gravity easily)
	SDL_Texture* currTex;
	SDL_Texture* characterTex;
	Animation* currentAnim;
	Animation idleAnim;
	Animation jumpAnim;
	Animation runAnim;
	Animation deathAnim;
	uint anim_tile_width = 0;
	uint anim_tile_height = 0;

	float moveSpeedAir;//(pixels/s)
	float moveSpeedGnd;//(pixels/s)
	float jumpSpeed;//(pixels/s)

	uint tile_size;

	p2SString path;
	SDL_RendererFlip flip;
	float deltaTime = 0;
	Uint32 lastTime = 0;
	Uint32 currTime = 0;
	bool isFirstFrame = true;
	float gravity;
	Collider* playerCol;
	Collider* footCol;

	bool isOnPlatform = false;
	bool checkFoot = false;
};

#endif