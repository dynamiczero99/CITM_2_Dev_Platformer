#ifndef _OBJ_ENEMY_FLYING_H_
#define _OBJ_ENEMY_FLYING_H_

#include "j1Object.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2DynArray.h"

struct Collider;

class ObjEnemyFlying : public GameObject {
public:
	ObjEnemyFlying(fPoint position, int index, pugi::xml_node &object_node);
	bool Update(float dt) override;
	bool PreUpdate() override;
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

	// pathfinding relatives
	p2DynArray<iPoint> last_path = NULL;
	iPoint GetNextWorldNode();
	void MoveToWorldNode(const iPoint& node);
	void CopyLastGeneratedPath();
	iPoint GetMapPosition() const;

	void followPath();

	// provisional timer
	Uint32 start_time = 0;
	Uint32 frequency_time = 1000;

};

#endif