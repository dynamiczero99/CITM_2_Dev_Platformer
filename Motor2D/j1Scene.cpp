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
#include "j1Gui.h"
#include "j1Object.h"
#include "j1Collision.h"
#include "j1Window.h"
#include "ObjPlayer.h"
#include "j1Pathfinding.h"
#include "Brofiler/Brofiler.h"
#include "UI_Sprite.h"

#include "j1Particles.h"

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

	cameraHMultiplier = node.child("camera").child("horizontal_multiplier").text().as_float();
	cameraJumpMultiplier = node.child("camera").child("jump_multiplier").text().as_float();
	cameraFallMultiplier = node.child("camera").child("fall_multiplier").text().as_float();

	menu = node.child("camera").child("name").text().as_string();

	uint width, height;
	App->win->GetWindowSize(width, height);
	horizontalScreenDivision = (1.0f / node.child("camera").child("horizontal_screen_divisions").text().as_float()) * width;
	verticalScreenDivision = (1.0f / node.child("camera").child("vertical_screen_division").text().as_float()) * height;

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

	SearchValidCameraPos();

	if (App->map->data.loadedLevel == menu)
	{
		CreateWidgets();
	}
	

	// loads music
	App->audio->PlayMusic(App->map->data.properties.music_name.GetString(), 0.0f);


	return true;
}

bool j1Scene::PreUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
		App->map->showNavLayer = !App->map->showNavLayer;
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("SCENE UPDATE", Profiler::Color::DeepSkyBlue);
	if (!App->render->cameraDebug) {
		CameraLogic(dt);
	}

	// checks for debug input
	DebugInput();
	// ----------------------
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && !App->fade_to_black->IsFading()) {
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

	if (App->map->data.loadedLevel != menu)
	{
		App->gui->DestroyAllUIElements();
	}

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

	iPoint playerPivotPos;
	playerPivotPos.x = -(int)(App->object->player->position.x * (int)App->win->GetScale()); // center of current player pivot
	playerPivotPos.y = -(int)(App->object->player->position.y * (int)App->win->GetScale());

	fPoint target;
	target.x = (playerPivotPos.x + (int)offset.x);
	target.y = (playerPivotPos.y + (int)offset.y);

	cameraPos.x += (target.x - App->render->camera.x) * cameraHMultiplier * dt;
	if (App->render->camera.y >= target.y) {
		cameraPos.y += (target.y - App->render->camera.y) * cameraJumpMultiplier * dt;
	}
	else {
		cameraPos.y += (target.y - App->render->camera.y) * cameraFallMultiplier  *dt;
	}

	App->render->camera.x = cameraPos.x;
	App->render->camera.y = cameraPos.y;
	
}

 void j1Scene::DebugInput()
{
	 if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		 App->particles->AddParticle(App->particles->teleport02, 450, 250, COLLIDER_ENEMY);

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		// Load the first level of the list. Returning to main menu
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

	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		p2List_item<Levels*>* levelData = App->map->data.levels.end;

		// check current level and loads the previous one, if next is null, load the first one
		while (levelData != NULL)
		{
			p2SString loadedLevel = App->map->data.loadedLevel.GetString();
			p2SString compareLevel = levelData->data->name.GetString();

			if (loadedLevel == compareLevel)
			{
				LOG("coincidence");
				levelData = levelData->prev;

				if (levelData == NULL)
				{
					levelData = App->map->data.levels.start;
				}
				break;
			}
			levelData = levelData->prev;
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

 void j1Scene::CreateWidgets()
 {
	//Big buttons rects
	SDL_Rect section = { 9 , 2,128,26 };
	SDL_Rect hover = { 7,57,129,28 };
	SDL_Rect clicked = { 9,30,127,26};
	SDL_Rect disabled = { 0,0,0,0 };
	//Small buttons rects
	SDL_Rect section1 = { 11 , 120, 61, 26 };
	SDL_Rect hover1 = { 9, 172, 63, 28 };
	SDL_Rect clicked1 =  { 11, 148, 61, 26 };
	SDL_Rect disabled1 = { 11, 204, 61, 26};
	 
	// UI_Button* button = App->gui->CreateButton({ 315, 200 }, this, section1, hover1, disabled1, clicked1);


	//PLAY
	UI_Button* button3 = App->gui->CreateButton(ButtonType::PLAY, { 390, 200 }, this, section1, hover1, disabled1, clicked1);
	//CONTINUE
	UI_Button* button4 = App->gui->CreateButton(ButtonType::CONTINUE, { 465, 200 }, this, section1, hover1, disabled1, clicked1);
	//SETTINGS
	UI_Button* button1 = App->gui->CreateButton(ButtonType::SETTINGS, { 390, 240 }, this, section1, hover1, disabled1, clicked1);
	//CREDITS
	UI_Button* button2 = App->gui->CreateButton(ButtonType::CREDITS, { 465, 240 }, this, section1, hover1, disabled1, clicked1);
	
	//This is not hardcoded. Gets values from xml in j1GUi::Awake
	UI_Sprite* title = App->gui->CreateSprite(App->gui->title_pos, this, App->gui->title_Rect);
	title->draggable = true;

	
 }

 bool j1Scene::SearchValidCameraPos()
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

 bool j1Scene::OnEvent(UI_Button* button)
 {
	 bool ret = true;
	 //Lots of else if == switch
	 switch (button->button_type)
	 {
	 case ButtonType::PLAY:
	 {
		 p2List_item<Levels*>* levelData = App->map->data.levels.start;
		 App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 1.0f);
		 break;
	 }
	 case ButtonType::CONTINUE:
	 {
		 //TODO
		 break;
	 }
	 case ButtonType::SETTINGS:
	 {
		 //TODO
		 break;
	 }
	 case ButtonType::CREDITS:
	 {
		 //TODO
		 break;
	 }
	 case ButtonType::EXIT:
	 {
		 ret = false;
		 break;
	 }
	 case ButtonType::WEBPAGE:
	 {
		 ShellExecuteA(NULL, "open", "https://github.com/dynamiczero99/CITM_2_Dev_Platformer", NULL, NULL, SW_SHOWNORMAL);
		 break;
	 }

	 return ret;
	 }
 }
