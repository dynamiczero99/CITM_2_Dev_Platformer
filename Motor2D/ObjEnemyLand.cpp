#include "ObjEnemyLand.h"
#include "j1App.h"
#include "j1Object.h"
#include "p2Point.h"
#include "ObjPlayer.h"
#include "p2Log.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Pathfinding.h"
#include "j1Map.h"

#define TILE_WIDTH 8

ObjEnemyLand::ObjEnemyLand(fPoint & position, int index, pugi::xml_node & enemy_node) : ObjEnemy(position, index)
{
	detectionRange = 10;
	//detectionRange = enemy_node.child("detection_range").text().as_int();
	SDL_Rect colRect = {(int)position.x, (int)position.y, enemy_node.child("collider_rectangle").attribute("w").as_int(),  enemy_node.child("collider_rectangle").attribute("h").as_int() };
	col = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);
	LoadAnimation(enemy_node.child("animation").child("idle_animation"), idleAnim);
	LoadAnimation(enemy_node.child("animation").child("moving_animation"), movingAnim);
	currAnim = &movingAnim;
	pivot = Pivot(PivotV::bottom, PivotH::middle);
	updateCycle = 1000;
	gravity = 1500.0f;
	moveSpeed = 10.0f;
	maxFallSpeed = 50.0f;
	acceleration.y = gravity;
	reachOffset = 10;
}

bool ObjEnemyLand::PreUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		pathDebugDraw = !pathDebugDraw;
	}
	return true;
}

//Create a path to the player if it is in range
bool ObjEnemyLand::TimedUpdate(float dt)
{
	if (IsPlayerInTileRange(detectionRange) && App->object->player->position.y >= position.y - 10) {
		iPoint srcPos = App->map->WorldToMap((int)position.x, (int)position.y - TILE_WIDTH * 0.5f);
		iPoint trgPos = App->map->WorldToMap(App->object->player->position.x, App->object->player->position.y - TILE_WIDTH * 0.5f);
		App->pathfinding->CreatePathLand(srcPos, trgPos);
		pathData.CopyLastGeneratedPath();
		step = 0u;
		enemy_state = enemyState::CHASING;
	}
	else {
		GoIdle();
	}
	return true;
}

bool ObjEnemyLand::Update(float dt) {
	switch (enemy_state) {
	case enemyState::INVALID:
		LOG("Invalid state");
		break;
	case enemyState::IDLE:

		break;
	case enemyState::CHASING:
		if (pathData.path.Count() > 0) {
			//Check if we've reached the next node in the path
			iPoint enemyPos = App->map->WorldToMap((int)position.x, (int)position.y - TILE_WIDTH * 0.5f);
			if (abs(pathData.path[step].x - enemyPos.x) < reachOffset) {
				if (step + 1 < pathData.path.Count()) {
					step++;
				}
				else {
					GoIdle();
				}
			}
			//Move in the x direction to the next node
			if (pathData.path[step].x < enemyPos.x) {
				velocity.x = -moveSpeed;
			}
			else if (pathData.path[step].x > enemyPos.x){
				velocity.x = moveSpeed;
			}
			else {
				velocity.x = 0.0f;
			}
		}
		break;
	}

	velocity = velocity + acceleration * dt;//TODO: * 0.5f
	//TODO: Limit fall velocity
	position = position + velocity * dt;

	iPoint colPos = GetRectPos(pivot, (int)position.x, (int)position.y, col->rect.w, col->rect.h);
	col->SetPos(colPos.x, colPos.y);

	return true;
}

void ObjEnemyLand::GoIdle() {
	step = 0u;
	velocity.x = 0.0f;
	enemy_state = enemyState::IDLE;
}

bool ObjEnemyLand::PostUpdate() {
	DebugPath();
	SDL_Rect currRect = currAnim->GetCurrentFrame();
	iPoint blitPos = GetRectPos(pivot, (int)position.x, (int)position.y, currRect.w, currRect.h);
	App->render->Blit(App->object->robotTilesetTex, blitPos.x, blitPos.y, &currRect, 1.0f, flip);
	return true;
}

bool ObjEnemyLand::OnDestroy()
{
	App->collision->DeleteCollider(col);
	return true;
}

void ObjEnemyLand::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_BOX || c2->type == COLLIDER_GLASS) {
		uint dist[(uint)dir::max];
		dist[(uint)dir::invalid] = INT_MAX;
		dist[(uint)dir::left] = c2->rect.GetRight() - c1->rect.GetLeft();
		dist[(uint)dir::right] = c1->rect.GetRight() - c2->rect.GetLeft();
		dist[(uint)dir::down] = c1->rect.GetBottom() - c2->rect.GetTop();
		dist[(uint)dir::up] = c2->rect.GetBottom() - c1->rect.GetTop();
		dir nearestDir = dir::invalid;
		for (int i = 1; i < (int)dir::max; ++i) {
			if (dist[i] < dist[(uint)nearestDir]) {
				nearestDir = (dir)i;
			}
		}
		switch (nearestDir) {
		case dir::down:
			position.y = c2->rect.GetTop();
			velocity.y = 0;
			acceleration.y = 0;
			break;
		case dir::left:
			position.x = c2->rect.GetRight() + c1->rect.w / 2;
			velocity.x = 0;
			break;
		case dir::right:
			position.x = c2->rect.GetLeft() - c1->rect.w / 2;
			velocity.x = 0;
			break;
		case dir::up:
			position.y = c2->rect.GetBottom() + c1->rect.h;
			velocity.y = 0;
			break;
		default:
			LOG("Error checking enemy land collsion");
			break;
		}
		iPoint colPos = GetRectPos(pivot, (int)position.x, (int)position.y, c1->rect.w, c1->rect.h);
		c1->SetPos(colPos.x, colPos.y);
	}
}
