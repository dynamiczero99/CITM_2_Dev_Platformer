#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Animation.h"

#define GRAVITY 9.8f

j1Player::j1Player() : j1Module()
{
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& player_node)
{
	this->player_node = player_node;
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
	position.x = 0;
	position.y = 0;
	velocity.x = 0;
	velocity.y = 0;
	//TODO: Set them to be 0,0 when they are made, not in here
	currentAnim = idleAnim;
	return true;
}

bool j1Player::Start()
{
	//INFO: We can't load the texture in awake because the render is not initialized yet
	//p2SString path;
	//p2SString image;
	//path.create(player_node.child("path").value());
	//image.create(player_node.child("image").value());
	//characterTex = App->tex->LoadTexture(PATH(path.GetString(), image.GetString()));
	//TODO: Why the code above doesn't work even though it's the same as the map code?
	characterTex = App->tex->LoadTexture("textures/engineer character/engineer-idle.png");
	return true;
}

bool j1Player::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) {
		flip = SDL_FLIP_HORIZONTAL;
		velocity.x -= moveSpeed;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		velocity.x += moveSpeed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		flip = SDL_FLIP_NONE;
		velocity.x += moveSpeed;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		velocity.x -= moveSpeed;
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
		acceleration.y = GRAVITY;
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