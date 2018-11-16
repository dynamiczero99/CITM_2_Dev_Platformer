#ifndef _OBJ_ENEMY_H_
#define _OBJ_ENEMY_H_

#include "j1Object.h"

class ObjEnemy : public GameObject {
public:
	ObjEnemy(fPoint &position, int index);
protected:
	enum enemyState
	{
		INVALID,
		IDLE, // represents idle state, searching a player in manhattan range
		CHASING, // player is in range and has a viable path
		DEATH
	};
	bool isPlayerInTileRange(const uint range) const;
};

#endif // !

