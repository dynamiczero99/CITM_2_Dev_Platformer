#ifndef _OBJ_ENEMY_LAND_H_
#define _OBJ_ENEMY_LAND_H

#include "ObjEnemy.h"
#include "j1Object.h"
#include "p2Animation.h"
#include "SDL/include/SDL_render.h"

struct SDL_Texture;
struct Collider;

class ObjEnemyLand : public ObjEnemy {
public:
	ObjEnemyLand(fPoint &position, int index, pugi::xml_node &object_node);
	bool PreUpdate() override;
	bool TimedUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void OnCollision(Collider * c1, Collider * c2) override;
	bool Load(pugi::xml_node & loadNode) override;
	bool Save(pugi::xml_node & saveNode) const override;

private:
	void GoIdle();

private:
	int detectionRange = 0;
	Animation idleAnim;
	Animation movingAnim;
	Animation * currAnim = nullptr;
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
	float gravity = 0.0f;
	uint step = 0u;//The node in which it is in the array
	float moveSpeed = 0.0f;
	float maxFallSpeed = 0.0f;
	int reachOffset = 0;
	//The distance (pixels) in which it determines it has reached that node
	//If reachOffset is very small and moveSpeed too big it may pass the node and never go to the next node
};

#endif _OBJ_ENEMY_LAND_H_