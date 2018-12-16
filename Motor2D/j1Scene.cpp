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
	//Creates widgets only in main menu

	// Load the first level of the list on first game start -------------------------

	if (firstStart)
	{
		p2List_item<Levels*>* levelData = App->map->data.levels.start;
		App->map->Load(levelData->data->name.GetString());
		CreateMainMenu();
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
		enable_continue = true;
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

	//if (App->map->data.loadedLevel != menu && !escape_menu && !in_mainmenu)
	//{
	//	App->gui->DestroyAllUIElements();
	//}

	if (musicLabel != nullptr)
	{
		p2SString temp;
		temp.create("Music: %i", App->audio->musicVolume);
		musicLabel->ChangeContent(temp.GetString());
	}

	if (sfxLabel != nullptr)
	{
		p2SString temp;
		temp.create("SFX: %i", App->audio->sfxVolume);
		sfxLabel->ChangeContent(temp.GetString());
	}


	return mustClose;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && in_mainmenu == false)
	{
		pauseGame = !pauseGame;
		CreateInGameMenu();

		if (!pauseGame)
			DestroyInGameMenu();

	}



	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	//App->collision->Disable();
	if (App->object->IsEnabled())
		App->object->Disable();


	return true;
}

void j1Scene::CameraLogic(float dt)
{
	iPoint offset = { 0, 0 };
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
		in_mainmenu = true;
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
	pugi::xml_node camNode = saveNode.append_child("camera");
	camNode.append_attribute("x") = cameraPos.x;
	camNode.append_attribute("y") = cameraPos.y;

	pugi::xml_node currentLevelNode = saveNode.append_child("current_level");
	currentLevelNode.append_attribute("name") = App->map->data.loadedLevel.GetString();

	return true;
}

//void j1Scene::CreateWidgets()
//{
   //



//}

void j1Scene::CreateMainMenu()
{
	title = App->gui->CreateSprite(App->gui->title_pos, this, App->gui->title_Rect);

	play = App->gui->CreateButton(ButtonType::PLAY, { 95, 140 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	 playLabel = App->gui->CreateLabel({ 30, 30 }, this);
	 play->Attach(playLabel, { 15, 10 });
	 playLabel->SetText("PLAY", { 255, 255, 255, 255 }, App->font->defaultFont);
	_continue = App->gui->CreateButton(ButtonType::CONTINUE, { 170, 140 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	_continue->enabled = false;
	contLabel = App->gui->CreateLabel({ 30, 30 }, this);

	_continue->Attach(contLabel, { 13, 10 });
	contLabel->SetText("CONT.", { 255, 255, 255, 255 }, App->font->defaultFont);

	if (enable_continue)
		_continue->enabled = true;


	settings = App->gui->CreateButton(ButtonType::SETTINGS, { 95, 169 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	settLabel = App->gui->CreateLabel({ 30, 30 }, this);
	settings->Attach(settLabel, { 13, 10 });
	settLabel->SetText("SETT.", { 255, 255, 255, 255 }, App->font->defaultFont);

	credits = App->gui->CreateButton(ButtonType::CREDITS, { 170, 169 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	credLabel = App->gui->CreateLabel({ 30, 30 }, this);
	credits->Attach(credLabel, { 13, 10 });
	credLabel->SetText("CRED.", { 255, 255, 255, 255 }, App->font->defaultFont);

	exit = App->gui->CreateButton(ButtonType::EXIT, { 310, 5 }, this,
		App->gui->X_Button_Section, App->gui->X_Button_Hover,
		App->gui->X_Button_Disabled, App->gui->X_Button_Clicked);

	webpage = App->gui->CreateButton(ButtonType::WEBPAGE, { 305, 220 }, this,
		App->gui->W_Button_Section, App->gui->W_Button_Hover,
		App->gui->W_Button_Disabled, App->gui->W_Button_Clicked);

	in_mainmenu = true;
}

void j1Scene::CreateSettings()
{
	iPoint settings_pos = { 100, 30 };
	SDL_Rect small_windows_coords = { 151, 15, 142, 163 };

	settings_window = App->gui->CreateSprite(settings_pos, this, small_windows_coords);

	toggle_fullscreen = App->gui->CreateButton(ButtonType::TOGGLE_FULLSCREEN, { 465, 155 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	UI_Label* fullscreenLabel = App->gui->CreateLabel({ 0,0 }, this);
	fullscreenLabel->SetText("FULLSC.", { 255, 255, 255, 255 }, App->font->defaultFont);
	toggle_fullscreen->Attach(fullscreenLabel, { 7, 10 });

	settings_window->Attach(toggle_fullscreen, { 39, 15 });

	toggle_framerate = App->gui->CreateButton(ButtonType::LIMIT_FRAMERATE, { 400, 155 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);
	settings_window->Attach(toggle_framerate, { 39, 45 });
	UI_Label* framerateLabel = App->gui->CreateLabel({ 0,0 }, this);
	framerateLabel->SetText("FPS CAP", { 255, 255, 255, 255 }, App->font->defaultFont);
	toggle_framerate->Attach(framerateLabel, { 7, 10 });

	close_settings = App->gui->CreateButton(ButtonType::CLOSE_SETTINGS, { 503, 130 }, this,
		App->gui->X_Button_Section, App->gui->X_Button_Hover,
		App->gui->X_Button_Disabled, App->gui->X_Button_Clicked);
	settings_window->Attach(close_settings, { 115, 5 });

	musicLabel = App->gui->CreateDynamicLabel({ 50, 140 }, this);
	musicThumb = App->gui->CreateSprite({ 0,0 }, this, { 301, 3, 12, 24 });
	musicSlider = App->gui->CreateSlider({ 110, 115, }, this, { 152, 4, 122, 7 }, musicThumb, &App->audio->musicVolume);

	sfxLabel = App->gui->CreateDynamicLabel({ 50, 140 }, this);
	sfxThumb = App->gui->CreateSprite({ 0,0 }, this, { 301, 3, 12, 24 });
	sfxSlider = App->gui->CreateSlider({ 110, 145, }, this, { 152, 4, 122, 7 }, sfxThumb, &App->audio->sfxVolume);

	App->audio->musicVolume = 10;
	App->audio->sfxVolume = 10;

	musicSlider->Attach(musicLabel, { 15, -10 });
	sfxSlider->Attach(sfxLabel, { 15, -10 });
	
	active_window = true;
}

void j1Scene::CreateCredits()
{
	iPoint credits_pos = { 100, 30 };
	SDL_Rect big_windows_coords = { 59, 364, 274, 244 };
	credits_window = App->gui->CreateSprite(credits_pos, this, big_windows_coords);

	//TODO UI_Label* credits_content = nullptr;


	close_credits = App->gui->CreateButton(ButtonType::CLOSE_CREDITS, { 567, 80 }, this,
		App->gui->X_Button_Section, App->gui->X_Button_Hover,
		App->gui->X_Button_Disabled, App->gui->X_Button_Clicked);

	active_window = true;
}

void j1Scene::CreateInGameMenu()
{
	iPoint settings_pos = { 390, 120 };
	SDL_Rect small_windows_coords = { 151, 15, 142, 163 };

	ingame_window = App->gui->CreateSprite(settings_pos, this, small_windows_coords);

	close_ingame_menu = App->gui->CreateButton(ButtonType::CLOSE_INGAME, { 503, 130 }, this,
		App->gui->X_Button_Section, App->gui->X_Button_Hover,
		App->gui->X_Button_Disabled, App->gui->X_Button_Clicked);

	resume = App->gui->CreateButton(ButtonType::RESUME, { 465, 155 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);

	to_mainmenu = App->gui->CreateButton(ButtonType::TO_MAIN_MENU, { 400, 155 }, this,
		App->gui->S_Button_Section, App->gui->S_Button_Hover,
		App->gui->S_Button_Disabled, App->gui->S_Button_Clicked);

	active_window = true;
}

void j1Scene::DestroyMainMenu()
{
	App->gui->DestroyUIElement(title);
	App->gui->DestroyUIElement(play);
	App->gui->DestroyUIElement(_continue);
	App->gui->DestroyUIElement(settings);
	App->gui->DestroyUIElement(credits);
	App->gui->DestroyUIElement(exit);
	App->gui->DestroyUIElement(webpage);

	UI_Sprite* title = nullptr;
	UI_Button* play = nullptr;
	UI_Button* _continue = nullptr;
	UI_Button* settings = nullptr;
	UI_Button* credits = nullptr;
	UI_Button* exit = nullptr;
	UI_Button* webpage = nullptr;

	in_mainmenu = false;
}

void j1Scene::DestroySettings()
{
	App->gui->DestroyUIElement(settings_window);
	App->gui->DestroyUIElement(toggle_fullscreen);
	App->gui->DestroyUIElement(toggle_framerate);
	App->gui->DestroyUIElement(close_settings);
	App->gui->DestroyUIElement(musicLabel);
	App->gui->DestroyUIElement(musicSlider);
	App->gui->DestroyUIElement(musicThumb);
	App->gui->DestroyUIElement(sfxLabel);
	App->gui->DestroyUIElement(sfxSlider);
	App->gui->DestroyUIElement(sfxThumb);

	UI_Sprite* settings_window = nullptr;
	UI_Button* toggle_fullscreen = nullptr;
	UI_Button* toggle_framerate = nullptr;
	UI_Button* close_settings = nullptr;

	active_window = false;
}

void j1Scene::DestroyCredits()
{
	App->gui->DestroyUIElement(credits_window);
	//App->gui->DestroyUIElement(credits_content);
	App->gui->DestroyUIElement(close_credits);
	UI_Sprite* credits_window = nullptr;
	//UI_Label* credits_content = nullptr;
	UI_Button* close_credits = nullptr;

	active_window = false;
}

void j1Scene::DestroyInGameMenu()
{
	App->gui->DestroyUIElement(ingame_window);
	App->gui->DestroyUIElement(close_ingame_menu);
	App->gui->DestroyUIElement(resume);
	App->gui->DestroyUIElement(to_mainmenu);
	UI_Sprite* ingame_window = nullptr;
	UI_Button* close_ingame_menu = nullptr;
	UI_Button* resume = nullptr;
	UI_Button* to_mainmenu = nullptr;

	in_mainmenu = false;
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
		if (!active_window)
		{
			in_mainmenu = false;
			p2List_item<Levels*>* levelData = App->map->data.levels.start;
			levelData = levelData->next;
			App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 1.5f);
			DestroyMainMenu();
		}

	}
	case ButtonType::CONTINUE:
	{
		if (!App->fade_to_black->IsFading()) {
			App->LoadGame("savegame.xml");
		}
		break;
	}
	case ButtonType::SETTINGS:
	{
		if (!active_window)
			CreateSettings();

		break;
	}
	case ButtonType::CREDITS:
	{
		if (!active_window)
			CreateCredits();

		break;
	}
	case ButtonType::EXIT:
	{
		if (!active_window)
			mustClose = false;
		break;
	}
	case ButtonType::WEBPAGE:
	{
		ShellExecuteA(NULL, "open", "https://github.com/dynamiczero99/CITM_2_Dev_Platformer",
			NULL, NULL, SW_SHOWNORMAL);
		break;
	}
	case ButtonType::TOGGLE_FULLSCREEN:
	{
		if (!fullscreen)
		{
			fullscreen = true;
			SDL_SetWindowFullscreen(App->win->window, SDL_WINDOW_FULLSCREEN);
		}

		else if (fullscreen)
		{
			fullscreen = false;
			SDL_SetWindowFullscreen(App->win->window, 0);
		}

		break;
	}

	case ButtonType::LIMIT_FRAMERATE:
	{
		if (active_window)
			App->capFrames = !App->capFrames;

		break;
	}
	case ButtonType::CLOSE_SETTINGS:
	{
		DestroySettings();

		break;
	}
	case ButtonType::CLOSE_CREDITS:
	{
		DestroyCredits();

		break;
	}
	case ButtonType::CLOSE_INGAME:
	{
		DestroyInGameMenu();

		break;
	}
	//--------------------------INGAME BUTTONS-----------------------
	case ButtonType::RESUME:
	{
		pauseGame = !pauseGame;

		DestroyInGameMenu();
		if (!pauseGame)
			DestroyInGameMenu();

		break;
	}

	case ButtonType::SAVE_GAME:
	{
		if (active_window)


			break;
	}

	case ButtonType::LOAD_GAME:
	{
		if (active_window)


			break;
	}

	case ButtonType::TO_MAIN_MENU:
	{
		// Load the first level of the list. Returning to main menu
		pauseGame = false;
		DestroyInGameMenu();
		p2List_item<Levels*>* levelData = App->map->data.levels.start;
		App->fade_to_black->FadeToBlack(levelData->data->name.GetString(), 1.0f);
		in_mainmenu = true;

		break;
	}

	return ret;
	}
}
