#ifndef __j1OBJECT_H__
#define __j1OBJECT_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_rect.h"
#include "PugiXml/src/pugixml.hpp"

#define MAX_OBJECTS 50

struct Collider;
enum COLLIDER_TYPE;

enum class OBJECT_TYPE {
	NONE,
	PLAYER,
	BOX,
	ENEMY
};

class Gameobject {
public:
	fPoint position;
	fPoint velocity;
	fPoint acceleration;

	//The position in the objects module array
	int index = -1;

	Gameobject(fPoint position, int index);
	~Gameobject();
	virtual bool PreUpdate();
	virtual bool Update();
	virtual bool PostUpdate();
	virtual void OnCollision(Collider * c1, Collider * c2);
};

class j1Object : public j1Module
{
public:
	j1Object();
	bool Awake(pugi::xml_node& node);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	//bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

	Gameobject* AddObject(OBJECT_TYPE type, fPoint position);
	bool DeleteObject(Gameobject * object);

private:
	Gameobject * objects [MAX_OBJECTS];
	uint actualObjects = 0;
	pugi::xml_node object_node;
};

#endif