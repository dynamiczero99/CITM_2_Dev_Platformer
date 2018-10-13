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
#include "j1Object.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "ObjPlayer.h"

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
	// Load the first level of the list on first game start -------------------------
	if (firstStart)
	{
		p2List_item<Levels*>* levelData = App->map->data.levels.start;
		App->map->Load(levelData->data->name.GetString());
		firstStart = false;
	}
	// ----------------------------------------------------------
	if (!App->collision->IsEnabled()) { App->collision->Enable(); }
	// TODO, search a workaround to reload player info
	if (!App->object->IsEnabled()) { App->object->Enable(); }

	// TODO, search a less ugly tornaround, maybe in module player?
	// to loads its position on every new map load
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
	// checks for debug input
	DebugInput();
	// ----------------------
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("savegame.xml");

	if(App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		App->SaveGame("savegame.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 6;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 6;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 6;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 6;

	// testing teleporting camera
	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
	{
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
	App->object->Disable();


	return true;
}

void j1Scene::CameraLogic()
{
	uint width, height = 0;
	App->win->GetWindowSize(width, height);

	float x = 0.0f;
	if (App->object->player->flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL)
		x = width * 0.25f * 2.5f;
	else
		x = width * 0.25f * 1.5f; // situates player on the middle of second screen partition(of 4)
	float y = height * 0.33f *2.5f; // 
	
	iPoint offset = { (int)x , (int)y };

	iPoint playerPivotPos;
	playerPivotPos.x = -(int)(App->object->player->position.x * App->win->GetScale()); // center of current player pivot
	playerPivotPos.y = -(int)(App->object->player->position.y * App->win->GetScale());

	float targetX = (playerPivotPos.x + (int)offset.x);
	float targetY = (playerPivotPos.y + (int)offset.y);

	if (!teleport)
	{

		speedx += (targetX - App->render->camera.x) / 20;

		if (App->render->camera.y >= targetY)
			speedy += (targetY - App->render->camera.y) / 3;
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

 void j1Scene::DebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		// Load the first level of the list -------------------------
		p2List_item<Levels*>* levelData = App->map->data.levels.start;
		App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 2.0f);
	}

	// START from the current level
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fade_to_black->FadeToBlack(App->map->data.loadedLevel.GetString(), 2.0f);

	// switch between levels
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		p2List_item<Levels*>* levelData = App->map->data.levels.start;

		// check current level and loads the next, if next is null, load the first one
		while (levelData != NULL)
		{
			p2SString loadedLevel = App->map->data.loadedLevel.GetString();
			p2SString compareLevel = levelData->data->name.GetString();

			if (loadedLevel == compareLevel)
			{
				LOG("coincidence");
				levelData = levelData->next;

				if (levelData == NULL)
				{
					levelData = App->map->data.levels.start;
				}
				break;
			}
			levelData = levelData->next;
		}
		
		App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 2.0f);
	}
}