#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Object.h"
#include <assert.h>
#include "p2Log.h"
#include "Brofiler/Brofiler.h"

j1Collision::j1Collision()
{
	name.create("Collision");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_WALL][COLLIDER_BOX] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_WALL][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_WALL][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_WALL][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_WALL][COLLIDER_GLASS] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_BOX] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH_ZONE] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN_ZONE] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_TRIGGER] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GLASS] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_PLAYER_GOD][COLLIDER_WALL] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_BOX] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_WIN_ZONE] = true;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_TRIGGER] = true;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_GLASS] = false;
	matrix[COLLIDER_PLAYER_GOD][COLLIDER_ENEMY] = false;
	
	matrix[COLLIDER_BOX][COLLIDER_WALL] = true;
	matrix[COLLIDER_BOX][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_BOX][COLLIDER_BOX] = true;
	matrix[COLLIDER_BOX][COLLIDER_PLAYER_SHOT] = true;
	matrix[COLLIDER_BOX][COLLIDER_PLAYER_GOD] = true;
	matrix[COLLIDER_BOX][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_BOX][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_BOX][COLLIDER_TRIGGER] = true;
	matrix[COLLIDER_BOX][COLLIDER_GLASS] = true;
	matrix[COLLIDER_BOX][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_BOX] = true;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_GLASS] = false;
	matrix[COLLIDER_PLAYER_SHOT][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_DEATH_ZONE][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_BOX] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_GLASS] = false;
	matrix[COLLIDER_DEATH_ZONE][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_WIN_ZONE][COLLIDER_WALL] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_BOX] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_GLASS] = false;
	matrix[COLLIDER_WIN_ZONE][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_TRIGGER][COLLIDER_WALL] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_TRIGGER][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_BOX] = true;
	matrix[COLLIDER_TRIGGER][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_GLASS] = false;
	matrix[COLLIDER_TRIGGER][COLLIDER_ENEMY] = false;

	matrix[COLLIDER_GLASS][COLLIDER_WALL] = false;
	matrix[COLLIDER_GLASS][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_GLASS][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_GLASS][COLLIDER_BOX] = false;
	matrix[COLLIDER_GLASS][COLLIDER_PLAYER_SHOT] = false;
	matrix[COLLIDER_GLASS][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_GLASS][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_GLASS][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_GLASS][COLLIDER_GLASS] = false;
	matrix[COLLIDER_GLASS][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER_GOD] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_BOX] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER_SHOT] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH_ZONE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WIN_ZONE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_TRIGGER] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_GLASS] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;


	colliderColor[COLLIDER_WALL] = ColorRGB(0, 0, 255);
	colliderColor[COLLIDER_PLAYER] = ColorRGB(0, 255, 0);
	colliderColor[COLLIDER_PLAYER_SHOT] = ColorRGB(255, 0, 0);
	colliderColor[COLLIDER_BOX] = ColorRGB(255, 255, 0);
	colliderColor[COLLIDER_PLAYER_GOD] = ColorRGB(255, 0, 255);
	colliderColor[COLLIDER_DEATH_ZONE] = ColorRGB(95, 0, 232);
	colliderColor[COLLIDER_WIN_ZONE] = ColorRGB(250, 255, 0);
	colliderColor[COLLIDER_TRIGGER] = ColorRGB(239, 89, 232);
	colliderColor[COLLIDER_GLASS] = ColorRGB(95 ,232 ,0);
	colliderColor[COLLIDER_ENEMY] = ColorRGB(124, 13, 13);

	actualColliders = 0;
}

// Destructor
j1Collision::~j1Collision()
{}

// Called before render is available
bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Collisions Update", Profiler::Color::Purple);
	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (c1->callbackObj != nullptr && matrix[c1->type][c2->type]) {
					c1->callbackObj->OnCollision(c1, c2);
				}
				if (c1 != nullptr && c2 != nullptr && c2->callbackObj != nullptr && matrix[c2->type][c1->type]){
					c2->callbackObj->OnCollision(c2, c1);
				}
			}
		}
	}

	return true;
}

bool j1Collision::PostUpdate() {
	DebugDraw();

	return true;
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
	}

	if (debug == false) {
		return;
	}

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr) {
			continue;
		}
		App->render->DrawQuad(colliders[i]->rect, colliders[i]->color.r, colliders[i]->color.g, colliders[i]->color.b, alpha);
	}

}

// Called before quitting
bool j1Collision::CleanUp()
{
	if (!exitGameLoop) // to avoid general cleanup order when exits the game
	{
		LOG("Freeing all colliders");

		for (uint i = 0; i < MAX_COLLIDERS; ++i)
		{
			if (colliders[i] != nullptr)
			{
				delete colliders[i];
				colliders[i] = nullptr;
			}
		}
	}
	else {
		exitGameLoop = false;
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, GameObject  *callbackObj)
{
	ColorRGB color = colliderColor[type];
	return AddCollider(rect, type, color, callbackObj);	
}


Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, ColorRGB color, GameObject  *callbackObj)
{
	Collider* returnCollider = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			returnCollider = colliders[i] = new Collider(rect, type, callbackObj, color, i);
			actualColliders++;
			return returnCollider;
		}
	}

	LOG("Reached maximum collider capacity, no more colliders can be added.");
	return returnCollider;
}

bool j1Collision::DeleteCollider(Collider * collider) {
	if (collider == nullptr || collider->index == -1 || collider->index >= MAX_COLLIDERS) {
		LOG("Invalid collider index");
		return false;
	}
	if (colliders[collider->index] != nullptr)
	{
		int colIndex = collider->index;
		delete colliders[colIndex];
		colliders[colIndex] = collider = nullptr;
		actualColliders--;
		return true;
	}
	return false;
}

// -----------------------------------------------------

void Collider::SetPos(int x, int y)
{
	rect.x = x;
	rect.y = y;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return !(r.x >= (rect.x + rect.w) || (r.x + r.w) <= rect.x || r.y >= (rect.y + rect.h) || (r.y + r.h) <= rect.y);
}