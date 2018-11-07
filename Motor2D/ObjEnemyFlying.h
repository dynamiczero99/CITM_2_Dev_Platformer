#ifndef _OBJ_ENEMY_FLYING_H_
#define _OBJ_ENEMY_FLYING_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

struct Collider;

class ObjEnemyFlying : public GameObject {
public:
	ObjEnemyFlying(fPoint position, int index, pugi::xml_node &object_node);
	bool Update() override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void MarkObject(bool mark) override;

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

public:
	Animation idleAnim;
	Animation * currAnim = nullptr;

private:
	uint animTileWidth = 0u;
	uint animTileHeight = 0u;
	Collider * collider = nullptr;
};

#endif