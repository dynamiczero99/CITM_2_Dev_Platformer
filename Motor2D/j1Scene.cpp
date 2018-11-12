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
#include "j1Pathfinding.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& node)
{
	bool ret = true;

	LOG("Loading Scene");

	catchingSpeedHorizontal = node.child("catching_speed_horizontal").text().as_float();
	catchingSpeedJumping = node.child("catching_speed_jumping").text().as_float();
	catchingSpeedFalling = node.child("catching_speed_falling").text().as_float();

	uint width, height;
	App->win->GetWindowSize(width, height);
	horizontalScreenDivision = (1.0f / node.child("screen_divisions_horizontal").text().as_float()) * width;
	verticalScreenDivision = (1.0f / node.child("screen_divisions_vertical").text().as_float()) * height;

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

	// create walkability map
	if (App->map->map_loaded)
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->LoadTexture("maps/calculatedPathTex.png");

	// TODO, search a less ugly tornaround, maybe in module player?
	// to loads its position on every new map load

	searchValidCameraPos();

	// loads music
	App->audio->PlayMusic(App->map->data.properties.music_name.GetString(), 0.0f);

	return true;
}

bool j1Scene::PreUpdate() {
	//// debug pathfing ------------------
	//static iPoint origin;
	//static bool origin_selected = false;

	//int x, y;
	//App->input->GetMousePosition(x, y);
	//iPoint p = App->render->ScreenToWorld(x, y);
	//p = App->map->WorldToMap(p.x, p.y);

	//if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	//{
	//	if (origin_selected == true)
	//	{
	//		App->pathfinding->CreatePath(origin, p);
	//		origin_selected = false;
	//		CopyLastGeneratedPath();
	//	}
	//	else
	//	{
	//		origin = p;
	//		origin_selected = true;
	//	}
	//}
	// // ---------------------------------

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (!App->render->cameraDebug) {
		CameraLogic(dt);
	}

	// checks for debug input
	DebugInput();
	// ----------------------
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		App->LoadGame("savegame.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		App->SaveGame("savegame.xml");
	}

	App->map->Draw();

	//// Debug pathfinding ------------------------------
	//int x, y;
	//App->input->GetMousePosition(x, y);
	//iPoint p = App->render->ScreenToWorld(x, y);
	//p = App->map->WorldToMap(p.x, p.y);
	//p = App->map->MapToWorld(p.x, p.y);

	//App->render->Blit(debug_tex, p.x, p.y);

	//for (uint i = 0; i < last_path.Count(); ++i)
	//{
	//	iPoint pos = App->map->MapToWorld(last_path.At(i)->x, last_path.At(i)->y);
	//	App->render->Blit(debug_tex, pos.x, pos.y);
	//}
	//// ------------------------------------------------

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
	if(App->object->IsEnabled())
		App->object->Disable();


	return true;
}

void j1Scene::CameraLogic(float dt)
{
	//- Calculate camera with and height
	uint width, height = 0u;
	App->win->GetWindowSize(width, height);

	//- Calculate offset
	iPoint offset = {0, 0};
	//-- The screen is horizontally divided into 8 parts (see config.xml)
	if (App->object->player->flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		//-- Place the player on the 5th part
		offset.x = horizontalScreenDivision * 5.0f;
	}
	else {
		//-- Place the player on the 3rd part
		offset.x = horizontalScreenDivision * 3.0f;
	}
	//-- The screen is vertically divided into 6 parts (see config.xml)
	//-- Place the player on the 5th part
	offset.y = verticalScreenDivision * 5.0f;	

	//- Calculate the player's pivot positon
	iPoint playerPivotPos;
	playerPivotPos.x = -(int)(App->object->player->position.x * (int)App->win->GetScale()); // center of current player pivot
	playerPivotPos.y = -(int)(App->object->player->position.y * (int)App->win->GetScale());

	//- Calculate camera target position
	fPoint target;
	target.x = (playerPivotPos.x + (int)offset.x);
	target.y = (playerPivotPos.y + (int)offset.y);

	cameraPos.x += (target.x - App->render->camera.x) * catchingSpeedHorizontal * dt;
	if (App->render->camera.y >= target.y) {
		cameraPos.y += (target.y - App->render->camera.y) * catchingSpeedJumping * dt;
	}
	else {
		cameraPos.y += (target.y - App->render->camera.y) * catchingSpeedFalling  *dt;
	}

	App->render->camera.x = cameraPos.x;
	App->render->camera.y = cameraPos.y;
	
}

 void j1Scene::DebugInput()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		// Load the first level of the list -------------------------
		p2List_item<Levels*>* levelData = App->map->data.levels.start;
		App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 1.0f);
	}

	// START from the current level
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		App->fade_to_black->FadeToBlack(App->map->data.loadedLevel.GetString(), 1.0f);

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
		
		App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 1.0f);
	}
}

 bool j1Scene::Load(pugi::xml_node& loadNode)
 {
	 LOG("Loading scene data from saved game");

	 cameraPos.x = loadNode.child("camera").attribute("x").as_float();
	 cameraPos.y = loadNode.child("camera").attribute("y").as_float();

	 return true;
 }

 bool j1Scene::Save(pugi::xml_node& saveNode) const
 {
	pugi::xml_node camNode =  saveNode.append_child("camera");
	camNode.append_attribute("x") = cameraPos.x;
	camNode.append_attribute("y") = cameraPos.y;
	
	pugi::xml_node currentLevelNode = saveNode.append_child("current_level");
	currentLevelNode.append_attribute("name") = App->map->data.loadedLevel.GetString();
	
	 return true;
 }

 bool j1Scene::searchValidCameraPos()
 {
	 // update offset
	 uint width, height = 0;
	 App->win->GetWindowSize(width, height);

	 float x = width * 0.25f * 1.5f; // situates player on the middle of second screen partition(of 4)
	 float y = height * 0.33f *2.5f; // 

	 iPoint offset = { (int)x , (int)y };
	 // -------------
	 fPoint startPosition;
	 startPosition.x = App->map->playerData.x;
	 startPosition.y = App->map->playerData.y;

	 App->render->camera.x = cameraPos.x = (-startPosition.x * App->win->GetScale()) + offset.x;
	 App->render->camera.y = cameraPos.y = (-startPosition.y * App->win->GetScale()) + offset.y;


	 return true;
 }

 /*iPoint j1Scene::UpdateCameraOffset()
 {

 }*/

 // debug path generation with mouse
 void j1Scene::CopyLastGeneratedPath()
 {
	 const p2DynArray<iPoint>* pathToCopy = App->pathfinding->GetLastPath();

	 last_path.Clear();
	 for (uint i = 0; i < pathToCopy->Count(); ++i)
	 {
		 last_path.PushBack(*pathToCopy->At(i));
	 }
 }