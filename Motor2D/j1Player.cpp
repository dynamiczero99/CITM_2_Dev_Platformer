#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "p2Animation.h"

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
		velocity.x -= horizontalSpeed;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		velocity.x += horizontalSpeed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		flip = SDL_FLIP_NONE;
		velocity.x += horizontalSpeed;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		velocity.x -= horizontalSpeed;
	}

	//&& Check that it is hitting the ground to be able to jump (he could jump on the air if not)
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		fPoint force;
		force.x = 0;
		force.y = -2.0f;
		AddForce(force);
	}

	return true;
}

bool j1Player::Update(float dt)
{
	//PHYSICS UPDATE
	//1- Add external forces
	//Gravity
	//Provisional value
	acceleration.y += 0.25f;

	//2- Checks if it has hit a wall
	//Provisional, it only detects if it has reached a certain position in the y axis (we don't have collision detection yet)
	if(position.y >= 50 && acceleration.y > 0)
	{
		acceleration.y = 0;
		velocity.y = 0;
	}

	if (!acceleration.IsZero()) {
		velocity += acceleration;
	}

	if (!velocity.IsZero()) {
		position += velocity;
	}

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

void j1Player::AddForce(fPoint force) {
	acceleration += force;
	//We don't have mass into consideration
}