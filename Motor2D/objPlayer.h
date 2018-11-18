#ifndef _OBJ_PLAYER_H_
#define _OBJ_PLAYER_H_

#include "j1Object.h"
#include "j1Collision.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"
#include "p2SString.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "j1Timer.h"

struct SDL_Texture;
struct SDL_Rect;
class Animation;
struct Collider;
class ObjProjectile;

class ObjPlayer : public GameObject {
public:
	ObjPlayer(pugi::xml_node & object_node, fPoint &position, int index);

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

	bool PreUpdate() override;

	bool Update(float dt) override;
	
	bool PostUpdate() override;
	bool OnDestroy() override;
	void OnCollision(Collider* c1, Collider* c2) override;

	void SetSwapObject(GameObject * markedObject);
	void DestroyProjectile();
	void Die();

private:
	void ToggleGodMode();
	void StandardControls();
	void GodControls();
	void StandardMovement(float dt);
	void GodMovement(float dt);

	void ShootProjectile();
	void SwapPosition();

	void OnCollisionFeet(Collider * c2);
	void SolveCollision(Collider * c2);
	void CollideWinZone();

	int LimitDistance(int distance);
	dir GetSmallestDirFiltered(Collider * c2);
	dir GetSmallestDir(Collider * c2);

	//Variables
public:
	float gravity = 0.0f;
	float moveSpeedAir = 0.0f;//(pixels/s)
	float moveSpeedGnd = 0.0f;//(pixels/s)
	float jumpSpeed = 0.0f;//(pixels/s)

	//Physics
	Collider* feetCol = nullptr;
	bool isOnPlatform = false;
	bool checkFallPlatform = false;

	//Animation
	uint animTileWidth = 0;
	uint animTileHeight = 0;

	SDL_Texture* currTex = nullptr;
	Animation* currAnim = nullptr;

	Animation idleAnim;
	Animation jumpAnim;
	Animation runAnim;
	Animation fallAnim;

	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;

	//Shoot
	ObjProjectile * projectile = nullptr;
	uint shootHeight = 0u;
	j1Timer recoveryTimer;
	float recoveryTime = 0.0f;//Time it takes for the player to move again after shooting
	float angle = 0.0f;

private:
	//Position swap
	GameObject * swapObject = nullptr;

	//God mode
	bool godMode = false;

	// sound effects id relative to player
	uint teleport = 0u;
	uint shoot = 0u;
	uint aim = 0u;
	uint death = 0u;
	uint jump = 0u;
	uint win = 0u;
};

#endif