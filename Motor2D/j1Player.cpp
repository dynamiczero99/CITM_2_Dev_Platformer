#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Animation.h"
#include "p2Defs.h"
#include "math.h"

j1Player::j1Player() : j1Module()
{
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& player_node)
{
	//General values
	position.x = 0;
	position.y = 0;
	velocity.x = 0;
	velocity.y = 0;
	acceleration.x = 0;
	acceleration.y = 0;

	//Values from xml
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
	return true;
}

bool j1Player::PreUpdate()
{
	//TODO: Control the animations from an state machine (Ryu like)

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE) {
		flip = SDL_FLIP_HORIZONTAL;
		if (IsStanding()) {
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
		if (IsStanding()) {
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
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		velocity.y = jumpSpeed;
	}

	return true;
}

bool j1Player::Update(float dt)
{
	//PHYSICS UPDATE
	//- Calculate time since last frame
	deltaTime = SDL_GetTicks() - lastTime;
	deltaTime /= 1000;//1 second is 1000 miliseconds
	lastTime = SDL_GetTicks();

	//- Add gravity
	if (!IsStanding()) {
		acceleration.y = gravity;
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

bool j1Player::IsStanding() {
	//Change to true when it is standing in a collider, not when it's in a certain position
	return (position.y >= 50);
}

//We can consider that 1 tile = 1 meter to make it easier for us to imagine the different values
inline float j1Player::tile_to_pixel(uint pixel) {
	return pixel * tile_size;
}