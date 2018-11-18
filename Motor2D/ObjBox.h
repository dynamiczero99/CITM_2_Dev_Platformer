#ifndef _OBJ_BOX_H_
#define _OBJ_BOX_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"

struct Collider;

class ObjBox : public GameObject {
public:
	ObjBox(fPoint &position, int index, pugi::xml_node &object_node, int objectID);
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool OnDestroy() override;
	void MarkObject(bool mark) override;

	void OnCollision(Collider * c1, Collider * c2) override;
	bool OnTriggerEnter();
	bool OnTriggerExit();

	bool Load(pugi::xml_node& loadNode);
	bool Save(pugi::xml_node& saveNode) const;

public:
	Animation inactiveAnim;
	Animation activeAnim;
	Animation * currAnim = nullptr;

private:
	uint animTileWidth = 0u;
	uint animTileHeight = 0u;
	float gravity = 0.0f;
};

#endif