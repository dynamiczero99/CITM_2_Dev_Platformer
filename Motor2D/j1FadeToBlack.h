#ifndef __j1FADETOBLACK_H__
#define __j1FADETOBLACK_H__

#include "j1Module.h"
#include "SDL\include\SDL_rect.h"

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack() {};
	~j1FadeToBlack();

	bool Awake(pugi::xml_node&);
	bool Start();
	//bool Update(float dt);
	bool PostUpdate();
	bool FadeToBlack(const char* lvlName, float time = 1.0f);
	bool IsFading() const;

private:

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	float fadePerCent = 0.0F;//It will go from 0 to 1 (fade to black) and then from 1 to 0 (fade from black)
	//j1Module* to_enable = nullptr;
	//j1Module* to_disable = nullptr;
	const char* lvl_to_load = nullptr;
};

#endif //__MODULEFADETOBLACK_H__