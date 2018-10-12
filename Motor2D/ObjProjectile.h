#ifndef _OBJ_PROJECTILE_H_
#define _OBJ_PROJECTILE_H_

#include "p2Point.h"
#include "j1Object.h"
#include "PugiXml/src/pugixml.hpp"

class ObjPlayer;
struct Collider;

class ObjProjectile : public Gameobject {
public:
	ObjProjectile(fPoint position, int index, pugi::xml_node & projectile_node, fPoint direction, ObjPlayer* player);
	~ObjProjectile();
	bool Update() override;
	bool PostUpdate() override;
	void OnCollision(Collider * c1, Collider * c2) override;

public:
	Collider * collider;
	ObjPlayer * player;
};

#endif
