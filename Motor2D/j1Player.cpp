#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Animation.h"
#include "p2Defs.h"
#include "j1Collision.h"
#include "math.h"
#include "p2Log.h"

j1Player::j1Player() : j1Module()
{
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& player_node)
{
	//Values from xml
	SDL_Rect colliderRect;
	colliderRect.w = player_node.child("collider_width").text().as_int();
	colliderRect.h = player_node.child("collider_height").text().as_int();
	playerCol = App->collision->AddCollider(colliderRect, COLLIDER_PLAYER, this);
	SDL_Rect foot_collider_rect;
	foot_collider_rect.w = 10;
	foot_collider_rect.h = 5;
	footCol = App->collision->AddCollider(foot_collider_rect, COLLIDER_PLAYER, this);
	tile_size = player_node.child("tile_size").text().as_uint();
	gravity = tile_to_pixel(player_node.child("gravity").text().as_float());
	moveSpeedGnd = tile_to_pixel(player_node.child("move_speed_ground").text().as_float());
	moveSpeedAir = tile_to_pixel(player_node.child("move_speed_air").text().as_float());
	//- This formula traduces gives us the speed necessary to reach a certain height
	//- It is calculated using the conservation of mechanic energy
	jumpSpeed = -sqrtf(gravity * tile_to_pixel(player_node.child("jump_height").text().as_float()) * 2.0f);

	//Animation
	anim_tile_width = player_node.child("animation").attribute("tile_width").as_uint();
	anim_tile_height = player_node.child("animation").attribute("tile_height").as_uint();

	idle_path = player_node.child("animation").child("idle_image").text().as_string();
	run_path = player_node.child("animation").child("run_image").text().as_string();
	jump_path = player_node.child("animation").child("jump_image").text().as_string();

	LoadAnimation(player_node.child("animation").child("idle_animation").child("sprite"), idleAnim);
	idleAnim.speed = player_node.child("animation").child("idle_animation").attribute("speed").as_float();
	LoadAnimation(player_node.child("animation").child("run_animation").child("sprite"), runAnim);
	runAnim.speed = player_node.child("animation").child("run_animation").attribute("speed").as_float();
	LoadAnimation(player_node.child("animation").child("jump_animation").child("sprite"), jumpAnim);
	jumpAnim.speed = player_node.child("animation").child("jump_animation").attribute("speed").as_float();
	LoadAnimation(player_node.child("animation").child("fall_animation").child("sprite"), fallAnim);
	fallAnim.speed = player_node.child("animation").child("fall_animation").attribute("speed").as_float();
	currAnim = &idleAnim;
	return true;
}

bool j1Player::Start()
{
	//INFO: We can't load the texture in awake because the render is not initialized yet
	idleTex = App->tex->LoadTexture(idle_path.GetString());
	runTex = App->tex->LoadTexture(run_path.GetString());
	jumpTex = App->tex->LoadTexture(jump_path.GetString());
	currTex = idleTex;

	//General values
	position.x = 80;
	position.y = 50;
	velocity.x = 0;
	velocity.y = 0;
	acceleration.x = 0;
	acceleration.y = 0;

	return true;
}

bool j1Player::PreUpdate()
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

	// Check that it is hitting the ground to be able to jump (he could jump on the air if not)
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isOnPlatform) {
		velocity.y = jumpSpeed;
		isOnPlatform = false;
		checkFoot = false;
	}

	return true;
}

bool j1Player::Update(float dt)
{
	//PHYSICS UPDATE
	CalculateDeltaTime();
	MovePlayer();
	MoveProjectile();
	return true;
}

void j1Player::MoveProjectile()
{
	//if (App->input->GetMouseButton(1) == KEY_DOWN) {
	//	iPoint mousePos;
	//	App->input->GetMousePosition(mousePos.x, mousePos.y);
	//	projectilePos = (iPoint)mousePos;
	//	projectileVel.x = mousePos.x - (int)position.x;
	//	projectileVel.y = mousePos.y - (int)position.y;
	//	projectileVel = projectileVel.Normalize() * projectileSpeed;
	//}
	//Move projectile
	projectilePos = projectilePos + projectileVel;
	//App->render->Blit(idleTex, mousePos.x, mousePos.y, &idleAnim.GetCurrentFrame());
}

void j1Player::MovePlayer()
{
	//Add gravity
	if (!isOnPlatform) {
		acceleration.y = gravity;
		checkFoot = false;
	}

	//- Move the player
	velocity = velocity + acceleration * deltaTime;
	position = position + velocity * deltaTime;
	playerCol->SetPos(position.x - playerCol->rect.w / 2, position.y - playerCol->rect.h);
	footCol->SetPos(position.x - footCol->rect.w / 2, position.y);

	isOnPlatform = false;//This value is going to be changed to true if the character exits the platform
}

void j1Player::CalculateDeltaTime()
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

bool j1Player::PostUpdate()
{
	//Once the movement and the physical resolution has happened, determine the animations it must play

	if (velocity.x > 0) {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}
	else if (velocity.x < 0) {
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}

	if (isOnPlatform) {
		if(velocity.x != 0) {
			currTex = runTex;
			currAnim = &runAnim;
		}
		else {
			currTex = idleTex;
			currAnim = &idleAnim;
		}
	}
	else {
		currTex = jumpTex;
		if(velocity.y <= 0){
			currAnim = &jumpAnim;
		}
		else {
			currAnim = &fallAnim;
		}
	}

	App->render->Blit(currTex, (int)position.x - anim_tile_width / 2, (int)position.y - anim_tile_height, &currAnim->GetCurrentFrame(), 1.0f, flip);
	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnloadTexture(idleTex);
	return true;
}

bool j1Player::Load(pugi::xml_node&)
{
	return true;
}

bool j1Player::Save(pugi::xml_node&) const
{
	return true;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	if (c1 == playerCol) {
		switch (c2->type) {
		case COLLIDER_WALL:
			//1. Check which direction it was going to
			bool direction[(uint)dir::max];
			direction[(uint)dir::up]	= velocity.y < 0;
			direction[(uint)dir::down]	= velocity.y > 0;
			direction[(uint)dir::left]	= velocity.x < 0;
			direction[(uint)dir::right]	= velocity.x > 0;
			//2. Check which point (opposite to those directions) is the nearest
			//If it has entered in the direction "up" the distance is from the character to the bottom of the other collider
			uint dist[(uint)dir::max];
			dist[(uint)dir::up] = (c2->rect.y + c2->rect.h) - (position.y - playerCol->rect.h);
			dist[(uint)dir::down] = position.y - c2->rect.y;
			dist[(uint)dir::left] = (c2->rect.x + c2->rect.w) - (position.x - playerCol->rect.w/2);
			dist[(uint)dir::right] = (position.x + playerCol->rect.w / 2) - c2->rect.x;
			dir nearestDir = (dir)0u;
			for (uint i = 0; i < (uint)dir::max; ++i) {
				if (direction[i]) {
					if (dist[i] < dist[(uint)nearestDir]) {
						nearestDir = (dir)i;
					}
				}
			}
			//3. Move it to that point
			switch (nearestDir) {
			case dir::down:
				position.y = c2->rect.y;
				playerCol->SetPos(position.x - playerCol->rect.w / 2, position.y - playerCol->rect.h);
				footCol->SetPos(position.x - footCol->rect.w / 2, position.y);
				velocity.y = 0;
				acceleration.y = 0;
				checkFoot = true;
				isOnPlatform = true;
				break;
			case dir::up:
				position.y = c2->rect.y + c2->rect.h + playerCol->rect.h;
				playerCol->SetPos(position.x - playerCol->rect.w / 2, position.y - playerCol->rect.h);
				footCol->SetPos(position.x - footCol->rect.w / 2, position.y);
				velocity.y = 0;
				acceleration.y = 0;
				break;
			case dir::left:
				position.x = c2->rect.x + c2->rect.w + playerCol->rect.w / 2;
				playerCol->SetPos(position.x - playerCol->rect.w / 2, position.y - playerCol->rect.h);
				footCol->SetPos(position.x - footCol->rect.w / 2, position.y);
				velocity.x = 0;
				break;
			case dir::right:
				position.x = c2->rect.x - playerCol->rect.w / 2;
				playerCol->SetPos(position.x - playerCol->rect.w / 2, position.y - playerCol->rect.h);
				footCol->SetPos(position.x - footCol->rect.w / 2, position.y);
				velocity.x = 0;
				break;
			}
			break;
		}
	}

	//Only starts checking if it falls when it snaps to the platform
	//Otherwise it would stop adding gravity the moment the foot collider touched the floor, making the character be way above what it should be
	if (c1 == footCol && checkFoot) {
		isOnPlatform = true;
	}
}

//We can consider that 1 tile = 1 meter to make it easier for us to imagine the different values
inline float j1Player::tile_to_pixel(uint pixel) {
	return pixel * tile_size;
}

bool j1Player::LoadAnimation(pugi::xml_node &node, Animation &anim) {
	for (; node; node = node.next_sibling("sprite")) {
		SDL_Rect frame;
		frame.x = node.attribute("x").as_int();
		frame.y = node.attribute("y").as_int();
		frame.w = node.attribute("w").as_int();
		frame.h = node.attribute("h").as_int();
		anim.PushBack(frame);
	}
	return true;
}