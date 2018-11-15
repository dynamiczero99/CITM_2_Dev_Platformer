#ifndef _OBJ_ENEMY_LAND_H_
#define _OBJ_ENEMY_LAND_H

#include "j1Object.h"

class ObjEnemyLand : public GameObject {

public:
	ObjEnemyLand(fPoint &position, int index, pugi::xml_node &object_node);

};

#endif _OBJ_ENEMY_LAND_H_