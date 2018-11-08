#include <iostream> 
#include <sstream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1App.h"
#include "j1Collision.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Object.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;
	want_to_save = want_to_load = false;

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	audio = new j1Audio();
	scene = new j1Scene();
	map = new j1Map();
	collision = new j1Collision();
	fade_to_black = new j1FadeToBlack();
	object = new j1Object();
	pathfinding = new j1PathFinding();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);//Input is the first module to update beacause other modules use the input during their update
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(scene);
	AddModule(object);
	AddModule(fade_to_black);
	AddModule(collision);//Collision is the penultimate module to update because it calcules all the overlaping collisions and resolves them just before rendering
	AddModule(render);// render last to swap buffer

}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	config_file;
	config;
	app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());
	}

	if(ret == true)
	{
		p2List_item<j1Module*>* item;
		item = modules.start;

		while(item != NULL && ret == true)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	bool ret = true;
	p2List_item<j1Module*>* item;
	item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	DeltaTimeCorrection();

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();
	CalculateDeltaTime();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if (want_to_load == true)
	{
		if (!prepareToLoad)
		{
			p2SString level_name;
			if (GetLevelToLoadName(level_name))
			{
				App->fade_to_black->FadeToBlack(level_name.GetString(), 1.0f);
				prepareToLoad = true;
			}
			else
			{
				LOG("Load game failed");
				want_to_load = false;
			}
		}
		else if (readyToLoad)
		{
			LoadGameNow();
			prepareToLoad = false;
		}
	}
		
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for(p2List_item<j1Module*>* item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for(p2List_item<j1Module*>* item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for(p2List_item<j1Module*>* item = modules.start; item != NULL && ret == true; item = item->next)
	{
		pModule = item->data;

		if(pModule->active == false) {
			continue;
		}

		ret = item->data->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list


	load_game.create(file);
	want_to_load = true;
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game.create(file);
}

// ---------------------------------------
void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

float j1App::GetDeltaTime() const
{
	//return deltaTime;
	return (1.0f / 60.0f);
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.GetString());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.GetString());

		root = data.child("game_state");

		ret = true;

		p2List_item<j1Module*>* item = modules.start;
		while(item != NULL && ret == true)
		{
			ret = item->data->Load(root.child(item->data->name.GetString()));
			item = item->next;
		}

		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.GetString(), result.description());

	want_to_load = false;
	readyToLoad = false;

	return ret;
}

bool j1App::SavegameNow() const
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.GetString());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret == true)
	{
		ret = item->data->Save(root.append_child(item->data->name.GetString()));
		item = item->next;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
//		fs->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());

		data.save_file(save_game.GetString());
		LOG("... finished saving", save_game.GetString());
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->data->name.GetString() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

void j1App::DeltaTimeCorrection()
{
	//CalculateDeltaTime() executes on the preupdate, so just before that, in the start, we make the lastTime equal the current time so that when it substracts with currTime it will be basically the same
	lastTime = SDL_GetTicks();
}

void j1App::CalculateDeltaTime()
{
	currTime = SDL_GetTicks();
	deltaTime = (currTime - lastTime) / 1000.0F;//INFO: 1 second is 1000 miliseconds
	lastTime = currTime;
}

bool j1App::GetLevelToLoadName(p2SString& level_name) const
{

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.GetString());

	if (result != NULL)
	{
		root = data.child("game_state");

		pugi::xml_node levelCheck = root.child("scene").child("current_level");
		p2SString name = levelCheck.attribute("name").as_string();

		if (name != NULL)
		{
			LOG("Returning level to load %s from file %s...", name.GetString(), load_game.GetString());

			level_name = name;
			return true;
		}
		else
		{
			LOG("level name not found");
			return false;
		}
	}
	else
	{
		LOG("Failed to find load game file");
		return false;
	}

}