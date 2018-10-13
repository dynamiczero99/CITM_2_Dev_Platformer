#ifndef _OBJ_PLAYER_H_
#define _OBJ_PLAYER_H_

#include "j1Object.h"
#include "j1Collision.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"
#include "p2SString.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

struct SDL_Texture;
struct SDL_Rect;
class Animation;
struct Collider;
class ObjProjectile;

class ObjPlayer : public Gameobject {
private:
	enum class dir : uint {
		left,
		right,
		up,
		down,
		max
	};

public:
	ObjPlayer(pugi::xml_node & object_node, fPoint position, int index);

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

	bool PreUpdate() override;
	bool Update() override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void OnCollision(Collider* c1, Collider* c2) override;

	void OnCollisionPlayer(Collider * c2);
	void OnCollisionFeet(Collider * c2);

	void ShootProjectile();
	void MovePlayer();
	void CalculateDeltaTime();
	inline float tile_to_pixel(uint pixel);

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

	//Shoot
	ObjProjectile * projectile = nullptr;
	uint shootHeight;

	//Position swap
	Gameobject * swapObject = nullptr;
};

#endif