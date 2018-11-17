#ifndef _OBJ_PROJECTILE_H_
#define _OBJ_PROJECTILE_H_

#include "p2Point.h"
#include "j1Object.h"
#include "PugiXml/src/pugixml.hpp"

class ObjPlayer;
struct Collider;

class ObjProjectile : public GameObject {
public:
	ObjProjectile(fPoint &position, int index, pugi::xml_node & projectile_node, fPoint direction, ObjPlayer* player);
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void OnCollision(Collider * c1, Collider * c2) override;

	bool Save(pugi::xml_node& saveNode) const;

public:
	ObjPlayer * player = nullptr;
	uint impactSound = 0u;
};

#endif
