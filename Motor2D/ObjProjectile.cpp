#include "ObjProjectile.h"
#include "ObjPlayer.h"
#include "p2Point.h"
#include "p2Log.h"
#include "PugiXml/src/pugixml.hpp"
#include "j1App.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "ObjBox.h"
#include "j1Audio.h"

ObjProjectile::ObjProjectile (fPoint &position, int index, pugi::xml_node & projectile_node, fPoint direction, ObjPlayer* player) : player(player), GameObject (position, index) {
	acceleration = fPoint(0.0f, 0.0f);

	if (projectile_node.empty()) {
		LOG("Empty node");
	}

	velocity = direction * TileToPixel(projectile_node.child("speed").text().as_float());
	SDL_Rect colRect;
	colRect.w = projectile_node.child("collider_width").text().as_int();
	colRect.h = projectile_node.child("collider_height").text().as_int();
	iPoint colPos = GetRectPos(Pivot(PivotV::middle, PivotH::middle), position.x, position.y, colRect.w, colRect.h);
	colRect.x = colPos.x;
	colRect.y = colPos.y;
	col = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_PLAYER_SHOT, this);

	pivot = Pivot(PivotV::middle, PivotH::middle);
	
	// loads sfx relative to projectile ---
	
}

bool ObjProjectile::OnDestroy() {
	App->collision->DeleteCollider(col);
	return true;
}

bool ObjProjectile::Update(float dt) {
	position += velocity * dt;
	iPoint colPos = GetRectPos(pivot, position.x, position.y, col->rect.w, col->rect.h);
	col->SetPos(colPos.x, colPos.y);
	return true;
}

bool ObjProjectile::PostUpdate() {
	//INFO: In this case the width of the sprite and the collider is the same
	//INFO: If the sprite was animates we would get the property from Animation.rect.w & Animation.rect.h
	iPoint blitPos = GetRectPos(pivot, position.x, position.y, col->rect.w, col->rect.h);
	App->render->Blit(App->object->projectileTex, blitPos.x, blitPos.y);
	return true;
}

void ObjProjectile::OnCollision(Collider * c1, Collider * c2) {
	switch (c2->type) {
	case COLLIDER_TYPE::COLLIDER_BOX:
		App->audio->PlayFx(App->object->impactBoxSFX);
		if (player != nullptr) {
			c2->callbackObj->MarkObject(true);
			player->SetSwapObject(c2->callbackObj);
		}
		break;
	case COLLIDER_TYPE::COLLIDER_ENEMY:
		App->audio->PlayFx(App->object->impactBoxSFX);
		if (player != nullptr) {
			c2->callbackObj->MarkObject(true);
			player->SetSwapObject(c2->callbackObj);
		}
		break;
	case COLLIDER_TYPE::COLLIDER_WALL:
		player->DestroyProjectile();
		break;
	}


}

bool ObjProjectile::Save(pugi::xml_node& node) const
{
	LOG("Saving obj projectile");

	pugi::xml_node projectileNode = node.append_child("Projectile");

	projectileNode.append_attribute("x") = position.x;
	projectileNode.append_attribute("y") = position.y;
	projectileNode.append_attribute("velocity_x") = velocity.x;
	projectileNode.append_attribute("velocity_y") = velocity.y;

	return true;
}