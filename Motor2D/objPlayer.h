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

class ObjPlayer : public GameObject {
public:
	ObjPlayer(pugi::xml_node & object_node, fPoint position, int index);

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

	bool PreUpdate() override;

	bool Update(float dt) override;
	
	bool PostUpdate() override;
	bool OnDestroy() override;
	void OnCollision(Collider* c1, Collider* c2) override;

	void SetSwapObject(GameObject * markedObject);
	void DestroyProjectile();

private:
	void ToggleGodMode();
	void StandardControls();
	void GodControls();
	void StandardMovement(float dt);
	void GodMovement(float dt);

	void LimitFallVelocity();
	void ShootProjectile();
	void SwapPosition();

	void OnCollisionFeet(Collider * c2);
	void SolveCollision(Collider * c2);
	void CollideDeathZone();
	void CollideWinZone();

	int LimitDistance(int distance);
	GameObject::dir GetSmallestDirFiltered(Collider * c2);
	GameObject::dir GetSmallestDir(Collider * c2);

public:
	//Variables
	float gravity = 0.0f;
	float moveSpeedAir = 0.0f;//(pixels/s)
	float moveSpeedGnd = 0.0f;//(pixels/s)
	float jumpSpeed = 0.0f;//(pixels/s)

	//Physics
	Collider* playerCol = nullptr;
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
	bool movingLeft = false;
	bool movingRight = false;
	bool goingUp = false;
	bool goingDown = false;

	//Shoot
	ObjProjectile * projectile = nullptr;
	uint shootHeight = 0u;

private:
	//Position swap
	float maxFallVelocity = 0.0f;
	GameObject * swapObject = nullptr;

	//God mode
	bool godMode = false;

	// sound effects id relative to player
	uint teleport = 0u;
	uint shoot = 0u;
	uint death = 0u;
	uint jump = 0u;
	uint win = 0u;
};

#endif