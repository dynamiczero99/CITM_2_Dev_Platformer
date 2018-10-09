#ifndef __J1COLLISION_H__
#define __J1COLLISION_H__

#define MAX_COLLIDERS 300

#include "j1Module.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLAYER,
	COLLIDER_PLAYER_GOD,
	COLLIDER_ENEMY,
	COLLIDER_PLAYER_SHOT,
	COLLIDER_ENEMY_SHOT,
	//COLLIDER_POWER_UP,
	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;
	Uint32 damage;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr, Uint32 damage = 0) :
		rect(rectangle),
		type(type),
		callback(callback),
		damage(damage)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	// Called before render is available
	//bool Awake(pugi::xml_node&);

	// Called before the first frame
	//bool Start();

	// Called each loop iteration
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate()override;

	// Called before quitting
	bool CleanUp() override;

	// Load / Save
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;
	
	// counter of presents colliders on scene
	int actualColliders;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr, Uint32 damage = 0);
	void DebugDraw();

	bool exitGameLoop = false;

private:

	Collider * colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = true;
};

#endif // __J1COLLISION_H__