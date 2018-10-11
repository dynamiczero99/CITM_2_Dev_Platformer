#ifndef _OBJ_PLAYER_H_
#define _OBJ_PLAYER_H_

#include "j1Object.h"
#include "j1Collision.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"
#include "p2SString.h"
#include "p2Animation.h"
#include "PugiXml/src/pugiconfig.hpp"

struct SDL_Texture;
struct SDL_Rect;
class Animation;
struct Collider;

class ObjPlayer : public Gameobject {
private:
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

	enum class dir : uint {
		left,
		right,
		up,
		down,
		max
	};

public:
	ObjPlayer(pugi::xml_node & object_node, fPoint position, int index);
	~ObjPlayer();

	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	void OnCollision(Collider* c1, Collider* c2) override;

	void OnCollisionPlayer(Collider * c2);
	void OnCollisionFeet(Collider * c2);

	void UpdateProjectile();
	void UpdatePlayer();
	void CalculateDeltaTime();
	inline float tile_to_pixel(uint pixel);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);
	//Returns the position it should draw (Blit) or put the collider (SetPos) considering a pivot point
	iPoint GetPosFromPivot(pivot pivot, int x, int y, uint w, uint h);

	//VARIABLES
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