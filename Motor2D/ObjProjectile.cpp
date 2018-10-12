#include "ObjProjectile.h"
#include "ObjPlayer.h"
#include "p2Point.h"

ObjProjectile::ObjProjectile (fPoint position, int index) : Gameobject (position, index) {
}

bool ObjProjectile::Update() {
	position += velocity;
	//TODO: Multiply by deltaTime
	return true;
}

void ObjProjectile::OnCollision(Collider * c1, Collider * c2) {
	//Change properties of player
}