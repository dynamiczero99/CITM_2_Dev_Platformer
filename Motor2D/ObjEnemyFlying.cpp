#include "ObjEnemyFlying.h"
#include "j1Object.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1App.h"
#include "p2Animation.h"
#include "PugiXml/src/pugixml.hpp"
#include "p2Log.h"
#include "j1Pathfinding.h"
#include"objPlayer.h"
#include "j1Map.h"

ObjEnemyFlying::ObjEnemyFlying(fPoint position, int index, pugi::xml_node &enemy_node) : GameObject(position, index) {
	LoadAnimation(enemy_node.child("animation").child("idle_animation").child("sprite"), idleAnim);
	currAnim = &idleAnim;
	SDL_Rect colRect = {(int)position.x, (int)position.y, 10, 10};
	collider = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);

	
}

bool ObjEnemyFlying::OnDestroy() {
	App->collision->DeleteCollider(collider);
	return true;
}

void ObjEnemyFlying::MarkObject(bool mark)
{
	if (mark) {
	}
	else {
	}
}

bool ObjEnemyFlying::Update() {
	iPoint colPos = GetRectPos(pivot::bottom_middle, position.x, position.y, animTileWidth, animTileHeight);
	collider->SetPos(colPos.x, colPos.y);

	// pathfinding debug draw ---------------------------------------------------
	for (uint i = 0; i < last_path.Count() ; ++i)
	{
		iPoint pos = App->map->MapToWorld(last_path.At(i)->x, last_path.At(i)->y);
		App->render->Blit(App->object->debugEnemyPathTex, pos.x, pos.y);
	}
	// --------------------------------------------------------------------------

	return true;
}

bool ObjEnemyFlying::PostUpdate() {
	iPoint blitPos = GetRectPos(pivot::bottom_middle, position.x, position.y, animTileWidth, animTileHeight);
	App->render->Blit(App->object->robotTilesetTex, blitPos.x, blitPos.y, &currAnim->GetCurrentFrame());

	// testing path
	if (SDL_GetTicks() > start_time + frequency_time)
	{
		iPoint thisPos = App->map->WorldToMap((int)position.x, (int)position.y);
		iPoint playerPos = App->map->WorldToMap((int)App->object->player->position.x, (int)App->object->player->position.y);

		App->pathfinding->CreatePath(thisPos, playerPos);

		CopyLastGeneratedPath();

		start_time = SDL_GetTicks();
	}
	
	return true;
}

bool ObjEnemyFlying::Load(pugi::xml_node& node)
{
	return true;
}

bool ObjEnemyFlying::Save(pugi::xml_node& node) const
{
	LOG("Saving obj Enemy Flying");

	pugi::xml_node boxNode = node.append_child("EnemyFlying");

	boxNode.append_attribute("x") = position.x;
	boxNode.append_attribute("y") = position.y;
	boxNode.append_attribute("velocity_x") = velocity.x;
	boxNode.append_attribute("velocity_y") = velocity.y;


	return true;
}

void ObjEnemyFlying::followPath()
{


}

void ObjEnemyFlying::CopyLastGeneratedPath()
{
	const p2DynArray<iPoint>* pathToCopy = App->pathfinding->GetLastPath();

	last_path.Clear();
	for (uint i = 0; i < pathToCopy->Count(); ++i)
	{
		last_path.PushBack(*pathToCopy->At(i));
	}
}