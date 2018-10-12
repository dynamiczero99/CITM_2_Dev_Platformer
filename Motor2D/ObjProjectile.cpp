#include "ObjProjectile.h"
#include "ObjPlayer.h"
#include "p2Point.h"
#include "p2Log.h"
#include "PugiXml/src/pugixml.hpp"
#include "j1App.h"
#include "j1Collision.h"

ObjProjectile::ObjProjectile (fPoint position, int index, pugi::xml_node & projectile_node, fPoint direction, ObjPlayer* player) : player(player), Gameobject (position, index) {
	if (projectile_node.empty()) {
		LOG("Empty node");
	}

	velocity = direction * projectile_node.child("speed").text().as_float();

	SDL_Rect colRect;
	colRect.x = 0;
	colRect.y = 0;
	colRect.w = projectile_node.child("collider_width").text().as_int();
	colRect.h = projectile_node.child("collider_height").text().as_int();
	collider = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_PLAYER_SHOT, this);
}

ObjProjectile::~ObjProjectile() {
	App->collision->DeleteCollider(collider);
}

bool ObjProjectile::Update() {
	position += velocity;
	//TODO: Multiply by deltaTime

	//Move projectile
	//projectilePos += projectileVelocity;
	//iPoint blitPos = GetPosFromPivot(pivot::middle_middle, projectilePos.x, projectilePos.y, projectileColRect.w, projectileColRect.h);
	//App->render->Blit(projectileTex, blitPos.x, blitPos.y);

	return true;
}

bool ObjProjectile::PostUpdate() {
	//Blit
	return true;
}

void ObjProjectile::OnCollision(Collider * c1, Collider * c2) {
	switch (c2->type) {
	case COLLIDER_TYPE::COLLIDER_BOX:
		//TODO: Mark that box visually
		//TODO: Play SFX
		if (player != nullptr) {
			player->swapObject = c2->callbackObj;
		}
		break;
	}
}