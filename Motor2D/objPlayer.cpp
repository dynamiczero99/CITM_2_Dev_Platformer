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
#include "j1Window.h"
#include "ObjBox.h"
#include "j1Audio.h"

ObjPlayer::ObjPlayer(pugi::xml_node & playerNode, fPoint position, int index) : Gameobject(position, index) {

	if (playerNode.empty())
		LOG("empty node");

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
	maxFallVelocity = playerNode.child("maximum_fall_velocity").text().as_float();
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

	// sound effects
	teleport = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "teleport").attribute("value").as_string());
	jump = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "jump").attribute("value").as_string());
	shoot = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "shoot").attribute("value").as_string());
	//die = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "die").attribute("value").as_string());


}

bool ObjPlayer::OnDestroy() {
	App->collision->DeleteCollider(playerCol);
	playerCol = nullptr;
	App->collision->DeleteCollider(feetCol);
	feetCol = nullptr;

	// free the sfx
	App->audio->UnloadSFX(); // unload all list of sfx loaded

	return true;
}

bool ObjPlayer::PreUpdate() {
	ToggleGodMode();
	if (!godMode) {
		StandardControls();
	}
	else {
		GodControls();
	}
	return true;
}

void ObjPlayer::GodControls()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {
		velocity.x = -moveSpeedGnd;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
		velocity.x = moveSpeedGnd;
	}
	else {
		velocity.x = 0;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == KEY_IDLE) {
		velocity.y = moveSpeedGnd;
	}
	else if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_IDLE) {
		velocity.y = -moveSpeedGnd;
	}
	else {
		velocity.y = 0;
	}
}

void ObjPlayer::StandardControls()
{
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
		// play sfx
		App->audio->PlayFx(jump);
	}
}

bool ObjPlayer::Update() {
	if (!godMode) {
		StandardMovement();
	}
	else {
		GodMovement();
	}
	ShootProjectile();
	return true;
}

void ObjPlayer::ToggleGodMode()
{
	acceleration = fPoint(0.0f, 0.0f);
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (godMode) {
			playerCol->type = COLLIDER_TYPE::COLLIDER_PLAYER;
			godMode = false;
		}
		else {
			playerCol->type = COLLIDER_TYPE::COLLIDER_PLAYER_GOD;
			godMode = true;
		}
	}
}

bool ObjPlayer::PostUpdate() {
	SwapPosition();

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
	if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_BOX) {
		//1. Check which direction it was going to
		bool direction[(uint)dir::max];
		direction[(uint)dir::up] = velocity.y < 0;
		direction[(uint)dir::down] = velocity.y > 0;
		direction[(uint)dir::left] = velocity.x < 0;
		direction[(uint)dir::right] = velocity.x > 0;

		//2. Check which point (opposite to those directions) is the nearest
		//Ex.: If it has entered in the direction "up" the distance is from the character to the bottom of the other collider
		uint dist[(uint)dir::max];
		dist[(uint)dir::up] = c2->rect.GetBottom() - playerCol->rect.GetTop();
		dist[(uint)dir::down] = playerCol->rect.GetBottom()- c2->rect.GetTop();
		dist[(uint)dir::left] = c2->rect.GetRight() - playerCol->rect.GetLeft();
		dist[(uint)dir::right] = playerCol->rect.GetRight() - c2->rect.GetLeft();
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

		//2.5 If the player isn't moving (which is sometimes the case when he swaps) we also need to make it exit from the nearest point
		if (nearestDir == -1) {
			for (int i = 0; i < (int)dir::max; ++i) {
				if (nearestDir == -1) {
					nearestDir = i;
				}
				else if (dist[i] < dist[nearestDir]) {
					nearestDir = i;
				}
			}
		}

		//3. Move it to that point
		//INFO: Keep in mind that the player uses a pivot::bottom-middle
		switch (nearestDir) {
		case (int)dir::down:
			position.y = c2->rect.GetTop();
			velocity.y = 0;
			acceleration.y = 0;
			checkFall = true;
			isOnPlatform = true;
			break;
		case (int)dir::up:
			position.y = c2->rect.GetBottom() + playerCol->rect.h;
			velocity.y = 0;
			break;
		case (int)dir::left:
			position.x = c2->rect.GetRight() + playerCol->rect.w / 2;
			velocity.x = 0;
			break;
		case (int)dir::right:
			position.x = c2->rect.GetLeft() - playerCol->rect.w / 2;
			velocity.x = 0;
			break;
		}
		iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
		playerCol->SetPos(colPos.x, colPos.y);
		feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);
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

//We can use this variable to make it easier for us to understand the different distance the player can move or jump while building levels in the tiled editor
inline float ObjPlayer::tile_to_pixel(uint pixel) {
	return pixel * tileSize;
}

void ObjPlayer::StandardMovement()
{
	if (isOnPlatform) {
		acceleration.y = 0;
	}
	else {
		acceleration.y = gravity;
		checkFall = false;
	}

	velocity = velocity + acceleration * App->GetDeltaTime();
	LimitFallVelocity();
	position = position + velocity * App->GetDeltaTime();
	iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
	playerCol->SetPos(colPos.x, colPos.y);
	feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);

	// - If this value remains false after checking the collision we'll consider the player has fallen from the platform
	isOnPlatform = false;
}

void ObjPlayer::GodMovement() {
	velocity = velocity + acceleration * App->GetDeltaTime();
	position = position + velocity * App->GetDeltaTime();
	iPoint colPos = GetPosFromPivot(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
	playerCol->SetPos(colPos.x, colPos.y);
	feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);
}

void ObjPlayer::LimitFallVelocity() {
	if (velocity.y > maxFallVelocity) {
		velocity.y = maxFallVelocity;
	}
}

void ObjPlayer::ShootProjectile()
{
	if (App->input->GetMouseButton(1) == KEY_DOWN) {

		if (projectile != nullptr) {
			App->object->DeleteObject(projectile);
			projectile = nullptr;
		}

		if (swapObject != nullptr) {
			swapObject->MarkObject(false);
			swapObject = nullptr;
		}

		fPoint projectilePosition;
		projectilePosition.x = position.x;
		projectilePosition.y = position.y - shootHeight;

		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);
		//INFO: Get the world position, not the screen position

		// workaround to get correct values ------
		fPoint swap;
		if(App->render->camera.x < 0)
			swap.x = -(App->render->camera.x) / App->win->GetScale();
		else
		{
			swap.x = App->render->camera.x / App->win->GetScale();
			swap.x = -swap.x;
		}
		if (App->render->camera.y > 0)
		{
			swap.y = App->render->camera.y / App->win->GetScale();
			swap.y = -swap.y;
		}
		else
		{
			swap.y = -App->render->camera.y / App->win->GetScale();
		}
	
		mousePos.x += swap.x;
		mousePos.y += swap.y;
		// ----------------------------------------

		fPoint projectileDirection;
		projectileDirection = (fPoint)mousePos - projectilePosition;
		projectileDirection.Normalize();

		projectile = App->object->AddObjProjectile(projectilePosition, projectileDirection, this);

		// play sfx
		App->audio->PlayFx(shoot);
	}
}

void ObjPlayer::SwapPosition() {
	if (App->input->GetMouseButton(3) == KEY_DOWN) {
		if (swapObject != nullptr) {
			fPoint auxPos = position;
			position = swapObject->position;
			swapObject->position = auxPos;
			swapObject->MarkObject(false);
			swapObject = nullptr;
			// play sfx
			App->audio->PlayFx(teleport);
		}
	}
}

bool ObjPlayer::Load(pugi::xml_node& loadNode)
{

	position.x = loadNode.child("Player").attribute("x").as_float();
	position.y = loadNode.child("Player").attribute("y").as_float();
	velocity.x = loadNode.child("Player").attribute("velocity_x").as_float();
	velocity.y = loadNode.child("Player").attribute("velocity_y").as_float();
	
	p2SString flipDir = loadNode.child("Player").attribute("flip_direction").as_string();
	// check facing direction
	if (flipDir == "true")
	{
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}
	else {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}

	return true;
}

void ObjPlayer::SetSwapObject(Gameobject * markedObject) {
	App->object->DeleteObject(projectile);
	projectile = nullptr;
	swapObject = markedObject;
}

bool ObjPlayer::Save(pugi::xml_node& saveNode) const
{
	pugi::xml_node player = saveNode.append_child("Player");

	player.append_attribute("x") = position.x;
	player.append_attribute("y") = position.y;
	player.append_attribute("velocity_x") = velocity.x;
	player.append_attribute("velocity_y") = velocity.y;
	if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
		player.append_attribute("flip_direction") = "true";
	else
		player.append_attribute("flip_direction") = "false";

	return true;
}