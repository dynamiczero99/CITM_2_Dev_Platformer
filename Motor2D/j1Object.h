#ifndef __j1OBJECT_H__
#define __j1OBJECT_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
#include "PugiXml/src/pugixml.hpp"

#define MAX_OBJECTS 50

struct Collider;
enum COLLIDER_TYPE;
class ObjPlayer;
class ObjProjectile;
class ObjBox;
struct SDL_Texture;
class Animation;

class Gameobject {
protected:
	enum class dir : uint {
		left,
		right,
		up,
		down,
		max
	};

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

public:
	Gameobject(fPoint position, int index);
	~Gameobject();
	virtual bool PreUpdate();
	virtual bool Update();
	virtual bool PostUpdate();
	virtual bool OnDestroy();
	virtual void OnCollision(Collider * c1, Collider * c2);
	virtual void MarkObject(bool mark);

	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;

protected:
	//Returns the position it should draw (Blit) or put the collider (SetPos) considering a pivot point
	iPoint GetPosFromPivot(pivot pivot, int x, int y, uint w, uint h);
	bool LoadAnimation(pugi::xml_node &node, Animation &anim);

public:
	fPoint position;
	fPoint velocity;
	fPoint acceleration;
	int index = -1;	//The position in the objects module array
};

class j1Object : public j1Module
{
public:
	j1Object();
	bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	int FindEmptyPosition();

	ObjPlayer * AddObjPlayer(fPoint position);
	ObjProjectile * AddObjProjectile(fPoint position, fPoint direction, ObjPlayer * objPlayer);
	ObjBox * AddObjBox(fPoint position);
	bool DeleteObject(Gameobject * object);

public:
	SDL_Texture * projectileTex = nullptr;
	SDL_Texture * playerIdleTex = nullptr;
	SDL_Texture * playerRunTex = nullptr;
	SDL_Texture * playerJumpTex = nullptr;
	SDL_Texture * robotTex = nullptr;
	ObjPlayer * player = nullptr;

private:
	Gameobject * objects [MAX_OBJECTS];
	uint actualObjects = 0;
	pugi::xml_node object_node;
	// sfx id
	uint impactBoxSFX = 0;
};

#endif