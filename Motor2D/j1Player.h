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
	void MoveProjectile();
	void MovePlayer();
	void CalculateDeltaTime();
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	void OnCollision(Collider* c1, Collider* c2);

	inline float tile_to_pixel(uint pixel);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);
	void ChangeAnimation(SDL_Texture* tex, Animation &anim);

private:

private:
	fPoint position;
	fPoint velocity;
	fPoint acceleration;

	//Animation
	uint anim_tile_width = 0;
	uint anim_tile_height = 0;

	SDL_Texture* currTex = nullptr;
	SDL_Texture* idleTex = nullptr;
	SDL_Texture* runTex = nullptr;
	SDL_Texture* jumpTex = nullptr;

	Animation* currAnim = nullptr;
	Animation idleAnim;
	Animation jumpAnim;
	Animation runAnim;
	Animation fallAnim;

	float moveSpeedAir;//(pixels/s)
	float moveSpeedGnd;//(pixels/s)
	float jumpSpeed;//(pixels/s)

	uint tile_size;

	p2SString idle_path;
	p2SString run_path;
	p2SString jump_path;
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

	fPoint projectilePos;
	fPoint projectileVel;
	float projectileSpeed;
};

#endif