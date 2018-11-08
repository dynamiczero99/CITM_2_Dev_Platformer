#include <math.h>
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

bool j1FadeToBlack::Awake(pugi::xml_node&)
{
	name.create("FadeToBlack");

	uint width, height = 0;
	App->win->GetWindowSize(width, height);

	screen = { 0, 0, (int)width * (int)App->win->GetScale(), (int)height * (int)App->win->GetScale() };
	return true;
}

j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeToBlack::PostUpdate()//float dt)
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			App->scene->Disable();
			// clean up the current map
			if (App->map->Reset())// load new map
			{
				if (App->want_to_load) // if the call comes from a loadgame
					App->readyToLoad = true; // active to call all virtual loads

				// now map knows if the call is from a load game call
				if(App->map->Load(lvl_to_load))
					App->scene->Enable();
				// TODO: maybe we need to search a less ugly workaround to restart scene
				//App->scene->Disable();
			
			}

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point deactivate one module, then activate the other
bool j1FadeToBlack::FadeToBlack(const char* lvlName, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		//to_enable = module_on;
		//to_disable = module_off;
		lvl_to_load = lvlName;
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::IsFading() const
{
	return current_step != fade_step::none;
}