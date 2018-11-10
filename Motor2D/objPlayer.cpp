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
#include "j1Map.h"
#include "j1FadeToBlack.h"

ObjPlayer::ObjPlayer(pugi::xml_node & playerNode, fPoint position, int index) : GameObject(position, index) {

	velocity = fPoint(0.0F, 0.0F);
	acceleration = fPoint(0.0F, 0.0F);

	if (playerNode.empty())
		LOG("empty node");

	SDL_Rect colliderRect;
	colliderRect.w = playerNode.child("collider_width").text().as_int();
	colliderRect.h = playerNode.child("collider_height").text().as_int();
	playerCol = App->collision->AddCollider(colliderRect, COLLIDER_PLAYER, this);
	SDL_Rect feetColRect;
	feetColRect.w = playerNode.child("feet_collider_width").text().as_int();
	feetColRect.h = playerNode.child("feet_collider_height").text().as_int();
	feetCol = App->collision->AddCollider(feetColRect, COLLIDER_PLAYER, ColorRGB(255,255,255), this);
	tileSize = playerNode.child("tile_size").text().as_uint();
	gravity = tile_to_pixel(playerNode.child("gravity").text().as_float());
	moveSpeedGnd = tile_to_pixel(playerNode.child("move_speed_ground").text().as_float());
	moveSpeedAir = tile_to_pixel(playerNode.child("move_speed_air").text().as_float());
	//- This formula traduces gives us the speed necessary to reach a certain height
	//- It is calculated using the conservation of mechanic energy
	jumpSpeed = -sqrtf(gravity * tile_to_pixel(playerNode.child("jump_height").text().as_float()) * 2.0F);
	maxFallVelocity = playerNode.child("maximum_fall_velocity").text().as_float();
	shootHeight = playerNode.child("shoot_height").text().as_uint();

	//Animation
	animTileWidth = playerNode.child("animation").attribute("tile_width").as_uint();
	animTileHeight = playerNode.child("animation").attribute("tile_height").as_uint();

	currTex = App->object->playerIdleTex;

	LoadAnimation(playerNode.child("animation").child("idle_animation"), idleAnim);
	idleAnim.speed = playerNode.child("animation").child("idle_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("run_animation"), runAnim);
	runAnim.speed = playerNode.child("animation").child("run_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("jump_animation"), jumpAnim);
	jumpAnim.speed = playerNode.child("animation").child("jump_animation").attribute("speed").as_float();
	LoadAnimation(playerNode.child("animation").child("fall_animation"), fallAnim);
	fallAnim.speed = playerNode.child("animation").child("fall_animation").attribute("speed").as_float();
	currAnim = &idleAnim;

	// sound effects
	teleport = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "teleport").attribute("value").as_string());
	jump = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "jump").attribute("value").as_string());
	shoot = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "shoot").attribute("value").as_string());
	death = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "death").attribute("value").as_string());
	win = App->audio->LoadFx(playerNode.find_child_by_attribute("name", "win").attribute("value").as_string());


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
		checkFallPlatform = false;
		App->audio->PlayFx(jump);
	}
}

bool ObjPlayer::Update(float dt) {
	if (!godMode) {
		StandardMovement(dt);
	}
	else {
		GodMovement(dt);
	}
	ShootProjectile();
	return true;
}

void ObjPlayer::ToggleGodMode()
{
	acceleration = fPoint(0.0F, 0.0F);
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
		currTex = App->object->playerJumpTex;
		if (velocity.y <= 0) {
			currAnim = &jumpAnim;
		}
		else {
			currAnim = &fallAnim;
		}
	}

	iPoint blitPos = GetRectPos(pivot::bottom_middle, (int)position.x, (int)position.y, animTileWidth, animTileHeight);
	App->render->Blit(currTex, blitPos.x, blitPos.y, &currAnim->GetCurrentFrame(), 1.0F, flip);
	return true;
}

void ObjPlayer::OnCollision(Collider * c1, Collider * c2) {
	if (c1 == playerCol) {
		if (c2->type == COLLIDER_WALL || c2->type == COLLIDER_BOX) {
			SolveCollision(c2);
		}
		else if (c2->type == COLLIDER_DEATH_ZONE) {
			CollideDeathZone();
		}
		else if (c2->type == COLLIDER_WIN_ZONE) {
			CollideWinZone();
		}
	}
	else if (c1 == feetCol) {
		OnCollisionFeet(c2);
	}
}

//Returns the direction that has the smallest distance inside the other collider
GameObject::dir ObjPlayer::GetSmallestDir(Collider * c2) {
	GameObject::dir smallestDir = dir::invalid;

	int dist[(uint)dir::max];
	dist[(uint)dir::invalid] = INT_MAX;
	dist[(uint)dir::up] = LimitDistance(c2->rect.GetBottom() - playerCol->rect.GetTop());
	dist[(uint)dir::down] = LimitDistance(playerCol->rect.GetBottom() - c2->rect.GetTop());
	dist[(uint)dir::left] = LimitDistance(c2->rect.GetRight() - playerCol->rect.GetLeft());
	dist[(uint)dir::right] = LimitDistance(playerCol->rect.GetRight() - c2->rect.GetLeft());

	for (int i = 1; i < (uint)dir::max; ++i) {
		if (dist[i] < dist[(uint)smallestDir]) {
			smallestDir = (GameObject::dir)i;
		}
	}

	return smallestDir;
}

//If it gets a negative distance, it means that the player is not colliding with the rectangle in that side
int ObjPlayer::LimitDistance(int distance) {
	if (distance < 0) {
		return INT_MAX;
	}
	return distance;
}

//Returns the direction that the player is moving to and has the smallest distance inside the other collider
//Returns -1 if the players isn't moving to any direction
GameObject::dir ObjPlayer::GetSmallestDirFiltered(Collider * c2) {

	GameObject::dir smallestDir = dir::invalid;

	bool direction[(uint)dir::max];
	direction[(uint)dir::invalid] = true;
	direction[(uint)dir::up] = velocity.y < 0;
	direction[(uint)dir::down] = velocity.y > 0;
	direction[(uint)dir::left] = velocity.x < 0;
	direction[(uint)dir::right] = velocity.x > 0;

	int dist[(uint)dir::max];
	dist[(uint)dir::invalid] = INT_MAX;
	dist[(uint)dir::up] = LimitDistance(c2->rect.GetBottom() - playerCol->rect.GetTop());
	dist[(uint)dir::down] = LimitDistance(playerCol->rect.GetBottom() - c2->rect.GetTop());
	dist[(uint)dir::left] = LimitDistance(c2->rect.GetRight() - playerCol->rect.GetLeft());
	dist[(uint)dir::right] = LimitDistance(playerCol->rect.GetRight() - c2->rect.GetLeft());

	for (int i = 1; i < (uint)dir::max; ++i) {
		if (direction[i] && dist[i] < dist[(uint)smallestDir]) {
			smallestDir = (GameObject::dir)i;
		}
	}

	return smallestDir;
}

void ObjPlayer::SolveCollision(Collider * c2) {
	GameObject::dir smallestDir;
	if (!velocity.IsZero()) {
		//Exit the collision from the smallest distance the player is moving
		smallestDir = GetSmallestDirFiltered(c2);
	}
	else {
		//In the exceptional case that the player is colliding but wasn't moving, simply exit the collision from the smallest distance
		smallestDir = GetSmallestDir(c2);
	}

	//INFO: Keep in mind that the player uses a pivot::bottom-middle
	switch (smallestDir) {
	case dir::down:
		position.y = (float)c2->rect.GetTop();
		velocity.y = 0;
		acceleration.y = 0;
		isOnPlatform = true;
		checkFallPlatform = true;
		break;
	case dir::up:
		position.y = (float)(c2->rect.GetBottom() + playerCol->rect.h);
		velocity.y = 0;
		break;
	case dir::left:
		position.x = (float)(c2->rect.GetRight() + playerCol->rect.w / 2);
		velocity.x = 0;
		break;
	case dir::right:
		position.x = (float)(c2->rect.GetLeft() - playerCol->rect.w / 2);
		velocity.x = 0;
		break;
	default:
		LOG("Error getting the direction the player must exit on the collsion.");
		break;
	}
	iPoint colPos = GetRectPos(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
	playerCol->SetPos(colPos.x, colPos.y);
	feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);
}

void ObjPlayer::CollideDeathZone() {
	App->audio->PlayFx(death);
	App->fade_to_black->FadeToBlack(App->map->data.loadedLevel.GetString(), 2.0f);
}

void ObjPlayer::CollideWinZone() {
	p2List_item<Levels*>* item = App->map->data.levels.start;

	while (item != NULL)
	{
		if (item->data->name == App->map->data.loadedLevel)
		{
			item = item->next;
			if (item == NULL)
			{
				item = App->map->data.levels.start;
			}
			break;
		}
		item = item->next;
	}

	// play win round sfx
	//App->audio->PlayFx(win);
	App->fade_to_black->FadeToBlack(item->data->name.GetString(), 2.0f);
}

void ObjPlayer::OnCollisionFeet(Collider * c2)
{
	//Only starts checking if it falls when it snaps to the platform
	//Otherwise it would stop adding gravity the moment the foot collider touched the floor, making the character be way above what it should be
	if (checkFallPlatform) {
		isOnPlatform = true;
	}
}

//We can use this variable to make it easier for us to understand the different distance the player can move or jump while building levels in the tiled editor
inline float ObjPlayer::tile_to_pixel(uint pixel) {
	return pixel * tileSize;
}

void ObjPlayer::StandardMovement(float dt)
{
	//Evaluate the result we got from last frame's OnCollision()
	if (!isOnPlatform) {
		acceleration.y = gravity;
		checkFallPlatform = false;
	}

	velocity = velocity + acceleration * dt;
	LimitFallVelocity();
	position = position + velocity * dt;
	iPoint colPos = GetRectPos(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
	playerCol->SetPos(colPos.x, colPos.y);
	feetCol->SetPos(position.x - feetCol->rect.w / 2, position.y);

	//LOG("Player position: %f, %f", position.x, position.y);

	// - If this value remains false after checking the collision we'll consider the player has fallen from the platform
	isOnPlatform = false;
}

void ObjPlayer::GodMovement(float dt) {
	velocity = velocity + acceleration * dt;
	position = position + velocity * dt;
	iPoint colPos = GetRectPos(pivot::bottom_middle, (int)position.x, (int)position.y, playerCol->rect.w, playerCol->rect.h);
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

		fPoint sourcePosProjectile;
		sourcePosProjectile.x = position.x;
		sourcePosProjectile.y = position.y - shootHeight;

		//INFO: Get the world position, not the screen position
		iPoint mousePos;
		App->input->GetMousePosition(mousePos.x, mousePos.y);

		iPoint cameraPos;
		cameraPos.x = -App->render->camera.x / (int)App->win->GetScale();
		cameraPos.y = -App->render->camera.y / (int)App->win->GetScale();

		iPoint targetPosProjectile;
		targetPosProjectile = cameraPos + mousePos;

		fPoint projectileDirection;
		projectileDirection = (fPoint)targetPosProjectile - sourcePosProjectile;
		projectileDirection.Normalize();

		projectile = App->object->AddObjProjectile(sourcePosProjectile, projectileDirection, this);

		//Play sfx
		App->audio->PlayFx(shoot);
	}
}

void ObjPlayer::SwapPosition() {
	if (App->input->GetMouseButton(3) == KEY_DOWN && swapObject != nullptr) {
		fPoint auxPos = position;
		position = swapObject->position;
		swapObject->position = auxPos;
		swapObject->MarkObject(false);
		swapObject = nullptr;
		App->audio->PlayFx(teleport);
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

	// player checks if we have alive projectile too
	// projectile
	for (pugi::xml_node projectiles = loadNode.child("Projectile"); projectiles; projectiles = projectiles.next_sibling("Projectile"))
	{
		LOG("node found");
		fPoint direction;
		direction.x = projectiles.attribute("velocity_x").as_float();
		direction.y = projectiles.attribute("velocity_y").as_float();
		direction.Normalize();

		projectile = App->object->AddObjProjectile({ projectiles.attribute("x").as_float(), projectiles.attribute("y").as_float() }, direction, this);
	}
	// player checks if we have a marked box on savegame too
	if (App->object->object_box_markedOnLoad != nullptr)
	{
		swapObject = App->object->object_box_markedOnLoad;
		// and "release" the previous pointer
		App->object->object_box_markedOnLoad = nullptr;
	}

	return true;
}

void ObjPlayer::SetSwapObject(GameObject * markedObject) {
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

void ObjPlayer::DestroyProjectile() {
	App->object->DeleteObject(projectile);
	projectile = nullptr;
}