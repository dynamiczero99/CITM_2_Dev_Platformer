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
	bool TimedUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate();
	bool OnDestroy() override;

private:
	Collider * col = nullptr;
	int detectionRange = 0;
	Animation idleAnim;
	Animation movingAnim;
	Animation * currAnim = nullptr;
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
};

#endif _OBJ_ENEMY_LAND_H_