#ifndef _OBJ_BOX_H_
#define _OBJ_BOX_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

struct Collider;

class ObjBox : public Gameobject {
public:
	ObjBox(fPoint position, int index, pugi::xml_node &object_node);
	bool Update() override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void MarkObject(bool mark) override;

	void OnCollision(Collider * c1, Collider * c2) override;

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

public:
	Animation inactiveAnim;
	Animation activeAnim;
	Animation * currAnim = nullptr;

private:
	uint animTileWidth = 0u;
	uint animTileHeight = 0u;
	Collider * collider = nullptr;
	float gravity = 0.0f;
};

#endif