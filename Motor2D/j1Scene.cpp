#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Player.h"
#include "j1Collision.h"
#include "j1Window.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	App->map->Load("level001.tmx");
	//App->map->Load("iso.tmx");

	if (!App->collision->IsEnabled()) App->collision->Enable();
	
	if (!App->player->IsEnabled()) {
		// TODO, search a workaround to reload player info
		App->player->Enable();
	}
	
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	CameraLogic();
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 6;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 6;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 6;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 6;
	
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		App->fade_to_black->FadeToBlack("level002.tmx", 2.0f);

	// testing teleporting camera
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
		App->player->SetPosition({ 310, 50 });
		teleport = true;
	}

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.columns, App->map->data.rows,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;
	
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	//App->collision->Disable();
	App->player->Disable();


	return true;
}

void j1Scene::CameraLogic()
{
	uint width, height = 0;
	App->win->GetWindowSize(width, height);

	float x = 0.0f;
	if (App->player->flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
		x = width * 0.25f * 2.5f;
	else
		x = width * 0.25f * 1.5f; // situates player on the middle of second screen partition of 4
	float y = height * 0.33f *2.5f; // 
	
	iPoint offset = { (int)x , (int)y };

	iPoint playerPivotPos;
	playerPivotPos.x = -(int)(App->player->GetPlayerPos().x * App->win->GetScale()); // center of current player pivot
	playerPivotPos.y = -(int)(App->player->GetPlayerPos().y * App->win->GetScale());

	float targetX = (playerPivotPos.x + (int)offset.x);
	float targetY = (playerPivotPos.y + (int)offset.y);

	if (!teleport)
	{

		speedx += (targetX - App->render->camera.x) / 20;

		if (App->render->camera.y >= targetY)
			speedy += (targetY - App->render->camera.y) / 5;
		else
			speedy += (targetY - App->render->camera.y) / 50;

		
	}
	else
	{
		// translate the camera to center the player at the middle of screen
		speedx = playerPivotPos.x + width * 0.5f;
		speedy = playerPivotPos.y + height * 0.5f;
		teleport = false;

	}

	App->render->camera.x = speedx;
	App->render->camera.y = speedy;
	
}
