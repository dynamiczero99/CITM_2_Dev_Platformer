#ifndef _OBJ_PROJECTILE_H_
#define _OBJ_PROJECTILE_H_

#include "p2Point.h"
#include "j1Object.h"

class ObjPlayer;
struct Collider;

class ObjProjectile : public Gameobject {
public:
	ObjProjectile(fPoint position, int index);
	bool Update() override;
	void OnCollision(Collider * c1, Collider * c2) override;

public:
	ObjPlayer * player;
};

#endif
