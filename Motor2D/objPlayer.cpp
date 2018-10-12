#include "j1Collision.h"
#include "math.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1App.h"
#include "p2Animation.h"
#include "p2Defs.h"
#include "p2Point.h"
#include "PugiXml/src/pugixml.hpp"
#include "ObjProjectile.h"
#include "j1Object.h"
#include "ObjPlayer.h"

ObjPlayer::ObjPlayer(pugi::xml_node & playerNode, fPoint position, int index) : Gameobject(position, index) {

	if (playerNode.empty())
		LOG("empty node");

	//Values from xml
	SDL_Rect colliderRect;
	colliderRect.w = playerNode.child("collider_width").text().as_int();
	colliderRect.h = playerNode.child("collider_height").text().as_int();
	playerCol = App->collision->AddCollider(colliderRect, COLLIDER_PLAYER, this);
	SDL_Rect feetColRect;
	feetColRect.w = playerNode.child("feet_collider_width").text().as_int();
	feetColRect.h = playerNode.child("feet_collider_height").text().as_int();
	feetCol = App->collision->AddCollider(feetColRect, COLLIDER_PLAYER, this);
	tileSize = playerNode.child("tile_size").text().as_uint();
	gravity = tile_to_pixel(playerNode.child("gravity").text().as_float());
	moveSpeedGnd = tile_to_pixel(playerNode.child("move_speed_ground").text().as_float());
	moveSpeedAir = tile_to_pixel(playerNode.child("move_speed_air").text().as_float());
	//- This formula traduces gives us the speed necessary to reach a certain height
	//- It is calculated using the conservation of mechanic energy
	jumpSpeed = -sqrtf(gravity * tile_to_pixel(playerNode.child("jump_height").text().as_float()) * 2.0f);

	//Shoot
	shootHeight = playerNode.child("shoot_height").text().as_uint();

	//Animation
	animTileWidth = playerNode.child("animation").attribute("tile_width").as_uint();
	animTileHeight = playerNode.child("animation").attribute("tile_height").as_uint();

	currTex = App->object->playerIdleTex;

	LoadAnimation(playerNode.child("animation").child("idle_animation").child("sprite"), idleAnim);
	idleAnim.speed = playerNode.child("animation").child("idle_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("run_animation").child("sprite"), runAnim);
	runAnim.speed = playerNode.child("animation").child("run_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("jump_animation").child("sprite"), jumpAnim);
	jumpAnim.speed = playerNode.child("animation").child("jump_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("fall_animation").child("sprite"), fallAnim);
	fallAnim.speed = playerNode.child("animation").child("fall_animation").attribute("speed").as_float();
	currAnim = &idleAnim;
}

bool ObjPlayer::OnDestroy() {
	App->collision->DeleteCollider(playerCol);
	App->collision->DeleteCollider(feetCol);
	return true;
}

bool ObjPlayer::PreUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {
		if (isOnPlatform) {
			velocity.x = -moveSpeedGnd;
		}
		else {
			velocity.x = -moveSpeedAir;
		}
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
		if (isOnPlatform) {
			velocity.x = moveSpeedGnd;
		}
		else {
			velocity.x = moveSpeedAir;
		}
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}
	else {
		velocity.x = 0;
	}

	// Check that it is hitting the ground to be able to jump (he could jump on the air otherwise)
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isOnPlatform) {
		velocity.y = jumpSpeed;
		isOnPlatform = false;
		checkFall = false;
	}

	return true;
}

bool ObjPlayer::Update() {
	CalculateDeltaTime();
	MovePlayer();
	ShootProjectile();
	return true;
}

bool ObjPlayer::PostUpdate() {
	//Once the movement and the physical resolution has happened, determine the animations it must play

	if (velocity.x > 0) {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}
	else if (velocity.x < 0) {
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}

	if (isOnPlatform) {
		if (velocity.x != 0) {
			currTex = App->object->playerRunTex;
			currAnim = &runAnim;
		}
		else {
			currTex = App->object->playerIdleTex;
			currAnim = &idleAnim;
		}
	}
	else {
		currTex = currTex = App->object->playerJumpTex;
		if (velocity.y <= 0) {
			currAnim = &jumpAnim;
		}
		else {
			currAnim = &fallAnim;
		}
	}

	iPoint blitPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, animTileWidth, animTileHeight);
	App->render->Blit(currTex, blitPos.x, blitPos.y, &currAnim->GetCurrentFrame(), 1.0f, flip);
	return true;
}

void ObjPlayer::OnCollision(Collider * c1, Collider * c2) {
	if (c1 == playerCol) {
		OnCollisionPlayer(c2);
	}
	else if (c1 == feetCol) {
		OnCollisionFeet(c2);
	}
}

void ObjPlayer::OnCollisionPlayer(Collider * c2)
{
	switch (c2->type) {
	case COLLIDER_WALL:
		//1. Check which direction it was going to
		bool direction[(uint)dir::max];
		direction[(uint)dir::up] = velocity.y < 0;
		direction[(uint)dir::down] = velocity.y > 0;
		direction[(uint)dir::left] = velocity.x < 0;
		direction[(uint)dir::right] = velocity.x > 0;

		//2. Check which point (opposite to those directions) is the nearest
		//Ex.: If it has entered in the direction "up" the distance is from the character to the bottom of the other collider
		uint dist[(uint)dir::max];
		dist[(uint)dir::up] = (c2->rect.y + c2->rect.h) - (position.y - playerCol->rect.h);
		dist[(uint)dir::down] = position.y - c2->rect.y;
		dist[(uint)dir::left] = (c2->rect.x + c2->rect.w) - (position.x - playerCol->rect.w / 2);
		dist[(uint)dir::right] = (position.x + playerCol->rect.w / 2) - c2->rect.x;
		int nearestDir = -1;
		for (int i = 0; i < (int)dir::max; ++i) {
			if (direction[i]) {
				if (nearestDir == -1) {
					nearestDir = i;
				}
				else if (dist[i] < dist[nearestDir]) {
					nearestDir = i;
				}
			}
		}

		//3. Move it to that point
		switch (nearestDir) {
		case (int)dir::down:
			position.y = c2->rect.y;
			velocity.y = 0;
			acceleration.y = 0;
			checkFall = true;
			isOnPlatform = true;
			break;
		case (int)dir::up:
			position.y = c2->rect.y + c2->rect.h + playerCol->rect.h;
			velocity.y = 0;
			break;
		case (int)dir::left:
			position.x = c2->rect.x + c2->rect.w + playerCol->rect.w / 2;
			velocity.x = 0;
			break;
		case (int)dir::right:
			position.x = c2->rect.x - playerCol->rect.w / 2;
			velocity.x = 0;
			break;
		}
		iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
		playerCol->SetPos(colPos.x, colPos.y);
		feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);
		break;
	}
}

void ObjPlayer::OnCollisionFeet(Collider * c2)
{
	//Only starts checking if it falls when it snaps to the platform
	//Otherwise it would stop adding gravity the moment the foot collider touched the floor, making the character be way above what it should be
	if (checkFall) {
		isOnPlatform = true;
	}
}

//We can consider that 1 tile = 1 meter to make it easier for us to imagine the different values
inline float ObjPlayer::tile_to_pixel(uint pixel) {
	return pixel * tileSize;
}

void ObjPlayer::CalculateDeltaTime()
{
	if (isFirstFrame) {
		currTime = lastTime = SDL_GetTicks();
		isFirstFrame = false;
	}
	else {
		currTime = SDL_GetTicks();
	}
	deltaTime = currTime - lastTime;
	deltaTime /= 1000;//1 second is 1000 miliseconds
	lastTime = currTime;
}

void ObjPlayer::MovePlayer()
{
	if (isOnPlatform) {
		acceleration.y = 0;
	}
	else {
		acceleration.y = gravity;
		checkFall = false;
	}

	velocity = velocity + acceleration * deltaTime;
	position = position + velocity * deltaTime;
	iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
	playerCol->SetPos(colPos.x, colPos.y);
	feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);

	// - If this value remains false after checking the collision we'll consider the player has fallen from the platform
	isOnPlatform = false;
}

void ObjPlayer::ShootProjectile()
{
	if (App->input->GetMouseButton(1) == KEY_DOWN) {

		if (projectile != nullptr) {
			App->object->DeleteObject(projectile);
		}

		fPoint projectilePosition;
		projectilePosition.x = position.x;
		projectilePosition.y = position.y - shootHeight;

		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);

		fPoint projectileDirection;
		projectileDirection = (fPoint)mousePos - projectilePosition;
		projectileDirection.Normalize();

		projectile = App->object->AddObjProjectile(projectilePosition, projectileDirection, this);
	}
}