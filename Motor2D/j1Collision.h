#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#define MAX_COLLIDERS 300

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"
#include "j1Render.h"

class GameObject;

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_PLAYER,
	COLLIDER_PLAYER_SHOT,
	COLLIDER_BOX,
	COLLIDER_PLAYER_GOD,
	COLLIDER_DEATH_ZONE,
	COLLIDER_WIN_ZONE,
	COLLIDER_TRIGGER,
	COLLIDER_GLASS,
	COLLIDER_ENEMY,
	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	COLLIDER_TYPE type;
	GameObject * callbackObj = nullptr;
	ColorRGB color;

	//The position in the collider module array
	int index = -1;


	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, GameObject * callbackObj, ColorRGB color, int index) : rect(rectangle), type(type), callbackObj(callbackObj), color(color), index(index) {}
	void SetPos(int x, int y);
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
	bool Update(float dt) override;
	bool PostUpdate()override;

	// Called before quitting
	bool CleanUp() override;

	// Load / Save
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

	// counter of presents colliders on scene
	int actualColliders;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, ColorRGB color, GameObject * callbackObj = nullptr);
	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, GameObject * callbackObj = nullptr);

	bool DeleteCollider(Collider * collider);

	void DebugDraw();

	bool exitGameLoop = false;

private:

	Collider * colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	ColorRGB colliderColor[COLLIDER_MAX];
	bool debug = false;
};

#endif // __J1COLLISION_H__