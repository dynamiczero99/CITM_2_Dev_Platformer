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
	enum class pivot : uint {
		top_left,
		top_middle,
		top_right,
		middle_left,
		middle_middle,
		middle_right,
		bottom_left,
		bottom_middle,
		bottom_right
	};

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
	void UpdateProjectile();
	void UpdatePlayer();
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
	//Returns the position it should draw (Blit) or put the collider (SetPos) considering a pivot point
	iPoint GetPosFromPivot(pivot pivot, int x, int y, uint w, uint h);

private:

private:
	//Distances, speed and acceleration
	fPoint position;
	fPoint velocity;
	fPoint acceleration;

	uint tileSize;
	float gravity;
	float moveSpeedAir;//(pixels/s)
	float moveSpeedGnd;//(pixels/s)
	float jumpSpeed;//(pixels/s)

	//Physics
	Collider* playerCol;
	Collider* feetCol;
	bool isOnPlatform = false;
	bool checkFall = false;

	//Time
	float deltaTime = 0;
	Uint32 lastTime = 0;
	Uint32 currTime = 0;
	bool isFirstFrame = true;

	//Animation
	uint animTileWidth = 0;
	uint animTileHeight = 0;

	SDL_Texture* currTex = nullptr;
	SDL_Texture* idleTex = nullptr;
	SDL_Texture* runTex = nullptr;
	SDL_Texture* jumpTex = nullptr;

	Animation* currAnim = nullptr;
	p2SString idlePath;
	p2SString runPath;
	p2SString jumpPath;
	Animation idleAnim;
	Animation jumpAnim;
	Animation runAnim;
	Animation fallAnim;

	SDL_RendererFlip flip;
	bool movingLeft;
	bool movingRight;
	bool goingUp;
	bool goingDown;

	//Projectile
	p2SString projectilePath;
	SDL_Texture* projectileTex;
	SDL_Rect projectileColRect;
	Collider* projectileCol;
	uint projectileStartHeight;
	fPoint projectilePos;
	fPoint projectileVelocity;
	float projectileSpeed;
};

#endif