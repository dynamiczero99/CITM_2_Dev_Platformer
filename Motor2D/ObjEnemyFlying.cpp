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
#include "ObjPlayer.h"
#include "j1Map.h"

ObjEnemyFlying::ObjEnemyFlying(fPoint position, int index, pugi::xml_node &enemy_node) : GameObject(position, index) {
	LoadAnimation(enemy_node.child("animation").child("idle_animation"), idleAnim);
	currAnim = &idleAnim;
	SDL_Rect colRect = {(int)position.x, (int)position.y, 14, 22};
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

bool ObjEnemyFlying::PreUpdate()
{
	// testing path
	if (SDL_GetTicks() > start_time + frequency_time)
		//static bool cmon = false;
		//if(!cmon)
	{
		iPoint thisPos = App->map->WorldToMap((int)position.x, (int)position.y);
		iPoint playerPos = App->map->WorldToMap((int)App->object->player->position.x, (int)App->object->player->position.y);

		if (thisPos.DistanceManhattan(playerPos) > 1) // if the enemy is at more than 1 distance manhattan
		{
			if (App->pathfinding->CreatePath(thisPos, playerPos) > 0) // if new path cannot be created, continue with the last valid one
			{
				CopyLastGeneratedPath();
			}
		}

		start_time = SDL_GetTicks();
		//cmon = true;
	}

	return true;
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

	// pathfinding
	if (last_path.Count() > 0)
		followPath();

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
	iPoint nextNode = GetNextWorldNode();
	//iPoint thisPos = GetMapPosition();

	LOG("next world node: %i,%i", nextNode.x, nextNode.y);
	LOG("this position: %i,%i", (int)position.x, (int)position.y);

	MoveToWorldNode(nextNode);

}

iPoint ObjEnemyFlying::GetMapPosition() const
{
	return App->map->WorldToMap(position.x, position.y);
}


void ObjEnemyFlying::MoveToWorldNode(const iPoint& node)
{
	fPoint velocity_vector;
	velocity_vector.x = (int)node.x;
	velocity_vector.y = (int)node.y;

	velocity_vector -= position;
	velocity_vector.Normalize();

	LOG("velocity %f,%f", velocity_vector.x, velocity_vector.y);

	position.x += velocity_vector.x * 1.5f;
	position.y += velocity_vector.y * 1.5f;

}

iPoint ObjEnemyFlying::GetNextWorldNode()
{
	// get the enemy pos (this position) on map coords. (tile coords.)
	iPoint thisPos;
	thisPos = App->map->WorldToMap((int)position.x, (int)position.y);

	// get the nextNodePos, the last on dyn array (the first pop out) || copylastgenerated path flip the order
	iPoint nextNodePos = *last_path.At(last_path.Count() - 1);

	// compare enemy and nextNode on tile coords, if is the same, pop and get the new nextNode
	iPoint areaPoint = { 1,1 }; // tile values
	if (!(thisPos.x >= (nextNodePos.x + areaPoint.x) || (thisPos.x + 2) <= nextNodePos.x || // enemy tile width 
		thisPos.y >= (nextNodePos.y + areaPoint.y) || (thisPos.y + 3) <= nextNodePos.y)) // enemy tile height
	{
		last_path.Pop(nextNodePos);
		LOG("enemy are on target tile pos: tile: %i,%i enemy: %i,%i", nextNodePos.x, nextNodePos.y, thisPos.x, thisPos.y);
	}

	if (last_path.Count() > 0)
		return App->map->MapToWorld(nextNodePos.x, nextNodePos.y);
	else
		return thisPos;
}

void ObjEnemyFlying::CopyLastGeneratedPath()
{
	const p2DynArray<iPoint>* pathToCopy = App->pathfinding->GetLastPath();

	last_path.Clear();
	for (uint i = 0; i < pathToCopy->Count(); ++i)
	{
		last_path.PushBack(*pathToCopy->At(i));
	}
	last_path.Flip();
}