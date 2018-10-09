#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Animation.h"
#include "p2Defs.h"
#include "j1Collision.h"
#include "math.h"

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

	//Animations from xml
	path = player_node.child("path").text().as_string();
	idleAnim = new Animation();
	for (pugi::xml_node node_iterator = player_node.child("sprite"); node_iterator; node_iterator = node_iterator.next_sibling("sprite")) {
		SDL_Rect frame;
		frame.x = node_iterator.attribute("x").as_int();
		frame.y = node_iterator.attribute("y").as_int();
		frame.w = node_iterator.attribute("w").as_int();
		frame.h = node_iterator.attribute("h").as_int();
		idleAnim->PushBack(frame);
	}
	idleAnim->speed = 0.5f;
	currentAnim = idleAnim;
	return true;
}

bool j1Player::Start()
{
	//INFO: We can't load the texture in awake because the render is not initialized yet
	characterTex = App->tex->LoadTexture(path.GetString());

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
	//TODO: Control the animations from an state machine (Ryu like)

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {
		flip = SDL_FLIP_HORIZONTAL;
		if (isOnPlatform) {
			//currentAnim = idleAnim;
			velocity.x = -moveSpeedGnd;
		}
		else {
			//currentAnim = jumpingAnim;
			velocity.x = -moveSpeedAir;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
		flip = SDL_FLIP_NONE;
		if (isOnPlatform) {
			//currentAnim = idleAnim;
			velocity.x = moveSpeedGnd;
		}
		else {
			//currentAnim = jumpingAnim;
			velocity.x = moveSpeedAir;
		}
	}
	else {
		velocity.x = 0;
	}

	// Check that it is hitting the ground to be able to jump (he could jump on the air if not)
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isOnPlatform) {
		velocity.y = jumpSpeed;
		isOnPlatform = false;
		//TODO: Is standing should also turn false when the character leaves the platform
	}

	return true;
}

bool j1Player::Update(float dt)
{
	//PHYSICS UPDATE
	//- Calculate time since last frame
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
	//- Add gravity
	if (!isOnPlatform) {
		acceleration.y = gravity;
		checkFoot = false;
	}
	else {
		//Stop the player falling when it reaches a ground collider
		acceleration.y = 0.0f;
		if (velocity.y > 0) {
			velocity.y = 0.0f;
		}
	}

	//- Move the player
	velocity = velocity + acceleration * deltaTime;
	position = position + velocity * deltaTime;
	playerCol->SetPos(position.x, position.y);
	footCol->SetPos(position.x, position.y + playerCol->rect.h);

	isOnPlatform = false;//This value is going to be changed to true if

	return true;
}

bool j1Player::PostUpdate()
{
	App->render->Blit(characterTex, (int)position.x, (int)position.y, &currentAnim->GetCurrentFrame(), 1.0f, flip);
	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnloadTexture(characterTex);
	delete(idleAnim);
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
			dist[(uint)dir::up] = (c2->rect.y + c2->rect.h) - (c1->rect.y);
			dist[(uint)dir::down] = (c1->rect.y + c1->rect.h) - (c2->rect.y);
			dist[(uint)dir::left] = (c2->rect.y + c2->rect.w) - (c1->rect.x);
			dist[(uint)dir::right] = (c1->rect.x + c1->rect.w) - (c2->rect.y);
			dir nearestDir = (dir)0u;
			for (uint i = 0; i < (uint)dir::max; ++i) {
				if (direction[i]) {
					if (dist[i] < dist[(uint)nearestDir]) {
						nearestDir = (dir)i;
					}
				}
			}
			//3. Move it to that point
			//+1 offset so that the next frame they don't collide again
			switch (nearestDir) {
			case dir::down:
				position.y = c2->rect.y - c1->rect.h - 1;
				c1->SetPos(position.x, position.y);
				velocity.y = 0;
				acceleration.y = 0;
				checkFoot = true;
				isOnPlatform = true;
				break;
			case dir::up:
				position.y = c2->rect.y + c2->rect.h + 1;
				c1->SetPos(position.x, position.y);
				velocity.y = 0;
				acceleration.y = 0;
				break;
			case dir::left:
				position.x = c2->rect.x + c2->rect.w + 1;
				c1->SetPos(position.x, position.y);
				velocity.x = 0;
				acceleration.y = 0;
				break;
			case dir::right:
				position.x = c2->rect.x - c1->rect.w - 1;
				c1->SetPos(position.x, position.y);
				velocity.x = 0;
				acceleration.y = 0;
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