#include "ObjEnemyLand.h"
#include "j1App.h"
#include "j1Object.h"
#include "p2Point.h"
#include "ObjPlayer.h"
#include "p2Log.h"
#include "j1Collision.h"
#include "j1Render.h"

ObjEnemyLand::ObjEnemyLand(fPoint & position, int index, pugi::xml_node & enemy_node) : ObjEnemy(position, index)
{
	detectionRange = enemy_node.child("detection_range").text().as_int();
	SDL_Rect colRect = {(int)position.x, (int)position.y, enemy_node.child("collider_rectangle").attribute("w").as_int(),  enemy_node.child("collider_rectangle").attribute("h").as_int() };
	App->collision->AddCollider(colRect, COLLIDER_TYPE::COLLIDER_BOX, this);
	LoadAnimation(enemy_node.child("animation").child("idle_animation"), idleAnim);
	LoadAnimation(enemy_node.child("animation").child("moving_animation"), movingAnim);
	currAnim = &movingAnim;
}

//Create a path to the player if it is in range
bool ObjEnemyLand::TimedUpdate(float dt)
{
	if (position.DistanceManhattan(App->object->player->position) < detectionRange &&
		App->object->player->position.y <= position.y) {
		//Search path
		//Put in path to follow var
		//if found path
		state = enemyState::CHASING;
	}
	return false;
}

bool ObjEnemyLand::Update(float dt) {
	switch (state) {
	case enemyState::INVALID:
		LOG("Invalid state");
		break;
	case enemyState::IDLE:

		break;
	case enemyState::CHASING:

		break;
	}
	return true;
}

bool ObjEnemyLand::PostUpdate() {
	SDL_Rect currRect = currAnim->GetCurrentFrame();
	iPoint blitPos = GetRectPos(pivot::bottom_middle, (int)position.x, (int)position.y, currRect.w, currRect.h);
	App->render->Blit(App->object->robotTilesetTex, blitPos.x, blitPos.y, &currRect, 1.0f, flip);
	return true;
}