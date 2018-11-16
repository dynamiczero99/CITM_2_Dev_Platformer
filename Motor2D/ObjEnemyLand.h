#ifndef _OBJ_ENEMY_LAND_H_
#define _OBJ_ENEMY_LAND_H

#include "j1Object.h"
#include "p2Animation.h"
#include "SDL/include/SDL_render.h"

struct SDL_Texture;
struct Collider;

class ObjEnemyLand : public GameObject {
	enum class state {
		invalid,
		moving,//Moves left and right of the platform
		chasing//Follows the player
	};

public:
	ObjEnemyLand(fPoint &position, int index, pugi::xml_node &object_node);
	bool TimedUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate();

private:
	state state = state::moving;
	Collider * col = nullptr;
	int detectionRange = 0;
	Animation idleAnim;
	Animation movingAnim;
	Animation * currAnim = nullptr;
	SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE;
};

#endif _OBJ_ENEMY_LAND_H_