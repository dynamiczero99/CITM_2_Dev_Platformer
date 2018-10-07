#include "j1Player.h"
#include "j1Input.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"


j1Player::j1Player() : j1Module()
{
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& player_node)
{
	this->player_node = player_node;
	characterAnim = new SDL_Rect();
	characterAnim->x = player_node.child("sprite").attribute("x").as_int();
	characterAnim->y = player_node.child("sprite").attribute("y").as_int();
	characterAnim->w = player_node.child("sprite").attribute("w").as_int();
	characterAnim->h = player_node.child("sprite").attribute("h").as_int();
	//TODO: Put this in a for to start getting all the values of the animation
	position.x = 0;
	position.y = 0;
	velocity.x = 0;
	velocity.y = 0;
	//TODO: Set them to be 0,0 when they are made, not in here
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

	return true;
}

bool j1Player::Update(float dt)
{
	//PHYSICS UPDATE
	//TODO: Transfer from acceleration to velocity
	if (!velocity.IsZero()) {
		position.x += velocity.x;
		position.y += velocity.y;
	}
	

	return true;
}

bool j1Player::PostUpdate()
{
	App->render->Blit(characterTex, (int)position.x, (int)position.y, characterAnim, 1.0f, flip);
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