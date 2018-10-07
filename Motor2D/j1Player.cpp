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
	characterTex = App->tex->LoadTexture(player_node.child("path").value());
	//TODO: Put this in a for to start getting all the values of the animation
	characterAnim = new SDL_Rect();
	characterAnim->x = player_node.child("sprite").attribute("x").as_int();
	characterAnim->y = player_node.child("sprite").attribute("y").as_int();
	characterAnim->w = player_node.child("sprite").attribute("w").as_int();
	characterAnim->h = player_node.child("sprite").attribute("h").as_int();
	return true;
}

bool j1Player::Start()
{
	return true;
}

bool j1Player::PreUpdate()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN) {
		velocity.x -= horizontalSpeed;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		velocity.x += horizontalSpeed;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
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
	App->render->Blit(characterTex, (int)position.x, (int)position.y, characterAnim);
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