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

	void SetSwapObject(Gameobject * markedObject);
	void DestroyProjectile();

private:
	void ToggleGodMode();
	void StandardControls();
	void GodControls();
	void StandardMovement();
	void GodMovement();
	void LimitFallVelocity();
	void ShootProjectile();
	void SwapPosition();

	inline float tile_to_pixel(uint pixel);

public:
	//VARIABLES
	uint tileSize;
	float gravity;
	float moveSpeedAir;//(pixels/s)
	float moveSpeedGnd;//(pixels/s)
	float jumpSpeed;//(pixels/s)

	//Physics
	Collider* playerCol = nullptr;
	Collider* feetCol = nullptr;
	bool isOnPlatform = false;
	bool checkFall = false;

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

private:
	//Position swap
	float maxFallVelocity = 0.0f;
	Gameobject * swapObject = nullptr;

	//God mode
	bool godMode = false;

	// sound effects id relative to player
	uint teleport = 0;
	uint shoot = 0;
	uint die = 0;
	uint jump = 0;
};

#endif