#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL_render.h"

struct SDL_Texture;
struct SDL_Rect;

class j1Player : public j1Module
{
public:
	j1Player();
	// Called before render is available
	bool Awake(pugi::xml_node&);
	// Called before the first frame
	bool Start();
	// Called each loop iteration
	bool PreUpdate();
	// Called each loop iteration
	bool Update(float dt);
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

private:
	float horizontalSpeed = 1;//Pixels it will move every second
	//TODO: Set the velocity in the xml

	fPoint position;
	fPoint velocity;
	//TODO: Add acceleration (to use incorporate gravity easily)

	SDL_Texture* characterTex;
	SDL_Rect* characterAnim;
	//TODO: Change to animation class (which can hold multiple rectangles)

	pugi::xml_node player_node;
	SDL_RendererFlip flip;
};

#endif