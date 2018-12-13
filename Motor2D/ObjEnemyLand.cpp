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

ObjEnemyLand::ObjEnemyLand(fPoint & position, int index, pugi::xml_node & enemy_node) : ObjEnemy(position, index)
{
	detectionRange = enemy_node.child("detection_range").text().as_int();
	SDL_Rect colRect = {(int)position.x, (int)position.y, enemy_node.child("collider_rectangle").attribute("w").as_int(),  enemy_node.child("collider_rectangle").attribute("h").as_int() };
	col = App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);
	LoadAnimation(enemy_node.child("animation").child("idle_animation"), idleAnim);
	LoadAnimation(enemy_node.child("animation").child("moving_animation"), movingAnim);
	currAnim = &idleAnim;
	updateCycle = enemy_node.child("update_cycle").text().as_uint();
	gravity = TileToPixel(enemy_node.child("gravity").text().as_uint());
	moveSpeed = enemy_node.child("move_speed").text().as_float();
	reachOffset = enemy_node.child("reach_offset").text().as_int();
	maxFallSpeed = enemy_node.child("maximum_fall_velocity").text().as_float();

	pivot = Pivot(PivotV::bottom, PivotH::middle);
	acceleration.y = gravity;
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
	//LOG("Timed update");
	if (IsPlayerInTileRange(detectionRange) && App->object->player->position.y >= position.y - 10) {
		iPoint srcPos = App->map->WorldToMap((int)position.x, (int)position.y - App->map->GetTileWidth() * 0.5f);
		iPoint trgPos = App->map->WorldToMap(App->object->player->position.x, App->object->player->position.y - App->map->GetTileWidth() * 0.5f);
		if (App->pathfinding->CreatePathLand(srcPos, trgPos) > -1) {
			pathData.CopyLastGeneratedPath();
			step = 0u;
			currAnim = &movingAnim;
			enemy_state = enemyState::CHASING;
		}
	}
	else {
		GoIdle();
	}
	return true;
}

bool ObjEnemyLand::Update(float dt) {
	//LOG("Velocity %f", velocity.x);
	switch (enemy_state) {
	case enemyState::INVALID:
		//LOG("Invalid state");
		break;
	case enemyState::IDLE:

		break;
	case enemyState::CHASING:
		if (pathData.path.Count() > 0) {
			//Check if we've reached the next node in the path
			iPoint iposition ((int)position.x, (int)position.y);
			iPoint targetTileWorldPos = App->map->MapToWorld(pathData.path[step].x, pathData.path[step].y);
			//The position of the tiles is given from the top left
			targetTileWorldPos.x += App->map->GetTileWidth() * 0.5;//Middle
			targetTileWorldPos.y += App->map->GetTileWidth();//Bottom
			LOG("src pos y: %i", iposition.y);
			LOG("trg pos y: %i", targetTileWorldPos.y);
			if (iposition.DistanceManhattan(targetTileWorldPos) < reachOffset) {
				if (step + 1 < pathData.path.Count()) {
					step++;
				}
				else {
					GoIdle();
				}
			}
			//Move in the x direction to the next node
			if (targetTileWorldPos.x < iposition.x) {
				velocity.x = -moveSpeed;
				flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			}
			else if (targetTileWorldPos.x > iposition.x){
				velocity.x = moveSpeed;
				flip = SDL_RendererFlip::SDL_FLIP_NONE;
			}
			else {
				velocity.x = 0.0f;
			}
		}
		break;
	}

	velocity = velocity + acceleration * dt;
	LimitFallSpeed(dt);
	position = position + velocity * dt;

	iPoint colPos = GetRectPos(pivot, (int)position.x, (int)position.y, col->rect.w, col->rect.h);
	col->SetPos(colPos.x, colPos.y);

	return true;
}

void ObjEnemyLand::GoIdle() {
	step = 0u;
	velocity.x = 0.0f;
	currAnim = &idleAnim;
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

void ObjEnemyLand::MarkObject(bool mark)
{
	//if (mark) {
	//	currAnim = &idleAnimMarked;
	//}
	//else {
	//	currAnim = &idleAnimSearching;
	//}

	//marked = mark; // updates internal state
}

void ObjEnemyLand::OnCollision(Collider * c1, Collider * c2) {
	if (c2->type == COLLIDER_PLAYER) {
		if (c2->callbackObj->position.y > position.y - col->rect.h * 0.5f) {
			//TODO: This will cause problems if we add 2 players
			App->object->player->Die();
		}
	}
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

bool ObjEnemyLand::Load(pugi::xml_node & loadNode)
{
	p2SString flipDir = loadNode.child("landEnemy").attribute("flip_direction").as_string();
	// check facing direction
	if (flipDir == "true")
	{
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}
	else {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}
	return false;
}

bool ObjEnemyLand::Save(pugi::xml_node& node) const
{
	LOG("LandEnemy");

	pugi::xml_node landEnemy = node.append_child("landEnemy");

	iPoint pos = App->map->WorldToMap((int)position.x, (int)position.y); // stick to map coords.
	pos = App->map->MapToWorld(pos.x, pos.y); // and save in world coords to easy load

	landEnemy.append_attribute("x") = pos.x;
	landEnemy.append_attribute("y") = pos.y;
	//landEnemy.append_attribute("velocity_x") = velocity.x;
	//landEnemy.append_attribute("velocity_y") = velocity.y;

	//if (marked) {
	//	landEnemy.append_attribute("isMarked") = true;
	//}

	if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		landEnemy.append_attribute("flip_direction") = "true";
	}
	else {
		landEnemy.append_attribute("flip_direction") = "false";
	}

	return true;
}
